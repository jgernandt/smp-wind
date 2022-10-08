#include "pch.h"
#include "Config.h"
#include "papyrus.h"
#include "Wind.h"

namespace wind
{
	extern Config g_config;
	extern Config g_configDefault;
}

bool wind::registerFunctions(VMClassRegistry* vmcr)
{
	assert(vmcr);

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

	vmcr->SetFunctionFlags("JGWD_MCM", "GetFloat", VMClassRegistry::kFunctionFlag_NoWait);
	vmcr->SetFunctionFlags("JGWD_MCM", "GetFloatDefault", VMClassRegistry::kFunctionFlag_NoWait);

	return true;
}

float wind::getFloat(StaticFunctionTag*, SInt32 id)
{
	if (id >= 0 && id < Config::FLOAT_COUNT) {
		return g_config.getf(id);
	}
	else {
		return 0.0f;
	}
}

float wind::getFloatDefault(StaticFunctionTag*, SInt32 id)
{
	if (id >= 0 && id < Config::FLOAT_COUNT) {
		return g_configDefault.getf(id);
	}
	else {
		return 0.0f;
	}
}

void wind::setFloat(StaticFunctionTag*, SInt32 id, float f)
{
	if (id >= 0 && id < Config::FLOAT_COUNT) {
		g_config.set(id, f);
	}
}
