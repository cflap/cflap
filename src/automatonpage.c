#include <gtk/gtk.h>

#include "automatonpage.h"
#include "automata/state.h"
#include "drawingareacallbacks.h"
#include "automata/fsa_alphabet_retriever.h"
#include "automata/fsa_simulator.h"
#include "automata/fsa_checker.h"
#include "automata/fsa_nfa_to_dfa.h"
#include "automata/fsa_minimizer.h"

struct _GuiAutomatonPage {
	GtkGrid parent;
	
	Automaton *automaton;
	int automaton_type;
	AutomatonDrawer drawer;
	AlphabetRetriever *alphabet_retriever;
	AutomatonSimulator *simulator;
	
	GtkOverlay *overlay_container;
	
	// Toggle elements
	GtkBox *toggle_container;
	GtkToggleButton *toggle_select;
	GtkToggleButton *toggle_state;
	GtkToggleButton *toggle_transition;
	
	GtkBox *right_menu;
	// Right menu elements
	GtkLabel *automaton_type_label;
	GtkLabel *states_number_label;
	GtkLabel *transitions_number_label;
	GtkLabel *initial_state_name;
	GtkText *alphabet_label;
	GtkEntry *simulator_entry;
	GtkLabel *simulation_result_label;
	
	GtkScrolledWindow *scrolled_window;
	GtkDrawingArea *drawing_area;
	
	// gesture
	GtkGesture *select_click;
	GtkGesture *select_drag;
	GtkGesture *state_drag;
	GtkGesture *transition_drag;
	
	GtkScale *zoom_controller;
};

G_DEFINE_TYPE(GuiAutomatonPage, gui_automaton_page, GTK_TYPE_GRID);

static void remove_all_controllers(GuiAutomatonPage *self)
{
	// select click pressed
	g_signal_handlers_disconnect_by_func(self->select_click, G_CALLBACK(callback_select_click_pressed), self);
	// select drag 
	g_signal_handlers_disconnect_by_func(self->select_drag, G_CALLBACK(callback_select_drag_begin), self);
	g_signal_handlers_disconnect_by_func(self->select_drag, G_CALLBACK(callback_select_drag_update), self);
	g_signal_handlers_disconnect_by_func(self->select_drag, G_CALLBACK(callback_select_drag_end), self);
	// state drag
	g_signal_handlers_disconnect_by_func(self->state_drag, G_CALLBACK(callback_state_drag_begin), self);
	g_signal_handlers_disconnect_by_func(self->state_drag, G_CALLBACK(callback_state_drag_update), self);
	g_signal_handlers_disconnect_by_func(self->state_drag, G_CALLBACK(callback_state_drag_end), self);
	// transition drag
	g_signal_handlers_disconnect_by_func(self->transition_drag, G_CALLBACK(callback_transition_drag_begin), self);
	g_signal_handlers_disconnect_by_func(self->transition_drag, G_CALLBACK(callback_transition_drag_update), self);
	g_signal_handlers_disconnect_by_func(self->transition_drag, G_CALLBACK(callback_transition_drag_end), self);
}

static void set_select_callback(GtkButton *self, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(gtk_widget_get_ancestor(GTK_WIDGET(self), GUI_AUTOMATON_PAGE_TYPE));
	GtkDrawingArea *drawing_area = automaton_page->drawing_area;
	// Remove other gtkdrawingarea callbacks here
	remove_all_controllers(automaton_page);

	g_signal_connect(automaton_page->select_drag, "drag-begin", G_CALLBACK(callback_select_drag_begin), automaton_page);
	g_signal_connect(automaton_page->select_drag, "drag-update", G_CALLBACK(callback_select_drag_update), automaton_page);
	g_signal_connect(automaton_page->select_drag, "drag-end", G_CALLBACK(callback_select_drag_end), automaton_page);
	
	g_signal_connect(automaton_page->select_click, "pressed", G_CALLBACK(callback_select_click_pressed), automaton_page);
}

static void set_state_callback(GtkButton *self, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(gtk_widget_get_ancestor(GTK_WIDGET(self), GUI_AUTOMATON_PAGE_TYPE));
	GtkDrawingArea *drawing_area = automaton_page->drawing_area;
	// Remove other drawing_area's callbacks
	remove_all_controllers(automaton_page);
	
	g_signal_connect(automaton_page->state_drag, "drag-begin", G_CALLBACK(callback_state_drag_begin), automaton_page);
	g_signal_connect(automaton_page->state_drag, "drag-update", G_CALLBACK(callback_state_drag_update), automaton_page);
	g_signal_connect(automaton_page->state_drag, "drag-end", G_CALLBACK(callback_state_drag_end), automaton_page);
}

