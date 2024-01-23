#pragma once
#include "skse64_common/skse_version.h"

constexpr unsigned long VERSION_MAJOR{ 2 };
constexpr unsigned long VERSION_MINOR{ 2 };
constexpr unsigned long VERSION_PATCH{ 0 };
constexpr unsigned long VERSION_TARGET{ RUNTIME_VERSION_1_6_1170 };
constexpr unsigned long VERSION = (VERSION_MAJOR & 0xFF) << 24 | (VERSION_MINOR & 0xFF) << 16 | (VERSION_PATCH & 0xFF) << 8;

//id 13878
//constexpr unsigned long GetSkyOffset{ 0x00181810 };//353
//constexpr unsigned long GetSkyOffset{ 0x00183530 };//640
//constexpr unsigned long GetSkyOffset{ 0x001c2550 };//1130
constexpr unsigned long GetSkyOffset{ 0x001c2640 };//1170
