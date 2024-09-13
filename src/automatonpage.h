#pragma once

#include <gtk/gtk.h>

#include "automata/automaton.h"
#include "automaton_drawer.h"

// Defines automata types
#define AUTOMATON_FSA_TYPE 0
#define AUTOMATON_TM_TYPE 1
// End of types of automata

#define GUI_AUTOMATON_PAGE_TYPE (gui_automaton_page_get_type ())
G_DECLARE_FINAL_TYPE (GuiAutomatonPage, gui_automaton_page, GUI, AUTOMATON_PAGE, GtkGrid)

GuiAutomatonPage *gui_automaton_page_new(Automaton *automaton, int automaton_type);

Automaton *gui_automaton_page_get_automaton(GuiAutomatonPage *self);
AutomatonDrawer *gui_automaton_page_get_automaton_drawer(GuiAutomatonPage *self);
GtkOverlay *gui_automaton_page_get_overlay_container(GuiAutomatonPage *self);
GtkDrawingArea *gui_automaton_page_get_drawing_area(GuiAutomatonPage *self);

void gui_automaton_page_set_scrolledwindow_sensitive(GuiAutomatonPage *self, gboolean status);

void gui_automaton_page_update_states_number(GuiAutomatonPage *self);
void gui_automaton_page_update_transitions_number(GuiAutomatonPage *self);
void gui_automaton_page_update_initial_state(GuiAutomatonPage *self);
void gui_automaton_page_update_alphabet(GuiAutomatonPage *self);