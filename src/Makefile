all:
	glib-compile-resources templates/cflap.gresource.xml --target=resources.c --generate-source
	gcc $$(pkg-config --cflags gtk4) *.c automata/*.c -o CFLAP.exe $$(pkg-config --libs gtk4)