#pragma once

class VMClassRegistry;
struct StaticFunctionTag;

namespace wind
{
	bool registerFunctions(VMClassRegistry* vcr);

	float getFloat(StaticFunctionTag*, SInt32 id);
	float getFloatDefault(StaticFunctionTag*, SInt32 id);
	void setFloat(StaticFunctionTag*, SInt32 id, float f);
}