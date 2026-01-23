#include "IO/IOMapping.h"
#include "SDL3/SDL.h"

#include <array>

namespace io
{ 
	static constexpr std::array<Key, SDL_SCANCODE_COUNT> BuildMap()
	{
		std::array<Key, SDL_SCANCODE_COUNT> map{};
		map.fill(Key::Unknown);

		map[SDL_SCANCODE_A] = Key::A;
		map[SDL_SCANCODE_B] = Key::B;
		map[SDL_SCANCODE_C] = Key::C;
		map[SDL_SCANCODE_D] = Key::D;
		map[SDL_SCANCODE_E] = Key::E;
		map[SDL_SCANCODE_F] = Key::F;
		map[SDL_SCANCODE_G] = Key::G;
		map[SDL_SCANCODE_H] = Key::H;
		map[SDL_SCANCODE_I] = Key::I;
		map[SDL_SCANCODE_J] = Key::J;
		map[SDL_SCANCODE_K] = Key::K;
		map[SDL_SCANCODE_L] = Key::L;
		map[SDL_SCANCODE_M] = Key::M;
		map[SDL_SCANCODE_N] = Key::N;
		map[SDL_SCANCODE_O] = Key::O;
		map[SDL_SCANCODE_P] = Key::P;
		map[SDL_SCANCODE_Q] = Key::Q;
		map[SDL_SCANCODE_R] = Key::R;
		map[SDL_SCANCODE_S] = Key::S;
		map[SDL_SCANCODE_T] = Key::T;
		map[SDL_SCANCODE_U] = Key::U;
		map[SDL_SCANCODE_V] = Key::V;
		map[SDL_SCANCODE_W] = Key::W;
		map[SDL_SCANCODE_X] = Key::X;
		map[SDL_SCANCODE_Y] = Key::Y;
		map[SDL_SCANCODE_Z] = Key::Z;

		map[SDL_SCANCODE_0] = Key::Num0;
		map[SDL_SCANCODE_1] = Key::Num1;
		map[SDL_SCANCODE_2] = Key::Num2;
		map[SDL_SCANCODE_3] = Key::Num3;
		map[SDL_SCANCODE_4] = Key::Num4;
		map[SDL_SCANCODE_5] = Key::Num5;
		map[SDL_SCANCODE_6] = Key::Num6;
		map[SDL_SCANCODE_7] = Key::Num7;
		map[SDL_SCANCODE_8] = Key::Num8;
		map[SDL_SCANCODE_9] = Key::Num9;

		map[SDL_SCANCODE_LEFT] = Key::Left;
		map[SDL_SCANCODE_RIGHT] = Key::Right;
		map[SDL_SCANCODE_UP] = Key::Up;
		map[SDL_SCANCODE_DOWN] = Key::Down;

		map[SDL_SCANCODE_SPACE] = Key::Space;
		map[SDL_SCANCODE_RETURN] = Key::Enter;
		map[SDL_SCANCODE_ESCAPE] = Key::Escape;
		map[SDL_SCANCODE_TAB] = Key::Tab;
		map[SDL_SCANCODE_BACKSPACE] = Key::Backspace;

		map[SDL_SCANCODE_LSHIFT] = Key::LShift;
		map[SDL_SCANCODE_RSHIFT] = Key::RShift;
		map[SDL_SCANCODE_LCTRL] = Key::LCtrl;
		map[SDL_SCANCODE_RCTRL] = Key::RCtrl;
		map[SDL_SCANCODE_LALT] = Key::LAlt;
		map[SDL_SCANCODE_RALT] = Key::RAlt;

		map[SDL_SCANCODE_F1] = Key::F1;
		map[SDL_SCANCODE_F2] = Key::F2;
		map[SDL_SCANCODE_F3] = Key::F3;
		map[SDL_SCANCODE_F4] = Key::F4;
		map[SDL_SCANCODE_F5] = Key::F5;
		map[SDL_SCANCODE_F6] = Key::F6;
		map[SDL_SCANCODE_F7] = Key::F7;
		map[SDL_SCANCODE_F8] = Key::F8;
		map[SDL_SCANCODE_F9] = Key::F9;
		map[SDL_SCANCODE_F10] = Key::F10;
		map[SDL_SCANCODE_F11] = Key::F11;
		map[SDL_SCANCODE_F12] = Key::F12;

		return map;
	}

	static constexpr std::array<Key, SDL_SCANCODE_COUNT> s_KeyboardMap = BuildMap();

	Key IOMapper::ConvertKey(int code)
	{
		if (code < 0 || code >= static_cast<int>(s_KeyboardMap.size()))
			return Key::Unknown;
		return s_KeyboardMap[static_cast<size_t>(code)];
	}

