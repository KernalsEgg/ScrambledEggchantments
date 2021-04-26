#include "Fixes/EnchantmentCost.h"
#include "Patches/EnchantmentValue.h"
#include "Patches/MultipleEnchantmentEffects.h"
#include "Serialization.h"



extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_queryInterface, SKSE::PluginInfo* a_pluginInfo)
{
	auto path = SKSE::log::log_directory();

	if (!path)
	{
		return false;
	}

	*path /= "ScrambledEggchantments.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	auto logger = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(logger));
	spdlog::set_pattern("[%H:%M:%S:%e] %g:%# (%^%l%$) %v"s);

	a_pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	a_pluginInfo->name = "ScrambledEggchantments";
	a_pluginInfo->version = 1;

	if (a_queryInterface->IsEditor())
	{
		SKSE::log::critical("Loaded in editor, marking as incompatible"sv);

		return false;
	}

	const auto runtimeVersion = a_queryInterface->RuntimeVersion();

	if (runtimeVersion < SKSE::RUNTIME_1_5_39)
	{
		SKSE::log::critical(FMT_STRING("Unsupported runtime version {}"), runtimeVersion.string());

		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_loadInterface)
{
	SKSE::Init(a_loadInterface);

	if (!Settings::Load())
	{
		return false;
	}

	const auto& settings = Settings::GetSingleton();

	if (settings.fixes.enchantmentCost)
	{
		Fixes::EnchantmentCost::Install();

		auto serializationInferface = SKSE::GetSerializationInterface();
		serializationInferface->SetUniqueID(Serialization::kUniqueID);
		serializationInferface->SetSaveCallback(Serialization::Save);
		serializationInferface->SetLoadCallback(Serialization::Load);
	}

	if (settings.patches.enchantmentValue)
	{
		Patches::EnchantmentValue::Install();
	}

	if (settings.patches.multipleEnchantmentEffects)
	{
		Patches::MultipleEnchantmentEffects::Install();
	}

	return true;
}
