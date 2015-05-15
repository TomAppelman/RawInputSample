#include "InputInterface.h"
#include "InputInterface.h"

#include <stdio.h>

inline void processKeyboard(KeyboardState& state, RAWKEYBOARD* pRawKeyboard){
	const unsigned short index = pRawKeyboard->VKey;
	if (pRawKeyboard->Flags == 3 || pRawKeyboard->Flags == RI_KEY_BREAK){
		state.Key[index] = KeyState::kUp;
	}
	else if (pRawKeyboard->Flags == 2 || pRawKeyboard->Flags == RI_KEY_MAKE){
		state.Key[index] = KeyState::kDown;
	}

	printf("keyboard key pressed : 0x%02x %s\n", index, (state.Key[index] == KeyState::kDown)? "Down" : "Up");
}
inline void processMouse(MouseState& state, RAWMOUSE* pRawMouse){
	state.ScrollWheel = 0;
	state.Buttons = 0;
	if (MOUSE_MOVE_RELATIVE == pRawMouse->usFlags){
		state.Axis.x += pRawMouse->lLastX;
		state.Axis.y += pRawMouse->lLastY;
	}
	if (RI_MOUSE_WHEEL & pRawMouse->usButtonFlags){
		state.ScrollWheel = (short)pRawMouse->usButtonData;
	}
	else{
		state.Buttons = pRawMouse->ulButtons;
	}

	printf("Mouse state : axis(%i, %i), button(0x%04x), Scoll(%i)\n", state.Axis.x, state.Axis.y, state.Buttons, state.ScrollWheel);
}


InputInterface::InputInterface(){
	m_lastKeyboardKeyCode = 0;
}
InputInterface::~InputInterface(){

}

bool InputInterface::init(HWND hWnd){
	RAWINPUTDEVICE Rid[2];

	// this enabled mouse detection
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = RIDEV_DEVNOTIFY;
	Rid[0].hwndTarget = hWnd;

	// this enabled keyboard detection
	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = RIDEV_DEVNOTIFY;
	Rid[1].hwndTarget = hWnd;

	//  register input
	if (!RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE))){
		appGetLastErrorMsg();
		return false;
	}

	m_Mutex.Open();
	ZeroMemory(&m_keyboardState, sizeof(KeyboardState));
	ZeroMemory(&m_mouseState, sizeof(MouseState));
	m_Mutex.Close();
	return true;
}
void InputInterface::shutdown(){
	RAWINPUTDEVICE Rid[2];

	// this does init but removes all devices
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = RIDEV_REMOVE;
	Rid[0].hwndTarget = 0;

	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = RIDEV_REMOVE;
	Rid[1].hwndTarget = 0;

	RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE));
}
bool InputInterface::handleMessage(HRAWINPUT* pRawData){
	if (pRawData == NULL){
		return false;
	}
	UINT dwSize;
	GetRawInputData(*pRawData, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

	BYTE lpb[4096];
	int readSize = GetRawInputData((HRAWINPUT)*pRawData, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	if (readSize != dwSize){
		return 0;
	}

	RAWINPUT* pRaw = (RAWINPUT*)lpb;


	if (pRaw->header.dwType == RIM_TYPEKEYBOARD){
		m_Mutex.Open();
		processKeyboard(m_keyboardState, &pRaw->data.keyboard);
		if (m_keyboardState.Key[pRaw->data.keyboard.VKey] != RI_KEY_BREAK){
			m_lastKeyboardKeyCode = pRaw->data.keyboard.VKey;
			m_LastKeyboardKeyNew = true;
			printf("last keyboard key pressed : 0x%02x\n", m_lastKeyboardKeyCode);
		}
		m_Mutex.Close();
	}else
	if (pRaw->header.dwType == RIM_TYPEMOUSE){
		m_Mutex.Open();
		processMouse(m_mouseState, &pRaw->data.mouse);
		m_Mutex.Close();
	}
	return true;
}

//
void InputInterface::deviceAdded(HANDLE hDevice){
	appDebugf(L"New Device attached\n");
}
void InputInterface::deviceRemoved(HANDLE hDevice){
	appDebugf(L"New Device attached\n");
}

//
void InputInterface::getKeyboardState(KeyboardState& state){
	m_Mutex.Open();
	state = m_keyboardState;
	m_Mutex.Close();
}
void InputInterface::getMouseState(MouseState& state){
	m_Mutex.Open();
	state = m_mouseState;
	m_Mutex.Close();
}

//
bool InputInterface::isKeyboardKeyDown(unsigned short& KeyCode){
	m_Mutex.Open();
	bool result = (m_keyboardState.Key[KeyCode] == KeyState::kDown);
	m_Mutex.Close();
	return result;
}
bool InputInterface::getKeyboardLastPressedKey(unsigned short& KeyCode){
	m_Mutex.Open();
	bool result = m_LastKeyboardKeyNew;
	m_LastKeyboardKeyNew = false;
	KeyCode = m_lastKeyboardKeyCode;
	m_Mutex.Close();
	return result;
}
