#pragma once

#include "fsa.h"

void fsa_min_remove_unreachable_states(Automaton *automaton);
Automaton *fsa_min_minimize(Automaton *automaton);