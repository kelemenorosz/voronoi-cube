#include <source_reader_callback.h>

//Link libraries
#pragma comment(lib, "shlwapi.lib")

//Project external includes
#include <shlwapi.h>
#include <wrl.h>

//Project internal includes
#include <helper.h>

namespace mWRL = Microsoft::WRL;

//IUnkown interface functions
STDMETHODIMP SourceReaderCallback::QueryInterface(REFIID iid, void** ppv) {

    static const QITAB qit[] =
    {
        QITABENT(SourceReaderCallback, IMFSourceReaderCallback),
        { 0 },
    };
    return QISearch(this, qit, iid, ppv);

}

STDMETHODIMP_(ULONG) SourceReaderCallback::AddRef() {

    return InterlockedIncrement(&m_nRefCount);

}

STDMETHODIMP_(ULONG) SourceReaderCallback::Release() {

    ULONG uCount = InterlockedDecrement(&m_nRefCount);
    if (uCount == 0)
    {
        delete this;
    }
    return uCount;

}



//IMFSourceReaderCallback interface functions
HRESULT SourceReaderCallback::OnFlush(DWORD dwStreamIndex) {

    return S_OK;

}

HRESULT SourceReaderCallback::OnEvent(DWORD dwStreamIndex, IMFMediaEvent* pEvent) {

    return S_OK;

}

HRESULT SourceReaderCallback::OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample* pSample) {

    m_sample = pSample;

    ::SetEvent(m_event);

    return S_OK;

}

void SourceReaderCallback::Initialize(HANDLE argEvent, SourceReaderCallback** sourceReaderCallback) {

    *sourceReaderCallback = new SourceReaderCallback(argEvent);

}



void SourceReaderCallback::Wait(DWORD dwMilliseconds) {

    ::WaitForSingleObject(m_event, dwMilliseconds);

}

void SourceReaderCallback::GetSample(IMFSample** sample) {

    *sample = m_sample.Get();
    (*sample)->AddRef();

    m_sample = nullptr;

}



SourceReaderCallback::SourceReaderCallback(HANDLE argEvent) : m_nRefCount(1), m_event(argEvent) {



}

SourceReaderCallback::~SourceReaderCallback() {



}
