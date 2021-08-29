/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
// we.c: Windows eyes, using GWMicro COM object

#include "../../include/UniversalSpeech.h"

#include "encoding-conversion.h"
#include "disphelper.h"

void dhAutoInit(void);

static IDispatch* we = NULL;

DLLEXPORT int weIsAvailable() {
	return !!FindWindow("GWMExternalControl", "External Control");
}

DLLEXPORT BOOL weLoad() {
	dhAutoInit();
	if (we) SAFE_RELEASE(we);
	dhCreateObject(L"GWSpeak.Speak", NULL, &we);
	return !!we;
}

DLLEXPORT void weUnload() {
	SAFE_RELEASE(we);
}

DLLEXPORT BOOL weSayA(const char* str, int interupt) {
	if (!we && !weLoad()) return FALSE;
	dhCallMethod(we, L".SpeakString(%s)", str);
	return TRUE;
}

DLLEXPORT BOOL weSayW(const wchar_t* str, int interupt) {
	if (!we && !weLoad()) return FALSE;
	dhCallMethod(we, L".SpeakString(%S)", str);
	return TRUE;
}

DLLEXPORT BOOL weBrailleA(const char* str) {
	if (!we && !weLoad()) return FALSE;
	dhCallMethod(we, L".Display(%s)", str);
	return TRUE;
}

DLLEXPORT BOOL weBrailleW(const wchar_t* str) {
	if (!we && !weLoad()) return FALSE;
	dhCallMethod(we, L".Display(%S)", str);
	return TRUE;
}

DLLEXPORT BOOL weStopSpeech(void) {
	if (!we && !weLoad()) return FALSE;
	dhCallMethod(we, L".Silence()");
	return TRUE;
}

