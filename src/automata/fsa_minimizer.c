#include "fsa_minimizer.h"
#include "fsa_nfa_to_dfa.h"

void fsa_min_remove_unreachable_states(Automaton *automaton)
{
	State *initial_state = automaton_get_initial_state(automaton);
	GList *states = automaton_get_states_as_list(automaton);
	GHashTable *reachable_states = g_hash_table_new(NULL, NULL);
	g_hash_table_add(reachable_states, initial_state);
	
	GQueue *queue = g_queue_new();
	g_queue_push_tail(queue, initial_state);
	State *head = NULL;
	while (head = g_queue_peek_head(queue)) {
		GList *transitions_from_state = automaton_get_transitions_from_state(automaton, head);
		GList *current_transition_node = transitions_from_state;
		for(; current_transition_node != NULL; current_transition_node = current_transition_node->next) {
			Transition *transition = (Transition *) current_transition_node->data;
			State *to_state = transition_get_to_state(transition);
			if (!g_hash_table_contains(reachable_states, to_state)) {
				g_hash_table_add(reachable_states, to_state);
				if (g_queue_find(queue, to_state) == NULL) {
					g_queue_push_tail(queue, to_state);
				}
			}
		}
		head = g_queue_pop_head(queue);
	}
	g_queue_free(queue);
	
	GList *unreachable_states = NULL;
	GList *current_state_node = states;
	for (; current_state_node != NULL; current_state_node = current_state_node->next) {
		State *s = (State *)current_state_node->data;
		if (!g_hash_table_contains(reachable_states, s)) {
			unreachable_states = g_list_append(unreachable_states, s);
		}
	}
	
	GList *current_us_node = unreachable_states;
	for (; current_us_node != NULL; current_us_node = current_us_node->next) {
		State *s = (State *)current_us_node->data;
		automaton_remove_state(automaton, s);
	}
	
	g_list_free(states);
	g_hash_table_unref(reachable_states);
	g_list_free(unreachable_states);
}

GList *fsa_min_split_accept_non_accept(Automaton *automaton)
{
	GList *partitions = NULL;
	GList *accept = NULL;
	GList *non_accept = NULL;
	
	GList *states = automaton_get_states_as_list(automaton);
	GList *current_state_node = states;
	for (; current_state_node != NULL; current_state_node = current_state_node->next) {
		State *s = (State *)current_state_node->data;
		if (automaton_is_final_state(automaton, s)) {
			accept = g_list_append(accept, s);
		} else {
			non_accept = g_list_append(non_accept, s);
		}
	}
	
	g_list_free(states);
	
	partitions = g_list_append(partitions, accept);
	partitions = g_list_append(partitions, non_accept);
	
	return partitions;
}

static GList *get_states_that_lead_to_a(Automaton *automaton, GList *A, const char *letter)
{
	GList *X = NULL;
	GList *current_A_node = A;
	for (; current_A_node != NULL; current_A_node = current_A_node->next) {
		State *state_in_A = (State *)current_A_node->data;
		// Get all transitions that lead to this state
		GList *transitions_to_state = automaton_get_transitions_to_state(automaton, state_in_A);
		GList *current_tts_node = transitions_to_state;
		for (; current_tts_node != NULL; current_tts_node = current_tts_node->next) {
			Transition *t = (Transition *)current_tts_node->data;
			const char *symbol = fsa_transition_get_symbol(t);
			/*
			 * If the transition has the same symbol of the provided one it must be included
			 * this means that exist a transition with that symbol that leads to partition A
			 */
			if (!strcmp(symbol, letter)) {
				State *to_add_in_X = transition_get_from_state(t);
				// Add the state only if it's not present yet in the list
				if (g_list_find(X, to_add_in_X) == NULL) {
					X = g_list_append(X, to_add_in_X);
				}
			}
		}
	}
	
	return X;
}

static GList *get_intersection(GList *list1, GList *list2)
{
	GList *intersection = NULL;
	GList *current_list1_node = list1;
	for (; current_list1_node != NULL; current_list1_node = current_list1_node->next) {
		State *s = (State *)current_list1_node->data;
		if (g_list_find(list2, s) != NULL) {
			if (g_list_find(intersection, s) == NULL) {
				intersection = g_list_append(intersection, s);
			}
		}
	}
	
	return intersection;
}

static GList *get_subtraction(GList *list1, GList *list2)
{
	GList *list1_copy = g_list_copy(list1);
	int list2_length = g_list_length(list2);
	for (int i = 0; i < list2_length; i++) {
		State *s = g_list_nth_data(list2, i);
		list1_copy = g_list_remove(list1_copy, s);
	}
	
	return list1_copy;
}

static GList *get_partition_containing_state(GList *partitions, State *state)
{
	GList *current_partition_node = partitions;
	for (; current_partition_node != NULL; current_partition_node = current_partition_node->next) {
		GList *p = (GList *)current_partition_node->data;
		if (g_list_find(p, state) != NULL) {
			return p;
		}
	}
	
	return NULL;
}

