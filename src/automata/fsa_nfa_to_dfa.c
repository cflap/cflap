#include "fsa_nfa_to_dfa.h"
#include "closure_taker.h"

State *ntd_create_initial_state(Automaton *nfa, Automaton *dfa)
{
	State *initial_state = automaton_get_initial_state(nfa);
	GList *initial_closure = closure_taker_get_closure(initial_state, nfa);
	State *state = ntd_create_state_with_states(dfa, initial_closure, nfa);
	automaton_set_initial_state(dfa, state);
	
	return state;
}

gboolean ntd_has_final_state(GList *states, Automaton *automaton)
{
	GList *current = states;
	while (current != NULL) {
		State *s = (State *)current->data;
		if (automaton_is_final_state(automaton, s)) {
			return TRUE;
		}
		current = current->next;
	}
	
	return FALSE;
}

GList *ntd_get_states_for_state(State *state, Automaton *automaton)
{
	const char *label = state_get_label(state);
	GList *states_for_state = NULL;
	if (label == NULL)
		return states_for_state;
	
	char *label_to_tokenize = strdup(label);
	char *token = strtok(label_to_tokenize, " ");
	while (token != NULL) {
		int id = atoi(token);
		State *s = automaton_get_state_by_id(automaton, id);
		states_for_state = g_list_append(states_for_state, s);
		
		token = strtok(NULL, " ");
	}
	
	free(label_to_tokenize);
	
	return states_for_state;
}

char *ntd_get_string_for_states(GList *states)
{
	char *string = strdup("");
	GList *current = states;
	while (current != NULL) {
		State *s = (State *)current->data;
		int id = state_get_id(s);
		char *to_free = string;
		string = g_strdup_printf("%s%d ", string, id);
		free(to_free);
		current = current->next;
	}
	
	return string;
}

GList *ntd_get_states_on_terminal(const char *terminal, GList *states, Automaton *automaton)
{
	GList *states_on_terminal = NULL;
	
	GList *current_state_node = states;
	while (current_state_node != NULL) {
		State *s = (State *)current_state_node->data;
		GList *transitions = automaton_get_transitions_from_state(automaton, s);
		
		while (transitions != NULL) {
			Transition *t = (Transition *)transitions->data;
			const char *t_symbol = fsa_transition_get_symbol(t);
			if (!strcmp(terminal, t_symbol)) {
				State *to_state = transition_get_to_state(t);
				GList *closure = closure_taker_get_closure(to_state, automaton);
				GList *current_closure = closure;
				while (current_closure != NULL) {
					State *closure_state = (State *)current_closure->data;
					if (g_list_find(states_on_terminal, closure_state) == NULL) {
						states_on_terminal = g_list_append(states_on_terminal, closure_state);
					}
					current_closure = current_closure->next;
				}
				g_list_free(closure);
			}
			transitions = transitions->next;
		}
		current_state_node = current_state_node->next;
	}
	
	return states_on_terminal;
}

gboolean ntd_contains_state(State *state, GList *states)
{
	return g_list_find(states, state) != NULL ? TRUE : FALSE;
}

gboolean ntd_contain_same_states(GList *states1, GList *states2)
{
	int length1 = g_list_length(states1);
	int length2 = g_list_length(states2);
	
	if (length1 != length2) {
		return FALSE;
	}
	
	GList *copy1 = g_list_copy(states1);
	GList *copy2 = g_list_copy(states2);
	
	copy1 = g_list_sort(copy1, (GCompareFunc) state_compare_by_id);
	copy2 = g_list_sort(copy2, (GCompareFunc) state_compare_by_id);
	
	GList *current_copy1 = copy1;
	GList *current_copy2 = copy2;
	
	while (current_copy1 != NULL) {
		State *s1 = (State *)current_copy1->data;
		State *s2 = (State *)current_copy2->data;
		if (s1 != s2)
			return FALSE;
		
		current_copy1 = current_copy1->next;
		current_copy2 = current_copy2->next;
	}
	
	g_list_free(copy1);
	g_list_free(copy2);
	
	return TRUE;
}

State *ntd_get_state_for_states(GList *states, Automaton *dfa, Automaton *nfa)
{
	GList *dfa_states = automaton_get_states_as_list(dfa);
	
	while (dfa_states != NULL) {
		State *s = (State *)dfa_states->data;
		GList *nfa_states = ntd_get_states_for_state(s, nfa);
		if (ntd_contain_same_states(nfa_states, states)) {
			g_list_free(nfa_states);
			return s;
		}
		dfa_states = dfa_states->next;
	}
	
	return NULL;
}

