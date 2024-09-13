#include "drawingareacallbacks.h"
#include "automata/automaton.h"
#include "automatonpage.h"
#include "automaton_drawer.h"
#include "automata/fsa_transition.h"

static State *selected_state = NULL;

// select
void callback_select_click_pressed(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	guint btn = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
	
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	double zoom_multiplier = drawer->zoom_multiplier;
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	GtkWidget *drawing_area = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self));
	
	State *state = automaton_drawer_get_state_at(drawer, automaton, x / zoom_multiplier, y / zoom_multiplier);
	if (state != NULL) {
		GtkWidget *popover = state_get_popover(state, automaton_page);
		GdkRectangle rect = {x, y, 0, 0};
		gtk_popover_set_pointing_to(GTK_POPOVER(popover), &rect);
		
		gtk_widget_set_parent(popover, drawing_area);
		gtk_popover_popup(GTK_POPOVER(popover));
		return;
	}
	
	Transition *transition = automaton_drawer_get_transition_at(drawer, automaton, x / zoom_multiplier, y / zoom_multiplier);
	if (transition != NULL) {
		GtkWidget *popover = transition_get_popover(transition, automaton_page);
		GdkRectangle rect = {x, y, 0, 0};
		gtk_popover_set_pointing_to(GTK_POPOVER(popover), &rect);
		
		gtk_widget_set_parent(popover, drawing_area);
		gtk_popover_popup(GTK_POPOVER(popover));
	}
}

