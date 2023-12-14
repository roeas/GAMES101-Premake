#pragma once

#include <iostream>
#include <cmath>
#include <random>

inline float clamp(const float lo, const float hi, const float v)
{
    return std::max(lo, std::min(hi, v));
}

inline float get_random_float()
{
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    return dist(rng);
}

inline void UpdateProgress(const float progress)
{
    constexpr int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(progress * 100.0f) << " %\r";
    std::cout.flush();
};
