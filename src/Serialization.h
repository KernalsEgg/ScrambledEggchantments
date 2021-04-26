#pragma once



namespace Serialization
{
	enum : std::uint32_t
	{
		kUniqueID = 'ECF',
		kVersion = 1
	};



	class Enchantment
	{
	public:
		enum : std::uint32_t
		{
			kType = 'ENCH'
		};



		static bool Deserialize(SKSE::SerializationInterface* a_serializationInterface);
		static bool Serialize(SKSE::SerializationInterface* a_serializationInterface, RE::EnchantmentItem* a_enchantment);



		RE::FormID			formID; // 0
		RE::MagicItem::Data data;	// 4
	};
	static_assert(sizeof(Serialization::Enchantment) == 0xC);



	void Load(SKSE::SerializationInterface* a_serializationInterface);
	void Save(SKSE::SerializationInterface* a_serializationInterface);
};
