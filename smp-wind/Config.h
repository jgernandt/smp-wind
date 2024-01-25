#pragma once
#include <filesystem>
#include <map>

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
			THREADS,

			INT_COUNT
		};

	public:
		Config();
		~Config();

		bool load(const std::filesystem::path& path);

		bool getb(int id) const { assert(id >= 0 && id < BOOL_COUNT); return m_bools[id]; }
		void set(int id, bool b);

		float getf(int id) const { assert(id >= 0 && id < FLOAT_COUNT); return m_floats[id]; }
		void set(int id, float f);

		int geti(int id) const { assert(id >= 0 && id < INT_COUNT); return m_ints[id]; }
		void set(int id, int i);

		bool hasBoneFactors() const { return !m_boneFactors.empty(); }
		float getBoneFactor(const char* name) const
		{
			if (auto it = m_boneFactors.find(name); it != m_boneFactors.end()) {
				return it->second;
			}
			else {
				return 1.0;
			}
		}
		void setBoneFactor(const char* name, float f) { m_boneFactors[name] = f; }

	private:
		std::filesystem::path m_path;
		float m_floats[FLOAT_COUNT];
		int m_ints[INT_COUNT];
		bool m_bools[BOOL_COUNT];

		struct StringCompare
		{
			bool operator() (const char* lhs, const char* rhs) const {
				return std::strcmp(lhs, rhs) < 0;
			}
		};

		std::map<const char*, float, StringCompare> m_boneFactors;
	};
}
