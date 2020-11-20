#include "StateChangeItem.h"
#include <mutex>
#include "User.h"
#include "SecondaryStat.h"
#include "ThiefSkills.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"

#define REGISTER_TS(name, value)\
retTSFlag |= GET_TS_FLAG(##name);\
pRef = &pSS->m_mSetByTS[TemporaryStat::TS_##name]; pRef->second.clear();\
pSS->n##name##_ = bResetByItem ? 0 : value;\
pSS->r##name##_ = bResetByItem ? 0 : -nItemID;\
pSS->t##name##_ = bResetByItem ? 0 : (int)((double)tTime * dTimeBonusRate);\
pSS->nLv##name##_ = 0;\
if(!bResetByItem)\
{\
	pRef->first = bForcedSetTime ? nForcedSetTime : (tCur + (int)((double)tTime * dTimeBonusRate));\
	pRef->second.push_back(&pSS->n##name##_);\
	pRef->second.push_back(&pSS->r##name##_);\
	pRef->second.push_back(&pSS->t##name##_);\
	pRef->second.push_back(&pSS->nLv##name##_);\
} else { pSS->m_mSetByTS.erase(TemporaryStat::TS_##name); }

TemporaryStat::TS_Flag StateChangeItem::Apply(User *pUser, unsigned int tCur, bool bApplyBetterOnly, bool bResetByItem, bool bForcedSetTime, unsigned int nForcedSetTime)
{
	auto &pSS = pUser->GetSecondaryStat();
	std::lock_guard<std::recursive_mutex> userLock(pUser->GetLock());
	std::lock_guard<std::recursive_mutex> lock(pSS->m_mtxLock);
	auto &pBS = pUser->GetBasicStat();
	auto &pS = pUser->GetCharacterData()->mStat;
	auto info = spec.end(), end = spec.end(), time = spec.find("time");

	std::pair<long long int, std::vector<int*>>* pRef = nullptr;
	long long int liFlag = 0;
	int tTime = (nForcedSetTime > tCur) ? (nForcedSetTime - tCur) : (time == end ? 0 : time->second);
	TemporaryStat::TS_Flag retTSFlag;

	SkillEntry *pAlchemist = nullptr;
	int nSLVAlchemist = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), ThiefSkills::Hermit_Alchemist, &pAlchemist);
	double dAmountBonusRate = 1.0, dTimeBonusRate = 1.0;

	if (pAlchemist && nSLVAlchemist)
	{
		dAmountBonusRate = (double)pAlchemist->GetLevelData(nSLVAlchemist)->m_nX / 100.0;
		dTimeBonusRate = (double)pAlchemist->GetLevelData(nSLVAlchemist)->m_nY / 100.0;
	}

	for (auto& info : spec)
	{
		if (info.first == "hp")
		{
			pS->nHP += (int)((double)info.second * dAmountBonusRate);
			pS->nHP = pS->nHP > pBS->nMHP ? pBS->nMHP : pS->nHP;
			liFlag |= BasicStat::BS_HP;
		}
		else if (info.first == "mp")
		{
			pS->nMP += (int)((double)info.second * dAmountBonusRate);
			pS->nMP = pS->nMP > pBS->nMMP ? pBS->nMMP : pS->nMP;
			liFlag |= BasicStat::BS_MP;
		}
		if (info.first == "hpR")
		{
			pS->nHP += (int)((double)pBS->nMHP * dAmountBonusRate * (info.second) / 100);
			pS->nHP = pS->nHP > pBS->nMHP ? pBS->nMHP : pS->nHP;
			liFlag |= BasicStat::BS_HP;
		}
		else if (info.first == "mpR")
		{
			pS->nMP += (int)((double)pBS->nMMP * dAmountBonusRate * (info.second) / 100);
			pS->nMP = pS->nMP > pBS->nMMP ? pBS->nMMP : pS->nMP;
			liFlag |= BasicStat::BS_MP;
		}
		else if (info.first == "mad")
		{
			REGISTER_TS(MAD, info.second);
		}
		else if (info.first == "mdd")
		{
			REGISTER_TS(MDD, info.second);
		}
		else if (info.first == "pad")
		{
			REGISTER_TS(PAD, info.second);
		}
		else if (info.first == "pdd")
		{
			REGISTER_TS(PDD, info.second);
		}
		else if (info.first == "acc")
		{
			REGISTER_TS(ACC, info.second);
		}
		else if (info.first == "eva")
		{
			REGISTER_TS(EVA, info.second);
		}
		else if (info.first == "craft")
		{
			REGISTER_TS(Craft, info.second);
		}
		else if (info.first == "speed")
		{
			REGISTER_TS(Speed, info.second);
		}
		else if (info.first == "jump")
		{
			REGISTER_TS(Jump, info.second);
		}
		else if (info.first == "thaw")
		{
			REGISTER_TS(Thaw, info.second);
		}
		else if (info.first == "morph")
		{
			REGISTER_TS(Morph, info.second);
		}
		else if (info.first == "mesoupbyitem")
		{
			REGISTER_TS(MesoUpByItem, info.second);
		}
		else if (info.first == "weakness")
		{
			REGISTER_TS(Weakness, info.second);
		}
		else if (info.first == "darkness")
		{
			REGISTER_TS(Darkness, info.second);
		}
		else if (info.first == "curse")
		{
			REGISTER_TS(Curse, info.second);
		}
		else if (info.first == "seal")
		{
			REGISTER_TS(Seal, info.second);
		}
		else if (info.first == "poison")
		{
			REGISTER_TS(Poison, info.second);
		}
		else if (info.first == "stun")
		{
			REGISTER_TS(Stun, info.second);
		}
		else if (info.first == "slow")
		{
			REGISTER_TS(Slow, info.second);
		}
		else if (info.first == "mhp_temp")
		{
			REGISTER_TS(MaxHP, info.second);
		}
		else if (info.first == "mmp_temp")
		{
			REGISTER_TS(MaxMP, info.second);
		}
	}
	
	if (!bForcedSetTime)
	{
		pUser->SendTemporaryStatReset(retTSFlag);
		pUser->SendTemporaryStatSet(retTSFlag, 0);
	}
	pUser->SendCharacterStat(true, liFlag);
	pUser->ValidateStat();
	return retTSFlag;
}