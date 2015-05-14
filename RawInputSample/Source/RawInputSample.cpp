#include "RawInputSample.h"
#include "platform.h"
#include "InputInterface.h"

//
Window				inputWindow;
InputInterface		inputInterface;

//
LRESULT CALLBACK InputWndCallback(HWND hwnd, UINT MessageID, WPARAM wParam, LPARAM lParam){
	switch (MessageID){
	default:break;
	case WM_CREATE:
		appOpenConsole();
		if (!inputInterface.init(hwnd)){
			return false;
		}
		break;
	case WM_CLOSE:
		inputWindow.Destroy();
		break;
	case WM_DESTROY:
		inputInterface.shutdown();
		appCloseConsole();
		PostQuitMessage(0);
		break;
	case WM_INPUT:
		inputInterface.handleMessage((HRAWINPUT*)&lParam);
		break;
	case WM_INPUT_DEVICE_CHANGE:
		if (wParam == GIDC_ARRIVAL){
			inputInterface.deviceAdded((HANDLE)lParam);
		}
		if (wParam == GIDC_REMOVAL){
			inputInterface.deviceRemoved((HANDLE)lParam);
		}
		break;
	}
	return DefWindowProc(hwnd, MessageID, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, wchar_t* cmdline, int nShow){
	MSG msg;
	RECT wndRect = { 0, 0, 500, 400 };
	if (!inputWindow.Create(L"Input dummy window", wndRect, InputWndCallback, true)){
		return false;
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}