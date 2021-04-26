#pragma once



class Settings
{
public:
	struct Fixes
	{
		bool enchantmentCost{ false };
	};

	struct Patches
	{
		bool enchantmentValue{ false };
		bool multipleEnchantmentEffects{ false };
	};



	static Settings& GetSingleton();
	static bool		 Load();



	Settings::Fixes	  fixes;
	Settings::Patches patches;
};
