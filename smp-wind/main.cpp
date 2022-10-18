#include "pch.h"

#include "Config.h"
#include "papyrus.h"
#include "Wind.h"

namespace wind
{
	constexpr unsigned long VERSION_MAJOR{ 1 };
	constexpr unsigned long VERSION_MINOR{ 1 };
	constexpr unsigned long VERSION_PATCH{ 0 };
	constexpr unsigned long pluginVersion = (VERSION_MAJOR & 0xFF) << 24 | (VERSION_MINOR & 0xFF) << 16 | (VERSION_PATCH & 0xFF) << 8;

	PluginHandle g_pluginHandle;
	SKSEMessagingInterface* g_skseMessagingInterface;

	Config g_config;
	Config g_configDefault;
	Wind g_wind;

	void smpCallback(SKSEMessagingInterface::Message* msg)
	{
		if (msg && msg->type == hdt::PluginInterface::MSG_STARTUP && msg->data) {

			auto smp = reinterpret_cast<hdt::PluginInterface*>(msg->data);

			auto&& info = smp->getVersionInfo();
			if (info.interfaceVersion.major != hdt::PluginInterface::INTERFACE_VERSION.major ||
				info.interfaceVersion.minor < hdt::PluginInterface::INTERFACE_VERSION.minor)
			{
				_ERROR("Incompatible HDT-SMP interface.");
				return;
			}

			if (info.bulletVersion.major != hdt::PluginInterface::BULLET_VERSION.major ||
				info.bulletVersion.minor < hdt::PluginInterface::BULLET_VERSION.minor)
			{
				_ERROR("Incompatible Bullet version.");
				return;
			}

			_MESSAGE("Connection established.\n");

			//Load config file
			PWSTR wpath;
			HRESULT res = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &wpath);
			if (SUCCEEDED(res)) {
				std::filesystem::path documents(wpath, std::filesystem::path::native_format);
				std::filesystem::path configPath(documents / "My Games\\Skyrim Special Edition\\SKSE\\SMP Wind.ini");

				_MESSAGE("Loading config file %s...", configPath.string().c_str());

				if (g_config.load(configPath)) {
					_MESSAGE("Config file loaded.\n");
				}
				else {
					_WARNING("Failed to load config file. Settings will not be saved.\n");
				}
			}
			else {
				_WARNING("Documents folder not found. Settings will not be saved.\n");
			}

			g_wind.init(g_config);

			smp->addListener(&g_wind);

			_MESSAGE("Initialisation complete.\n");
		}
	}

	void skseCallback(SKSEMessagingInterface::Message* msg)
	{
		if (msg && msg->type == SKSEMessagingInterface::kMessage_PostLoad) {
			if (g_skseMessagingInterface->RegisterListener(g_pluginHandle, "hdtSMP64", &smpCallback)) {
				_MESSAGE("Connecting to HDT-SMP...");
			}
			else {
				_ERROR("HDT-SMP is not loaded.");
			}
		}
	}
}

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

		wind::g_pluginHandle = skse->GetPluginHandle();

		wind::g_skseMessagingInterface = static_cast<SKSEMessagingInterface*>(skse->QueryInterface(kInterface_Messaging));
		auto spi = static_cast<SKSEPapyrusInterface*>(skse->QueryInterface(kInterface_Papyrus));

		if (!wind::g_skseMessagingInterface) {
			_FATALERROR("Failed to get an SKSE messaging interface. Plugin loading aborted.");
			return false;
		}
		if (!spi) {
			_FATALERROR("Failed to get an SKSE papyrus interface. Plugin loading aborted.");
			return false;
		}

		if (!spi->Register(wind::registerFunctions)) {
			_FATALERROR("Function registration failed. Plugin loading aborted.");
			return false;
		}

		wind::g_skseMessagingInterface->RegisterListener(wind::g_pluginHandle, "SKSE", &wind::skseCallback);

		return true;
	}
};


