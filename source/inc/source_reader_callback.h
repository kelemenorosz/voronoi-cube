#pragma once

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <wrl.h>

class SourceReaderCallback : IMFSourceReaderCallback {

public:

	//IUnkown functions
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IMFSourceReaderCallback functions
	HRESULT OnFlush(DWORD dwStreamIndex);
	HRESULT OnEvent(DWORD dwStreamIndex, IMFMediaEvent* pEvent);
	HRESULT OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample* pSample);

	static void Initialize(HANDLE argEvent, SourceReaderCallback** sourceReaderCallback);

	void Wait(DWORD dwMilliseconds);
	void GetSample(IMFSample** sample);

private:

	SourceReaderCallback(HANDLE argEvent);
	~SourceReaderCallback();

	LONG m_nRefCount;
	HANDLE m_event;

	Microsoft::WRL::ComPtr<IMFSample> m_sample;

};