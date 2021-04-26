#include "Serialization.h"



namespace Serialization
{
	bool Enchantment::Deserialize(SKSE::SerializationInterface* a_serializationInterface)
	{
		Serialization::Enchantment record;
		a_serializationInterface->ReadRecordData(record);

		if (!a_serializationInterface->ResolveFormID(record.formID, record.formID))
		{
			SKSE::log::error("Form ID not found."sv);

			return false;
		}

		auto enchantment = RE::TESForm::LookupByID<RE::EnchantmentItem>(record.formID);

		if (!enchantment)
		{
			SKSE::log::error("Enchantment not found."sv);

			return false;
		}

		auto data = enchantment->GetData2();

		if (!data)
		{
			SKSE::log::error("Data not found."sv);

			return false;
		}

		data->costOverride = record.data.costOverride;
		data->flags = record.data.flags;

		return true;
	}

	bool Enchantment::Serialize(SKSE::SerializationInterface* a_serializationInterface, RE::EnchantmentItem* a_enchantment)
	{
		if (!a_enchantment)
		{
			SKSE::log::error("Enchantment not found."sv);

			return false;
		}

		auto data = a_enchantment->GetData1();

		if (!data)
		{
			SKSE::log::error("Data not found."sv);

			return false;
		}

		Serialization::Enchantment record{ a_enchantment->formID, data->costOverride, data->flags };
		a_serializationInterface->WriteRecord(Serialization::Enchantment::kType, Serialization::kVersion, record);

		return true;
	}

	void Load(SKSE::SerializationInterface* a_serializationInterface)
	{
		SKSE::log::info("Load"sv);

		auto persistentFormManager = RE::PersistentFormManager::GetSingleton();

		auto& spinLock = persistentFormManager->spinLock;
		spinLock.Lock();

		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;

		while (a_serializationInterface->GetNextRecordInfo(type, version, length))
		{
			if (version != Serialization::kVersion)
			{
				SKSE::log::error("Version mismatch."sv);

				continue;
			}

			if (type != Serialization::Enchantment::kType)
			{
				SKSE::log::error("Type mismatch."sv);

				continue;
			}

			if (length != sizeof(Serialization::Enchantment))
			{
				SKSE::log::error("Length mismatch."sv);

				continue;
			}

			Serialization::Enchantment::Deserialize(a_serializationInterface);
		}

		spinLock.Unlock();
	}

	void Save(SKSE::SerializationInterface* a_serializationInterface)
	{
		SKSE::log::info("Save"sv);

		auto persistentFormManager = RE::PersistentFormManager::GetSingleton();

		auto& spinLock = persistentFormManager->spinLock;
		spinLock.Lock();

		const auto& armorEnchantments = persistentFormManager->armorEnchantments;

		for (const auto& armorEnchantment : armorEnchantments)
		{
			Serialization::Enchantment::Serialize(a_serializationInterface, armorEnchantment.enchantment);
		}

		const auto& weaponEnchantments = persistentFormManager->weaponEnchantments;

		for (const auto& weaponEnchantment : weaponEnchantments)
		{
			Serialization::Enchantment::Serialize(a_serializationInterface, weaponEnchantment.enchantment);
		}

		spinLock.Unlock();
	}
}
