/*
 * This file contains the implementation of the interface of a finite state automaton simulator
 */

#include <stdlib.h>
#include <string.h>

#include "automaton_simulator_private.h"
#include "fsa_simulator_private.h"
#include "fsa_configuration.h"
#include "fsa_transition.h"
#include "closure_taker.h"

/*
 * Initializes a new fsa simulator
 */
AutomatonSimulator *fsa_simulator_new(Automaton *automaton)
{
	FsaSimulator *simulator = malloc(sizeof(FsaSimulator));
	
	if (simulator == NULL)
		return NULL;
	
	AutomatonSimulator *super = automaton_simulator_new(automaton);
	
	if (super == NULL)
		free(simulator);
	
	super->derived = simulator;
	
	simulator->super = super;
	
	simulator->super->destroy = &fsa_simulator_destroy_private;
	simulator->super->get_initial_configurations = &fsa_simulator_get_initial_configurations_private;
	simulator->super->step_configuration = &fsa_simulator_step_configuration_private;
	simulator->super->is_accepted = &fsa_simulator_is_accepted_private;
	simulator->super->simulate_input = &fsa_simulator_simulate_input_private;
	
	return simulator->super;
}

/*
 * Private implementation of the destroy
 */
void fsa_simulator_destroy_private(AutomatonSimulator *simulator)
{
	free(simulator->derived);
	automaton_simulator_destroy_private(simulator);
}

/*
 * Private implementation of get_initial_configurations
 */
GList *fsa_simulator_get_initial_configurations_private(AutomatonSimulator *simulator, const char *input)
{
	Automaton *automaton = simulator->automaton;
	State *initial_state = automaton_get_initial_state(automaton);
	GList *closure = closure_taker_get_closure(initial_state, automaton);
	
	GList *initial_configurations = NULL;
	GList *current_closure_node = closure;
	for (; current_closure_node != NULL; current_closure_node = current_closure_node->next) {
		State *s = (State *)current_closure_node->data;
		Configuration *conf = fsa_configuration_new(s, NULL, input, input);
		initial_configurations = g_list_prepend(initial_configurations, conf);
	}
	
	initial_configurations = g_list_reverse(initial_configurations);
	
	return initial_configurations;
}

/*
 * Private implementation of step_configuration
 * returns the list of the configuration after one step on the input
 * returns NULL if couldn't allocate memory for new configurations
 * TODO ADD EPSILON TRANSITIONS
 */
GList *fsa_simulator_step_configuration_private(AutomatonSimulator *simulator, Configuration *configuration)
{
	Automaton *automaton = simulator->automaton;
	State *state = configuration_get_state(configuration);
	const char *input = fsa_configuration_get_input(configuration);
	const char *unprocessed_input = fsa_configuration_get_unprocessed_input(configuration);
	
	GList *transitions = automaton_get_transitions_from_state(automaton, state);
	
	GList *next_confs = NULL;
	
	// iterate over all the transitions that start from the state and create new configurations
	// for the transitions that match the input
	GList *current_transition_node = transitions;
	for (; current_transition_node != NULL; current_transition_node = current_transition_node->next) {
		Transition *t = (Transition *)current_transition_node->data;
		const char *symbol = fsa_transition_get_symbol(t);
		if (strlen(symbol) > 0) {
			if (strncmp(symbol, unprocessed_input, strlen(symbol)) == 0) {
				State *to_state = transition_get_to_state(t);
				/*Configuration *new_conf = fsa_configuration_new(to_state, configuration, input, 
										unprocessed_input + strlen(symbol));
				if (new_conf == NULL)
					return NULL;
				
				next_confs = g_list_append(next_confs, new_conf);*/
				
				// closure of the to_state
				GList *closure = closure_taker_get_closure(to_state, automaton);
				GList *current_closure_node = closure;
				for (; current_closure_node != NULL; current_closure_node = current_closure_node->next) {
					State *s = (State *)current_closure_node->data;
					Configuration *new_conf = fsa_configuration_new(s, NULL, input, unprocessed_input + strlen(symbol));
					next_confs = g_list_prepend(next_confs, new_conf);
				}
			}
		}
	}
	
	next_confs = g_list_reverse(next_confs);
	return next_confs;
}

int fsa_simulator_is_accepted_private(AutomatonSimulator *simulator)
{
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, simulator->current_configurations);
	Automaton *automaton = simulator->automaton;
	
	Configuration *conf = NULL;
	while (g_hash_table_iter_next(&iter, (gpointer *)&conf, NULL)) {
		State *s = configuration_get_state(conf);
		if (automaton_is_final_state(automaton, s) && 
			!strcmp(fsa_configuration_get_unprocessed_input(conf), "")) {
			return 1;	
		}
	}
	
	return 0;
}

int fsa_simulator_simulate_input_private(AutomatonSimulator *simulator, const char *input)
{
	g_hash_table_remove_all(simulator->current_configurations);

	GList *initial_confs = automaton_simulator_get_initial_configurations(simulator, input);
	
	for (int i = 0; i < g_list_length(initial_confs); i++) {
		g_hash_table_add(simulator->current_configurations, g_list_nth_data(initial_confs, i));
	}
	
	while (g_hash_table_size(simulator->current_configurations) > 0) {
		if (automaton_simulator_is_accepted(simulator)) {
			g_list_free(initial_confs);
			return 1;
		}
		GHashTableIter iter;
		g_hash_table_iter_init(&iter, simulator->current_configurations);
		
		Configuration *conf = NULL;
		GList *to_add_confs = NULL;
		while (g_hash_table_iter_next(&iter, (gpointer *)&conf, NULL)) {
			GList *next_confs = automaton_simulator_step_configuration(simulator, conf);
			to_add_confs = g_list_concat(to_add_confs, next_confs);
			g_hash_table_iter_remove(&iter);
		}
		// Add configurations to current configurations of simulator
		GList *current_tac_node = to_add_confs;
		for (; current_tac_node != NULL; current_tac_node = current_tac_node->next) {
			g_hash_table_add(simulator->current_configurations, current_tac_node->data);
		}
		g_list_free(to_add_confs);
	}
	
	g_list_free(initial_confs);
	
	return 0;
}
