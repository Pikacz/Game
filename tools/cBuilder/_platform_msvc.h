#pragma once

#include "_base.h"

#ifdef TARGETING_MSVC
#include <direct.h>

const CompilationFlag CompilationFlag::cpp17 = CompilationFlag{"/std:c++17"};
const CompilationFlag CompilationFlag::usesExceptions =
    CompilationFlag{"/EHsc"};

CompilationFlag CompilationFlag::include(std::filesystem::path const& path) {
  std::stringstream ss;
  ss << "/I " << path.string();
  return CompilationFlag{ss.str()};
}

CompilationFlag CompilationFlag::define(std::string const& varName,
                                        const char* value) {
  std::stringstream ss;
  ss << "/D " << varName;
  if (value) {
    ss << "=" << value;
  }
  return CompilationFlag{ss.str()};
}

std::filesystem::path platform_target_outputPath(std::filesystem::path mainFile,
                                                 std::filesystem::path outDir,
                                                 TargetType type) {
  std::string filename = mainFile.stem().string();
  if (type == TargetExecutable) {
    filename += ".exe";
  } else if (type == TargetObject) {
    filename += ".obj";
  } else if (type == TargetDynamicLibrary) {
    filename += ".dll";
  } else {
    std::abort();
  }
  return outDir / filename;
}

std::vector<std::string> _parseClIncludes(std::string const* str) {
  std::vector<std::string> result;

  const std::string newLine = "\n";
  const std::string noteInclude = "Note: including file:";

  size_t start = 0;

  size_t found;
  std::string value;

  found = 0;
  while ((found = str->find(noteInclude, start)) != std::string::npos) {
    start = found + noteInclude.length();
    while ((*str)[start] == ' ') {
      start += 1;
    }
    found = str->find(newLine, start);
    value = str->substr(start, found - start);
    start = found + newLine.length();

    if (value.find("Microsoft Visual Studio") == std::string::npos &&
        value.find("Windows Kits") == std::string::npos) {
      result.push_back(value);
    }
  }
  return result;
}

CompilationMetadata platform_target_compile(const Target* target) {
  system_ensureDirectoryExist(target->outDir);
  _chdir(target->outDir.string().c_str());
  CompilationMetadata metadata;
  std::stringstream ss;
  ss << "cl /showIncludes /Zc:preprocessor ";
  if (target->type == TargetObject) {
    ss << "/c ";
  }
  ss << target->mainFile.string() << " ";
  for (size_t i = 0; i < target->flags.size(); i++) {
    ss << target->flags[i].val << " ";
  }

  ss << "/link ";
  if (target->type == TargetDynamicLibrary) {
    ss << "/DLL /OUT:" << target->mainFile.stem() << ".dll ";
  }
  ss << "/INCREMENTAL:NO ";
  for (size_t i = 0; i < target->linkFlags.size(); i++) {
    ss << target->linkFlags[i].val << " ";
  }

  std::string command = ss.str();
  std::string compilationResult = system_exec(command.c_str());
  metadata.sources = _parseClIncludes(&compilationResult);

  auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      std::chrono::system_clock::now());
  time_t time = std::chrono::system_clock::to_time_t(sctp);
  metadata.compiledAt = static_cast<int64_t>(time);
  return metadata;
}

#endif