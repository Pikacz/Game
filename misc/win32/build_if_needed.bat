@echo off

echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

call "%~dp0setup.bat"

if %ERRORLEVEL% neq 0 (
    echo Setup failed.
    exit /b %ERRORLEVEL%
)

set "SharedCompilerFlags=/D WIN32 /WX /std:c++17 -openmp:experimental"
set "SharedLinkerFlags=/INCREMENTAL:NO"
set "PlatformCompilerFlags=/I "%DirGamePlatformNoDash%""
set "PlatformLinkerFlags=kernel32.lib User32.lib Gdi32.lib"
set "GameCompilerFlags="
set "GameLinkerFlags="

@REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BUILD PLATFORM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

setlocal
set "OutFile=%DirBuild%main.exe"
set "OutFileTimestamp=%DirBuildTimestamps%main.txt"

for /F "tokens=*" %%F in ('%ExeTimestamps% %DirPlatform% %DirGamePlatform%') do SET SourcesModification=%%F
if %ERRORLEVEL% neq 0 (
    echo Unable to get modification date of platform code
    exit /b %ERRORLEVEL%
)
set /a SourcesModification=%SourcesModification%

if not exist "%OutFile%" (
    goto :PlatformBuild
)
if not exist "%OutFileTimestamp%" (
    goto :PlatformBuild
)

set /p BuildTimestamp=<"%DirBuildTimestamps%main.txt"
set /a BuildTimestamp=%BuildTimestamp%
if %ERRORLEVEL% neq 0 (
    goto :PlatformBuild
)

if %BuildTimestamp% geq %SourcesModification% (
    goto :SkipPlatformBuild
)

:PlatformBuild

echo Building platform

for /f "tokens=*" %%a in ('powershell -command "(Get-Date).Millisecond"') do set startTime=%%a
set /a startTime=%startTime%

pushd %DirBuild%

taskkill /F /IM main.exe /T
if exist main.exe (
    del main.exe
)
if %ERRORLEVEL% neq 0 (
    cmd /c "exit /b 0"
)

cl %SharedCompilerFlags% %PlatformCompilerFlags% "%DirPlatform%win32_main.cpp" /link /OUT:main.exe %SharedLinkerFlags% %PlatformLinkerFlags%

if %ERRORLEVEL% neq 0 (
    echo Unable to build platform
    exit /b %ERRORLEVEL%
)
if exist win32_main.obj (
    del win32_main.obj
)
popd
echo %SourcesModification% > "%OutFileTimestamp%"

for /f "tokens=*" %%a in ('powershell -command "(Get-Date).Millisecond"') do set endTime=%%a
set /a endTime=%endTime%
set /a elapsedTime=%endTime% - %startTime%
if %elapsedTime% lss 0 set /a elapsedTime+=1000

echo Platform built in %elapsedTime% milliseconds.

:SkipPlatformBuild
endlocal

@REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BUILD GAME ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


setlocal
set "OutFile=%DirBuild%game.dll"
set "OutFileTimestamp=%DirBuildTimestamps%game.txt"

for /F "tokens=*" %%F in ('%ExeTimestamps% %DirGame%') do SET SourcesModification=%%F
if %ERRORLEVEL% neq 0 (
    echo Unable to get modification date of platform code
    exit /b %ERRORLEVEL%
)
set /a SourcesModification=%SourcesModification%


if not exist "%OutFile%" (
    goto :GameBuild
)
if not exist "%OutFileTimestamp%" (
    goto :GameBuild
)

set /p BuildTimestamp=<"%DirBuildTimestamps%game.txt"
set /a BuildTimestamp=%BuildTimestamp%
if %ERRORLEVEL% neq 0 (
    goto :GameBuild
)



if %BuildTimestamp% geq %SourcesModification% (
    goto :SkipGameBuild
)

:GameBuild

echo Building game

for /f "tokens=*" %%a in ('powershell -command "(Get-Date).Millisecond"') do set startTime=%%a
set /a startTime=%startTime%

pushd %DirBuild%
if exist game.dll (
    del game.dll
)

cl %SharedCompilerFlags% %GameCompilerFlags% "%DirGame%game.cpp" /link /DLL /OUT:game.dll %SharedLinkerFlags% %GameLinkerFlags%

if %ERRORLEVEL% neq 0 (
    echo Unable to build game
    exit /b %ERRORLEVEL%
)
if exist game.exp (
    del game.exp
)
if exist game.obj (
    del game.obj
)
if exist game.lib (
    del game.lib
)
popd
echo %SourcesModification% > "%OutFileTimestamp%"

for /f "tokens=*" %%a in ('powershell -command "(Get-Date).Millisecond"') do set endTime=%%a
set /a endTime=%endTime%
set /a elapsedTime=%endTime% - %startTime%
if %elapsedTime% lss 0 set /a elapsedTime+=1000

echo Game built in %elapsedTime% milliseconds.

:SkipGameBuild
endlocal


@REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Assets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

setlocal

set "OutFileTimestamp=%DirBuildTimestamps%assets.txt"

for /F "tokens=*" %%F in ('%ExeTimestamps% %DirAssets%') do SET SourcesModification=%%F
if %ERRORLEVEL% neq 0 (
    echo Unable to get modification date of platform code
    exit /b %ERRORLEVEL%
)
set /a SourcesModification=%SourcesModification%


if not exist "%OutFileTimestamp%" (
    goto :CopyAssets
)

set /p BuildTimestamp=<"%DirBuildTimestamps%assets.txt"
set /a BuildTimestamp=%BuildTimestamp%
if %ERRORLEVEL% neq 0 (
    goto :CopyAssets
)

if %BuildTimestamp% geq %SourcesModification% (
    goto :SkipCopyAssets
)

:CopyAssets

echo Copying assets.

if exist %DirBuild%assets (
    rd /s /q %DirBuild%assets
)


robocopy %DirAssets% %DirBuild%assets /E /MIR

if %ERRORLEVEL% geq 8 (
    echo Unable copy assets
    exit /b %ERRORLEVEL%
)

echo %SourcesModification% > "%OutFileTimestamp%"

:SkipCopyAssets

endlocal