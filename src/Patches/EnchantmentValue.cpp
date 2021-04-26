#include "Patches/EnchantmentValue.h"



namespace Patches
{
	bool EnchantmentValue::Install()
	{
		const auto		   address = Offsets::InventoryEntryData::GetValue.address();
		const std::uint8_t expected[] = { 0xE8 };

		// Armor
		if (std::memcmp(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(address + 0xEE)), expected, sizeof(expected)))
		{
			SKSE::log::error("Expected bytes not found."sv);

			return false;
		}

		// Weapon
		if (std::memcmp(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(address + 0x138)), expected, sizeof(expected)))
		{
			SKSE::log::error("Expected bytes not found."sv);

			return false;
		}

		auto& trampoline = SKSE::GetTrampoline();

		// Armor
		SKSE::AllocTrampoline(1 << 4); // 0x10
		trampoline.write_call<5>(address + 0xEE, EnchantmentValue::CalculateEnchantmentCost);

		// Weapon
		SKSE::AllocTrampoline(1 << 4); // 0x10
		trampoline.write_call<5>(address + 0x138, EnchantmentValue::CalculateEnchantmentCost);

		return true;
	}

	float EnchantmentValue::CalculateEnchantmentCost(RE::EnchantmentItem* a_enchantment, RE::Character* a_caster)
	{
		auto cost = 0.0F;
		auto effects = a_enchantment->effects;

		if (effects.size() > 0)
		{
			for (const auto& effect : effects)
			{
				if (!(effect->baseEffect->data.flags & RE::EffectSetting::EffectSettingData::Flag::kNoCost))
				{
					if (effect->cost > 0.0F)
					{
						cost += effect->cost;
					}
				}
			}
		}

		return cost;
	}
}
