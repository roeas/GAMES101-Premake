#pragma once

#include <string>
#include <filesystem>

inline std::string PathFromAsset(std::string relativePath)
{
    return (std::filesystem::path(FRAME_ASSET_PATH) / std::move(relativePath)).generic_string();
}
