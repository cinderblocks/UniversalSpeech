/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
//Encoding conversion using windows routines

#include "../../include/UniversalSpeech.h"
#include "encoding-conversion.h"

#include <Windows.h>
#include <malloc.h>
#include <wchar.h>

static void* strptr = NULL;

wchar_t* mb2wc(const char* str, int inCP) {
	size_t len = MultiByteToWideChar(inCP, 0, str, -1, NULL, 0);
	wchar_t* wstr = malloc((len + 1)* sizeof(wchar_t));
	if (wstr == NULL) { return NULL; }
	MultiByteToWideChar(inCP, 0, str, -1, wstr, len);
	wstr[len] = 0;
	return wstr;
}

char* wc2mb(const wchar_t* wstr, int outCP) {
	size_t len = WideCharToMultiByte(outCP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = (char*)malloc((len + 1) * sizeof(char));
	if (str == NULL) { return NULL; }
	WideCharToMultiByte(outCP, 0, wstr, -1, str, len, NULL, NULL);
	str[len] = 0;
	return str;
}

char* toEncoding(const wchar_t* unicode, int targetEncoding) {
	if (!unicode) return NULL;
	if (strptr) free(strptr);
	strptr = wc2mb(unicode, targetEncoding);
	return strptr;
}

wchar_t* fromEncoding(const char* str, int encoding) {
	if (!str) return NULL;
	if (strptr) free(strptr);
	strptr = mb2wc(str, encoding);
	return strptr;
}
