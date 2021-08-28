/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
//cobra.c: Cobra, using Baume COM objects

#include "../../include/UniversalSpeech.h"

#include "disphelper.h"
#include "encoding-conversion.h"

void dhAutoInit(void);
BOOL __declspec(dllexport) FindProcess(const char* needle, char* buf, size_t bufsize);

static IDispatch* cbr = NULL;

DLLEXPORT void cbrUnload() {
	SAFE_RELEASE(cbr);
	cbr = NULL;
}

DLLEXPORT BOOL cbrIsAvailable(void) {
	static int found = 0;
	static clock_t last = 0;
	clock_t now = clock();
	if (now - last < 60000) { return found; }
	last = now;
	found = FindProcess("cobra.exe", NULL, 0);
	return found;
}

DLLEXPORT BOOL cbrLoad(void) {
	dhAutoInit();
	if (cbr) return TRUE;
	IDispatch* broker = NULL, * cobra = NULL, * procobj = NULL;
	dhCreateObject(L"CBR_Broker.cBroker", NULL, &broker);
	if (!broker) goto end;
	dhGetValue(L"%o", &cobra, broker, L".GetCobra()");
	if (!cobra) goto end;
	dhGetValue(L"%o", &procobj, cobra, L".ProcObj()");
	if (!procobj) goto end;
	cbr = procobj;
end:
	SAFE_RELEASE(cobra);
	SAFE_RELEASE(broker);
	return !!cbr;
}

DLLEXPORT BOOL cbrStopSpeech(void) {
	if (!cbr && !cbrLoad()) return FALSE;
	dhCallMethod(cbr, L".speechStop()");
	return TRUE;
}

DLLEXPORT BOOL cbrSayA(const char* str) {
	if (!cbr && !cbrLoad()) return FALSE;
	dhCallMethod(cbr, L".speechout(%s)", str);
	return TRUE;
}

DLLEXPORT BOOL cbrSayW(const wchar_t* str) {
	if (!cbr && !cbrLoad()) return FALSE;
	dhCallMethod(cbr, L".speechout(%S)", str);
	return TRUE;
}

DLLEXPORT BOOL cbrBrailleA(const char* str) {
	if (!cbr && !cbrLoad()) return FALSE;
	dhCallMethod(cbr, L".brailleout(%s)", str);
	return TRUE;
}

DLLEXPORT BOOL cbrBrailleW(const wchar_t* str) {
	if (!cbr && !cbrLoad()) return FALSE;
	dhCallMethod(cbr, L".brailleout(%S)", str);
	return TRUE;
}
