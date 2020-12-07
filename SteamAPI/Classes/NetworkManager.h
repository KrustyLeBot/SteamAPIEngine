#pragma once
#include "Classes/OnlineManager.h"
#include "Steam/steam_api.h"

//Network Data Structures
////////////////////////////////////////////////////////////////
enum DataStructuresChannelEnum
{
	SampleMessageDataChannel = 0,
	BackgroundColorDataChannel = 1
};

struct SampleMessageData
{
	std::string m_message;
};

struct BackgroundColorData
{
	int R = 0;
	int G = 0;
	int B = 0;
};
////////////////////////////////////////////////////////////////

class NetworkManager : public OnlineManager
{
public:
	NetworkManager();
	~NetworkManager();

	void Update() override;

	template<class T>
	void SendDataToUser(CSteamID recipientId, T data, int nSendFlags)
	{
		SteamNetworkingIdentity recipientNetworkingIdentity;
		recipientNetworkingIdentity.SetSteamID(recipientId);

		EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, &data, sizeof(data), nSendFlags, DataStructuresChannelEnum::SampleMessageDataChannel);
	}

	template<class T>
	void SendDataToAllLobby(T data, int nSendFlags)
	{
		std::vector<CSteamID> recipients = popGetLobbyManager()->GetCurrentLobbyPlayerList();
		for (const CSteamID id : recipients)
		{
			SteamNetworkingIdentity recipientNetworkingIdentity;
			recipientNetworkingIdentity.SetSteamID(id);

			EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, &data, sizeof(data), nSendFlags, DataStructuresChannelEnum::BackgroundColorDataChannel);
		}
	}

	std::vector<std::pair<CSteamID, std::string>>& GetIncomingMessageQueue() { return m_incomingMessageQueue; }

private:
	void HandleIncomingMessage(SteamNetworkingMessage_t* ppOutMessage, int channel);

	STEAM_CALLBACK_MANUAL(NetworkManager, HandleNetworkingMessagesErrors, SteamNetworkingMessagesSessionFailed_t, SteamNetworkingMessagesErrorCallback);
	STEAM_CALLBACK_MANUAL(NetworkManager, HandleNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t, SteamNetworkingMessagesSessionRequestCallback);

private:
	std::vector<std::pair<CSteamID, std::string>> m_incomingMessageQueue;
};

#define popGetNetworkManager() NetworkManager::GetInstancePtr<NetworkManager>()