static gboolean partition_contains_initial_state(GList *partition, Automaton *automaton)
{
	GList *current_partition_node = partition;
	for (; current_partition_node != NULL; current_partition_node = current_partition_node->next) {
		State *s = (State *)current_partition_node->data;
		if (automaton_is_initial_state(automaton, s)) {
			return TRUE;
		}
	}
	
	return FALSE;
}

static gboolean partition_contains_final_state(GList *partition, Automaton *automaton)
{
	GList *current_partition_node = partition;
	for (; current_partition_node != NULL; current_partition_node = current_partition_node->next) {
		State *s = (State *)current_partition_node->data;
		if (automaton_is_final_state(automaton, s)) {
			return TRUE;
		}
	}
	
	return FALSE;
}

Automaton *fsa_min_minimize(Automaton *automaton)
{	
	remove_multiple_characters_label_from_automaton(automaton);
	fsa_min_remove_unreachable_states(automaton);
	
	GList *p = fsa_min_split_accept_non_accept(automaton);
	GList *w = g_list_copy(p);
	
	AlphabetRetriever *alphabet_retriever = fsa_alphabet_retriever_new();
	GList *alphabet = alphabet_retriever_get_alphabet(alphabet_retriever, automaton);
	int alphabet_length = g_list_length(alphabet);
	alphabet_retriever_destroy(alphabet_retriever);
	
	while (g_list_length(w) > 0) {
		GList *set_a = g_list_nth_data(w, 0);
		w = g_list_remove(w, set_a);
		for (int i = 0; i < alphabet_length; i++) {
			const char *letter = g_list_nth_data(alphabet, i);
			GList *set_x = get_states_that_lead_to_a(automaton, set_a, letter);
			GList *p_iter = g_list_copy(p);
			GList *current_piter_node = p_iter;
			for (; current_piter_node != NULL; current_piter_node = current_piter_node->next) {
				GList *y = (GList *)current_piter_node->data;
				GList *intersection = get_intersection(set_x, y);
				GList *subtraction = get_subtraction(y, set_x);
				int intersection_length = g_list_length(intersection);
				int subtraction_length = g_list_length(subtraction);
				if (intersection_length > 0 && subtraction_length > 0) {
					p = g_list_remove(p, y);
					p = g_list_append(p, intersection);
					p = g_list_append(p, subtraction);
					if (g_list_find(w, y) != NULL) {
						w = g_list_remove(w, y);
						w = g_list_append(w, intersection);
						w = g_list_append(w, subtraction);
					} else {
						if (intersection_length <= subtraction_length) {
							w = g_list_append(w, intersection);
						} else {
							w = g_list_append(w, subtraction);
						}
					}
				} else {
					g_list_free(intersection);
					g_list_free(subtraction);
				}
			}
			g_list_free(p_iter);
			g_list_free(set_x);
		}
	}
	
	g_list_free(w);
	g_list_free(alphabet);
	
	p = g_list_remove(p, NULL);
	
	Automaton *minimized_automaton = fsa_new();
	
	GHashTable *partition_state = g_hash_table_new(NULL, NULL);
	
	GList *current_p_node = p;
	for (; current_p_node != NULL; current_p_node = current_p_node->next) {
		GList *partition = (GList *)current_p_node->data;
		int x = g_random_int() % 500;
		int y = g_random_int() % 500;
		State *s = automaton_create_state(minimized_automaton, x, y);
		
		if (partition_contains_initial_state(partition, automaton)) {
			automaton_set_initial_state(minimized_automaton, s);
		}
		if (partition_contains_final_state(partition, automaton)) {
			automaton_add_final_state(minimized_automaton, s);
		}
		g_hash_table_insert(partition_state, partition, s);
	}
	
	current_p_node = p;
	for (; current_p_node != NULL; current_p_node = current_p_node->next) {
		GList *partition = (GList *)current_p_node->data;
		State *from_state = g_hash_table_lookup(partition_state, partition);
		State *state_from_partition = g_list_nth_data(partition, 0);
		GList *transitions = automaton_get_transitions_from_state(automaton, state_from_partition);
		GList *current_transition_node = transitions;
		for (; current_transition_node != NULL; current_transition_node = current_transition_node->next) {
			Transition *t = (Transition *)current_transition_node->data;
			const char *symbol = fsa_transition_get_symbol(t);
			State *to_state_of_transition = transition_get_to_state(t);
			GList *to_state_partition = get_partition_containing_state(p, to_state_of_transition);
			State *to_state = g_hash_table_lookup(partition_state, to_state_partition);
			Transition *new_transition = fsa_transition_new(from_state, to_state, symbol);
			automaton_add_transition(minimized_automaton, new_transition);
		}
	}
	
	g_hash_table_unref(partition_state);
	g_list_free(p);
	
	return minimized_automaton;
}