#include "LobbyManager.h"

LobbyManager::LobbyManager()
{
	m_currentLobby = CSteamID();
	m_SteamCallResultLobbyCreated.Cancel();
	m_SteamCallResultLobbyJoined.Cancel();

	LobbyJoinRequestedCallback.Register(this, &LobbyManager::HandleLobbyJoinRequested);
}

LobbyManager::~LobbyManager()
{
	LobbyJoinRequestedCallback.Unregister();
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
		}
		else
		{
		}
	}
	else if(!m_SteamCallResultLobbyCreated.IsActive())
	{
		// If you are not in a lobby and one is not currently being created, create one
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(k_ELobbyTypePrivate, 4);
		m_SteamCallResultLobbyCreated.Set(hSteamAPICall, this, &LobbyManager::OnLobbyCreated);

		//Add to buffer the invite to send it when the lobby will be created
		m_inviteBuffer.push_back(playerId);
	}
}

void LobbyManager::LeaveCurrentLobby()
{
	if (IsCurrentLobbyValid())
	{
		SteamMatchmaking()->LeaveLobby(m_currentLobby);
		m_currentLobby = CSteamID();
	}
}

std::vector<CSteamID> LobbyManager::GetCurrentLobbyPlayerList()
{
	std::vector<CSteamID> playerList;
	if (IsCurrentLobbyValid())
	{
		int lobbyPlayerCount = SteamMatchmaking()->GetNumLobbyMembers(m_currentLobby);
		for (int index = 0; index < lobbyPlayerCount; index++)
		{
			CSteamID id = SteamMatchmaking()->GetLobbyMemberByIndex(m_currentLobby, index);
			if (id != SteamUser()->GetSteamID())
			{
				playerList.push_back(id);
			}
		}
		
	}
	return playerList;
}

bool LobbyManager::IsLocalPlayerCurrentLobbyOwner()
{
	if (IsCurrentLobbyValid())
	{
		return SteamUser()->GetSteamID() == SteamMatchmaking()->GetLobbyOwner(m_currentLobby);
	}
	return false;
}

void LobbyManager::OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_eResult == k_EResultOK)
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(pCallback->m_ulSteamIDLobby);
		m_SteamCallResultLobbyJoined.Set(hSteamAPICall, this, &LobbyManager::OnLobbyJoined);
	}
}

void LobbyManager::OnLobbyJoined(LobbyEnter_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess)
	{
		m_currentLobby = pCallback->m_ulSteamIDLobby;

		//Handle invites that need to be send now that we are in a lobby
		for (const CSteamID playerId : m_inviteBuffer)
		{
			InviteFriendToCurrentLobby(playerId);
		}
		m_inviteBuffer.clear();
	}
}

void LobbyManager::HandleLobbyJoinRequested(GameLobbyJoinRequested_t *pParam)
{
	if (pParam != nullptr)
	{
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(pParam->m_steamIDLobby);
		m_SteamCallResultLobbyJoined.Set(hSteamAPICall, this, &LobbyManager::OnLobbyJoined);
	}
}