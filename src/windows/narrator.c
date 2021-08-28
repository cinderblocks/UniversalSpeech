/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
// Windows 10 narrator

#include "../../include/UniversalSpeech.h"

#include "disphelper.h"
#include "encoding-conversion.h"
#include <time.h>

void dhAutoInit(void);
BOOL __declspec(dllexport) FindProcess(const char* needle, char* buf, size_t bufsize);

DLLEXPORT BOOL narIsAvailable(void) {
	static int found = 0;
	static clock_t last = 0;
	clock_t now = clock();
	if (now - last < 60000) { return found; }
	last = now;
	found = FindProcess("narrator.exe", NULL, 0);
	return found;
}
