#pragma once

#include <string>
#include <cctype>
#include "common.h"

#define CALL_MEMBER_FN(object, ptr) ((object).*(ptr))()

signed int ability_modifier(int ab);
u64 ri(u64 a, u64 b);
bool fiftyfifty();
bool x_in_y(u64 x, u64 y);
bool one_in(int chance);
int dice(int num, int sides, signed int modifier);
int roll_die();

u32 PT_ColorizePixel(u32 dest, u32 src);

// vim: fdm=syntax ft=cpp
