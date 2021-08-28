/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
// List of available engines under windows
#include "../../include/UniversalSpeech.h"

#include "../private.h"

DLLEXPORT int jfwIsAvailable(void);
DLLEXPORT int jfwSayW(const wchar_t*, int);
DLLEXPORT int jfwBrailleW(const wchar_t*);
DLLEXPORT int jfwStopSpeech(void);
DLLEXPORT int jfwUnload(void);
DLLEXPORT int nvdaIsAvailable(void);
DLLEXPORT int nvdaUnload(void);
DLLEXPORT int nvdaSayW(const wchar_t*, int);
DLLEXPORT int nvdaBraille(const wchar_t*);
DLLEXPORT int nvdaStopSpeech(void);
DLLEXPORT int weIsAvailable(void);
DLLEXPORT int weSayW(const wchar_t*, int);
DLLEXPORT int weBrailleW(const wchar_t*);
DLLEXPORT int weStopSpeech(void);
DLLEXPORT int weUnload(void);
DLLEXPORT int saUnload(void);
DLLEXPORT int saIsAvailable(void);
DLLEXPORT int saSayW(const wchar_t*, int);
DLLEXPORT int saBrailleW(const wchar_t*);
DLLEXPORT int saStopSpeech(void);
DLLEXPORT int dolUnload(void);
DLLEXPORT int dolIsAvailable(void);
DLLEXPORT int dolSay(const wchar_t*, int);
DLLEXPORT int dolStopSpeech(void);
DLLEXPORT int cbrUnload(void);
DLLEXPORT int cbrIsAvailable(void);
DLLEXPORT int cbrSayW(const wchar_t*, int);
DLLEXPORT int cbrBrailleW(const wchar_t*);
DLLEXPORT int cbrStopSpeech(void);
DLLEXPORT int ztLoad(void);
DLLEXPORT int ztUnload(void);
DLLEXPORT int ztIsAvailable(void);
DLLEXPORT int ztSayW(const wchar_t*, int);
DLLEXPORT int ztStopSpeech(void);
DLLEXPORT int sapiIsAvailable(void);
DLLEXPORT int sapiSayW(const wchar_t*, int);
DLLEXPORT int sapiStopSpeech(void);
DLLEXPORT int sapiUnload(void);
DLLEXPORT int sapiSetValue(int, int);
DLLEXPORT int sapiGetValue(int);
DLLEXPORT const void* sapiGetString(int);
DLLEXPORT int narIsAvailable(void);

static int doNothing() { return 1; }

const engine engines[] = {
{.name = L"Jaws", .isAvailable = jfwIsAvailable, .unload = jfwUnload, .say = jfwSayW, .stop = jfwStopSpeech, .braille = jfwBrailleW, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"Windows eye", .isAvailable = weIsAvailable, .unload = weUnload, .say = weSayW, .stop = weStopSpeech, .braille = weBrailleW, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"NVDA", .isAvailable = nvdaIsAvailable, .unload = nvdaUnload, .say = nvdaSayW, .braille = nvdaBraille, .stop = nvdaStopSpeech, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"System access", .isAvailable = saIsAvailable, .unload = saUnload, .say = saSayW, .stop = saStopSpeech, .braille = saBrailleW, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"Supernova", .isAvailable = dolIsAvailable, .unload = dolUnload, .say = dolSay, .stop = dolStopSpeech, .braille = doNothing, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"ZoomText", .isAvailable = ztIsAvailable, .unload = ztUnload, .say = ztSayW, .stop = ztStopSpeech, .braille = doNothing, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"Cobra", .isAvailable = cbrIsAvailable, .unload = cbrUnload, .say = cbrSayW, .stop = cbrStopSpeech, .braille = cbrBrailleW, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"Narrator", .isAvailable = narIsAvailable, .unload = doNothing, .say = doNothing, .stop = doNothing, .braille = doNothing, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL  },
{.name = L"SAPI5", .isAvailable = sapiIsAvailable, .unload = sapiUnload, .say = sapiSayW, .stop = sapiStopSpeech, .braille = doNothing, .setValue = sapiSetValue, .getValue = sapiGetValue, .setString = NULL, .getString = sapiGetString  },
{.name = NULL, .isAvailable = NULL, .unload = NULL, .stop = NULL, .say = NULL, .braille = NULL, .setValue = NULL, .getValue = NULL, .setString = NULL, .getString = NULL }
};
const int numEngines = sizeof(engines) / sizeof(engine) - 1;
