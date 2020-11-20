#pragma once
#include <vector>
#include "InventoryManipulator.h"
#include "ExchangeElement.h"

class User;
class InPacket;
struct GW_ItemSlotBase;

class QWUInventory
{
public:
	QWUInventory();
	~QWUInventory();
	static bool ChangeSlotPosition(User* pUser, int bOnExclRequest, int nTI, int nPOS1, int nPOS2, int nCount, int tRequestTime);
	static void OnChangeSlotPositionRequest(User* pUser, InPacket* iPacket);
	static bool PickUpMoney(User* pUser, bool byPet, int nAmount);
	static bool PickUpItem(User* pUser, bool byPet, ZSharedPtr<GW_ItemSlotBase>& pItem);
	static bool RawRemoveItemByID(User* pUser, int nItemID, int nCount);
	static bool RawRemoveItem(User* pUser, int nTI, int nPOS, int nCount, std::vector<InventoryManipulator::ChangeLog>* aChangeLog, int &nDecRet, ZSharedPtr<GW_ItemSlotBase>* ppItemRemoved);
	static int RemoveItem(User* pUser, ZSharedPtr<GW_ItemSlotBase>& pItem, int nCount = 1, bool bSendInventoryOperation = true, bool bOnExclResult = false, ZSharedPtr<GW_ItemSlotBase>* ppItemRemoved = nullptr);
	static bool RawRechargeItem(User *pUser, int nPOS, std::vector<InventoryManipulator::ChangeLog>* aChangeLog);
	static int Exchange(User* pUser, int nMoney, std::vector<ExchangeElement>& aExchange);
	static int Exchange(User* pUser, int nMoney, std::vector<ExchangeElement>& aExchange, std::vector<InventoryManipulator::ChangeLog>* aLogAdd, std::vector<InventoryManipulator::ChangeLog>* aLogRemove, std::vector<BackupItem>& aBackupItem, bool bSendOperation = true);
	static void SendInventoryOperation(User* pUser, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog>& aChangeLog);
	static void OnUpgradeItemRequest(User* pUser, InPacket *iPacket);
	static void UpgradeEquip(User* pUser, int nUPOS, int nEPOS, int nWhiteScroll, bool bEnchantSkill, int tReqTime);
	static void RestoreFromTemp(User* pUser, std::map<int, int> mItemTrading[6]);
	static void RawMoveItemToTemp(User* pUser, ZSharedPtr<GW_ItemSlotBase>* pItemCopyed, int nTI, int nPOS, int nNumber, std::vector<InventoryManipulator::ChangeLog>& aChangeLog);
	static void MoveItemToTemp(User* pUser, ZSharedPtr<GW_ItemSlotBase>* pItemCopyed, int nTI, int nPOS, int nNumber);
	static bool MoveMoneyToTemp(User *pUser, int nAmount);
	static bool WasteItem(User *pUser, int nItemID, int nCount, bool bProtected);
	static bool RawWasteItem(User *pUser, int nPOS, int nCount, std::vector<InventoryManipulator::ChangeLog>& aChangeLog);
	static void UpdatePetItem(User *pUser, int nPos);

	static int GetSlotCount(User *pUser, int nTI);
	static int GetHoldCount(User *pUser, int nTI);
	static int GetFreeCount(User *pUser, int nTI);
	static int GetItemCount(User *pUser, int nItemID, bool bProtected);
	//static void RestoreMoneyFromTemp(User* pUser, bool bByPet, int nAmount);
};

