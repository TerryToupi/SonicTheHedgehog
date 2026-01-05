#pragma once

#include "Utils/Common.h"
#include "Rendering/Bitmap.h"

namespace gfx
{
	void Open(const char* title, Dim rw, Dim rh);
	void Close(void);
	Dim	 GetResWidth(void);
	Dim  GetResHeight(void);

	Color  GetBackgroundColor(void);
	void   SetBackgroundColor(Color c);
	Bitmap GetScreenBuffer(void);
	Rect   GetScreenRect(void);

	void RaiseWindowResizeEvent(void);

	void Flush();
}