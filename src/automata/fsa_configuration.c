/*
 * This file contains the implementation of the interface of a FsaConfiguration
 */
 
#include <stdlib.h>

#include "configuration_private.h"
#include "fsa_configuration_private.h"

/*
 * Initializes a new FsaConfiguration with a current state and a parent configuration (can be NULL)
 * an input (at least blank string) and an unprocessed input (at least blank string)
 */
Configuration *fsa_configuration_new(State *curr_state, Configuration *parent_config, const char *input, const char *unprocessed_input)
{
	if (input == NULL || unprocessed_input == NULL) {
		return NULL;
	}
	
	FsaConfiguration *fsa_config = malloc(sizeof(FsaConfiguration));
	
	if (fsa_config == NULL) 
		return NULL;
		
	Configuration *super = configuration_new(curr_state, parent_config);
	
	if (super == NULL) {
		free(fsa_config);
		return NULL;
	}
	
	super->derived = fsa_config;
	super->destroy = &fsa_configuration_destroy_private;
	
	fsa_config->super = super;
	fsa_config->input = input;
	fsa_config->unprocessed_input = unprocessed_input;
	
	return fsa_config->super;
}

/*
 * Destroys the FsaConfiguration
 */
void fsa_configuration_destroy_private(Configuration *configuration)
{
	if (configuration == NULL)
		return;
	
	FsaConfiguration *fsa_configuration = configuration->derived;
	//free((char *)fsa_configuration->unprocessed_input);
	free(fsa_configuration);
	
	configuration_destroy_private(configuration);
}

/*
 * Getters
 */
const char *fsa_configuration_get_input(Configuration *fsa_configuration)
{
	if (fsa_configuration == NULL)
		return NULL;
	
	return ((FsaConfiguration *)fsa_configuration->derived)->input;
}

const char *fsa_configuration_get_unprocessed_input(Configuration *fsa_configuration)
{
	if (fsa_configuration == NULL)
		return NULL;
	
	return ((FsaConfiguration *)fsa_configuration->derived)->unprocessed_input;
}
