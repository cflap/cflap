typedef struct _AutomatonDrawer AutomatonDrawer;

#ifndef AUTOMATON_DRAWER_H
#define AUTOMATON_DRAWER_H

#include <gtk/gtk.h>
#include "automata/automaton.h"

struct _AutomatonDrawer {
	float zoom_multiplier;
	GdkRGBA primary;
	GdkRGBA secondary;
	
	// Transition tool points
	double tline_start_x;
	double tline_start_y;
	double tline_end_x;
	double tline_end_y;
	
	void (*draw)(AutomatonDrawer *, Automaton *, cairo_t *);
};

void automaton_drawer_init(AutomatonDrawer *drawer,
						float zoom_multiplier,
						GdkRGBA *primary,
						GdkRGBA *secondary,
						void (*draw)(AutomatonDrawer *, Automaton *, cairo_t *));
						
State *automaton_drawer_get_state_at(AutomatonDrawer *self, Automaton *automaton, double x, double y);
Transition *automaton_drawer_get_transition_at(AutomatonDrawer *self, Automaton *automaton, double x, double y);

//probably the only one
void automaton_drawer_draw(AutomatonDrawer *drawer, Automaton *automaton, cairo_t *cr);

#endif // AUTOMATON_DRAWER_H