#pragma once

namespace io
{
	enum class Key : int
	{
		Unknown = 0,
		A, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
		Left, Right, Up, Down,
		Space, Enter, Escape, Tab, Backspace,
		LShift, RShift, LCtrl, RCtrl, LAlt, RAlt,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};

	enum class Button : int
	{
		Unknown = 0,
		Left,
		Middle,
		Right,
		X1,
		X2
	};

	class IOMapper final
	{
	public:
		static Key		ConvertKey(int code);
		static Button	ConvertButton(int button);
		static int		GetScancode(Key key);
	};
}
