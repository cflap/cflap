#include <gtk/gtk.h>

#include "cflapapp.h"
#include "cflapappwin.h"
#include "new_automaton_dialog.h"

struct _CFlapApp {
	GtkApplication parent;
};

G_DEFINE_TYPE(CFlapApp, cflap_app, GTK_TYPE_APPLICATION);

static void new_automaton_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *parent = gtk_application_get_active_window(GTK_APPLICATION(app));
	GtkWindow *dialog = new_automaton_dialog_new(parent);
	gtk_window_present(dialog);
}

static GActionEntry app_entries[] = 
{
	{"new_automaton", new_automaton_activated, NULL, NULL, NULL}
};

static void cflap_app_init(CFlapApp *app)
{
}

static void cflap_app_startup(GApplication *app)
{
	G_APPLICATION_CLASS (cflap_app_parent_class)->startup(app);
	
	g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
}

static void cflap_app_activate(GApplication *app)
{
	CFlapAppWindow *win;
	win = cflap_app_window_new(CFLAP_APP(app));
	gtk_window_present (GTK_WINDOW(win));
}

static void cflap_app_open()
{
	// This function is called when cflap is called with arguments
}

static void cflap_app_class_init(CFlapAppClass *class)
{
	G_APPLICATION_CLASS (class)->startup = cflap_app_startup;
	G_APPLICATION_CLASS (class)->activate = cflap_app_activate;
	G_APPLICATION_CLASS (class)->open = cflap_app_open;
}

CFlapApp *cflap_app_new(void)
{
	return g_object_new (CFLAP_APP_TYPE, 
						"application-id",
						"com.cflap",
						"flags", G_APPLICATION_HANDLES_OPEN,
						NULL);
}