#pragma once

#include "Animations/FrameRangeAnimation.h"
#include "Scene/Sprite.h"
#include "Physics/BoundingArea.h"
#include "Animations/AnimationFilmHolder.h"
#include "Animations/FrameRangeAnimator.h"
#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"
#include "Sound/Sound.h"

class Sonic : public scene::Sprite
{
public:
	enum class State { IDLE, WALKING, BALL };
	enum class Direction { LEFT, RIGHT };

	Sonic(int x, int y, scene::GridMap* map, scene::TileLayer* layer);
	~Sonic();

	void Update();
	void UpdateBoundingArea();

	int GetCenterX() const { return m_X + 15; }
	int GetCenterY() const { return m_Y + 20; }
	int GetBottomY() const { return m_Y + 32; }  // Bottom of sprite for ground tracking
	bool IsOnGround() const { return m_OnGround; }

	// Damage handling
	void OnHit();
	bool IsInvincible() const { return m_InvincibilityFrames > 0; }
	bool IsInBallState() const { return m_State == State::BALL; }
	void BounceOffEnemy();  // Small upward boost when killing an enemy

private:
	void HandleInput();
	void ApplyMovement();
	void UpdateAnimationState();
	void SetState(State newState);
	void UpdateAnimation();

	// Films
	anim::AnimationFilm* m_IdleFilm = nullptr;
	anim::AnimationFilm* m_WalkFilm = nullptr;
	anim::AnimationFilm* m_BallFilm = nullptr;

	// Animations
	anim::FrameRangeAnimation* m_IdleAnim = nullptr;
	anim::FrameRangeAnimation* m_WalkAnim = nullptr;
	anim::FrameRangeAnimation* m_BallAnim = nullptr;

	// Animator
	anim::FrameRangeAnimator* m_Animator = nullptr;

	// State
	State m_State = State::BALL;  // Start in ball state since spawning in air
	Direction m_Direction = Direction::RIGHT;
	bool m_OnGround = false;  // Start false so gravity check runs immediately

	// Physics
	int m_VelocityX = 0;
	int m_VelocityY = 0;
	bool m_JumpHeld = false;
	int m_GravityFrame = 0;  // Counter to apply gravity every N frames

	// References
	scene::GridMap* m_Grid = nullptr;
	scene::TileLayer* m_TileLayer = nullptr;

	// Constants
	static constexpr int WALK_SPEED = 3;
	static constexpr int JUMP_VELOCITY = -15;
	static constexpr int GRAVITY = 1;
	static constexpr int MAX_FALL_SPEED = 8;

	// Shared sound effects
	static sound::SFX s_JumpSound;
	static sound::SFX s_HitSound;

	// Invincibility after being hit
	int m_InvincibilityFrames = 0;
	static constexpr int INVINCIBILITY_DURATION = 120;  // ~2 seconds at 60fps
};
