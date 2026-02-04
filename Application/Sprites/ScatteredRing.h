#pragma once

#include "Scene/Sprite.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"
#include "Sound/Sound.h"

class ScatteredRing : public scene::Sprite
{
public:
    ScatteredRing(int x, int y, float velocityX, float velocityY);
    ~ScatteredRing();

    void StartAnimation();
    void StopAnimation();
    void Update();  // Called each frame for physics
    void OnCollected();
    void UpdateBoundingArea();

    bool IsCollectable() const;  // After delay period
    bool IsCollected() const;
    bool IsExpired() const;  // Should be removed from scene
    bool IsCollectionFinished() const;

private:
    // Films (owned by AnimationFilmHolder, not us)
    anim::AnimationFilm* m_SpinFilm = nullptr;
    anim::AnimationFilm* m_CollectedFilm = nullptr;

    // Animations
    anim::FrameRangeAnimation* m_SpinAnimation = nullptr;
    anim::FrameRangeAnimation* m_CollectedAnimation = nullptr;

    // Animator (reused for both animations)
    anim::FrameRangeAnimator* m_Animator = nullptr;

    // Physics state (floating point for smooth movement)
    float m_PosX;
    float m_PosY;
    float m_VelocityX;
    float m_VelocityY;
    int m_BounceCount = 0;

    // Lifecycle state
    bool m_Collected = false;
    bool m_CollectionFinished = false;
    int m_FrameCount = 0;  // Frames since spawn

    // Shared sound effect for all scattered rings
    static sound::SFX s_CollectSound;

    // Physics constants
    static constexpr float GRAVITY = 0.21875f;
    static constexpr float BOUNCE_DAMPING = 0.75f;
    static constexpr int MAX_BOUNCES = 4;

    // Lifecycle constants
    static constexpr int COLLECT_DELAY_FRAMES = 64;     // ~1 second at 60fps
    static constexpr int LIFETIME_FRAMES = 256;         // ~4.3 seconds total
    static constexpr int FLASH_START_FRAMES = 192;      // Start flashing at ~3.2 seconds
    static constexpr int FLASH_INTERVAL = 4;            // Flash every 4 frames

    // Animation constants
    static constexpr unsigned SPIN_START_FRAME = 0;
    static constexpr unsigned SPIN_END_FRAME = 3;
    static constexpr unsigned SPIN_DELAY_MS = 100;

    static constexpr unsigned COLLECTED_START_FRAME = 0;
    static constexpr unsigned COLLECTED_END_FRAME = 3;
    static constexpr unsigned COLLECTED_DELAY_MS = 50;
};
