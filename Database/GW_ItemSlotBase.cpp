#include "GW_ItemSlotBase.h"
#include "WvsUnified.h"
#include "GW_ItemSlotBundle.h"
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotPet.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

std::atomic<GW_ItemSlotBase::ATOMIC_COUNT_TYPE> GW_ItemSlotBase::ms_liSN[6];
int GW_ItemSlotBase::ms_nChannelID;
int GW_ItemSlotBase::ms_nWorldID;

GW_ItemSlotBase::GW_ItemSlotBase()
{
	liItemSN = -1;
}

GW_ItemSlotBase::~GW_ItemSlotBase()
{
}

/*
Load Item SN Atomic Value
*/
GW_ItemSlotBase::ATOMIC_COUNT_TYPE GW_ItemSlotBase::InitItemSN(GW_ItemSlotType type, int nWorldID)
{
	unsigned long long int liHBound = 0;
	((unsigned char*)&liHBound)[7] = (nWorldID) & 0x1F;

	std::string strTableName = "";
	if (type == GW_ItemSlotType::EQUIP)
		strTableName = "ItemSlot_EQP";
	else if (type == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (type == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (type == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (type == GW_ItemSlotType::CASH) 
		strTableName = "CashItem_Bundle";

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	ATOMIC_COUNT_TYPE result = 0;

	if (type == GW_ItemSlotType::CASH)
	{
		queryStatement << "SELECT MAX(CashItemSN) FROM CashItem_Bundle UNION ALL SELECT MAX(CashItemSN) FROM CashItem_Pet UNION ALL SELECT MAX(CashItemSN) FROM CashItem_EQP";
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
			if(!recordSet["MAX(CashItemSN)"].isEmpty())
				result = std::max(result, (ATOMIC_COUNT_TYPE)recordSet["MAX(CashItemSN)"]);
	}
	else
	{
		queryStatement << "SELECT MAX(ItemSN) From " << strTableName << " WHERE ItemSN < " << liHBound; //0~2^40 is reserved for Center.
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		if (recordSet.rowCount() == 0 || recordSet["MAX(ItemSN)"].isEmpty())
			return 2;
		result = (ATOMIC_COUNT_TYPE)recordSet["MAX(ItemSN)"];
	}
	return result > 1 ? result : 2;
}

GW_ItemSlotBase::ATOMIC_COUNT_TYPE GW_ItemSlotBase::GetInitItemSN(GW_ItemSlotType type, int nWorldID, int nChannelID)
{
	unsigned long long int liLBound = 0, liHBound = 0;
	((unsigned char*)&liLBound)[6] = (nChannelID) << 2; //Uses highest 6 bits as Channel flag.
	((unsigned char*)&liLBound)[7] = (nWorldID) & 0x1F; //Uses only 5 bits for World flag.

	((unsigned char*)&liHBound)[6] = (nChannelID + 1) << 2; 
	((unsigned char*)&liHBound)[7] = (nWorldID) & 0x1F;

	std::string strTableName = "";
	if (type == GW_ItemSlotType::EQUIP)
		strTableName = "ItemSlot_EQP";
	else if (type == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (type == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (type == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (type == GW_ItemSlotType::CASH)
		strTableName = "CashItem_Bundle";

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	ATOMIC_COUNT_TYPE result = 0;

	if (type == GW_ItemSlotType::CASH)
	{
		queryStatement << "SELECT MAX(CashItemSN) FROM CashItem_Bundle UNION ALL SELECT MAX(CashItemSN) FROM CashItem_Pet UNION ALL SELECT MAX(CashItemSN) FROM CashItem_EQP";
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
			if (!recordSet["MAX(CashItemSN)"].isEmpty())
				result = std::max(result, (ATOMIC_COUNT_TYPE)recordSet["MAX(CashItemSN)"]);
	}
	else
	{
		queryStatement << "SELECT MAX(ItemSN) From " << strTableName << " WHERE ItemSN >= " << liLBound << " AND ItemSN < " << liHBound;
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		if (recordSet.rowCount() == 0 || recordSet["MAX(ItemSN)"].isEmpty())
			return 2;
		result = (ATOMIC_COUNT_TYPE)recordSet["MAX(ItemSN)"];
	}
	((unsigned char*)&result)[6] = 0;

	return result > 1 ? result : 2;
}

#if defined(DBLIB) || defined(_WVSCENTER)

std::atomic<GW_ItemSlotBase::ATOMIC_COUNT_TYPE> GW_ItemSlotBase::ms_atEqpAtomicCounter;
std::atomic<GW_ItemSlotBase::ATOMIC_COUNT_TYPE> GW_ItemSlotBase::ms_atConAtomicCounter;
std::atomic<GW_ItemSlotBase::ATOMIC_COUNT_TYPE> GW_ItemSlotBase::ms_atInsAtomicCounter;
std::atomic<GW_ItemSlotBase::ATOMIC_COUNT_TYPE> GW_ItemSlotBase::ms_atEtcAtomicCounter;
std::atomic<GW_ItemSlotBase::ATOMIC_COUNT_TYPE> GW_ItemSlotBase::ms_atCashAtomicCounter;

GW_ItemSlotBase::ATOMIC_COUNT_TYPE GW_ItemSlotBase::IncItemSN(GW_ItemSlotType type)
{
	if (type == GW_ItemSlotType::EQUIP)
		return ++ms_atEqpAtomicCounter;
	else if (type == GW_ItemSlotType::CONSUME)
		return ++ms_atConAtomicCounter;
	else if (type == GW_ItemSlotType::INSTALL)
		return ++ms_atInsAtomicCounter;
	else if (type == GW_ItemSlotType::ETC)
		return ++ms_atEtcAtomicCounter;
	else if (type == GW_ItemSlotType::CASH)
		return ++ms_atCashAtomicCounter;
}

void GW_ItemSlotBase::InitItemSN(int nWorldID)
{
	ms_atEqpAtomicCounter = InitItemSN(GW_ItemSlotType::EQUIP, nWorldID);
	ms_atConAtomicCounter = InitItemSN(GW_ItemSlotType::CONSUME, nWorldID);
	ms_atInsAtomicCounter = InitItemSN(GW_ItemSlotType::INSTALL, nWorldID);
	ms_atEtcAtomicCounter = InitItemSN(GW_ItemSlotType::ETC, nWorldID);
	ms_atCashAtomicCounter = InitItemSN(GW_ItemSlotType::CASH, nWorldID);
}

#endif

void GW_ItemSlotBase::SetInitSN(int nTI, ATOMIC_COUNT_TYPE liSN)
{
	ms_liSN[nTI] = liSN;
}

GW_ItemSlotBase::ATOMIC_COUNT_TYPE GW_ItemSlotBase::GetNextSN(int nTI)
{
	ATOMIC_COUNT_TYPE ret = ++ms_liSN[nTI];
	((unsigned char*)&ret)[6] = (ms_nChannelID) << 2;
	((unsigned char*)&ret)[7] = (ms_nWorldID) & 0x1F;

	return ret;
}

bool GW_ItemSlotBase::IsProtectedItem() const
{
	return nAttribute & ItemAttribute::eItemAttr_Protected;
}

/*
Encode Item Type, Where Equip = 1, Stackable = 2, Pet = 3
*/
void GW_ItemSlotBase::Encode(OutPacket *oPacket, bool bForInternal) const
{
	oPacket->Encode1(nInstanceType); //Pet = 3
}

/*
Encode Basic Item Information
*/
void GW_ItemSlotBase::RawEncode(OutPacket *oPacket) const
{
	GW_ItemSlotBase::Encode(oPacket, false);
	oPacket->Encode4(nItemID);
	//bool bIsCashItem = liCashItemSN != -1; //liCashItemSN.QuadPart
	oPacket->Encode1(bIsCash); //
	if (bIsCash)
		oPacket->Encode8(liCashItemSN);	
	oPacket->Encode8(liExpireDate);
}

/*
Encode Inventory Item Position
*/
void GW_ItemSlotBase::EncodeInventoryPosition(OutPacket *oPacket) const
{
	auto nEncodePos = nPOS;
	if (nEncodePos <= -1)
	{
		nEncodePos *= -1;
		if (nEncodePos > 100 && nEncodePos < 1000)
			nEncodePos -= 100;
	}

	oPacket->Encode1((char)nEncodePos);
}

/*
Trading System Item Position Encoding
*/
void GW_ItemSlotBase::EncodeTradingPosition(OutPacket *oPacket) const
{
	auto nEncodePos = nPOS;
	if (nEncodePos <= -1)
	{
		nEncodePos *= -1;
		if (nEncodePos > 100 && nEncodePos < 1000)
			nEncodePos -= 100;
	}
	oPacket->Encode1((char)nEncodePos);
}

void GW_ItemSlotBase::LoadAll(int nType, int nCharacterID, std::map<int, ZSharedPtr<GW_ItemSlotBase>>& mRes)
{
	if (nType == GW_ItemSlotBase::EQUIP) 
	{
		//cash equips are loaded from different sql table, so tagging is required.
		GW_ItemSlotEquip::LoadAll(nCharacterID, false, mRes);
		GW_ItemSlotEquip::LoadAll(nCharacterID, true, mRes);
	}
	else 
	{
		GW_ItemSlotBundle::LoadAll(nType, nCharacterID, mRes);
		if(nType == GW_ItemSlotType::CASH)
			GW_ItemSlotPet::LoadAll(nCharacterID, mRes);
	}
}

void GW_ItemSlotBase::DecodeInventoryPosition(InPacket * iPacket) 
{
	nPOS = iPacket->Decode1();
}

void GW_ItemSlotBase::Decode(InPacket *iPacket, bool bForInternal)
{
	bIsPet = (iPacket->Decode1() == GW_ItemSlotInstanceType::GW_ItemSlotPet_Type);
}

void GW_ItemSlotBase::RawDecode(InPacket *iPacket)
{
	GW_ItemSlotBase::Decode(iPacket, false);
	nItemID = iPacket->Decode4();
	bIsCash = iPacket->Decode1() == 1;
	if (bIsCash)
		liCashItemSN = iPacket->Decode8();
	liExpireDate = iPacket->Decode8();
}

GW_ItemSlotBase* GW_ItemSlotBase::CreateItem(int nInstanceType)
{
	GW_ItemSlotBase *pRet = nullptr;
	if (nInstanceType == GW_ItemSlotEquip_Type)
		pRet = AllocObj(GW_ItemSlotEquip);
	else if (nInstanceType == GW_ItemSlotBundle_Type)
		pRet = AllocObj(GW_ItemSlotBundle);
	else if (nInstanceType == GW_ItemSlotPet_Type)
		pRet = AllocObj(GW_ItemSlotPet);
	return pRet;
}