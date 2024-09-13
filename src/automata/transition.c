/*
 * This file contains the implementation of the interface of a generic automaton transition
 */

#include <stdlib.h>

#include "transition_private.h"

/*
 * Initializes a new Transition
 */
Transition *transition_new(State *from_state, State *to_state)
{
	Transition *transition = malloc(sizeof(Transition));
	
	if (transition == NULL)
		return NULL;
	
	transition->derived = NULL;
	
	transition->from_state = from_state;
	transition->to_state = to_state;
	
	transition->destroy = &transition_destroy_private;
	transition->equals = &transition_equals_private;
	transition->hash_code = &transition_hash_code_private;
	transition->clone = &transition_clone_private;
	
	return transition;
}


/*
 * Private implementation of destroy
 * 
 */
void transition_destroy_private(Transition *transition)
{
	// If NULL free() does nothing
	free(transition);
}

/*
 * Destroys the transition
 */
void transition_destroy(Transition *transition)
{
	if (transition == NULL)
		return;
	
	transition->destroy(transition);
}

/*
 * Getters
 */
State *transition_get_from_state(Transition *transition)
{
	if (transition == NULL)
		return NULL;
	
	return transition->from_state;
}

State *transition_get_to_state(Transition *transition)
{
	if (transition == NULL)
		return NULL;
	
	return transition->to_state;
}

/*
 * Setters
 * Modify the Transition added to an Automaton could invalidate its structure.
 */
void transition_set_from_state(Transition *transition, State *state)
{
	if (transition == NULL)
		return;
	
	transition->from_state = state;
}

void transition_set_to_state(Transition *transition, State *state)
{
	if (transition == NULL)
		return;
	
	transition->to_state = state;
}

/*
 * Private implementation of equals
 */
int transition_equals_private(Transition *transition1, Transition *transition2)
{
	return (transition1->from_state == transition2->from_state) && (transition1->to_state == transition2->to_state);
}

/*
 * Returns TRUE (1) if t1 == t2, FALSE (0) if t1 != t2
 */
int transition_equals(Transition *transition1, Transition *transition2)
{
	if ((transition1 == NULL) && (transition2 == NULL))
		return 1;
		
	if ((transition1 == NULL) || (transition2 == NULL))
		return 0;
	
	return transition1->equals(transition1, transition2);
}

/*
 * Private implementation of hash_code
 * Probably THIS implementation will never be used
 */
guint transition_hash_code_private(Transition *transition)
{
	return ~(unsigned long int) transition->from_state ^ (unsigned long int) transition->to_state;
} 

/*
 * Hash code used by the hash_table for the key as hash
 * this function IS NOT NULL SAFE
 */
guint transition_hash_code(Transition *transition)
{
	return transition->hash_code(transition);
}

/*
 * Private implementation of clone
 */
Transition *transition_clone_private(Transition *transition)
{
	return transition_new(transition->from_state, transition->to_state);
}

/*
 * Create a clone of the transition in a new memory address
 */
Transition *transition_clone(Transition *transition)
{
	if (transition == NULL)
		return NULL;
	
	return transition->clone(transition);
}

void transition_draw(Transition *transition, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr, int arrow_style, int group_index)
{
	transition->draw(transition, zoom_multiplier, primary, secondary, cr, arrow_style, group_index);
}

gboolean transition_contains_point(Transition *transition, double x, double y)
{
	return transition->contains_point(transition, x, y);
}

GtkWidget *transition_get_popover(Transition *transition, gpointer automaton_page)
{
	return transition->get_popover(transition, automaton_page);
}