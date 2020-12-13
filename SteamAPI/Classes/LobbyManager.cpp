#include "LobbyManager.h"
#include <windows.h>
#include "Classes/FriendsManager.h"
#include "Classes/GameManager.h"

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
	if (IsCurrentLobbyValid() && (!m_timer.IsStarted() || m_timer.GetElapsedTimeMs() >= 1000 * 2))
	{
		int lobbyPlayerCount = SteamMatchmaking()->GetNumLobbyMembers(m_currentLobby);
		m_playerList.clear();
		for (int index = 0; index < lobbyPlayerCount; index++)
		{
			CSteamID id = SteamMatchmaking()->GetLobbyMemberByIndex(m_currentLobby, index);
			m_playerList.push_back(id);
			popGetFriendsManager()->AddPlayerToCache(id);
		}

		m_timer.Reset();
		m_timer.Start();
	}
}

bool LobbyManager::IsCurrentLobbyValid()
{
	return m_currentLobby.ConvertToUint64() != 0;
}

void LobbyManager::InviteFriendToCurrentLobby(CSteamID playerId)
{
	if (IsCurrentLobbyValid())
	{
		//Invite friend to lobby
		bool result = SteamMatchmaking()->InviteUserToLobby(m_currentLobby, playerId);
		OutputDebugString(result ? "Invite sent\n" : "Invite failed");
	}
	else if(!m_SteamCallResultLobbyCreated.IsActive())
	{
		// If you are not in a lobby and one is not currently being created, create one
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(k_ELobbyTypePrivate, LOBBY_MAX_PLAYERS);
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

bool LobbyManager::IsLocalPlayerCurrentLobbyOwner()
{
	if (IsCurrentLobbyValid())
	{
		return SteamUser()->GetSteamID() == SteamMatchmaking()->GetLobbyOwner(m_currentLobby);
	}
	return false;
}

bool LobbyManager::IsPlayerInCurrentLobby(CSteamID playerId)
{
	if (IsCurrentLobbyValid() && std::find(m_playerList.begin(), m_playerList.end(), playerId) != m_playerList.end())
	{
		return true;
	}
	return false;
}

void LobbyManager::SetLobbyMetadata(std::string key, std::string value)
{
	SteamMatchmaking()->SetLobbyData(m_currentLobby, key.c_str(), value.c_str());
}

std::string LobbyManager::GetLobbyMetadata(std::string key)
{
	return SteamMatchmaking()->GetLobbyData(m_currentLobby, key.c_str());
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
	else
	{
		LeaveCurrentLobby();
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