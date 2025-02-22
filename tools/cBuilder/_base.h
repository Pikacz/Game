#pragma once

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>

std::filesystem::path _getExecutablePath() {
  wchar_t path[32767];
  if (GetModuleFileNameW(NULL, path, 32767)) {
    return std::filesystem::path(path);
  }
  throw std::runtime_error("Failed to get executable path");
}
#endif

struct CompilationFlag {
  const std::string val;

  static const CompilationFlag cpp17;
  static const CompilationFlag usesExceptions;

  static CompilationFlag include(std::filesystem::path const& path);

  static CompilationFlag define(std::string const& varName,
                                const char* value = nullptr);
};

struct LinkFlag {
  const std::string val;
};

enum TargetType { TargetExecutable, TargetObject, TargetDynamicLibrary };

struct Target {
  TargetType type;
  std::filesystem::path mainFile;
  std::filesystem::path outDir;
  std::vector<CompilationFlag> flags;
  std::vector<LinkFlag> linkFlags;
  std::vector<Target*> dependencies;
};

struct CompilationMetadata {
  int64_t compiledAt;
  std::vector<std::string> sources;
};

struct CopyTarget {
  std::filesystem::path sourceDir;
  std::filesystem::path outDir;
};

// MARK: PLATFORM

std::filesystem::path platform_target_outputPath(std::filesystem::path mainFile,
                                                 std::filesystem::path outDir,
                                                 TargetType type);

CompilationMetadata platform_target_compile(const Target* target);

// MARK: SYSTEM

std::string system_exec(const char* cmd) {
  std::cout << cmd << "\n";
  std::ostringstream result;
  FILE* pipe = _popen(cmd, "r");
  if (!pipe) return "Error opening pipe!";

  char buffer[128];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result << buffer;
  }

  int exitCode = _pclose(pipe);

  if (exitCode != 0) {
    std::cerr << result.str();
    std::abort();
  }

  return result.str();
}

int64_t system_fileModificationTimestamp(const std::string* filePath) {
  auto ftime = std::filesystem::last_write_time(*filePath);
  auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      ftime - std::filesystem::file_time_type::clock::now() +
      std::chrono::system_clock::now());
  time_t time = std::chrono::system_clock::to_time_t(sctp);
  return static_cast<int64_t>(time);
}

std::filesystem::path system_getPath(const std::string& path) {
  static std::filesystem::path exe_path = _getExecutablePath().parent_path();
  return std::filesystem::absolute(exe_path / path);
}

void system_ensureDirectoryExist(std::filesystem::path dirPath) {
  std::filesystem::path tmp;
  std::filesystem::path parent;

  while (!std::filesystem::exists(dirPath)) {
    tmp = dirPath;
    parent = tmp.parent_path();
    while (!std::filesystem::exists(parent) && parent != "") {
      tmp = parent;
      parent = tmp.parent_path();
    }
    if (!std::filesystem::is_directory(parent)) {
      std::filesystem::remove(parent);
      std::filesystem::create_directory(parent);
    }
    if (std::filesystem::exists(tmp) && !std::filesystem::is_directory(tmp)) {
      std::filesystem::remove(tmp);
    }
    if (!std::filesystem::exists(tmp)) {
      std::filesystem::create_directory(tmp);
    }
  }
}

// MARK: METADATA

CompilationMetadata metadata_read(const std::filesystem::path* path) {
  CompilationMetadata result{0, {}};
  if (!std::filesystem::exists(*path)) {
    return result;
  }
  std::ifstream file(*path);
  if (!file) {
    return result;
  }
  size_t count;
  file >> result.compiledAt >> count;
  std::string tmp;
  for (size_t i = 0; i < count; ++i) {
    file >> tmp;
    result.sources.push_back(tmp);
  }
  return result;
}

void metadata_save(const CompilationMetadata* metadata,
                   const std::filesystem::path* path) {
  system_ensureDirectoryExist(path->parent_path());
  std::ofstream file(*path);

  file << metadata->compiledAt << " " << metadata->sources.size() << "\n";
  for (size_t i = 0; i < metadata->sources.size(); i++) {
    file << metadata->sources[i] << "\n";
  }
}

// MARK: TARGET

std::filesystem::path target_outputPath(const Target* target) {
  return platform_target_outputPath(target->mainFile, target->outDir,
                                    target->type);
}

std::filesystem::path target_metadataPath(const Target* target) {
  return target->outDir / "build_metadata" / target->mainFile.stem();
}

CompilationMetadata target_getMetadata(const Target* target) {
  if (!std::filesystem::exists(target_outputPath(target))) {
    return {0, {}};
  }
  std::filesystem::path metadataPath = target_metadataPath(target);
  return metadata_read(&metadataPath);
}

void target_saveMetadata(const Target* target,
                         const CompilationMetadata* metadata) {
  std::filesystem::path metadataPath = target_metadataPath(target);
  metadata_save(metadata, &metadataPath);
}

bool _target_shouldCompile(const Target* target,
                           CompilationMetadata* targetMetadata) {
  *targetMetadata = target_getMetadata(target);

  if (targetMetadata->compiledAt == 0) {
    return true;
  }
  {
    std::string mainPath = target->mainFile.string();
    if (system_fileModificationTimestamp(&mainPath) >=
        targetMetadata->compiledAt) {
      return true;
    }
  }
  for (size_t i = 0; i < targetMetadata->sources.size(); ++i) {
    if (system_fileModificationTimestamp(&targetMetadata->sources[i]) >=
        targetMetadata->compiledAt) {
      return true;
    }
  }

  CompilationMetadata childMetadata;
  for (size_t i = 0; i < target->dependencies.size(); ++i) {
    if (_target_shouldCompile(target->dependencies[i], &childMetadata)) {
      return true;
    }
    if (childMetadata.compiledAt >= targetMetadata->compiledAt) {
      return true;
    }
  }
  return false;
}

