#include "Sprites/Checkpoint.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"
#include "Rendering/Bitmap.h"
#include "Physics/BoundingArea.h"
#include "Game/GameStats.h"

#include <algorithm>

// Static member initialization
sound::SFX Checkpoint::s_CheckpointSound = nullptr;

Checkpoint::Checkpoint(int x, int y)
    : scene::Sprite(x, y, "Checkpoint")
{
    // Load checkpoint sound effect (shared across all instances)
    if (!s_CheckpointSound)
    {
        s_CheckpointSound = sound::LoadSFX(ASSETS "/Sounds/sonic-checkpoint.mp3");
    }
    // Get films from the holder (must be loaded first via FilmParser)
    m_BaseFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("checkpoint.base")
    );
    m_BlueOrbFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("checkpoint.orb.blue")
    );
    m_RedOrbFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("checkpoint.orb.red")
    );
    m_TriggeredFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("checkpoint.triggered")
    );

    // Set base film as current (used for positioning/box calculations)
    SetFilm(m_BaseFilm);
    m_FrameNo = 255;  // Force frame box update
    SetFrame(0);
    SetVisibility(true);
    SetHasDirectMotion(true);  // Checkpoint doesn't need physics movement

    // Setup bounding area for collision detection
    // Covers full height: orb at y-24 to base bottom at y+48
    SetBoundingArea(new physics::BoundingBox(x, y - 24, x + 16, y + 48));

    // Create the triggered animation (plays twice)
    m_TriggeredAnimation = new anim::FrameRangeAnimation(
        "checkpoint.triggered.anim",
        TRIGGERED_START_FRAME,
        TRIGGERED_END_FRAME,
        TRIGGERED_REPS,     // Play twice
        0,                  // dx (no movement)
        0,                  // dy (no movement)
        TRIGGERED_DELAY_MS
    );

    // Create the animator (but don't start it yet - checkpoint starts idle)
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

Checkpoint::~Checkpoint()
{
    // Stop the animator if running
    if (m_Animator)
    {
        m_Animator->Stop();
    }

    if (m_TriggeredAnimation)
    {
        m_TriggeredAnimation->Destroy();
        m_TriggeredAnimation = nullptr;
    }

    if (m_Animator)
    {
        m_Animator->Destroy();
        m_Animator = nullptr;
    }
}

void Checkpoint::Display(gfx::Bitmap dest, const Rect& dpyArea, const gfx::Clipper& clipper) const
{
    if (!IsVisible())
        return;

    if (!m_Triggered || m_AnimationFinished)
    {
        // Composite state: render base + orb (blue if idle, red if finished)
        // Use visibility check then calculate screen positions directly

        Rect baseFrame = m_BaseFilm->GetFrameBox(0);
        const Point& baseOffset = m_BaseFilm->GetFrameOffset(0);

        // Select orb based on state
        anim::AnimationFilm* orbFilm = m_AnimationFinished ? m_RedOrbFilm : m_BlueOrbFilm;
        Rect orbFrame = orbFilm->GetFrameBox(0);
        const Point& orbOffset = orbFilm->GetFrameOffset(0);

        // Calculate combined bounding box for visibility check
        int baseLocalX = baseOffset.x;
        int baseLocalY = baseOffset.y;
        int orbLocalX = ORB_OFFSET_X + orbOffset.x;
        int orbLocalY = ORB_OFFSET_Y + orbOffset.y;

        int minX = std::min(baseLocalX, orbLocalX);
        int minY = std::min(baseLocalY, orbLocalY);
        int maxX = std::max(baseLocalX + baseFrame.w, orbLocalX + orbFrame.w);
        int maxY = std::max(baseLocalY + baseFrame.h, orbLocalY + orbFrame.h);

        Rect combinedWorldBox = {
            m_X + minX,
            m_Y + minY,
            maxX - minX,
            maxY - minY
        };

        // Just check if any part is visible
        Rect clippedBox;
        Point dpyPos;
        if (clipper.Clip(combinedWorldBox, dpyArea, &dpyPos, &clippedBox))
        {
            // Screen position formula: screenX = dpyPos.x + (worldX - combinedWorldBox.x) + clippedBox.x
            // This accounts for the clipper's coordinate system where clippedBox.x can be negative

            int baseScreenX = dpyPos.x + (m_X + baseLocalX - combinedWorldBox.x) + clippedBox.x;
            int baseScreenY = dpyPos.y + (m_Y + baseLocalY - combinedWorldBox.y) + clippedBox.y;
            int orbScreenX = dpyPos.x + (m_X + orbLocalX - combinedWorldBox.x) + clippedBox.x;
            int orbScreenY = dpyPos.y + (m_Y + orbLocalY - combinedWorldBox.y) + clippedBox.y;

            // Blit full frames
            gfx::BitmapBlit(m_BaseFilm->GetBitmap(), baseFrame, dest, {baseScreenX, baseScreenY});
            gfx::BitmapBlit(orbFilm->GetBitmap(), orbFrame, dest, {orbScreenX, orbScreenY});
        }
    }
    else
    {
        // Animating state: custom rendering to handle frame offsets at screen edges
        // (Sprite::Display has issues with offsets + clipping)

        Rect frame = m_CurrFilm->GetFrameBox(m_FrameNo);
        const Point& offset = m_CurrFilm->GetFrameOffset(m_FrameNo);

        // World box includes the offset for proper visibility check
        Rect worldBox = {
            m_X + offset.x,
            m_Y + offset.y,
            frame.w,
            frame.h
        };

        Rect clippedBox;
        Point dpyPos;
        if (clipper.Clip(worldBox, dpyArea, &dpyPos, &clippedBox))
        {
            // Screen position using same formula as composite state
            int screenX = dpyPos.x + (m_X + offset.x - worldBox.x) + clippedBox.x;
            int screenY = dpyPos.y + (m_Y + offset.y - worldBox.y) + clippedBox.y;

            gfx::BitmapBlit(m_CurrFilm->GetBitmap(), frame, dest, {screenX, screenY});
        }
    }
}

void Checkpoint::OnTriggered()
{
    if (m_Triggered)
        return;

    m_Triggered = true;

    // Switch to the triggered film (red orb spinning - complete frames)
    SetFilm(m_TriggeredFilm);
    m_FrameNo = 255;  // Force frame box update
    SetFrame(0);

    // Set OnFinish callback to mark animation as complete
    m_Animator->SetOnFinish(
        [this](anim::Animator* animator)
        {
            m_AnimationFinished = true;
            // Stay on the last frame (red orb on top, stationary)
        }
    );

    // Start the triggered animation
    if (m_Animator && m_TriggeredAnimation)
    {
        m_Animator->Start(m_TriggeredAnimation, core::SystemClock::Get().GetCurrTime());
    }

    // Play checkpoint sound effect
    if (s_CheckpointSound)
    {
        sound::PlaySFX(s_CheckpointSound);
    }

    // Add checkpoint bonus to score
    GameStats::Get().AddScore(100);
}

bool Checkpoint::IsTriggered() const
{
    return m_Triggered;
}

bool Checkpoint::IsAnimationFinished() const
{
    return m_AnimationFinished;
}
