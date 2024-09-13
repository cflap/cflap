/*
 * This file contains the private interface of transition
 */

#ifndef TRANSITION_PRIVATE_H
#define TRANSITION_PRIVATE_H

#include "transition.h"

struct _Transition {
	void *derived;
	State *from_state;
	State *to_state;
	
	double x1;
	double y1;
	double x2;
	double y2;
	double x3;
	double y3;
	double x4;
	double y4;
	
	void (*destroy)(Transition *);
	int (*equals)(Transition *, Transition *);
	guint (*hash_code)(Transition *);
	Transition *(*clone)(Transition *);
	void (*draw)(Transition *, float, GdkRGBA *, GdkRGBA*, cairo_t *, int, int);
	gboolean (*contains_point)(Transition *, double, double);
	GtkWidget *(*get_popover)(Transition *, gpointer);
};

void transition_destroy_private(Transition *transition);

int transition_equals_private(Transition *transition1, Transition *transition2);

guint transition_hash_code_private(Transition *transition);

Transition *transition_clone_private(Transition *transition);

#endif // TRANSITION_PRIVATE_H
