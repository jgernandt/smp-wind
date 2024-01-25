#include "pch.h"
#include "Config.h"

constexpr bool DEFAULTSB[wind::Config::BOOL_COUNT]{
	false,
	false,
};

constexpr const char* KEYSB[wind::Config::BOOL_COUNT]{
	"bMassIndependent",
	"bLogPerformance",
};

constexpr float DEFAULTSF[wind::Config::FLOAT_COUNT]{
	10.0f,
	0.67f,
	4.29f,
	0.55f,
	8.93f,
	1.0f,
	1.0f,
	1.0f,
};

constexpr const char* KEYSF[wind::Config::FLOAT_COUNT]{
	"fOverallForce",
	"fOsc01Force",
	"fOsc01Frequency",
	"fOsc02Force",
	"fOsc02Frequency",
	"fOsc02Span",
	"fNoise",
	"fHeightFactor",
};

constexpr int DEFAULTSI[wind::Config::INT_COUNT]{
	50,
	4,
};

constexpr const char* KEYSI[wind::Config::INT_COUNT]{
	"iMultithreadThreshold",
	"iThreads",
};

constexpr const char* HEADER = "Wind";

wind::Config::Config()
{
	for (int i = 0; i < BOOL_COUNT; i++) {
		m_bools[i] = DEFAULTSB[i];
	}
	for (int i = 0; i < FLOAT_COUNT; i++) {
		m_floats[i] = DEFAULTSF[i];
	}
	for (int i = 0; i < INT_COUNT; i++) {
		m_ints[i] = DEFAULTSI[i];
	}
}

wind::Config::~Config()
{
	for (auto item : m_boneFactors) {
		delete[] item.first;
	}
}

bool wind::Config::load(const std::filesystem::path& path)
{
	if (path.extension().string() != ".ini") {
		return false;
	}
	else {
		m_path = path;

		if (std::filesystem::exists(path)) {
			char buf[256];
			for (int i = 0; i < BOOL_COUNT; i++) {
				DWORD res = GetPrivateProfileString(HEADER, KEYSB[i], NULL, buf, sizeof(buf), m_path.string().c_str());
				if (res == 0) {
					m_bools[i] = DEFAULTSB[i];
				}
				else {
					int j;
					if (sscanf(buf, "%d", &j) == 0) {
						m_bools[i] = DEFAULTSB[i];
					}
					else {
						m_bools[i] = static_cast<bool>(j);
					}
				}
			}

			for (int i = 0; i < FLOAT_COUNT; i++) {
				DWORD res = GetPrivateProfileString(HEADER, KEYSF[i], NULL, buf, sizeof(buf), m_path.string().c_str());
				if (res == 0) {
					m_floats[i] = DEFAULTSF[i];
				}
				else {
					errno = 0;
					char* end;
					float result = std::strtof(buf, &end);
					if (end == &buf[0] || errno == ERANGE) {
						_WARNING("WARNING: invalid float value %s for %s. Using default.", buf, KEYSF[i]);
						m_floats[i] = DEFAULTSF[i];
					}
					else {
						m_floats[i] = result;
					}
				}
			}
			for (int i = 0; i < INT_COUNT; i++) {
				DWORD res = GetPrivateProfileString(HEADER, KEYSI[i], NULL, buf, sizeof(buf), m_path.string().c_str());
				if (res == 0) {
					m_ints[i] = DEFAULTSI[i];
				}
				else {
					errno = 0;
					char* end;
					int result = std::strtol(buf, &end, 10);
					if (end == &buf[0] || errno == ERANGE) {
						_WARNING("WARNING: invalid integer value %s for %s. Using default.", buf, KEYSI[i]);
						m_ints[i] = DEFAULTSI[i];
					}
					else {
						m_ints[i] = result;
					}
				}
			}

			std::ifstream file(path);

			while (file.getline(buf, 256)) {
				if (strcmp(buf, "[Bones]") == 0) {
					break;
				}
			}

			while (file.good()) {

				char next = file.peek();
				if (std::isspace(next) || next == '[' || !file.good()) {
					break;
				}

				auto s = new char[64];
				file.getline(s, 64, '=');

				file.getline(buf, 64);

				if (!m_boneFactors.contains(s)) {

					float f = std::max(std::strtof(buf, nullptr), 0.0f);

					if (f != 1.0) {
						m_boneFactors[s] = f;

						_MESSAGE("Scaling wind on bone \"%s\" by a factor %g.", s, f);
					}
				}
				else {
					_WARNING("WARNING: Ignoring duplicate entry for bone \"%s\".", s);
					delete[] s;
				}
			}
		}
		else {
			for (int i = 0; i < BOOL_COUNT; i++) {
				set(i, DEFAULTSB[i]);
			}
			for (int i = 0; i < FLOAT_COUNT; i++) {
				set(i, DEFAULTSF[i]);
			}
			for (int i = 0; i < INT_COUNT; i++) {
				set(i, DEFAULTSI[i]);
			}
		}
		return true;
	}
}

void wind::Config::set(int id, bool b)
{
	assert(id >= 0 && id < BOOL_COUNT);

	m_bools[id] = b;

	WritePrivateProfileString(HEADER, KEYSB[id], b ? "1" : "0", m_path.string().c_str());
}

void wind::Config::set(int id, float f)
{
	assert(id >= 0 && id < FLOAT_COUNT);

	m_floats[id] = f;

	char buf[16];
	snprintf(buf, sizeof(buf), "%f", m_floats[id]);
	WritePrivateProfileString(HEADER, KEYSF[id], buf, m_path.string().c_str());
}

void wind::Config::set(int id, int i)
{
	assert(id >= 0 && id < INT_COUNT);

	m_ints[id] = i;

	char buf[16];
	snprintf(buf, sizeof(buf), "%d", m_ints[id]);
	WritePrivateProfileString(HEADER, KEYSI[id], buf, m_path.string().c_str());
}
