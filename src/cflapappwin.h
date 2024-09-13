#pragma one

#include <gtk/gtk.h>
#include "cflapapp.h"
#include "new_automaton_dialog.h"
#include "automata/fsa.h"

#define CFLAP_APP_WINDOW_TYPE (cflap_app_window_get_type())
G_DECLARE_FINAL_TYPE (CFlapAppWindow, cflap_app_window, CFLAP, APP_WINDOW, GtkApplicationWindow)

CFlapAppWindow *cflap_app_window_new(CFlapApp *app);
void cflap_app_window_open(CFlapAppWindow *win, GFile *file);
void cflap_app_window_create_automaton_tab(CFlapAppWindow *window, NewAutomatonDialogData *data);