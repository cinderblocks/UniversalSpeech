/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
// nvda.c: Non-Visual Desktop Access (NVDA), using nvdaControllerClient.dll

#include "../../include/UniversalSpeech.h"

#include "encoding-conversion.h"
#include <wtypes.h>

const wchar_t* composePath(const wchar_t* dll);
BOOL __declspec(dllexport) FindProcess(const char* needle, char* buf, size_t bufsize);
BOOL GetProcessVersionInfo(const char* pfn, int mode, char* buf, int buflen);

static HINSTANCE nvda = NULL;
static int(* nvdaController_speakText)(const wchar_t*) = NULL;
static int(* nvdaController_cancelSpeech)(void) = NULL;
static int(*nvdaController_testIfRunning)(void) = NULL;
static int(*nvdaController_brailleMessage)(const wchar_t*) = NULL;

DLLEXPORT void nvdaUnload(void) {
	nvdaController_cancelSpeech = NULL;
	nvdaController_testIfRunning = NULL;
	nvdaController_speakText = NULL;
	nvdaController_brailleMessage = NULL;
	if (nvda) FreeLibrary(nvda);
	nvda = NULL;
}

DLLEXPORT BOOL nvdaLoad(void) {
	nvdaUnload();
	nvda = LoadLibraryW(composePath(L"nvdaControllerClient.dll"));
	if (!nvda) { nvda = LoadLibraryW(composePath(L"nvdaControllerClient32.dll")); }
	if (!nvda) { return FALSE; }

	nvdaController_speakText = (int(*)(const wchar_t*))GetProcAddress(nvda, "nvdaController_speakText"); 
	if (!nvdaController_speakText) { nvdaUnload(); return FALSE; }
	nvdaController_cancelSpeech = (int(*)(void))GetProcAddress(nvda, "nvdaController_cancelSpeech"); 
	if (!nvdaController_cancelSpeech) { nvdaUnload(); return FALSE; }
	nvdaController_testIfRunning = (int(*)(void))GetProcAddress(nvda, "nvdaController_testIfRunning"); 
	if (!nvdaController_testIfRunning) { nvdaUnload(); return FALSE; }
	nvdaController_brailleMessage = (int(*)(const wchar_t*))GetProcAddress(nvda, "nvdaController_brailleMessage");
	if (!nvdaController_brailleMessage) { nvdaUnload(); return FALSE; }

	return TRUE;
}

DLLEXPORT BOOL nvdaIsAvailable(void) {
	if (!nvdaController_testIfRunning) nvdaLoad();
	if (nvdaController_testIfRunning) return !nvdaController_testIfRunning();
	else return FALSE;
}

DLLEXPORT BOOL nvdaSay(const wchar_t* w) {
	if (!nvdaController_speakText) nvdaLoad();
	if (nvdaController_speakText) return !nvdaController_speakText(w);
	else return FALSE;
}

DLLEXPORT BOOL nvdaBraille(const wchar_t* w) {
	if (!nvdaController_brailleMessage) nvdaLoad();
	if (nvdaController_brailleMessage) return !nvdaController_brailleMessage(w);
	else return FALSE;
}

DLLEXPORT BOOL nvdaStopSpeech(void) {
	if (!nvdaController_cancelSpeech) nvdaLoad();
	if (nvdaController_cancelSpeech) return !nvdaController_cancelSpeech();
	else return FALSE;
}

DLLEXPORT int nvdaSayW(const wchar_t* str, int interrupt) {
	if (interrupt && !nvdaStopSpeech()) return FALSE;
	return nvdaSay(str);
}

DLLEXPORT BOOL nvdaGetRunningVersion(char* buf, int bufmax) {
	if (!FindProcess("nvda.exe", buf, bufmax)) return FALSE;
	return GetProcessVersionInfo(buf, 2, buf, bufmax);
}

