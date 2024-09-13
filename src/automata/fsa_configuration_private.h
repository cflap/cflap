/*
 * This file contains the private interface of a FsaConfiguration
 */

#ifndef FSA_CONFIGURATION_PRIVATE_H
#define FSA_CONFIGURATION_PRIVATE_H

#include "fsa_configuration.h"

struct _FsaConfiguration {
	Configuration *super;
	void *derived;
	const char *input;
	const char *unprocessed_input;
};

void fsa_configuration_destroy_private(Configuration *configuration);

#endif // FSA_CONFIGURATION_PRIVATE_H