void set_transition_callback(GtkButton *self, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(gtk_widget_get_ancestor(GTK_WIDGET(self), GUI_AUTOMATON_PAGE_TYPE));
	GtkDrawingArea *drawing_area = automaton_page->drawing_area;
	// Remove other gtkdrawingarea callback here
	remove_all_controllers(automaton_page);
	
	g_signal_connect(automaton_page->transition_drag, "drag-begin", G_CALLBACK(callback_transition_drag_begin), automaton_page);
	g_signal_connect(automaton_page->transition_drag, "drag-update", G_CALLBACK(callback_transition_drag_update), automaton_page);
	g_signal_connect(automaton_page->transition_drag, "drag-end", G_CALLBACK(callback_transition_drag_end), automaton_page);
}

void gui_automaton_page_set_scrolledwindow_sensitive(GuiAutomatonPage *self, gboolean status)
{
	GtkWidget *scrolled = GTK_WIDGET(self->scrolled_window);
	gtk_widget_set_sensitive(GTK_WIDGET(scrolled), status);
}

void gui_automaton_page_update_states_number(GuiAutomatonPage *self)
{
	Automaton *automaton = self->automaton;
	GList *states_list = automaton_get_states_as_list(automaton);
	int state_number = g_list_length(states_list);
	char *string_to_set = g_strdup_printf("%d", state_number);
	gtk_label_set_text(self->states_number_label, string_to_set);
	free(string_to_set);
}

void gui_automaton_page_update_transitions_number(GuiAutomatonPage *self)
{
	Automaton *automaton = self->automaton;
	GList *transitions_list = automaton_get_transitions_as_list(automaton);
	int transitions_number = g_list_length(transitions_list);
	char *string_to_set = g_strdup_printf("%d", transitions_number);
	gtk_label_set_text(self->transitions_number_label, string_to_set);
	free(string_to_set);
}

void gui_automaton_page_update_initial_state(GuiAutomatonPage *self)
{
	Automaton *automaton = self->automaton;
	State *initial_state = automaton_get_initial_state(automaton);
	char *string_to_set;
	if (initial_state == NULL)
		string_to_set = "N/A";
	else
		string_to_set = strdup(state_get_name(initial_state));
	gtk_label_set_text(self->initial_state_name, string_to_set);
}

void gui_automaton_page_update_alphabet(GuiAutomatonPage *self)
{
	Automaton *automaton = self->automaton;
	AlphabetRetriever *alphabet_retriever = self->alphabet_retriever;
	GtkText *alphabet_label = self->alphabet_label;
	
	GList *alphabet = alphabet_retriever_get_alphabet(alphabet_retriever, automaton);
	int alphabet_length = g_list_length(alphabet);
	char *alphabet_string = strdup("{");
	GList *current_alphabet_node = alphabet;
	for (int i = 0; current_alphabet_node != NULL; current_alphabet_node = current_alphabet_node->next, ++i) {
		char *letter = (char *)current_alphabet_node->data;
		char *to_free = alphabet_string;
		if (i + 1 == alphabet_length) {
			alphabet_string = g_strdup_printf("%s%s", alphabet_string, letter);
		} else {
			alphabet_string = g_strdup_printf("%s%s,", alphabet_string, letter);
		}
		free(to_free);
		free(letter);
	}
	g_list_free(alphabet);
	char *to_free = alphabet_string;
	alphabet_string = g_strdup_printf("%s}", alphabet_string);
	free(to_free);
	GtkEntryBuffer *buffer = gtk_text_get_buffer(alphabet_label);
	gtk_entry_buffer_set_text(buffer, alphabet_string, strlen(alphabet_string));
	free(alphabet_string);
}

Automaton *gui_automaton_page_get_automaton(GuiAutomatonPage *self)
{
	return self->automaton;
}

AutomatonDrawer *gui_automaton_page_get_automaton_drawer(GuiAutomatonPage *self)
{
	return &(self->drawer);
}

GtkOverlay *gui_automaton_page_get_overlay_container(GuiAutomatonPage *self)
{
	return self->overlay_container;
}

GtkDrawingArea *gui_automaton_page_get_drawing_area(GuiAutomatonPage *self)
{
	return self->drawing_area;
}

