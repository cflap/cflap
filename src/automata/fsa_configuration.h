/*
 * This file contains the interface and the structure of a FsaConfiguration
 */

typedef struct _FsaConfiguration FsaConfiguration;

#ifndef FSA_CONFIGURATION_H
#define FSA_CONFIGURATION_H

#include "configuration.h"

Configuration *fsa_configuration_new(State *curr_state, Configuration *parent_config, const char *input, const char *unprocessed_input);

/*
 * Getters
 */
const char *fsa_configuration_get_input(Configuration *fsa_configuration);
const char *fsa_configuration_get_unprocessed_input(Configuration *fsa_configuration);

#endif // FSA_CONFIGURATION_H
