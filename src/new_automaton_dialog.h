#pragma once

#include <gtk/gtk.h>

typedef struct new_automaton_dialog_data {
	char *new_tab_name;
	int automaton_type;
}NewAutomatonDialogData;

GtkWindow *new_automaton_dialog_new(GtkWindow *parent);