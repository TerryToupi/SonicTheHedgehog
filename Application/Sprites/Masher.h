#pragma once

#include "Scene/Sprite.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"
#include "Sound/Sound.h"

class Masher : public scene::Sprite
{
public:
    Masher(int x, int y);
    ~Masher();

    void StartAnimation();
    void StopAnimation();
    void Update();  // Call each frame to handle jump logic
    bool IsAlive() const { return m_IsAlive; }
    void Kill();    // Called when Sonic destroys the enemy
    void SetJumpDelay(int frames) { m_JumpCooldown = frames; }  // Offset initial timing

private:
    void StartJump();
    void ApplyGravity();

    // Film (owned by AnimationFilmHolder)
    anim::AnimationFilm* m_Film = nullptr;

    // Animation (mouth open/close)
    anim::FrameRangeAnimation* m_Animation = nullptr;
    anim::FrameRangeAnimator* m_Animator = nullptr;

    // State
    bool m_IsAlive = true;
    int m_SpawnX = 0;       // Original spawn position
    int m_SpawnY = 0;

    // Jump physics
    int m_VelocityY = 0;
    int m_JumpCooldown = 0;     // Frames until next jump
    bool m_IsJumping = false;
    int m_GravityFrame = 0;     // Counter for gravity application

    // Constants
    static constexpr int JUMP_VELOCITY = -15;    // Initial upward velocity
    static constexpr int GRAVITY = 1;            // Gravity acceleration per application
    static constexpr int GRAVITY_FRAMES = 2;     // Apply gravity every N frames (higher = floatier)
    static constexpr int MAX_FALL_SPEED = 10;    // Terminal velocity
    static constexpr int JUMP_COOLDOWN = 60;     // Frames between jumps (~1 second at 60fps)
    static constexpr unsigned ANIM_DELAY_MS = 120;  // Mouth animation speed

    // Shared sound effect for all mashers
    static sound::SFX s_DeathSound;
};
