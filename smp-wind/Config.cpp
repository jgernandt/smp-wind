#include "pch.h"
#include "Config.h"

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

constexpr const char* HEADER = "Wind";

wind::Config::Config()
{
	for (int i = 0; i < FLOAT_COUNT; i++) {
		m_floats[i] = DEFAULTSF[i];
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
			for (int i = 0; i < FLOAT_COUNT; i++) {
				DWORD res = GetPrivateProfileString(HEADER, KEYSF[i], NULL, buf, sizeof(buf), m_path.string().c_str());
				if (res == 0) {
					m_floats[i] = DEFAULTSF[i];
				}
				else {
					float f;
					if (sscanf(buf, "%f", &f) == 0) {
						m_floats[i] = DEFAULTSF[i];
					}
					else {
						m_floats[i] = f;
					}
				}
			}
		}
		else {
			for (int i = 0; i < FLOAT_COUNT; i++) {
				set(i, DEFAULTSF[i]);
			}
		}
		return true;
	}
}

void wind::Config::set(int id, float f)
{
	assert(id >= 0 && id < FLOAT_COUNT);

	m_floats[id] = f;

	char buf[16];
	snprintf(buf, sizeof(buf), "%f", m_floats[id]);
	WritePrivateProfileString(HEADER, KEYSF[id], buf, m_path.string().c_str());
}
