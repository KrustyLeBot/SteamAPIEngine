#include "DebugMenu.h"
#include <sstream>
#include "Classes/FriendsManager.h"
#include "Classes/LobbyManager.h"
#include "Classes/NetworkManager.h"
#include "Classes/GameManager.h"

static char buf[128];
static std::pair<CSteamID, std::string> currentItemFriendList(CSteamID(), "Select a friend");

void DrawMainDebugMenu()
{
	ImGui::Begin("Main Debug Menu");

	//Handle the list of friends
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
	ImGui::InputText("", buf, 128);
	if (ImGui::Button("Send message") && currentItemFriendList.first != CSteamID())
	{
		popGetNetworkManager()->SendDataToUser(currentItemFriendList.first, std::string(buf));
	}

	ImGui::End();
}


static int R = 0;
static int G = 0;
static int B = 0;

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
		ImGui::SliderInt("Red", &R, 0, 255);
		ImGui::SliderInt("Green", &G, 0, 255);
		ImGui::SliderInt("Blue", &B, 0, 255);

		popGetGameManager()->SetBackgroundData(R, G, B);

		//Start and Stop game
		if (ImGui::Button("START GAME"))
		{
			popGetGameManager()->StartGame();
		}
		ImGui::SameLine();
		if (ImGui::Button("STOP GAME"))
		{
			popGetGameManager()->StopGame();
		}
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