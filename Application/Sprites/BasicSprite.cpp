#include "BasicSprite.h"
#include "Animations/FrameRangeAnimation.h"
#include "Core/SystemClock.h"
#include "Core/Input.h"
#include "Utilities/MoverUtilities.h"
#include "Game/GameStats.h"

// Static member initialization
sound::SFX Sonic::s_JumpSound = nullptr;
sound::SFX Sonic::s_HitSound = nullptr;

Sonic::Sonic(int x, int y, scene::GridMap* map, scene::TileLayer* layer)
	: scene::Sprite(x, y, "Sonic"), m_Grid(map), m_TileLayer(layer)
{
	// Load sound effects (shared across all instances)
	if (!s_JumpSound)
	{
		s_JumpSound = sound::LoadSFX(ASSETS "/Sounds/sonicjump.mp3");
	}
	if (!s_HitSound)
	{
		s_HitSound = sound::LoadSFX(ASSETS "/Sounds/stormtrooper-scream.mp3");
	}

	// Load films from AnimationFilmHolder
	m_IdleFilm = const_cast<anim::AnimationFilm*>(
		anim::AnimationFilmHolder::Get().GetFilm("sonic.idle")
	);
	m_WalkFilm = const_cast<anim::AnimationFilm*>(
		anim::AnimationFilmHolder::Get().GetFilm("sonic.walk")
	);
	m_BallFilm = const_cast<anim::AnimationFilm*>(
		anim::AnimationFilmHolder::Get().GetFilm("sonic.ball")
	);

	// Set initial film and frame (ball since spawning in air)
	SetFilm(m_BallFilm);
	m_FrameNo = 255; // Force update
	SetFrame(0);
	SetVisibility(true);

	// Create animations
	// FrameRangeAnimation(id, startFrame, endFrame, reps, dx, dy, delayMs)
	m_IdleAnim = new anim::FrameRangeAnimation("sonic.idle.anim", 0, 0, 0, 0, 0, 100);
	m_IdleAnim->SetForever();

	m_WalkAnim = new anim::FrameRangeAnimation("sonic.walk.anim", 0, 5, 0, 0, 0, 100);
	m_WalkAnim->SetForever();

	m_BallAnim = new anim::FrameRangeAnimation("sonic.ball.anim", 0, 4, 0, 0, 0, 100);
	m_BallAnim->SetForever();

	// Create animator with OnAction callback
	m_Animator = new anim::FrameRangeAnimator();
	m_Animator->SetOnAction(
		[this](anim::Animator* animator, anim::Animation* animation) {
			auto* frameAnimator = static_cast<anim::FrameRangeAnimator*>(animator);
			this->SetFrame(static_cast<byte>(frameAnimator->GetCurrFrame()));
		}
	);

	// Setup bounding area
	m_BoundingArea = new physics::BoundingBox(x, y, x + 24, y + 32);

	// Grid Y offset - the collision grid starts at world y=256
	constexpr int GRID_Y_OFFSET = 0;  // Full-height 1x1 grid covers entire level

	// Setup mover for collision detection (with grid offset)
	SetMover(MakeSpriteGridLayerMover(map, this, GRID_Y_OFFSET));

	// Enable motion quantizer for step-by-step collision detection
	// Range of 2 pixels gives finer movement steps for smoother terrain following
	m_Quantizer.SetRange(2, 2);

	// Setup gravity handler
	GetGravityHandler().SetGravityAddected(true);

	GetGravityHandler().SetOnSolidGround([this](Rect& r) {
		// Adjust rect for grid Y offset
		constexpr int GRID_Y_OFFSET = 0;  // Full-height 1x1 grid covers entire level
		Rect gridRect = { r.x, r.y - GRID_Y_OFFSET, r.w, r.h };
		// Check if bottom edge is within grid bounds (not just top edge)
		if (gridRect.y + gridRect.h <= 0) return false;
		return m_Grid->IsOnSolidGround(gridRect);
	});

	// Note: We don't use the gravity handler callbacks for state management
	// because the ground check flickers rapidly while walking. Instead,
	// we rely on collision detection to stop downward movement, and check
	// VelocityY to determine animation state.
	// Set empty callbacks to avoid null pointer issues.
	GetGravityHandler().SetOnStartFalling([]() {});
	GetGravityHandler().SetOnStopFalling([]() {});

	// Start ball animation (since spawning in air)
	m_Animator->Start(m_BallAnim, core::SystemClock::Get().GetCurrTime());
}

Sonic::~Sonic()
{
	// Stop animator before destroying to avoid assertion failure
	if (m_Animator) m_Animator->Stop();

	if (m_IdleAnim) m_IdleAnim->Destroy();
	if (m_WalkAnim) m_WalkAnim->Destroy();
	if (m_BallAnim) m_BallAnim->Destroy();
	if (m_Animator) m_Animator->Destroy();
}

void Sonic::Update()
{
	HandleInput();
	ApplyMovement();
	UpdateAnimationState();
	UpdateBoundingArea();

	// Decrement invincibility frames
	if (m_InvincibilityFrames > 0)
	{
		--m_InvincibilityFrames;

		// Flicker visibility during invincibility
		SetVisibility((m_InvincibilityFrames % 8) < 4);
	}
	else
	{
		SetVisibility(true);
	}
}

void Sonic::OnHit()
{
	// Ignore if already invincible
	if (m_InvincibilityFrames > 0)
		return;

	// Play hit sound
	if (s_HitSound)
	{
		sound::PlaySFX(s_HitSound);
	}

	// Lose a life
	GameStats::Get().LoseLife();

	// Start invincibility frames
	m_InvincibilityFrames = INVINCIBILITY_DURATION;

	// Apply knockback (bounce upward and backward)
	m_VelocityY = -8;
	m_VelocityX = (m_Direction == Direction::RIGHT) ? -5 : 5;
	m_OnGround = false;
	SetState(State::BALL);
}

