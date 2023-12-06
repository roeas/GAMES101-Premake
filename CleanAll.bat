@echo off

set "ROOT=%cd%"

set "FRAME_BINARARY=%ROOT%\Frame\bin"
set "FRAME_INTERMEDIATE=%ROOT%\Frame\int"
set "FRAME_VSCACHE=%ROOT%\.vs"

set "FREETYPE=%ROOT%\Frame\Source\ThirdParty\freetype\build"
set "GLEW=%ROOT%\Frame\Source\ThirdParty\glew\build\cmake\build"
set "GLFW=%ROOT%\Frame\Source\ThirdParty\glfw\build"
set "OPENCV_DOWNLOAD=%ROOT%\Frame\Source\ThirdParty\opencv\.cache"
set "OPENCV_BUILD=%ROOT%\Frame\Source\ThirdParty\opencv\build"

set "FRAME_SOLUTION=%ROOT%\Frame.sln"
if exist "%FRAME_SOLUTION%" (
    echo Deleting "%FRAME_SOLUTION%"...
    del /f %FRAME_SOLUTION%
) else (
    echo Path "%FRAME_SOLUTION%" does not exist.
)

set "pathsToDelete=FRAME_BINARARY FRAME_INTERMEDIATE FRAME_VSCACHE FREETYPE GLEW GLFW OPENCV_DOWNLOAD OPENCV_BUILD"
for %%i in (%pathsToDelete%) do (
    setlocal enabledelayedexpansion
    set "path=!%%i!"
    
    if exist "!path!" (
        echo Deleting "!path!"...
        rd /s /q "!path!"
    ) else (
        echo Path "!path!" does not exist.
    )
    
    endlocal
)

pause
