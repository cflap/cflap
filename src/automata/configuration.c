/*
 *
 * This file contains the implementation of the interface of
 * a generic configuration to be inherited by more specific configurations
 *
 */

#include <stdlib.h>

#include "configuration_private.h"

/*
 * Initializes a new configuration with a current state and a parent configuration (can be NULL)
 */
Configuration *configuration_new(State *curr_state, Configuration *parent_config)
{
	Configuration *configuration = malloc(sizeof(Configuration));
	
	// if malloc couldn't allocate memory for a new Configuration return NULL
	if (configuration == NULL)
		return NULL;
	
	configuration->derived = NULL;
	configuration->curr_state = curr_state;
	configuration->parent_config = parent_config;
	
	configuration->destroy = &configuration_destroy_private;
	
	return configuration;
}

/*
 * Private implementation of the destroy
 */
void configuration_destroy_private(Configuration *configuration)
{	
	free(configuration);
}

/*
 * Destroys the configuration
 */
void configuration_destroy(Configuration *configuration)
{
	if (configuration == NULL)
		return;
	
	configuration->destroy(configuration);
}

/*
 * Getters
 */

State *configuration_get_state(Configuration *configuration)
{
	if (configuration == NULL)
		return NULL;
	
	return configuration->curr_state;
}

Configuration *configuration_get_parent(Configuration *configuration)
{
	if (configuration == NULL)
		return NULL;
	
	return configuration->parent_config;
}
