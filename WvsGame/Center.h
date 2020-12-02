#pragma once
#include "..\WvsLib\Net\SocketBase.h"

class Center :
	public SocketBase
{
private:
	int m_nCenterIndex, m_nWorldID;
	void OnConnected();

public:
	Center(asio::io_service& serverService);
	~Center();

	void OnClosed();
	void OnConnectFailed();
	void SetCenterIndex(int idx);
	void SetWorldID(int nWorldID);
	int GetWorldID() const;

	void OnPacket(InPacket *iPacket);
	void OnCenterMigrateInResult(InPacket *iPacket);
	void OnTransferChannelResult(InPacket *iPacket);
	void OnRemoteBroadcasting(InPacket *iPacket);
	void OnEntrustedShopResult(InPacket *iPacket);
	void OnCheckMigrationState(InPacket *iPacket);
	void OnGuildBBSResult(InPacket *iPacket);
	void OnCheckGivePopularityResult(InPacket *iPacket);

	static void OnNotifyCenterDisconnected(SocketBase *pSocket);
};

