#!/bin/bash

set -e

export MiscDir=$(dirname "$(realpath "$0")")
export RootDir=$(realpath "$MiscDir/../..")
export CodeDir=$(realpath "$RootDir/code")
export BuildRootDir="$RootDir/build"
export BuildDir="$BuildRootDir/macOS"
export BuildTimestampDir="$BuildDir/.time"

export ExeName="game"
export DLibName="game.dlib"

CommonCompilerFlags=""
PlatformCompilerFlags="-D PLATFORM=1 -D GAME_DLIB=\"$DLibName\""
PlatformFrameworks="-framework Cocoa"
GameCompilerFlags=""

function get_modification_timestamp {
    path="$1"
    if [[ -d "$path" ]]; then
        newest_file=$(find "$path" -type f -exec stat -f "%m %N" {} + | sort -n | tail -n 1 | cut -d ' ' -f 2-)
        if [[ -n "$newest_file" ]]; then
            path=$newest_file
        fi
    fi

    if [[ -f "$path" ]]; then
        timestamp=$(stat -f %m "$path")
        echo "$timestamp"
    else
        echo "$path does not exist or is not a file or directory."
    fi
}

##### SETUP ###########################################################################################################

mkdir -p "$BuildRootDir"
mkdir -p "$BuildDir"
mkdir -p "$BuildTimestampDir"

##### BUILD PLATFORM ##################################################################################################

LastPlatformBuildTimestamp="0"
PlatformTimestampFile="$BuildTimestampDir/game.txt"
if [ -e "$PlatformTimestampFile" ]; then
    LastPlatformBuildTimestamp=$(<"$PlatformTimestampFile")
fi

PlatformModificationTime=$(get_modification_timestamp "$CodeDir/platform/macOS")
GameHeaderModificationTime=$(get_modification_timestamp "$CodeDir/game/game_platform_header.h")

if [ "$PlatformModificationTime" -lt "$GameHeaderModificationTime" ]; then
    PlatformModificationTime="$GameHeaderModificationTime"
fi

if [ "$LastPlatformBuildTimestamp" -lt "$PlatformModificationTime" ]; then
    echo "Building platform"
    PID=$(pgrep -x "$ExeName") || PID=""
    if [ -n "$PID" ]; then
        kill -9 "$PID"
    fi
    StartTime=$(date +%s)
    clang $CommonCompilerFlags $PlatformCompilerFlags -o "$BuildDir/$ExeName" "-I$CodeDir/game/" "$CodeDir/platform/macOS/main.mm" $PlatformFrameworks
    echo "$PlatformModificationTime" >"$PlatformTimestampFile"
    EndTime=$(date +%s)
    Duration=$((EndTime - StartTime))
    echo "Platform built in $Duration seconds"
fi

##### BUILD GAME ######################################################################################################

LastGameBuildTimestamp="0"
GameTimestampFile="$BuildTimestampDir/game_dll.txt"
if [ -e "$GameTimestampFile" ]; then
    LastGameBuildTimestamp=$(<"$GameTimestampFile")
fi

GameModificationTime=$(get_modification_timestamp "$CodeDir/game")

if [ "$LastGameBuildTimestamp" -lt "$GameModificationTime" ]; then
    echo "Building game"
    StartTime=$(date +%s)
    clang $CommonCompilerFlags $GameCompilerFlags -dynamiclib -o "$BuildDir/$DLibName" "$CodeDir/game/game.cpp" "-Wl,-exported_symbols_list,$CodeDir/game/macOS_game.def"
    echo "$GameModificationTime" >"$GameTimestampFile"
    EndTime=$(date +%s)
    Duration=$((EndTime - StartTime))
    echo "Game built in $Duration seconds"
fi
