/*
 * This file contains the interface and the structure of a generic automaton simulator
 */

typedef struct _AutomatonSimulator AutomatonSimulator;

#ifndef AUTOMATON_SIMULATOR_H
#define AUTOMATON_SIMULATOR_H

#include "automaton.h"
#include "configuration.h"

void automaton_simulator_destroy(AutomatonSimulator *simulator);

GList *automaton_simulator_get_initial_configurations(AutomatonSimulator *simulator, const char *input);
GList *automaton_simulator_step_configuration(AutomatonSimulator *simulator, Configuration *configuration);
int automaton_simulator_is_accepted(AutomatonSimulator *simulator);
int automaton_simulator_simulate_input(AutomatonSimulator *simulator, const char *input);

#endif // AUTOMATON_SIMULATOR_H
