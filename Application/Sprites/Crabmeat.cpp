#include "Sprites/Crabmeat.h"
#include "Sprites/CrabBullet.h"
#include "Animations/AnimationFilmHolder.h"
#include "Core/SystemClock.h"
#include "Core/LatelyDestroyable.h"
#include "Physics/BoundingArea.h"

// Static member initialization
sound::SFX Crabmeat::s_DeathSound = nullptr;

Crabmeat::Crabmeat(int x, int y, scene::GridMap* grid)
    : scene::Sprite(x, y, "Crabmeat"), m_SpawnX(x), m_Grid(grid)
{
    // Load death sound effect (shared across all instances)
    if (!s_DeathSound)
    {
        s_DeathSound = sound::LoadSFX(ASSETS "/Sounds/enemy-death.mp3");
    }

    // Get the animation films from the holder
    m_IdleFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("crab.idle")
    );
    m_WalkFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("crab.walk")
    );
    m_AttackFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("crab.attack")
    );

    // Start with idle film
    SetFilm(m_IdleFilm);
    m_FrameNo = 255;  // Force frame box update
    SetFrame(0);
    SetVisibility(true);
    SetHasDirectMotion(true);  // We handle our own movement

    // Setup bounding area for collision detection (48x32 sprite)
    SetBoundingArea(new physics::BoundingBox(x, y, x + 48, y + 32));

    // Create the idle animation (single frame, but looped for consistency)
    m_IdleAnim = new anim::FrameRangeAnimation(
        "crab.idle.anim",
        0, 0, 0, 0, 0, IDLE_ANIM_DELAY_MS
    );
    m_IdleAnim->SetForever();

    // Create the walk animation (3 frames)
    m_WalkAnim = new anim::FrameRangeAnimation(
        "crab.walk.anim",
        0, 2, 0, 0, 0, WALK_ANIM_DELAY_MS
    );
    m_WalkAnim->SetForever();

    // Create the attack animation (single frame)
    m_AttackAnim = new anim::FrameRangeAnimation(
        "crab.attack.anim",
        0, 0, 0, 0, 0, IDLE_ANIM_DELAY_MS
    );
    m_AttackAnim->SetForever();

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

    // Start in walking state (moving left)
    m_State = State::WALKING;
    m_Direction = Direction::LEFT;
    m_PatternTimer = 0;
    m_WaitingIdle = false;
}

Crabmeat::~Crabmeat()
{
    StopAnimation();

    // Clean up bullets
    for (auto* bullet : m_Bullets)
    {
        bullet->StopAnimation();
        bullet->Destroy();
    }
    m_Bullets.clear();

    if (m_IdleAnim)
    {
        m_IdleAnim->Destroy();
        m_IdleAnim = nullptr;
    }

    if (m_WalkAnim)
    {
        m_WalkAnim->Destroy();
        m_WalkAnim = nullptr;
    }

    if (m_AttackAnim)
    {
        m_AttackAnim->Destroy();
        m_AttackAnim = nullptr;
    }

    if (m_Animator)
    {
        m_Animator->Destroy();
        m_Animator = nullptr;
    }
}

void Crabmeat::StartAnimation()
{
    if (m_Animator && m_WalkAnim && m_IsAlive)
    {
        SetFilm(m_WalkFilm);
        m_FrameNo = 255;
        SetFrame(0);
        m_Animator->Start(m_WalkAnim, core::SystemClock::Get().GetCurrTime());
    }
}

void Crabmeat::StopAnimation()
{
    if (m_Animator)
    {
        m_Animator->Stop();
    }
}

void Crabmeat::Update()
{
    if (!m_IsAlive)
        return;

    // Update bullets
    UpdateBullets();

    // Movement pattern: walk 2 seconds, idle 1 second, repeat
    m_PatternTimer++;

    if (m_WaitingIdle)
    {
        // In idle pause
        if (m_PatternTimer >= IDLE_DURATION_FRAMES)
        {
            // End idle, start walking in opposite direction
            m_WaitingIdle = false;
            m_PatternTimer = 0;
            m_HasFiredThisCycle = false;

            // Switch direction
            if (m_Direction == Direction::LEFT)
                m_Direction = Direction::RIGHT;
            else
                m_Direction = Direction::LEFT;

            SetState(State::WALKING);
        }
    }
    else
    {
        // Walking phase
        if (m_PatternTimer >= WALK_DURATION_FRAMES)
        {
            // End walk, enter idle pause (and fire bullets)
            m_WaitingIdle = true;
            m_PatternTimer = 0;

            // Fire bullets when stopping
            if (!m_HasFiredThisCycle)
            {
                FireBullets();
                m_HasFiredThisCycle = true;
            }

            SetState(State::IDLE);
        }
        else
        {
            // Move in current direction (only every N frames for slower walk)
            m_MoveFrame++;
            if (m_MoveFrame >= MOVE_EVERY_N_FRAMES)
            {
                m_MoveFrame = 0;
                if (m_Direction == Direction::LEFT)
                {
                    m_X -= WALK_SPEED;
                    SetFlipHorizontal(false);
                }
                else
                {
                    m_X += WALK_SPEED;
                    SetFlipHorizontal(true);
                }
            }
        }
    }

    // Update bounding area to match current position
    auto* box = static_cast<physics::BoundingBox*>(m_BoundingArea);
    box->x1 = m_X;
    box->y1 = m_Y;
    box->x2 = m_X + 48;
    box->y2 = m_Y + 32;
}

void Crabmeat::SetState(State newState)
{
    if (m_State == newState)
        return;

    m_State = newState;
    UpdateAnimation();
}

void Crabmeat::UpdateAnimation()
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
        case State::ATTACKING:
            newFilm = m_AttackFilm;
            newAnim = m_AttackAnim;
            break;
    }

    if (newFilm && newAnim)
    {
        SetFilm(newFilm);
        m_FrameNo = 255;
        SetFrame(0);
        m_Animator->Start(newAnim, core::SystemClock::Get().GetCurrTime());
    }
}

void Crabmeat::FireBullets()
{
    // Fire two bullets at 45-degree angles (one left, one right)
    int bulletY = m_Y;  // Top of crab
    int bulletLeftX = m_X;  // Left side
    int bulletRightX = m_X + 32;  // Right side

    // Left bullet: moves left and up initially
    CrabBullet* leftBullet = new CrabBullet(bulletLeftX, bulletY, -3, -4, m_Grid);
    leftBullet->StartAnimation();
    m_Bullets.push_back(leftBullet);

    // Right bullet: moves right and up initially
    CrabBullet* rightBullet = new CrabBullet(bulletRightX, bulletY, 3, -4, m_Grid);
    rightBullet->StartAnimation();
    m_Bullets.push_back(rightBullet);
}

void Crabmeat::UpdateBullets()
{
    // Update all bullets
    for (auto* bullet : m_Bullets)
    {
        if (bullet->IsActive())
        {
            bullet->Update();
        }
    }

    // Remove inactive bullets
    auto it = m_Bullets.begin();
    while (it != m_Bullets.end())
    {
        if (!(*it)->IsActive())
        {
            (*it)->StopAnimation();
            (*it)->Destroy();
            it = m_Bullets.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Crabmeat::Kill()
{
    if (!m_IsAlive)
        return;

    m_IsAlive = false;
    StopAnimation();
    SetVisibility(false);

    // Clean up bullets
    for (auto* bullet : m_Bullets)
    {
        bullet->StopAnimation();
        bullet->Destroy();
    }
    m_Bullets.clear();

    // Play death sound
    if (s_DeathSound)
    {
        sound::PlaySFX(s_DeathSound);
    }
}
