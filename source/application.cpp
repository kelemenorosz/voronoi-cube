#include <application.h>

//Link DirectX libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

//DirectX includes
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

//Project external includes
#include <memory>
#include <wrl.h>

//Project internal includes
#include <commandqueue.h>
#include <config.h>
#include <helper.h>
#include <cqrender.h>
#include <subspace_render.h>
#include <cube_lighting_render.h>
#include <clustering_iterations_render.h>

namespace mWRL = Microsoft::WRL;

void Application::Initialize(Application** application, HINSTANCE instance) {

	(*application) = new Application(instance);

}

void Application::Destroy(Application** application) {

	delete (*application);

}

void Application::Run() {

	this->OnStartup();
	::ShowWindow(m_window, SW_SHOW);

	LONG useCount = 0;
	useCount = m_copyCQ.use_count();

	m_renderer = std::make_unique<SubspaceRender>(m_dxDevice, m_copyCQ, m_directCQ);
	m_renderer->LoadContent(&m_updateFPS, m_backBuffers[0]->GetDesc());

	m_t0 = m_clock.now();

	m_state = aRunning;
	MSG msg = { 0 };
	while (::GetMessage(&msg, NULL, 0, 0) != 0) {

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

	}

	m_copyCQ->Flush();
	m_directCQ->Flush();
	m_renderer.reset();

	return;

}

LRESULT CALLBACK Application::s_WindowProc(HWND windowInstance, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	Application* application = nullptr;
	if (uMsg == WM_NCCREATE) {

		LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		application = static_cast<Application*>(createStruct->lpCreateParams);
		::SetWindowLongPtr(windowInstance, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));

	}
	else {

		application = reinterpret_cast<Application*>(::GetWindowLongPtr(windowInstance, GWLP_USERDATA));

	}

	if (application == nullptr) return::DefWindowProc(windowInstance, uMsg, wParam, lParam);

	return application->WindowProc(windowInstance, uMsg, wParam, lParam);

}

LRESULT Application::WindowProc(HWND windowInstance, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	unsigned __int8 scancode = NULL;
	unsigned __int8 isRepeat = NULL;

	switch (uMsg) {

	case WM_DESTROY:
		::DestroyWindow(windowInstance);
		::PostQuitMessage(0);
		break;
	case WM_PAINT:
		this->OnPaint();
		break;
	case WM_LBUTTONDOWN:
		//Reset coordinates to current mouse position
		mX1 = LOWORD(lParam);
		mY1 = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		//If left button is down
		if (wParam == MK_LBUTTON) {

			WORD mX2 = LOWORD(lParam);
			WORD mY2 = HIWORD(lParam);
			
			if (mX1 != mX2 && mY1 != mY2) this->OnMouseButtonLeftMove(mX1, mX2, mY1, mY2);

			mX1 = mX2;
			mY1 = mY2;

		}
		break;
	case WM_MOUSEWHEEL:
		this->OnMouseWheel(wParam, lParam);
		break;
	case WM_CHAR:
		
		scancode = (__int8)(lParam >> 16);
		isRepeat = (__int8)(lParam >> 24);

		//If SPACE is pressed once
		if (scancode == 0x39 && ((0b01000000 & isRepeat) == 0))	this->OnSpace();

		break;
	default:
		return::DefWindowProc(windowInstance, uMsg, wParam, lParam);

	}

	return S_OK;

}

AppState Application::GetState() { return m_state; }



Application::Application(HINSTANCE hinstnace) : m_instance(hinstnace), m_state(aOpen), m_window(NULL) {}

Application::~Application() {

	ThrowIfFailed(m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_DX, DXGI_DEBUG_RLO_ALL));

}