static void zoom_changed(GtkRange* self, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	AutomatonDrawer *drawer = &automaton_page->drawer;
	drawer->zoom_multiplier = gtk_range_get_value(self);
	
	gtk_widget_queue_draw(GTK_WIDGET(automaton_page->drawing_area));
}

static void simulate_icon_press_callback(GtkEntry *self, GtkEntryIconPosition icon_position, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	AutomatonSimulator *simulator = automaton_page->simulator;
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(self);
	char *input = strdup(gtk_entry_buffer_get_text(buffer));
	int result = automaton_simulator_simulate_input(simulator, input);
	free(input);
	GtkLabel *result_label = automaton_page->simulation_result_label;
	char *string_to_set;
	if (result) {
		string_to_set = "Simulation result: ACCEPTED";
	} else {
		string_to_set = "Simulation result: REJECTED";
	}
	gtk_label_set_text(result_label, string_to_set);
}

static void input_inserted_callback(GtkEntryBuffer* self, guint position, gchar* chars, guint n_chars, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = user_data;
	GtkLabel *sim_result = automaton_page->simulation_result_label;
	
	gtk_label_set_text(sim_result, "Simulation result: N/A");
}

static void input_deleted_callback(GtkEntryBuffer* self, guint position, guint n_chars, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = user_data;
	GtkLabel *sim_result = automaton_page->simulation_result_label;
	
	gtk_label_set_text(sim_result, "Simulation result: N/A");
}

static void gui_automaton_page_init(GuiAutomatonPage *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
	
	// Creates toggle buttons group
	gtk_toggle_button_set_group(self->toggle_select, self->toggle_state);
	gtk_toggle_button_set_group(self->toggle_select, self->toggle_state);
	gtk_toggle_button_set_group(self->toggle_select, self->toggle_transition);
	gtk_toggle_button_set_group(self->toggle_state, self->toggle_transition);
	// Set first button as active
	gtk_toggle_button_set_active(self->toggle_select, TRUE);
	// Add toggle buttons callback to set correct drawing method
	g_signal_connect(self->toggle_select, "clicked", (GCallback) set_select_callback, NULL);
	g_signal_connect(self->toggle_state, "clicked", (GCallback) set_state_callback, NULL);
	g_signal_connect(self->toggle_transition, "clicked", (GCallback) set_transition_callback, NULL);
	
	self->select_click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(self->select_click), 3);
	gtk_gesture_single_set_exclusive(GTK_GESTURE_SINGLE(self->select_click), TRUE);
	gtk_widget_add_controller(GTK_WIDGET(self->drawing_area), GTK_EVENT_CONTROLLER(self->select_click));
	
	self->select_drag = gtk_gesture_drag_new();
	gtk_widget_add_controller(GTK_WIDGET(self->drawing_area), GTK_EVENT_CONTROLLER(self->select_drag));
	
	self->state_drag = gtk_gesture_drag_new();
	gtk_widget_add_controller(GTK_WIDGET(self->drawing_area), GTK_EVENT_CONTROLLER(self->state_drag));
	
	self->transition_drag = gtk_gesture_drag_new();
	gtk_widget_add_controller(GTK_WIDGET(self->drawing_area), GTK_EVENT_CONTROLLER(self->transition_drag));
	
	gtk_range_set_range(GTK_RANGE(self->zoom_controller), 0.1, 5.0);
	gtk_range_set_value(GTK_RANGE(self->zoom_controller), 1.0);
	g_signal_connect(self->zoom_controller, "value-changed", G_CALLBACK(zoom_changed), self);
	
	g_signal_connect(self->simulator_entry, "icon-release", G_CALLBACK(simulate_icon_press_callback), self);
	
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(self->simulator_entry);
	g_signal_connect(buffer, "inserted-text", G_CALLBACK(input_inserted_callback), self);
	g_signal_connect(buffer, "deleted-text", G_CALLBACK(input_deleted_callback), self);
}

static void gui_automaton_page_dispose(GObject *gobject)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(gobject);
	Automaton *automaton = automaton_page->automaton;
	automaton_destroy(automaton);
	AlphabetRetriever *alphabet_retriever = automaton_page->alphabet_retriever;
	alphabet_retriever_destroy(alphabet_retriever);
	AutomatonSimulator *simulator = automaton_page->simulator;
	automaton_simulator_destroy(simulator);
	
	gtk_widget_dispose_template(GTK_WIDGET(gobject), GUI_AUTOMATON_PAGE_TYPE);
	
	G_OBJECT_CLASS (gui_automaton_page_parent_class)->dispose (gobject);
}

