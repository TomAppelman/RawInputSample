#pragma once
#include <wtypes.h>
#include "platform.h"

struct MouseState{
	unsigned long	Buttons;
	short	ScrollWheel;
	POINT			Axis;
};
struct KeyCode{
	enum{
		kEscape = 0x1b,
		kF1 = 0x70,
		kF2 = 0x71,
		kF3 = 0x72,
		kF4 = 0x73,
		kF5 = 0x74,
		kF6 = 0x75,
		kF7 = 0x76,
		kF8 = 0x77,
		kF9 = 0x78,
		kF10 = 0x79,
		kF11 = 0x7A,
		kF12 = 0x7B,
		kCtrl = 0x11,
		kAlt = 0x12,
		kSpace = 0x20,
		kTab = 0x09,
		kTilda = 0x7e,
		kShift = 0x10,
		kBackspace = 0x08,
		kEnter = 0x0d,
		kHome = 0x24,
		kEnd = 0x23,
		kPageUp = 0x21,
		kPageDown = 0x22,
		kUp = 0x26,
		kDown = 0x28,
		kLeft = 0x25,
		kRight = 0x27,
		k0 = 0x30,
		k1 = 0x31,
		k2 = 0x32,
		k3 = 0x33,
		k4 = 0x34,
		k5 = 0x35,
		k6 = 0x36,
		k7 = 0x37,
		k8 = 0x38,
		k9 = 0x39,
		kA = 0x41,
		kB = 0x42,
		kC = 0x43,
		kD = 0x44,
		kE = 0x45,
		kF = 0x46,
		kG = 0x47,
		kH = 0x48,
		kI = 0x49,
		kJ = 0x4A,
		kK = 0x4B,
		kL = 0x4C,
		kM = 0x4D,
		kN = 0x4E,
		kO = 0x4F,
		kP = 0x50,
		kQ = 0x51,
		kR = 0x52,
		kS = 0x53,
		kT = 0x54,
		kU = 0x55,
		kV = 0x56,
		kW = 0x57,
		kX = 0x58,
		kY = 0x59,
		kZ = 0x5A,
		kMin = 0x31,
		kPlus = 0x31,
		kEqual = 0x31,
	};
};
struct KeyState{
	enum{
		kDown = 1,
		kUp = 0,
	};
};
struct KeyboardState{
	enum{
		kNumKeys = 256,
	};
	char Key[kNumKeys];
};

class InputInterface{
public:
	InputInterface();
	~InputInterface();
	bool	init(HWND hWnd);
	void	shutdown();
	void	reset();

	bool	handleMessage(HRAWINPUT* pRawData);
	void	deviceAdded(HANDLE hDevice);
	void	deviceRemoved(HANDLE hDevice);

	void	getKeyboardState(KeyboardState& state);
	void	getMouseState(MouseState& state);

	bool	isKeyboardKeyDown(unsigned short& KeyCode);
	bool	getKeyboardLastPressedKey(unsigned short& KeyCode);

private:
	Mutex			m_Mutex;
	unsigned short	m_lastKeyboardKeyCode;
	bool			m_LastKeyboardKeyNew;
	KeyboardState	m_keyboardState;
	MouseState		m_mouseState;
};