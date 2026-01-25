#pragma once

#include "Animations/FrameRangeAnimation.h"
#include "Scene/Sprite.h"
#include "Physics/BoundingArea.h"
#include "Animations/AnimationFilmHolder.h"
#include "Animations/FrameRangeAnimator.h"
#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"

class Sonic : public scene::Sprite
{
public:
	Sonic(int x, int y, scene::GridMap* map, scene::TileLayer* layer);
	~Sonic();
	
private:
	// example animation in use	
	anim::FrameRangeAnimation* m_movingLeft = nullptr;
	anim::FrameRangeAnimation* m_movingRight = nullptr;
	anim::FrameRangeAnimation* m_crouch = nullptr;
	anim::FrameRangeAnimation* m_falling = nullptr;

	// animator in use for the animations above	
	anim::FrameRangeAnimator*  m_animator = nullptr;
};