#pragma once

#include <windows.h>
#include <exception>

inline void ThrowIfFailed(HRESULT result) {

	if (FAILED(result)) throw std::exception();

}

//Safe release COM pointers
template<class T> void SafeRelease(T** ptr) {

	if (*ptr != nullptr) {

		(*ptr)->Release();
		(*ptr) = nullptr;

	}
	return;

}