#pragma once

#include <filesystem>

namespace Utils
{

inline std::string PathFromRoot(std::string_view relativePath)
{
    return (std::filesystem::path(FRAME_ROOT_PATH) / relativePath).generic_string();
}

inline std::string PathFromAsset(std::string_view relativePath)
{
    return (std::filesystem::path(FRAME_ASSET_PATH) / relativePath).generic_string();
}

inline bool FloatEqual(const float a, const float b, const float EPSILON = FLT_EPSILON)
{
    return std::abs(a - b) <= EPSILON;
}

} // namespace Utils
