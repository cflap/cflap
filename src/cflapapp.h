#pragma once

#include <gtk/gtk.h>

#define CFLAP_APP_TYPE (cflap_app_get_type())
G_DECLARE_FINAL_TYPE (CFlapApp, cflap_app, CFLAP, APP, GtkApplication)

CFlapApp *cflap_app_new(void);