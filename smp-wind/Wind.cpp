#include "pch.h"
#include "Config.h"
#include "Timer.h"
#include "Wind.h"

RelocAddr<Sky* (*)()> GetSky(0x00183530);

void wind::Wind::onEvent(const hdt::PreStepEvent& e)
{
	//static float average = 0.0f;
	//static int steps = 0;

	//Timer<long long, std::micro> timer;

	m_currentTime += e.timeStep;

	assert(m_sky && m_config);

	if (m_sky->mode == Sky::kFull && m_sky->windSpeed != 0.0f) {

		m_currentDir = btVector3(std::cosf(m_sky->windDirection), std::sinf(m_sky->windDirection), 0.0f);
		m_orthoDir = btVector3(std::cosf(m_sky->windDirection - 1.5708f), std::sinf(m_sky->windDirection - 1.5708f), 0.0f);

		//auto&& bodies = e.world->getCollisionObjectArray();
		for (int i = 0; i < e.objects.size(); i++) {
			btRigidBody* body = btRigidBody::upcast(e.objects[i]);
			if (body && !body->isStaticOrKinematicObject()) {
				//scale by 100 * m, since that's the oom we adapted the wind for
				float rescale = m_config->getb(Config::MASS_INDEPENDENT) ? 100.0f * body->getMass() : 1.0f;
				body->applyCentralForce(rescale * eval(body->getWorldTransform().getOrigin()));
			}
		}
	}
	else {
		//We'll lose time resolution if the game runs for several hours (possible!).
		//To prevent this, reset the clock when the player is indoors.
		m_currentTime = 0.0f;
	}

	//average = 0.75f * average + 0.25f * timer.elapsed();
	//steps++;

	//if (steps % 120 == 0) {
	//	_MESSAGE("Average time: %f", average);
	//}
}

void wind::Wind::init(const Config& config)
{
	m_config = &config;
	m_sky = GetSky();
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
