#pragma once

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <windows.h>
#include <wrl.h>
#include <memory>

#include <source_reader_callback.h>

class Resolver {


public:

	Resolver(LPCWSTR filePath);
	~Resolver();

	void GetFPS(DOUBLE* videoFPS);
	void EndGetSample(IMFSample** sample);
	void StartGetSample();
	void GetVideoFrameMetrics(UINT64* videoWidth, UINT64* videoHeight, UINT64* videoStride);

private:

	Microsoft::WRL::ComPtr<IMFSourceReader> m_sourceReader;
	Microsoft::WRL::ComPtr<SourceReaderCallback> m_sourceReaderCallback;

	DWORD m_videoStreamIndex { 0 };
	HANDLE m_callbackEvent;

	DOUBLE m_videoFPS { 0.0f };
	UINT64 m_videoWidth { 0 };
	UINT64 m_videoHeight { 0 };
	UINT64 m_videoStride { 0 };
};