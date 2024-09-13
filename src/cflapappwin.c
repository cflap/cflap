#include <gtk/gtk.h>

#include "cflapapp.h"
#include "cflapappwin.h"
#include "tab.h"
#include "automatonpage.h"

struct _CFlapAppWindow {
	GtkApplicationWindow parent;
	
	GtkNotebook *notebook;
	GtkWidget *gears;
};

G_DEFINE_TYPE(CFlapAppWindow, cflap_app_window, GTK_TYPE_APPLICATION_WINDOW);

static void cflap_app_window_init(CFlapAppWindow *win)
{
	gtk_widget_init_template(GTK_WIDGET(win));	
	
	// Gets the menu from the ui file
	GtkBuilder *builder;
	GMenuModel *menu;
	builder = gtk_builder_new_from_resource("/com/cflap/templates/main_menu.ui");
	menu = G_MENU_MODEL(gtk_builder_get_object(builder, "main_menu"));
	gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(win->gears), menu);
	g_object_unref(builder);
	
	// Add a welcome tab to the notebook;
	GtkBox *welcome_content = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
	// TODO Inflate a ui file for the welcome_content
	gtk_box_append(welcome_content, GTK_WIDGET(gtk_label_new("Welcome")));
	gtk_notebook_append_page(win->notebook, GTK_WIDGET(welcome_content), GTK_WIDGET(gui_tab_new("Welcome", GTK_WIDGET(welcome_content))));
}

static void cflap_app_window_class_init(CFlapAppWindowClass *class)
{
	g_type_ensure(GUI_TAB_TYPE);
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/com/cflap/templates/window_template.ui");
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CFlapAppWindow, notebook);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CFlapAppWindow, gears);
}

CFlapAppWindow *cflap_app_window_new(CFlapApp *app)
{
	return g_object_new(CFLAP_APP_WINDOW_TYPE, "application", app, NULL);
}

void cflap_app_window_open(CFlapAppWindow *win, GFile *file)
{
	
}

void cflap_app_window_create_automaton_tab(CFlapAppWindow *window, NewAutomatonDialogData *data)
{
	GtkNotebook *notebook = window->notebook;
	char *tab_name = data->new_tab_name;
	
	Automaton *automaton = fsa_new();
	
	GuiAutomatonPage *content = gui_automaton_page_new(automaton, data->automaton_type);
	gtk_notebook_append_page(window->notebook, GTK_WIDGET(content), 
							GTK_WIDGET(gui_tab_new(tab_name, GTK_WIDGET(content))));
	gtk_notebook_set_current_page(window->notebook, -1);
}