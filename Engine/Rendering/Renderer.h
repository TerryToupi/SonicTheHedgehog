#pragma once

#include "Utils/Common.h"

namespace gfx
{
	void		Open(const char* title, Dim rw, Dim rh);
	void		Close(void);
	Dim			GetResWidth(void);
	Dim			GetResHeight(void);
}