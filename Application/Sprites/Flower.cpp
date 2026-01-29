#include "Sprites/Flower.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"

Flower::Flower(int x, int y, const std::string& filmId)
    : scene::Sprite(x, y, "Flower")
{
    m_Film = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm(filmId)
    );

    SetFilm(m_Film);
    m_FrameNo = 255;
    SetFrame(0);
    SetVisibility(true);
    SetHasDirectMotion(true);

    // Create idle animation (loops forever)
    unsigned endFrame = m_Film ? static_cast<unsigned>(m_Film->GetTotalFrames() - 1) : 0;
    m_Animation = new anim::FrameRangeAnimation(
        filmId + ".anim",
        0,
        endFrame,
        0,
        0,
        0,
        ANIMATION_DELAY_MS
    );
    m_Animation->SetForever();

    m_Animator = new anim::FrameRangeAnimator();
    m_Animator->SetOnAction(
        [this](anim::Animator* animator, anim::Animation* animation)
        {
            auto* frameAnimator = static_cast<anim::FrameRangeAnimator*>(animator);
            this->SetFrame(static_cast<byte>(frameAnimator->GetCurrFrame()));
        }
    );
}

Flower::~Flower()
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

void Flower::StartAnimation()
{
    if (m_Animator && m_Animation)
    {
        m_Animator->Start(m_Animation, core::SystemClock::Get().GetCurrTime());
    }
}

void Flower::StopAnimation()
{
    if (m_Animator)
    {
        m_Animator->Stop();
    }
}
