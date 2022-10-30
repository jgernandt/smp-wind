#include "pch.h"

#include "PluginHelper.h"

#include "Config.h"
#include "Wind.h"

class VMClassRegistry;

namespace wind
{
	constexpr unsigned long VERSION_MAJOR{ 1 };
	constexpr unsigned long VERSION_MINOR{ 1 };
	constexpr unsigned long VERSION_PATCH{ 0 };
	constexpr unsigned long pluginVersion = (VERSION_MAJOR & 0xFF) << 24 | (VERSION_MINOR & 0xFF) << 16 | (VERSION_PATCH & 0xFF) << 8;

	Config g_config;
	Config g_configDefault;
	Wind g_wind;

	bool registerFunctions(VMClassRegistry* vcr);
}

class MyHelper
{
public:
	inline static hdt::PluginInterface::Version interfaceMin{ 1, 0, 0 };
	inline static hdt::PluginInterface::Version interfaceMax{ 2, 0, 0 };

	inline static hdt::PluginInterface::Version bulletMin{ hdt::PluginInterface::BULLET_VERSION };
	inline static hdt::PluginInterface::Version bulletMax{ hdt::PluginInterface::BULLET_VERSION.major + 1, 0, 0 };

	static void onConnect(hdt::PluginInterface* smp)
	{
		PWSTR wpath;
		HRESULT res = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &wpath);
		if (SUCCEEDED(res)) {
			std::filesystem::path documents(wpath, std::filesystem::path::native_format);
			std::filesystem::path configPath(documents / "My Games\\Skyrim Special Edition\\SKSE\\SMP Wind.ini");

			_MESSAGE("Loading config file %s...", configPath.string().c_str());

			if (wind::g_config.load(configPath)) {
				_MESSAGE("Config file loaded.\n");
			}
			else {
				_WARNING("WARNING: Failed to load config file. Settings will not be saved.\n");
			}
		}
		else {
			_WARNING("WARNING: Documents folder not found. Settings will not be saved.\n");
		}

		wind::g_wind.init(wind::g_config);

		smp->addListener(&wind::g_wind);

		_MESSAGE("Initialisation complete.\n");
	}

	static void skseCallback(SKSEMessagingInterface::Message* msg) {}
};

extern "C" {

	__declspec(dllexport) SKSEPluginVersionData SKSEPlugin_Version = {
		SKSEPluginVersionData::kVersion,
		wind::pluginVersion,
		"SMP Wind",
		"jgernandt",
		"",
		0,
		0,
		{ RUNTIME_VERSION_1_6_640, 0 },
		0,
	};

	__declspec(dllexport) bool SKSEPlugin_Load(const SKSEInterface* skse)
	{
		assert(skse);

		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\SMP Wind.log");
#ifdef _DEBUG
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
#else
		gLog.SetPrintLevel(IDebugLog::kLevel_Message);
		gLog.SetLogLevel(IDebugLog::kLevel_Message);
#endif

		unsigned int runtime = skse->runtimeVersion;

		_MESSAGE("Game version %d.%d.%d",
			GET_EXE_VERSION_MAJOR(runtime),
			GET_EXE_VERSION_MINOR(runtime),
			GET_EXE_VERSION_BUILD(runtime));
		_MESSAGE("SKSE version %d.%d.%d",
			GET_EXE_VERSION_MAJOR(skse->skseVersion),
			GET_EXE_VERSION_MINOR(skse->skseVersion),
			GET_EXE_VERSION_BUILD(skse->skseVersion));
		_MESSAGE("Plugin version %d.%d.%d-640\n", wind::VERSION_MAJOR, wind::VERSION_MINOR, wind::VERSION_PATCH);

		auto spi = static_cast<SKSEPapyrusInterface*>(skse->QueryInterface(kInterface_Papyrus));

		if (!spi) {
			_FATALERROR("Failed to get an SKSE papyrus interface. Plugin loading aborted.");
			return false;
		}

		if (!spi->Register(wind::registerFunctions)) {
			_FATALERROR("Function registration failed. Plugin loading aborted.");
			return false;
		}

		hdt::PluginHelper<MyHelper>::tryConnect(skse);

		return true;
	}
};


