#include"../Public/InputManager.h"
#include"../../../AngelCore/AngelSubSystem/Public/SubSystemsResource.h"
#include"../../../AngelCore/AngelSubSystem/Public/VRManager.h"
#include"../../../AngelCore/AngelSubSystem/Public/SubSystemManager.h"

AngelCore::AngelSubSystem::InputManager *AngelCore::AngelSubSystem::inputManager;

int AngelCore::AngelSubSystem::InputManager::Keys::A = 0x1E;
int AngelCore::AngelSubSystem::InputManager::Keys::One=               0x02;
int AngelCore::AngelSubSystem::InputManager::Keys::Two=               0x03;
int AngelCore::AngelSubSystem::InputManager::Keys::Three=               0x04;
int AngelCore::AngelSubSystem::InputManager::Keys::Four=               0x05;
int AngelCore::AngelSubSystem::InputManager::Keys::Five=               0x06;
int AngelCore::AngelSubSystem::InputManager::Keys::Six=               0x07;
int AngelCore::AngelSubSystem::InputManager::Keys::Seven=               0x08;
int AngelCore::AngelSubSystem::InputManager::Keys::Eight=               0x09;
int AngelCore::AngelSubSystem::InputManager::Keys::Nine=               0x0A;
int AngelCore::AngelSubSystem::InputManager::Keys::Zero=               0x0B;
int AngelCore::AngelSubSystem::InputManager::Keys::MINUS=           0x0C;    /* - on main keyboard */
int AngelCore::AngelSubSystem::InputManager::Keys::EQUALS=          0x0D;
int AngelCore::AngelSubSystem::InputManager::Keys::BACK=            0x0E;    /* backspace */
int AngelCore::AngelSubSystem::InputManager::Keys::TAB=             0x0F;
int AngelCore::AngelSubSystem::InputManager::Keys::Q=               0x10;
int AngelCore::AngelSubSystem::InputManager::Keys::W=               0x11;
int AngelCore::AngelSubSystem::InputManager::Keys::E=               0x12;
int AngelCore::AngelSubSystem::InputManager::Keys::R=               0x13;
int AngelCore::AngelSubSystem::InputManager::Keys::T=               0x14;
int AngelCore::AngelSubSystem::InputManager::Keys::Y=               0x15;
int AngelCore::AngelSubSystem::InputManager::Keys::U=               0x16;
int AngelCore::AngelSubSystem::InputManager::Keys::I=               0x17;
int AngelCore::AngelSubSystem::InputManager::Keys::O=               0x18;
int AngelCore::AngelSubSystem::InputManager::Keys::P=               0x19;
int AngelCore::AngelSubSystem::InputManager::Keys::LBRACKET=        0x1A;
int AngelCore::AngelSubSystem::InputManager::Keys::RBRACKET=        0x1B;
int AngelCore::AngelSubSystem::InputManager::Keys::RETURN=          0x1C;    /* Enter on main keyboard */
int AngelCore::AngelSubSystem::InputManager::Keys::LCONTROL=        0x1D;
int AngelCore::AngelSubSystem::InputManager::Keys::S=               0x1F;
int AngelCore::AngelSubSystem::InputManager::Keys::D=               0x20;
int AngelCore::AngelSubSystem::InputManager::Keys::F=               0x21;
int AngelCore::AngelSubSystem::InputManager::Keys::G=               0x22;
int AngelCore::AngelSubSystem::InputManager::Keys::H=               0x23;
int AngelCore::AngelSubSystem::InputManager::Keys::J=               0x24;
int AngelCore::AngelSubSystem::InputManager::Keys::K=               0x25;
int AngelCore::AngelSubSystem::InputManager::Keys::L=               0x26;
int AngelCore::AngelSubSystem::InputManager::Keys::SEMICOLON=       0x27;
int AngelCore::AngelSubSystem::InputManager::Keys::APOSTROPHE=      0x28;
int AngelCore::AngelSubSystem::InputManager::Keys::GRAVE=           0x29;    /* accent grave */
int AngelCore::AngelSubSystem::InputManager::Keys::LSHIFT=          0x2A;
int AngelCore::AngelSubSystem::InputManager::Keys::BACKSLASH =      0x2B;
int AngelCore::AngelSubSystem::InputManager::Keys::Z=               0x2C;
int AngelCore::AngelSubSystem::InputManager::Keys::X=               0x2D;
int AngelCore::AngelSubSystem::InputManager::Keys::C=               0x2E;
int AngelCore::AngelSubSystem::InputManager::Keys::V=               0x2F;
int AngelCore::AngelSubSystem::InputManager::Keys::B=               0x30;
int AngelCore::AngelSubSystem::InputManager::Keys::N=               0x31;
int AngelCore::AngelSubSystem::InputManager::Keys::M=               0x32;
int AngelCore::AngelSubSystem::InputManager::Keys::COMMA=           0x33;
int AngelCore::AngelSubSystem::InputManager::Keys::PERIOD =         0x34;    /* . on main keyboard */
int AngelCore::AngelSubSystem::InputManager::Keys::SLASH=           0x35;    /* / on main keyboard */
int AngelCore::AngelSubSystem::InputManager::Keys::RSHIFT=          0x36;
int AngelCore::AngelSubSystem::InputManager::Keys::MULTIPLY =       0x37;    /* * on numeric keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::LMENU=           0x38;    /* left Alt */
int AngelCore::AngelSubSystem::InputManager::Keys::SPACE  =         0x39;
int AngelCore::AngelSubSystem::InputManager::Keys::CAPITAL =        0x3A;
int AngelCore::AngelSubSystem::InputManager::Keys::F1=              0x3B;
int AngelCore::AngelSubSystem::InputManager::Keys::F2=              0x3C;
int AngelCore::AngelSubSystem::InputManager::Keys::F3=              0x3D;
int AngelCore::AngelSubSystem::InputManager::Keys::F4=              0x3E;
int AngelCore::AngelSubSystem::InputManager::Keys::F5=              0x3F;
int AngelCore::AngelSubSystem::InputManager::Keys::F6=              0x40;
int AngelCore::AngelSubSystem::InputManager::Keys::F7=              0x41;
int AngelCore::AngelSubSystem::InputManager::Keys::F8=              0x42;
int AngelCore::AngelSubSystem::InputManager::Keys::F9=              0x43;
int AngelCore::AngelSubSystem::InputManager::Keys::F10=             0x44	;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMLOCK =        0x45;
int AngelCore::AngelSubSystem::InputManager::Keys::SCROLL=          0x46;    /* Scroll Lock */
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD7=         0x47;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD8=         0x48;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD9 =        0x49;
int AngelCore::AngelSubSystem::InputManager::Keys::SUBTRACT=        0x4A;    /* - on numeric keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD4 =        0x4B;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD5 =        0x4C;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD6 =        0x4D;
int AngelCore::AngelSubSystem::InputManager::Keys::ADD   =          0x4E;    /* + on numeric keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD1 =        0x4F;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD2  =       0x50;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD3  =       0x51;
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPAD0  =       0x52;
int AngelCore::AngelSubSystem::InputManager::Keys::DECIMAL  =       0x53;    /* . on numeric keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::OEM_102  =       0x56;    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
int AngelCore::AngelSubSystem::InputManager::Keys::F11 =            0x57;
int AngelCore::AngelSubSystem::InputManager::Keys::F12 =            0x58;
int AngelCore::AngelSubSystem::InputManager::Keys::F13 =            0x64;    /*                     (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::F14 =            0x65;    /*                     (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::F15 =            0x66;    /*                     (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::KANA =           0x70;    /* (Japanese keyboard)            */
int AngelCore::AngelSubSystem::InputManager::Keys::ABNT_C1=         0x73;    /* /? on Brazilian keyboard */
int AngelCore::AngelSubSystem::InputManager::Keys::CONVERT =        0x79;    /* (Japanese keyboard)            */
int AngelCore::AngelSubSystem::InputManager::Keys::NOCONVERT  =     0x7B;    /* (Japanese keyboard)            */
int AngelCore::AngelSubSystem::InputManager::Keys::YEN =            0x7D;    /* (Japanese keyboard)            */
int AngelCore::AngelSubSystem::InputManager::Keys::ABNT_C2=         0x7E;    /* Numpad . on Brazilian keyboard */
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPADEQUALS=    0x8D;    /* = on numeric keypad (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::PREVTRACK=       0x90;    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
int AngelCore::AngelSubSystem::InputManager::Keys::AT =             0x91;    /*                     (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::COLON  =         0x92;    /*                     (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::UNDERLINE =      0x93;    /*                     (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::KANJI =          0x94;    /* (Japanese keyboard)            */
int AngelCore::AngelSubSystem::InputManager::Keys::STOP  =          0x95;    /*                     (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::AX    =          0x96;    /*                     (Japan AX) */
int AngelCore::AngelSubSystem::InputManager::Keys::UNLABELED =      0x97;    /*                        (J3100) */
int AngelCore::AngelSubSystem::InputManager::Keys::NEXTTRACK =      0x99;    /* Next Track */
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPADENTER =    0x9C;    /* Enter on numeric keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::RCONTROL =       0x9D;
int AngelCore::AngelSubSystem::InputManager::Keys::MUTE  =          0xA0;    /* Mute */
int AngelCore::AngelSubSystem::InputManager::Keys::CALCULATOR =     0xA1;    /* Calculator */
int AngelCore::AngelSubSystem::InputManager::Keys::PLAYPAUSE =      0xA2;    /* Play / Pause */
int AngelCore::AngelSubSystem::InputManager::Keys::MEDIASTOP =      0xA4;    /* Media Stop */
int AngelCore::AngelSubSystem::InputManager::Keys::VOLUMEDOWN=      0xAE;    /* Volume - */
int AngelCore::AngelSubSystem::InputManager::Keys::VOLUMEUP=        0xB0;    /* Volume + */
int AngelCore::AngelSubSystem::InputManager::Keys::WEBHOME  =       0xB2;    /* Web home */
int AngelCore::AngelSubSystem::InputManager::Keys::NUMPADCOMMA  =   0xB3;    /* , on numeric keypad (NEC PC98) */
int AngelCore::AngelSubSystem::InputManager::Keys::DIVIDE =         0xB5;    /* / on numeric keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::SYSRQ =          0xB7;
int AngelCore::AngelSubSystem::InputManager::Keys::RMENU  =         0xB8;    /* right Alt */
int AngelCore::AngelSubSystem::InputManager::Keys::PAUSE =          0xC5;    /* Pause */
int AngelCore::AngelSubSystem::InputManager::Keys::HOME =           0xC7;    /* Home on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::UP   =           0xC8;    /* UpArrow on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::PRIOR  =         0xC9;    /* PgUp on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::LEFT  =          0xCB;    /* LeftArrow on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::RIGHT  =         0xCD;    /* RightArrow on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::END  =           0xCF;    /* End on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::DOWN  =          0xD0;    /* DownArrow on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::NEXT  =          0xD1;    /* PgDn on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::INSERT=          0xD2;    /* Insert on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::DELETEKey =         0xD3;    /* Delete on arrow keypad */
int AngelCore::AngelSubSystem::InputManager::Keys::LWIN  =          0xDB;    /* Left Windows key */
int AngelCore::AngelSubSystem::InputManager::Keys::RWIN =           0xDC;    /* Right Windows key */
int AngelCore::AngelSubSystem::InputManager::Keys::APPS =           0xDD;    /* AppMenu key */
int AngelCore::AngelSubSystem::InputManager::Keys::POWER  =         0xDE;    /* System Power */
int AngelCore::AngelSubSystem::InputManager::Keys::SLEEP  =         0xDF;    /* System Sleep */
int AngelCore::AngelSubSystem::InputManager::Keys::WAKE     =       0xE3;    /* System Wake */
int AngelCore::AngelSubSystem::InputManager::Keys::WEBSEARCH =      0xE5;    /* Web Search */
int AngelCore::AngelSubSystem::InputManager::Keys::WEBFAVORITES=    0xE6;    /* Web Favorites */
int AngelCore::AngelSubSystem::InputManager::Keys::WEBREFRESH=      0xE7;    /* Web Refresh */
int AngelCore::AngelSubSystem::InputManager::Keys::WEBSTOP =        0xE8;    /* Web Stop */
int AngelCore::AngelSubSystem::InputManager::Keys::WEBFORWARD=      0xE9;    /* Web Forward */
int AngelCore::AngelSubSystem::InputManager::Keys::WEBBACK  =       0xEA;    /* Web Back */
int AngelCore::AngelSubSystem::InputManager::Keys::MYCOMPUTER =     0xEB;    /* My Computer */
int AngelCore::AngelSubSystem::InputManager::Keys::MAIL  =          0xEC;    /* Mail */
int AngelCore::AngelSubSystem::InputManager::Keys::MEDIASELECT  =   0xED;    /* Media Select */

