/*
 *
 * This file contains the implementation of the interface of
 * a generic automaton state to be inherited by more specific automaton states
 *
 */

#include <stdlib.h>
#include <string.h>

#include "state_private.h"
#include "../automatonpage.h"

/*
 * Initializes a new State
 */
State *state_new(int id, const char *name, Automaton *automaton, double x, double y)
{
	State *state = malloc(sizeof(State));
	
	if (state == NULL)
		return NULL;
	
	state->derived = NULL;	
	
	state->id = id;
	
	if (name == NULL)
		name = "";
	state->name = strdup(name);
	// Check if strdup() returned NULL
	if (state->name == NULL)
		return NULL;
	
	state->label = NULL;
	
	state->x = x;
	state->y = y;
	
	state->automaton = automaton;
	
	state->destroy = &state_destroy_private;
	state->equals = &state_equals_private;
	
	state->draw = &state_draw_private;
	state->get_popover = &state_get_popover_private;
	
	return state;
}

/*
 * Private implementation of destroy
 */
void state_destroy_private(State *state)
{
	free(state->name);
	free(state->label);
	free(state);
}

/*
 * Destroys the state
 */
void state_destroy(State *state)
{
	if (state == NULL)
		return;
	
	state->destroy(state);
}

/*
 * Getters
 */

int state_get_id(State *state)
{
	if (state == NULL)
		return -1;
		
	return state->id;
}

const char *state_get_name(State *state)
{
	if (state == NULL)
		return NULL;
		
	return state->name;
}

Automaton *state_get_automaton(State *state)
{
	if (state == NULL)
		return NULL;
	
	return state->automaton;
}

double state_get_x(State *state)
{
	return state->x;
}

double state_get_y(State *state)
{
	return state->y;
}

/*
 * Setters
 */
 
void state_set_id(State *state, int id)
{
	if (state == NULL)
		return;
	
	state->id = id;
}

void state_set_name(State *state, const char *name)
{
	if (state == NULL || name == NULL)
		return;
		
	char *new_name = strdup(name);
	if (new_name == NULL)
		return;
	else {
		free(state->name);
		state->name = new_name;
	}
}

void state_set_automaton(State *state, Automaton *automaton)
{
	if (state == NULL)
		return;
	
	state->automaton = automaton;
}

void state_set_x(State *state, double x)
{
	state->x = x;
}

void state_set_y(State *state, double y)
{
	state->y = y;
}

/*
 * Compares two given states using the ids of the states
 * returns negative value if state1 < state2
 * 0 if state1 == state2
 * positive value if state1 > state2
 */
int state_compare_by_id(State *state1, State *state2)
{
	if ((state1 == NULL) && (state2 == NULL))
		return 0;
	
	if (state1 == NULL)
		return -1;
	
	if (state2 == NULL)
		return 1;
	
	int id1 = state1->id;
	int id2 = state2-> id;
	
	return id1 - id2;
}

/*
 * Compares two given states using the names of the states
 * returns negative value if state1 < state2
 * 0 if state1 == state2
 * positive value if state1 > state2
 */
int state_compare_by_name(State *state1, State *state2)
{
	if ((state1 == NULL) && (state2 == NULL))
		return 0;
	
	if (state1 == NULL)
		return -1;
	
	if (state2 == NULL)
		return 1;
	
	return strcmp(state1->name, state2->name);
}

/*
 * Compares two given states using the automatons which the states belong to
 * returns 0 if state1 == state2
 * returns 1 if state1 != state2
 */
int state_compare_by_automaton(State *state1, State *state2)
{
	if ((state1 == NULL) && (state2 == NULL))
		return 0;
	
	if (state1 == NULL)
		return -1;
	
	if (state2 == NULL)
		return 1;
	
	// TODO change automaton equals with the overridable
	return !automaton_equals(state1->automaton, state2->automaton);
}

/*
 * Private implementation of equals
 */
int state_equals_private(State *state1, State *state2)
{
	return (!state_compare_by_id(state1, state2) && !state_compare_by_name(state1, state2) && !state_compare_by_automaton(state1, state2));
}

