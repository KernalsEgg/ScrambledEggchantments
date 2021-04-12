namespace Offset
{
	namespace PersistentFormManager
	{
		inline constexpr REL::ID CreateArmorEnchantment(static_cast<std::uint64_t>(35264));	 // SkyrimSE.exe + 0x59F190
		inline constexpr REL::ID CreateWeaponEnchantment(static_cast<std::uint64_t>(35263)); // SkyrimSE.exe + 0x59F0F0
		inline constexpr REL::ID FindEnchantment(static_cast<std::uint64_t>(35284));		 // SkyrimSE.exe + 0x5A1070
		inline constexpr REL::ID Singleton(static_cast<std::uint64_t>(514172));				 // SkyrimSE.exe + 0x1EBEAE8
	}
}

namespace Serialization
{
	enum : std::uint32_t
	{
		kUniqueID = 'ECF',
		kVersion = 1
	};

	enum Type : std::uint32_t
	{
		kEnchantment = 'ENCH'
	};



	struct Enchantment
	{
		RE::FormID			formID; // 0
		RE::MagicItem::Data data;	// 4
	};
	static_assert(sizeof(Serialization::Enchantment) == 0xC);



	bool Deserialize(SKSE::SerializationInterface* a_serializationInterface)
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

	bool Serialize(SKSE::SerializationInterface* a_serializationInterface, RE::EnchantmentItem* a_enchantment)
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
		a_serializationInterface->WriteRecord(Serialization::Type::kEnchantment, Serialization::kVersion, record);

		return true;
	}
};



class FindEnchantment
{
public:
	static void Hook()
	{
		SKSE::GetTrampoline().write_call<5>(Offset::PersistentFormManager::FindEnchantment.address() + 0x72, NotEqualTo);
	}

private:
	static bool NotEqualTo(RE::Effect* a_left, RE::Effect* a_right) // SkyrimSE.exe + 0xF8290 (VID 10916)
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
};

class PersistentFormManager
{
public:
	struct Enchantment
	{
		RE::EnchantmentItem*   enchantment; // 0
		volatile std::uint32_t refCount;	// 8
		std::uint32_t		   padC;		// C
	};
	static_assert(sizeof(PersistentFormManager::Enchantment) == 0x10);

	struct Unknown
	{
		void*		  unknown0;	 // 0
		std::uint64_t unknown8;	 // 8
		void*		  unknown10; // 10
		std::uint64_t unknown18; // 18
		std::uint32_t unknown20; // 20
		std::uint32_t unknown24; // 24
		std::uint32_t unknown28; // 28
		std::uint32_t unknown2C; // 2C
	};
	static_assert(sizeof(PersistentFormManager::Unknown) == 0x30);



	static PersistentFormManager* GetSingleton()
	{
		REL::Relocation<PersistentFormManager**> singleton{ Offset::PersistentFormManager::Singleton };
		return *singleton;
	}

	RE::EnchantmentItem* CreateArmorEnchantment(RE::BSTArray<RE::Effect>* a_effects)
	{
		REL::Relocation<decltype(&PersistentFormManager::CreateArmorEnchantment)> createArmorEnchantment{ Offset::PersistentFormManager::CreateArmorEnchantment };
		return createArmorEnchantment(this, a_effects);
	}

	RE::EnchantmentItem* CreateWeaponEnchantment(RE::BSTArray<RE::Effect>* a_effects)
	{
		REL::Relocation<decltype(&PersistentFormManager::CreateWeaponEnchantment)> createWeaponEnchantment{ Offset::PersistentFormManager::CreateWeaponEnchantment };
		return createWeaponEnchantment(this, a_effects);
	}



	std::uint64_t									 unknown0;			 // 0
	RE::BSTArray<PersistentFormManager::Enchantment> weaponEnchantments; // 8
	RE::BSTArray<PersistentFormManager::Enchantment> armorEnchantments;	 // 20
	std::uint64_t									 unknown38;			 // 38
	std::uint32_t									 unknown40;			 // 40
	std::uint32_t									 unknown44;			 // 44
	std::uint32_t									 unknown48;			 // 48
	std::uint32_t									 unknown4C;			 // 4C
	PersistentFormManager::Unknown					 unknown50;			 // 50
	PersistentFormManager::Unknown					 unknown80;			 // 80
	void*											 unknownB0;			 // B0
	std::uint64_t									 unknownB8;			 // B8
	std::uint64_t									 unknownC0;			 // C0
	mutable RE::BSSpinLock							 spinLock;			 // C8
};
static_assert(sizeof(PersistentFormManager) == 0xD0);



void Load(SKSE::SerializationInterface* a_serializationInterface)
{
	SKSE::log::info("Load"sv);

	auto persistentFormManager = PersistentFormManager::GetSingleton();

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

			break;
		}

		switch (type)
		{
			case Serialization::Type::kEnchantment:
				{
					if (length != sizeof(Serialization::Enchantment))
					{
						SKSE::log::error("Length mismatch."sv);

						break;
					}

					Serialization::Deserialize(a_serializationInterface);

					break;
				}
			default:
				{
					SKSE::log::error("Type mismatch."sv);

					break;
				}
		}
	}

	spinLock.Unlock();
}

void Save(SKSE::SerializationInterface* a_serializationInterface)
{
	SKSE::log::info("Save"sv);

	auto persistentFormManager = PersistentFormManager::GetSingleton();

	auto& spinLock = persistentFormManager->spinLock;
	spinLock.Lock();

	const auto& armorEnchantments = persistentFormManager->armorEnchantments;

	for (const auto& armorEnchantment : armorEnchantments)
	{
		Serialization::Serialize(a_serializationInterface, armorEnchantment.enchantment);
	}

	const auto& weaponEnchantments = persistentFormManager->weaponEnchantments;

	for (const auto& weaponEnchantment : weaponEnchantments)
	{
		Serialization::Serialize(a_serializationInterface, weaponEnchantment.enchantment);
	}

	spinLock.Unlock();
}



extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_queryInterface, SKSE::PluginInfo* a_pluginInfo)
{
	auto path = SKSE::log::log_directory();

	if (!path)
	{
		return false;
	}

	*path /= "EnchantmentChargeFix.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	auto logger = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(logger));
	spdlog::set_pattern("[%H:%M:%S:%e] %g:%# (%^%l%$) %v"s);

	a_pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	a_pluginInfo->name = "EnchantmentChargeFix";
	a_pluginInfo->version = 1;

	if (a_queryInterface->IsEditor())
	{
		SKSE::log::critical("Loaded in editor, marking as incompatible"sv);

		return false;
	}

	const auto runtimeVersion = a_queryInterface->RuntimeVersion();

	if (runtimeVersion < SKSE::RUNTIME_1_5_39)
	{
		SKSE::log::critical(FMT_STRING("Unsupported runtime version {}"), runtimeVersion.string());

		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_queryInterface)
{
	SKSE::Init(a_queryInterface);

	SKSE::AllocTrampoline(1 << 4); // 0x10
	FindEnchantment::Hook();

	auto serializationInferface = SKSE::GetSerializationInterface();
	serializationInferface->SetUniqueID(Serialization::kUniqueID);
	serializationInferface->SetSaveCallback(Save);
	serializationInferface->SetLoadCallback(Load);

	return true;
}
