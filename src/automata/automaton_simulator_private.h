/*
 * This file contains the private interface and the structure of a generic automaton simulator
 */

typedef struct _AutomatonSimulator AutomatonSimulator;

#ifndef AUTOMATON_SIMULATOR_PRIVATE_H
#define AUTOMATON_SIMULATOR_PRIVATE_H

#include "automaton_simulator.h"

struct _AutomatonSimulator {
	void *derived;
	// the automaton to simulate
	Automaton *automaton;
	// configurations the machine could be in at a given moment
	GHashTable *current_configurations;
	
	void (*destroy)(AutomatonSimulator *);
	/* 
 	 * Calling these functions on AutomatonSimulators instance that are not an extension of
 	 * AutomatonSimulator will lead to undefined behaviors
 	 */
	GList *(*get_initial_configurations)(AutomatonSimulator *, const char *);
	GList *(*step_configuration)(AutomatonSimulator *, Configuration *);
	int (*is_accepted)(AutomatonSimulator *);
	int (*simulate_input)(AutomatonSimulator *, const char *);
};

// Constructor is private because automaton_simulator is abstract
AutomatonSimulator *automaton_simulator_new(Automaton *automaton);

void automaton_simulator_destroy_private(AutomatonSimulator *simulator);

#endif // AUTOMATON_SIMULATOR_PRIVATE_H
