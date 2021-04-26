#pragma once



namespace Fixes
{
	class EnchantmentCost
	{
	public:
		static bool Install();

	private:
		static bool NotEqualTo(RE::Effect* a_left, RE::Effect* a_right); // SkyrimSE.exe + 0xF8290 (VID 10916)
	};
}
