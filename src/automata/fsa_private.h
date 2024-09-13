/*
 * This file contains the private interface of a fsa
 */

#ifndef FSA_PRIVATE_H
#define FSA_PRIVATE_H

#include "fsa.h"

struct _FsAutomaton {
	Automaton *super;
};

GtkWidget *fsa_create_transition_input_private(Automaton *automaton, State *from, State *to, gpointer automaton_page);
void fsa_destroy_private(Automaton *automaton);

#endif // FSA_PRIVATE_H
