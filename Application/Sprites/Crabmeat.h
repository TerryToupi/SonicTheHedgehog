#pragma once

#include "Scene/Sprite.h"
#include "Scene/GridLayer.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"
#include "Sound/Sound.h"

// Forward declaration
class CrabBullet;

class Crabmeat : public scene::Sprite
{
public:
    enum class State { IDLE, WALKING, ATTACKING };
    enum class Direction { LEFT, RIGHT };

    Crabmeat(int x, int y, scene::GridMap* grid);
    ~Crabmeat();

    void StartAnimation();
    void StopAnimation();
    void Update();
    bool IsAlive() const { return m_IsAlive; }
    void Kill();

    // Get bullets for collision registration
    const std::vector<CrabBullet*>& GetBullets() const { return m_Bullets; }
    std::vector<CrabBullet*>& GetBullets() { return m_Bullets; }

private:
    void SetState(State newState);
    void UpdateAnimation();
    void FireBullets();
    void UpdateBullets();

    // Films (owned by AnimationFilmHolder)
    anim::AnimationFilm* m_IdleFilm = nullptr;
    anim::AnimationFilm* m_WalkFilm = nullptr;
    anim::AnimationFilm* m_AttackFilm = nullptr;

    // Animations
    anim::FrameRangeAnimation* m_IdleAnim = nullptr;
    anim::FrameRangeAnimation* m_WalkAnim = nullptr;
    anim::FrameRangeAnimation* m_AttackAnim = nullptr;

    // Animator
    anim::FrameRangeAnimator* m_Animator = nullptr;

    // State
    State m_State = State::IDLE;
    Direction m_Direction = Direction::LEFT;
    bool m_IsAlive = true;
    int m_SpawnX = 0;
    scene::GridMap* m_Grid = nullptr;

    // Movement pattern timing (in frames at ~60fps)
    int m_PatternTimer = 0;
    int m_IdleTimer = 0;
    int m_MoveFrame = 0;  // Counter for slowing down movement
    bool m_WaitingIdle = false;  // True when in idle pause between walks

    // Bullets
    std::vector<CrabBullet*> m_Bullets;
    bool m_HasFiredThisCycle = false;

    // Constants
    static constexpr int WALK_SPEED = 1;
    static constexpr int MOVE_EVERY_N_FRAMES = 3;  // Move once every 3 frames (slower walk)
    static constexpr int WALK_DURATION_FRAMES = 120;  // 2 seconds at 60fps
    static constexpr int IDLE_DURATION_FRAMES = 60;   // 1 second at 60fps
    static constexpr unsigned WALK_ANIM_DELAY_MS = 150;
    static constexpr unsigned IDLE_ANIM_DELAY_MS = 200;

    // Shared sound effect
    static sound::SFX s_DeathSound;
};
