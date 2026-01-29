#pragma once

#include "Utils/Common.h"
#include "Rendering/Bitmap.h"
#include "Scene/GravityHandler.h"
#include "Scene/MotionQuantizer.h"
#include "Rendering/Clipper.h"
#include "Animations/AnimationFilm.h"
#include "Core/LatelyDestroyable.h"
#include "Physics/BoundingArea.h"

#include <string>
#include <functional>

namespace scene
{
	using namespace core;
	using namespace gfx;
	using namespace anim;
	using namespace physics;

	class Sprite : public LatelyDestroyable
	{
	public:
		using Mover = std::function<void(Rect& r, int* dx, int* dy)>;

	public:
		void SetMover(const Mover& f);

		const Rect GetBox(void) const;

		GravityHandler& GetGravityHandler(void);
		void SetHasDirectMotion(bool v);
		bool GetHasDirectMotion(void);

		void Move(int dx, int dy);
		void MoveDirect(int dx, int dy);
		void SetPos(int _x, int _y);

		void SetZorder(unsigned z);
		unsigned GetZorder(void);

		void SetBoundingArea(BoundingArea* area);
		auto GetBoundingArea(void) const -> const BoundingArea*;
		bool CollisionCheck(const Sprite* s) const;

		void SetFrame(byte i);
		byte GetFrame(void);
		void SetTypeID(const std::string& _id);
		auto GetTypeID(void) -> const std::string&;
		void SetVisibility(bool v);
		bool IsVisible(void) const;

		void SetFlipHorizontal(bool flip) { m_FlipHorizontal = flip; }
		bool GetFlipHorizontal(void) const { return m_FlipHorizontal; }

		virtual void Display(Bitmap dest, const Rect& dpyArea, const Clipper& clipper) const;

		void SetFilm(AnimationFilm* film);
		auto GetFilm(void) const -> const AnimationFilm&;

		Sprite(int _x, int _y, const std::string& _typeID = "");

	protected:
		byte m_FrameNo = 0;
		Rect m_FrameBox;
		int m_X = 0;
		int m_Y = 0;
		bool m_IsVisible = false;

		AnimationFilm* m_CurrFilm = nullptr;
		unsigned m_Zorder = 0;

		BoundingArea* m_BoundingArea = nullptr;

		std::string m_TypeID, m_StateID;
		Mover m_Mover;
		MotionQuantizer m_Quantizer;
		GravityHandler  m_Gravity;

		bool m_DirectMotion = false;
		bool m_FlipHorizontal = false;
	};
}