#pragma once

#include <cfloat>
#include <string>
#include <filesystem>

namespace Utils
{

inline std::string PathFromAsset(std::string relativePath)
{
    return (std::filesystem::path(FRAME_ASSET_PATH) / std::move(relativePath)).generic_string();
}

inline bool FloatEqual(const float a, const float b)
{
    return (std::abs(a - b) <= FLT_EPSILON);
}

} // namespace Utils
