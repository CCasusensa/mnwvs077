#include "ShopInfo.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsGame\ItemInfo.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\Database\GW_CashItemInfo.h"

ShopInfo::ShopInfo()
{
}


ShopInfo::~ShopInfo()
{
}

ShopInfo * ShopInfo::GetInstance()
{
	static ShopInfo* pInstance = new ShopInfo;
	return pInstance;
}

const CSCommodity * ShopInfo::GetCSCommodity(int nSN) const
{
	auto findIter = m_mCommodity.find(nSN);
	if (findIter == m_mCommodity.end())
		return nullptr;
	return &(findIter->second);
}

const std::vector<const CSCommodity*>& ShopInfo::GetCashPackage(int nSN) const
{
	const static std::vector<const CSCommodity*> aEmpty;
	auto findIter = m_mCashPackage.find(nSN);
	if (findIter == m_mCashPackage.end())
		return aEmpty;
	return findIter->second;
}

void ShopInfo::LoadCommodity(void * pCommodity, bool bCheckValid)
{
	auto& refCommodity = *((WzIterator*)pCommodity);
	for (auto& item : refCommodity)
	{
		CSCommodity& commodity = m_mOriginalCommodity[(int)item["SN"]];
		commodity.nSN = (int)item["SN"];
		commodity.nItemID = (int)item["ItemId"];
		commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::ItemID;

		commodity.nCount = (int)item["Count"];
		commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::Count;

		commodity.nPrice = (int)item["Price"];
		commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::Price;

		commodity.nMeso = (int)item["Meso"];
		//commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::Meso;

		commodity.nMaplePoint = (int)item["MaplePoint"];
		//commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::MaplePoint;

		commodity.nBonus = (int)item["Bonus"];
		commodity.nPeriod = (int)item["Period"];
		commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::Period;

		commodity.nPriority = (int)item["Priority"];
		commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::Priority;

		commodity.nGender = (int)item["Gender"];
		commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::CommdityGender;

		commodity.nOnSale = (int)item["OnSale"];
		commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::OnSale;

		commodity.nForcedCatagory = (int)item["forcedCategory"];
		commodity.nForcedSubCategory = (int)item["forcedSubCategory"];
		commodity.nGameWorld = (int)item["gameWorld"];

		commodity.nPbCash = (int)item["PbCash"];
		//commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::PbCash;

		commodity.nPbPoint = (int)item["PbPoint"];
		//commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::PbPoint;

		commodity.nPbGift = (int)item["PbGift"];
		//commodity.nModifiedFlag |= CSCommodity::ModifiedFlag::PbGift;

		commodity.nRefundable = (int)item["Refundable"];
		commodity.nWebShop = (int)item["WebShop"];

		if (commodity.nSN >= 13000000 && 
			commodity.nSN <= 81000000 && 
			commodity.nPrice > 0)
			m_mModifiedCommodity.insert({
			commodity.nSN, commodity
		});
	}
	m_mCommodity = m_mOriginalCommodity;
}

void ShopInfo::LoadCashPackage(void * pCashPackage, bool bCheckValid)
{
	auto& refCashPackage = *((WzIterator*)pCashPackage);

	const CSCommodity* pCommodity = nullptr;
	for (auto& package : refCashPackage)
	{
		std::vector<const CSCommodity*> aCommodity;
		auto& nodeSN = package["SN"];
		for (auto& SN : nodeSN)
		{
			pCommodity = GetCSCommodity((int)SN);
			if (pCommodity)
				aCommodity.push_back(pCommodity);
		}
		m_mCashPackage.insert({ atoi(package.GetName().c_str()), std::move(aCommodity) });
	}
}

GW_CashItemInfo * ShopInfo::GetCashItemInfo(const CSCommodity * pCS) const
{
	GW_CashItemInfo *pRet = AllocObj(GW_CashItemInfo);
	pRet->nItemID = pCS->nItemID;
	pRet->bRefundable = (pCS->nRefundable == 1);
	pRet->nNumber = pCS->nCount;
	pRet->nCommodityID = pCS->nSN;
	if (pCS->nPeriod)
		pRet->liDateExpire = GameDateTime::GetDateExpireFromPeriod(pCS->nPeriod);
	return pRet;
}

void ShopInfo::Init()
{
	auto& wzEtc = WzResMan::GetInstance()->GetWz(Wz::Etc);
	LoadCommodity((void*)&(wzEtc["Commodity"]), true);
	LoadCashPackage((void*)&(wzEtc["CashPackage"]), true);
}

void ShopInfo::EncodeModifiedCommodity(OutPacket *oPacket)
{
	oPacket->Encode2((short)m_mModifiedCommodity.size());
	for (auto& pCommodity : m_mModifiedCommodity)
	{
		oPacket->Encode4(pCommodity.second.nSN);
		pCommodity.second.EncodeModifiedData(oPacket);
	}
}
