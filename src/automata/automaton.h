/*
 *
 * This file contains the interface and the structure of a generic automaton
 *
 */

typedef struct _Automaton Automaton;

#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <glib.h>

#include "state.h"
#include "transition.h"
#include "../automatonpage.h"

Automaton *automaton_new(void);
void automaton_destroy(Automaton *automaton);

State *automaton_create_state(Automaton *automaton, double x, double y);
void automaton_remove_state(Automaton *automaton, State *state);
GList *automaton_get_states_as_list(Automaton *automaton);

void automaton_add_final_state(Automaton *automaton, State *state);
void automaton_remove_final_state(Automaton *automaton, State *state);
GList *automaton_get_final_states_as_list(Automaton *automaton);

State *automaton_set_initial_state(Automaton *automaton, State *state);
State *automaton_get_initial_state(Automaton *automaton);
State *automaton_remove_initial_state(Automaton *automaton);

void automaton_add_transition(Automaton *automaton, Transition *transition);
void automaton_remove_transition(Automaton *automaton, Transition *transition);
GList *automaton_get_transitions_as_list(Automaton *automaton);
GList *automaton_get_transitions_from_state(Automaton *automaton, State *state);
GList *automaton_get_transitions_to_state(Automaton *automaton, State *state);

int automaton_equals(Automaton *automaton1, Automaton *automaton2);

int automaton_is_initial_state(Automaton *automaton, State *state);
int automaton_is_final_state(Automaton *automaton, State *state);

GtkWidget *automaton_create_transition_input(Automaton *automaton, State *from, State *to, gpointer automaton_page);

State *automaton_get_state_by_id(Automaton *automaton, int id);

#endif // AUTOMATON_H
