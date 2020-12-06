#pragma once
#include "Classes/OnlineManager.h"
#include "Steam/steam_api.h"

class NetworkManager : public OnlineManager
{
public:
	NetworkManager();
	~NetworkManager();

	struct SampleMessageDataStructure
	{
		std::string m_message;
	};

	void Update() override;

	template<class T>
	bool SendDataToUser(CSteamID recipientId, T data)
	{
		SteamNetworkingIdentity recipientNetworkingIdentity;
		recipientNetworkingIdentity.SetSteamID(recipientId);

		EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, &data, sizeof(data), k_EP2PSendUnreliable, 0);
		return result == k_EResultOK;
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