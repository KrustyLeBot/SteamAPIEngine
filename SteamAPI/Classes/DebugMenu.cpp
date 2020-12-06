#include "DebugMenu.h"

#include <sstream>

#include "Classes/FriendsManager.h"
#include "Classes/LobbyManager.h"
#include "Classes/NetworkManager.h"

void DrawMainDebugMenu()
{
	ImGui::Begin("Main Debug Menu");

	//Handle the list of friends
	static std::pair<CSteamID, std::string> currentItemFriendList(CSteamID(), "");
	std::vector<std::pair<CSteamID, std::string>>& friendList = popGetFriendsManager()->GetFriendList();

	if (ImGui::BeginCombo("##combo", currentItemFriendList.second.c_str()))
	{
		for (const auto friendInfo : friendList)
		{
			bool is_selected = (currentItemFriendList.first == friendInfo.first);
			if (ImGui::Selectable(friendInfo.second.c_str(), is_selected))
				currentItemFriendList = friendInfo;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	//Send invite to a friend
	if (ImGui::Button("Invite friend") && currentItemFriendList.first != CSteamID())
	{
		popGetLobbyManager()->InviteFriendToCurrentLobby(currentItemFriendList.first);
	}

	//Send message to a friend
	static char buf[128];
	ImGui::InputText("", buf, 128);
	if (ImGui::Button("Send message") && currentItemFriendList.first != CSteamID())
	{
		NetworkManager::SampleMessageDataStructure data;
		data.m_message = buf;
		popGetNetworkManager()->SendDataToUser<NetworkManager::SampleMessageDataStructure>(currentItemFriendList.first, data);
	}

	ImGui::End();
}

void DrawLobbyDebugMenu()
{
	ImGui::Begin("Lobby Debug Menu");

	//Show current lobby id
	std::stringstream  stringStream;
	stringStream << "Current lobby id: " << popGetLobbyManager()->GetCurrentLobby().ConvertToUint64();
	ImGui::Text(stringStream.str().c_str());

	//Leave current lobby
	if (ImGui::Button("Leave lobby"))
	{
		popGetLobbyManager()->LeaveCurrentLobby();
	}

	//Show all player in the lobby
	std::vector<std::string> playerNameList = popGetLobbyManager()->GetCurrentLobbyPlayerNameList();
	for (const auto playerName : playerNameList)
	{
		ImGui::Text(playerName.c_str());
	}

	ImGui::End();
}

void DrawMessageQueueDebugMenu()
{
	ImGui::Begin("Message Queue Debug Menu");

	//Show all player in the lobby
	std::vector<std::pair<CSteamID, std::string>>& messageQueue = popGetNetworkManager()->GetIncomingMessageQueue();
	for (const auto message : messageQueue)
	{
		ImGui::Text("Player name: ");
		ImGui::SameLine();
		ImGui::Text(SteamFriends()->GetFriendPersonaName(message.first));
		ImGui::SameLine();
		ImGui::Text(", Message: ");
		ImGui::SameLine();
		ImGui::Text(message.second.c_str());
	}

	ImGui::End();
}