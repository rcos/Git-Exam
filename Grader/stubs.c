#include <stdio.h>
#include <stdlib.h>

#include "stubs.h"

int Tk_GetNumMainWindows(void) {
	fprintf(stderr, "ERROR: Tk_GetNumMainWindows unexpectedly invoked!");
	exit(-1);
}

int Tk_Init(void* interp) {
	fprintf(stderr, "ERROR: Tk_Init unexpectedly invoked!");
	exit(-1);
}

void* Tk_MainWindow(void* interp) {
	fprintf(stderr, "ERROR: Tk_MainWindow unexpectedly invoked!");
	exit(-1);
}
