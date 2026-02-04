#include "Sprites/ScatteredRing.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"
#include "Physics/BoundingArea.h"
#include "Game/GameStats.h"

// Static member initialization
sound::SFX ScatteredRing::s_CollectSound = nullptr;

ScatteredRing::ScatteredRing(int x, int y, float velocityX, float velocityY)
    : scene::Sprite(x, y, "ScatteredRing")
    , m_PosX(static_cast<float>(x))
    , m_PosY(static_cast<float>(y))
    , m_VelocityX(velocityX)
    , m_VelocityY(velocityY)
{
    // Load the collection sound effect once (shared across all scattered rings)
    if (!s_CollectSound)
    {
        s_CollectSound = sound::LoadSFX(ASSETS "/Sounds/coin-collect.mp3");
    }

    // Get both films from the holder (must be loaded first via FilmParser)
    m_SpinFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("ring.spin")
    );
    m_CollectedFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("ring.collected")
    );

    SetFilm(m_SpinFilm);
    m_FrameNo = 255;  // Force frame box update
    SetFrame(0);
    SetVisibility(true);
    SetHasDirectMotion(true);

    // Setup bounding area for collision detection (16x16 ring)
    SetBoundingArea(new physics::BoundingBox(x, y, x + 16, y + 16));

    // Create the spinning animation (loops forever)
    m_SpinAnimation = new anim::FrameRangeAnimation(
        "scattered.ring.spin.anim",
        SPIN_START_FRAME,
        SPIN_END_FRAME,
        0,              // reps (0 for infinite with SetForever)
        0,              // dx (no movement)
        0,              // dy (no movement)
        SPIN_DELAY_MS   // delay between frames
    );
    m_SpinAnimation->SetForever();

    // Create the collected animation (plays once, faster)
    m_CollectedAnimation = new anim::FrameRangeAnimation(
        "scattered.ring.collected.anim",
        COLLECTED_START_FRAME,
        COLLECTED_END_FRAME,
        1,                  // reps = 1 (play once)
        0,                  // dx (no movement)
        0,                  // dy (no movement)
        COLLECTED_DELAY_MS  // faster delay for sparkle effect
    );

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

ScatteredRing::~ScatteredRing()
{
    StopAnimation();

    if (m_SpinAnimation)
    {
        m_SpinAnimation->Destroy();
        m_SpinAnimation = nullptr;
    }

    if (m_CollectedAnimation)
    {
        m_CollectedAnimation->Destroy();
        m_CollectedAnimation = nullptr;
    }

    if (m_Animator)
    {
        m_Animator->Destroy();
        m_Animator = nullptr;
    }
}

void ScatteredRing::StartAnimation()
{
    if (m_Animator && m_SpinAnimation && !m_Collected)
    {
        m_Animator->Start(m_SpinAnimation, core::SystemClock::Get().GetCurrTime());
    }
}

void ScatteredRing::StopAnimation()
{
    if (m_Animator)
    {
        m_Animator->Stop();
    }
}

void ScatteredRing::Update()
{
    if (m_Collected || m_CollectionFinished)
        return;

    ++m_FrameCount;

    // Check if expired
    if (m_FrameCount >= LIFETIME_FRAMES)
    {
        SetVisibility(false);
        return;
    }

    // Handle flashing before despawn
    if (m_FrameCount >= FLASH_START_FRAMES)
    {
        // Flash visibility on/off
        bool visible = ((m_FrameCount - FLASH_START_FRAMES) / FLASH_INTERVAL) % 2 == 0;
        SetVisibility(visible);
    }

    // Apply gravity
    m_VelocityY += GRAVITY;

    // Update position
    m_PosX += m_VelocityX;
    m_PosY += m_VelocityY;

    // Simple bounce off bottom of screen (approximate ground level)
    // In a full implementation, this would use the GridMap for collision
    // For now, bounce at spawn Y level + some offset, or a fixed floor
    if (m_VelocityY > 0 && m_BounceCount < MAX_BOUNCES)
    {
        // Check if we've fallen past a reasonable "ground" level
        // Using a simple approach: bounce when moving downward significantly
        // This could be improved by checking actual tile collisions
        if (m_PosY > static_cast<float>(m_Y + 100))  // Bounce after falling 100 pixels
        {
            m_VelocityY = -m_VelocityY * BOUNCE_DAMPING;
            m_VelocityX *= BOUNCE_DAMPING;
            ++m_BounceCount;

            // Clamp to bounce position
            m_PosY = static_cast<float>(m_Y + 100);
        }
    }

    // Update sprite position
    m_X = static_cast<int>(m_PosX);
    m_Y = static_cast<int>(m_PosY);

    // Update bounding area
    UpdateBoundingArea();
}

void ScatteredRing::UpdateBoundingArea()
{
    auto* box = const_cast<physics::BoundingBox*>(
        static_cast<const physics::BoundingBox*>(GetBoundingArea())
    );
    if (box)
    {
        box->x1 = m_X;
        box->y1 = m_Y;
        box->x2 = m_X + 16;
        box->y2 = m_Y + 16;
    }
}

void ScatteredRing::OnCollected()
{
    if (m_Collected || !IsCollectable())
        return;

    m_Collected = true;

    // Stop the spin animation
    StopAnimation();

    // Switch to the collected film and reset frame
    SetFilm(m_CollectedFilm);
    m_FrameNo = 255;
    SetFrame(0);

    // Set OnFinish callback
    m_Animator->SetOnFinish(
        [this](anim::Animator* animator)
        {
            m_CollectionFinished = true;
            SetVisibility(false);
        }
    );

    // Start the collected animation
    if (m_Animator && m_CollectedAnimation)
    {
        m_Animator->Start(m_CollectedAnimation, core::SystemClock::Get().GetCurrTime());
    }

    // Play collection sound effect
    if (s_CollectSound)
    {
        sound::PlaySFX(s_CollectSound);
    }

    // Add to player's ring count and score
    GameStats::Get().AddRing();
}

bool ScatteredRing::IsCollectable() const
{
    return !m_Collected && m_FrameCount >= COLLECT_DELAY_FRAMES;
}

bool ScatteredRing::IsCollected() const
{
    return m_Collected;
}

bool ScatteredRing::IsExpired() const
{
    return m_FrameCount >= LIFETIME_FRAMES;
}

bool ScatteredRing::IsCollectionFinished() const
{
    return m_CollectionFinished;
}
