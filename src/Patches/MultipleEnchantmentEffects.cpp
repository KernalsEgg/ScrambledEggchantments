#include "Patches/MultipleEnchantmentEffects.h"



namespace BSTArray
{
	std::uint32_t CopyEffect(RE::BSTArray<RE::Effect>* a_this, RE::Effect* a_source)
	{
		REL::Relocation<decltype(&BSTArray::CopyEffect)> copyEffect{ REL::ID(Offsets::BSTArray::CopyEffect) };

		return copyEffect(a_this, a_source);
	}
}

namespace EnchantmentItem
{
	float ModifyPower(float a_maximumPower, float a_enchanting)
	{
		REL::Relocation<decltype(&EnchantmentItem::ModifyPower)> modifyPower{ REL::ID(Offsets::EnchantmentItem::ModifyPower) };

		return modifyPower(a_maximumPower, a_enchanting);
	}
}

namespace Patches
{
	bool MultipleEnchantmentEffects::Install()
	{
		REL::Relocation<std::uintptr_t> virtualFunctionTable{ Offsets::CraftingSubMenus::EnchantConstructMenu::CreateEffectFunctor::VirtualFunctionTable };
		virtualFunctionTable.write_vfunc(0x1, MultipleEnchantmentEffects::TraverseEffect);

		return true;
	}

	std::uint32_t MultipleEnchantmentEffects::TraverseEffect(RE::CraftingSubMenus::EnchantConstructMenu::CreateEffectFunctor* a_this, RE::Effect* a_source)
	{
		auto index = BSTArray::CopyEffect(&a_this->effects, a_source);

		auto magnitude = a_source->GetMagnitude();
		auto duration = a_source->GetDuration();

		auto powerAffectsMagnitude = a_source->baseEffect->data.flags & RE::EffectSetting::EffectSettingData::Flag::kPowerAffectsMagnitude;
		auto powerAffectsDuration = a_source->baseEffect->data.flags & RE::EffectSetting::EffectSettingData::Flag::kPowerAffectsDuration;

		auto power = 1.0F;
		auto enchantmentEntry = a_this->enchantmentEntry;

		if (enchantmentEntry)
		{
			auto maximumPower = 1.0F;

			if (powerAffectsMagnitude)
			{
				maximumPower = magnitude;
			}
			else if (powerAffectsDuration)
			{
				maximumPower = static_cast<decltype(maximumPower)>(duration);
			}

			auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			auto enchanting = playerCharacter->GetActorValue(RE::ActorValue::kEnchanting);

			maximumPower = EnchantmentItem::ModifyPower(maximumPower, enchanting);
			RE::ApplyPerkEntries(RE::BGSEntryPoint::ENTRY_POINT::kModEnchantmentPower, playerCharacter, enchantmentEntry->enchantment, a_this->item, &maximumPower);
			maximumPower = std::floor(maximumPower);

			if (a_source == a_this->costliestEffect)
			{
				power = enchantmentEntry->power;

				if (power > maximumPower)
				{
					power = maximumPower;
				}
				else if (power < 1.0F)
				{
					power = 1.0F;
				}

				if (maximumPower != enchantmentEntry->maximumPower)
				{
					power = maximumPower;
				}

				enchantmentEntry->maximumPower = maximumPower;

				if (enchantmentEntry->filterFlag == RE::CraftingSubMenus::EnchantConstructMenu::EnchantmentEntry::FilterFlag::kEffectWeapon)
				{
					enchantmentEntry->power = power;
				}
				else if (enchantmentEntry->filterFlag == RE::CraftingSubMenus::EnchantConstructMenu::EnchantmentEntry::FilterFlag::kEffectArmor)
				{
					power = maximumPower * a_this->powerMultiplier;

					if (power < 1.0F)
					{
						power = 1.0F;
					}

					enchantmentEntry->power = power;
				}
			}
			else
			{
				power = maximumPower;

				if (enchantmentEntry->filterFlag == RE::CraftingSubMenus::EnchantConstructMenu::EnchantmentEntry::FilterFlag::kEffectArmor)
				{
					power = maximumPower * a_this->powerMultiplier;

					if (power < 1.0F)
					{
						power = 1.0F;
					}
				}
			}
		}

		if (powerAffectsMagnitude)
		{
			magnitude = power;
		}
		else if (powerAffectsDuration)
		{
			duration = static_cast<decltype(duration)>(std::round(power));
		}

		(&a_this->effects[index])->SetMagnitude(magnitude);
		(&a_this->effects[index])->SetDuration(duration);

		return 1;
	}
}
