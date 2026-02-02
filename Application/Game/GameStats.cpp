#include "Game/GameStats.h"

GameStats& GameStats::Get()
{
    static GameStats instance;
    return instance;
}

void GameStats::AddRing()
{
    ++m_Rings;
    AddScore(RING_SCORE);
}

int GameStats::GetRings() const
{
    return m_Rings;
}

void GameStats::AddScore(int points)
{
    m_Score += points;
}

int GameStats::GetScore() const
{
    return m_Score;
}

void GameStats::StartTimer()
{
    m_StartTime = 0;
    m_ElapsedMs = 0;
}

void GameStats::UpdateTimer(TimeStamp currentTime)
{
    if (m_StartTime == 0)
    {
        m_StartTime = currentTime;
    }
    m_ElapsedMs = currentTime - m_StartTime;
}

int GameStats::GetTimeSeconds() const
{
    return static_cast<int>((m_ElapsedMs / 1000) % 60);
}

int GameStats::GetTimeMinutes() const
{
    return static_cast<int>(m_ElapsedMs / 60000);
}

void GameStats::LoseLife()
{
    if (m_Lives > 0)
    {
        --m_Lives;
    }
}

void GameStats::AddLife()
{
    if (m_Lives < MAX_LIVES)
    {
        ++m_Lives;
    }
}

int GameStats::GetLives() const
{
    return m_Lives;
}

bool GameStats::IsGameOver() const
{
    return m_Lives <= 0;
}

void GameStats::AddSpringBounce()
{
    ++m_SpringBounces;
    AddScore(SPRING_SCORE);
}

void GameStats::AddMonitorDestroyed()
{
    ++m_MonitorsDestroyed;
    AddScore(MONITOR_SCORE);
}

int GameStats::GetSpringBounces() const
{
    return m_SpringBounces;
}

int GameStats::GetMonitorsDestroyed() const
{
    return m_MonitorsDestroyed;
}

void GameStats::Reset()
{
    m_Rings = 0;
    m_Score = 0;
    m_Lives = DEFAULT_LIVES;
    m_StartTime = 0;
    m_ElapsedMs = 0;
    m_SpringBounces = 0;
    m_MonitorsDestroyed = 0;
}