static void gui_automaton_page_class_init(GuiAutomatonPageClass *klass)
{
	G_OBJECT_CLASS (klass)->dispose = gui_automaton_page_dispose;
	
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class,
                                               "/com/cflap/templates/tab_content.ui");
	
	// Bind overlay container
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, overlay_container);	
	// Bind toggle elements
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, toggle_container);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, toggle_select);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, toggle_state);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, toggle_transition);
	// Bind right menu elements		
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, right_menu);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, automaton_type_label);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, states_number_label);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, transitions_number_label);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, initial_state_name);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, alphabet_label);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, simulator_entry);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, simulation_result_label);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, scrolled_window);
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, drawing_area);
	
	gtk_widget_class_bind_template_child (widget_class, GuiAutomatonPage, zoom_controller);
}

static void automaton_page_draw_func(GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data)
{
	GuiAutomatonPage *parent = GUI_AUTOMATON_PAGE(gtk_widget_get_ancestor(GTK_WIDGET (drawing_area), GUI_AUTOMATON_PAGE_TYPE));
	AutomatonDrawer *drawer = &(parent->drawer);
	drawer->draw(drawer, parent->automaton, cr);
	
	cairo_new_path(cr);
	GdkRGBA red;
	red.red = 1.0;
	red.green = 0.0;
	red.blue = 0.0;
	red.alpha = 0.5;
	gdk_cairo_set_source_rgba(cr, &red);
	cairo_set_line_width(cr, STATE_LINE_WIDTH * drawer->zoom_multiplier);
	cairo_move_to(cr, drawer->tline_start_x * drawer->zoom_multiplier, drawer->tline_start_y * drawer->zoom_multiplier);
	cairo_line_to(cr, drawer->tline_end_x * drawer->zoom_multiplier, drawer->tline_end_y * drawer->zoom_multiplier);
	cairo_stroke(cr);
}

static void convert_nfa_to_dfa_callback(GtkButton *self, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	
	Automaton *old = automaton_page->automaton;
	if (automaton_get_initial_state(old) == NULL) {
		GtkWindow *parent = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(automaton_page), GTK_TYPE_WINDOW));
		GtkWidget *warning_dialog = gtk_window_new();
		GtkWidget *warning_label = gtk_label_new( "No initial state found");
		gtk_window_set_child(GTK_WINDOW(warning_dialog), warning_label);
		gtk_window_set_default_size(GTK_WINDOW(warning_dialog), 200, 200);
		gtk_window_set_resizable(GTK_WINDOW(warning_dialog), FALSE);
		gtk_window_set_transient_for(GTK_WINDOW(warning_dialog), parent);
		// Block interactions with parent window
		gtk_window_set_modal(GTK_WINDOW(warning_dialog), TRUE);
		gtk_window_present(GTK_WINDOW(warning_dialog));
		return;
	}
	
	Automaton *new = ntd_convert_to_dfa(old);
	
	if (new == NULL)
		return;
	
	automaton_page->automaton = new;
	automaton_simulator_destroy(automaton_page->simulator);
	automaton_page->simulator = fsa_simulator_new(new);
	automaton_destroy(old);
	
	gui_automaton_page_update_states_number(automaton_page);
	gui_automaton_page_update_transitions_number(automaton_page);
	gui_automaton_page_update_initial_state(automaton_page);
	gui_automaton_page_update_alphabet(automaton_page);
	
	g_signal_emit_by_name(automaton_page->toggle_select, "clicked", NULL);
	gtk_widget_queue_draw(GTK_WIDGET(automaton_page->drawing_area));
}

