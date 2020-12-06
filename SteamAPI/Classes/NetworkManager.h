#pragma once
#include "Classes/OnlineManager.h"
#include "Steam/steam_api.h"

class NetworkManager : public OnlineManager
{
public:
	NetworkManager();
	~NetworkManager();

	void Update() override;

	//Network Data Structures
	////////////////////////////////////////////////////////////////
	struct SampleMessageDataStructure
	{
		std::string m_message = "";
	};

	struct BackgroundColorData
	{
		int R = 0;
		int G = 0;
		int B = 0;
	};
	////////////////////////////////////////////////////////////////

	template<class T>
	void SendDataToUser(CSteamID recipientId, T data)
	{
		SteamNetworkingIdentity recipientNetworkingIdentity;
		recipientNetworkingIdentity.SetSteamID(recipientId);

		EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, &data, sizeof(data), k_EP2PSendUnreliable, 0);
	}

	template<class T>
	void SendDataToAllLobby(T data)
	{
		std::vector<CSteamID> recipients = popGetLobbyManager()->GetCurrentLobbyPlayerList();
		for (const CSteamID id : recipients)
		{
			SteamNetworkingIdentity recipientNetworkingIdentity;
			recipientNetworkingIdentity.SetSteamID(id);

			EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, &data, sizeof(data), k_EP2PSendUnreliable, 0);
		}
	}

	std::vector<std::pair<CSteamID, std::string>>& GetIncomingMessageQueue() { return m_incomingMessageQueue; }

private:
	void HandleIncomingMessage(SteamNetworkingMessage_t* ppOutMessage);

	STEAM_CALLBACK_MANUAL(NetworkManager, HandleNetworkingMessagesErrors, SteamNetworkingMessagesSessionFailed_t, SteamNetworkingMessagesErrorCallback);
	STEAM_CALLBACK_MANUAL(NetworkManager, HandleNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t, SteamNetworkingMessagesSessionRequestCallback);

private:
	std::vector<std::pair<CSteamID, std::string>> m_incomingMessageQueue;
};

#define popGetNetworkManager() NetworkManager::GetInstancePtr<NetworkManager>()