void Sonic::BounceOffEnemy()
{
	// Small upward boost when killing an enemy (only affects vertical momentum)
	m_VelocityY = -10;
	m_OnGround = false;
}

void Sonic::UpdateBoundingArea()
{
	auto* box = static_cast<physics::BoundingBox*>(m_BoundingArea);
	box->x1 = m_X;
	box->y1 = m_Y;
	box->x2 = m_X + 24;
	box->y2 = m_Y + 32;
}

void Sonic::HandleInput()
{
	int targetVX = 0;

	// Horizontal movement
	if (core::Input::IsKeyPressed(io::Key::Left) ||
		core::Input::IsKeyPressed(io::Key::A))
	{
		targetVX = -WALK_SPEED;
		m_Direction = Direction::LEFT;
		SetFlipHorizontal(true);
	}
	else if (core::Input::IsKeyPressed(io::Key::Right) ||
			 core::Input::IsKeyPressed(io::Key::D))
	{
		targetVX = WALK_SPEED;
		m_Direction = Direction::RIGHT;
		SetFlipHorizontal(false);
	}

	// Jump
	if ((core::Input::IsKeyPressed(io::Key::Space) ||
		 core::Input::IsKeyPressed(io::Key::W)) && m_OnGround && !m_JumpHeld)
	{
		m_JumpHeld = true;
		m_VelocityY = JUMP_VELOCITY;
		m_OnGround = false;
		SetState(State::BALL);
		if (s_JumpSound)
		{
			sound::PlaySFX(s_JumpSound);
		}
	}

	// Reset jump hold when key released
	if (!core::Input::IsKeyPressed(io::Key::Space) &&
		!core::Input::IsKeyPressed(io::Key::W))
	{
		m_JumpHeld = false;
	}

	m_VelocityX = targetVX;
}

void Sonic::ApplyMovement()
{
	// Apply gravity only when not on ground
	// This prevents fighting between gravity and ground-following
	if (!m_OnGround)
	{
		// Apply gravity every other frame for smoother, less aggressive falling
		m_GravityFrame++;
		if (m_GravityFrame >= 2)
		{
			m_GravityFrame = 0;
			m_VelocityY += GRAVITY;
			if (m_VelocityY > MAX_FALL_SPEED)
				m_VelocityY = MAX_FALL_SPEED;
		}
	}
	else
	{
		m_GravityFrame = 0;  // Reset when on ground
	}

	// Apply movement (collision and ground-following handled by mover callback)
	if (m_VelocityX != 0 || m_VelocityY != 0)
	{
		Move(m_VelocityX, m_VelocityY);
	}

	// Check ground contact after movement using the gravity handler's ground check
	constexpr int GRID_Y_OFFSET = 0;  // Full-height 1x1 grid covers entire level
	Rect groundCheckRect = { m_X, m_Y - GRID_Y_OFFSET, m_FrameBox.w, m_FrameBox.h };

	// Check if bottom edge is within grid bounds
	bool wasOnGround = m_OnGround;
	if (groundCheckRect.y + groundCheckRect.h > 0)
	{
		m_OnGround = m_Grid->IsOnSolidGround(groundCheckRect);
	}
	else
	{
		m_OnGround = false;
	}

	// Landing: just became grounded
	if (m_OnGround && !wasOnGround)
	{
		m_VelocityY = 0;
	}

	// Walking off edge: just left ground without jumping
	if (!m_OnGround && wasOnGround && m_VelocityY >= 0)
	{
		// Start falling - gravity will be applied next frame
		m_VelocityY = 1;
	}
}

void Sonic::UpdateAnimationState()
{
	// Animation state based on ground contact, not velocity
	// This prevents flickering when walking on slopes
	if (!m_OnGround)
	{
		// In the air (jumping or falling)
		SetState(State::BALL);
	}
	else
	{
		// On ground
		if (m_VelocityX != 0)
		{
			SetState(State::WALKING);
		}
		else
		{
			SetState(State::IDLE);
		}
	}
}

void Sonic::SetState(State newState)
{
	if (m_State == newState)
		return;

	m_State = newState;
	UpdateAnimation();
}

void Sonic::UpdateAnimation()
{
	m_Animator->Stop();

	anim::AnimationFilm* newFilm = nullptr;
	anim::FrameRangeAnimation* newAnim = nullptr;

	switch (m_State)
	{
		case State::IDLE:
			newFilm = m_IdleFilm;
			newAnim = m_IdleAnim;
			break;
		case State::WALKING:
			newFilm = m_WalkFilm;
			newAnim = m_WalkAnim;
			break;
		case State::BALL:
			newFilm = m_BallFilm;
			newAnim = m_BallAnim;
			break;
	}

	if (newFilm && newAnim)
	{
		// Get current height before changing film
		int oldHeight = m_FrameBox.h;

		SetFilm(newFilm);
		m_FrameNo = 255;
		SetFrame(0);

		// Get new height after changing film
		int newHeight = m_FrameBox.h;

		// Adjust Y position to keep bottom edge at same position
		// This prevents clipping through ground when animation changes to taller sprite
		if (newHeight > oldHeight && oldHeight > 0)
		{
			m_Y -= (newHeight - oldHeight);
		}

		m_Animator->Start(newAnim, core::SystemClock::Get().GetCurrTime());
	}
}
