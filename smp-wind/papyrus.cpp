#include "pch.h"
#include "Config.h"
#include "Wind.h"

namespace wind
{
	extern Config g_config;
	extern Config g_configDefault;

	static bool getBool(StaticFunctionTag*, SInt32 id)
	{
		if (id >= 0 && id < Config::BOOL_COUNT) {
			return g_config.getb(id);
		}
		else {
			return false;
		}
	}

	static bool getBoolDefault(StaticFunctionTag*, SInt32 id)
	{
		if (id >= 0 && id < Config::BOOL_COUNT) {
			return g_configDefault.getb(id);
		}
		else {
			return false;
		}
	}

	static void setBool(StaticFunctionTag*, SInt32 id, bool b)
	{
		if (id >= 0 && id < Config::BOOL_COUNT) {
			g_config.set(id, b);
		}
	}

	static float getFloat(StaticFunctionTag*, SInt32 id)
	{
		if (id >= 0 && id < Config::FLOAT_COUNT) {
			return g_config.getf(id);
		}
		else {
			return 0.0f;
		}
	}

	static float getFloatDefault(StaticFunctionTag*, SInt32 id)
	{
		if (id >= 0 && id < Config::FLOAT_COUNT) {
			return g_configDefault.getf(id);
		}
		else {
			return 0.0f;
		}
	}

	static void setFloat(StaticFunctionTag*, SInt32 id, float f)
	{
		if (id >= 0 && id < Config::FLOAT_COUNT) {
			g_config.set(id, f);
		}
	}

	static SInt32 getInt(StaticFunctionTag*, SInt32 id)
	{
		if (id >= 0 && id < Config::INT_COUNT) {
			return g_config.geti(id);
		}
		else {
			return 0;
		}
	}

	static SInt32 getIntDefault(StaticFunctionTag*, SInt32 id)
	{
		if (id >= 0 && id < Config::INT_COUNT) {
			return g_configDefault.geti(id);
		}
		else {
			return 0;
		}
	}

	static void setInt(StaticFunctionTag*, SInt32 id, SInt32 i)
	{
		if (id >= 0 && id < Config::INT_COUNT) {
			g_config.set(id, static_cast<int>(i));
		}
	}


	bool registerFunctions(VMClassRegistry* vmcr)
	{
		assert(vmcr);

		vmcr->RegisterFunction(new NativeFunction1<StaticFunctionTag,
			bool,
			SInt32>
			("GetBool", "JGWD_MCM", &getBool, vmcr));
		vmcr->RegisterFunction(new NativeFunction1<StaticFunctionTag,
			bool,
			SInt32>
			("GetBoolDefault", "JGWD_MCM", &getBoolDefault, vmcr));
		vmcr->RegisterFunction(new NativeFunction2<StaticFunctionTag,
			void,
			SInt32, bool>
			("SetBool", "JGWD_MCM", &setBool, vmcr));

		vmcr->RegisterFunction(new NativeFunction1<StaticFunctionTag,
			float,
			SInt32>
			("GetFloat", "JGWD_MCM", &getFloat, vmcr));
		vmcr->RegisterFunction(new NativeFunction1<StaticFunctionTag,
			float,
			SInt32>
			("GetFloatDefault", "JGWD_MCM", &getFloatDefault, vmcr));
		vmcr->RegisterFunction(new NativeFunction2<StaticFunctionTag,
			void,
			SInt32, float>
			("SetFloat", "JGWD_MCM", &setFloat, vmcr));

		vmcr->RegisterFunction(new NativeFunction1<StaticFunctionTag,
			SInt32,
			SInt32>
			("GetInt", "JGWD_MCM", &getInt, vmcr));
		vmcr->RegisterFunction(new NativeFunction1<StaticFunctionTag,
			SInt32,
			SInt32>
			("GetIntDefault", "JGWD_MCM", &getIntDefault, vmcr));
		vmcr->RegisterFunction(new NativeFunction2<StaticFunctionTag,
			void,
			SInt32, SInt32>
			("SetInt", "JGWD_MCM", &setInt, vmcr));

		vmcr->SetFunctionFlags("JGWD_MCM", "GetBool", VMClassRegistry::kFunctionFlag_NoWait);
		vmcr->SetFunctionFlags("JGWD_MCM", "GetBoolDefault", VMClassRegistry::kFunctionFlag_NoWait);

		vmcr->SetFunctionFlags("JGWD_MCM", "GetFloat", VMClassRegistry::kFunctionFlag_NoWait);
		vmcr->SetFunctionFlags("JGWD_MCM", "GetFloatDefault", VMClassRegistry::kFunctionFlag_NoWait);

		vmcr->SetFunctionFlags("JGWD_MCM", "GetInt", VMClassRegistry::kFunctionFlag_NoWait);
		vmcr->SetFunctionFlags("JGWD_MCM", "GetIntDefault", VMClassRegistry::kFunctionFlag_NoWait);

		return true;
	}
}
