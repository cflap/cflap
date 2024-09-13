/*
 * This file contains the interface and the structure of a finit state automaton transition
 */

typedef struct _FsaTransition FsaTransition;

#ifndef FSA_TRANSITION_H
#define FSA_TRANSITION_H

#include "transition.h"

#define FSA_TRANSITION_SYMBOL_OFFSET 5

Transition *fsa_transition_new(State *from_state, State *to_state, const char *symbol);

const char *fsa_transition_get_symbol(Transition *fsa_transition);
void fsa_transition_set_symbol(Transition *fsa_transition, const char *symbol);

#endif // FSA_TRANSITION_H
