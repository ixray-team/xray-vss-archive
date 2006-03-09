#pragma once

#include "GameSpy_FuncDefs.h"
#include "GameSpy_QR2.h"

class CServerList;

struct GameInfo	{
	shared_str	InfoName;
	shared_str	InfoData;
	GameInfo(LPCSTR Name, LPCSTR Data) { InfoName._set(Name); InfoData._set(Data); };
};

struct PlayerInfo {
	string128	Name;
	s16			Frags;
	u16			Deaths;
	u8			Rank;
	u8			Team;
	bool		Spectator;
	u8			Artefacts;
};

struct TeamInfo
{
	u8			Score;
};

struct ServerInfo{
//	SBServer pSBServer;
	string128	m_Address;
	string128	m_HostName;
	string128	m_ServerName;
	string128	m_SessionName;
	string128	m_ServerGameType;
	u8			m_GameType;

	s16						m_ServerNumPlayers;
	s16						m_ServerMaxPlayers;
	s16						m_ServerNumTeams;
	bool					m_bDedicated;
	bool					m_bFFire;
	s16						m_s16FFire;
	bool					m_bPassword;
	s16						m_Ping;
	s16						m_Port, m_HPort;

	xr_vector<GameInfo>		m_aInfos;
	xr_vector<PlayerInfo>	m_aPlayers;
	xr_vector<TeamInfo>		m_aTeams;

	int						Index;

	ServerInfo () {};
	ServerInfo (string128 NewAddress) 
	{
		strcpy(m_Address, NewAddress);
	};

	bool			operator	==		(LPCSTR Address){int res = xr_strcmp(m_Address, Address);return	res	 == 0;};
};

class CGameSpy_Browser
{
private:
	string16	m_SecretKey;

	HMODULE	hGameSpyDLL;

	void	LoadGameSpy();

	void*	m_pGSBrowser;
	
	void	ReadServerInfo	(ServerInfo* pServerInfo, void* pServer);

	CGameSpy_QR2	m_QR2;

	CServerList*		m_pServerList;

public:
	CGameSpy_Browser();
	~CGameSpy_Browser();

	bool			Init(CServerList* pServerList);
	void			RefreshList_Full(bool Local);
	void			RefreshQuick(int Index);
	
	void CallBack_OnUpdateCompleted		();

	int				GetServersCount();
	void			GetServerInfoByIndex(ServerInfo* pServerInfo, int idx);

	void			OnUpdateFailed		(void* server);
	void			Update();	

	void			UpdateServerList	();

private:
	//------------------------------- GameSpy_ServerBrowser -----------------------
	GAMESPY_FN_VAR_DECL(void*, ServerBrowserNew, (const char *queryForGamename, const char *queryFromGamename, const char *queryFromKey, int queryFromVersion, int maxConcUpdates, int queryVersion, fnSBCallback callback, void *instance));
	GAMESPY_FN_VAR_DECL(void, ServerBrowserFree, (void* sb));
	GAMESPY_FN_VAR_DECL(void, ServerBrowserClear, (void* sb));
	
	GAMESPY_FN_VAR_DECL(SBError, ServerBrowserThink, (void* sb));
	GAMESPY_FN_VAR_DECL(SBState, ServerBrowserState, (void* sb));
	GAMESPY_FN_VAR_DECL(void, ServerBrowserHalt, (void* sb));
	GAMESPY_FN_VAR_DECL(SBError, ServerBrowserUpdate, (void* sb, SBBool async, SBBool disconnectOnComplete, const unsigned char *basicFields, int numBasicFields, const char *serverFilter));
	GAMESPY_FN_VAR_DECL(SBError, ServerBrowserLANUpdate, (void* sb, SBBool async, unsigned short startSearchPort, unsigned short endSearchPort));
	GAMESPY_FN_VAR_DECL(int, ServerBrowserCount, (void* sb));
	GAMESPY_FN_VAR_DECL(void*, ServerBrowserGetServer, (void* sb, int index));
	GAMESPY_FN_VAR_DECL(void*, ServerBrowserGetServerByIP, (void* sb, const char* ip, unsigned short port));

	GAMESPY_FN_VAR_DECL(char *, SBServerGetPublicAddress, (void * server));
	GAMESPY_FN_VAR_DECL(unsigned short, SBServerGetPublicQueryPort, (void * server));
	GAMESPY_FN_VAR_DECL(const char *, SBServerGetStringValue, (void * server, const char *keyname, const char *def));
	GAMESPY_FN_VAR_DECL(int, SBServerGetIntValue, (void * server, const char *key, int idefault));
	GAMESPY_FN_VAR_DECL(double, SBServerGetFloatValue, (void * server, const char *key, double fdefault));
	GAMESPY_FN_VAR_DECL(SBBool, SBServerGetBoolValue, (void * server, const char *key, SBBool bdefault));
	GAMESPY_FN_VAR_DECL(int, SBServerGetPing, (void * server));

	GAMESPY_FN_VAR_DECL(SBError, ServerBrowserAuxUpdateServer, (void* sb, void* server, SBBool async, SBBool fullUpdate));
	GAMESPY_FN_VAR_DECL(SBError, ServerBrowserAuxUpdateIP, (void* sb, const char *ip, unsigned short port, SBBool viaMaster, SBBool async, SBBool fullUpdate));

	GAMESPY_FN_VAR_DECL(const char *, SBServerGetPlayerStringValue, (void* server, int playernum, const char *key, const char *sdefault));
	GAMESPY_FN_VAR_DECL(int, SBServerGetPlayerIntValue, (void* server, int playernum, const char *key, int idefault));
	GAMESPY_FN_VAR_DECL(double, SBServerGetPlayerFloatValue, (void* server, int playernum, const char *key, double fdefault));

	GAMESPY_FN_VAR_DECL(const char *, SBServerGetTeamStringValue, (void* server, int teamnum, const char *key, const char *sdefault));
	GAMESPY_FN_VAR_DECL(int, SBServerGetTeamIntValue, (void* server, int teamnum, const char *key, int idefault));
	GAMESPY_FN_VAR_DECL(double, SBServerGetTeamFloatValue, (void* server, int teamnum, const char *key, double fdefault));

	GAMESPY_FN_VAR_DECL(void, ServerBrowserRemoveIP, (void* sb, const char *ip, unsigned short port));
	GAMESPY_FN_VAR_DECL(void, ServerBrowserRemoveServer, (void* sb, void* server));

};