static void minimize_dfa_callback(GtkButton *self, gpointer user_data)
{
	GuiAutomatonPage *automaton_page = GUI_AUTOMATON_PAGE(user_data);
	
	Automaton *old = automaton_page->automaton;
	if (automaton_get_initial_state(old) == NULL) {
		GtkWindow *parent = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(automaton_page), GTK_TYPE_WINDOW));
		GtkWidget *warning_dialog = gtk_window_new();
		GtkWidget *warning_label = gtk_label_new( "No initial state found");
		gtk_window_set_child(GTK_WINDOW(warning_dialog), warning_label);
		gtk_window_set_default_size(GTK_WINDOW(warning_dialog), 200, 200);
		gtk_window_set_resizable(GTK_WINDOW(warning_dialog), FALSE);
		gtk_window_set_transient_for(GTK_WINDOW(warning_dialog), parent);
		// Block interactions with parent window
		gtk_window_set_modal(GTK_WINDOW(warning_dialog), TRUE);
		gtk_window_present(GTK_WINDOW(warning_dialog));
		return;
	}
	
	if (fsa_is_nfa(old)) {
		GtkWindow *parent = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(automaton_page), GTK_TYPE_WINDOW));
		GtkWidget *warning_dialog = gtk_window_new();
		GtkWidget *warning_label = gtk_label_new( "Can't minimize a NFA");
		gtk_window_set_child(GTK_WINDOW(warning_dialog), warning_label);
		gtk_window_set_default_size(GTK_WINDOW(warning_dialog), 200, 200);
		gtk_window_set_resizable(GTK_WINDOW(warning_dialog), FALSE);
		gtk_window_set_transient_for(GTK_WINDOW(warning_dialog), parent);
		// Block interactions with parent window
		gtk_window_set_modal(GTK_WINDOW(warning_dialog), TRUE);
		gtk_window_present(GTK_WINDOW(warning_dialog));
		return;
	}
	
	Automaton *new = fsa_min_minimize(old);
	
	if (new == NULL)
		return;
	
	automaton_page->automaton = new;
	automaton_simulator_destroy(automaton_page->simulator);
	automaton_page->simulator = fsa_simulator_new(new);
	automaton_destroy(old);
	
	gui_automaton_page_update_states_number(automaton_page);
	gui_automaton_page_update_transitions_number(automaton_page);
	gui_automaton_page_update_initial_state(automaton_page);
	gui_automaton_page_update_alphabet(automaton_page);
	
	g_signal_emit_by_name(automaton_page->toggle_select, "clicked", NULL);
	gtk_widget_queue_draw(GTK_WIDGET(automaton_page->drawing_area));
}

static void load_fsa_menu(GuiAutomatonPage *self)
{
	GtkBuilder *builder = gtk_builder_new_from_resource("/com/cflap/templates/fsa_menu.ui");
	GtkWidget *fsa_menu = GTK_WIDGET(gtk_builder_get_object(builder, "fsa_menu"));
	
	
	GtkWidget *nfa_to_dfa_button = GTK_WIDGET(gtk_builder_get_object(builder, "nfa_to_dfa_button"));
	g_signal_connect(nfa_to_dfa_button, "clicked", G_CALLBACK(convert_nfa_to_dfa_callback), self);
	
	GtkWidget *minimize_dfa_button = GTK_WIDGET(gtk_builder_get_object(builder, "minimize_dfa_button"));
	g_signal_connect(minimize_dfa_button, "clicked", G_CALLBACK(minimize_dfa_callback), self);
	
	gtk_box_append(self->right_menu, fsa_menu);
	
	g_object_unref(builder);
}

GuiAutomatonPage *gui_automaton_page_new(Automaton *automaton, int automaton_type)
{
	GuiAutomatonPage *self = g_object_new(GUI_AUTOMATON_PAGE_TYPE, NULL);
	
	self->automaton = automaton;
	self->automaton_type = automaton_type;
	
	GdkRGBA primary;
	GdkRGBA secondary;
	primary.red = 0.23;
	primary.green = 0.52;
	primary.blue = 1.0;
	primary.alpha = 1.0;
	secondary.red = 0.67;
	secondary.green = 0.79;
	secondary.blue = 0.98;
	secondary.alpha = 1.0;
	
	char *automaton_type_string;
	switch (automaton_type) {
		case AUTOMATON_FSA_TYPE: automaton_type_string = strdup("Finite State Automaton");
								automaton_drawer_init(&(self->drawer), 1.0, &primary, &secondary, &automaton_drawer_draw);
								self->alphabet_retriever = fsa_alphabet_retriever_new();
								self->simulator = fsa_simulator_new(automaton);
								load_fsa_menu(self);
								break;
		case AUTOMATON_TM_TYPE: automaton_type_string = strdup("Turing Machine");
								break;
		default: break;
	}
	// Sets the automaton type string
	gtk_label_set_text(self->automaton_type_label, automaton_type_string);
	free(automaton_type_string);
	// Sets draw function of the AutomatonDrawer
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(self->drawing_area), automaton_page_draw_func, NULL, NULL);
	
	gui_automaton_page_update_states_number(self);
	gui_automaton_page_update_transitions_number(self);
	gui_automaton_page_update_initial_state(self);
	gui_automaton_page_update_alphabet(self);
	
	return self;
}