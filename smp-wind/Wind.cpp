#include "pch.h"
#include "Config.h"
#include "Timer.h"
#include "Wind.h"
#include "version.h"

RelocAddr<Sky* (*)()> GetSky(GetSkyOffset);

wind::Wind::~Wind()
{
	shutdown();
}

void wind::Wind::onEvent(const hdt::PreStepEvent& e)
{
#ifdef LOG_TIMER
	static float averageTime = 0.0f;
	static float averageObjs = 0.0f;
	static int steps = 0;

	Timer<long long, std::micro> timer;
#endif

	assert(m_config);

	m_currentTime += e.timeStep;
	m_sky = GetSky();
	if (m_sky && m_sky->mode == Sky::kFull && m_sky->windSpeed != 0.0f) {

		m_currentDir = btVector3(std::cosf(m_sky->windDirection), std::sinf(m_sky->windDirection), 0.0f);
		m_orthoDir = btVector3(std::cosf(m_sky->windDirection - 1.5708f), std::sinf(m_sky->windDirection - 1.5708f), 0.0f);

		//The overhead of waking up the threads is bigger than the time save, unless there are a lot of objects to process.
		//This is likely to be situational, though.
		//Potentially, this could be tuned automatically by alternating between the paths and favouring whichever
		//is currently faster. Probably not worth the trouble, though.
		if (e.objects.size() < m_config->geti(Config::MULTITHREAD_THRESHOLD)) {
			//do it ourselves
			for (int i = 0; i < e.objects.size(); i++) {
				process(e.objects[i]);
			}
		}
		else {
			//call the workers
			m_objectArray = &e.objects;
			m_arrayIndex.store(e.objects.size(), std::memory_order::release);
			m_workerCount.store(WORKERS, std::memory_order::release);
			m_startSignal.release(WORKERS);
			m_stopSignal.acquire();
			m_objectArray = nullptr;
		}
	}
	else {
		//We'll lose time resolution if the game runs for several hours (possible!).
		//To prevent this, reset the clock when the player is indoors.
		m_currentTime = 0.0f;
	}

#ifdef LOG_TIMER
	averageTime = 0.75f * averageTime + 0.25f * timer.elapsed();
	averageObjs = 0.75f * averageObjs + 0.25f * e.objects.size();
	steps++;

	if (steps % 120 == 0) {
		_MESSAGE("Average time: %f", averageTime);
		_MESSAGE("Average number of objects: %f", averageObjs);
	}
#endif
}

void wind::Wind::init(const Config& config)
{
	m_config = &config;

	for (auto&& thread : m_workers) {
		assert(thread.get_id() == std::thread::id());
		thread = std::thread(&Wind::worker, this);
	}
}

void wind::Wind::shutdown()
{
	m_objectArray = nullptr;
	m_startSignal.release(WORKERS);

	for (auto&& thread : m_workers) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

btVector3 wind::Wind::eval(const btVector3& at)
{
	assert(m_sky && m_config);

	//Let wind speed increase linearly with height above sea level (z ~= -14000)
	float speed = (1.0f + m_config->getf(Config::HEIGHTFACTOR) * (1.0e-4f * at[2] + 1.4f)) * m_sky->windSpeed;

	if (speed > 0.0f) {
		//Spatial oscillation should have a period on the order of meters.
		//Phase velocity in the wind direction increases with wind speed.
		float U = 2.0e-3f * at.dot(m_currentDir) / speed;
		float V = 4.0e-3f * at.dot(m_orthoDir);
		float W = 2.0e-3f * at[2];

		//High frequency implies high gust speed, so divide spatial phase shift by frequency
		float phase01 = m_config->getf(Config::OSC01FREQ) * m_currentTime - U / m_config->getf(Config::OSC01FREQ) - V - W;
		float phase02 = m_config->getf(Config::OSC02FREQ) * m_currentTime - U / m_config->getf(Config::OSC02FREQ) - V - W;

		//Two different frequencies makes fluctuations less predictable
		btVector3 base = (1.0f + 0.5f * m_config->getf(Config::OSC01FORCE) * (std::cosf(phase01) + std::cosf(0.2692f * phase01))) * m_currentDir;

		float angle = m_sky->windDirection + 0.5f * m_config->getf(Config::OSC02SPAN) * (std::cosf(phase02) + std::cosf(0.3101f * phase02));
		btVector3 spread = m_config->getf(Config::OSC02FORCE) * btVector3(std::cosf(angle), std::sinf(angle), 0.0f);

		//Fractions of a length unit is effectively noise (order of mm)
		btVector3 noise = m_config->getf(Config::NOISE) * btVector3(
			std::fmodf(10.0f * at[0], 2.0f) - std::copysignf(1.0f, at[0]),
			std::fmodf(10.0f * at[1], 2.0f) - std::copysignf(1.0f, at[1]),
			std::fmodf(10.0f * at[2], 2.0f) - std::copysignf(1.0f, at[2]));

		return m_config->getf(Config::FORCE) * speed * (base + spread + noise);
	}
	else {
		return btVector3(0.0f, 0.0f, 0.0f);
	}
}

void wind::Wind::process(btCollisionObject* object)
{
	assert(m_config);

	btRigidBody* body = btRigidBody::upcast(object);
	if (body && !body->isStaticOrKinematicObject()) {
		//scale by 100 * m, since that's the oom we adapted the wind for
		float rescale = m_config->getb(Config::MASS_INDEPENDENT) ? 100.0f * body->getMass() : 1.0f;
		body->applyCentralForce(rescale * eval(body->getWorldTransform().getOrigin()));
	}
}

void wind::Wind::worker()
{
	while (true) {
		m_startSignal.acquire();

		if (auto objArray = m_objectArray) {
			//do work
			int i = m_arrayIndex.fetch_sub(1, std::memory_order::acq_rel) - 1;
			while (i >= 0) {
				process((*objArray)[i]);
				i = m_arrayIndex.fetch_sub(1, std::memory_order::acq_rel) - 1;
			}

			//done
			if (m_workerCount.fetch_sub(1, std::memory_order::acq_rel) == 1) {
				//we're the last to finish
				m_stopSignal.release();
			}
		}
		else {
			//terminate
			break;
		}
	}
}
