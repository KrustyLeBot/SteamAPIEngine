#include "LobbyManager.h"
#include <windows.h>

LobbyManager::LobbyManager()
{
	m_currentLobby = CSteamID();
	m_SteamCallResultLobbyCreated.Cancel();
	m_SteamCallResultLobbyJoined.Cancel();
}

LobbyManager::~LobbyManager()
{
}

void LobbyManager::Update()
{
}

bool LobbyManager::IsCurrentLobbyValid()
{
	return m_currentLobby.ConvertToUint64() != 0;
}

void LobbyManager::InviteFriendToCurrentLobby(const CSteamID playerId)
{
	if (IsCurrentLobbyValid())
	{
		//Invite friend to lobby
		bool result = SteamMatchmaking()->InviteUserToLobby(m_currentLobby, playerId);
		if (result)
		{
			OutputDebugString("Friend invited\n");
		}
		else
		{
			OutputDebugString("Friend invite failed\n");
		}
	}
	else if(!m_SteamCallResultLobbyCreated.IsActive())
	{
		// If you are not in a lobby and one is not currently being created, create one
		OutputDebugString("Not in a lobby, creating one\n");
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(k_ELobbyTypePrivate, 4);
		m_SteamCallResultLobbyCreated.Set(hSteamAPICall, this, &LobbyManager::OnLobbyCreated);

		//Add to buffer the invite to send it when the lobby will be created
		m_inviteBuffer.push_back(playerId);
	}
}

void LobbyManager::OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_eResult == k_EResultOK)
	{
		OutputDebugString("Lobby created\n");
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(pCallback->m_ulSteamIDLobby);
		m_SteamCallResultLobbyJoined.Set(hSteamAPICall, this, &LobbyManager::OnLobbyJoined);
	}
}

void LobbyManager::OnLobbyJoined(LobbyEnter_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess)
	{
		OutputDebugString("Lobby joined\n");
		m_currentLobby = pCallback->m_ulSteamIDLobby;

		//Handle invites that need to be send now that we are in a lobby
		for (const CSteamID playerId : m_inviteBuffer)
		{
			InviteFriendToCurrentLobby(playerId);
		}
		m_inviteBuffer.clear();
	}
}