bool AngelCore::AngelSubSystem::InputManager::StartUp(...)
{
	HRESULT hr = DirectInput8Create(AngelSubSystemResources::WindowProperties::GetHINSTANCE(),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&this->m_DirectInput, NULL);

	hr = this->m_DirectInput->CreateDevice(GUID_SysMouse, &this->m_DIMouse, NULL);

	hr = this->m_DirectInput->CreateDevice(GUID_SysKeyboard, &this->m_DIKeyboard, NULL);

	hr = m_DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_DIKeyboard->SetCooperativeLevel(AngelSubSystemResources::WindowProperties::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = m_DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = m_DIMouse->SetCooperativeLevel(AngelSubSystemResources::WindowProperties::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	this->m_mousePosition.x = 0;
	this->m_mousePosition.y = 0;
	this->m_isRightClicked = false;
	this->m_leftClicked = false;
	return true;

}

bool AngelCore::AngelSubSystem::InputManager::ShutDown()
{
	m_DIKeyboard->Unacquire();
	m_DIMouse->Unacquire();
	m_DirectInput->Release();
	return true;
}

void AngelCore::AngelSubSystem::InputManager::DetectInput()
{
	//DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];
	ZeroMemory(keyboardState, sizeof(keyboardState));

	m_DIKeyboard->Acquire();
	//m_DIMouse->Acquire();

	//m_DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	m_DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	for (unsigned int i = 0; i < 256;i++)
	{
		this->keys[i] = keyboardState[i];
		if (keysPressed[i])
		{
			if (!this->keys[i])
				keysPressed[i] = false;
		}

		if (this->keys[i])
		{
			heldCounter[i]++;
		}

		if (!this->keys[i])
		{
			heldCounter[i] = 0;
		}
	}
	//Store mouse offset.
	m_mouseOffset.x =(m_mousePosition.x - m_lMousePosition.x);
	m_mouseOffset.y =(m_mousePosition.y - m_lMousePosition.y);

	m_lMousePosition = m_mousePosition;
}

bool AngelCore::AngelSubSystem::InputManager::IsKeyDown(int key)
{
	return this->keys[key] == true;
}

bool AngelCore::AngelSubSystem::InputManager::IsKeyUp(int key)
{
	return this->keys[key] == false;
}

bool AngelCore::AngelSubSystem::InputManager::IsKeyHeld(int key)
{
	return this->heldCounter[key] > 25 ? true : false;
}

bool AngelCore::AngelSubSystem::InputManager::IsAnyKeyPressed()
{
	for (int i = 0;i < 256;i++)
	{
		if (keys[i]) return true;
	}
	return false;
}

bool AngelCore::AngelSubSystem::InputManager::IsKeyPressed(int key)
{
	if (keys[key])
	{
		if (!keysPressed[key])
		{
			keysPressed[key] = true;
			return true;
		}
	}
	return false;
}

bool AngelCore::AngelSubSystem::InputManager::IsKeyPressed(std::string &key)
{
	bool ret = false;
	if (_strcmpi(key.c_str(), "A") == 0)
		ret = IsKeyPressed(0x1E);
	if (_strcmpi(key.c_str(),  "1") == 0)
		ret = IsKeyPressed(0x02);
	if (_strcmpi(key.c_str(),  "2") == 0)
		ret = IsKeyPressed(0x03);
	if (_strcmpi(key.c_str(),  "3") == 0)
		ret = IsKeyPressed(0x04);
	if (_strcmpi(key.c_str(),  "4") == 0)
		ret = IsKeyPressed(0x05);
	if (_strcmpi(key.c_str(),  "5") == 0)
		ret = IsKeyPressed(0x06);
	if (_strcmpi(key.c_str(),  "6") == 0)
		ret = IsKeyPressed(0x07);
	if (_strcmpi(key.c_str(),  "7") == 0)
		ret = IsKeyPressed(0x08);
	if (_strcmpi(key.c_str(),  "8") == 0)
		ret = IsKeyPressed(0x09);
	if (_strcmpi(key.c_str(),  "9") == 0)
		ret = IsKeyPressed(0x0A);
	if (_strcmpi(key.c_str(),  "0") == 0)
		ret = IsKeyPressed(0x0B);
	if (_strcmpi(key.c_str(),  "-") == 0)
		ret = IsKeyPressed(0x0C);
	if (_strcmpi(key.c_str(),  "=") == 0)
		ret = IsKeyPressed(0x0D);
	if (_strcmpi(key.c_str(),  "Back") == 0)
		ret = IsKeyPressed(0x0E);
	if (_strcmpi(key.c_str(),  "Tab") == 0)
		ret = IsKeyPressed(0x0F);
	if (_strcmpi(key.c_str(),  "Q") == 0)
		ret = IsKeyPressed(0x10);
	if (_strcmpi(key.c_str(),  "W") == 0)
		ret = IsKeyPressed(0x11);
	if (_strcmpi(key.c_str(),  "E") == 0)
		ret = IsKeyPressed(0x12);
	if (_strcmpi(key.c_str(),  "R") == 0)
		ret = IsKeyPressed(0x13);
	if (_strcmpi(key.c_str(),  "T") == 0)
		ret = IsKeyPressed(0x14);
	if (_strcmpi(key.c_str(),  "Y") == 0)
		ret = IsKeyPressed(0x15);
	if (_strcmpi(key.c_str(),  "U") == 0)
		ret = IsKeyPressed(0x16);
	if (_strcmpi(key.c_str(),  "I") == 0)
		ret = IsKeyPressed(0x17);
	if (_strcmpi(key.c_str(),  "O") == 0)
		ret = IsKeyPressed(0x18);
	if (_strcmpi(key.c_str(),  "P") == 0)
		ret = IsKeyPressed(0x19);
	if (_strcmpi(key.c_str(),  "[") == 0)
		ret = IsKeyPressed(0x1A);
	if (_strcmpi(key.c_str(),  "]") == 0)
		ret = IsKeyPressed(0x1B);
	if (_strcmpi(key.c_str(),  "Enter") == 0)
		ret = IsKeyPressed(0x1C);
	if (_strcmpi(key.c_str(),  "LCtrl") == 0)
		ret = IsKeyPressed(0x1D);
	if (_strcmpi(key.c_str(),  "S") == 0)
		ret = IsKeyPressed(0x1F);
	if (_strcmpi(key.c_str(),  "D") == 0)
		ret = IsKeyPressed(0x20);
	if (_strcmpi(key.c_str(),  "F") == 0)
		ret = IsKeyPressed(0x21);
	if (_strcmpi(key.c_str(),  "G") == 0)
		ret = IsKeyPressed(0x22);
	if (_strcmpi(key.c_str(),  "H") == 0)
		ret = IsKeyPressed(0x23);
	if (_strcmpi(key.c_str(),  "J") == 0)
		ret = IsKeyPressed(0x24);
	if (_strcmpi(key.c_str(),  "K") == 0)
		ret = IsKeyPressed(0x25);
	if (_strcmpi(key.c_str(),  "L") == 0)
		ret = IsKeyPressed(0x26);
	if (_strcmpi(key.c_str(),  ";") == 0)
		ret = IsKeyPressed(0x27);
	if (_strcmpi(key.c_str(),  "'") == 0)
		ret = IsKeyPressed(0x28);
	if (_strcmpi(key.c_str(),  "|") == 0)
		ret = IsKeyPressed(0x29);
	if (_strcmpi(key.c_str(),  "LShift") == 0)
		ret = IsKeyPressed(0x2A);
	if (_strcmpi(key.c_str(),  "\\") == 0)
		ret = IsKeyPressed(0x2B);
	if (_strcmpi(key.c_str(),  "Z") == 0)
		ret = IsKeyPressed(0x2C);
	if (_strcmpi(key.c_str(),  "X") == 0)
		ret = IsKeyPressed(0x2D);
	if (_strcmpi(key.c_str(),  "C") == 0)
		ret = IsKeyPressed(0x2E);
	if (_strcmpi(key.c_str(),  "V") == 0)
		ret = IsKeyPressed(0x2F);
	if (_strcmpi(key.c_str(),  "B") == 0)
		ret = IsKeyPressed(0x30);
	if (_strcmpi(key.c_str(),  "N") == 0)
		ret = IsKeyPressed(0x31);
	if (_strcmpi(key.c_str(),  "M") == 0)
		ret = IsKeyPressed(0x32);
	if (_strcmpi(key.c_str(),  ",") == 0)
		ret = IsKeyPressed(0x33);
	if (_strcmpi(key.c_str(),  ".") == 0)
		ret = IsKeyPressed(0x34);
	if (_strcmpi(key.c_str(),  "/") == 0)
		ret = IsKeyPressed(0x35);
	if (_strcmpi(key.c_str(),  "RShift") == 0)
		ret = IsKeyPressed(0x36);
	if (_strcmpi(key.c_str(),  "*") == 0)
		ret = IsKeyPressed(0x37);
	if (_strcmpi(key.c_str(),  "LAlt") == 0)
		ret = IsKeyPressed(0x38);
	if (_strcmpi(key.c_str(),  "Space") == 0)
		ret = IsKeyPressed(0x39);
	if (_strcmpi(key.c_str(),  "Capital") == 0)
		ret = IsKeyPressed(0x3A);
	if (_strcmpi(key.c_str(),  "F1") == 0)
		ret = IsKeyPressed(0x3B);
	if (_strcmpi(key.c_str(),  "F2") == 0)
		ret = IsKeyPressed(0x3C);
	if (_strcmpi(key.c_str(),  "F3") == 0)
		ret = IsKeyPressed(0x3D);
	if (_strcmpi(key.c_str(),  "F4") == 0)
		ret = IsKeyPressed(0x3E);
	if (_strcmpi(key.c_str(),  "F5") == 0)
		ret = IsKeyPressed(0x3F);
	if (_strcmpi(key.c_str(),  "F6") == 0)
		ret = IsKeyPressed(0x40);
	if (_strcmpi(key.c_str(),  "F7") == 0)
		ret = IsKeyPressed(0x41);
	if (_strcmpi(key.c_str(),  "F8") == 0)
		ret = IsKeyPressed(0x42);
	if (_strcmpi(key.c_str(),  "F9") == 0)
		ret = IsKeyPressed(0x43);
	if (_strcmpi(key.c_str(),  "F10") == 0)
		ret = IsKeyPressed(0x44);
	if (_strcmpi(key.c_str(),  "NumLock") == 0)
		ret = IsKeyPressed(0x45);
	if (_strcmpi(key.c_str(),  "ScrollLock") == 0)
		ret = IsKeyPressed(0x46);
	if (_strcmpi(key.c_str(),  "NumPad7") == 0)
		ret = IsKeyPressed(0x47);
	if (_strcmpi(key.c_str(),  "NumPad8") == 0)
		ret = IsKeyPressed(0x48);
	if (_strcmpi(key.c_str(),  "NumPad9") == 0)
		ret = IsKeyPressed(0x49);
	if (_strcmpi(key.c_str(),  "NumPad-") == 0)
		ret = IsKeyPressed(0x4A);
	if (_strcmpi(key.c_str(),  "NumPad4") == 0)
		ret = IsKeyPressed(0x4B);
	if (_strcmpi(key.c_str(),  "NumPad5") == 0)
		ret = IsKeyPressed(0x4C);
	if (_strcmpi(key.c_str(),  "NumPad6") == 0)
		ret = IsKeyPressed(0x4D);
	if (_strcmpi(key.c_str(),  "NumPad+") == 0)
		ret = IsKeyPressed(0x4E);
	if (_strcmpi(key.c_str(),  "NumPad1") == 0)
		ret = IsKeyPressed(0x4F);
	if (_strcmpi(key.c_str(),  "NumPad2") == 0)
		ret = IsKeyPressed(0x50);
	if (_strcmpi(key.c_str(),  "NumPad3") == 0)
		ret = IsKeyPressed(0x51);
	if (_strcmpi(key.c_str(),  "NumPad0") == 0)
		ret = IsKeyPressed(0x52);
	if (_strcmpi(key.c_str(),  "NumPad.") == 0)
		ret = IsKeyPressed(0x53);
	if (_strcmpi(key.c_str(),  "F11") == 0)
		ret = IsKeyPressed(0x57);
	if (_strcmpi(key.c_str(),  "F12") == 0)
		ret = IsKeyPressed(0x58);
	if (_strcmpi(key.c_str(),  "PrevTrack") == 0)
		ret = IsKeyPressed(0x90);
	if (_strcmpi(key.c_str(),  "NumPadEnter") == 0)
		ret = IsKeyPressed(0x9C);
	if (_strcmpi(key.c_str(),  "RCtrl") == 0)
		ret = IsKeyPressed(0x9D);
	if (_strcmpi(key.c_str(),  "Mute") == 0)
		ret = IsKeyPressed(0xA0);
	if (_strcmpi(key.c_str(),  "NumPad/") == 0)
		ret = IsKeyPressed(0xB5);
	if (_strcmpi(key.c_str(),  "RAlt") == 0)
		ret = IsKeyPressed(0xB8);
	if (_strcmpi(key.c_str(),  "Pause") == 0)
		ret = IsKeyPressed(0xC5);
	if (_strcmpi(key.c_str(),  "Home") == 0)
		ret = IsKeyPressed(0xC7);
	if (_strcmpi(key.c_str(),  "Up") == 0)
		ret = IsKeyPressed(0xC8);
	if (_strcmpi(key.c_str(),  "PageDown") == 0)
		ret = IsKeyPressed(0xC9);
	if (_strcmpi(key.c_str(),  "Left") == 0)
		ret = IsKeyPressed(0xCB);
	if (_strcmpi(key.c_str(),  "Right") == 0)
		ret = IsKeyPressed(0xCD);
	if (_strcmpi(key.c_str(),  "End") == 0)
		ret = IsKeyPressed(0xCF);
	if (_strcmpi(key.c_str(),  "Down") == 0)
		ret = IsKeyPressed(0xD0);
	if (_strcmpi(key.c_str(),  "Insert") == 0)
		ret = IsKeyPressed(0xD2);
	if (_strcmpi(key.c_str(),  "Delete") == 0)
		ret = IsKeyPressed(0xD3);
	if (_strcmpi(key.c_str(),  "LWin") == 0)
		ret = IsKeyPressed(0xDB);
	if (_strcmpi(key.c_str(),  "RWin") == 0)
		ret = IsKeyPressed(0xDC);
	return ret;
}
bool AngelCore::AngelSubSystem::InputManager::IsKeyUp(std::string &key)
{
	bool ret = false;
	if (_strcmpi(key.c_str(), "A") == 0)
		ret = IsKeyUp(0x1E);
	if (_strcmpi(key.c_str(), "1") == 0)
		ret = IsKeyUp(0x02);
	if (_strcmpi(key.c_str(), "2") == 0)
		ret = IsKeyUp(0x03);
	if (_strcmpi(key.c_str(), "3") == 0)
		ret = IsKeyUp(0x04);
	if (_strcmpi(key.c_str(), "4") == 0)
		ret = IsKeyUp(0x05);
	if (_strcmpi(key.c_str(), "5") == 0)
		ret = IsKeyUp(0x06);
	if (_strcmpi(key.c_str(), "6") == 0)
		ret = IsKeyUp(0x07);
	if (_strcmpi(key.c_str(), "7") == 0)
		ret = IsKeyUp(0x08);
	if (_strcmpi(key.c_str(), "8") == 0)
		ret = IsKeyUp(0x09);
	if (_strcmpi(key.c_str(), "9") == 0)
		ret = IsKeyUp(0x0A);
	if (_strcmpi(key.c_str(), "0") == 0)
		ret = IsKeyUp(0x0B);
	if (_strcmpi(key.c_str(), "-") == 0)
		ret = IsKeyUp(0x0C);
	if (_strcmpi(key.c_str(), "=") == 0)
		ret = IsKeyUp(0x0D);
	if (_strcmpi(key.c_str(), "Back") == 0)
		ret = IsKeyUp(0x0E);
	if (_strcmpi(key.c_str(), "Tab") == 0)
		ret = IsKeyUp(0x0F);
	if (_strcmpi(key.c_str(), "Q") == 0)
		ret = IsKeyUp(0x10);
	if (_strcmpi(key.c_str(), "W") == 0)
		ret = IsKeyUp(0x11);
	if (_strcmpi(key.c_str(), "E") == 0)
		ret = IsKeyUp(0x12);
	if (_strcmpi(key.c_str(), "R") == 0)
		ret = IsKeyUp(0x13);
	if (_strcmpi(key.c_str(), "T") == 0)
		ret = IsKeyUp(0x14);
	if (_strcmpi(key.c_str(), "Y") == 0)
		ret = IsKeyUp(0x15);
	if (_strcmpi(key.c_str(), "U") == 0)
		ret = IsKeyUp(0x16);
	if (_strcmpi(key.c_str(), "I") == 0)
		ret = IsKeyUp(0x17);
	if (_strcmpi(key.c_str(), "O") == 0)
		ret = IsKeyUp(0x18);
	if (_strcmpi(key.c_str(), "P") == 0)
		ret = IsKeyUp(0x19);
	if (_strcmpi(key.c_str(), "[") == 0)
		ret = IsKeyUp(0x1A);
	if (_strcmpi(key.c_str(), "]") == 0)
		ret = IsKeyUp(0x1B);
	if (_strcmpi(key.c_str(), "Enter") == 0)
		ret = IsKeyUp(0x1C);
	if (_strcmpi(key.c_str(), "LCtrl") == 0)
		ret = IsKeyUp(0x1D);
	if (_strcmpi(key.c_str(), "S") == 0)
		ret = IsKeyUp(0x1F);
	if (_strcmpi(key.c_str(), "D") == 0)
		ret = IsKeyUp(0x20);
	if (_strcmpi(key.c_str(), "F") == 0)
		ret = IsKeyUp(0x21);
	if (_strcmpi(key.c_str(), "G") == 0)
		ret = IsKeyUp(0x22);
	if (_strcmpi(key.c_str(), "H") == 0)
		ret = IsKeyUp(0x23);
	if (_strcmpi(key.c_str(), "J") == 0)
		ret = IsKeyUp(0x24);
	if (_strcmpi(key.c_str(), "K") == 0)
		ret = IsKeyUp(0x25);
	if (_strcmpi(key.c_str(), "L") == 0)
		ret = IsKeyUp(0x26);
	if (_strcmpi(key.c_str(), ";") == 0)
		ret = IsKeyUp(0x27);
	if (_strcmpi(key.c_str(), "'") == 0)
		ret = IsKeyUp(0x28);
	if (_strcmpi(key.c_str(), "|") == 0)
		ret = IsKeyUp(0x29);
	if (_strcmpi(key.c_str(), "LShift") == 0)
		ret = IsKeyUp(0x2A);
	if (_strcmpi(key.c_str(), "\\") == 0)
		ret = IsKeyUp(0x2B);
	if (_strcmpi(key.c_str(), "Z") == 0)
		ret = IsKeyUp(0x2C);
	if (_strcmpi(key.c_str(), "X") == 0)
		ret = IsKeyUp(0x2D);
	if (_strcmpi(key.c_str(), "C") == 0)
		ret = IsKeyUp(0x2E);
	if (_strcmpi(key.c_str(), "V") == 0)
		ret = IsKeyUp(0x2F);
	if (_strcmpi(key.c_str(), "B") == 0)
		ret = IsKeyUp(0x30);
	if (_strcmpi(key.c_str(), "N") == 0)
		ret = IsKeyUp(0x31);
	if (_strcmpi(key.c_str(), "M") == 0)
		ret = IsKeyUp(0x32);
	if (_strcmpi(key.c_str(), ",") == 0)
		ret = IsKeyUp(0x33);
	if (_strcmpi(key.c_str(), ".") == 0)
		ret = IsKeyUp(0x34);
	if (_strcmpi(key.c_str(), "/") == 0)
		ret = IsKeyUp(0x35);
	if (_strcmpi(key.c_str(), "RShift") == 0)
		ret = IsKeyUp(0x36);
	if (_strcmpi(key.c_str(), "*") == 0)
		ret = IsKeyUp(0x37);
	if (_strcmpi(key.c_str(), "LAlt") == 0)
		ret = IsKeyUp(0x38);
	if (_strcmpi(key.c_str(), "Space") == 0)
		ret = IsKeyUp(0x39);
	if (_strcmpi(key.c_str(), "Capital") == 0)
		ret = IsKeyUp(0x3A);
	if (_strcmpi(key.c_str(), "F1") == 0)
		ret = IsKeyUp(0x3B);
	if (_strcmpi(key.c_str(), "F2") == 0)
		ret = IsKeyUp(0x3C);
	if (_strcmpi(key.c_str(), "F3") == 0)
		ret = IsKeyUp(0x3D);
	if (_strcmpi(key.c_str(), "F4") == 0)
		ret = IsKeyUp(0x3E);
	if (_strcmpi(key.c_str(), "F5") == 0)
		ret = IsKeyUp(0x3F);
	if (_strcmpi(key.c_str(), "F6") == 0)
		ret = IsKeyUp(0x40);
	if (_strcmpi(key.c_str(), "F7") == 0)
		ret = IsKeyUp(0x41);
	if (_strcmpi(key.c_str(), "F8") == 0)
		ret = IsKeyUp(0x42);
	if (_strcmpi(key.c_str(), "F9") == 0)
		ret = IsKeyUp(0x43);
	if (_strcmpi(key.c_str(), "F10") == 0)
		ret = IsKeyUp(0x44);
	if (_strcmpi(key.c_str(), "NumLock") == 0)
		ret = IsKeyUp(0x45);
	if (_strcmpi(key.c_str(), "ScrollLock") == 0)
		ret = IsKeyUp(0x46);
	if (_strcmpi(key.c_str(), "NumPad7") == 0)
		ret = IsKeyUp(0x47);
	if (_strcmpi(key.c_str(), "NumPad8") == 0)
		ret = IsKeyUp(0x48);
	if (_strcmpi(key.c_str(), "NumPad9") == 0)
		ret = IsKeyUp(0x49);
	if (_strcmpi(key.c_str(), "NumPad-") == 0)
		ret = IsKeyUp(0x4A);
	if (_strcmpi(key.c_str(), "NumPad4") == 0)
		ret = IsKeyUp(0x4B);
	if (_strcmpi(key.c_str(), "NumPad5") == 0)
		ret = IsKeyUp(0x4C);
	if (_strcmpi(key.c_str(), "NumPad6") == 0)
		ret = IsKeyUp(0x4D);
	if (_strcmpi(key.c_str(), "NumPad+") == 0)
		ret = IsKeyUp(0x4E);
	if (_strcmpi(key.c_str(), "NumPad1") == 0)
		ret = IsKeyUp(0x4F);
	if (_strcmpi(key.c_str(), "NumPad2") == 0)
		ret = IsKeyUp(0x50);
	if (_strcmpi(key.c_str(), "NumPad3") == 0)
		ret = IsKeyUp(0x51);
	if (_strcmpi(key.c_str(), "NumPad0") == 0)
		ret = IsKeyUp(0x52);
	if (_strcmpi(key.c_str(), "NumPad.") == 0)
		ret = IsKeyUp(0x53);
	if (_strcmpi(key.c_str(), "F11") == 0)
		ret = IsKeyUp(0x57);
	if (_strcmpi(key.c_str(), "F12") == 0)
		ret = IsKeyUp(0x58);
	if (_strcmpi(key.c_str(), "PrevTrack") == 0)
		ret = IsKeyUp(0x90);
	if (_strcmpi(key.c_str(), "NumPadEnter") == 0)
		ret = IsKeyUp(0x9C);
	if (_strcmpi(key.c_str(), "RCtrl") == 0)
		ret = IsKeyUp(0x9D);
	if (_strcmpi(key.c_str(), "Mute") == 0)
		ret = IsKeyUp(0xA0);
	if (_strcmpi(key.c_str(), "NumPad/") == 0)
		ret = IsKeyUp(0xB5);
	if (_strcmpi(key.c_str(), "RAlt") == 0)
		ret = IsKeyUp(0xB8);
	if (_strcmpi(key.c_str(), "Pause") == 0)
		ret = IsKeyUp(0xC5);
	if (_strcmpi(key.c_str(), "Home") == 0)
		ret = IsKeyUp(0xC7);
	if (_strcmpi(key.c_str(), "Up") == 0)
		ret = IsKeyUp(0xC8);
	if (_strcmpi(key.c_str(), "PageDown") == 0)
		ret = IsKeyUp(0xC9);
	if (_strcmpi(key.c_str(), "Left") == 0)
		ret = IsKeyUp(0xCB);
	if (_strcmpi(key.c_str(), "Right") == 0)
		ret = IsKeyUp(0xCD);
	if (_strcmpi(key.c_str(), "End") == 0)
		ret = IsKeyUp(0xCF);
	if (_strcmpi(key.c_str(), "Down") == 0)
		ret = IsKeyUp(0xD0);
	if (_strcmpi(key.c_str(), "Insert") == 0)
		ret = IsKeyUp(0xD2);
	if (_strcmpi(key.c_str(), "Delete") == 0)
		ret = IsKeyUp(0xD3);
	if (_strcmpi(key.c_str(), "LWin") == 0)
		ret = IsKeyUp(0xDB);
	if (_strcmpi(key.c_str(), "RWin") == 0)
		ret = IsKeyUp(0xDC);
	return ret;
}
bool AngelCore::AngelSubSystem::InputManager::IsKeyDown(std::string &key)
{
	bool ret = false;
	if (_strcmpi(key.c_str(), "A") == 0)
		ret = IsKeyDown(0x1E);
	if (_strcmpi(key.c_str(), "1") == 0)
		ret = IsKeyDown(0x02);
	if (_strcmpi(key.c_str(), "2") == 0)
		ret = IsKeyDown(0x03);
	if (_strcmpi(key.c_str(), "3") == 0)
		ret = IsKeyDown(0x04);
	if (_strcmpi(key.c_str(), "4") == 0)
		ret = IsKeyDown(0x05);
	if (_strcmpi(key.c_str(), "5") == 0)
		ret = IsKeyDown(0x06);
	if (_strcmpi(key.c_str(), "6") == 0)
		ret = IsKeyDown(0x07);
	if (_strcmpi(key.c_str(), "7") == 0)
		ret = IsKeyDown(0x08);
	if (_strcmpi(key.c_str(), "8") == 0)
		ret = IsKeyDown(0x09);
	if (_strcmpi(key.c_str(), "9") == 0)
		ret = IsKeyDown(0x0A);
	if (_strcmpi(key.c_str(), "0") == 0)
		ret = IsKeyDown(0x0B);
	if (_strcmpi(key.c_str(), "-") == 0)
		ret = IsKeyDown(0x0C);
	if (_strcmpi(key.c_str(), "=") == 0)
		ret = IsKeyDown(0x0D);
	if (_strcmpi(key.c_str(), "Back") == 0)
		ret = IsKeyDown(0x0E);
	if (_strcmpi(key.c_str(), "Tab") == 0)
		ret = IsKeyDown(0x0F);
	if (_strcmpi(key.c_str(), "Q") == 0)
		ret = IsKeyDown(0x10);
	if (_strcmpi(key.c_str(), "W") == 0)
		ret = IsKeyDown(0x11);
	if (_strcmpi(key.c_str(), "E") == 0)
		ret = IsKeyDown(0x12);
	if (_strcmpi(key.c_str(), "R") == 0)
		ret = IsKeyDown(0x13);
	if (_strcmpi(key.c_str(), "T") == 0)
		ret = IsKeyDown(0x14);
	if (_strcmpi(key.c_str(), "Y") == 0)
		ret = IsKeyDown(0x15);
	if (_strcmpi(key.c_str(), "U") == 0)
		ret = IsKeyDown(0x16);
	if (_strcmpi(key.c_str(), "I") == 0)
		ret = IsKeyDown(0x17);
	if (_strcmpi(key.c_str(), "O") == 0)
		ret = IsKeyDown(0x18);
	if (_strcmpi(key.c_str(), "P") == 0)
		ret = IsKeyDown(0x19);
	if (_strcmpi(key.c_str(), "[") == 0)
		ret = IsKeyDown(0x1A);
	if (_strcmpi(key.c_str(), "]") == 0)
		ret = IsKeyDown(0x1B);
	if (_strcmpi(key.c_str(), "Enter") == 0)
		ret = IsKeyDown(0x1C);
	if (_strcmpi(key.c_str(), "LCtrl") == 0)
		ret = IsKeyDown(0x1D);
	if (_strcmpi(key.c_str(), "S") == 0)
		ret = IsKeyDown(0x1F);
	if (_strcmpi(key.c_str(), "D") == 0)
		ret = IsKeyDown(0x20);
	if (_strcmpi(key.c_str(), "F") == 0)
		ret = IsKeyDown(0x21);
	if (_strcmpi(key.c_str(), "G") == 0)
		ret = IsKeyDown(0x22);
	if (_strcmpi(key.c_str(), "H") == 0)
		ret = IsKeyDown(0x23);
	if (_strcmpi(key.c_str(), "J") == 0)
		ret = IsKeyDown(0x24);
	if (_strcmpi(key.c_str(), "K") == 0)
		ret = IsKeyDown(0x25);
	if (_strcmpi(key.c_str(), "L") == 0)
		ret = IsKeyDown(0x26);
	if (_strcmpi(key.c_str(), ";") == 0)
		ret = IsKeyDown(0x27);
	if (_strcmpi(key.c_str(), "'") == 0)
		ret = IsKeyDown(0x28);
	if (_strcmpi(key.c_str(), "|") == 0)
		ret = IsKeyDown(0x29);
	if (_strcmpi(key.c_str(), "LShift") == 0)
		ret = IsKeyDown(0x2A);
	if (_strcmpi(key.c_str(), "\\") == 0)
		ret = IsKeyDown(0x2B);
	if (_strcmpi(key.c_str(), "Z") == 0)
		ret = IsKeyDown(0x2C);
	if (_strcmpi(key.c_str(), "X") == 0)
		ret = IsKeyDown(0x2D);
	if (_strcmpi(key.c_str(), "C") == 0)
		ret = IsKeyDown(0x2E);
	if (_strcmpi(key.c_str(), "V") == 0)
		ret = IsKeyDown(0x2F);
	if (_strcmpi(key.c_str(), "B") == 0)
		ret = IsKeyDown(0x30);
	if (_strcmpi(key.c_str(), "N") == 0)
		ret = IsKeyDown(0x31);
	if (_strcmpi(key.c_str(), "M") == 0)
		ret = IsKeyDown(0x32);
	if (_strcmpi(key.c_str(), ",") == 0)
		ret = IsKeyDown(0x33);
	if (_strcmpi(key.c_str(), ".") == 0)
		ret = IsKeyDown(0x34);
	if (_strcmpi(key.c_str(), "/") == 0)
		ret = IsKeyDown(0x35);
	if (_strcmpi(key.c_str(), "RShift") == 0)
		ret = IsKeyDown(0x36);
	if (_strcmpi(key.c_str(), "*") == 0)
		ret = IsKeyDown(0x37);
	if (_strcmpi(key.c_str(), "LAlt") == 0)
		ret = IsKeyDown(0x38);
	if (_strcmpi(key.c_str(), "Space") == 0)
		ret = IsKeyDown(0x39);
	if (_strcmpi(key.c_str(), "Capital") == 0)
		ret = IsKeyDown(0x3A);
	if (_strcmpi(key.c_str(), "F1") == 0)
		ret = IsKeyDown(0x3B);
	if (_strcmpi(key.c_str(), "F2") == 0)
		ret = IsKeyDown(0x3C);
	if (_strcmpi(key.c_str(), "F3") == 0)
		ret = IsKeyDown(0x3D);
	if (_strcmpi(key.c_str(), "F4") == 0)
		ret = IsKeyDown(0x3E);
	if (_strcmpi(key.c_str(), "F5") == 0)
		ret = IsKeyDown(0x3F);
	if (_strcmpi(key.c_str(), "F6") == 0)
		ret = IsKeyDown(0x40);
	if (_strcmpi(key.c_str(), "F7") == 0)
		ret = IsKeyDown(0x41);
	if (_strcmpi(key.c_str(), "F8") == 0)
		ret = IsKeyDown(0x42);
	if (_strcmpi(key.c_str(), "F9") == 0)
		ret = IsKeyDown(0x43);
	if (_strcmpi(key.c_str(), "F10") == 0)
		ret = IsKeyDown(0x44);
	if (_strcmpi(key.c_str(), "NumLock") == 0)
		ret = IsKeyDown(0x45);
	if (_strcmpi(key.c_str(), "ScrollLock") == 0)
		ret = IsKeyDown(0x46);
	if (_strcmpi(key.c_str(), "NumPad7") == 0)
		ret = IsKeyDown(0x47);
	if (_strcmpi(key.c_str(), "NumPad8") == 0)
		ret = IsKeyDown(0x48);
	if (_strcmpi(key.c_str(), "NumPad9") == 0)
		ret = IsKeyDown(0x49);
	if (_strcmpi(key.c_str(), "NumPad-") == 0)
		ret = IsKeyDown(0x4A);
	if (_strcmpi(key.c_str(), "NumPad4") == 0)
		ret = IsKeyDown(0x4B);
	if (_strcmpi(key.c_str(), "NumPad5") == 0)
		ret = IsKeyDown(0x4C);
	if (_strcmpi(key.c_str(), "NumPad6") == 0)
		ret = IsKeyDown(0x4D);
	if (_strcmpi(key.c_str(), "NumPad+") == 0)
		ret = IsKeyDown(0x4E);
	if (_strcmpi(key.c_str(), "NumPad1") == 0)
		ret = IsKeyDown(0x4F);
	if (_strcmpi(key.c_str(), "NumPad2") == 0)
		ret = IsKeyDown(0x50);
	if (_strcmpi(key.c_str(), "NumPad3") == 0)
		ret = IsKeyDown(0x51);
	if (_strcmpi(key.c_str(), "NumPad0") == 0)
		ret = IsKeyDown(0x52);
	if (_strcmpi(key.c_str(), "NumPad.") == 0)
		ret = IsKeyDown(0x53);
	if (_strcmpi(key.c_str(), "F11") == 0)
		ret = IsKeyDown(0x57);
	if (_strcmpi(key.c_str(), "F12") == 0)
		ret = IsKeyDown(0x58);
	if (_strcmpi(key.c_str(), "PrevTrack") == 0)
		ret = IsKeyDown(0x90);
	if (_strcmpi(key.c_str(), "NumPadEnter") == 0)
		ret = IsKeyDown(0x9C);
	if (_strcmpi(key.c_str(), "RCtrl") == 0)
		ret = IsKeyDown(0x9D);
	if (_strcmpi(key.c_str(), "Mute") == 0)
		ret = IsKeyDown(0xA0);
	if (_strcmpi(key.c_str(), "NumPad/") == 0)
		ret = IsKeyDown(0xB5);
	if (_strcmpi(key.c_str(), "RAlt") == 0)
		ret = IsKeyDown(0xB8);
	if (_strcmpi(key.c_str(), "Pause") == 0)
		ret = IsKeyDown(0xC5);
	if (_strcmpi(key.c_str(), "Home") == 0)
		ret = IsKeyDown(0xC7);
	if (_strcmpi(key.c_str(), "Up") == 0)
		ret = IsKeyDown(0xC8);
	if (_strcmpi(key.c_str(), "PageDown") == 0)
		ret = IsKeyDown(0xC9);
	if (_strcmpi(key.c_str(), "Left") == 0)
		ret = IsKeyDown(0xCB);
	if (_strcmpi(key.c_str(), "Right") == 0)
		ret = IsKeyDown(0xCD);
	if (_strcmpi(key.c_str(), "End") == 0)
		ret = IsKeyDown(0xCF);
	if (_strcmpi(key.c_str(), "Down") == 0)
		ret = IsKeyDown(0xD0);
	if (_strcmpi(key.c_str(), "Insert") == 0)
		ret = IsKeyDown(0xD2);
	if (_strcmpi(key.c_str(), "Delete") == 0)
		ret = IsKeyDown(0xD3);
	if (_strcmpi(key.c_str(), "LWin") == 0)
		ret = IsKeyDown(0xDB);
	if (_strcmpi(key.c_str(), "RWin") == 0)
		ret = IsKeyDown(0xDC);
	return ret;
}
bool AngelCore::AngelSubSystem::InputManager::IsKeyHeld(std::string &key)
{
	bool ret = false;
	if (_strcmpi(key.c_str(), "A") == 0)
		ret = IsKeyHeld(0x1E);
	if (_strcmpi(key.c_str(), "1") == 0)
		ret = IsKeyHeld(0x02);
	if (_strcmpi(key.c_str(), "2") == 0)
		ret = IsKeyHeld(0x03);
	if (_strcmpi(key.c_str(), "3") == 0)
		ret = IsKeyHeld(0x04);
	if (_strcmpi(key.c_str(), "4") == 0)
		ret = IsKeyHeld(0x05);
	if (_strcmpi(key.c_str(), "5") == 0)
		ret = IsKeyHeld(0x06);
	if (_strcmpi(key.c_str(), "6") == 0)
		ret = IsKeyHeld(0x07);
	if (_strcmpi(key.c_str(), "7") == 0)
		ret = IsKeyHeld(0x08);
	if (_strcmpi(key.c_str(), "8") == 0)
		ret = IsKeyHeld(0x09);
	if (_strcmpi(key.c_str(), "9") == 0)
		ret = IsKeyHeld(0x0A);
	if (_strcmpi(key.c_str(), "0") == 0)
		ret = IsKeyHeld(0x0B);
	if (_strcmpi(key.c_str(), "-") == 0)
		ret = IsKeyHeld(0x0C);
	if (_strcmpi(key.c_str(), "=") == 0)
		ret = IsKeyHeld(0x0D);
	if (_strcmpi(key.c_str(), "Back") == 0)
		ret = IsKeyHeld(0x0E);
	if (_strcmpi(key.c_str(), "Tab") == 0)
		ret = IsKeyHeld(0x0F);
	if (_strcmpi(key.c_str(), "Q") == 0)
		ret = IsKeyHeld(0x10);
	if (_strcmpi(key.c_str(), "W") == 0)
		ret = IsKeyHeld(0x11);
	if (_strcmpi(key.c_str(), "E") == 0)
		ret = IsKeyHeld(0x12);
	if (_strcmpi(key.c_str(), "R") == 0)
		ret = IsKeyHeld(0x13);
	if (_strcmpi(key.c_str(), "T") == 0)
		ret = IsKeyHeld(0x14);
	if (_strcmpi(key.c_str(), "Y") == 0)
		ret = IsKeyHeld(0x15);
	if (_strcmpi(key.c_str(), "U") == 0)
		ret = IsKeyHeld(0x16);
	if (_strcmpi(key.c_str(), "I") == 0)
		ret = IsKeyHeld(0x17);
	if (_strcmpi(key.c_str(), "O") == 0)
		ret = IsKeyHeld(0x18);
	if (_strcmpi(key.c_str(), "P") == 0)
		ret = IsKeyHeld(0x19);
	if (_strcmpi(key.c_str(), "[") == 0)
		ret = IsKeyHeld(0x1A);
	if (_strcmpi(key.c_str(), "]") == 0)
		ret = IsKeyHeld(0x1B);
	if (_strcmpi(key.c_str(), "Enter") == 0)
		ret = IsKeyHeld(0x1C);
	if (_strcmpi(key.c_str(), "LCtrl") == 0)
		ret = IsKeyHeld(0x1D);
	if (_strcmpi(key.c_str(), "S") == 0)
		ret = IsKeyHeld(0x1F);
	if (_strcmpi(key.c_str(), "D") == 0)
		ret = IsKeyHeld(0x20);
	if (_strcmpi(key.c_str(), "F") == 0)
		ret = IsKeyHeld(0x21);
	if (_strcmpi(key.c_str(), "G") == 0)
		ret = IsKeyHeld(0x22);
	if (_strcmpi(key.c_str(), "H") == 0)
		ret = IsKeyHeld(0x23);
	if (_strcmpi(key.c_str(), "J") == 0)
		ret = IsKeyHeld(0x24);
	if (_strcmpi(key.c_str(), "K") == 0)
		ret = IsKeyHeld(0x25);
	if (_strcmpi(key.c_str(), "L") == 0)
		ret = IsKeyHeld(0x26);
	if (_strcmpi(key.c_str(), ";") == 0)
		ret = IsKeyHeld(0x27);
	if (_strcmpi(key.c_str(), "'") == 0)
		ret = IsKeyHeld(0x28);
	if (_strcmpi(key.c_str(), "|") == 0)
		ret = IsKeyHeld(0x29);
	if (_strcmpi(key.c_str(), "LShift") == 0)
		ret = IsKeyHeld(0x2A);
	if (_strcmpi(key.c_str(), "\\") == 0)
		ret = IsKeyHeld(0x2B);
	if (_strcmpi(key.c_str(), "Z") == 0)
		ret = IsKeyHeld(0x2C);
	if (_strcmpi(key.c_str(), "X") == 0)
		ret = IsKeyHeld(0x2D);
	if (_strcmpi(key.c_str(), "C") == 0)
		ret = IsKeyHeld(0x2E);
	if (_strcmpi(key.c_str(), "V") == 0)
		ret = IsKeyHeld(0x2F);
	if (_strcmpi(key.c_str(), "B") == 0)
		ret = IsKeyHeld(0x30);
	if (_strcmpi(key.c_str(), "N") == 0)
		ret = IsKeyHeld(0x31);
	if (_strcmpi(key.c_str(), "M") == 0)
		ret = IsKeyHeld(0x32);
	if (_strcmpi(key.c_str(), ",") == 0)
		ret = IsKeyHeld(0x33);
	if (_strcmpi(key.c_str(), ".") == 0)
		ret = IsKeyHeld(0x34);
	if (_strcmpi(key.c_str(), "/") == 0)
		ret = IsKeyHeld(0x35);
	if (_strcmpi(key.c_str(), "RShift") == 0)
		ret = IsKeyHeld(0x36);
	if (_strcmpi(key.c_str(), "*") == 0)
		ret = IsKeyHeld(0x37);
	if (_strcmpi(key.c_str(), "LAlt") == 0)
		ret = IsKeyHeld(0x38);
	if (_strcmpi(key.c_str(), "Space") == 0)
		ret = IsKeyHeld(0x39);
	if (_strcmpi(key.c_str(), "Capital") == 0)
		ret = IsKeyHeld(0x3A);
	if (_strcmpi(key.c_str(), "F1") == 0)
		ret = IsKeyHeld(0x3B);
	if (_strcmpi(key.c_str(), "F2") == 0)
		ret = IsKeyHeld(0x3C);
	if (_strcmpi(key.c_str(), "F3") == 0)
		ret = IsKeyHeld(0x3D);
	if (_strcmpi(key.c_str(), "F4") == 0)
		ret = IsKeyHeld(0x3E);
	if (_strcmpi(key.c_str(), "F5") == 0)
		ret = IsKeyHeld(0x3F);
	if (_strcmpi(key.c_str(), "F6") == 0)
		ret = IsKeyHeld(0x40);
	if (_strcmpi(key.c_str(), "F7") == 0)
		ret = IsKeyHeld(0x41);
	if (_strcmpi(key.c_str(), "F8") == 0)
		ret = IsKeyHeld(0x42);
	if (_strcmpi(key.c_str(), "F9") == 0)
		ret = IsKeyHeld(0x43);
	if (_strcmpi(key.c_str(), "F10") == 0)
		ret = IsKeyHeld(0x44);
	if (_strcmpi(key.c_str(), "NumLock") == 0)
		ret = IsKeyHeld(0x45);
	if (_strcmpi(key.c_str(), "ScrollLock") == 0)
		ret = IsKeyHeld(0x46);
	if (_strcmpi(key.c_str(), "NumPad7") == 0)
		ret = IsKeyHeld(0x47);
	if (_strcmpi(key.c_str(), "NumPad8") == 0)
		ret = IsKeyHeld(0x48);
	if (_strcmpi(key.c_str(), "NumPad9") == 0)
		ret = IsKeyHeld(0x49);
	if (_strcmpi(key.c_str(), "NumPad-") == 0)
		ret = IsKeyHeld(0x4A);
	if (_strcmpi(key.c_str(), "NumPad4") == 0)
		ret = IsKeyHeld(0x4B);
	if (_strcmpi(key.c_str(), "NumPad5") == 0)
		ret = IsKeyHeld(0x4C);
	if (_strcmpi(key.c_str(), "NumPad6") == 0)
		ret = IsKeyHeld(0x4D);
	if (_strcmpi(key.c_str(), "NumPad+") == 0)
		ret = IsKeyHeld(0x4E);
	if (_strcmpi(key.c_str(), "NumPad1") == 0)
		ret = IsKeyHeld(0x4F);
	if (_strcmpi(key.c_str(), "NumPad2") == 0)
		ret = IsKeyHeld(0x50);
	if (_strcmpi(key.c_str(), "NumPad3") == 0)
		ret = IsKeyHeld(0x51);
	if (_strcmpi(key.c_str(), "NumPad0") == 0)
		ret = IsKeyHeld(0x52);
	if (_strcmpi(key.c_str(), "NumPad.") == 0)
		ret = IsKeyHeld(0x53);
	if (_strcmpi(key.c_str(), "F11") == 0)
		ret = IsKeyHeld(0x57);
	if (_strcmpi(key.c_str(), "F12") == 0)
		ret = IsKeyHeld(0x58);
	if (_strcmpi(key.c_str(), "PrevTrack") == 0)
		ret = IsKeyHeld(0x90);
	if (_strcmpi(key.c_str(), "NumPadEnter") == 0)
		ret = IsKeyHeld(0x9C);
	if (_strcmpi(key.c_str(), "RCtrl") == 0)
		ret = IsKeyHeld(0x9D);
	if (_strcmpi(key.c_str(), "Mute") == 0)
		ret = IsKeyHeld(0xA0);
	if (_strcmpi(key.c_str(), "NumPad/") == 0)
		ret = IsKeyHeld(0xB5);
	if (_strcmpi(key.c_str(), "RAlt") == 0)
		ret = IsKeyHeld(0xB8);
	if (_strcmpi(key.c_str(), "Pause") == 0)
		ret = IsKeyHeld(0xC5);
	if (_strcmpi(key.c_str(), "Home") == 0)
		ret = IsKeyHeld(0xC7);
	if (_strcmpi(key.c_str(), "Up") == 0)
		ret = IsKeyHeld(0xC8);
	if (_strcmpi(key.c_str(), "PageDown") == 0)
		ret = IsKeyHeld(0xC9);
	if (_strcmpi(key.c_str(), "Left") == 0)
		ret = IsKeyHeld(0xCB);
	if (_strcmpi(key.c_str(), "Right") == 0)
		ret = IsKeyHeld(0xCD);
	if (_strcmpi(key.c_str(), "End") == 0)
		ret = IsKeyHeld(0xCF);
	if (_strcmpi(key.c_str(), "Down") == 0)
		ret = IsKeyHeld(0xD0);
	if (_strcmpi(key.c_str(), "Insert") == 0)
		ret = IsKeyHeld(0xD2);
	if (_strcmpi(key.c_str(), "Delete") == 0)
		ret = IsKeyHeld(0xD3);
	if (_strcmpi(key.c_str(), "LWin") == 0)
		ret = IsKeyHeld(0xDB);
	if (_strcmpi(key.c_str(), "RWin") == 0)
		ret = IsKeyHeld(0xDC);
	return ret;
}
void AngelCore::AngelSubSystem::InputManager::SetMousePosition(DirectX::XMFLOAT2 pos)
{
	if(!AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->GetEnable())
		this->m_mousePosition = pos;
	else
	{
		this->m_mousePosition = pos;
		this->m_mousePosition.y *= 2;
	}
} 