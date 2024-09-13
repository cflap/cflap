#include "closure_taker.h"

GList *closure_taker_get_closure(State *state, Automaton *automaton)
{
	GList *states = NULL;
	
	states = g_list_append(states, state);
	
	GList *current_state_node = states;
	while (current_state_node != NULL) {
		State *s = (State *)current_state_node->data;
		GList *transition_from_state = automaton_get_transitions_from_state(automaton, s);
		
		GList *current_transition_node = transition_from_state;
		while (current_transition_node != NULL) {
			Transition *t = (Transition *)current_transition_node->data;
			const char *symbol = fsa_transition_get_symbol(t);
			// Check if it's an epsilon transition
			if (!strcmp(symbol, "")) {
				State *to_state = transition_get_to_state(t);
				if ((g_list_find(states, to_state)) == NULL) {
					states = g_list_append(states, to_state);
				}
			}
			current_transition_node = current_transition_node->next;
		}
		current_state_node = current_state_node->next;
	}
	
	return states;
}