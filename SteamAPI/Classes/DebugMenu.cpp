#include "DebugMenu.h"

#include <sstream>

#include "Classes/FriendsManager.h"
#include "Classes/LobbyManager.h"

void DrawMenu()
{
	ImGui::Begin("Debug Menu");

	std::stringstream  stringStream;
	stringStream << "Current lobby id: " << popGetLobbyManager()->GetCurrentLobby().ConvertToUint64();
	ImGui::Text(stringStream.str().c_str());

	static std::pair<CSteamID, std::string> current_item(CSteamID(), "");
	std::vector<std::pair<CSteamID, std::string>>& friendList = popGetFriendsManager()->GetFriendList();

	if (ImGui::BeginCombo("##combo", current_item.second.c_str())) // The second parameter is the label previewed before opening the combo.
	{
		for (const auto friendInfo : friendList)
		{
			bool is_selected = (current_item.first == friendInfo.first); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(friendInfo.second.c_str(), is_selected))
				current_item = friendInfo;
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	if (ImGui::Button("Invite friend") && current_item.first != CSteamID())
	{
		popGetLobbyManager()->InviteFriendToCurrentLobby(current_item.first);
	}

	ImGui::End();
}