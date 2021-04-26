#pragma once



namespace Patches
{
	class MultipleEnchantmentEffects
	{
	public:
		static bool Install();

	private:
		static std::uint32_t TraverseEffect(RE::CraftingSubMenus::EnchantConstructMenu::CreateEffectFunctor* a_this, RE::Effect* a_source); // SkyrimSE.exe + 0x868A00 (VID 50366)
	};
}
