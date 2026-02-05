#pragma once

#include "Scene/Sprite.h"
#include "Scene/GridLayer.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"

class CrabBullet : public scene::Sprite
{
public:
    CrabBullet(int x, int y, int velocityX, int velocityY, scene::GridMap* grid);
    ~CrabBullet();

    void StartAnimation();
    void StopAnimation();
    void Update();

    bool IsActive() const { return m_IsActive; }
    void Deactivate() { m_IsActive = false; }

private:
    // Film (owned by AnimationFilmHolder)
    anim::AnimationFilm* m_Film = nullptr;

    // Animation
    anim::FrameRangeAnimation* m_Animation = nullptr;
    anim::FrameRangeAnimator* m_Animator = nullptr;

    // Physics
    int m_VelocityX = 0;
    int m_VelocityY = 0;
    int m_GravityFrame = 0;

    // State
    bool m_IsActive = true;
    int m_LifetimeFrames = 0;

    // Grid reference for terrain collision
    scene::GridMap* m_Grid = nullptr;

    // Constants
    static constexpr int GRAVITY = 1;
    static constexpr int GRAVITY_FRAMES = 3;  // Apply gravity every N frames
    static constexpr int MAX_FALL_SPEED = 6;
    static constexpr int MAX_LIFETIME_FRAMES = 180;  // 3 seconds at 60fps
    static constexpr unsigned ANIM_DELAY_MS = 100;
};
