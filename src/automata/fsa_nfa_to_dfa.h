#pragma once

#include "fsa.h"
#include "fsa_alphabet_retriever.h"
#include "fsa_checker.h"

State *ntd_create_initial_state(Automaton *nfa, Automaton *dfa);
gboolean ntd_has_final_state(GList *states, Automaton *automaton);
GList *ntd_get_states_for_state(State *state, Automaton *automaton);
char *ntd_get_string_for_states(GList *states);
GList *ntd_get_states_on_terminal(const char *terminal, GList *states, Automaton *automaton);
gboolean ntd_contains_state(State *state, GList *states);
gboolean ntd_contain_same_states(GList *states1, GList *states2);
State *ntd_get_state_for_states(GList *states, Automaton *dfa, Automaton *nfa);
GList *ntd_expand_state(State *state, Automaton *nfa, Automaton *dfa);
State *ntd_create_state_with_states(Automaton *dfa, GList *states, Automaton *nfa);
Automaton *ntd_convert_to_dfa(Automaton *automaton);
void remove_multiple_characters_label_from_automaton(Automaton *automaton);