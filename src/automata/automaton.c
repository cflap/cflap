/*
 *
 * This file contains the implementation of the interface of a generic automaton
 * to be inherited by more specific automata
 *
 */

#include <stdlib.h>

#include "automaton_private.h"

/*
 * Initializes an empty automaton
 */
Automaton *automaton_new(void)
{
	Automaton *automaton = malloc(sizeof(Automaton));
	
	// Check malloc() result, if automaton == NULL, malloc() couldn't allocate memory
	if (automaton == NULL)
		return NULL;
	
	automaton->derived = NULL;
	
	// Since this hashtable is used as a set, we only store keys, therefore on the destroy callback for value we set NULL
	// Setting the destroy callback on the key we destroy every state of this automaton freeing the memory
	automaton->states_set = g_hash_table_new_full(NULL, NULL, (GDestroyNotify)state_destroy, NULL);
	automaton->final_states_set = g_hash_table_new(NULL, NULL);
	
	// Setting the initial state to NULL for the newly created automaton (it means that the automaton doesn't have an initial state)
	automaton->initial_state = NULL;
	
	automaton->transitions_set = g_hash_table_new_full((GHashFunc)transition_hash_code, (GEqualFunc)transition_equals, (GDestroyNotify)transition_destroy, NULL);
	
	automaton->transition_from_state_map = g_hash_table_new(NULL, NULL);
	automaton->transition_to_state_map = g_hash_table_new(NULL, NULL);
	
	automaton->create_transition_input = NULL;
	
	automaton->destroy = &automaton_destroy;
	
	return automaton;
}

/*
 * Private implementation of destroy
 */
void automaton_destroy_private(Automaton *automaton)
{
	g_hash_table_unref(automaton->states_set);
	g_hash_table_unref(automaton->final_states_set);
	g_hash_table_unref(automaton->transitions_set);
	
	GHashTableIter iter;
	GList *list;
	
	g_hash_table_iter_init(&iter, automaton->transition_from_state_map);
	while (g_hash_table_iter_next(&iter, NULL, (gpointer *)&list)) {
		g_list_free(g_steal_pointer(&list));
	}
	g_hash_table_unref(automaton->transition_from_state_map);
	
	g_hash_table_iter_init(&iter, automaton->transition_to_state_map);
	while (g_hash_table_iter_next(&iter, NULL, (gpointer *)&list)) {
		g_list_free(g_steal_pointer(&list));
	}
	g_hash_table_unref(automaton->transition_to_state_map);
	
	free(automaton);
}

/*
 * Destroys the automaton
 */
void automaton_destroy(Automaton *automaton)
{
	if (automaton == NULL)
		return;
	
	automaton->destroy(automaton);
}

State *automaton_create_state(Automaton *automaton, double x, double y)
{
	if (automaton == NULL)
		return NULL;
	
	int id = 0;
	
	while (automaton_get_state_by_id(automaton, id) != NULL)
		++id;
	
	char *id_string = g_strdup_printf("%d", id);
	
	// Size of name = sizeof('q') + id_string + sizeof('\0')
	char name[1 + strlen(id_string) + 1];
	strcpy(name, "q");
	// Creating default state name as q*** where *** is the id (examples: q0, q11, q234...)
	strcat(name, id_string);
	State *new_state = state_new(id, name, automaton, x, y);
	
	// Add created state to the set
	g_hash_table_add(automaton->states_set, new_state);
	
	// Add created state transition_from_state_map with NULL value (NULL is considered an empty list)
	g_hash_table_insert(automaton->transition_from_state_map, new_state, NULL);
	
	// Add created state transition_to_state_map with NULL value (NULL is considered an empty list)
	g_hash_table_insert(automaton->transition_to_state_map, new_state, NULL);
	
	/*
	 * Release id_string since state constructor creates a copy of the name. 
	 * name is released when getting out of scope of this function
	 */
	free(id_string);
	
	return new_state;
}

/*
 * Removes the given state from the automaton's states set.
 * TODO remove transitions too
 */
void automaton_remove_state(Automaton *automaton, State *state)
{
	if (automaton == NULL)
		return;
	
	if (automaton->initial_state == state)
		automaton->initial_state = NULL;
	
	// removing all the transitions that contain the state to remove
	GList *transitions = g_hash_table_get_keys(automaton->transitions_set);
	
	GList *current_transition_node = transitions;
	for (; current_transition_node != NULL; current_transition_node = current_transition_node->next) {
		Transition *t = (Transition *)current_transition_node->data;
		
		State *from_state = transition_get_from_state(t);
		State *to_state = transition_get_to_state(t);
		// compare the states by address
		if (from_state == state || to_state == state)
			automaton_remove_transition(automaton, t);
	}
	
	g_list_free(transitions);
	
	g_hash_table_remove(automaton->states_set, state);
}

/*
 * Returns the list of the keys OWNED by the hashtable,
 * therefore CHANGES ON THE LIST ARE REFLECTED ON THE HASHTABLE
 */
GList *automaton_get_states_as_list(Automaton *automaton)
{
	if (automaton == NULL)
		return NULL;
	
	GList *states = g_hash_table_get_keys(automaton->states_set);
	return g_list_sort(states, (GCompareFunc) state_compare_by_id);
}

/*
 * Returns the list of the keys OWNED by the hashtable,
 * therefore CHANGES ON THE LIST ARE REFLECTED ON THE HASHTABLE
 */
GList *automaton_get_final_states_as_list(Automaton *automaton)
{
	if (automaton == NULL)
		return NULL;
	
	return g_hash_table_get_keys(automaton->final_states_set);
}

/*
 * Adds a state to the final states set, the state should be already in the states set of the automaton
 */
