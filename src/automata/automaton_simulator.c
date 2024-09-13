/*
 * This file contains the implementation of the interface of a generic automaton simulator
 */

#include <stdlib.h>

#include "automaton_simulator_private.h"

/*
 * Initializes a new automaton simulator with the given automaton
 */
AutomatonSimulator *automaton_simulator_new(Automaton *automaton)
{
	AutomatonSimulator *simulator = malloc(sizeof(AutomatonSimulator));
	
	if (simulator == NULL || automaton == NULL)
		return NULL;
	
	simulator->derived = NULL;
	simulator->automaton = automaton;
	
	simulator->current_configurations = g_hash_table_new_full(NULL, NULL, (GDestroyNotify)configuration_destroy, NULL);
	
	simulator->destroy = &automaton_simulator_destroy_private;
	simulator->get_initial_configurations = NULL;
	simulator->step_configuration = NULL;
	simulator->is_accepted = NULL;
	simulator->simulate_input = NULL;
	
	return simulator;
}

/*
 * Destroys the given automaton simulator
 */
void automaton_simulator_destroy_private(AutomatonSimulator *simulator)
{
	g_hash_table_destroy(simulator->current_configurations);
	free(simulator);
}

/*
 * Destroys the given automaton simulator
 */
void automaton_simulator_destroy(AutomatonSimulator *simulator)
{
	if (simulator == NULL)
		return;
	
	simulator->destroy(simulator);
}

/*
 * Returns a list of initial configurations
 * REMEMBER TO FREE THE RETURNED LIST
 */
GList *automaton_simulator_get_initial_configurations(AutomatonSimulator *simulator, const char *input)
{
	if (simulator == NULL)
		return NULL;
	
	simulator->get_initial_configurations(simulator, input);
}

/*
 * Steps on the next configuration
 */
GList *automaton_simulator_step_configuration(AutomatonSimulator *simulator, Configuration *configuration)
{
	if (simulator == NULL)
		return NULL;
	
	simulator->step_configuration(simulator, configuration);
}

/*
 * Returns 1 if is in an accepting configuration and the input has been consumed
 * returns 0 if it's in a non-accepting consiguration or there's still input to consume
 */
int automaton_simulator_is_accepted(AutomatonSimulator *simulator)
{
	if (simulator == NULL)
		return 0;
	
	simulator->is_accepted(simulator);
}

/*
 * Returns 1 if the input is accepted and 0 if not
 */
int automaton_simulator_simulate_input(AutomatonSimulator *simulator, const char *input)
{
	if (simulator == NULL)
		return 0;
	
	simulator->simulate_input(simulator, input);
}
