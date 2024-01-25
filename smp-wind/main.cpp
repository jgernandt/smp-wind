#include "pch.h"

#include "PluginHelper.h"

#include "Config.h"
#include "Wind.h"

constexpr unsigned long VERSION_MAJOR{ 2 };
constexpr unsigned long VERSION_MINOR{ 2 };
constexpr unsigned long VERSION_PATCH{ 0 };
constexpr unsigned long VERSION = (VERSION_MAJOR & 0xFF) << 24 | (VERSION_MINOR & 0xFF) << 16 | (VERSION_PATCH & 0xFF) << 8;

class VMClassRegistry;

namespace wind
{
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

			_MESSAGE("Loading settings...");
			if (wind::g_config.load(documents / "My Games\\" SAVE_FOLDER_NAME "\\SKSE\\SMP Wind.ini")) {
				_MESSAGE("Settings loaded.\n");
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
		VERSION,
		"SMP Wind",
		"jgernandt",
		"",
#if CURRENT_RELEASE_RUNTIME > RUNTIME_VERSION_1_6_353
		0,
#endif
		0,
		{ CURRENT_RELEASE_RUNTIME, 0 },
		0,
	};

	__declspec(dllexport) bool SKSEPlugin_Load(const SKSEInterface* skse)
	{
		assert(skse);

		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\" SAVE_FOLDER_NAME "\\SKSE\\SMP Wind.log");
#ifdef _DEBUG
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
#else
		gLog.SetPrintLevel(IDebugLog::kLevel_Message);
		gLog.SetLogLevel(IDebugLog::kLevel_Message);
#endif

		unsigned int runtime = skse->runtimeVersion;

		_MESSAGE("Skyrim version:\t%d.%d.%d",
			GET_EXE_VERSION_MAJOR(runtime),
			GET_EXE_VERSION_MINOR(runtime),
			GET_EXE_VERSION_BUILD(runtime));
		_MESSAGE("SKSE version:\t%d.%d.%d",
			GET_EXE_VERSION_MAJOR(skse->skseVersion),
			GET_EXE_VERSION_MINOR(skse->skseVersion),
			GET_EXE_VERSION_BUILD(skse->skseVersion));
		_MESSAGE("Plugin version:\t%d.%d.%d-%x\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, CURRENT_RELEASE_RUNTIME);

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

	__declspec(dllexport) bool SKSEPlugin_Query(const SKSEInterface*, PluginInfo* info)
	{
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = SKSEPlugin_Version.name;
		info->version = SKSEPlugin_Version.pluginVersion;

		return false;
	}
};
