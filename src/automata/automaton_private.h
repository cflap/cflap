/*
 * This file contains private interface of automaton
 */

#ifndef AUTOMATON_PRIVATE_H
#define AUTOMATON_PRIVATE_H

#include "automaton.h"

struct _Automaton {
	void *derived;
	// GHashTable in this case is used as a set for states (k: State, v: NULL)
	GHashTable *states_set;
	// GHashTable in this case is used as a set for final states (k: State, v: NULL)
	GHashTable *final_states_set;
	// GHashTable in this case is used as a set for the transitions (k: Transition, v: NULL)
	GHashTable *transitions_set;
	State *initial_state;
	
	// Map containing transitions from states (k: State, v: GList *)
	GHashTable *transition_from_state_map;
	// Map containing trnasitions to states (k: State, v: GList *)
	GHashTable *transition_to_state_map;
	
	GtkWidget *(*create_transition_input) (Automaton *, State *, State *, gpointer);
	
	void (*destroy)(Automaton *);
};

void automaton_destroy_private(Automaton *automaton);



#endif // AUTOMATON_PRIVATE_H
