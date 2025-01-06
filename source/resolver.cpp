#include <resolver.h>

//Media foundation libraries
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")

//Media foundation includes
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

//Project external includes
#include <windows.h>
#include <memory>

//Project internal includes
#include <helper.h>
#include <media_type_debug.h>
#include <source_reader_callback.h>

//Include chrono
#ifdef max
#undef max
#endif

#include <chrono>

//Initialize source reader
Resolver::Resolver(LPCWSTR filePath) {

	//Initialize COM
	ThrowIfFailed(::CoInitializeEx(NULL, COINIT_MULTITHREADED));

	//Initialize MF
	ThrowIfFailed(MFStartup(MF_VERSION, MFSTARTUP_FULL));

	//Create callback object and the associated event handle
	m_callbackEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	SourceReaderCallback::Initialize(m_callbackEvent, &m_sourceReaderCallback);

	// -- Parse directory for video file
	// The program assumes that there is only one file in the directory
	// And that the 'filePath' variable is set to that directory

	WIN32_FIND_DATA fileInfo = {};
	wchar_t* search_path_buffer = new wchar_t[1024];
	wcscpy(search_path_buffer, filePath);
	wcscat(search_path_buffer, L"*.mkv");

	FindFirstFile(search_path_buffer, &fileInfo);

	wcscpy(search_path_buffer, filePath);
	wcscat(search_path_buffer, fileInfo.cFileName);

	//Create source reader from URL
	IMFAttributes* sourceReaderAttributes = nullptr;
	ThrowIfFailed(MFCreateAttributes(&sourceReaderAttributes, 0));
	ThrowIfFailed(sourceReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE)); //Enable YUV to RGB conversion
	ThrowIfFailed(sourceReaderAttributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, FALSE)); //Enable converters for YUV to RGB conversion
	ThrowIfFailed(sourceReaderAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, (IUnknown*) m_sourceReaderCallback.Get())); //Enable async mode
	ThrowIfFailed(MFCreateSourceReaderFromURL(search_path_buffer, sourceReaderAttributes, &m_sourceReader));
	SafeRelease(&sourceReaderAttributes);

	delete[] search_path_buffer;

	//Get first video stream index, disable audio stream
	HRESULT hr = S_OK;
	DWORD streamIndex = 0;
	DWORD videoStreamIndex = 0;
	DWORD mediaTypeIndex = 0;
	IMFMediaType* streamMediaType = nullptr;
	GUID streamMajorTypeGUID = {};
	
	//Loop through all streams
	while (hr == S_OK) {

		//Loop through a stream's supported media types
		mediaTypeIndex = 0;
		while (hr == S_OK) {

			//Get the stream's nth media type
			hr = m_sourceReader->GetNativeMediaType(streamIndex, mediaTypeIndex, &streamMediaType);
			if (hr == MF_E_NO_MORE_TYPES) {
				
				//No more media types supported for the stream, exit the loop
				hr = S_OK;
				break;

			}

			//If stream exists
			if (hr == S_OK) {

				ThrowIfFailed(streamMediaType->GetMajorType(&streamMajorTypeGUID));
				GetGUIDNameConst(streamMajorTypeGUID);

				//If stream's major media type is video
				if (streamMajorTypeGUID == MFMediaType_Video) {

					videoStreamIndex = streamIndex;

				}
				if (streamMajorTypeGUID == MFMediaType_Audio) {

					ThrowIfFailed(m_sourceReader->SetStreamSelection(streamIndex, FALSE));

				}

				SafeRelease(&streamMediaType);
			
			}
			
			++mediaTypeIndex;

		}
		
		//If there are no more stream, exit the loop
		if (hr == MF_E_INVALIDSTREAMNUMBER) {

			hr = S_OK;
			break;

		}

		++streamIndex;

	}

	//Set decoded video media type to RBG 32 bpp
	IMFMediaType* decodedVideoMediaType = nullptr;
	ThrowIfFailed(m_sourceReader->GetCurrentMediaType(videoStreamIndex, &decodedVideoMediaType));
	ThrowIfFailed(decodedVideoMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
	ThrowIfFailed(m_sourceReader->SetCurrentMediaType(videoStreamIndex, NULL, decodedVideoMediaType));
	SafeRelease(&decodedVideoMediaType);

	//Get video fps
	UINT32 fpsNumerator = 0;
	UINT32 fpsDenominator = 0;
	UINT32 videoWidth = 0;
	UINT32 videoHeight = 0;
	INT32 videoStride = 0;
	ThrowIfFailed(m_sourceReader->GetCurrentMediaType(videoStreamIndex, &decodedVideoMediaType));
	ThrowIfFailed(MFGetAttributeRatio(decodedVideoMediaType, MF_MT_FRAME_RATE, &fpsNumerator, &fpsDenominator));
	ThrowIfFailed(MFGetAttributeRatio(decodedVideoMediaType, MF_MT_FRAME_SIZE, &videoWidth, &videoHeight));
	ThrowIfFailed(decodedVideoMediaType->GetUINT32(MF_MT_DEFAULT_STRIDE, reinterpret_cast<UINT32*>(&videoStride)));
	SafeRelease(&decodedVideoMediaType);

	m_videoStreamIndex = videoStreamIndex;
	m_videoFPS = (DOUBLE)fpsNumerator / (DOUBLE)fpsDenominator;
	m_videoWidth = (UINT64)videoWidth;
	m_videoHeight = (UINT64)videoHeight;
	m_videoStride = (UINT64)(abs(videoStride));

}

Resolver::~Resolver() {

	//Release source reader
	m_sourceReader.Reset();

	//Release source reader callback object
	m_sourceReaderCallback.Reset();

	//Close event handle
	BOOL closeHandle = ::CloseHandle(m_callbackEvent);

	//Uninitialize MF
	ThrowIfFailed(MFShutdown());

	//Uninitialize COM
	::CoUninitialize();

}

void Resolver::GetFPS(DOUBLE* videoFPS) {

	*videoFPS = m_videoFPS;

}

void Resolver::StartGetSample() {

	m_sourceReader->ReadSample(m_videoStreamIndex, 0, NULL, NULL, NULL, NULL);

}

void Resolver::EndGetSample(IMFSample** sample) {

	m_sourceReaderCallback->Wait(static_cast<DWORD>(std::chrono::milliseconds::max().count()));
	m_sourceReaderCallback->GetSample(sample);

}

void Resolver::GetVideoFrameMetrics(UINT64* videoWidth, UINT64* videoHeight, UINT64* videoStride) {

	*videoWidth = m_videoWidth;
	*videoHeight = m_videoHeight;
	*videoStride = m_videoStride;

}