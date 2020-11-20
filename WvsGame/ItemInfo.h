#pragma once
#include "StateChangeItem.h"
#include "EquipItem.h"
#include "BundleItem.h"
#include "UpgradeItem.h"
#include "PortalScrollItem.h"
#include "PetFoodItem.h"
#include "MobSummonItem.h"
#include "TamingMobFoodItem.h"
#include "BridleItem.h"
#include "SkillLearnItem.h"
#include "PortableChairItem.h"
#include "CashItem.h"
#include "PetSkillChangeItem.h"
#include "StateChangingWeatherItem.h"

#include <vector>

class User;
struct GA_Character;
struct GW_ItemSlotBase;

class ItemInfo
{
private:
	bool m_bInitialized = false;

public:
	enum ItemVariationOption
	{
		ITEMVARIATION_NONE, 
		ITEMVARIATION_BETTER,
		ITEMVARIATION_NORMAL,
		ITEMVARIATION_GREAT,
		ITEMVARIATION_GACHAPON
	};

	enum CashItemType
	{
		CashItemType_None = 0x00,
		CashItemType_HairStyleCoupon,
		CashItemType_FaceStyleCoupon,
		CashItemType_SkinCareCoupon,
		CashItemType_PersonalStore,
		CashItemType_WaterOfLife,
		CashItemType_Emotion,
		CashItemType_Charm,
		CashItemType_Effect = 0x09, //Skip 0x08 for 500xxxx ?
		CashItemType_ThrowingStar,
		CashItemType_EntrustedShop,
		CashItemType_SpeakerChannel,
		CashItemType_SpeakerHeart,
		CashItemType_SpeakerSkull,
		CashItemType_SpeakerWorld,
		CashItemType_Weather,
		CashItemType_SetPetName,
		CashItemType_MessageBox,
		CashItemType_MoneyPocket, 
		CashItemType_JukeBox,
		CashItemType_SendMemo,
		CashItemType_MapTransfer,
		CashItemType_StatChange,
		CashItemType_SkillChange,
		CashItemType_SetItemName,
		CashItemType_ItemProtector,
		CashItemType_Incubator,
		CashItemType_PetSkill, 
		CashItemType_ShopScanner,
		CashItemType_ADBoard, 
		CashItemType_GachaponTicket,
		CashItemType_PetFood,
		CashItemType_Morph, 
		CashItemType_WeddingTicket,
		CashItemType_DeliveryTicket,
		CashItemType_EvolutionRock,
		CashItemType_AvatarMegaphone,
		CashItemType_MapleTV,
		CashItemType_MapleSoleTV,
		CashItemType_MapleLoveTV,
		CashItemType_MegaTV,
		CashItemType_MegaSoleTV,
		CashItemType_MegaLoveTV,
		CashItemType_NameChange,
		CashItemType_MembershipCoupon,
		CashItemType_TransferWorld,
	};

	ItemInfo();
	~ItemInfo();

	static ItemInfo* GetInstance();
	void Initialize();
	void LoadItemSellPriceByLv();
	void IterateMapString(void *dataNode);
	void IterateItemString(void *dataNode);
	void IterateEquipItem(void *dataNode);
	void IterateBundleItem();
	void IteratePetItem();
	void IterateCashItem();
	void RegisterSpecificItems();
	void RegisterNoRollbackItem();
	void RegisterSetHalloweenItem();

	void RegisterEquipItemInfo(EquipItem* pEqpItem, int nItemID, void* pProp);
	void RegisterUpgradeItem(int nItemID, void *pProp);
	void RegisterPortalScrollItem(int nItemID, void *pProp);
	void RegisterMobSummonItem(int nItemID, void *pProp);
	void RegisterPetFoodItem(int nItemID, void *pProp);
	void RegisterTamingMobFoodItem(int nItemID, void *pProp);
	void RegisterBridleItem(int nItemID, void *pProp);
	void RegisterPortableChairItem(int nItemID, void *pProp);
	void RegisterSkillLearnItem(int nItemID, void *pProp);
	void RegisterStateChangeItem(int nItemID, void *pProp);
	void RegisterPetSkillChangeItem(int nItemID, void *pProp);
	void RegisterStateChangingWeatherItem(int nItemID, void *pProp);
	void LoadPetSkillChangeInfo(void *pImg, void *pFlag);

