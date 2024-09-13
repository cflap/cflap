/*
 * This file contains the implementation of the interface of a finite state automaton transition
 */

#include <stdlib.h>

#include "transition_private.h"
#include "fsa_transition_private.h"
#include "fsa.h"

/*
 * Initializes a new fsa_transition with two states and a symbol
 */
Transition *fsa_transition_new(State *from_state, State *to_state, const char *symbol)
{
	FsaTransition *fsa_transition = malloc(sizeof(FsaTransition));
	
	if (fsa_transition == NULL)
		return NULL;
	
	Transition *super = transition_new(from_state, to_state);
	
	if (super == NULL) {
		free(fsa_transition);
		return NULL;
	}
	
	super->derived = fsa_transition;
	super->destroy = &fsa_transition_destroy_private;
	super->equals = &fsa_transition_equals_private;
	super->hash_code = &fsa_transition_hash_code_private;
	super->clone = &fsa_transition_clone_private;
	super->draw = &fsa_transition_draw_private;
	super->contains_point = &fsa_transition_contains_point_private;
	super->get_popover = &fsa_transition_get_popover_private;
	fsa_transition->super = super;
	fsa_transition->derived = NULL;
	
	if (symbol == NULL) {
		free(super);
		free(fsa_transition);
	}
	
	fsa_transition->symbol = strdup(symbol);
	
	if (fsa_transition->symbol == NULL) {
		free(super);
		free(fsa_transition);
		return NULL;
	}
	
	return super;
}

/*
 * Destroys the transition
 */
void fsa_transition_destroy_private(Transition *transition)
{
	if (transition == NULL)
		return;
	
	FsaTransition *fsa_transition = transition->derived;
	
	free(fsa_transition->symbol);
	free(fsa_transition);
	transition_destroy_private(transition);
}

/*
 * Getters
 */

const char *fsa_transition_get_symbol(Transition *fsa_transition)
{
	if (fsa_transition == NULL)
		return NULL;
	
	return ((FsaTransition *)fsa_transition->derived)->symbol;
}

/*
 * Setters
 */

void fsa_transition_set_symbol(Transition *fsa_transition, const char *symbol)
{
	if (fsa_transition == NULL || symbol == NULL)
		return;
		
	char *new_symbol = strdup(symbol);
	if (new_symbol == NULL)
		return;
	else {
		free(((FsaTransition *)fsa_transition->derived)->symbol);
		((FsaTransition *)fsa_transition->derived)->symbol = new_symbol;
	}
}

/*
 * Tests if two (fsa) transitions are equal
 */
int fsa_transition_equals_private(Transition *transition1, Transition *transition2)
{
	if ((transition1 == NULL) && (transition2 == NULL))
		return 1;
		
	if ((transition1 == NULL) || (transition2 == NULL))
		return 0;
	
	FsaTransition *fsa_transition1 = transition1->derived;
	FsaTransition *fsa_transition2 = transition2->derived;
	
	return transition_equals_private(transition1, transition2) && !strcmp(fsa_transition1->symbol, fsa_transition2->symbol);
}

guint fsa_transition_hash_code_private(Transition *transition)
{
	FsaTransition *fsa_transition = transition->derived;
	return transition_hash_code_private(transition) ^ strcmp(fsa_transition->symbol, "");
}

Transition *fsa_transition_clone_private(Transition *transition)
{	
	if (transition == NULL)
		return NULL;
	
	State *from_state = transition->from_state;
	State *to_state = transition->to_state;
	const char *symbol = ((FsaTransition *)transition->derived)->symbol;
	Transition *clone = fsa_transition_new(from_state, to_state, symbol);
	
	return clone;
}

static double bezier_x(double t, double x0, double x1, double x2, double x3)
{
	return (1-t)*(1-t)*(1-t) * x0 + 3*(1-t)*(1-t) * t * x1 + 3*(1-t) * t*t * x2 + t*t*t * x3;
}

