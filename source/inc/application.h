#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <windows.h>
#include <wrl.h>
#include <memory>
#include <chrono>

#include <commandqueue.h>
#include <IRender.h>
#include <config.h>

enum AppState {

	aOpen = 0,
	aRunning,
	aClosing,
	aClosed

};

class Application {

public:

	static void Initialize(Application** application, HINSTANCE instance);
	void Run();
	static void Destroy(Application** application);
	static LRESULT CALLBACK s_WindowProc(HWND windowInstance, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(HWND windowInstance, UINT uMsg, WPARAM wParam, LPARAM lParam);
	AppState GetState();

private:

	void OnStartup();
	BOOL OnRender();
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2);
	void OnSpace();
	void OnPaint();
	BOOL OnUpdate();

	Application(HINSTANCE instance);
	~Application();

	HINSTANCE m_instance;
	AppState m_state;
	HWND m_window;

	Microsoft::WRL::ComPtr<ID3D12Device2> m_dxDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;

	Microsoft::WRL::ComPtr<IDXGIDebug> m_dxgiDebug;

	std::shared_ptr<CommandQueue> m_directCQ;
	std::shared_ptr<CommandQueue> m_copyCQ;
	std::unique_ptr<IRender> m_renderer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_backBuffers[SWAP_CHAIN_BUFFER_COUNT];

	UINT64 m_fenceValues[SWAP_CHAIN_BUFFER_COUNT] = { 0 };

	WORD mX1 = 0;
	WORD mY1 = 0;

	std::chrono::high_resolution_clock m_clock;
	std::chrono::high_resolution_clock::time_point m_t0;
	DOUBLE m_elapsedTime { 0 };
	DOUBLE m_elapsedTimeOneSecond { 0 };
	DOUBLE m_updateFPS { 0 };

};