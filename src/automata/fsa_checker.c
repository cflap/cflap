#include "fsa_checker.h"

gboolean fsa_are_non_deterministic(Transition *t1, Transition *t2)
{
	const char *symbol1 = fsa_transition_get_symbol(t1);
	const char *symbol2 = fsa_transition_get_symbol(t2);
	if (!strcmp(symbol1, symbol2)) {
		return 1;
	} else if (!strncmp(symbol1, symbol2, strlen(symbol1))) {
		return 1;
	} else if (!strncmp(symbol1, symbol2, strlen(symbol2))) {
		return 1;
	} else {
		return 0;
	}
}

GList *fsa_get_non_deterministic_states(Automaton *automaton)
{
	GList *nd_states= NULL;
	GList *states = automaton_get_states_as_list(automaton);
	GList *current_state_node = states;
	for (; current_state_node != NULL; current_state_node = current_state_node->next) {
		State *s = (State *)current_state_node->data;
		GList *from_state_transitions = automaton_get_transitions_from_state(automaton, s);
		GList *current_fst_node = from_state_transitions;
		for (; current_fst_node != NULL; current_fst_node = current_fst_node->next) {
			Transition *t1 = (Transition *)current_fst_node->data;
			if (fsa_is_epsilon_transition(t1)) {
				if ((g_list_find(nd_states, s)) == NULL) {
					nd_states = g_list_append(nd_states, s);
				}
			} else {
				GList *current_fst_node2 = current_fst_node->next;
				for (;current_fst_node2 != NULL; current_fst_node2 = current_fst_node2->next) {
					Transition *t2 = (Transition *)current_fst_node2->data;
					if (fsa_are_non_deterministic(t1, t2)) {
						if ((g_list_find(nd_states, s)) == NULL) {
							nd_states = g_list_append(nd_states, s);
						}
					}
				}
			}
		}
	}
	g_list_free(states);
	return nd_states;
}

gboolean fsa_is_nfa(Automaton *automaton)
{
	GList *non_deterministic_states = fsa_get_non_deterministic_states(automaton);
	gboolean result = g_list_length(non_deterministic_states) > 0;
	g_list_free(non_deterministic_states);
	
	return result;
}

gboolean fsa_is_epsilon_transition(Transition *t)
{
	const char *symbol = fsa_transition_get_symbol(t);
	return !strcmp(symbol, "");
}