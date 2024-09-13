#include "new_automaton_dialog.h"
#include "cflapappwin.h"

typedef struct window_elements {
	// Parent window (the main window)
	GtkWindow *parent;
	GtkEntry *input_name;
	GtkDropDown *drop_down;
} WindowElements;

static void close_dialog(GtkButton *self, gpointer user_data)
{
	free(user_data);
	GtkWindow *dialog = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(self), GTK_TYPE_WINDOW));
	gtk_window_destroy(dialog);
}

static void create_new_automaton(GtkButton *self, gpointer user_data)
{
	WindowElements *elements = (WindowElements *) user_data;
	NewAutomatonDialogData *data = g_malloc(sizeof(NewAutomatonDialogData));
	// Fills data
	GtkEntryBuffer *name_buffer = gtk_entry_get_buffer(elements->input_name);
	const char *name = gtk_entry_buffer_get_text(name_buffer);
	if (strlen(name) <= 0) {
		name = "Automaton";
	}
	data->new_tab_name = strdup(name);
	
	data->automaton_type = gtk_drop_down_get_selected(elements->drop_down);
	
	cflap_app_window_create_automaton_tab(CFLAP_APP_WINDOW(elements->parent), data);
	
	// free data
	free(data->new_tab_name);
	free(data);
	
	close_dialog(self, elements);
}

GtkWindow *new_automaton_dialog_new(GtkWindow *parent)
{	
	GtkBuilder *builder = gtk_builder_new_from_resource("/com/cflap/templates/new_automaton_dialog.ui");
	GtkWindow *dialog = GTK_WINDOW(gtk_builder_get_object(builder, "dialog_window"));
	
	gtk_window_set_transient_for(dialog, parent);
	// Block interactions with parent window
	gtk_window_set_modal(dialog, TRUE);
	
	WindowElements *elements = g_malloc(sizeof(WindowElements));
	elements->parent = parent;
	elements->input_name = GTK_ENTRY(gtk_builder_get_object(builder, "new_automaton_dialog_name_input"));
	elements->drop_down = GTK_DROP_DOWN(gtk_builder_get_object(builder, "new_automaton_dialog_drop_down"));
	
	// Add listeners
	g_signal_connect(dialog, "close-request", G_CALLBACK(close_dialog), elements);
	GtkButton *ok_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_automaton_dialog_ok_button"));
	g_signal_connect(ok_button, "clicked", G_CALLBACK(create_new_automaton), elements);
	GtkButton *cancel_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_automaton_dialog_cancel_button"));
	g_signal_connect(cancel_button, "clicked", G_CALLBACK(close_dialog), elements);
	
	g_object_unref(builder);
	
	return dialog;
}