#pragma once
#include <vector>
#include "Classes/Singleton.h"
#include "Steam/steam_api.h"

#define LOBBY_MAX_PLAYERS 4

class LobbyManager : public Singleton
{
public:
	LobbyManager();
	~LobbyManager();

	void Update() override;
	bool IsCurrentLobbyValid();
	CSteamID& GetCurrentLobby() { return m_currentLobby; }
	void InviteFriendToCurrentLobby(CSteamID playerId);
	void LeaveCurrentLobby();
	std::vector<CSteamID>& GetCurrentLobbyPlayerList() { return m_playerList; }
	bool IsLocalPlayerCurrentLobbyOwner();
	bool IsPlayerInCurrentLobby(CSteamID playerId);
	void SetLobbyMetadata(std::string key, std::string value);
	std::string GetLobbyMetadata(std::string key);

	void OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);
	void OnLobbyJoined(LobbyEnter_t *pCallback, bool bIOFailure);

private:
	STEAM_CALLBACK_MANUAL(LobbyManager, HandleLobbyJoinRequested, GameLobbyJoinRequested_t, LobbyJoinRequestedCallback);

private:
	CSteamID m_currentLobby;
	CCallResult<LobbyManager, LobbyCreated_t> m_SteamCallResultLobbyCreated;
	CCallResult<LobbyManager, LobbyEnter_t> m_SteamCallResultLobbyJoined;

	std::vector<CSteamID> m_inviteBuffer;
	std::vector<CSteamID> m_playerList;
};

#define popGetLobbyManager() LobbyManager::GetInstancePtr<LobbyManager>()