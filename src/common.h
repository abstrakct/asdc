/*
 * common.h
 */

#pragma once

#include <stdint.h>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  i32;
typedef int64_t  i64;

extern boost::random::mt19937 rng;

const u64 playerID = 1;     // Entity with ID 1 is always the player.
// how many tiles in every direction the player can see. TODO: improve this - add as component?
#define FOV 100
