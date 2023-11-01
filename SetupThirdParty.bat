@echo off

set "ROOT_PATH=%~dp0"
set "THIRD_PARTY_PATH=%ROOT_PATH%\Frame\Source\ThirdParty"
echo Root path: %ROOT_PATH%
echo ThirdParty path: %THIRD_PARTY_PATH%

set "OPENCV_PATH=%THIRD_PARTY_PATH%\opencv"
echo [ opencv ] path: %OPENCV_PATH%
cd %OPENCV_PATH%

rem TODO : Remove useless modules.
cmake -B build -D CMAKE_CONFIGURATION_TYPES="Release;Debug" -D OPENCV_DOWNLOAD_MIRROR_ID=gitcode -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D BUILD_EXAMPLES=OFF -D BUILD_opencv_apps=OFF -D BUILD_opencv_world=ON

cmake --build build --target opencv_world --config Release
cmake --build build --target opencv_world --config Debug

echo.
pause
