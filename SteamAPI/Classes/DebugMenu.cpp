#include "DebugMenu.h"

#include <sstream>

#include "Classes/FriendsManager.h"
#include "Classes/LobbyManager.h"
#include "Classes/NetworkManager.h"
#include "Classes/GameManager.h"

void DrawMainDebugMenu()
{
	ImGui::Begin("Main Debug Menu");

	//Handle the list of friends
	static std::pair<CSteamID, std::string> currentItemFriendList(CSteamID(), "Select a friend");
	std::vector<std::pair<CSteamID, std::string>>& friendList = popGetFriendsManager()->GetFriendList();

	std::stringstream hinStringStream;
	hinStringStream << currentItemFriendList.second << " - " << currentItemFriendList.first.ConvertToUint64();

	if (ImGui::BeginCombo("##combo", hinStringStream.str().c_str()))
	{
		for (auto friendInfo : friendList)
		{
			bool is_selected = (currentItemFriendList.first == friendInfo.first);
			std::stringstream stringStream;
			stringStream << friendInfo.second << " - " << friendInfo.first.ConvertToUint64();
			if (ImGui::Selectable(stringStream.str().c_str(), is_selected))
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
	std::stringstream stringStream;
	stringStream << "Current lobby id: " << popGetLobbyManager()->GetCurrentLobby().ConvertToUint64();
	ImGui::Text(stringStream.str().c_str());

	//Leave current lobby
	if (popGetLobbyManager()->IsCurrentLobbyValid() && ImGui::Button("Leave lobby"))
	{
		popGetLobbyManager()->LeaveCurrentLobby();
	}

	//Show all player in the lobby
	std::vector<CSteamID> playerList = popGetLobbyManager()->GetCurrentLobbyPlayerList();
	for (const auto player : playerList)
	{
		ImGui::Text(SteamFriends()->GetFriendPersonaName(player));
	}

	//If we are lobby owner => show slider for background color
	if (popGetLobbyManager()->IsLocalPlayerCurrentLobbyOwner())
	{
		static int R;
		static int G;
		static int B;

		ImGui::SliderInt("Red", &R, 0, 255);
		ImGui::SliderInt("Green", &G, 0, 255);
		ImGui::SliderInt("Blue", &B, 0, 255);

		NetworkManager::BackgroundColorData data;
		data.R = R;
		data.G = G;
		data.B = B;

		popGetGameManager()->SetBackgroundData(data);
		popGetNetworkManager()->SendDataToAllLobby<NetworkManager::BackgroundColorData>(data);
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