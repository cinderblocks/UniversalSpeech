/**
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
/*
zt.c: ZoomText driver for UniversalSpeech
Copied and adapted from Tolk by Davy Kager <mail@davykager.nl>
*/

#include "../../include/UniversalSpeech.h"
#include <objbase.h> 

#ifndef _COM_Outptr_
#  define _COM_Outptr_
#endif
#ifndef _In_
#  define _In_
#endif
#ifndef _Out_opt_
# define _Out_opt_
#endif

#include "zoomtext.h"

static   IZoomText2* ztController = NULL;
static   ISpeech2* ztSpeech = NULL;

DLLEXPORT BOOL ztUnload(void) {
    if (ztSpeech) ztSpeech->lpVtbl->Release(ztSpeech);
    if (ztController)     ztController->lpVtbl->Release(ztController);
    ztController = NULL;
    ztSpeech = NULL;
    return TRUE;
}

DLLEXPORT BOOL ztLoad(void) {
    if (FAILED(CoCreateInstance(&CLSID_ZoomText, NULL, CLSCTX_LOCAL_SERVER, &IID_IZoomText2, (void**)&ztController)))     return FALSE;
    if (FAILED(ztController->lpVtbl->get_Speech(ztController, &ztSpeech))) {
        ztUnload();
        return FALSE;
    }
    return ztController && ztSpeech;
}

DLLEXPORT BOOL ztIsAvailable(void) {
    return !!FindWindowW(L"ZXSPEECHWNDCLASS", L"ZoomText Speech Processor");
}

DLLEXPORT BOOL ztIsActive(void) {
    if (!ztIsAvailable()) {
        ztUnload();
        return FALSE;
    }
    if (!ztController) ztLoad();
    return !!ztController;
}

DLLEXPORT BOOL ztSayW(const wchar_t* str, BOOL interrupt) {
    if (ztIsActive()) {
        const BSTR bstr = SysAllocString(str);
        IVoice* voice;
        if (FAILED(ztSpeech->lpVtbl->get_CurrentVoice(ztSpeech, &voice))) {
            ztUnload();
            return FALSE;
        }
        if (interrupt && FAILED(voice->lpVtbl->put_AllowInterrupt(voice, VARIANT_TRUE))) {
            voice->lpVtbl->Release(voice);
            ztUnload();
            return FALSE;
        }
        const BOOL succeeded = SUCCEEDED(voice->lpVtbl->Speak(voice, bstr));
        voice->lpVtbl->Release(voice);
        SysFreeString(bstr);
        if (interrupt && FAILED(voice->lpVtbl->put_AllowInterrupt(voice, VARIANT_FALSE))) {
            ztUnload();
            return FALSE;
        }
        return succeeded;
    }
    return FALSE;
}

DLLEXPORT BOOL ztStopSpeech(void) {
    if (ztIsActive()) {
        IVoice* voice;
        if (FAILED(ztSpeech->lpVtbl->get_CurrentVoice(ztSpeech, &voice))) {
            ztUnload();
            return FALSE;
        }
        const BOOL succeeded = SUCCEEDED(voice->lpVtbl->Stop(voice));
        voice->lpVtbl->Release(voice);
        return succeeded;
    }
    return FALSE;
}

