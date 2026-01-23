#pragma once

#include "IO/IOMapping.h"

namespace core
{
	class Input
	{
	public:
		static void Update();
		static bool IsKeyPressed(io::Key key);
	};
}