static double bezier_y(double t, double y0, double y1, double y2, double y3)
{
	return (1-t)*(1-t)*(1-t) * y0 + 3*(1-t)*(1-t) * t * y1 + 3*(1-t) * t*t * y2 + t*t*t * y3;
}

void fsa_transition_draw_private(Transition *transition, float zoom_multiplier, GdkRGBA *primary, GdkRGBA *secondary, cairo_t *cr, int arrow_style, int group_index)
{
	State *from = transition_get_from_state(transition);
	State *to = transition_get_to_state(transition);
	const char *symbol = fsa_transition_get_symbol(transition);
	if (!strcmp(symbol, "")) {
		symbol = "\u03B5";
	}
	
	double x1 = state_get_x(from) * zoom_multiplier;
	double y1 = state_get_y(from) * zoom_multiplier;
	double x2 = state_get_x(to) * zoom_multiplier;
	double y2 = state_get_y(to) * zoom_multiplier;
	
	//START LOOP TEST
	gboolean is_transition_loop = arrow_style == TRANSITION_ARROW_LOOP;
	gboolean are_coordinates_the_same = (x1 == x2) && (y1 == y2);
	double angle1_on_circle = (-125.0 * G_PI) / 180.0;
	double angle2_on_circle = (-55.0 * G_PI) / 180.0;
	if (are_coordinates_the_same && !is_transition_loop) {
		if (state_get_id(from) < state_get_id(to)) {
			angle1_on_circle = (-180.0 * G_PI) / 180.0;
			angle2_on_circle = (0.0 * G_PI) / 180.0;
		} else {
			angle1_on_circle = (0.0 * G_PI) / 180.0;
			angle2_on_circle = (-180.0 * G_PI) / 180.0;
		}
	}
	if (is_transition_loop || are_coordinates_the_same) {
		x1 = x1 + (STATE_RADIUS * zoom_multiplier) * cos(angle1_on_circle);
		y1 = y1 + (STATE_RADIUS * zoom_multiplier) * sin(angle1_on_circle);
		x2 = x2 + (STATE_RADIUS * zoom_multiplier) * cos(angle2_on_circle);
		y2 = y2 + (STATE_RADIUS * zoom_multiplier) * sin(angle2_on_circle);
	}
	//END LOOP TEST
	
	// Calcola il punto medio
	double mid_x = (x1 + x2) / 2;
    double mid_y = (y1 + y2) / 2;
	// Distanza tra i due punti
	double dx = x2 - x1;
    double dy = y2 - y1;
	// Lunghezza della linea
	double length = sqrt(dx * dx + dy * dy);
	double u1 = dx / length;
	double u2 = dy / length;
	
	double xt_start = is_transition_loop ? x1 : x1 + (STATE_RADIUS * zoom_multiplier) * u1;
	double yt_start = is_transition_loop ? y1 : y1 + (STATE_RADIUS * zoom_multiplier) * u2;
	double xt_end = is_transition_loop ? x2 : x2 - (STATE_RADIUS * zoom_multiplier) * u1;
	double yt_end = is_transition_loop ? y2 : y2 - (STATE_RADIUS * zoom_multiplier) * u2;
	
	// Definisci un fattore di inarcamento
	double arc_factor;
	switch(arrow_style) {
		case TRANSITION_ARROW_CURVED:	arc_factor = TRANSITION_ARROW_ARC * zoom_multiplier;
										break;
		case TRANSITION_ARROW_LOOP:		arc_factor = TRANSITION_ARROW_ARC_LOOP * zoom_multiplier;
										break;
		default: 						arc_factor = 0; // STRAIGHT TRANSITION 
										break;										
	}
	// Calcola il punto di controllo basato sulla direzione, con il verso invertito
	double control_x = mid_x + arc_factor * u2;
    double control_y = mid_y - arc_factor * u1;
	// Disegna la curva Bezier
	gdk_cairo_set_source_rgba(cr, primary);
	
	if (group_index == 0) {
		cairo_set_line_width(cr, STATE_LINE_WIDTH * zoom_multiplier);
		cairo_move_to(cr, xt_start, yt_start);
		cairo_curve_to(cr, control_x, control_y, control_x, control_y, xt_end, yt_end);
		cairo_stroke(cr);
	}
	
	// Calcolo della tangente inversa alla curva Bézier nel punto finale (xt_end, yt_end)
	double tangent_x = -3 * (control_x - xt_end);
    double tangent_y = -3 * (control_y - yt_end);
    double tangent_length = sqrt(tangent_x * tangent_x + tangent_y * tangent_y);
	tangent_x /= tangent_length;
    tangent_y /= tangent_length;
    double arrow_length = TRANSITION_ARROW_LENGTH * zoom_multiplier;
	double arrow_angle = M_PI / 6; 
    double arrow_base_x = xt_end - arrow_length * tangent_x;
    double arrow_base_y = yt_end - arrow_length * tangent_y;
    double arrow_x1 = arrow_base_x + arrow_length * tangent_y * sin(arrow_angle);
	double arrow_y1 = arrow_base_y - arrow_length * tangent_x * sin(arrow_angle);
    double arrow_x2 = arrow_base_x - arrow_length * tangent_y * sin(arrow_angle);
    double arrow_y2 = arrow_base_y + arrow_length * tangent_x * sin(arrow_angle);
    
	if (group_index == 0) {
		cairo_new_path(cr);
		cairo_move_to(cr, xt_end, yt_end);
		cairo_line_to(cr, arrow_x1, arrow_y1);
		cairo_line_to(cr, arrow_x2, arrow_y2);
		cairo_close_path(cr);
		cairo_fill(cr);
	}

	// SYMBOL DRAWING
	cairo_text_extents_t te;
	cairo_font_extents_t fe;
	cairo_set_font_size(cr, STATE_FONT_SIZE * zoom_multiplier);
	cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_text_extents(cr, symbol, &te);
	cairo_font_extents(cr, &fe);

	// Compute the angle of the line
	double angle = atan2(y2 - y1, x2 - x1);

	// Compute perpendicular offset
	double offset = FSA_TRANSITION_SYMBOL_OFFSET * zoom_multiplier; // Distance above the line
	double offset_y = offset + (fe.height * group_index);
	double curve_center_x = bezier_x(0.5, xt_start, control_x, control_x, xt_end);
	double curve_center_y = bezier_y(0.5, yt_start, control_y, control_y, yt_end);
	cairo_save(cr); // Save the current state of the cairo context
	// Move to the correct position but do not rotate
	cairo_translate(cr, curve_center_x, curve_center_y);
	cairo_move_to(cr, 0, 0);
	cairo_rotate(cr, angle);
	// Center the text horizontally along the x-axis
	cairo_move_to(cr, -te.width / 2.0, -offset_y);
	cairo_get_current_point(cr, &transition->x1, &transition->y1);
	transition->x2 = transition->x1 + te.width;
	transition->y2 = transition->y1;
	transition->x3 = transition->x2;
	transition->y3 = transition->y2 - fe.height;
	transition->x4 = transition->x1;
	transition->y4 = transition->y3;
	cairo_user_to_device(cr, &transition->x1, &transition->y1);
	cairo_user_to_device(cr, &transition->x2, &transition->y2);
	cairo_user_to_device(cr, &transition->x3, &transition->y3);
	cairo_user_to_device(cr, &transition->x4, &transition->y4);
	// Show the text without rotating it
	cairo_show_text(cr, symbol);
	cairo_restore(cr); // Restore the cairo context to its previous state
}

