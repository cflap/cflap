#include <gtk/gtk.h>

#include "tab.h"

struct _GuiTab {
	GtkBox parent;
	
	GtkWidget *page_content;
	// Gui elements
	GtkLabel *label;
	GtkButton *close_button;
};

G_DEFINE_TYPE(GuiTab, gui_tab, GTK_TYPE_BOX);

// Callback of close button in a tab
static void close_tab(GtkButton *button, gpointer user_data)
{
	// Gets the tab the clicked button belongs to
	GuiTab *tab = GUI_TAB(gtk_widget_get_parent(GTK_WIDGET(button)));
	// Gets the notebook the tab belongs to
	GtkNotebook *notebook = GTK_NOTEBOOK(gtk_widget_get_ancestor(GTK_WIDGET(tab), GTK_TYPE_NOTEBOOK));
	// Gets the index of the tab in the notebook
	int tab_index = gtk_notebook_page_num(notebook, tab->page_content);
	gtk_notebook_remove_page(notebook, tab_index);
}

static void gui_tab_init(GuiTab *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
	
	g_signal_connect(self->close_button, "clicked", G_CALLBACK(close_tab), NULL);
}

static void gui_tab_dispose(GObject *gobject)
{
	gtk_widget_dispose_template(GTK_WIDGET(gobject), GUI_TAB_TYPE);
	
	G_OBJECT_CLASS (gui_tab_parent_class)->dispose (gobject);
}

static void gui_tab_class_init(GuiTabClass *klass)
{
	G_OBJECT_CLASS (klass)->dispose = gui_tab_dispose;
	
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class,
                                               "/com/cflap/templates/tab.ui");

	gtk_widget_class_bind_template_child (widget_class, GuiTab, label);
	gtk_widget_class_bind_template_child (widget_class, GuiTab, close_button);
}

GuiTab *gui_tab_new(char *tab_name, GtkWidget *page_content)
{
	GuiTab *self = g_object_new(GUI_TAB_TYPE, NULL);
	
	self->page_content = page_content;
	gtk_label_set_label(self->label, tab_name);
	gtk_button_set_label(self->close_button, "X");
	return self;
}