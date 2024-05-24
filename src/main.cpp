#include "Events.h"
#include "Utils.h"

CSimpleIniA ini(true, false, false);
bool filthyCasualMode;
RE::PlayerCharacter* player;

void InitPlugin()
{
	Events::TESDeathEventWatcher* deathEvent = new Events::TESDeathEventWatcher();
	RE::DeathEventSource::GetSingleton()->RegisterSink(deathEvent);
}

void LoadConfigs()
{
	ini.LoadFile("Data\\F4SE\\Plugins\\GLXRM_Permadeath.ini");
	filthyCasualMode = std::stoi(ini.GetValue("General", "im_a_filthy_casual", "1"));
	ini.Reset();
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_f4se, F4SE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("--> %v"s);

	logger::info("{} v{}", Version::PROJECT, Version::NAME);
	logger::warn("---------------------------------------------------------------------");

	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = "GLXRM_Permadeath";
	a_info->version = Version::MAJOR;

	if (a_f4se->IsEditor()) {
		logger::critical("loaded in editor");
		return false;
	}

	const auto ver = a_f4se->RuntimeVersion();
	if (ver < F4SE::RUNTIME_1_10_162) {
		logger::critical("unsupported runtime v{}", ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se);

	LoadConfigs();

	const F4SE::MessagingInterface* messageInterface = F4SE::GetMessagingInterface();
	messageInterface->RegisterListener([](F4SE::MessagingInterface::Message* msg) -> void {
		if (msg->type == F4SE::MessagingInterface::kGameDataReady) {
			InitPlugin();
		} else if (msg->type == F4SE::MessagingInterface::kNewGame || msg->type == F4SE::MessagingInterface::kPostLoadGame) {
			player = RE::PlayerCharacter::GetSingleton();
		}
	});

	return true;
}




