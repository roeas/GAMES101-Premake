#pragma once

#include <cfloat>
#include <filesystem>
#include <string>

namespace Utils
{

inline std::string PathFromRoot(std::string relativePath)
{
    return (std::filesystem::path(FRAME_ROOT_PATH) / std::move(relativePath)).generic_string();
}

inline std::string PathFromAsset(std::string relativePath)
{
    return (std::filesystem::path(FRAME_ASSET_PATH) / std::move(relativePath)).generic_string();
}

inline bool FloatEqual(const float a, const float b, const float EPSILON = FLT_EPSILON)
{
    return std::abs(a - b) <= EPSILON;
}

} // namespace Utils
