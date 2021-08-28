/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
// sa.c: System Access, using saapi32.dll

#include "../../include/UniversalSpeech.h"

#include "encoding-conversion.h"
#include <wtypes.h>

void dhAutoInit(void);
const wchar_t* composePath(const wchar_t* dll);

static HINSTANCE systemaccess = NULL;
static BOOL(*SA_IsRunning)(void) = NULL;
static void(*SA_StopAudio)(void) = NULL;
static void(*SA_SayW)(const wchar_t*) = NULL;
static void(*SA_SayA)(const char*) = NULL;
static void(*SA_BrlShowTextW)(const wchar_t*) = NULL;
static void(*SA_BrlShowTextA)(const char*) = NULL;

DLLEXPORT void saUnload(void) {
	SA_IsRunning = NULL;
	SA_StopAudio = NULL;
	SA_SayW = NULL;
	SA_SayA = NULL;
	SA_BrlShowTextW = NULL;
	SA_BrlShowTextA = NULL;
	if (systemaccess) FreeLibrary(systemaccess);
	systemaccess = NULL;
}

DLLEXPORT BOOL saLoad(void) {
	saUnload();
	systemaccess = LoadLibraryW(composePath(L"SAAPI32.DLL"));
	if (!systemaccess) return FALSE;
#define LOAD(f) { f = GetProcAddress(systemaccess,#f); if (!f) { saUnload(); return FALSE; }}
	LOAD(SA_IsRunning) LOAD(SA_StopAudio)
		LOAD(SA_SayW) LOAD(SA_SayA)
		LOAD(SA_BrlShowTextW) LOAD(SA_BrlShowTextA)
#undef LOAD
		return TRUE;
}

DLLEXPORT BOOL saIsAvailable() {
	if (!SA_IsRunning) saLoad();
	if (SA_IsRunning) return SA_IsRunning();
	else return FALSE;
}

DLLEXPORT BOOL saSayA(const char* str) {
	if (!SA_SayA && !saLoad()) return FALSE;
	if (SA_SayA) SA_SayA(str);
	return TRUE;
}

DLLEXPORT BOOL saSayW(const wchar_t* str) {
	if (!SA_SayW && !saLoad()) return FALSE;
	if (SA_SayW) SA_SayW(str);
	return TRUE;
}

DLLEXPORT BOOL saBrailleA(const char* str) {
	if (!SA_BrlShowTextA && !saLoad()) return FALSE;
	if (SA_BrlShowTextA) SA_BrlShowTextA(str);
	return TRUE;
}

DLLEXPORT BOOL saBrailleW(const wchar_t* str) {
	if (!SA_BrlShowTextW && !saLoad()) return FALSE;
	if (SA_BrlShowTextW) SA_BrlShowTextW(str);
	return TRUE;
}

DLLEXPORT BOOL saStopSpeech(void) {
	if (!SA_StopAudio && !saLoad()) return FALSE;
	if (SA_StopAudio) SA_StopAudio();
	return TRUE;
}