void automaton_add_final_state(Automaton *automaton, State *state)
{
	if (automaton == NULL)
		return;
	
	g_hash_table_add(automaton->final_states_set, state);
}

/*
 * Removes the given state from the automaton's final states set.
 */
void automaton_remove_final_state(Automaton *automaton, State *state)
{
	if (automaton == NULL)
		return;
	
	g_hash_table_remove(automaton->final_states_set, state);
}

/*
 * Sets the initial state of the automaton, the initial state should be already in the states of the automaton.
 * Old initial state is returned
 */
State *automaton_set_initial_state(Automaton *automaton, State *state)
{
	if (automaton == NULL)
		return NULL;
	
	State *old_initial_state = automaton->initial_state;
	automaton->initial_state = state;
	
	return old_initial_state;
}

/*
 * Returns the initial state of the automaton
 */
State *automaton_get_initial_state(Automaton *automaton)
{
	if (automaton == NULL)
		return NULL;
	
	return automaton->initial_state;
}

/*
 * Marks the automaton as not having an initial state.
 * The state is not removed from the automaton
 */
State *automaton_remove_initial_state(Automaton *automaton)
{
	if (automaton == NULL)
		return NULL;
		
	State *previous_initial_state = automaton->initial_state;
	automaton->initial_state = NULL;
	
	return previous_initial_state;
}

/*
 * Adds the given transition to the automaton
 */
void automaton_add_transition(Automaton *automaton, Transition *transition)
{
	if (automaton == NULL || transition == NULL)
		return;
	
	State *from_state = transition_get_from_state(transition);
	State *to_state = transition_get_to_state(transition);
	
	if (from_state == NULL || to_state == NULL)
		return;
	
	if (g_hash_table_contains(automaton->transitions_set, transition)) {
		transition_destroy(transition);
		return;
	}
	
	g_hash_table_add(automaton->transitions_set, transition);
	
	GList *list = g_hash_table_lookup(automaton->transition_from_state_map, from_state);
	list = g_list_append(list, transition);
	g_hash_table_insert(automaton->transition_from_state_map, from_state, list);
	
	list = g_hash_table_lookup(automaton->transition_to_state_map, to_state);
	list = g_list_append(list, transition);
	g_hash_table_insert(automaton->transition_to_state_map, to_state, list);
}

/*
 * Removes the transition from the automaton
 * note that the transition is removed from transitions_set, transition_from_state_map and transition_to_state_map
 */
void automaton_remove_transition(Automaton *automaton, Transition *transition)
{
	if (automaton == NULL || transition == NULL)
		return;
	
	State *from_state = transition_get_from_state(transition);
	State *to_state = transition_get_to_state(transition);
	
	GList *from_list = g_hash_table_lookup(automaton->transition_from_state_map, from_state);
	from_list = g_list_remove(from_list, transition);
	g_hash_table_insert(automaton->transition_from_state_map, from_state, from_list);
	
	GList *to_list = g_hash_table_lookup(automaton->transition_to_state_map, to_state);
	to_list = g_list_remove(to_list, transition);
	g_hash_table_insert(automaton->transition_to_state_map, to_state, to_list);
	
	g_hash_table_remove(automaton->transitions_set, transition);
}

/*
 * Returns the list of the keys OWNED by the hashtable,
 * therefore CHANGES TO STATES ON LIST ARE REFLECTED ON THE HASHTABLE
 */
GList *automaton_get_transitions_as_list(Automaton *automaton)
{
	if (automaton == NULL)
		return NULL;
	
	return g_hash_table_get_keys(automaton->transitions_set);
}

/*
 * Returns the list of transitions contained in transition_from_state_map with the gives state as key
 * hence all the transitions that start from that state,
 * the returned list is OWNED by the hashtable there fore CHANGED ON THE LIST ARE REFLECTED ON THE HASTABLE
 */
GList *automaton_get_transitions_from_state(Automaton *automaton, State *state)
{
	if (automaton == NULL)
		return NULL;
	
	return g_hash_table_lookup(automaton->transition_from_state_map, state);
}

/*
 * Returns the list of transitions contained in transition_to_state_map with the gives state as key
 * hence all the transitions that go to that state
 * the returned list is OWNED by the hashtable there fore CHANGED ON THE LIST ARE REFLECTED ON THE HASTABLE
 */
GList *automaton_get_transitions_to_state(Automaton *automaton, State *state)
{
	if (automaton == NULL)
		return NULL;
	
	return g_hash_table_lookup(automaton->transition_to_state_map, state);
}

/*
 * Tests if two automatons are equal considering the pointer address
 * returns 0 (false) if they're not equal
 * return 1 (true) if they're equal
 */
int automaton_equals(Automaton *automaton1, Automaton *automaton2)
{
	return automaton1 == automaton2;
}

/*
 * Returns TRUE (1) if the given State is the initial state of the Automaton
 * FALSE (0) otherwise
 */
int automaton_is_initial_state(Automaton *automaton, State *state)
{
	return state == automaton->initial_state;
}

/*
 * Returns TRUE (1) if the given State is a final state in the given Automaton
 * FALSE (0) otherwise
 */
int automaton_is_final_state(Automaton *automaton, State *state)
{
	return g_hash_table_contains(automaton->final_states_set, state);
}

GtkWidget *automaton_create_transition_input(Automaton *automaton, State *from, State *to, gpointer automaton_page)
{
	return automaton->create_transition_input(automaton, from, to, automaton_page);
}

State *automaton_get_state_by_id(Automaton *automaton, int id)
{
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, automaton->states_set);
	State *state = NULL;
	while (g_hash_table_iter_next(&iter, (gpointer *) &state, NULL)) {
		int state_id = state_get_id(state);
		if (state_id == id) {
			return state;
		}
	}
	
	return NULL;
}