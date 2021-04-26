#include "Settings.h"

#include <fstream>
#include <nlohmann/json.hpp>



using json = nlohmann::json;



Settings& Settings::GetSingleton()
{
	static Settings singleton;

	return singleton;
}

bool Settings::Load()
{
	try
	{
		const auto jsonSettings = json::parse(std::ifstream("Data/SKSE/Plugins/ScrambledEggchantments.json"), nullptr, true, true);
		auto&	   settings = Settings::GetSingleton();

		jsonSettings.at("fixes").at("enchantmentCost").get_to(settings.fixes.enchantmentCost);
		jsonSettings.at("patches").at("enchantmentValue").get_to(settings.patches.enchantmentValue);
		jsonSettings.at("patches").at("multipleEnchantmentEffects").get_to(settings.patches.multipleEnchantmentEffects);
	}
	catch (const json::exception& exception)
	{
		SKSE::log::error("{}", exception.what());

		return false;
	}

	return true;
}
