#pragma once
#include <vector>
#include "Classes/Singleton.h"
#include "Steam/steam_api.h"

class FriendsManager : public Singleton
{
public:
	FriendsManager();
	~FriendsManager();

	void Update() override;
	bool IsFriend(CSteamID playerId);
	void AddPlayerToCache(CSteamID playerId);
	std::string GetPlayerName(CSteamID playerId);

	std::vector<std::pair<CSteamID, std::string>>& GetFriendList() { return m_friendList; }

private:
	std::vector<std::pair<CSteamID, std::string>> m_friendList;
	std::unordered_map<uint64, std::string> m_nameCache;
};

#define popGetFriendsManager() FriendsManager::GetInstancePtr<FriendsManager>()