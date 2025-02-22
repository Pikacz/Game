#include "tools/cBuilder/cBuilder.h"

using namespace std;

const vector<CompilationFlag> sharedCompilationFlags{
    CompilationFlag::cpp17,
#ifdef TARGETING_MSVC
    CompilationFlag::define("WIN32"),
#endif
};

const filesystem::path buildDir = system_getPath("build");

#ifdef TARGETING_MSVC
const filesystem::path platform_mainFile =
    system_getPath("code/platform_win32/win32_main.cpp");
const vector<LinkFlag> platform_platformLinks{
    LinkFlag{"kernel32.lib"},
    LinkFlag{"User32.lib"},
    LinkFlag{"Gdi32.lib"},
};
#endif

const filesystem::path testsDir = buildDir / "tests";

template <typename T>
void append(vector<T> &lhs, const vector<T> &rhs) {
  for (size_t i = 0; i < rhs.size(); ++i) {
    lhs.push_back(rhs[i]);
  }
}

int main() {
  Target platformMain;
  Target gameDll;

  platformMain.type = TargetExecutable;
  platformMain.mainFile = platform_mainFile;
  platformMain.outDir = buildDir;
  append(platformMain.flags, sharedCompilationFlags);
  platformMain.flags.push_back(
      CompilationFlag::include(system_getPath("code/game/platform")));
  append(platformMain.linkFlags, platform_platformLinks);

  gameDll.type = TargetDynamicLibrary;
  gameDll.mainFile = system_getPath("code/game/game.cpp");
  gameDll.outDir = buildDir;
  append(gameDll.flags, sharedCompilationFlags);

  target_compileIfNeeded(&gameDll);
  target_compileIfNeeded(&platformMain);

  CopyTarget assets;
  assets.sourceDir = system_getPath("assets");
  assets.outDir = system_getPath("build/assets");

  copy_processIfNeeded(&assets);

  // MARK: TESTS

  Target platformMainTests;
  Target gameDllTests;

  platformMainTests.type = TargetExecutable;
  platformMainTests.mainFile = testsDir / "src/platformTests.cpp";
  platformMainTests.outDir = testsDir;
  append(platformMainTests.flags, platformMain.flags);
  platformMainTests.flags.push_back(
      CompilationFlag::include(system_getPath("tools/tests/")));
  platformMainTests.flags.push_back(
      CompilationFlag::include(platformMain.mainFile.parent_path()));
  platformMainTests.flags.push_back(CompilationFlag::define("TESTS"));
  append(platformMainTests.linkFlags, platformMain.linkFlags);
  append(platformMainTests.dependencies, platformMain.dependencies);

  gameDllTests.type = TargetExecutable;
  gameDllTests.mainFile = testsDir / "src/gameTests.cpp";
  gameDllTests.outDir = testsDir;
  append(gameDllTests.flags, gameDll.flags);
  gameDllTests.flags.push_back(
      CompilationFlag::include(system_getPath("tools/tests/")));
  gameDllTests.flags.push_back(
      CompilationFlag::include(gameDll.mainFile.parent_path()));
  gameDllTests.flags.push_back(CompilationFlag::define("TESTS"));
  append(gameDllTests.linkFlags, gameDll.linkFlags);
  append(gameDllTests.dependencies, gameDll.dependencies);

  test_compileIfNeeded(&platformMainTests,
                       platformMain.mainFile.filename().string());
  test_compileIfNeeded(&gameDllTests, gameDll.mainFile.filename().string());

  cout << "Build succeeded!\n";

  return 0;
}