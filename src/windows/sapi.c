/*
Copyright (c) 2011-2021, Quentin Cosendey
This code is part of universal speech which is under multiple licenses.
Please refer to the readme file provided with the package for more information.
*/
// sapi.c: SAPI5, using MS COM object

#include "../../include/UniversalSpeech.h"

#include <string.h>
#include <objbase.h> 
#include <sapi.h>  
#include "disphelper.h"
#include "encoding-conversion.h"

void dhAutoInit(void);
DLLEXPORT BOOL sapiLoad(void);

struct {
    unsigned long data1;
    unsigned short data2, data3;
    unsigned long data4, data5;
}
Z_IID_ISpVoice = { 0x6C44DF74, 0x72B9, 0x4992, 0x99EFECA1, 0xD422046E },
Z_IID_ISpObjectTokenCategory = { 0x2D3D3845, 0x39AF, 0x4850, 0xB440F9BB, 0x1D018097 },
Z_SPDFID_WaveFormatEx = { 0xC31ADBAE, 0x527F, 0x4FF5, 0x2BF630A2, 0x0CF71FB6 },
Z_IID_ISpStreamFormat = { 0xBED530BE, 0x2606, 0x4F4D, 0xC554C0A1, 0x6F56A5CD },
Z_CLSID_SpVoice = { 0x96749377, 0x3391, 0x11D2, 0xC000E39E, 0x9673794F },
Z_CLSID_SpObjectTokenCategory = { 0xA910187F, 0x0C7A, 0x45AC, 0xED59CC92, 0x537BB7AF };

extern int nativeSpeechEnabled;

DLLEXPORT const wchar_t* sapiGetVoiceNameW(int n);

typedef int(*SapiWaveOutputCallback)(void*, void*, int);
typedef struct sapiOutputBuffer sapiOutputBuffer;

typedef struct {
    const struct ICBStreamVtbl* lpVtbl;
    SapiWaveOutputCallback callback;
    void* udata;
    DWORD refcnt;
    WAVEFORMATEX fmt;
} ICBStream;

