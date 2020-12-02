#include "FieldMan.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "TimerThread.h"

#include "Field.h"
#include "Field_GuildBoss.h"
#include "Field_MonsterCarnival.h"
#include "FieldSet.h"
#include "PortalMap.h"
#include "ReactorPool.h"
#include "WvsPhysicalSpace2D.h"

#include <filesystem>
#include <fstream>
#include <streambuf>

namespace fs = std::experimental::filesystem;

FieldMan::FieldMan()
{
}

FieldMan::~FieldMan()
{
}

FieldMan * FieldMan::GetInstance()
{
	static FieldMan *sPtrFieldMan = new FieldMan();
	return sPtrFieldMan;
}

void FieldMan::RegisterField(int nFieldID)
{
	FieldFactory(nFieldID);
}

void FieldMan::FieldFactory(int nFieldID)
{
	/*if (mField[nFieldID]->GetFieldID() != 0)
		return;*/
		/*
		Field
		Field_Tutorial
		Field_ShowaBath
		Field_WeddingPhoto
		Field_SnowBall
		Field_Tournament
		Field_Coconut
		Field_OXQuiz
		Field_PersonalTimeLimit
		Field_GuildBoss
		Field_MonsterCarnival
		Field_Wedding
		...
		*/
	std::string sField = std::to_string(nFieldID);
	while (sField.size() < 9)
		sField = "0" + sField;
	auto& mapWz = WzResMan::GetInstance()->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][sField];
	if (mapWz == mapWz.end())
		return;

	auto& infoData = mapWz["info"];
	int nFieldType = infoData["fieldType"];

	Field* pField = nullptr;
	switch (nFieldType)
	{
		case 8:
			pField = AllocObjCtor(Field_GuildBoss)(&mapWz, nFieldID);
			break;
		case 10:
			pField = AllocObjCtor(Field_MonsterCarnival)(&mapWz, nFieldID);
			break;
		default:
			pField = AllocObjCtor(Field)(&mapWz, nFieldID);
			break;
	}

	RestoreFoothold(pField, &(mapWz["foothold"]), nullptr, &infoData);
	pField->InitLifePool();
	m_mField[nFieldID] = pField;
	TimerThread::RegisterField(pField);
}

void FieldMan::LoadAreaCode()
{
	auto& mapWz = WzResMan::GetInstance()->GetWz(Wz::Map)["Map"]["AreaCode"];
	for (auto& area : mapWz)
		m_mAreaCode.insert({ atoi(area.GetName().c_str()), (int)area });
}

bool FieldMan::IsConnected(int nFrom, int nTo)
{
	nFrom /= 10000000;
	nTo /= 10000000;
	auto iterFrom = m_mAreaCode.find(nFrom);
	auto iterTo = m_mAreaCode.find(nTo);
	if (iterFrom == m_mAreaCode.end() || iterTo == m_mAreaCode.end())
		return false;

	return iterFrom->second == iterTo->second;
}

void FieldMan::LoadFieldSet()
{
	std::string strPath = "./DataSrv/FieldSet";
	for (auto &file : fs::directory_iterator(strPath))
	{
		FieldSet *pFieldSet = AllocObj( FieldSet );
		std::wstring wStr = file.path();
		//Convert std::wstring to std::string, note that the path shouldn't include any NON-ASCII character.
		pFieldSet->Init(std::string{ wStr.begin(), wStr.end() });
		m_mFieldSet[pFieldSet->GetFieldSetName()] = pFieldSet;
	}
	WzResMan::GetInstance()->RemountAll();
}

void FieldMan::RegisterAllField()
{
	for (int i = 1; i <= 9; ++i)
		for (auto& mapWz : WzResMan::GetInstance()->GetWz(Wz::Map)["Map"]["Map" + std::to_string(i)])
			RegisterField(atoi(mapWz.GetName().c_str()));
}

Field* FieldMan::GetField(int nFieldID)
{
	std::lock_guard<std::mutex> lock(m_mtxFieldMan);

	auto fieldResult = m_mField.find(nFieldID);
	if (fieldResult == m_mField.end())
		RegisterField(nFieldID);
	return m_mField[nFieldID];
}

FieldSet * FieldMan::GetFieldSet(const std::string & sFieldSetName)
{
	auto fieldResult = m_mFieldSet.find(sFieldSetName);
	if (fieldResult == m_mFieldSet.end())
		return nullptr;
	return fieldResult->second;
}

void FieldMan::RestoreFoothold(Field * pField, void * pPropFoothold, void * pLadderOrRope, void * pInfo)
{
	auto& refInfo = *((WzIterator*)pInfo);
	int nFieldLink = (nFieldLink = atoi(((std::string)refInfo["link"]).c_str()));
	if ((nFieldLink != 0))
	{
		auto fieldStr = StringUtility::LeftPadding(std::to_string(nFieldLink), 9, '0');
		auto& mapWz = WzResMan::GetInstance()->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldLink / 100000000)][fieldStr];
		pInfo = &(mapWz["info"]);
		pPropFoothold = &(mapWz["foothold"]);
	}
	pField->GetSpace2D()->Load(pPropFoothold, pLadderOrRope, pInfo);
	pField->SetLeftTop(
		pField->GetSpace2D()->GetRect().left,
		pField->GetSpace2D()->GetRect().top
	);
	pField->SetMapSize(
		pField->GetSpace2D()->GetRect().right - pField->GetSpace2D()->GetRect().left,
		pField->GetSpace2D()->GetRect().bottom - pField->GetSpace2D()->GetRect().top
	);
}
