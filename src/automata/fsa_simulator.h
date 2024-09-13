/*
 * This file contains the interface and the structure of a finite state automaton simulator
 */

typedef struct _FsaSimulator FsaSimulator;

#ifndef FSA_SIMULATOR_H
#define FSA_SIMULATOR_H

#include "automaton_simulator.h"

AutomatonSimulator *fsa_simulator_new(Automaton *automaton);

#endif // FSA_SIMULATOR_H