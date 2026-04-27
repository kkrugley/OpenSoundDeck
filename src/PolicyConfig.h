// PolicyConfig.h
// Interface for setting default audio endpoint on Windows Vista/7/8/10/11
// Required for programmatically setting default recording device

#pragma once

#include <Windows.h>
#include <Mmdeviceapi.h>

// Use DECLSPEC_SELECTANY to allow multiple inclusions without linker errors
// {00000000-0000-0000-0000-000000000000} - placeholder, will be set below
EXTERN_C const GUID DECLSPEC_SELECTANY IID_IPolicyConfig =
    { 0xf8679f50, 0x850a, 0x41cf, { 0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xd8 } };

// {870af99c-171d-4f9e-af0d-e63df40c2b29}
EXTERN_C const GUID DECLSPEC_SELECTANY CLSID_CPolicyConfigClient =
    { 0x870af99c, 0x171d, 0x4f9e, { 0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0x29 } };

// {f8679f50-850a-41cf-9c72-430f290290d8}
EXTERN_C const GUID DECLSPEC_SELECTANY IID_IPolicyConfigVista =
    { 0xf8679f50, 0x850a, 0x41cf, { 0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xd8 } };

// {294935CE-6370-4f1c-AF13-4E6E5A5A8D9E}
EXTERN_C const GUID DECLSPEC_SELECTANY CLSID_CPolicyConfigVistaClient =
    { 0x294935ce, 0x6370, 0x4f1c, { 0xaf, 0x13, 0x4e, 0x6e, 0x5a, 0x5a, 0x8d, 0x9e } };

// Interface for Windows 7+
struct DECLSPEC_UUID("f8679f50-850a-41cf-9c72-430f290290d8")
IPolicyConfig : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(
        PCWSTR pwstrDeviceId,
        WAVEFORMATEX **ppFormat) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(
        PCWSTR pwstrDeviceId,
        INT bDefault,
        WAVEFORMATEX **ppFormat) = 0;

    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(
        PCWSTR pwstrDeviceId) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(
        PCWSTR pwstrDeviceId,
        WAVEFORMATEX *pEndpointFormat,
        WAVEFORMATEX *pMixFormat) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(
        PCWSTR pwstrDeviceId,
        INT bDefault,
        PUINT64 pmftDefaultPeriod,
        PUINT64 pmftMinimumPeriod) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(
        PCWSTR pwstrDeviceId,
        PUINT64 pmftPeriod) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetShareMode(
        PCWSTR pwstrDeviceId,
        /*[out]*/ INT *pMode) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetShareMode(
        PCWSTR pwstrDeviceId,
        INT mode) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(
        PCWSTR pwstrDeviceId,
        const PROPERTYKEY *pKey,
        PROPVARIANT *pv) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(
        PCWSTR pwstrDeviceId,
        const PROPERTYKEY *pKey,
        const PROPVARIANT *pv) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(
        PCWSTR pwstrDeviceId,
        ERole role) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(
        PCWSTR pwstrDeviceId,
        INT bVisible) = 0;
};

// Interface for Windows Vista
struct DECLSPEC_UUID("568b9107-44bf-40b4-9006-86afe5b5a620")
IPolicyConfigVista : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(
        PCWSTR pwstrDeviceId,
        WAVEFORMATEX **ppFormat) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(
        PCWSTR pwstrDeviceId,
        INT bDefault,
        WAVEFORMATEX **ppFormat) = 0;

    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(
        PCWSTR pwstrDeviceId) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(
        PCWSTR pwstrDeviceId,
        WAVEFORMATEX *pEndpointFormat,
        WAVEFORMATEX *pMixFormat) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(
        PCWSTR pwstrDeviceId,
        INT bDefault,
        PUINT64 pmftDefaultPeriod,
        PUINT64 pmftMinimumPeriod) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(
        PCWSTR pwstrDeviceId,
        PUINT64 pmftPeriod) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetShareMode(
        PCWSTR pwstrDeviceId,
        /*[out]*/ INT *pMode) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetShareMode(
        PCWSTR pwstrDeviceId,
        INT mode) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(
        PCWSTR pwstrDeviceId,
        const PROPERTYKEY *pKey,
        PROPVARIANT *pv) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(
        PCWSTR pwstrDeviceId,
        const PROPERTYKEY *pKey,
        const PROPVARIANT *pv) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(
        PCWSTR pwstrDeviceId,
        ERole role) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(
        PCWSTR pwstrDeviceId,
        INT bVisible) = 0;
};
