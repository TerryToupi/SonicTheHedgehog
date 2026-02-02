#pragma once

#include "Scene/Sprite.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"
#include "Sound/Sound.h"

class Checkpoint : public scene::Sprite
{
public:
    Checkpoint(int x, int y);
    ~Checkpoint();

    void OnTriggered();
    bool IsTriggered() const;
    bool IsAnimationFinished() const;

    // Override to handle composite rendering in idle state
    void Display(gfx::Bitmap dest, const Rect& dpyArea, const gfx::Clipper& clipper) const override;

private:
    // Films (owned by AnimationFilmHolder, not us)
    anim::AnimationFilm* m_BaseFilm = nullptr;      // Pole/base (always rendered)
    anim::AnimationFilm* m_BlueOrbFilm = nullptr;   // Blue orb (idle state)
    anim::AnimationFilm* m_RedOrbFilm = nullptr;    // Red orb (after animation finishes)
    anim::AnimationFilm* m_TriggeredFilm = nullptr; // Red orb spinning animation

    // Animation (only plays when triggered)
    anim::FrameRangeAnimation* m_TriggeredAnimation = nullptr;

    // Animator (only active after trigger)
    anim::FrameRangeAnimator* m_Animator = nullptr;

    bool m_Triggered = false;
    bool m_AnimationFinished = false;

    // Offset for blue orb relative to base (adjust based on your sprites)
    static constexpr int ORB_OFFSET_X = 0;   // Horizontal offset from base
    static constexpr int ORB_OFFSET_Y = -24; // Orb sits above the base (negative = up)

    // Animation parameters - adjust these based on your films.json
    static constexpr unsigned TRIGGERED_START_FRAME = 0;
    static constexpr unsigned TRIGGERED_END_FRAME = 15;  // Adjust based on actual frame count
    static constexpr unsigned TRIGGERED_REPS = 2;        // Spins twice
    static constexpr unsigned TRIGGERED_DELAY_MS = 30;   // Adjust for desired speed

    // Shared sound effect
    static sound::SFX s_CheckpointSound;
};