	EquipItem* GetEquipItem(int nItemID);
	BundleItem* GetBundleItem(int nItemID);
	UpgradeItem* GetUpgradeItem(int nItemID);
	PortalScrollItem* GetPortalScrollItem(int nItemID);

	MobSummonItem* GetMobSummonItem(int nItemID);
	PetFoodItem* GetPetFoodItem(int nItemID);
	TamingMobFoodItem* GetTamingMobFoodItem(int nItemID);
	BridleItem* GetBridleItem(int nItemID);
	SkillLearnItem* GetSkillLearnItem(int nItemID);
	PortableChairItem* GetPortableChairItem(int nItemID);
	StateChangeItem* GetStateChangeItem(int nItemID);
	CashItem* GetCashItem(int nItemID);
	PetSkillChangeItem* GetPetSkillChangeItem(int nItemID);
	StateChangingWeatherItem* GetStateChangingWeatherItem(int nItemID);

	static int GetItemSlotType(int nItemID);
	static bool IsTreatSingly(int nItemID, long long int liExpireDate);
	static bool IsRechargable(int nItemID);
	int ConsumeOnPickup(int nItemID);
	bool ExpireOnLogout(int nItemID);
	int GetBulletPAD(int nItemID);
	static long long int GetItemDateExpire(const std::string& sDate);
	const std::string& GetItemString(int nItemID, const std::string& sKey);
	const std::string& GetItemName(int nItemID);
	bool IsAbleToEquip(int nGender, int nLevel, int nJob, int nSTR, int nDEX, int nINT, int nLUK, int nPOP, GW_ItemSlotBase* pPetItem, int nItemID);
	bool IsNotSaleItem(int nItemID);
	bool IsOnlyItem(int nItemID);
	bool IsTradeBlockItem(int nItemID);
	bool IsQuestItem(int nItemID);
	static bool IsWeapon(int nItemID);
	static bool IsCoat(int nItemID);
	static bool IsCape(int nItemID);
	static bool IsPants(int nItemID);
	static bool IsHair(int nItemID);
	static bool IsFace(int nItemID);
	static bool IsShield(int nItemID);
	static bool IsShoes(int nItemID);
	static bool IsLongcoat(int nItemID);
	static bool IsCap(int nItemID);
	static bool IsPet(int nItemID);
	static int GetCashSlotItemType(int nItemID);
	static int GetConsumeCashItemType(int nItemID);
#ifdef _WVSGAME
	static int GetWeaponMastery(GA_Character* pCharacter, int nWeaponID, int nSkillID, int nAttackType, int *pnACCInc, int *pnPADInc);
	static int GetWeaponType(int nItemID);
	static int GetCriticalSkillLevel(GA_Character* pCharacter, int nWeaponID, int nAttackType, int *pnProp, int *pnParam);
#endif
	GW_ItemSlotBase* GetItemSlot(int nItemID, ItemVariationOption enOption);

private:
	std::map<int, EquipItem*> m_mEquipItem;
	std::map<int, BundleItem*> m_mBundleItem;
	std::map<int, UpgradeItem*> m_mUpgradeItem;
	std::map<int, StateChangeItem*> m_mStateChangeItem;
	std::map<int, PortalScrollItem*> m_mPortalScrollItem;
	std::map<int, MobSummonItem*> m_mMobSummonItem;
	std::map<int, PetFoodItem*> m_mPetFoodItem;
	std::map<int, TamingMobFoodItem*> m_mTamingMobFoodItem;
	std::map<int, BridleItem*> m_mBridleItem;
	std::map<int, SkillLearnItem*> m_mSkillLearnItem;
	std::map<int, PortableChairItem*> m_mPortableChairItem;
	std::map<int, CashItem*> m_mCashItem;
	std::map<int, PetSkillChangeItem*> m_mPetSkillChangeItem;
	std::map<int, StateChangingWeatherItem*> m_mStateChangingWeatherItem;

	std::map<int, std::map<std::string, std::string>> m_mItemString, m_mMapString;
	std::map<int, int> m_mItemSellPriceByLv;
	
	void LoadIncrementStat(BasicIncrementStat& refStat, void *pProp);
	void LoadAbilityStat(BasicAbilityStat& refStat, void *pProp);
	int GetVariation(int v, ItemVariationOption enOption);
};

