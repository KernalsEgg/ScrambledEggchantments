#pragma once

#pragma warning(disable : 4100)



namespace Patches
{
	class EnchantmentValue
	{
	public:
		static bool Install();

	private:
		static float CalculateEnchantmentCost(RE::EnchantmentItem* a_enchantment, RE::Character* a_caster);
	};
}
