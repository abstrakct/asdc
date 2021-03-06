/*
 * common.h
 */

#pragma once

#include <stdint.h>
#include <random>

typedef std::vector<std::pair<int, int>> PointVector;

extern std::mt19937 rng;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  i32;
typedef int64_t  i64;

extern u64 playerID;