bool target_shouldCompile(const Target* target) {
  CompilationMetadata metadata;
  return _target_shouldCompile(target, &metadata);
}

void target_compileIfNeeded(const Target* target) {
  if (target_shouldCompile(target)) {
    for (size_t i = 0; i < target->dependencies.size(); ++i) {
      target_compileIfNeeded(target->dependencies[i]);
    }

    std::cout << "Compiling: " << target->mainFile.string() << "\n";
    CompilationMetadata metadata = platform_target_compile(target);
    target_saveMetadata(target, &metadata);
  }
}

// MARK: TESTS

void _test_writeMainContent(const std::string& fileToTest,
                            const std::vector<std::string>& tests,
                            std::ostream& stream) {
  stream << "#include \"TestsLib.h\"\n";
  stream << "#include \"" << fileToTest << "\"\n";
  stream << "\n";
  stream << "int _failed_tests_;\n";
  stream << "\n";
  stream << "int main() {\n";
  for (size_t i = 0; i < tests.size(); ++i) {
    stream << "  RunTest(" << tests[i] << ");\n";
  }
  stream << "  if (_failed_tests_) {\n";
  stream << "    printf(\"%d tests failed!\\n\", _failed_tests_);\n";
  stream << "  } else { \n";
  stream << "    printf(\"All tests passed!\\n\");\n";
  stream << "  }\n";
  stream << "  return _failed_tests_;\n";
  stream << "}\n";
}

void _test_createMailFile(const Target* target, const std::string& fileToTest,
                          const std::vector<std::string>& tests) {
  system_ensureDirectoryExist(target->mainFile.parent_path());
  std::ofstream file(target->mainFile);
  _test_writeMainContent(fileToTest, tests, file);
}

void _test_extractTests(std::filesystem::path filePath,
                        std::vector<std::string>& tests) {
  if (filePath.filename() == "TestsLib.h") {
    return;
  }
  std::ifstream f(filePath);
  std::stringstream ss;
  ss << f.rdbuf();
  std::string fileContent = ss.str();

  const std::string prefix = "CreateTest(";
  const std::string suffix = ")";
  std::string name;
  size_t start = 0, end = 0;
  while (start != std::string::npos) {
    start = fileContent.find(prefix, start);
    if (start == std::string::npos) {
      break;
    }
    start += prefix.length();
    end = fileContent.find(suffix, start);
    if (end == std::string::npos) {
      break;
    }
    name = fileContent.substr(start, end - start);
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
    if (name != "") {
      tests.push_back(name);
    }
  }
}

void test_compileIfNeeded(const Target* target, const std::string& fileToTest) {
  CompilationMetadata existingMetadata = target_getMetadata(target);
  if (existingMetadata.compiledAt == 0) {
    _test_createMailFile(target, fileToTest, {});
    target_compileIfNeeded(target);
    existingMetadata = target_getMetadata(target);
    std::filesystem::remove(target_outputPath(target));
  }

  if (target_shouldCompile(target)) {
    std::vector<std::string> tests;
    for (size_t i = 0; i < existingMetadata.sources.size(); ++i) {
      _test_extractTests(existingMetadata.sources[i], tests);
    }
    _test_createMailFile(target, fileToTest, tests);
    target_compileIfNeeded(target);
  }
}

// MARK: COPY

void _copy_removeRemovedFiles(std::filesystem::path outPath,
                              std::filesystem::path inPath) {
  if (!std::filesystem::exists(inPath) ||
      std::filesystem::is_directory(outPath) !=
          std::filesystem::is_directory(inPath)) {
    std::filesystem::remove_all(outPath);
    return;
  }

  if (std::filesystem::is_directory(outPath)) {
    for (auto const& dir_entry : std::filesystem::directory_iterator(outPath)) {
      _copy_removeRemovedFiles(dir_entry.path(),
                               inPath / dir_entry.path().filename());
    }
  }
}

void _copy_missingOrChangedFiles(std::filesystem::path outPath,
                                 std::filesystem::path inPath) {
  if (std::filesystem::is_directory(inPath)) {
    if (!std::filesystem::exists(outPath)) {
      std::filesystem::create_directory(outPath);
    }

    for (auto const& dir_entry : std::filesystem::directory_iterator(inPath)) {
      _copy_missingOrChangedFiles(outPath / dir_entry.path().filename(),
                                  dir_entry.path());
    }

  } else {
    if (!std::filesystem::exists(outPath) ||
        system_fileModificationTimestamp(&outPath.string()) !=
            system_fileModificationTimestamp(&inPath.string())) {
      std::filesystem::copy(inPath, outPath,
                            std::filesystem::copy_options::overwrite_existing);
    } else {
    }
  }
}

void copy_processIfNeeded(const CopyTarget* target) {
  std::cout << "Copying " << target->sourceDir.string() << "\n";
  try {
    system_ensureDirectoryExist(target->outDir);
    _copy_removeRemovedFiles(target->outDir, target->sourceDir);
    _copy_missingOrChangedFiles(target->outDir, target->sourceDir);
  } catch (const std::filesystem::filesystem_error& e) {
    std::cout << "Error: " << e.what() << "\n";
  }
  std::cout << "Done\n";
}