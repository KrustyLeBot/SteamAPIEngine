#include "FriendsManager.h"

FriendsManager::FriendsManager()
{
}

FriendsManager::~FriendsManager()
{
	m_friendList.clear();
}

void FriendsManager::Update()
{
	if (!m_timer.IsStarted() || m_timer.GetElapsedTimeMs() >= 1000 * 60 * 2)
	{
		int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);

		m_friendList.clear();

		for (int index = 0; index < friendCount; index++)
		{
			CSteamID id = SteamFriends()->GetFriendByIndex(index, k_EFriendFlagImmediate);
			std::string name = SteamFriends()->GetFriendPersonaName(id);
			m_nameCache[id.ConvertToUint64()] = name;
			m_friendList.push_back(std::make_pair(id, name));
		}

		m_timer.Reset();
		m_timer.Start();
	}
}

bool FriendsManager::IsFriend(CSteamID playerId)
{
	return SteamFriends()->HasFriend(playerId, k_EFriendFlagImmediate);
}

void FriendsManager::AddPlayerToCache(CSteamID playerId)
{
	std::string name = SteamFriends()->GetFriendPersonaName(playerId);
	if (!name.empty())
	{
		m_nameCache[playerId.ConvertToUint64()] = name;
	}
}

std::string FriendsManager::GetPlayerName(CSteamID playerId)
{
	if (m_nameCache.find(playerId.ConvertToUint64()) == m_nameCache.end())
	{
		AddPlayerToCache(playerId);
		return "Unknown Soldier";
	}
	return m_nameCache[playerId.ConvertToUint64()];
}