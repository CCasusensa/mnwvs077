#include "WvsCenter.h"
#include "WvsWorld.h"

#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"

WvsCenter::WvsCenter()
{
}

WvsCenter::~WvsCenter()
{
}

void WvsCenter::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	printf("[WvsCenter][WvsCenter::OnNotifySocketDisconnected]頻道伺服器[WvsGame]中斷連線，告知WvsLogin變更。\n");
	if (pSocket->GetServerType() == ServerConstants::SRV_GAME)
	{
		auto iter = m_mChannel.begin();
		for (; iter != m_mChannel.end(); ++iter)
			if (iter->second->GetLocalSocket().get() == pSocket)
				break;
		if (iter != m_mChannel.end())
		{
			FreeObj( iter->second );
			m_mChannel.erase(iter);
		}
		//--nConnectedChannel;
		NotifyWorldChanged();
	}
	else if (pSocket->GetServerType() == ServerConstants::SRV_SHOP) 
	{
		FreeObj( m_pShopEntry );
		m_pShopEntry = nullptr;
	}
}

LocalServerEntry * WvsCenter::GetChannel(int nIdx)
{
	auto findIter = m_mChannel.find(nIdx);
	return findIter == m_mChannel.end() ? nullptr : findIter->second;
}

int WvsCenter::GetChannelCount()
{
	return (int)m_mChannel.size();
}

void WvsCenter::Init()
{
}

void WvsCenter::NotifyWorldChanged()
{
	auto& socketList = WvsBase::GetInstance<WvsCenter>()->GetSocketList();
	for (const auto& socket : socketList)
	{
		if (socket.second->GetServerType() == ServerConstants::SRV_LOGIN)
		{
			OutPacket oPacket;
			oPacket.Encode2(CenterSendPacketFlag::CenterStatChanged);
			oPacket.Encode2(WvsBase::GetInstance<WvsCenter>()->GetChannelCount());
			for (auto& pEntry : m_mChannel)
				oPacket.Encode1(pEntry.first);
			socket.second->SendPacket(&oPacket);
		}
	}
}

LocalServerEntry * WvsCenter::GetShop()
{
	return m_pShopEntry;
}

void WvsCenter::SetShop(LocalServerEntry *pEntry)
{
	if (m_pShopEntry)
		FreeObj(m_pShopEntry);
	m_pShopEntry = pEntry;
}

LocalServerEntry *WvsCenter::GetLoginServer()
{
	return m_pLoginEntry;
}

void WvsCenter::SetLoginServer(LocalServerEntry *pEntry)
{
	if (m_pLoginEntry)
		FreeObj(m_pLoginEntry);

	m_pLoginEntry = pEntry;
}

void WvsCenter::RegisterChannel(int nChannelID, std::shared_ptr<SocketBase> &pServer, InPacket *iPacket)
{
	LocalServerEntry *pEntry = AllocObj( LocalServerEntry );

	pEntry->SetLocalSocket(pServer);
	pEntry->SetExternalIP(iPacket->Decode4());
	pEntry->SetExternalPort(iPacket->Decode2());
	printf("[WvsCenter][WvsCenter::RegisterChannel]新的頻道伺服器[Channel ID = %d][WvsGame]註冊成功，IP : ", nChannelID);
	auto ip = pEntry->GetExternalIP();
	for (int i = 0; i < 4; ++i)
		printf("%d ", (int)((char*)&ip)[i]);
	printf("\n Port = %d\n", pEntry->GetExternalPort());
	m_mChannel.insert({ nChannelID, pEntry });
	RestoreConnectedUser(nChannelID, iPacket);
}

void WvsCenter::RegisterCashShop(std::shared_ptr<SocketBase>& pServer, InPacket * iPacket)
{
	LocalServerEntry *pEntry = AllocObj( LocalServerEntry );

	pEntry->SetLocalSocket(pServer);
	pEntry->SetExternalIP(iPacket->Decode4());
	pEntry->SetExternalPort(iPacket->Decode2());
	printf("[WvsCenter][WvsCenter::RegisterCashShop]新的商城伺服器[WvsShop]註冊成功。\n");

	SetShop(pEntry);
	RestoreConnectedUser(WvsWorld::CHANNELID_SHOP, iPacket);
}

void WvsCenter::RestoreConnectedUser(int nChannelID, InPacket * iPacket)
{
	int nConnectedUserCount = iPacket->Decode4(), nUserID = 0;
	for (int i = 0; i < nConnectedUserCount; ++i)
	{
		nUserID = iPacket->Decode4();
		auto pwUser = AllocObj(WvsWorld::WorldUser);
		pwUser->m_nCharacterID = nUserID;
		pwUser->m_nAccountID = iPacket->Decode4();
		pwUser->m_bLoggedIn = true;
		pwUser->m_bMigrated = true;
		pwUser->m_nChannelID = nChannelID;

		WvsWorld::GetInstance()->SetUser(
			nUserID, pwUser
		);
	}
}

void WvsCenter::RegisterLoginServer(std::shared_ptr<SocketBase>& pServer)
{
	LocalServerEntry *pEntry = AllocObj(LocalServerEntry);
	pEntry->SetLocalSocket(pServer);
	SetLoginServer(pEntry);
}
