#pragma once

#include "Scene/Sprite.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"

#include <string>

class Flower : public scene::Sprite
{
public:
    Flower(int x, int y, const std::string& filmId);
    ~Flower();

    void StartAnimation();
    void StopAnimation();

private:
    anim::AnimationFilm* m_Film = nullptr;
    anim::FrameRangeAnimation* m_Animation = nullptr;
    anim::FrameRangeAnimator* m_Animator = nullptr;

    static constexpr unsigned ANIMATION_DELAY_MS = 400;
};
