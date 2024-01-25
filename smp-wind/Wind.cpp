#include "pch.h"
#include "Config.h"
#include "Timer.h"
#include "Wind.h"

//id 13878
#if CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_6_353
RelocAddr<Sky* (*)()> GetSky(0x00181810);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_6_640
RelocAddr<Sky* (*)()> GetSky(0x00183530);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_6_1130
RelocAddr<Sky* (*)()> GetSky(0x001c2550);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_6_1170
RelocAddr<Sky* (*)()> GetSky(0x001c2640);
#endif


void wind::Wind::onEvent(const hdt::PreStepEvent& e)
{
	constexpr int NFRAMES = 120;

	static std::vector<int> frameTimes(NFRAMES);
	static std::vector<int> frameObjs(NFRAMES);
	static int frame = 0;

	Timer<int, std::micro> timer;


	assert(m_config);

	m_currentTime += e.timeStep;
	m_sky = GetSky();
	if (m_sky && m_sky->mode == Sky::kFull && m_sky->windSpeed != 0.0f) {

		if (e.objects.size() > 0) {

			m_currentDir = btVector3(std::cosf(m_sky->windDirection), std::sinf(m_sky->windDirection), 0.0f);
			m_orthoDir = btVector3(std::cosf(m_sky->windDirection - 1.5708f), std::sinf(m_sky->windDirection - 1.5708f), 0.0f);

			if (m_threadPool && e.objects.size() > m_config->geti(Config::MULTITHREAD_THRESHOLD)) {

				m_objArr = &e.objects;
				m_nextElement.store(0);

				m_threadPool->release(this);

				processThreadsafe();

				m_threadPool->wait();
			}
			else {
				for (int i = 0; i < e.objects.size(); i++) {
					process(e.objects[i]);
				}
			}
		}
	}
	else {
		//We'll lose time resolution if the game runs for several hours (possible!).
		//To prevent this, reset the clock when the player is indoors.
		m_currentTime = 0.0f;
	}

	if (m_config->getb(Config::LOG_PERFORMANCE)) {
		frameObjs[frame] = e.objects.size();
		frameTimes[frame++] = timer.elapsed();

		if (frame == NFRAMES) {

			frame = 0;

			int meant = 0;
			int maxt = 0;
			int mint = std::numeric_limits<int>::max();
			int maxo = 0;
			int mino = std::numeric_limits<int>::max();
			for (int i = 0; i < NFRAMES; i++) {
				meant += frameTimes[i];
				maxt = std::max(frameTimes[i], maxt);
				mint = std::min(frameTimes[i], mint);
				maxo = std::max(frameObjs[i], maxo);
				mino = std::min(frameObjs[i], mino);
			}
			meant /= NFRAMES;

			float var = 0.0f;
			for (auto f : frameTimes) {
				var += (f - meant) * (f - meant);
			}
			var /= NFRAMES;

			_MESSAGE("Mean time (%d updates): %3d ± %-3d (%3d - %-3d) microseconds (%3d - %-3d collision objects)",
				NFRAMES, meant, (int)std::sqrt(var), mint, maxt, mino, maxo);
		}
	}
}

void wind::Wind::init(const Config& config)
{
	m_config = &config;
	updateThreadCount();
}

void wind::Wind::updateThreadCount()
{
	int threads = std::min(m_config->geti(Config::THREADS), ThreadPool::MAX_THREADS);

	if (m_threadPool && threads != m_threadPool->size() + 1) {
		m_threadPool.reset();
	}

	if (threads > 1) {
		m_threadPool = std::make_unique<ThreadPool>(threads - 1);
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

		float factor = 1.0;

		if (m_config->hasBoneFactors()) {

			//hack
			//The btRigidBody should be a member of an hdt::SkyrimBone, which also has the bone's NiNode* as a member
			//offsetof(hdt::SkyrimBone, m_name) == 16		doesn't actually store the name?
			//offsetof(hdt::SkyrimBone, m_rig) == 48		btRigidBody
			//offsetof(hdt::SkyrimBone, m_node) == 1112		NiNode*
			//NOTE: This is NOT part of FSMPs public API and may break, without warning, in any past or future version
			auto node = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(body) + 1064);
			if (node) {

				//name is really a BSFixedString, but we're unable to exploit this due to the renaming
				auto name = *reinterpret_cast<const char**>(node + 16);
				if (name) {

					//they should have added "hdtSSEPhysics_AutoRename_Armor_XXXXXXXX " or 
					// "hdtSSEPhysics_AutoRename_Head_XXXXXXXX " before the bone name

					const char* begin = name;

					if (name[25] == 'A') {
						begin = name + 40;
					}
					else if (name[25] == 'H') {
						begin = name + 39;
					}

					factor = m_config->getBoneFactor(begin);
				}
			}
		}

		if (factor > 0.0) {
			//scale by 100 * m, since that's the oom we adapted the wind for
			float rescale = m_config->getb(Config::MASS_INDEPENDENT) ? 100.0f * body->getMass() : 1.0f;
			body->applyCentralForce(factor * rescale * eval(body->getWorldTransform().getOrigin()));
		}
	}
}

void wind::Wind::processThreadsafe()
{
	assert(m_objArr);

	for (int i = m_nextElement.fetch_add(1); i < m_objArr->size(); i = m_nextElement.fetch_add(1)) {
		process((*m_objArr)[i]);
	}
}

wind::Wind::ThreadPool::ThreadPool(int count) :
	m_barrier(count + 1)
{
	assert(owner);
	assert(count > 0 && count <= MAX_THREADS);

	m_threads.resize(count);

	for (auto&& thread : m_threads) {
		thread = std::thread(&ThreadPool::worker, this);
	}
}

wind::Wind::ThreadPool::~ThreadPool()
{
	release(nullptr);

	for (auto&& thread : m_threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

void wind::Wind::ThreadPool::release(Wind* target)
{
	m_target = target;
	m_signal.release(m_threads.size());
}

void wind::Wind::ThreadPool::worker()
{
	while (true) {
		m_signal.acquire();

		if (m_target) {
			m_target->processThreadsafe();
			m_barrier.arrive_and_wait();
		}
		else {
			break;
		}
	}
}
