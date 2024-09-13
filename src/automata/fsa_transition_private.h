/*
 * This file contains the private interface of fsa transition
 */

#ifndef FSA_TRANSITION_PRIVATE_H
#define FSA_TRANSITION_PRIVATE_H

#include "fsa_transition.h"

struct _FsaTransition {
	Transition *super;
	void *derived;
	char *symbol;
};

void fsa_transition_destroy_private(Transition *transition);

int fsa_transition_equals_private(Transition *transition1, Transition *transition2);
guint fsa_transition_hash_code_private(Transition *transition);
Transition *fsa_transition_clone_private(Transition *transition);

void fsa_transition_draw_private(Transition *transition, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr, int arrow_style, int group_style);
gboolean fsa_transition_contains_point_private(Transition *transition, double x, double y);

GtkWidget *fsa_transition_get_popover_private(Transition *transition, gpointer automaton_page);

#endif // FSA_TRANSITION_PRIVATE_H