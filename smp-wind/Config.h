#pragma once
#include <filesystem>

namespace wind
{

	class Config
	{
	public:
		struct Floats
		{
			float force{ 15.0f };
			float osc01force{ 0.6f };
			float osc01freq{ 2.87f };
			float osc02force{ 0.4f };
			float osc02freq{ 6.93f };
			float osc02span{ 1.0f };
			float noise{ 1.0f };
			float heightFactor{ 1.0f };
		};
		enum FloatID : int
		{
			FORCE,
			OSC01FORCE,
			OSC01FREQ,
			OSC02FORCE,
			OSC02FREQ,
			OSC02SPAN,
			NOISE,
			HEIGHTFACTOR,
			FLOAT_COUNT
		};

	public:
		Config();

		bool load(const std::filesystem::path& path);

		float getf(int id) const { assert(id >= 0 && id < FLOAT_COUNT); return m_floats[id]; }
		void set(int id, float f);

	private:
		float m_floats[FLOAT_COUNT];
		std::filesystem::path m_path;
	};
}
