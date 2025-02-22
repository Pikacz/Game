if not defined VSCMD_VER (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

cl build_script.cpp /D TARGETING_MSVC /std:c++17 /EHsc /link /OUT:build.exe
del build_script.obj
