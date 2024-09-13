/*
 * This file contains the interface and the structure of a generic configuration
 */

typedef struct _Configuration Configuration;

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "state.h"

Configuration *configuration_new(State *curr_state, Configuration *parent_config);
void configuration_destroy(Configuration *configuration);
 
State *configuration_get_state(Configuration *configuration);
Configuration *configuration_get_parent(Configuration *configuration);

#endif // CONFIGURATION_H
