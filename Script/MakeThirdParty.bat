@echo off

rem Find MSBuild by vswhere
set "VSWHERE_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`"%VSWHERE_PATH%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set "MSBUILD_PATH=%%i"
  goto :exitLoop
)
:exitLoop
echo Found MSBuild at: %MSBUILD_PATH%

set "OPENCV_PATH=%THIRD_PARTY_PATH%\opencv"
echo [ opencv ] path: %OPENCV_PATH%

cd %OPENCV_PATH%
if not exist build mkdir build

cmake -S %OPENCV_PATH% -B %OPENCV_PATH%/build -D OPENCV_DOWNLOAD_MIRROR_ID=gitcode -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D BUILD_EXAMPLES=OFF -D BUILD_opencv_world=ON
cmake -S . -B build

::"%MSBUILD_PATH%" "%OPENCV_PATH%\build\OpenCV.sln" /p:Configuration=Debug /p:Platform=x64
::"%MSBUILD_PATH%" "%OPENCV_PATH%\build\OpenCV.sln" /p:Configuration=Release /p:Platform=x64

echo.
