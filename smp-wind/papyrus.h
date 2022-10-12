#pragma once

class VMClassRegistry;
struct StaticFunctionTag;

namespace wind
{
	bool registerFunctions(VMClassRegistry* vcr);

	bool getBool(StaticFunctionTag*, SInt32 id);
	bool getBoolDefault(StaticFunctionTag*, SInt32 id);
	void setBool(StaticFunctionTag*, SInt32 id, bool b);

	float getFloat(StaticFunctionTag*, SInt32 id);
	float getFloatDefault(StaticFunctionTag*, SInt32 id);
	void setFloat(StaticFunctionTag*, SInt32 id, float f);
}