#include "Fixes/EnchantmentCost.h"



namespace Fixes
{
	bool EnchantmentCost::Install()
	{
		const auto		   address = Offsets::PersistentFormManager::FindEnchantment.address();
		const std::uint8_t expected[] = { 0xE8 };

		if (std::memcmp(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(address + 0x72)), expected, sizeof(expected)))
		{
			SKSE::log::error("Expected bytes not found."sv);

			return false;
		}

		auto& trampoline = SKSE::GetTrampoline();

		SKSE::AllocTrampoline(1 << 4); // 0x10
		trampoline.write_call<5>(address + 0x72, EnchantmentCost::NotEqualTo);

		return true;
	}

	bool EnchantmentCost::NotEqualTo(RE::Effect* a_left, RE::Effect* a_right)
	{
		auto leftBaseEffect = a_left->baseEffect;
		auto rightBaseEffect = a_right->baseEffect;

		if (leftBaseEffect != rightBaseEffect)
		{
			return true;
		}

		auto leftFlags = leftBaseEffect->data.flags;
		auto rightFlags = rightBaseEffect->data.flags;

		auto& leftEffectItem = a_left->effectItem;
		auto& rightEffectItem = a_right->effectItem;

		auto leftMagnitude = leftFlags & RE::EffectSetting::EffectSettingData::Flag::kNoMagnitude ? 0.0F : leftEffectItem.magnitude;
		auto rightMagnitude = rightFlags & RE::EffectSetting::EffectSettingData::Flag::kNoMagnitude ? 0.0F : rightEffectItem.magnitude;

		if (leftMagnitude != rightMagnitude)
		{
			return true;
		}

		auto leftArea = leftFlags & RE::EffectSetting::EffectSettingData::Flag::kNoArea ? 0.0F : leftEffectItem.area;
		auto rightArea = rightFlags & RE::EffectSetting::EffectSettingData::Flag::kNoArea ? 0.0F : rightEffectItem.area;

		if (leftArea != rightArea)
		{
			return true;
		}

		auto leftDuration = leftFlags & RE::EffectSetting::EffectSettingData::Flag::kNoDuration ? 0.0F : leftEffectItem.duration;
		auto rightDuration = rightFlags & RE::EffectSetting::EffectSettingData::Flag::kNoDuration ? 0.0F : rightEffectItem.duration;

		if (leftDuration != rightDuration)
		{
			return true;
		}

		return false;
	}
}
