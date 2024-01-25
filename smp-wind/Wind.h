#pragma once

#include <barrier>
#include <semaphore>
#include <thread>

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
	private:
		class ThreadPool
		{
		public:
			static constexpr int MAX_THREADS = 128;

		public:
			ThreadPool(int count);
			ThreadPool(const ThreadPool&) = delete;

			~ThreadPool();

			ThreadPool& operator=(const ThreadPool&) = delete;

			int size() const { return (int)m_threads.size(); }

			void release(Wind* target);
			void wait() { m_barrier.arrive_and_wait(); }

		private:
			void worker();

		private:
			std::vector<std::thread> m_threads;
			std::counting_semaphore<MAX_THREADS> m_signal{ 0 };
			std::barrier<> m_barrier;
			Wind* m_target{ nullptr };
		};

	public:
		Wind() = default;
		~Wind() = default;

		virtual void onEvent(const hdt::PreStepEvent& e) override;

		void init(const Config& config);
		void updateThreadCount();

	private:
		btVector3 eval(const btVector3& at);
		void process(btCollisionObject* object);
		void processThreadsafe();

	private:
		const Sky* m_sky{ nullptr };
		const Config* m_config{ nullptr };

		float m_currentTime{ 0.0f };
		btVector3 m_currentDir;
		btVector3 m_orthoDir;

		std::unique_ptr<ThreadPool> m_threadPool;
		const btAlignedObjectArray<btCollisionObject*>* m_objArr{ nullptr };
		std::atomic<int> m_nextElement{ -1 };
	};
}