	io::Button IOMapper::ConvertButton(int button)
	{
		switch (button)
		{
		case SDL_BUTTON_LEFT:
			return Button::Left;
		case SDL_BUTTON_MIDDLE:
			return Button::Middle;
		case SDL_BUTTON_RIGHT:
			return Button::Right;
		case SDL_BUTTON_X1:
			return Button::X1;
		case SDL_BUTTON_X2:
			return Button::X2;
		default:
			return Button::Unknown;
		}
	}

	int IOMapper::GetScancode(Key key)
	{
		switch (key)
		{
		case Key::A: return SDL_SCANCODE_A;
		case Key::B: return SDL_SCANCODE_B;
		case Key::C: return SDL_SCANCODE_C;
		case Key::D: return SDL_SCANCODE_D;
		case Key::E: return SDL_SCANCODE_E;
		case Key::F: return SDL_SCANCODE_F;
		case Key::G: return SDL_SCANCODE_G;
		case Key::H: return SDL_SCANCODE_H;
		case Key::I: return SDL_SCANCODE_I;
		case Key::J: return SDL_SCANCODE_J;
		case Key::K: return SDL_SCANCODE_K;
		case Key::L: return SDL_SCANCODE_L;
		case Key::M: return SDL_SCANCODE_M;
		case Key::N: return SDL_SCANCODE_N;
		case Key::O: return SDL_SCANCODE_O;
		case Key::P: return SDL_SCANCODE_P;
		case Key::Q: return SDL_SCANCODE_Q;
		case Key::R: return SDL_SCANCODE_R;
		case Key::S: return SDL_SCANCODE_S;
		case Key::T: return SDL_SCANCODE_T;
		case Key::U: return SDL_SCANCODE_U;
		case Key::V: return SDL_SCANCODE_V;
		case Key::W: return SDL_SCANCODE_W;
		case Key::X: return SDL_SCANCODE_X;
		case Key::Y: return SDL_SCANCODE_Y;
		case Key::Z: return SDL_SCANCODE_Z;
		case Key::Num0: return SDL_SCANCODE_0;
		case Key::Num1: return SDL_SCANCODE_1;
		case Key::Num2: return SDL_SCANCODE_2;
		case Key::Num3: return SDL_SCANCODE_3;
		case Key::Num4: return SDL_SCANCODE_4;
		case Key::Num5: return SDL_SCANCODE_5;
		case Key::Num6: return SDL_SCANCODE_6;
		case Key::Num7: return SDL_SCANCODE_7;
		case Key::Num8: return SDL_SCANCODE_8;
		case Key::Num9: return SDL_SCANCODE_9;
		case Key::Left: return SDL_SCANCODE_LEFT;
		case Key::Right: return SDL_SCANCODE_RIGHT;
		case Key::Up: return SDL_SCANCODE_UP;
		case Key::Down: return SDL_SCANCODE_DOWN;
		case Key::Space: return SDL_SCANCODE_SPACE;
		case Key::Enter: return SDL_SCANCODE_RETURN;
		case Key::Escape: return SDL_SCANCODE_ESCAPE;
		case Key::Tab: return SDL_SCANCODE_TAB;
		case Key::Backspace: return SDL_SCANCODE_BACKSPACE;
		case Key::LShift: return SDL_SCANCODE_LSHIFT;
		case Key::RShift: return SDL_SCANCODE_RSHIFT;
		case Key::LCtrl: return SDL_SCANCODE_LCTRL;
		case Key::RCtrl: return SDL_SCANCODE_RCTRL;
		case Key::LAlt: return SDL_SCANCODE_LALT;
		case Key::RAlt: return SDL_SCANCODE_RALT;
		case Key::F1: return SDL_SCANCODE_F1;
		case Key::F2: return SDL_SCANCODE_F2;
		case Key::F3: return SDL_SCANCODE_F3;
		case Key::F4: return SDL_SCANCODE_F4;
		case Key::F5: return SDL_SCANCODE_F5;
		case Key::F6: return SDL_SCANCODE_F6;
		case Key::F7: return SDL_SCANCODE_F7;
		case Key::F8: return SDL_SCANCODE_F8;
		case Key::F9: return SDL_SCANCODE_F9;
		case Key::F10: return SDL_SCANCODE_F10;
		case Key::F11: return SDL_SCANCODE_F11;
		case Key::F12: return SDL_SCANCODE_F12;
		default: return SDL_SCANCODE_UNKNOWN;
		}
	}
}

