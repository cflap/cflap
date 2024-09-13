#pragma once

#include <gtk/gtk.h>

#define GUI_TAB_TYPE (gui_tab_get_type ())
G_DECLARE_FINAL_TYPE (GuiTab, gui_tab, GUI, TAB, GtkBox)

GuiTab *gui_tab_new(char *tab_name, GtkWidget *page_content);