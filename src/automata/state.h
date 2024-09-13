/*
 * This file contains the interface and the structure of a generic automaton state
 */

typedef struct _State State;

#ifndef STATE_H
#define STATE_H

#include <glib.h>
#include <gtk/gtk.h>

#include "automaton.h"

#define STATE_RADIUS 28
#define INTERNAL_CIRCLE_RADIUS 24
#define STATE_TRIANGLE_MULTIPLIER 0.75
#define STATE_LINE_WIDTH 2
#define STATE_FONT_SIZE 16.0

State *state_new(int id, const char *name, Automaton *automaton, double x, double y);
void state_destroy(State *state);

/*
 * Getters
 */
int state_get_id(State *state);
const char *state_get_name(State *state);
Automaton *state_get_automaton(State *state);
double state_get_x(State *state);
double state_get_y(State *state);
const char *state_get_label(State *state);

/*
 * Setters
 */
void state_set_id(State *state, int id);
void state_set_name(State *state, const char *name);
void state_set_automaton(State *state, Automaton *automaton);
void state_set_x(State *state, double x);
void state_set_y(State *state, double y);
void state_set_label(State *state, char *label);

/*
 * Comparators
 */
int state_compare_by_id(State *state1, State *state2);
int state_compare_by_name(State *state1, State *state2);
int state_compare_by_automaton(State *state1, State *state2);
int state_equals(State *state1, State *state2);

void state_draw(State *state, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr);

GtkWidget *state_get_popover(State *state, gpointer automaton_page);

#endif // STATE_H
