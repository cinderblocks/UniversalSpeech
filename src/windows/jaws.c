/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
// jaws.c: Jaws For Windows, using FSAPI / Freedomsci.JawsApi COM objects

#include "../../include/UniversalSpeech.h"

#include "disphelper.h"
#include "encoding-conversion.h"

void dhAutoInit(void);
BOOL __declspec(dllexport) FindProcess(const char* needle, char* buf, size_t bufsize);
BOOL GetProcessVersionInfo(const char* pfn, int mode, char* buf, int buflen);

static IDispatch* jfw = NULL;

DLLEXPORT BOOL jfwLoad() {
	dhAutoInit();
	if (jfw) SAFE_RELEASE(jfw);
	if (!jfw) dhCreateObject(L"FreedomSci.JawsApi", NULL, &jfw);
	if (!jfw) dhCreateObject(L"JFWApi", NULL, &jfw);
	if (!jfw) dhCreateObject(L"JFWApi.1", NULL, &jfw);
	return !!jfw;
}

DLLEXPORT void jfwUnload(void) {
	SAFE_RELEASE(jfw);
}

DLLEXPORT int jfwIsAvailable() {
	return !!FindWindow("JFWUI2", NULL);
}

DLLEXPORT BOOL jfwSayA(const char* str, BOOL interrupt) {
	if (!jfw && !jfwLoad()) return FALSE;
	BOOL result = FALSE;
	dhGetValue(L"%b", &result, jfw, L".SayString(%s,%b)", str, interrupt);
	return result;
}

DLLEXPORT BOOL jfwSayW(const wchar_t* str, BOOL interrupt) {
	if (!jfw && !jfwLoad()) return FALSE;
	BOOL result = FALSE;
	dhGetValue(L"%b", &result, jfw, L".SayString(%S,%b)", str, interrupt);
	return result;
}

DLLEXPORT BOOL jfwRunScriptA(const char* scriptName) {
	if (!jfw && !jfwLoad()) return FALSE;
	BOOL result = FALSE;
	dhGetValue(L"%b", &result, jfw, L".RunScript(%s)", scriptName);
	return result;
}

DLLEXPORT BOOL jfwRunScriptW(const wchar_t* scriptName) {
	if (!jfw && !jfwLoad()) return FALSE;
	BOOL result = FALSE;
	dhGetValue(L"%b", &result, jfw, L".RunScript(%S)", scriptName);
	return result;
}

DLLEXPORT BOOL jfwRunFunctionA(const char* scriptName) {
	if (!jfw && !jfwLoad()) return FALSE;
	BOOL result = FALSE;
	dhGetValue(L"%b", &result, jfw, L".RunFunction(%s)", scriptName);
	return result;
}

DLLEXPORT BOOL jfwRunFunctionW(const wchar_t* scriptName) {
	if (!jfw && !jfwLoad()) return FALSE;
	BOOL result = FALSE;
	dhGetValue(L"%b", &result, jfw, L".RunFunction(%S)", scriptName);
	return result;
}

DLLEXPORT BOOL jfwBrailleA(const char* str) {
	size_t len = strlen(str);
	char* buf = malloc((len + 20) * sizeof(char));
	if (buf == NULL) { return FALSE; }
	buf[0] = 0;
	strcat(buf, "BrailleString(\"");
	strcat(buf, str);
	strcat(buf, "\")");
	char* c = strchr(buf, 34), * end = strrchr(buf, 34);
	while (++c < end) {
		if (*c == 34 || *c == 92 || *c < 32) *c = 32;
	}
	BOOL ret = jfwRunFunctionA(buf);
	free(buf);
	return ret;
}

DLLEXPORT BOOL jfwBrailleW(const wchar_t* str) {
	size_t len = wcslen(str);
	wchar_t* buf = malloc((len + 20) * sizeof(char));
	if (buf == NULL) { return FALSE; }
	buf[0] = 0;
	wcscat(buf, L"BrailleString(\"");
	wcscat(buf, str);
	wcscat(buf, L"\")");
	wchar_t* c = wcschr(buf, 34), * end = wcsrchr(buf, 34);
	while (++c < end) {
		if (*c == 34 || *c == 92 || *c < 32) *c = 32;
	}
	BOOL ret = jfwRunFunctionW(buf);
	free(buf);
	return ret;
}

DLLEXPORT BOOL jfwStopSpeech() {
	if (!jfw && !jfwLoad()) return FALSE;
	BOOL result = FALSE;
	dhGetValue(L"%b", &result, jfw, L".StopSpeech()");
	return result;
}

DLLEXPORT BOOL jfwGetUserSettingsDirectory(char* buf, size_t bufmax) {
	if (!FindProcess("jfw.exe", buf, bufmax)) return FALSE;
	char sVer[16] = { 0 }, * tok = NULL, * appdata = getenv("APPDATA");
	tok = strtok(buf, "\\");
	while (tok) {
		int n;
		if (sscanf(tok, "%d", &n)) break;
		tok = strtok(NULL, "\\");
	}
	if (!tok || !appdata) return FALSE;
	strncpy(sVer, tok, 15);
	int pos = snprintf(buf, bufmax, "%s\\Freedom Scientific\\Jaws\\%s\\Settings\\???", appdata, sVer) - 3;
	WIN32_FIND_DATA fd;
	HANDLE hfd = FindFirstFile(buf, &fd);
	if (!hfd) return FALSE;
	do {
		if (!strcmp(fd.cFileName, ".") && !strcmp(fd.cFileName, "..") && !strcmp(fd.cFileName, "enu")) break;
	} while (FindNextFile(hfd, &fd));
	FindClose(hfd);
	snprintf(buf + pos, bufmax - pos, "%s\\", fd.cFileName);
	return TRUE;
}

DLLEXPORT BOOL jfwGetRunningVersion(char* buf, int bufmax) {
	if (!FindProcess("jfw.exe", buf, bufmax)) return FALSE;
	return GetProcessVersionInfo(buf, 1, buf, bufmax);
}


