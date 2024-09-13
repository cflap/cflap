/*
 * This file contains the private interface of state
 */

#ifndef STATE_PRIVATE_H
#define STATE_PRIVATE_H

#include "state.h"

struct _State {
	void *derived;
	int id;
	char *name;
	char *label;
	Automaton *automaton;
	
	double x;
	double y;
	
	void (*destroy)(State *);
	int (*equals)(State *, State *);
	void (*draw)(State *, float, GdkRGBA *, GdkRGBA*, cairo_t *);
	GtkWidget *(*get_popover)(State *, gpointer);
};

void state_destroy_private(State *state);

int state_equals_private(State *state1, State *state2);

void state_draw_private(State *state, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr);

GtkWidget *state_get_popover_private(State *state, gpointer automaton_page);

#endif // STATE_PRIVATE_H
