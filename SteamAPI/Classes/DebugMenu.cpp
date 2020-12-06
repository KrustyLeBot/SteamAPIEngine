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

	if (ImGui::BeginCombo("##combo", currentItemFriendList.second.c_str())) // The second parameter is the label previewed before opening the combo.
	{
		for (const auto friendInfo : friendList)
		{
			bool is_selected = (currentItemFriendList.first == friendInfo.first); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(friendInfo.second.c_str(), is_selected))
				currentItemFriendList = friendInfo;
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
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

	//Handle message received queue
	static std::pair<CSteamID, std::string> currentItemMessageQueue(CSteamID(), "");
	std::vector<std::pair<CSteamID, std::string>>& messageQueue = popGetNetworkManager()->GetIncomingMessageQueue();

	if (ImGui::BeginCombo("##combo", currentItemMessageQueue.second.c_str())) // The second parameter is the label previewed before opening the combo.
	{
		for (const auto friendInfo : friendList)
		{
			bool is_selected = (currentItemMessageQueue.first == friendInfo.first); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(friendInfo.second.c_str(), is_selected))
				currentItemMessageQueue = friendInfo;
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
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