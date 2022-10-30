#pragma once
#include <filesystem>

namespace wind
{

	class Config
	{
	public:
		enum BoolID : int
		{
			MASS_INDEPENDENT,

			BOOL_COUNT
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
		enum IntID : int
		{
			MULTITHREAD_THRESHOLD,

			INT_COUNT
		};

	public:
		Config();

		bool load(const std::filesystem::path& path);

		bool getb(int id) const { assert(id >= 0 && id < BOOL_COUNT); return m_bools[id]; }
		void set(int id, bool b);

		float getf(int id) const { assert(id >= 0 && id < FLOAT_COUNT); return m_floats[id]; }
		void set(int id, float f);

		int geti(int id) const { assert(id >= 0 && id < INT_COUNT); return m_ints[id]; }
		void set(int id, int i);

	private:
		std::filesystem::path m_path;
		float m_floats[FLOAT_COUNT];
		int m_ints[INT_COUNT];
		bool m_bools[BOOL_COUNT];
	};
}