GList *ntd_expand_state(State *state, Automaton *nfa, Automaton *dfa)
{
	GList *expansion = NULL;
	AlphabetRetriever *alphabet_retriever = fsa_alphabet_retriever_new();
	GList *alphabet = alphabet_retriever_get_alphabet(alphabet_retriever, nfa);
	
	GList *current_alphabet_node = alphabet;
	while (current_alphabet_node != NULL) {
		const char *letter = (const char *)current_alphabet_node->data;
		
		GList *states_for_state = ntd_get_states_for_state(state, nfa);
		GList *states = ntd_get_states_on_terminal(letter, states_for_state, nfa);
		
		if (g_list_length(states) > 0) {
			State *to_state = ntd_get_state_for_states(states, dfa, nfa);
			if (to_state == NULL) {
				to_state = ntd_create_state_with_states(dfa, states, nfa);
				expansion = g_list_append(expansion, to_state);
			}
			Transition *t = fsa_transition_new(state, to_state, letter);
			automaton_add_transition(dfa, t);
		}
		
		g_list_free(states_for_state);
		g_list_free(states);
		
		current_alphabet_node = current_alphabet_node->next;
	}
	
	g_list_free(alphabet);
	
	alphabet_retriever_destroy(alphabet_retriever);
	
	return expansion;
}

State *ntd_create_state_with_states(Automaton *dfa, GList *states, Automaton *nfa)
{
	int x = g_random_int() % 500;
	int y = g_random_int() % 500;
	
	State *state = automaton_create_state(dfa, x, y);
	
	char *label = ntd_get_string_for_states(states);
	
	state_set_label(state, label);
	if (ntd_has_final_state(states, nfa)) {
		automaton_add_final_state(dfa, state);
	}
	
	free(label);
	
	return state;
}

static gboolean has_multiple_character_labels(Automaton *automaton)
{
	GList *transitions = automaton_get_transitions_as_list(automaton);
	GList *current_transition_node = transitions;
	while (current_transition_node != NULL) {
		Transition *t = (Transition *)current_transition_node->data;
		const char *symbol = fsa_transition_get_symbol(t);
		if (strlen(symbol) > 1) {
			return TRUE;
		}
		current_transition_node = current_transition_node->next;
	}
	
	g_list_free(transitions);
	
	return FALSE;
}

static void handle_label(Transition *transition, Automaton *automaton)
{
	State *from = transition_get_from_state(transition);
	State *to = transition_get_to_state(transition);
	State *f = from;
	
	char *symbol = strdup(fsa_transition_get_symbol(transition));
	automaton_remove_transition(automaton, transition);
	int symbol_length = strlen(symbol);
	
	for(int i = 0; i < symbol_length; i++) {
		State *going = NULL;
		if (i == (symbol_length - 1)) {
			going = to;
		} else {
			int x = (state_get_x(f) * (symbol_length - i - 1) + state_get_x(to) * (i + 1)) / symbol_length;
			int y = (state_get_y(f) * (symbol_length - i - 1) + state_get_y(to) * (i + 1)) / symbol_length;
			going = automaton_create_state(automaton, x, y);
		}
		char *new_symbol = g_strndup(symbol + i, 1);
		//g_print("new symbol: %s\n", new_symbol);
		Transition *new_transition = fsa_transition_new(from, going, new_symbol);
		free(new_symbol);
		automaton_add_transition(automaton, new_transition);
		from = going;
	}
	free(symbol);
}

void remove_multiple_characters_label_from_automaton(Automaton *automaton)
{
	GList *transitions = automaton_get_transitions_as_list(automaton);
	
	GList *current = transitions;
	while (current != NULL) {
		Transition *t = (Transition *)current->data;
		const char *symbol = fsa_transition_get_symbol(t);
		if (strlen(symbol) > 1) {
			handle_label(t, automaton);
		}
		current = current->next;
	}
	
	g_list_free(transitions);
}

Automaton *ntd_convert_to_dfa(Automaton *automaton)
{
	if(!fsa_is_nfa(automaton)) {
		return NULL;
	}
	
	if (has_multiple_character_labels(automaton)) {
		remove_multiple_characters_label_from_automaton(automaton);
	}
	
	Automaton *dfa = fsa_new();
	State *initial_state = ntd_create_initial_state(automaton, dfa);
	
	GQueue queue = G_QUEUE_INIT;
	g_queue_push_tail(&queue, initial_state);
	
	while (!g_queue_is_empty(&queue)) {
		State *state = g_queue_pop_tail(&queue);
		
		GList *states_to_expand = NULL;
		GList *expanded = ntd_expand_state(state, automaton, dfa);
		GList *current_expanded_node = expanded;
		
		while (current_expanded_node != NULL) {
			State *to_add = (State *)current_expanded_node->data;
			states_to_expand = g_list_prepend(states_to_expand, to_add);
			current_expanded_node = current_expanded_node->next;
		}
		g_list_free(expanded);
		
		GList *current_ste_node = states_to_expand;
		while (current_ste_node != NULL) {
			State *s = (State *)current_ste_node->data;
			g_queue_push_tail(&queue, s);
			current_ste_node = current_ste_node->next;
		}
		g_list_free(states_to_expand);
	}
	
	return dfa;
}