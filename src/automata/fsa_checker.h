#pragma once

#include "fsa_transition.h"

gboolean fsa_are_non_deterministic(Transition *t1, Transition *t2);
GList *fsa_get_non_deterministic_states(Automaton *automaton);
gboolean fsa_is_nfa(Automaton *automaton);
gboolean fsa_is_epsilon_transition(Transition *t);