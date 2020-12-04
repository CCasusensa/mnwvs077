#pragma once
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <vector>

class InPacket;
class OutPacket;
class User;

class GuildMan
{
public:
	static const int CREATE_GUILD_COST = 1500000;
	static const int REMOVE_GUILD_COST = 200000;
	static const int SET_MARK_COST = 5000000;
	static const int REMOVE_MARK_COST = 1000000;

	enum GuildRequest
	{
		rq_Guild_Load = 0x00,
		rq_Guild_Create = 0x02,
		rq_Guild_Invite = 0x05,
		rq_Guild_Join = 0x06,
		rq_Guild_Withdraw = 0x07,
		rq_Guild_Withdraw_Kick = 0x08,
		rq_Guild_SetGradeName = 0x0D,
		rq_Guild_SetMemberGrade = 0x0E,
		rq_Guild_SetMark = 0x0F,
		rq_Guild_SetNotice = 0x10,
		rq_Guild_IncMaxMemberNum = 0x11,
		rq_Guild_IncPoint = 0x12,
		rq_Guild_LevelOrJobChanged = 0x13,

		req_GuildQuest_CheckQuest = 0xA0,
		req_GuildQuest_RegisterQuest = 0xA1,
		req_GuildQuest_CancelQuest = 0xA2,
		req_GuildQuest_CheckEnteringOrder = 0xA3,
		req_GuildQuest_CompleteQuest = 0xA4,
	};

	enum GuildResult
	{
		res_Guild_Load = 0,
		res_Guild_Create = 1,
		res_Guild_Invite = 5,
		res_Guild_AskMark = 17,
		res_Guild_Update = 26,
		res_Guild_Failed_AlreadyInGuild = 40,
		res_Guild_Join = 39,
		res_Guild_Failed_JoinFailed = 43,
		res_Guild_Withdraw = 44,
		res_Guild_Withdraw_Kicked = 47,
		res_Guild_Send_Guild_DisbandSuccess_Dialog = 50,
		res_Guild_Send_Guild_DisbandFailed_Dialog = 52,
		res_Guild_Failed_NotAllowedToInvite = 53,
		res_Guild_Failed_InProcessing = 54,
		res_Guild_Failed_InvitationRejected = 55,
		res_Guild_IncMaxMemberNum = 58,
		res_Guild_Failed_IncMaxMemberNumError = 59,
		res_Guild_LevelOrJobChanged = 60,
		res_Guild_Notify_LoginOrLogout = 61,
		res_Guild_SetGradeName = 62,
		res_Guild_SetMemberGrade = 64,
		res_Guild_SetMark = 66,
		res_Guild_SetNotice = 68,
		res_Guild_IncPoint = 72,
		res_GuildQuest_SetRegisteredGuild = 126,
	};

	struct MemberData
	{
		std::string sCharacterName;
		int nLevel = 0, 
			nJob = 0, 
			nGrade = 0, 
			nContribution = 0;

		bool bOnline = false;

		void Set(const std::string& strName, int nLevel, int nJob, bool bOnline);
		void Encode(OutPacket *oPacket) const;
		void Decode(InPacket *iPacket);
	};

	struct GuildData
	{
		std::string sGuildName, sNotice;
		std::vector<std::string> asGradeName;
		std::vector<int> anCharacterID;
		std::vector<MemberData> aMemberData;

		int nGuildID = 0,
			nPoint = 0,
			nMaxMemberNum = 0,
			nMark = 0,
			nMarkBg = 0,
			nMarkColor = 0,
			nMarkBgColor = 0,
			nLoggedInUserCount = 0;

		void Encode(OutPacket *oPacket);
		void Decode(InPacket *iPacket);
	};

private:
	std::recursive_mutex m_mtxGuildLock;
	std::atomic<int> m_atiGuildIDCounter;

	GuildMan();
	~GuildMan();

#ifdef _WVSGAME
	void Update();
#endif

	std::map<int, int> m_mCharIDToGuildID;
	std::map<int, GuildData*> m_mGuild;
	std::map<std::string, GuildData*> m_mNameToGuild;

	//--GUILD QUEST STUFF--
	//<GuildID, <ChannelID, nCharacterID>>
	std::map<int, std::pair<int, int>> m_mQuestRegisteredChannel;
	//<ChannelID, <Register Time, GuildID>> --> use ordered map for searching the entering order.
	std::map<int, std::map<unsigned int, int>> m_mChannelRegisteredQuest;

