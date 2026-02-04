#include "Sprites/Ring.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"
#include "Physics/BoundingArea.h"
#include "Game/GameStats.h"

// Static member initialization
sound::SFX Ring::s_CollectSound = nullptr;

Ring::Ring(int x, int y)
    : scene::Sprite(x, y, "Ring")
{
    // Load the collection sound effect once (shared across all rings)
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
    // Force frame box initialization (SetFrame(0) doesn't update if m_FrameNo is already 0)
    m_FrameNo = 255;  // Set to invalid value first
    SetFrame(0);      // Now this will actually update m_FrameBox
    SetVisibility(true);
    SetHasDirectMotion(true);  // Ring doesn't need physics movement

    // Setup bounding area for collision detection (16x16 ring)
    SetBoundingArea(new physics::BoundingBox(x, y, x + 16, y + 16));

    // Create the spinning animation (loops forever)
    m_SpinAnimation = new anim::FrameRangeAnimation(
        "ring.spin.anim",
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
        "ring.collected.anim",
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

    // Note: OnFinish callback is set in OnCollected() to avoid being triggered
    // when stopping the spin animation (Stop() calls Finish() which triggers OnFinish)
}

Ring::~Ring()
{
    // Stop the animator first (must be stopped before it can be destroyed)
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

void Ring::StartAnimation()
{
    if (m_Animator && m_SpinAnimation && !m_Collected)
    {
        m_Animator->Start(m_SpinAnimation, core::SystemClock::Get().GetCurrTime());
    }
}

void Ring::StopAnimation()
{
    if (m_Animator)
    {
        m_Animator->Stop();
    }
}

void Ring::OnCollected()
{
    if (m_Collected)
        return;

    m_Collected = true;

    // Stop the spin animation
    StopAnimation();

    // Switch to the collected film and reset frame
    SetFilm(m_CollectedFilm);
    m_FrameNo = 255;  // Force frame box update
    SetFrame(0);

    // Set OnFinish callback NOW (after Stop has already triggered)
    // This ensures we only hide the ring when the collected animation actually finishes
    m_Animator->SetOnFinish(
        [this](anim::Animator* animator)
        {
            m_CollectionFinished = true;
            SetVisibility(false);
        }
    );

    // Start the collected animation (plays once then triggers OnFinish)
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

bool Ring::IsCollected() const
{
    return m_Collected;
}

bool Ring::IsCollectionFinished() const
{
    return m_CollectionFinished;
}
