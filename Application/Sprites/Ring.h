#pragma once

#include "Scene/Sprite.h"
#include "Animations/AnimationFilm.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"

class Ring : public scene::Sprite
{
public:
    Ring(int x, int y);
    ~Ring();

    void StartAnimation();
    void StopAnimation();
    void OnCollected();
    bool IsCollected() const;
    bool IsCollectionFinished() const;

private:
    // Films (owned by AnimationFilmHolder, not us)
    anim::AnimationFilm* m_SpinFilm = nullptr;
    anim::AnimationFilm* m_CollectedFilm = nullptr;

    // Animations
    anim::FrameRangeAnimation* m_SpinAnimation = nullptr;
    anim::FrameRangeAnimation* m_CollectedAnimation = nullptr;

    // Animator (reused for both animations)
    anim::FrameRangeAnimator* m_Animator = nullptr;

    bool m_Collected = false;
    bool m_CollectionFinished = false;

    static constexpr unsigned SPIN_START_FRAME = 0;
    static constexpr unsigned SPIN_END_FRAME = 3;
    static constexpr unsigned SPIN_DELAY_MS = 100;

    static constexpr unsigned COLLECTED_START_FRAME = 0;
    static constexpr unsigned COLLECTED_END_FRAME = 3;
    static constexpr unsigned COLLECTED_DELAY_MS = 50;  // Faster for sparkle effect
};
