#pragma once
#include <stdio.h>

extern int _failed_tests_;

#define CreateTest(name) void name()

#define RunTest(name)                   \
  {                                     \
    int failsSoFar = _failed_tests_;    \
    printf("Running %s:\n", #name);     \
    name();                             \
    if (failsSoFar == _failed_tests_) { \
      printf("\b \033[32mOK\033[0m\n"); \
    }                                   \
  }

#define TEST_FAIL(message)                                         \
  {                                                                \
    _failed_tests_ += 1;                                           \
    printf(__FILE__ ":%d \033[31m%s\033[0m\n", __LINE__, message); \
    return;                                                        \
  }

#define TEST_ASSERT(formula, ...) \
  _TEST_ASSERT(formula, __VA_ARGS__ __VA_OPT__(, ) "")

#define _TEST_ASSERT(formula, message, ...) \
  {                                         \
    if (!(formula)) {                       \
      if (message[0]) {                     \
        TEST_FAIL(message);                 \
      } else {                              \
        TEST_FAIL(#formula " failed");      \
      }                                     \
    }                                       \
  }
