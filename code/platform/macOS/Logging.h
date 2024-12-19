#pragma once

#include <cstdio>

#define LOG_ERROR(message, ...) printf(message, ##__VA_ARGS__)