/*
 * Tests if the two states are equals or not
 * returns 0 (false) if their not equals
 * returns 1 (true) if their equals
 */
int state_equals(State *state1, State *state2)
{	
	if (state1 == NULL && state2 == NULL)
		return 1;
	
	if (state1 == NULL || state2 == NULL)
		return 0;
		
	return state1->equals(state1, state2);
}

void state_draw_private(State *state, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr)
{
	double x = state_get_x(state) * zoom_multiplier;
	double y = state_get_y(state) * zoom_multiplier;
	const char *state_name = state_get_name(state);
	
	cairo_new_path(cr);
	cairo_arc(cr, x, y, STATE_RADIUS * zoom_multiplier, 0, 2 * G_PI);
	
	gdk_cairo_set_source_rgba(cr, secondary);
	cairo_fill_preserve(cr);
	
	gdk_cairo_set_source_rgba(cr, primary);
	cairo_set_line_width(cr, STATE_LINE_WIDTH * zoom_multiplier);
	cairo_stroke(cr);
	
	cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, STATE_FONT_SIZE * zoom_multiplier);
	cairo_text_extents_t te;
	cairo_text_extents(cr, state_name, &te);
	cairo_move_to(cr, x - te.width / 2.0, y + te.height / 2.0);

	cairo_show_text(cr, state_name);
	
	Automaton *automaton = state_get_automaton(state);
	if (automaton_is_initial_state(automaton, state)) {
		double triangle_size = STATE_RADIUS * zoom_multiplier * STATE_TRIANGLE_MULTIPLIER;
		
		cairo_new_path(cr);
		cairo_move_to(cr, x - STATE_RADIUS * zoom_multiplier, y);
		cairo_line_to(cr, x - STATE_RADIUS * zoom_multiplier - triangle_size, y - triangle_size);
		cairo_line_to(cr, x - STATE_RADIUS * zoom_multiplier - triangle_size, y + triangle_size);
		cairo_close_path(cr);
		
		gdk_cairo_set_source_rgba(cr, secondary);
		cairo_fill_preserve(cr);  // Fill the triangle and preserve the path for stroking

		// Stroke the triangle's outline
		gdk_cairo_set_source_rgba(cr, primary);
		cairo_stroke(cr);
	}
	
	if (automaton_is_final_state(automaton, state)) {
		cairo_new_path(cr);
		cairo_arc(cr, x, y, INTERNAL_CIRCLE_RADIUS * zoom_multiplier, 0, 2 * G_PI);
		cairo_set_line_width(cr, STATE_LINE_WIDTH * zoom_multiplier);
		cairo_stroke(cr);
	}
}

void state_draw(State *state, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr)
{
	state->draw(state, zoom_multiplier, primary, secondary, cr);
}

struct state_info {
	State *state;
	GuiAutomatonPage *automaton_page;
};

static void callback_popover_close(GtkPopover *self, gpointer user_data)
{
	gtk_widget_unparent(GTK_WIDGET(self));
	free(user_data);
}

static void initial_toggle_callback(GtkCheckButton *self, gpointer user_data)
{
	gboolean status = gtk_check_button_get_active(self);
	
	struct state_info *data = (struct state_info *) user_data;
	State *state = data->state;
	GuiAutomatonPage *automaton_page = data->automaton_page;
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	
	if (status) {
		automaton_set_initial_state(automaton, state);
	} else {
		automaton_remove_initial_state(automaton);
	}
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
	gui_automaton_page_update_initial_state(automaton_page);
}

static void final_toggle_callback(GtkCheckButton *self, gpointer user_data)
{
	gboolean status = gtk_check_button_get_active(self);
	
	struct state_info *data = (struct state_info *) user_data;
	State *state = data->state;
	GuiAutomatonPage *automaton_page = data->automaton_page;
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	
	if (status) {
		automaton_add_final_state(automaton, state);
	} else {
		automaton_remove_final_state(automaton, state);
	}
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}

