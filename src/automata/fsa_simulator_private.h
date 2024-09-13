/*
 * This file contains the private interface and the structure of a finite state automaton simulator
 */

#ifndef FSA_SIMULATOR_PRIVATE_H
#define FSA_SIMULATOR_PRIVATE_H

#include "fsa_simulator.h"

struct _FsaSimulator {
	AutomatonSimulator *super;
};

void fsa_simulator_destroy_private(AutomatonSimulator *simulator);

GList *fsa_simulator_get_initial_configurations_private(AutomatonSimulator *simulator, const char *input);
GList *fsa_simulator_step_configuration_private(AutomatonSimulator *simulator, Configuration *configuration);
int fsa_simulator_is_accepted_private(AutomatonSimulator *simulator);
int fsa_simulator_simulate_input_private(AutomatonSimulator *simulator, const char *input);

#endif // FSA_SIMULATOR_PRIVATE_H
