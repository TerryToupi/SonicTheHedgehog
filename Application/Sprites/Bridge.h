#pragma once

#include "Scene/Sprite.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"

class Bridge : public scene::Sprite
{
public:
    Bridge(int x, int y);
    ~Bridge();

    void StartAnimation();
    void StopAnimation();

private:
    anim::AnimationFilm* m_Film = nullptr;
    anim::FrameRangeAnimation* m_Animation = nullptr;
    anim::FrameRangeAnimator* m_Animator = nullptr;

    static constexpr unsigned ANIMATION_DELAY_MS = 100;
};
