#pragma once

#include "Utils/Common.h"

class GameStats
{
public:
    static GameStats& Get();

    // Ring tracking
    void AddRing();
    int LoseRings(int count);  // Returns actual rings lost (capped at current)
    int GetRings() const;

    // Score
    void AddScore(int points);
    int GetScore() const;

    // Time
    void StartTimer();
    void UpdateTimer(TimeStamp currentTime);
    int GetTimeSeconds() const;
    int GetTimeMinutes() const;

    // Lives
    void LoseLife();
    void AddLife();
    int GetLives() const;
    bool IsGameOver() const;

    // Counters for future scoring
    void AddSpringBounce();
    void AddMonitorDestroyed();
    int GetSpringBounces() const;
    int GetMonitorsDestroyed() const;

    // Reset all stats (call when starting a new game/level)
    void Reset();

private:
    GameStats() = default;

    int m_Rings = 0;
    int m_Score = 0;
    int m_Lives = 3;
    TimeStamp m_StartTime = 0;
    TimeStamp m_ElapsedMs = 0;
    int m_SpringBounces = 0;
    int m_MonitorsDestroyed = 0;

    // Score values
    static constexpr int RING_SCORE = 10;
    static constexpr int SPRING_SCORE = 10;
    static constexpr int MONITOR_SCORE = 100;

    // Lives
    static constexpr int DEFAULT_LIVES = 3;
    static constexpr int MAX_LIVES = 9;
};
