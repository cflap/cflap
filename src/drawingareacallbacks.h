#pragma once

#include <gtk/gtk.h>

// select
void callback_select_click_pressed(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);
void callback_select_drag_begin(GtkGestureDrag *self, gdouble start_x, gdouble start_y, gpointer user_data);
void callback_select_drag_update(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data);
void callback_select_drag_end(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data);

// state
void callback_state_drag_begin(GtkGestureDrag *self, gdouble start_x, gdouble start_y, gpointer user_data);
void callback_state_drag_update(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data);
void callback_state_drag_end(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data);

// transition
void callback_transition_drag_begin(GtkGestureDrag *self, gdouble start_x, gdouble start_y, gpointer user_data);
void callback_transition_drag_update(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data);
void callback_transition_drag_end(GtkGestureDrag *self, gdouble offset_x, gdouble offset_y, gpointer user_data);