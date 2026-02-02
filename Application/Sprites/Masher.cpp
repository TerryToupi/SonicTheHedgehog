#include "Sprites/Masher.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"
#include "Physics/BoundingArea.h"

// Static member initialization
sound::SFX Masher::s_DeathSound = nullptr;

Masher::Masher(int x, int y)
    : scene::Sprite(x, y, "Masher"), m_SpawnX(x), m_SpawnY(y)
{
    // Load death sound effect (shared across all instances)
    if (!s_DeathSound)
    {
        s_DeathSound = sound::LoadSFX(ASSETS "/Sounds/enemy-death.mp3");
    }

    // Get the animation film from the holder
    m_Film = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("masher.animation")
    );

    SetFilm(m_Film);
    m_FrameNo = 255;  // Force frame box update
    SetFrame(0);
    SetVisibility(true);
    SetHasDirectMotion(true);  // We handle our own movement

    // Setup bounding area for collision detection (32x32 sprite)
    SetBoundingArea(new physics::BoundingBox(x, y, x + 32, y + 32));

    // Create the mouth animation (loops forever)
    m_Animation = new anim::FrameRangeAnimation(
        "masher.anim",
        0,              // startFrame
        1,              // endFrame (2 frames: mouth closed, mouth open)
        0,              // reps (0 for infinite with SetForever)
        0,              // dx (no movement from animation)
        0,              // dy (no movement from animation)
        ANIM_DELAY_MS   // delay between frames
    );
    m_Animation->SetForever();

    // Create the animator
    m_Animator = new anim::FrameRangeAnimator();

    // Set the OnAction callback to update sprite frame
    m_Animator->SetOnAction(
        [this](anim::Animator* animator, anim::Animation* animation)
        {
            auto* frameAnimator = static_cast<anim::FrameRangeAnimator*>(animator);
            this->SetFrame(static_cast<byte>(frameAnimator->GetCurrFrame()));
        }
    );

    // Start with a short delay before first jump
    m_JumpCooldown = JUMP_COOLDOWN / 2;
}

Masher::~Masher()
{
    StopAnimation();

    if (m_Animation)
    {
        m_Animation->Destroy();
        m_Animation = nullptr;
    }

    if (m_Animator)
    {
        m_Animator->Destroy();
        m_Animator = nullptr;
    }
}

void Masher::StartAnimation()
{
    if (m_Animator && m_Animation && m_IsAlive)
    {
        m_Animator->Start(m_Animation, core::SystemClock::Get().GetCurrTime());
    }
}

void Masher::StopAnimation()
{
    if (m_Animator)
    {
        m_Animator->Stop();
    }
}

void Masher::Update()
{
    if (!m_IsAlive)
        return;

    if (m_IsJumping)
    {
        ApplyGravity();
    }
    else
    {
        // Waiting to jump
        m_JumpCooldown--;
        if (m_JumpCooldown <= 0)
        {
            StartJump();
        }
    }

    // Update bounding area to match current position
    auto* box = static_cast<physics::BoundingBox*>(m_BoundingArea);
    box->x1 = m_X;
    box->y1 = m_Y;
    box->x2 = m_X + 32;
    box->y2 = m_Y + 32;
}

void Masher::StartJump()
{
    m_IsJumping = true;
    m_VelocityY = JUMP_VELOCITY;
    m_GravityFrame = 0;
}

void Masher::ApplyGravity()
{
    // Apply gravity every GRAVITY_FRAMES frames for floatier movement
    m_GravityFrame++;
    if (m_GravityFrame >= GRAVITY_FRAMES)
    {
        m_GravityFrame = 0;
        m_VelocityY += GRAVITY;
        if (m_VelocityY > MAX_FALL_SPEED)
            m_VelocityY = MAX_FALL_SPEED;
    }

    // Move
    m_Y += m_VelocityY;

    // Check if we've returned to spawn position (or below)
    if (m_VelocityY > 0 && m_Y >= m_SpawnY)
    {
        // Reset position and start cooldown
        m_Y = m_SpawnY;
        m_IsJumping = false;
        m_VelocityY = 0;
        m_JumpCooldown = JUMP_COOLDOWN;
    }
}

void Masher::Kill()
{
    if (!m_IsAlive)
        return;

    m_IsAlive = false;
    StopAnimation();
    SetVisibility(false);

    // Play death sound
    if (s_DeathSound)
    {
        sound::PlaySFX(s_DeathSound);
    }
}
