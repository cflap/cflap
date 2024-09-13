/*
 * This file contains the private interface of a generic configuration
 */

#ifndef CONFIGURATION_PRIVATE_H
#define CONFIGURATION_PRIVATE_H

#include "configuration.h"

struct _Configuration {
	void *derived;
	State *curr_state;
	Configuration *parent_config;
	
	void (*destroy)(Configuration *);
};

void configuration_destroy_private(Configuration *configuration);

#endif // CONFIGURATION_PRIVATE_H