#include <gtk/gtk.h>

#include "cflapapp.h"

int main(int argc, char *argv[])
{
	return g_application_run(G_APPLICATION(cflap_app_new()), argc, argv);
}