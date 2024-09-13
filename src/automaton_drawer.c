#include "automaton_drawer.h"

void automaton_drawer_init(AutomatonDrawer *drawer,
						float zoom_multiplier,
						GdkRGBA *primary,
						GdkRGBA *secondary,
						void (*draw)(AutomatonDrawer *, Automaton *, cairo_t *))
{
	drawer->zoom_multiplier = zoom_multiplier;
	
	drawer->primary.red = primary->red;
	drawer->primary.green = primary->green;
	drawer->primary.blue = primary->blue;
	drawer->primary.alpha = primary->alpha;
	
	drawer->secondary.red = secondary->red;
	drawer->secondary.green = secondary->green;
	drawer->secondary.blue = secondary->blue;
	drawer->secondary.alpha = secondary->alpha;
	
	drawer->tline_start_x = 0;
	drawer->tline_start_y = 0;
	drawer->tline_end_x = 0;
	drawer->tline_end_y = 0;
	
	drawer->draw = &automaton_drawer_draw;
}

State *automaton_drawer_get_state_at(AutomatonDrawer *self, Automaton *automaton, double x, double y)
{
	GList *states = automaton_get_states_as_list(automaton);
	
	GList *current_state_node = g_list_last(states);
	for (; current_state_node != NULL; current_state_node = current_state_node->prev) {
		State *s = (State *)current_state_node->data;
		double distance = sqrt(pow(x - state_get_x(s), 2) + pow(y - state_get_y(s), 2));
		if (distance <= STATE_RADIUS) {
			return s;
		}
	}
	
	return NULL;
}

static GHashTable *group_transitions_by_to_state(GList *list)
{
	GHashTable *groups = g_hash_table_new_full(NULL, (GEqualFunc) state_equals, NULL, (GDestroyNotify) g_list_free);
	
	GList *current_transition_node = list;
	for (; current_transition_node != NULL; current_transition_node = current_transition_node->next) {
		Transition *t = (Transition *)current_transition_node->data;
		State *to_state = transition_get_to_state(t);
		GList *to_state_transitions = NULL;
		if (g_hash_table_lookup_extended(groups, to_state, NULL, (gpointer *) &to_state_transitions)) {	
			to_state_transitions = g_list_append(to_state_transitions, t);
		} else {
			to_state_transitions = g_list_append(to_state_transitions, t);
			g_hash_table_insert(groups, to_state, to_state_transitions);
		}
	}
	
	return groups; // Must be freed by the caller
}

Transition *automaton_drawer_get_transition_at(AutomatonDrawer *self, Automaton *automaton, double x, double y)
{
	GList *transitions = automaton_get_transitions_as_list(automaton);
	
	GList *current_transition_node = transitions;
	for (; current_transition_node != NULL; current_transition_node = current_transition_node->next) {
		Transition *transition = (Transition *)current_transition_node->data;
		if (transition_contains_point(transition, x * self->zoom_multiplier, y * self->zoom_multiplier)) {
			g_list_free(transitions);
			return transition;
		}
	}
	g_list_free(transitions);
	return NULL;
}

//probably the only one
void automaton_drawer_draw(AutomatonDrawer *drawer, Automaton *automaton, cairo_t *cr)
{
	GList *states = automaton_get_states_as_list(automaton);
	
	GList *current_state_node = states;
	for (; current_state_node != NULL; current_state_node = current_state_node->next) {
		State *from_state = (State *)current_state_node->data;
		GList *transitions = automaton_get_transitions_from_state(automaton, from_state);
		GHashTable *grouped_transitions = group_transitions_by_to_state(transitions);
		GHashTableIter iter;
		g_hash_table_iter_init(&iter, grouped_transitions);
		State *to_state = NULL;
		transitions = NULL;
		while (g_hash_table_iter_next(&iter, (gpointer *) &to_state, (gpointer *) &transitions)) {
			int arrow_style;
			if (state_equals(from_state, to_state)) {
				arrow_style = TRANSITION_ARROW_LOOP;
			} else {
				arrow_style = TRANSITION_ARROW_STRAIGHT;
				GList *to_from_state = automaton_get_transitions_to_state(automaton, from_state);
				GList *current_tfs_node = to_from_state;
				for (; current_tfs_node != NULL; current_tfs_node = current_tfs_node->next) {
					Transition *to_check = (Transition *)current_tfs_node->data;
					State *from_state_to_check = transition_get_from_state(to_check);
					if (state_equals(from_state_to_check, to_state)) {
						arrow_style = TRANSITION_ARROW_CURVED;
						break;
					}
				}
			}
			
			GList *current_transition_node = transitions;
			for (int j = 0; current_transition_node != NULL; current_transition_node = current_transition_node->next,j++) {
				Transition *to_draw = (Transition *)current_transition_node->data;
				transition_draw(to_draw, drawer->zoom_multiplier, &(drawer->primary), &(drawer->secondary), cr, arrow_style, j);
			}
		}
		g_hash_table_unref(grouped_transitions);
	}
	
	current_state_node = states;
	for (; current_state_node != NULL; current_state_node = current_state_node->next) {
		State *s = (State *)current_state_node->data;
		state_draw(s, drawer->zoom_multiplier, &(drawer->primary), &(drawer->secondary), cr);
	}
	
	g_list_free(states);
}