struct ICBStreamVtbl {
    HRESULT(*__stdcall QueryInterface)(ICBStream*, REFIID, void**);
    HRESULT(*__stdcall AddRef)(ICBStream*);
    HRESULT(*__stdcall Release)(ICBStream*);
    HRESULT(*__stdcall Read)(ICBStream*, void*, ULONG, ULONG*);
    HRESULT(*__stdcall Write)(ICBStream*, void*, ULONG, ULONG*);
    HRESULT(*__stdcall Seek)(ICBStream*, LARGE_INTEGER, DWORD, ULARGE_INTEGER*);
    HRESULT(*__stdcall SetSize)(ICBStream*, ULARGE_INTEGER);
    HRESULT(*__stdcall CopyTo)(ICBStream*, IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*);
    HRESULT(*__stdcall Commit)(ICBStream*, DWORD);
    HRESULT(*__stdcall Revert)(ICBStream*);
    HRESULT(*__stdcall LockRegion)(ICBStream*, ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
    HRESULT(*__stdcall UnlockRegion)(ICBStream*, ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
    HRESULT(*__stdcall Stat)(ICBStream*, STATSTG*, DWORD);
    HRESULT(*__stdcall Clone)(ICBStream*, void**);
    HRESULT(*__stdcall GetFormat)(ICBStream*, GUID*, WAVEFORMATEX**);
};

static IEnumSpObjectTokens* cpEnum = NULL;
static ISpObjectTokenCategory* category = NULL;
static ISpVoice* pVoice = NULL;
static HRESULT hr;
static BOOL ispaused = FALSE;
static int curVoice = -1;

static inline BOOL wstreq(const wchar_t* s1, const wchar_t* s2) {
    while (*s1 && *s2 && *s1 == *s2) { s1++; s2++; }
    return !*s1 && !*s2;
}

DLLEXPORT int sapiGetNumVoices(void) {
    if (!cpEnum && !sapiLoad()) return -1;
    if (!cpEnum) return -1;
    unsigned long count = 0;
    hr = cpEnum->lpVtbl->GetCount(cpEnum, &count);
    if (SUCCEEDED(hr)) return count;
    else return -1;
}

static inline BOOL sapiInit2() {
    hr = CoCreateInstance(&Z_CLSID_SpObjectTokenCategory, NULL, CLSCTX_ALL, &Z_IID_ISpObjectTokenCategory, (void**)&category);
    if (!SUCCEEDED(hr)) goto error1;
    hr = category->lpVtbl->SetId(category, SPCAT_VOICES, TRUE);
    if (!SUCCEEDED(hr)) goto error1;
    hr = category->lpVtbl->EnumTokens(category, NULL, NULL, &cpEnum);
    if (!SUCCEEDED(hr)) goto error1;

    //hr = pVoice->lpVtbl->SetPriority(pVoice, SPVPRI_OVER);
    //if (!SUCCEEDED(hr)) MessageBeep(MB_ICONEXCLAMATION);

    curVoice = -1;
    ISpObjectToken* vc;
    hr = pVoice->lpVtbl->GetVoice(pVoice, &vc);
    if (SUCCEEDED(hr)) {
        wchar_t* s1 = NULL;
        const wchar_t* s2 = NULL;
        hr = vc->lpVtbl->GetStringValue(vc, NULL, &s1);
        if (SUCCEEDED(hr)) {
            int i = 0, n = sapiGetNumVoices();
            for (i = 0; i < n; i++) {
                s2 = sapiGetVoiceNameW(i);
                if (wstreq(s1, s2)) {
                    curVoice = i;
                    break;
                }
            }
        }
    }
    return TRUE;

error1:
    if (pVoice) pVoice->lpVtbl->Release(pVoice);
    if (cpEnum) cpEnum->lpVtbl->Release(cpEnum);
    if (category) category->lpVtbl->Release(category);
    category = NULL;
    cpEnum = NULL;
    pVoice = NULL;
    return FALSE;
}


DLLEXPORT BOOL sapiIsAvailable(void) {
    return nativeSpeechEnabled;
}

DLLEXPORT BOOL sapiLoad(void) {
    if (pVoice && cpEnum && category) return TRUE;
    dhAutoInit();
    hr = CoCreateInstance(&Z_CLSID_SpVoice, NULL, CLSCTX_ALL, &Z_IID_ISpVoice, (void**)&pVoice);
    if (SUCCEEDED(hr)) return sapiInit2();
    else return FALSE;
}

DLLEXPORT void sapiUnload(void) {
    if (!pVoice || !cpEnum || !category) return;
    if (pVoice) pVoice->lpVtbl->Release(pVoice);
    if (cpEnum) cpEnum->lpVtbl->Release(cpEnum);
    if (category) category->lpVtbl->Release(category);
    category = NULL;
    cpEnum = NULL;
    pVoice = NULL;
    //CoUninitialize();    
}

DLLEXPORT BOOL sapiStopSpeech(void) {
    if (!pVoice) return FALSE;
    hr = pVoice->lpVtbl->Speak(pVoice, NULL, SPF_IS_NOT_XML | SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);
    if (SUCCEEDED(hr)) return TRUE;
    else return FALSE;
}

DLLEXPORT BOOL sapiSayW(const WCHAR* str, BOOL interrupt) {
    if ((!pVoice && !sapiLoad()) || !pVoice || !str) return FALSE;
    DWORD flags = SPF_IS_NOT_XML | SPF_ASYNC | (interrupt ? SPF_PURGEBEFORESPEAK : 0);
    hr = pVoice->lpVtbl->Speak(pVoice, str, flags, NULL);
    if (SUCCEEDED(hr)) return TRUE;
    else return FALSE;
}

DLLEXPORT BOOL sapiSaySSMLW(const WCHAR* str, BOOL interrupt) {
    if ((!pVoice && !sapiLoad()) || !pVoice || !str) return FALSE;
    DWORD flags = SPF_PERSIST_XML | SPF_IS_XML | SPF_ASYNC | (interrupt ? SPF_PURGEBEFORESPEAK : 0);
    hr = pVoice->lpVtbl->Speak(pVoice, str, flags, NULL);
    if (SUCCEEDED(hr)) return TRUE;
    else return FALSE;
}

DLLEXPORT BOOL sapiSayA(const char* str, BOOL interrupt) {
    wchar_t* w = mb2wc(str, CP_ACP);
    BOOL re = sapiSayW(w, interrupt);
    free(w);
    return re;
}

DLLEXPORT BOOL sapiSaySSMLA(const char* str, BOOL interrupt) {
    wchar_t* w = mb2wc(str, CP_ACP);
    BOOL re = sapiSaySSMLW(w, interrupt);
    free(w);
    return re;
}

DLLEXPORT BOOL sapiSetRate(int rate) {
    if (!pVoice && !sapiLoad() || !pVoice) return FALSE;
    rate = (rate / 5) - 10;
    if (rate > 10) rate = 10;
    else if (rate < -10) rate = -10;

    hr = pVoice->lpVtbl->SetRate(pVoice, rate);
    if (SUCCEEDED(hr)) return TRUE;
    else return FALSE;
}

DLLEXPORT int sapiGetRate(void) {
    if (!pVoice && !sapiLoad() || !pVoice) return -1;
    LONG rate;
    hr = pVoice->lpVtbl->GetRate(pVoice, &rate);
    if (SUCCEEDED(hr)) return (rate + 10) * 5;
    else return -1;
}

DLLEXPORT BOOL sapiSetVolume(int volume) {
    if (!pVoice && !sapiLoad() || !pVoice) return FALSE;
    if (volume > 100) volume = 100;
    else if (volume < 0) volume = 0;
    hr = pVoice->lpVtbl->SetVolume(pVoice, volume);
    if (SUCCEEDED(hr)) return TRUE;
    else return FALSE;
}

DLLEXPORT int sapiGetVolume(void) {
    if (!pVoice && !sapiLoad() || !pVoice) return -1;
    USHORT volume;
    hr = pVoice->lpVtbl->GetVolume(pVoice, &volume);
    if (SUCCEEDED(hr)) return volume;
    else return -1;
}

DLLEXPORT BOOL sapiSetPaused(BOOL paused) {
    if (!pVoice) return FALSE;
    if (paused) hr = pVoice->lpVtbl->Pause(pVoice);
    else hr = pVoice->lpVtbl->Resume(pVoice);
    if (SUCCEEDED(hr)) { ispaused = paused; return TRUE; }
    else return FALSE;
}

DLLEXPORT BOOL sapiIsPaused(void) {
    return ispaused;
}

DLLEXPORT BOOL sapiWait(int timeout) {
    if (!pVoice) return FALSE;
    hr = pVoice->lpVtbl->WaitUntilDone(pVoice, timeout);
    if (SUCCEEDED(hr)) return TRUE;
    else return FALSE;
}

DLLEXPORT BOOL sapiIsSpeaking() {
    if (!pVoice) return FALSE;
    SPVOICESTATUS stat;
    hr = pVoice->lpVtbl->GetStatus(pVoice, &stat, NULL);
    return stat.dwRunningState != 1;
}

DLLEXPORT BOOL sapiSetVoice(int n) {
    if (!pVoice && !sapiLoad()) return FALSE;
    if (!cpEnum || !pVoice) return FALSE;
    ISpObjectToken* token = NULL;
    hr = cpEnum->lpVtbl->Item(cpEnum, n, &token);
    if (!SUCCEEDED(hr) || !token) return FALSE;
    hr = pVoice->lpVtbl->SetVoice(pVoice, token);
    if (SUCCEEDED(hr)) { curVoice = n; return TRUE; }
    else return FALSE;
}

DLLEXPORT const wchar_t* sapiGetVoiceNameW(int n) {
    if (!cpEnum && !sapiLoad()) return NULL;
    if (!cpEnum) return NULL;
    ISpObjectToken* token = NULL;
    hr = cpEnum->lpVtbl->Item(cpEnum, n, &token);
    if (!SUCCEEDED(hr) || !token) return NULL;
    WCHAR* ch = NULL;
    hr = token->lpVtbl->GetStringValue(token, NULL, &ch);
    if (SUCCEEDED(hr) && ch) return ch;
    else return NULL;
}

DLLEXPORT const char* sapiGetVoiceNameA(int n) {
    const wchar_t* wch = sapiGetVoiceNameW(n);
    if (!wch) return NULL;
    static char* ch = NULL;
    if (ch) free(ch);
    ch = wc2mb(wch, CP_ACP);
    return ch;
}

DLLEXPORT int sapiGetVoice(void) { return curVoice; }

DLLEXPORT int sapiGetValue(int what) {
    switch (what) {
    case SP_RATE: return sapiGetRate();
    case SP_VOLUME: return sapiGetVolume();
    case SP_VOICE: return sapiGetVoice();
    case SP_PAUSED: return sapiIsPaused();
    case SP_BUSY: return sapiIsSpeaking();
    case SP_WAIT: return sapiWait(-1);
    case SP_RATE_MIN:
    case SP_VOLUME_MIN:
        return 0;
    case SP_RATE_MAX:
    case SP_VOLUME_MAX:
        return 100;
    case SP_RATE_SUPPORTED:
    case SP_VOLUME_SUPPORTED:
    case SP_PAUSE_SUPPORTED:
    case SP_BUSY_SUPPORTED:
    case SP_WAIT_SUPPORTED:
        return 1;
    default:
        return 0;
    }
}

DLLEXPORT int sapiSetValue(int what, int value) {
    switch (what) {
    case SP_RATE: return sapiSetRate(value);
    case SP_VOLUME: return sapiSetVolume(value);
    case SP_VOICE: return sapiSetVoice(value);
    case SP_PAUSED: return sapiSetPaused(value);
    case SP_WAIT: return sapiWait(value);
    default: return 0;
    }
}

DLLEXPORT const wchar_t* sapiGetString(int what) {
    if (what >= SP_VOICE && what <= SP_VOICE + 0xFFFF) return sapiGetVoiceNameW(what - SP_VOICE);
    return NULL;
}

static HRESULT __stdcall ICBCommit(ICBStream* this, DWORD flags) { return S_OK; }
static HRESULT __stdcall ICBRevert(ICBStream* this) { return STG_E_INVALIDFUNCTION; }
static HRESULT __stdcall ICBSetSize(ICBStream* this, ULARGE_INTEGER newsize) { return S_OK; }
static HRESULT __stdcall ICBStat(ICBStream* this, STATSTG* x, DWORD flags) { return STG_E_INVALIDFUNCTION; }
static HRESULT __stdcall ICBClone(ICBStream* this, void** out) { *out = this; return S_OK; }
static HRESULT __stdcall ICBCopyTo(ICBStream* this, IStream* dest, ULARGE_INTEGER count, ULARGE_INTEGER* nRead, ULARGE_INTEGER* nWritten) { return STG_E_INVALIDFUNCTION; }
static HRESULT __stdcall ICBLockRegion(ICBStream* this, ULARGE_INTEGER offset, ULARGE_INTEGER count, DWORD type) { return STG_E_INVALIDFUNCTION; }
static HRESULT __stdcall ICBRead(ICBStream* this, void* buffer, ULONG count, ULONG* nRead) { if (nRead) *nRead = 0; return STG_E_INVALIDFUNCTION; }
static HRESULT __stdcall ICBSeek(ICBStream* this, ULARGE_INTEGER pos, DWORD ance, ULARGE_INTEGER* newpos) { return S_OK; }

static HRESULT __stdcall ICBWrite(ICBStream* this, void* buffer, ULONG length, ULONG* nWritten) {
    int n = -1;
    if (this->callback) n = this->callback(this->udata, buffer, length);
    if (nWritten) *nWritten = n;
    return S_OK;
}

static HRESULT __stdcall ICBGetFormat(ICBStream* this, GUID* guid, WAVEFORMATEX** wf) {
    *guid = *(GUID*)(&Z_SPDFID_WaveFormatEx);
    *wf = &(this->fmt);
    return S_OK;
}

static ULONG __stdcall ICBAddRef(ICBStream* this) { return ++(this->refcnt); }

static ULONG __stdcall ICBRelease(ICBStream* this) {
    //if (--(this->refcnt)<=0) { free(this); return 0; }
    //else  
    return this->refcnt;
}

static HRESULT __stdcall ICBQueryInterface(ICBStream* this, REFIID r, void** p) {
    hr = E_NOINTERFACE;
    *p = 0;
    if (IsEqualIID(r, &IID_IUnknown) || IsEqualIID(r, &IID_IStream) || IsEqualIID(r, &Z_IID_ISpStreamFormat)) {
        hr = NOERROR;
        *p = this;
    }
    return hr;
}

static ICBStream* ICBNew(int sampleRate, SapiWaveOutputCallback callback, void* udata) {
    ICBStream* this = (ICBStream*)(malloc(sizeof(ICBStream)));
    if (!this) return 0;

    static const struct ICBStreamVtbl ICBLpVtbl = {
    ICBQueryInterface, ICBAddRef, ICBRelease,
    ICBRead, ICBWrite, ICBSeek,
    ICBSetSize, ICBCopyTo, ICBCommit, ICBRevert,
    ICBLockRegion, ICBLockRegion, ICBStat, ICBClone,
    ICBGetFormat
    };
    WAVEFORMATEX fmt = {
    WAVE_FORMAT_PCM, 1, sampleRate,
    sampleRate * 2, 2, 16, 0
    };
    this->lpVtbl = &ICBLpVtbl;
    this->callback = callback;
    this->udata = udata;
    this->refcnt = 1;
    this->fmt = fmt;
    return this;
}

static ICBStream* stream = 0;

DLLEXPORT BOOL sapiSetOutputCallback(int sampleRate, SapiWaveOutputCallback callback, void* udata) {
    if (sampleRate <= 0 || !callback) return S_OK == pVoice->lpVtbl->SetOutput(pVoice, NULL, FALSE);
    stream = ICBNew(sampleRate, callback, udata);
    if (!stream || !pVoice) return FALSE;
    hr = pVoice->lpVtbl->SetOutput(pVoice, stream, FALSE);
    return hr == S_OK;
}

