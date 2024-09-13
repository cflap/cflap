/*
 * This file contains the interface and the structure of a FsaAutomaton
 */

typedef struct _FsAutomaton FsAutomaton;

#ifndef FSA_AUTOMATON_H
#define FSA_AUTOMATON_H

#include "automaton.h"
#include "fsa_transition.h"

Automaton *fsa_new(void);
//void fsa_automaton_destroy(Automaton *automaton);

Transition *fsa_change_transition_symbol(Automaton *automaton, Transition *transition, const char *symbol);

#endif // FSA_AUTOMATON_H
