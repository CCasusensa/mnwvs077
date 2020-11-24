#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "Center.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>

struct LoginEntry;

class WvsLogin : public WvsBase
{
private:
	std::recursive_mutex m_mtxLock;

	ConfigLoader* m_pCfgLoader;
	int m_nCenterCount = 0;
	std::shared_ptr<Center> m_apCenterInstance[ServerConstants::kMaxNumberOfCenters];
	std::shared_ptr<asio::io_service> aCenterServerService[ServerConstants::kMaxNumberOfCenters];
	std::shared_ptr<std::thread> m_apCenterWorkThread[ServerConstants::kMaxNumberOfCenters];

	std::map<unsigned int, int> m_mSocketIDToAccountID;
	std::map<int, ZSharedPtr<LoginEntry>> m_mAccountIDToLoginEntry;

	bool m_aIsConnecting[ServerConstants::kMaxNumberOfCenters];
	void CenterAliveMonitor(int idx);
	void ConnectToCenter(int nCenterIdx);

public:
	WvsLogin();
	~WvsLogin();

	int GetCenterCount() const;
	std::shared_ptr<Center>& GetCenter(int idx);
	void SetConfigLoader(ConfigLoader *pCfg);
	void InitializeCenter();
	void OnNotifySocketDisconnected(SocketBase *pSocket);

	//Security
	void RegisterLoginEntry(ZSharedPtr<LoginEntry> &pLoginEntry);
	void RemoveLoginEntryByAccountID(int nAccountID);
	void RemoveLoginEntryByLoginSocketID(unsigned int uLoginSocketSN);
	ZSharedPtr<LoginEntry> GetLoginEntryByAccountID(int nAccountID);
	ZSharedPtr<LoginEntry> GetLoginEntryByLoginSocketSN(unsigned int uLoginSocketSN);
	void RestoreLoginEntry(void *iPacket);
};