	int m_nQuestRegisteredCharacterID = 0, 
		m_nQuestRegisteredGuildID = 0, 
		m_nQuestStartedGuildID = 0;
	unsigned int m_tLastGuildQuestUpdate = 0;
public:
	static GuildMan* GetInstance();
	int GetGuildIDByCharID(int nCharacterID);
	GuildData *GetGuildByCharID(int nCharacterID);
	GuildData *GetGuild(int nGuildID);
	GuildData *GetGuildByName(const std::string& strName);
	int FindUser(int nCharacterID, GuildData *pData);
	void OnLeave(User *pUser);
	void Broadcast(OutPacket *oPacket, const std::vector<int>& anMemberID, int nPlusOne);
	void OnPacket(InPacket *iPacket);
	void OnGuildBBSRequest(User *pUser, InPacket *iPacket);
	void OnGuildRequest(User *pUser, InPacket *iPacket);
	void OnGuildLoadDone(InPacket *iPacket);
	void OnCreateNewGuildDone(InPacket *iPacket);
	bool IsGuildMaster(int nGuildID, int nCharacterID);
	bool IsGuildSubMaster(int nGuildID, int nCharacterID);
	bool IsGuildMember(int nGuildID, int nCharacterID);
	bool IsGuildFull(int nGuildID);
	int GetMemberGrade(int nGuildID, int nCharacterID);
	void RemoveUser(GuildData *pGuild, int nCharacterID);
	void OnGuildInviteRequest(User *pUser, InPacket *iPacket);
	void TryCreateNewGuild(User *pUser, InPacket *iPacket);
	void OnCreateNewGuildRequest(User *pUser, const std::string& strGuildName, const std::vector<std::pair<int, MemberData>>& aMember);

#ifdef _WVSGAME
	int GetQuestRegisteredCharacterID() const;
	int GetQuestRegisteredGuildID() const;
	void OnGuildQuestProcessing(bool bComplete = false);
	void SendGuildQuestComplete();
#endif

	void OnRemoveGuildRequest(User *pUser);
	void OnJoinGuildReqest(User *pUser, InPacket *iPacket);
	void OnJoinGuildDone(InPacket *iPacket);
	void OnWithdrawGuildRequest(User *pUser, InPacket *iPacket);
	void OnWithdrawGuildDone(InPacket *iPacket);
	void OnSetGradeNameRequest(User *pUser, InPacket *iPacket);
	void OnSetGradeNameDone(InPacket *iPacket);
	void OnSetNoticeRequest(User *pUser, InPacket *iPacket);
	void OnSetNoticeDone(InPacket *iPacket);
	void OnSetMemberGradeRequest(User *pUser, InPacket *iPacket);
	void OnSetMemberGradeDone(InPacket *iPacket);
	void OnAskGuildMark(User *pUser);
	void OnSetMarkRequest(User *pUser, InPacket *iPacket);
	void OnSetMarkDone(InPacket *iPacket);
	void OnNotifyLoginOrLogout(InPacket *iPacket);
	void OnIncMaxMemberNumRequest(User *pUser, int nInc, int nCost);
	void OnIncMaxMemberNum(InPacket *iPacket);
	void OnIncPointRequest(int nGuildID, int nInc);
	void OnIncPoint(InPacket *iPacket);
	void MakeGuildUpdatePacket(OutPacket *oPacket, GuildData *pGuild);
	void PostChangeLevelOrJob(User *pUser, int nVal, bool bLevelChanged);
	void OnChangeLevelOrJob(InPacket *iPacket);
	void SetRegisteredQuestGuild(InPacket *iPacket);

	///==========================CENTER=================================
#ifdef _WVSCENTER
	void SendToAll(GuildData* pGuild, OutPacket *oPacket);
	void LoadGuild(InPacket *iPacket, OutPacket *oPacket);
	void CreateNewGuild(InPacket *iPacket, OutPacket *oPacket);
	void JoinGuild(InPacket *iPacket, OutPacket *oPacket);
	void WithdrawGuild(InPacket *iPacket, OutPacket *oPacket);
	void SetNotice(InPacket *iPacket, OutPacket *oPacket);
	void SetGradeName(InPacket *iPacket, OutPacket *oPacket);
	void SetMemberGrade(InPacket *iPacket, OutPacket *oPacket);
	void SetMark(InPacket *iPacket, OutPacket *oPacket);
	void IncMaxMemberNum(InPacket *iPacket, OutPacket *oPacket);
	void IncPoint(InPacket *iPacket, OutPacket *oPacket);
	void ChangeJobOrLevel(InPacket *iPacket, OutPacket *oPacket);
	void OnGuildQuestRequest(int nCharacterID, int nGuildID, int nChannelID, int nType, OutPacket *oPacket);
	void NotifyLoginOrLogout(int nCharacterID, bool bMigrateIn);
#endif
};

