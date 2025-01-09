@echo off


if %ERRORLEVEL% neq 0 (
    cmd /c "exit /b 0"
)

call "%~dp0config.bat"

if not defined VSCMD_VER (
    call "%DirVisualStudio%VC\Auxiliary\Build\vcvarsall.bat" x64
)

for %%I in ("%~dp0..\..") do set "DirProject=%%~fI\"

set "DirPlatform=%DirProject%code\platform_win32\"
set "DirGamePlatformNoDash=%DirProject%code\game\platform"
set "DirGamePlatform=%DirGamePlatformNoDash%\"
set "DirGame=%DirProject%code\game\"
set "DirMisc=%DirProject%\misc\win32\"
set "DirMiscTools=%DirProject%\misc\win32\tools\"
set "DirBuild=%DirProject%build\win32\"
set "DirBuildTimestamps=%DirProject%build\win32\timestamp\"
set "DirBuildTools=%DirProject%build\win32\tools\"
set "DirBuildTimestampsTools=%DirProject%build\win32\timestamp\tools\"

set "ExeTimestamps=%DirBuildTools%timestamps.exe"

if not exist "%DirBuild%" (
    mkdir "%DirBuild%"
)
if not exist "%DirBuildTimestamps%" (
    mkdir "%DirBuildTimestamps%"
)
if not exist "%DirBuildTools%" (
    mkdir "%DirBuildTools%"
)
if not exist "%DirBuildTimestampsTools%" (
    mkdir "%DirBuildTimestampsTools%"
)

@REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BUILD TIMESTAMPS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if not exist "%ExeTimestamps%" (
    goto :TimestampsBuild
)
if not exist "%DirBuildTimestampsTools%timestamps.txt" (
    goto :TimestampsBuild
)


setlocal
set /p TimestampsBuildTimestamp=<"%DirBuildTimestampsTools%timestamps.txt"
set /a TimestampsBuildTimestamp=%TimestampsBuildTimestamp%

for /F "tokens=*" %%F in ('%ExeTimestamps% %DirMiscTools%timestamps.cpp') do SET TimestampsModification=%%F
if %ERRORLEVEL% neq 0 (
    echo Unable to get modification date of timestamps.cpp
    exit /b %ERRORLEVEL%
)
set /a TimestampsModification=%TimestampsModification%

if %TimestampsBuildTimestamp% geq %TimestampsModification% (
    goto :SkipTimestampsBuild
)


:TimestampsBuild

echo Building timestamps tool.
pushd %DirBuildTools%
cl "%DirMiscTools%\timestamps.cpp" /O2 /Fe:timestamps.exe
if %ERRORLEVEL% neq 0 (
    echo Unable to build timestamps tool.
    exit /b %ERRORLEVEL%
)
popd

if not defined TimestampsModification (
    for /F "tokens=*" %%F in ('%ExeTimestamps% %DirMiscTools%timestamps.cpp') do SET TimestampsModification=%%F
    if %ERRORLEVEL% neq 0 (
        echo %ExeTimestamps% %DirMiscTools%timestamps.cpp
        echo Unable to get modification date of timestamps.cpp
        exit /b %ERRORLEVEL%
    )
    set /a TimestampsModification=%TimestampsModification%
)

echo %TimestampsModification% > "%DirBuildTimestampsTools%timestamps.txt"

:SkipTimestampsBuild

endlocal