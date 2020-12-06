#pragma once
#include <vector>
#include "Classes/OnlineManager.h"
#include "Steam/steam_api.h"

class LobbyManager : public OnlineManager
{
public:
	LobbyManager();
	~LobbyManager();

	void Update() override;
	bool IsCurrentLobbyValid();
	CSteamID GetCurrentLobby() { return m_currentLobby; }
	void InviteFriendToCurrentLobby(const CSteamID playerId);
	void LeaveCurrentLobby();

	std::vector<std::string> GetCurrentLobbyPlayerNameList();

	void OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);
	void OnLobbyJoined(LobbyEnter_t *pCallback, bool bIOFailure);

private:
	STEAM_CALLBACK_MANUAL(LobbyManager, HandleLobbyJoinRequested, GameLobbyJoinRequested_t, LobbyJoinRequestedCallback);

private:
	CSteamID m_currentLobby;
	CCallResult<LobbyManager, LobbyCreated_t> m_SteamCallResultLobbyCreated;
	CCallResult<LobbyManager, LobbyEnter_t> m_SteamCallResultLobbyJoined;

	std::vector<CSteamID> m_inviteBuffer;
};

#define popGetLobbyManager() LobbyManager::GetInstancePtr<LobbyManager>()