/*
 * This file contains the interface and the structure of a generic automaton transition
 */
 
typedef struct _Transition Transition;

#ifndef TRANSITION_H
#define TRANSITION_H

#include <gtk/gtk.h>

#include "state.h"

#define TRANSITION_ARROW_LENGTH 15.0
#define TRANSITION_ARROW_ARC 25
#define TRANSITION_ARROW_ARC_LOOP 38
#define TRANSITION_ARROW_LOOP 0
#define TRANSITION_ARROW_STRAIGHT 1
#define TRANSITION_ARROW_CURVED 2

// THIS CLASS IS MEANT TO BE ABSTRACT, USE THIS METHOD ONLY WHEN EXTENDING
Transition *transition_new(State *from_state, State *to_state);
void transition_destroy(Transition *transition);

/*
 * Getters
 */
State *transition_get_from_state(Transition *transition);
State *transition_get_to_state(Transition *transition);

/*
 * Setters
 */
void transition_set_from_state(Transition *transition, State *state);
void transition_set_to_state(Transition *transition, State *state);

int transition_equals(Transition *transition1, Transition *transition2);

guint transition_hash_code(Transition *transition);

Transition *transition_clone(Transition *transition);

void transition_draw(Transition *transition, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr, int, int);
gboolean transition_contains_point(Transition *transition, double x, double y);

GtkWidget *transition_get_popover(Transition *transition, gpointer automaton_page);

#endif // TRANSITION_H