// Function to calculate the cross product of two vectors
static double cross_product(double x1, double y1, double x2, double y2, double x3, double y3) {
    return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
}

// Function to check if a point is inside a triangle
static gboolean is_point_in_triangle(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3) {
    double d1, d2, d3;
    bool has_neg, has_pos;

    d1 = cross_product(x, y, x1, y1, x2, y2);
    d2 = cross_product(x, y, x2, y2, x3, y3);
    d3 = cross_product(x, y, x3, y3, x1, y1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

// Function to check if a point is inside a quadrilateral
static gboolean is_point_in_quadrilateral(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    // Break the quadrilateral into two triangles: (v1, v2, v3) and (v1, v3, v4)
    return is_point_in_triangle(x, y, x1, y1, x2, y2, x3, y3) || is_point_in_triangle(x, y, x1, y1, x3, y3, x4, y4);
}

gboolean fsa_transition_contains_point_private(Transition *transition, double x, double y)
{
	return is_point_in_quadrilateral(x, y, transition->x1, transition->y1, transition->x2, transition->y2, transition->x3, transition->y3, transition->x4, transition->y4);
}

struct transition_info {
	Transition *transition;
	GuiAutomatonPage *automaton_page;
	GtkEntry *entry;
};

static void callback_popover_close(GtkPopover *self, gpointer user_data)
{
	gtk_widget_unparent(GTK_WIDGET(self));
	gui_automaton_page_update_alphabet(((struct transition_info *)user_data)->automaton_page);
	free(user_data);
}

static void save_button_callback(GtkButton *self, gpointer user_data)
{
	struct transition_info *data = (struct transition_info *)user_data;
	Transition *transition = data->transition;
	GuiAutomatonPage *automaton_page = data->automaton_page;
	GtkEntry *entry = data->entry;
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	State *from = transition_get_from_state(transition);
	State *to = transition_get_to_state(transition);
	
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
	const char *new_symbol = gtk_entry_buffer_get_text(buffer);
	
	fsa_change_transition_symbol(automaton, transition, new_symbol);
	
	GtkWidget *popover = gtk_widget_get_ancestor(GTK_WIDGET(self), GTK_TYPE_POPOVER);
	gtk_popover_popdown(GTK_POPOVER(popover));
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
	gui_automaton_page_update_transitions_number(automaton_page);
}

static void delete_button_callback(GtkButton *self, gpointer user_data)
{
	struct transition_info *data = (struct transition_info *)user_data;
	Transition *transition = data->transition;
	GuiAutomatonPage *automaton_page = data->automaton_page;
	GtkEntry *entry = data->entry;
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	
	automaton_remove_transition(automaton, transition);
	
	GtkWidget *popover = gtk_widget_get_ancestor(GTK_WIDGET(self), GTK_TYPE_POPOVER);
	gtk_popover_popdown(GTK_POPOVER(popover));
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
	gui_automaton_page_update_transitions_number(automaton_page);
}

GtkWidget *fsa_transition_get_popover_private(Transition *transition, gpointer automaton_page)
{
	const char *symbol = fsa_transition_get_symbol(transition);
	
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	
	GtkWidget *title = gtk_label_new_with_mnemonic("Edit transition");
	
	GtkWidget *entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	
	GtkWidget *symbol_entry = gtk_entry_new();
	GtkEntryBuffer *symbol_buffer = gtk_entry_get_buffer(GTK_ENTRY(symbol_entry));
	gtk_entry_buffer_set_text(symbol_buffer, symbol, strlen(symbol));
	
	struct transition_info *data = malloc(sizeof(struct transition_info));
	data->transition = transition;
	data->automaton_page = automaton_page;
	data->entry = GTK_ENTRY(symbol_entry);
	
	GtkWidget *save_button = gtk_button_new_with_label("Save");
	g_signal_connect(save_button, "clicked", G_CALLBACK(save_button_callback), data);
	gtk_box_append(GTK_BOX(entry_box), symbol_entry);
	gtk_box_append(GTK_BOX(entry_box), save_button);
	
	GtkWidget *delete_button = gtk_button_new_with_label("Delete");
	g_signal_connect(delete_button, "clicked", G_CALLBACK(delete_button_callback), data);
	
	gtk_box_append(GTK_BOX(box), title);
	gtk_box_append(GTK_BOX(box), entry_box);
	gtk_box_append(GTK_BOX(box), delete_button);
	
	GtkWidget *popover = gtk_popover_new();
	gtk_popover_set_child(GTK_POPOVER(popover), box);
		
	g_signal_connect(popover, "closed", G_CALLBACK(callback_popover_close), data);
	
	return popover;
}