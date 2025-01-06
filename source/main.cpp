//Setup ::_CrtDumpMemoryLeaks()
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//Project external includes
#include <windows.h>

//Project internal includes
#include <application.h>
#include <helper.h>

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nCmdShow) {

	ThrowIfFailed(::CoInitializeEx(NULL, COINIT_MULTITHREADED));

	Application* app = nullptr;
	Application::Initialize(&app, hInstance);

	app->Run();
	
	Application::Destroy(&app);
	::CoUninitialize();
	::_CrtDumpMemoryLeaks();
	return 0;

}