void Application::OnStartup() {

	//Register window class and create window
	WNDCLASSEX windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = Application::s_WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = m_instance;
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = (HBRUSH)(COLOR_GRAYTEXT + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"ColorQuantization";
	windowClass.hIconSm = NULL;

	::RegisterClassEx(&windowClass);
	m_window = ::CreateWindowEx(NULL, L"ColorQuantization", L"Color Quantization", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, m_instance, this);

	//Enable DirectX debug layer
	ID3D12Debug* debugInterface = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {

		debugInterface->EnableDebugLayer();
		SafeRelease(&debugInterface);

	}

	//Enable DXGI debug layer
	ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug)));

	//Create DirectX12 device, direct command queue and swap chain
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter* t_dxgiAdapter = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	DXGI_ADAPTER_DESC dxgiAdapterDescription = { 0 };
	SIZE_T maxDedicatedVideoMemory = 0;

	ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory)));
	for (UINT i = { 0 }; dxgiFactory->EnumAdapters(i, &t_dxgiAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {

		ThrowIfFailed(t_dxgiAdapter->GetDesc(&dxgiAdapterDescription));
		if (SUCCEEDED(D3D12CreateDevice(t_dxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)) &&
			maxDedicatedVideoMemory < dxgiAdapterDescription.DedicatedVideoMemory) {

			maxDedicatedVideoMemory = dxgiAdapterDescription.DedicatedVideoMemory;

			if (dxgiAdapter != nullptr) { SafeRelease(&dxgiAdapter); dxgiAdapter = nullptr; }
			dxgiAdapter = t_dxgiAdapter;
			dxgiAdapter->AddRef();

		}

		SafeRelease(&t_dxgiAdapter);
		t_dxgiAdapter = nullptr;

	}

	ThrowIfFailed(D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_dxDevice))); //Create DirectX device

	m_directCQ = std::make_shared<CommandQueue>(m_dxDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL); //Create direct command queue
	m_copyCQ = std::make_shared<CommandQueue>(m_dxDevice, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL); //Create copy command queue
	mWRL::ComPtr<ID3D12CommandQueue> directCommandQueue = m_directCQ->GetCommandQueue();

	//Check for tearing support
	BOOL tearingAllowed = FALSE;
	ThrowIfFailed(dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, (void*) &tearingAllowed, sizeof(tearingAllowed)));
	
	DXGI_SWAP_CHAIN_DESC1 swapChainDescription = {};
	swapChainDescription.Width = WINDOW_WIDTH;
	swapChainDescription.Height = WINDOW_HEIGHT;
	swapChainDescription.Format = SWAP_CHAIN_BUFFER_FORMAT;
	swapChainDescription.Stereo = FALSE;
	swapChainDescription.SampleDesc = { 1, 0 };
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
	swapChainDescription.Scaling = DXGI_SCALING_STRETCH;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDescription.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDescription.Flags = tearingAllowed ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	
	//Create swap chain and cast to IDXGISwapChain3
	IDXGISwapChain1* swapChain = nullptr;
	ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(directCommandQueue.Get(), m_window, &swapChainDescription, nullptr, nullptr, &swapChain));
	m_swapChain = (IDXGISwapChain3*) swapChain;
	SafeRelease(&swapChain);

	SafeRelease(&dxgiAdapter);
	SafeRelease(&dxgiFactory);

	//Create RTV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescription = {};
	descriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDescription.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
	descriptorHeapDescription.NodeMask = 0;
	descriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&descriptorHeapDescription, IID_PPV_ARGS(&m_RTVDescriptorHeap)));

	//Get back buffers from the swap chain and create RTVs for each
	D3D12_CPU_DESCRIPTOR_HANDLE RTVDescriptor = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	UINT RTVDescriptorIncrementSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {

		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i])));
		m_dxDevice->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, RTVDescriptor);

		RTVDescriptor.ptr = SIZE_T(UINT64(RTVDescriptor.ptr) + UINT64(RTVDescriptorIncrementSize));

	}

}

BOOL Application::OnRender() {

	//Get command list
	mWRL::ComPtr<ID3D12GraphicsCommandList> commandList = m_directCQ->GetCommandList();
	
	//Get current back buffer index and the corresponding render target view descriptor
	UINT currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	UINT RTVDescriptorSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE RTVDescriptor = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	RTVDescriptor.ptr = SIZE_T(UINT64(RTVDescriptor.ptr) + UINT64(RTVDescriptorSize) * currentBackBufferIndex);

	BOOL render = m_renderer->OnRender(commandList, m_backBuffers[currentBackBufferIndex], RTVDescriptor);

	//If no render happed return
	if (!render) return render;

	UINT64 fenceValue = m_directCQ->ExecuteCommandList(commandList);
	m_fenceValues[currentBackBufferIndex] = fenceValue;
	m_swapChain->Present(1, 0);
	m_directCQ->WaitForFenceValue(m_fenceValues[m_swapChain->GetCurrentBackBufferIndex()]);

	return render;

}

void Application::OnMouseWheel(WPARAM wParam, LPARAM lParam) {

	FLOAT delta = ((SHORT) HIWORD(wParam)) / (FLOAT) WHEEL_DELTA;
	m_renderer->OnMouseWheel(delta);

}

void Application::OnMouseButtonLeftMove(WORD mX1, WORD mX2, WORD mY1, WORD mY2) {

	m_renderer->OnMouseButtonLeftMove(mX1, mX2, mY1, mY2);

}

void Application::OnSpace() {

	m_renderer->OnSpace();

}

void Application::OnPaint() {

	BOOL update = this->OnUpdate();
	if (!update) return;

	BOOL render = this->OnRender();
	
}

BOOL Application::OnUpdate() {

	if (m_updateFPS == 0) return FALSE;

	std::chrono::high_resolution_clock::time_point t1 = m_clock.now();
	std::chrono::duration<LONGLONG, std::nano> deltaTime = t1 - m_t0;
	m_t0 = t1;

	BOOL update = FALSE;
	BYTE flag = CQRENDER_ONUPDATE_NO_FLAG;
	m_elapsedTime = m_elapsedTime + deltaTime.count() * 1e-9;
	m_elapsedTimeOneSecond = m_elapsedTimeOneSecond + deltaTime.count() * 1e-9;
	if (m_elapsedTime > m_updateFPS) {

		m_elapsedTime = m_elapsedTime - m_updateFPS;
		flag = flag | CQRENDER_ONUPDATE_UPDATE_SAMPLE_BUFFER;

	}

	if (m_elapsedTimeOneSecond > 1.0) {

		m_elapsedTimeOneSecond = 0.0;
		flag = flag | CQRENDER_ONUPDATE_UPDATE_ONE_SECOND;

	}

	update = m_renderer->OnUpdate(m_elapsedTime, flag);

	return update;

}