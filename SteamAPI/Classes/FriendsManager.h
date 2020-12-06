#pragma once
#include <vector>
#include "Classes/OnlineManager.h"
#include "Steam/steam_api.h"

class FriendsManager : public OnlineManager
{
public:
	FriendsManager();
	~FriendsManager();

	void Update() override;
	bool IsFriend(CSteamID playerId);

	std::vector<std::pair<CSteamID, std::string>>& GetFriendList() { return m_friendList; }

private:
	std::vector<std::pair<CSteamID, std::string>> m_friendList;
};

#define popGetFriendsManager() FriendsManager::GetInstancePtr<FriendsManager>()