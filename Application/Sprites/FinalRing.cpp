#include "Sprites/FinalRing.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"
#include "Physics/BoundingArea.h"

FinalRing::FinalRing(int x, int y)
    : scene::Sprite(x, y, "FinalRing")
{
    // Get both films from the holder (must be loaded first via FilmParser)
    m_SpinFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("final_ring.spin")
    );
    m_CollectedFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("final_ring.collected")
    );

    SetFilm(m_SpinFilm);
    // Force frame box initialization
    m_FrameNo = 255;
    SetFrame(0);
    SetVisibility(true);
    SetHasDirectMotion(true);

    // Setup bounding area for collision detection (64x64 ring)
    SetBoundingArea(new physics::BoundingBox(x, y, x + 64, y + 64));

    // Create the spinning animation (loops forever)
    m_SpinAnimation = new anim::FrameRangeAnimation(
        "final_ring.spin.anim",
        SPIN_START_FRAME,
        SPIN_END_FRAME,
        0,              // reps (0 for infinite with SetForever)
        0,              // dx (no movement)
        0,              // dy (no movement)
        SPIN_DELAY_MS   // delay between frames
    );
    m_SpinAnimation->SetForever();

    // Create the collected animation (plays once)
    m_CollectedAnimation = new anim::FrameRangeAnimation(
        "final_ring.collected.anim",
        COLLECTED_START_FRAME,
        COLLECTED_END_FRAME,
        1,                  // reps = 1 (play once)
        0,                  // dx (no movement)
        0,                  // dy (no movement)
        COLLECTED_DELAY_MS
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

FinalRing::~FinalRing()
{
    // Stop the animator first
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

    // Clean up music track
    if (m_StageClearMusic)
    {
        sound::DestroyTrack(m_StageClearMusic);
        m_StageClearMusic = nullptr;
    }
}

void FinalRing::StartAnimation()
{
    if (m_Animator && m_SpinAnimation && !m_Collected)
    {
        m_Animator->Start(m_SpinAnimation, core::SystemClock::Get().GetCurrTime());
    }
}

void FinalRing::StopAnimation()
{
    if (m_Animator)
    {
        m_Animator->Stop();
    }
}

void FinalRing::OnCollected()
{
    if (m_Collected)
        return;

    m_Collected = true;

    // Stop the spin animation
    StopAnimation();

    // Switch to the collected film and reset frame
    SetFilm(m_CollectedFilm);
    m_FrameNo = 255;
    SetFrame(0);

    // Set OnFinish callback for when collected animation completes
    m_Animator->SetOnFinish(
        [this](anim::Animator* animator)
        {
            m_CollectionFinished = true;
            SetVisibility(false);

            // Trigger game end callback immediately - GameScene handles timing
            if (m_OnGameEnd)
            {
                m_OnGameEnd();
            }
        }
    );

    // Start the collected animation
    if (m_Animator && m_CollectedAnimation)
    {
        m_Animator->Start(m_CollectedAnimation, core::SystemClock::Get().GetCurrTime());
    }

    // Load and play stage clear music
    m_StageClearMusic = sound::LoadTrack(ASSETS "/Sounds/stage-clear-theme.mp3");
    if (m_StageClearMusic)
    {
        sound::PlayTrack(m_StageClearMusic, 0);  // Play once
    }
}

bool FinalRing::IsCollected() const
{
    return m_Collected;
}

bool FinalRing::IsCollectionFinished() const
{
    return m_CollectionFinished;
}
