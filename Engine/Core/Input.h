#pragma once

#include "IO/IOMapping.h"

namespace core
{
	class Input
	{
	public:
		static void UpdateInputEvents();
		static void FlushEvents();  // Clear pending events (use between scene transitions)

		static bool IsKeyPressed(io::Key key);
		static void GetMousePosition(int* x, int* y);
	};
}