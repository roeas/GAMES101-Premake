@echo off

rem Set path
set "ROOT_PATH=%~dp0"
set "THIRD_PARTY_PATH=%ROOT_PATH%\Frame\Source\ThirdParty"
echo Root path: %ROOT_PATH%
echo ThirdParty path: %THIRD_PARTY_PATH%

rem freetype
set "FREETYPE_PATH=%THIRD_PARTY_PATH%\freetype"
echo [ freetype ] path: %FREETYPE_PATH%
cd %FREETYPE_PATH%

cmake -B build
cmake --build build --target freetype --config Release
cmake --build build --target freetype --config Debug

rem glew
set "GLEW_PATH=%THIRD_PARTY_PATH%\glew"
echo [ glew ] path: %GLEW_PATH%
cd %GLEW_PATH%\build\cmake

cmake -B build
cmake --build build --target glew_s --config Release
cmake --build build --target glew_s --config Debug

rem glfw
set "GLFW_PATH=%THIRD_PARTY_PATH%\glfw"
echo [ glfw ] path: %GLFW_PATH%
cd %GLFW_PATH%

cmake -B build -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF
cmake --build build --target glfw --config Release
cmake --build build --target glfw --config Debug

rem opencv
set "OPENCV_PATH=%THIRD_PARTY_PATH%\opencv"
echo [ opencv ] path: %OPENCV_PATH%
cd %OPENCV_PATH%

rem TODO : Remove useless modules.
cmake -B build -DOPENCV_DOWNLOAD_MIRROR_ID=gitcode -DBUILD_SHARED_LIBS=ON -DBUILD_WITH_STATIC_CRT=ON -DBUILD_opencv_world=ON
cmake --build build --target opencv_world --config Release
cmake --build build --target opencv_world --config Debug

echo.

pause
