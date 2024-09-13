#pragma once

#include "fsa_transition.h"

GList *closure_taker_get_closure(State *state, Automaton *automaton);