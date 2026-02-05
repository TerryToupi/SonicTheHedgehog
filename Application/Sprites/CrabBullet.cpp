#include "Sprites/CrabBullet.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"
#include "Physics/BoundingArea.h"

CrabBullet::CrabBullet(int x, int y, int velocityX, int velocityY, scene::GridMap* grid)
    : scene::Sprite(x, y, "CrabBullet"), m_VelocityX(velocityX), m_VelocityY(velocityY), m_Grid(grid)
{
    // Get the animation film from the holder
    m_Film = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("crab.bullet")
    );

    SetFilm(m_Film);
    m_FrameNo = 255;  // Force frame box update
    SetFrame(0);
    SetVisibility(true);
    SetHasDirectMotion(true);  // We handle our own movement

    // Setup bounding area for collision detection (16x16 sprite)
    SetBoundingArea(new physics::BoundingBox(x, y, x + 16, y + 16));

    // Create the animation (loops forever)
    m_Animation = new anim::FrameRangeAnimation(
        "crab.bullet.anim",
        0,              // startFrame
        1,              // endFrame (2 frames)
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
}

CrabBullet::~CrabBullet()
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

void CrabBullet::StartAnimation()
{
    if (m_Animator && m_Animation && m_IsActive)
    {
        m_Animator->Start(m_Animation, core::SystemClock::Get().GetCurrTime());
    }
}

void CrabBullet::StopAnimation()
{
    if (m_Animator)
    {
        m_Animator->Stop();
    }
}

void CrabBullet::Update()
{
    if (!m_IsActive)
        return;

    // Update lifetime
    m_LifetimeFrames++;
    if (m_LifetimeFrames >= MAX_LIFETIME_FRAMES)
    {
        m_IsActive = false;
        SetVisibility(false);
        return;
    }

    // Apply gravity
    m_GravityFrame++;
    if (m_GravityFrame >= GRAVITY_FRAMES)
    {
        m_GravityFrame = 0;
        m_VelocityY += GRAVITY;
        if (m_VelocityY > MAX_FALL_SPEED)
            m_VelocityY = MAX_FALL_SPEED;
    }

    // Move the bullet
    m_X += m_VelocityX;
    m_Y += m_VelocityY;

    // Check terrain collision
    if (m_Grid)
    {
        // Check center point of bullet against terrain
        int centerX = m_X + 8;
        int centerY = m_Y + 8;

        // Check if the bullet hit solid terrain
        GridIndex gridValue = m_Grid->GetGridTile(centerX, centerY);
        if (gridValue != scene::GRID_EMPTY_TILE)
        {
            m_IsActive = false;
            SetVisibility(false);
            return;
        }
    }

    // Update bounding area
    auto* box = static_cast<physics::BoundingBox*>(m_BoundingArea);
    box->x1 = m_X;
    box->y1 = m_Y;
    box->x2 = m_X + 16;
    box->y2 = m_Y + 16;
}
