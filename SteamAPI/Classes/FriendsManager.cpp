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
	if (!m_timer.IsStarted() || m_timer.GetElapsedTimeMs() > 1000 * 60 * 2)
	{
		int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);

		m_friendList.clear();

		for (int index = 0; index < friendCount; index++)
		{
			CSteamID id = SteamFriends()->GetFriendByIndex(index, k_EFriendFlagImmediate);
			std::string name = SteamFriends()->GetFriendPersonaName(id);
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