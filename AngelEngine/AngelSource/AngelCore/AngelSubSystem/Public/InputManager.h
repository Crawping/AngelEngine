#pragma once

#include<dinput.h>
#include"SubSystemBase.h"
#include<Windows.h>
#include<map>

namespace AngelCore
{
	namespace AngelSubSystem
	{

		class InputManager : public AngelSubSystem::ISubSystem
		{
		public:
			struct Keys
			{
				static int ESCAPE;
				static int One;
				static int Two;
				static int Three;
				static int Four;
				static int Five;
				static int Six;
				static int Seven;
				static int Eight;
				static int Nine;
				static int Zero;
				static int MINUS;    /* - on main keyboard */
				static int EQUALS;
				static int BACK;    /* backspace */
				static int TAB;
				static int Q;
				static int W;
				static int E;
				static int R;
				static int T;
				static int Y;
				static int U;
				static int I;
				static int O;
				static int P;
				static int LBRACKET;
				static int RBRACKET;
				static int RETURN;    /* Enter on main keyboard */
				static int LCONTROL;
				static int A;
				static int S;
				static int D;
				static int F;
				static int G;
				static int H;
				static int J;
				static int K;
				static int L;
				static int SEMICOLON;
				static int APOSTROPHE;
				static int GRAVE;    /* accent grave */
				static int LSHIFT;
				static int BACKSLASH;
				static int Z;
				static int X;
				static int C;
				static int V;
				static int B;
				static int N;
				static int M;
				static int COMMA;
				static int PERIOD;    /* . on main keyboard */
				static int SLASH;    /* / on main keyboard */
				static int RSHIFT;
				static int MULTIPLY;    /* * on numeric keypad */
				static int LMENU;    /* left Alt */
				static int SPACE;
				static int CAPITAL;
				static int F1;
				static int F2;
				static int F3;
				static int F4;
				static int F5;
				static int F6;
				static int F7;
				static int F8;
				static int F9;
				static int F10;
				static int NUMLOCK;
				static int SCROLL;    /* Scroll Lock */
				static int NUMPAD7;
				static int NUMPAD8;
				static int NUMPAD9;
				static int SUBTRACT;    /* - on numeric keypad */
				static int NUMPAD4;
				static int NUMPAD5;
				static int NUMPAD6;
				static int ADD;    /* + on numeric keypad */
				static int NUMPAD1;
				static int NUMPAD2;
				static int NUMPAD3;
				static int NUMPAD0;
				static int DECIMAL;    /* . on numeric keypad */
				static int OEM_102;    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
				static int F11;
				static int F12;
				static int F13;    /*                     (NEC PC98) */
				static int F14;    /*                     (NEC PC98) */
				static int F15;    /*                     (NEC PC98) */
				static int KANA;    /* (Japanese keyboard)            */
				static int ABNT_C1;    /* /? on Brazilian keyboard */
				static int CONVERT;    /* (Japanese keyboard)            */
				static int NOCONVERT;    /* (Japanese keyboard)            */
				static int YEN;    /* (Japanese keyboard)            */
				static int ABNT_C2;    /* Numpad . on Brazilian keyboard */
				static int NUMPADEQUALS;    /* = on numeric keypad (NEC PC98) */
				static int PREVTRACK;    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
				static int AT;    /*                     (NEC PC98) */
				static int COLON;    /*                     (NEC PC98) */
				static int UNDERLINE;    /*                     (NEC PC98) */
				static int KANJI;    /* (Japanese keyboard)            */
				static int STOP;    /*                     (NEC PC98) */
				static int AX;    /*                     (Japan AX) */
				static int UNLABELED;    /*                        (J3100) */
				static int NEXTTRACK;    /* Next Track */
				static int NUMPADENTER;    /* Enter on numeric keypad */
				static int RCONTROL;
				static int MUTE;    /* Mute */
				static int CALCULATOR;    /* Calculator */
				static int PLAYPAUSE;    /* Play / Pause */
				static int MEDIASTOP;    /* Media Stop */
				static int VOLUMEDOWN;    /* Volume - */
				static int VOLUMEUP;    /* Volume + */
				static int WEBHOME;    /* Web home */
				static int NUMPADCOMMA;    /* , on numeric keypad (NEC PC98) */
				static int DIVIDE;    /* / on numeric keypad */
				static int SYSRQ;
				static int RMENU;    /* right Alt */
				static int PAUSE;    /* Pause */
				static int HOME;    /* Home on arrow keypad */
				static int UP;    /* UpArrow on arrow keypad */
				static int PRIOR;    /* PgUp on arrow keypad */
				static int LEFT;    /* LeftArrow on arrow keypad */
				static int RIGHT;    /* RightArrow on arrow keypad */
				static int END;    /* End on arrow keypad */
				static int DOWN;    /* DownArrow on arrow keypad */
				static int NEXT;    /* PgDn on arrow keypad */
				static int INSERT;    /* Insert on arrow keypad */
				static int DELETEKey;    /* Delete on arrow keypad */
				static int LWIN;    /* Left Windows key */
				static int RWIN;    /* Right Windows key */
				static int APPS;    /* AppMenu key */
				static int POWER;    /* System Power */
				static int SLEEP;    /* System Sleep */
				static int WAKE;    /* System Wake */
				static int WEBSEARCH;    /* Web Search */
				static int WEBFAVORITES;    /* Web Favorites */
				static int WEBREFRESH;    /* Web Refresh */
				static int WEBSTOP;    /* Web Stop */
				static int WEBFORWARD;    /* Web Forward */
				static int WEBBACK;    /* Web Back */
				static int MYCOMPUTER;    /* My Computer */
				static int MAIL;    /* Mail */
				static int MEDIASELECT;    /* Media Select */

			};

		public:
			bool StartUp(...) override;
			bool ShutDown() override;

			void DetectInput();

			bool IsKeyDown(int key);
			bool IsKeyDown(std::string &key);
			bool IsKeyUp(int key);
			bool IsKeyUp(std::string &key);
			bool IsKeyPressed(int key);
			bool IsKeyPressed(std::string &key);
			bool IsKeyHeld(int key);
			bool IsKeyHeld(std::string &key);
			bool IsAnyKeyPressed();
			DirectX::XMFLOAT2 GetMousePosition()				{ return m_mousePosition; }
			Vector2 GetMousePosition() const					{ return m_mousePosition;  }
			void SetMousePosition(DirectX::XMFLOAT2 pos);
			bool IsRightMouseClicked()							{ return this->m_isRightClicked; }
			bool IsLeftMouseClicked()							{ return this->m_leftClicked; }

			void SetRightClicked(bool _b)						{ this->m_isRightClicked = _b; }
			void SetLeftClicked(bool _b)						{ this->m_leftClicked = _b; }
			//Get Mouse pointer movment from last frame.
			DirectX::XMFLOAT2 GetMouseOffset() { return m_mouseOffset; }


		private:
			IDirectInputDevice8*			m_DIKeyboard;
			IDirectInputDevice8*			m_DIMouse;
			DIMOUSESTATE					m_mouseLastState;
			LPDIRECTINPUT8					m_DirectInput;
			std::map<int, bool>				keys;
			std::map<int, bool>				keysPressed;
			std::map<int, int>				heldCounter;
			DirectX::XMFLOAT2				m_mousePosition;
			bool							m_isRightClicked;
			bool							m_leftClicked;
			DirectX::XMFLOAT2				m_lMousePosition;
			DirectX::XMFLOAT2				m_mouseOffset;
		};

		extern InputManager * inputManager;
	}
}
