#pragma once

#include "Animations/FrameRangeAnimation.h"
#include "Scene/Sprite.h"
#include "Physics/BoundingArea.h"
#include "Animations/AnimationFilmHolder.h"
#include "Animations/FrameRangeAnimator.h"
#include "Animations/TunnelPath.h"
#include "Animations/TunnelPathAnimator.h"
#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"
#include "Sound/Sound.h"

#include <vector>
#include <functional>

class Sonic : public scene::Sprite
{
public:
	enum class State { IDLE, WALKING, RUNNING, BALL, TUNNEL };
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
	bool IsInBallState() const { return m_State == State::BALL || m_State == State::TUNNEL; }
	void BounceOffEnemy();  // Small upward boost when killing an enemy

	// Ring scatter callback (called when Sonic loses rings on hit)
	using RingScatterCallback = std::function<void(int x, int y, int count)>;
	void SetRingScatterCallback(RingScatterCallback callback) { m_RingScatterCallback = std::move(callback); }

	// Allow passing through vertical terrain when in ball form and moving upward
	bool CanPassThroughCeiling() const override { return m_State == State::BALL && m_VelocityY < 0; }

	// Tunnel path system
	void SetTunnelPaths(const std::vector<anim::TunnelPath>* paths) { m_TunnelPaths = paths; }
	bool IsInTunnel() const { return m_InTunnel; }

private:
	void CheckTunnelTriggers();
	void EnterTunnel(const anim::TunnelPath* path);
	void ExitTunnel();

	void HandleInput();
	void ApplyMovement();
	void UpdateAnimationState();
	void SetState(State newState);
	void UpdateAnimation();

	// Films
	anim::AnimationFilm* m_IdleFilm = nullptr;
	anim::AnimationFilm* m_IdleLoopFilm = nullptr;
	anim::AnimationFilm* m_WalkFilm = nullptr;
	anim::AnimationFilm* m_RunFilm = nullptr;
	anim::AnimationFilm* m_BallFilm = nullptr;

	// Animations
	anim::FrameRangeAnimation* m_IdleAnim = nullptr;
	anim::FrameRangeAnimation* m_IdleLoopAnim = nullptr;
	anim::FrameRangeAnimation* m_WalkAnim = nullptr;
	anim::FrameRangeAnimation* m_RunAnim = nullptr;
	anim::FrameRangeAnimation* m_BallAnim = nullptr;

	// Animator
	anim::FrameRangeAnimator* m_Animator = nullptr;

	// State
	State m_State = State::BALL;  // Start in ball state since spawning in air
	Direction m_Direction = Direction::RIGHT;
	bool m_OnGround = false;  // Start false so gravity check runs immediately

	// Walk-to-run tracking
	TimeStamp m_WalkStartTime = 0;
	Direction m_WalkDirection = Direction::RIGHT;
	static constexpr int WALK_TO_RUN_MS = 1000;  // 1 second before switching to run

	// Idle loop tracking
	TimeStamp m_IdleStartTime = 0;
	bool m_PlayingIdleLoop = false;
	static constexpr int IDLE_TO_LOOP_MS = 1000;  // 1 second before switching to idle loop

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
	static constexpr int RUN_SPEED = 4;  // ~1.4x walk speed
	static constexpr int JUMP_VELOCITY = -12;
	static constexpr int GRAVITY = 1;
	static constexpr int MAX_FALL_SPEED = 8;

	// Shared sound effects
	static sound::SFX s_JumpSound;
	static sound::SFX s_HitSound;
	static sound::SFX s_TunnelSound;

	// Invincibility after being hit
	int m_InvincibilityFrames = 0;
	static constexpr int INVINCIBILITY_DURATION = 120;  // ~2 seconds at 60fps

	// Ring scatter callback
	RingScatterCallback m_RingScatterCallback;

	// Tunnel path system
	const std::vector<anim::TunnelPath>* m_TunnelPaths = nullptr;
	const anim::TunnelPath* m_CurrentTunnelPath = nullptr;
	anim::TunnelPathAnimator* m_TunnelAnimator = nullptr;
	bool m_InTunnel = false;
};
