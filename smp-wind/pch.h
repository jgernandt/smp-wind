#ifndef PCH_H
#define PCH_H

#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <random>
#include <semaphore>
#include <string>
#include <thread>

#include <ShlObj.h>
#include <Windows.h>
#undef min
#undef max

#include "common/IDebugLog.h"
#include "common/ITypes.h"

#include "skse64_common/Relocation.h"
#include "skse64_common/skse_version.h"

#include "skse64/GameForms.h"
#include "skse64/PapyrusArgs.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PluginAPI.h"

#define BT_USE_SSE_IN_API
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

#endif //PCH_H
