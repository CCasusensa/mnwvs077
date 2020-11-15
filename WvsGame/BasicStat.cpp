#include "BasicStat.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"

#include "SecondaryStat.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "..\Database\GW_SkillRecord.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

BasicStat::BasicStat()
{
}

BasicStat::~BasicStat()
{
}

void BasicStat::SetPermanentSkillStat(GA_Character * pChar)
{
	int nMaxHPIncRate = 0, nMaxMPIncRate = 0;

	for (auto& skillRecord : pChar->mSkillRecord)
	{
		auto pEntry = SkillInfo::GetInstance()->GetSkillByID(skillRecord.first);
		if (pEntry) 
		{
			auto pLevelData = pEntry->GetLevelData(skillRecord.second->nSLV);
			if (!pLevelData)
				continue;

			//Add stat by Character's level
		}
	}
	nMHP = nMHP * (nMaxHPIncRate + 100) / 100;
	nMMP = nMMP * (nMaxMPIncRate + 100) / 100;
	if (nMHP > 30000)
		nMHP = 30000;
	if (nMMP > 30000)
		nMMP = 30000;
}

void BasicStat::SetFrom(GA_Character * pChar, int nMaxHPIncRate, int nMaxMPIncRate, int nBasicStatInc)
{
	const GW_CharacterStat *pCS = pChar->mStat;

	nGender = pChar->mStat->nGender;
	nLevel = pChar->mLevel->nLevel;
	nJob = pCS->nJob;
	nSTR = pCS->nStr;
	nINT = pCS->nInt;
	nDEX = pCS->nDex;
	nLUK = pCS->nLuk;
	nPOP = pCS->nPOP;
	nMHP = pCS->nMaxHP;
	nMMP = pCS->nMaxMP;

	nCharismaEXP = pCS->nCharismaEXP;
	nInsightEXP = pCS->nInsightEXP;
	nWillEXP = pCS->nWillEXP;
	nSenseEXP = pCS->nSenseEXP;
	nCharmEXP = pCS->nCharmEXP;
	const GW_ItemSlotEquip* pEquip;
	for (const auto& itemEquipped : pChar->mItemSlot[1])
	{
		pEquip = (const GW_ItemSlotEquip*)itemEquipped.second;
		if (pEquip->nPOS >= 0)
			break;
		nSTR += pEquip->nSTR;
		nLUK += pEquip->nLUK;
		nDEX += pEquip->nDEX;
		nINT += pEquip->nINT;
		
		nMHP += pEquip->nMaxHP;
		nMMP += pEquip->nMaxMP;

		//Apply item option here.
	}
	nMHP = nMHP * (nMaxHPIncRate + 100) / 100;
	nMMP = nMMP * (nMaxMPIncRate + 100) / 100;
	if (nMHP > 30000)
		nMHP = 30000;
	if (nMMP > 30000)
		nMMP = 30000;

	nSTR += nBasicStatInc * pCS->nStr / 100;
	nLUK += nBasicStatInc * pCS->nLuk / 100;
	nDEX += nBasicStatInc * pCS->nDex / 100;
	nINT += nBasicStatInc * pCS->nInt / 100;
}