static void name_changed_inserted_callback(GtkEntryBuffer *self, guint position, gchar *chars, guint n_chars, gpointer user_data)
{
	struct state_info *data = (struct state_info *) user_data;
	State *state = data->state;
	GuiAutomatonPage *automaton_page = data->automaton_page;
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	
	state_set_name(state, gtk_entry_buffer_get_text(self));
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
	gui_automaton_page_update_initial_state(automaton_page);
}

static void name_changed_deleted_callback(GtkEntryBuffer *self, guint position, guint n_chars, gpointer user_data)
{
	struct state_info *data = (struct state_info *) user_data;
	State *state = data->state;
	GuiAutomatonPage *automaton_page = data->automaton_page;
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	
	state_set_name(state, gtk_entry_buffer_get_text(self));
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
	gui_automaton_page_update_initial_state(automaton_page);
}

void delete_button_callback(GtkButton *self, gpointer user_data)
{
	struct state_info *data = (struct state_info *) user_data;
	State *state = data->state;
	GuiAutomatonPage *automaton_page = data->automaton_page;
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	
	automaton_remove_state(automaton, state);
	
	GtkWidget *popover = gtk_widget_get_ancestor(GTK_WIDGET(self), GTK_TYPE_POPOVER);
	gtk_popover_popdown(GTK_POPOVER(popover));
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
	gui_automaton_page_update_initial_state(automaton_page);
	gui_automaton_page_update_transitions_number(automaton_page);
	gui_automaton_page_update_states_number(automaton_page);
	gui_automaton_page_update_alphabet(automaton_page);
}

GtkWidget *state_get_popover_private(State *state, gpointer automaton_page)
{
	Automaton *automaton = state_get_automaton(state);
	const char *state_name = state_get_name(state);
	
	struct state_info *data = malloc(sizeof(struct state_info));
	data->state = state;
	data->automaton_page = automaton_page;
	
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	GtkWidget *title = gtk_label_new_with_mnemonic("Edit state");
	
	GtkWidget *name_entry = gtk_entry_new();
	GtkEntryBuffer *name_buffer = gtk_entry_get_buffer(GTK_ENTRY(name_entry));
	gtk_entry_buffer_set_text(name_buffer, state_name, strlen(state_name));
	
	g_signal_connect(name_buffer, "inserted-text", G_CALLBACK(name_changed_inserted_callback), data);
	g_signal_connect_after(name_buffer, "deleted-text", G_CALLBACK(name_changed_deleted_callback), data);
	
	GtkWidget *initial_check = gtk_check_button_new_with_label("Initial");
	GtkWidget *final_check = gtk_check_button_new_with_label("Final");
	
	gtk_check_button_set_active(GTK_CHECK_BUTTON(initial_check), automaton_is_initial_state(automaton, state));
	gtk_check_button_set_active(GTK_CHECK_BUTTON(final_check), automaton_is_final_state(automaton, state));
		
	g_signal_connect(initial_check, "toggled", G_CALLBACK(initial_toggle_callback), data);
	g_signal_connect(final_check, "toggled", G_CALLBACK(final_toggle_callback), data);
	
	GtkWidget *delete_button = gtk_button_new_with_label("Delete");
	g_signal_connect(delete_button, "clicked", G_CALLBACK(delete_button_callback), data);
	
	gtk_box_append(GTK_BOX(box), title);
	gtk_box_append(GTK_BOX(box), name_entry);
	gtk_box_append(GTK_BOX(box), initial_check);
	gtk_box_append(GTK_BOX(box), final_check);
	gtk_box_append(GTK_BOX(box), delete_button);
	
	GtkWidget *popover = gtk_popover_new();
	gtk_popover_set_child(GTK_POPOVER(popover), box);
		
	g_signal_connect(popover, "closed", G_CALLBACK(callback_popover_close), data);
	
	return popover;
}

GtkWidget *state_get_popover(State *state, gpointer automaton_page)
{
	return state->get_popover(state, automaton_page);
}

const char *state_get_label(State *state)
{
	return state->label;
}

void state_set_label(State *state, char *label)
{
	free(state->label);
	state->label = strdup(label);
}