void callback_select_drag_begin(GtkGestureDrag *self, gdouble start_x, gdouble start_y, gpointer user_data)
{
	GtkDrawingArea *drawing_area = GTK_DRAWING_AREA(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	selected_state = automaton_drawer_get_state_at(drawer, automaton, start_x / drawer->zoom_multiplier, start_y / drawer->zoom_multiplier);
}

void callback_select_drag_update(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data)
{
	if (selected_state == NULL)
		return;
	GtkDrawingArea *drawing_area = GTK_DRAWING_AREA(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	
	double start_x = 0;
	double start_y = 0;
	gtk_gesture_drag_get_start_point(self, &start_x, &start_y);
	
	if ((start_x + offset_x) / drawer->zoom_multiplier >= STATE_RADIUS) {
		state_set_x(selected_state, (start_x + offset_x) / drawer->zoom_multiplier);
	}
	
	if ((start_y + offset_y) / drawer->zoom_multiplier >= STATE_RADIUS) {
		state_set_y(selected_state, (start_y + offset_y) / drawer->zoom_multiplier);
	}
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}

void callback_select_drag_end(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data)
{
	selected_state = NULL;
	
}

// state
void callback_state_drag_begin(GtkGestureDrag *self, gdouble start_x, gdouble start_y, gpointer user_data)
{
	GtkDrawingArea *drawing_area = GTK_DRAWING_AREA(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	
	if (start_x < STATE_RADIUS * drawer->zoom_multiplier) {
		start_x = STATE_RADIUS * drawer->zoom_multiplier;
	}
	
	if (start_y < STATE_RADIUS * drawer->zoom_multiplier) {
		start_y = STATE_RADIUS * drawer->zoom_multiplier;
	}
	
	automaton_create_state(automaton, start_x / drawer->zoom_multiplier, start_y / drawer->zoom_multiplier);
	gui_automaton_page_update_states_number(automaton_page);
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}

void callback_state_drag_update(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data)
{
	GtkDrawingArea *drawing_area = GTK_DRAWING_AREA(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	GList *states = automaton_get_states_as_list(automaton);
	State *current_state = (State *) g_list_nth_data(states, g_list_length(states) - 1);
	double start_x = 0;
	double start_y = 0;
	gtk_gesture_drag_get_start_point(self, &start_x, &start_y);
	
	if ((start_x + offset_x) / drawer->zoom_multiplier >= STATE_RADIUS) {
		state_set_x(current_state, (start_x + offset_x) / drawer->zoom_multiplier);
	}
	
	if ((start_y + offset_y) / drawer->zoom_multiplier >= STATE_RADIUS) {
		state_set_y(current_state, (start_y + offset_y) / drawer->zoom_multiplier);
	}
	g_list_free(states);
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}

void callback_state_drag_end(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data)
{
	;
}

// transition
void callback_transition_drag_begin(GtkGestureDrag *self, gdouble start_x, gdouble start_y, gpointer user_data)
{
	GtkDrawingArea *drawing_area = GTK_DRAWING_AREA(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	
	drawer->tline_start_x = start_x / drawer->zoom_multiplier;
	drawer->tline_start_y = start_y / drawer->zoom_multiplier;
	drawer->tline_end_x = start_x / drawer->zoom_multiplier;
	drawer->tline_end_y = start_y / drawer->zoom_multiplier;
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}

void callback_transition_drag_update(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data)
{
	GtkDrawingArea *drawing_area = GTK_DRAWING_AREA(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	
	double start_x = 0;
	double start_y = 0;
	gtk_gesture_drag_get_start_point(self, &start_x, &start_y);
	drawer->tline_end_x = (start_x + offset_x) / drawer->zoom_multiplier;
	drawer->tline_end_y = (start_y + offset_y) / drawer->zoom_multiplier;
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}

struct overlay_and_transition_input {
		GtkOverlay *overlay;
		GtkWidget *transition_input;
		GuiAutomatonPage *automaton_page;
};

static void abort_transition_input(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	struct overlay_and_transition_input *data = (struct overlay_and_transition_input *) user_data;
	GtkWidget *fixed = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self));
	GtkOverlay *overlay_container = data->overlay;
	GtkWidget *transition_input = data->transition_input;

	
	gtk_overlay_remove_overlay(GTK_OVERLAY(overlay_container), fixed);
	gtk_fixed_remove(GTK_FIXED(fixed), GTK_WIDGET(transition_input));
	gtk_widget_unparent(GTK_WIDGET(fixed));
	
	gui_automaton_page_set_scrolledwindow_sensitive(data->automaton_page, TRUE);
	
	free(data);
}

void callback_transition_drag_end(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data)
{
	GtkDrawingArea *drawing_area = GTK_DRAWING_AREA(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	AutomatonDrawer *drawer = gui_automaton_page_get_automaton_drawer(automaton_page);
	double zoom_multiplier = drawer->zoom_multiplier;
	GtkOverlay *overlay_container = gui_automaton_page_get_overlay_container(automaton_page);
	Automaton *automaton = gui_automaton_page_get_automaton(automaton_page);
	State *from = automaton_drawer_get_state_at(drawer, automaton, drawer->tline_start_x, drawer->tline_start_y);
	State *to = automaton_drawer_get_state_at(drawer, automaton, drawer->tline_end_x, drawer->tline_end_y);
	
	if (from != NULL && to != NULL) {
		GtkWidget *transition_input = automaton_create_transition_input(automaton, from, to, automaton_page);
		
		GtkWidget *fixed = gtk_fixed_new();
		gtk_widget_set_halign(fixed, GTK_ALIGN_FILL);
		gtk_widget_set_valign(fixed, GTK_ALIGN_FILL);
		double mid_x = ((drawer->tline_end_x + drawer->tline_start_x) / 2) * zoom_multiplier;
		double mid_y = ((drawer->tline_end_y + drawer->tline_start_y) / 2) * zoom_multiplier;
		int input_height = 0;
		int input_width = 0;
		gtk_widget_get_size_request(transition_input, &input_width, &input_height);
		gtk_fixed_put(GTK_FIXED(fixed), transition_input, (mid_x - input_width / 2), (mid_y - input_height / 2));
		
		gtk_overlay_add_overlay(overlay_container, GTK_WIDGET(fixed));
		gtk_overlay_set_clip_overlay(overlay_container, fixed, TRUE);
		
		GtkGesture *click = gtk_gesture_click_new();
		struct overlay_and_transition_input *data = malloc(sizeof(struct overlay_and_transition_input));
		data->overlay = overlay_container;
		data->automaton_page = automaton_page;
		data->transition_input = transition_input;
	
		g_signal_connect(click, "pressed", G_CALLBACK(abort_transition_input), data);
		gtk_widget_add_controller(GTK_WIDGET(fixed), GTK_EVENT_CONTROLLER(click));
		
		gtk_widget_child_focus(fixed, GTK_DIR_TAB_FORWARD);
		// Set scrolledwindow not clickable
		gui_automaton_page_set_scrolledwindow_sensitive(automaton_page, FALSE);
	}
	
	drawer->tline_start_x = 0;
	drawer->tline_start_y = 0;
	drawer->tline_end_x = 0;
	drawer->tline_end_y = 0;
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}