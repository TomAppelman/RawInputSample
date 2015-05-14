#include "platform.h"
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

//
int appDebugf(const wchar_t* fmt, ...){
	wchar_t strBuffer[1024] = L"";
	va_list arg;
	va_start(arg, fmt);
	int result = _vsnwprintf_s(strBuffer, 1024, 1024, fmt, arg);
	va_end(arg);
	OutputDebugStringW(strBuffer);
	return result;
}
int appMsgBox(const wchar_t* caption, const wchar_t* text, int type, HWND hWnd){
	return MessageBox(hWnd, text, caption, type);
}
int appGetLastErrorMsg(){
	DWORD err = GetLastError();
	wchar_t* Error = 0;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err, 0, (LPTSTR)&Error, 0, NULL) == 0)
	{
		return 0;
	}

	// Display message.
	appMsgBox(L"Error", Error, MB_OK | MB_ICONEXCLAMATION);

	// Free the buffer.
	if (Error){
		LocalFree(Error);
		Error = 0;
	}
	return 1;
}
void appExit(){
	ExitProcess(1);
}
void appOpenConsole(){
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
}
void appCloseConsole(){
	FreeConsole();
}


//
Mutex::Mutex(){
	m_hMutex = NULL;
}
Mutex::~Mutex(){

}
bool Mutex::Open(const wchar_t* name){
	WaitForSingleObject(m_hMutex, INFINITE);
	m_hMutex = CreateMutex(0, 0, name);
	return (m_hMutex != NULL);
}
void Mutex::Close(){
	ReleaseMutex(m_hMutex);
}

//
Window::Window(){
	m_hWnd = NULL;
}
Window::~Window(){

}
int Window::Create(const wchar_t* title, const RECT& rect, WNDPROC msgCallback, bool centered_to_screen){
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);;
	wc.hIcon = (HICON)LoadIcon(GetModuleHandle(0), IDI_APPLICATION);
	wc.hIconSm = (HICON)LoadImage(GetModuleHandle(0), IDI_APPLICATION, IMAGE_ICON, 16, 16, 0);
	wc.hCursor = (HCURSOR)LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = L"MAINWND";
	wc.lpfnWndProc = msgCallback;

	ATOM result = RegisterClassEx(&wc);
	if (!result){
		DWORD err = GetLastError();
		if (!result && !(err == ERROR_CLASS_ALREADY_EXISTS)){
			appGetLastErrorMsg();
			return false;
		}
	}

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"MAINWND", title, WS_OVERLAPPEDWINDOW, rect.left, rect.top, rect.right, rect.bottom, NULL, 0, GetModuleHandle(0), 0);
	if (!m_hWnd){
		appGetLastErrorMsg();
		return false;
	}

	RECT wndRect;
	::GetWindowRect(GetHandle(), &wndRect);
	int x = (::GetSystemMetrics(SM_CXSCREEN) / 2) + ((wndRect.left - wndRect.right) / 2);
	int y = (::GetSystemMetrics(SM_CYSCREEN) / 2) - ((wndRect.bottom - wndRect.top) / 2);
	::SetWindowPos(GetHandle(), 0, x, y, 0, 0, SWP_NOSIZE);

	UpdateWindow(m_hWnd);
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	return true;
}
void Window::Destroy(){
	DestroyWindow(m_hWnd);
}
HWND Window::GetHandle(){
	return m_hWnd;
}
