#pragma once

#include "PluginAPI.h"

#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

class TESClimate;
class TESWeather;

//A stripped-down version of the CommonlibSSE definition
class Sky
{
public:
	enum Mode : std::uint32_t
	{
		kNone,
		kInterior,
		kSkyDomeOnly,
		kFull,
		kTotal,
	};

	void* vtable;
	std::uint32_t unused01[14];
	TESClimate* currentClimate;                 //040
	TESWeather* currentWeather;                 //048
	TESWeather* lastWeather;                    //050
	TESWeather* nextWeather;					//058
	TESWeather* overrideWeather;                //060
	std::uint32_t unused02[73];
	float windSpeed;							//18c
	float windDirection;						//190
	std::uint32_t unused03[9];
	float transition;							//1b8
	Mode mode;									//1bc
	std::uint32_t unused04[66];
};
static_assert(sizeof(Sky) == 0x2c8);
static_assert(offsetof(Sky, currentClimate) == 0x40);
static_assert(offsetof(Sky, currentWeather) == 0x48);
static_assert(offsetof(Sky, lastWeather) == 0x50);
static_assert(offsetof(Sky, nextWeather) == 0x58);
static_assert(offsetof(Sky, overrideWeather) == 0x60);
static_assert(offsetof(Sky, windSpeed) == 0x18c);
static_assert(offsetof(Sky, windDirection) == 0x190);
static_assert(offsetof(Sky, transition) == 0x1b8);
static_assert(offsetof(Sky, mode) == 0x1bc);

namespace wind
{
	class Config;

	class Wind final : public hdt::IPreStepListener
	{
	public:
		virtual void onEvent(const hdt::PreStepEvent& e) override;

		void init(const Config& config);

	private:
		btVector3 eval(const btVector3& at);

	private:
		const Sky* m_sky{ nullptr };
		const Config* m_config{ nullptr };

		float m_currentTime{ 0.0f };
		btVector3 m_currentDir;
		btVector3 m_orthoDir;
	};
}