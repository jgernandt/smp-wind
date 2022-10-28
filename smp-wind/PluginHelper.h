#pragma once
#include "PluginAPI.h"

namespace hdt
{
	constexpr bool operator<(const PluginInterface::Version& lhs, const PluginInterface::Version& rhs)
	{
		return lhs.major < rhs.major || lhs.minor < rhs.minor || lhs.patch < rhs.patch;
	}
	constexpr bool operator>(const PluginInterface::Version& lhs, const PluginInterface::Version& rhs)
	{
		return rhs < lhs;
	}
	constexpr bool operator>=(const PluginInterface::Version& lhs, const PluginInterface::Version& rhs)
	{
		return !(lhs < rhs);
	}
	constexpr bool operator<=(const PluginInterface::Version& lhs, const PluginInterface::Version& rhs)
	{
		return !(rhs < lhs);
	}

	template<typename T>
	class PluginHelper : T
	{
	public:
		static bool tryConnect(const SKSEInterface* skse, bool externalCallback = false)
		{
			assert(skse);

			s_interface = reinterpret_cast<SKSEMessagingInterface*>(skse->QueryInterface(kInterface_Messaging));
			if (s_interface) {
				s_handle = skse->GetPluginHandle();
				if (!externalCallback) {
					s_interface->RegisterListener(s_handle, "SKSE", &skseCallback);
				}
				return true;
			}
			else {
				_ERROR("ERROR: Failed to get an SKSEMessagingInterface.");
				return false;
			}
		}

		static void skseCallback(SKSEMessagingInterface::Message* msg)
		{
			if (msg && msg->type == SKSEMessagingInterface::kMessage_PostLoad) {
				if (s_interface->RegisterListener(s_handle, "hdtSMP64", &smpCallback)) {
					_MESSAGE("Connecting to HDT-SMP...");
				}
				else {
					_ERROR("ERROR: HDT-SMP is not loaded.");
				}
			}
			T::skseCallback(msg);
		}

		static void smpCallback(SKSEMessagingInterface::Message* msg)
		{
			if (msg && msg->type == hdt::PluginInterface::MSG_STARTUP && msg->data) {
				auto smp = reinterpret_cast<hdt::PluginInterface*>(msg->data);

				auto&& info = smp->getVersionInfo();

				if (info.interfaceVersion >= interfaceMin && info.interfaceVersion < interfaceMax) {
					if (info.bulletVersion >= bulletMin && info.bulletVersion < bulletMax) {
						_MESSAGE("Connection established.\n");
						T::onConnect(smp);
					}
					else {
						_ERROR("ERROR: Incompatible Bullet version.");
					}
				}
				else {
					_ERROR("ERROR: Incompatible HDT-SMP interface.");
				}
			}
		}

	private:
		inline static SKSEMessagingInterface* s_interface{};
		inline static PluginHandle s_handle{};
	};
}
