/*
 * This file contains the implementation of the interface of a FsaAutomaton
 */

#include <stdlib.h>
#include <glib.h>
 
#include "automaton_private.h"
#include "fsa_private.h"

Automaton *fsa_new(void)
{
	FsAutomaton *fsa_automaton = malloc(sizeof(FsAutomaton));
	if (fsa_automaton == NULL)
		return NULL;
		
	Automaton *super = automaton_new();
	if (super == NULL) {
		free(fsa_automaton);
		return NULL;
	}
	
	super->destroy = &fsa_destroy_private;
	super->create_transition_input = &fsa_create_transition_input_private;
	super->derived = fsa_automaton;
	fsa_automaton->super = super;
	
	return fsa_automaton->super;
}

void fsa_destroy_private(Automaton *automaton)
{		
	FsAutomaton *fsa_automaton = automaton->derived;
	
	free(fsa_automaton);
	automaton_destroy_private(automaton);
}

Transition *fsa_change_transition_symbol(Automaton *automaton, Transition *transition, const char *symbol)
{
	if (automaton == NULL || transition == NULL)
		return NULL;
		
	Transition *clone = transition_clone(transition);
	fsa_transition_set_symbol(clone, symbol);
	
	automaton_remove_transition(automaton, transition);
	
	automaton_add_transition(automaton, clone);
	
	return clone;
}

struct transition_info {
	State *from;
	State *to;
	Automaton *automaton;
	gpointer automaton_page;
};

static void pressed_enter_key_callback(GtkEntry *self, gpointer user_data)
{
	struct transition_info *t_info = (struct transition_info *) user_data;
	State *from = t_info->from;
	State *to = t_info->to;
	Automaton *automaton = t_info->automaton;
	GuiAutomatonPage *automaton_page = (GuiAutomatonPage *) t_info->automaton_page;
	GtkDrawingArea *drawing_area = gui_automaton_page_get_drawing_area(automaton_page);
	GtkEntryBuffer *symbol_buffer = gtk_entry_get_buffer(self);
	const char *symbol = gtk_entry_buffer_get_text(symbol_buffer);
	Transition *new_transition = fsa_transition_new(from, to, symbol);
	automaton_add_transition(automaton, new_transition);
	
	GtkWidget *fixed = gtk_widget_get_ancestor(GTK_WIDGET(self), GTK_TYPE_FIXED);
	GtkWidget *parent_fixed = gtk_widget_get_parent(fixed);
	gtk_overlay_remove_overlay(GTK_OVERLAY(parent_fixed), fixed);
	gtk_fixed_remove(GTK_FIXED(fixed), GTK_WIDGET(self));
	gtk_widget_unparent(fixed);
	
	free(t_info);
	gui_automaton_page_update_transitions_number(automaton_page);
	
	gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
	
	gui_automaton_page_set_scrolledwindow_sensitive(automaton_page, TRUE);
	gui_automaton_page_update_alphabet(automaton_page);
}

GtkWidget *fsa_create_transition_input_private(Automaton *automaton, State *from, State *to, gpointer automaton_page)
{
	GtkWidget *symbol_entry = gtk_entry_new();
	gtk_widget_set_size_request(symbol_entry, 80, 25);
	
	struct transition_info *t_info = malloc(sizeof(struct transition_info));
	t_info->from = from;
	t_info->to = to;
	t_info->automaton = automaton;
	t_info->automaton_page = automaton_page;
	g_signal_connect(symbol_entry, "activate", (GCallback) pressed_enter_key_callback, t_info);
	
	return symbol_entry;
}
