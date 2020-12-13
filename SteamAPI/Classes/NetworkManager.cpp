#include "NetworkManager.h"
#include <windows.h>
#include "Classes/FriendsManager.h"
#include "Classes/GameManager.h"

#define NUMBER_MESSAGE_HANDLED_PER_UPDATE 2

static int previous_timestamp = 0;

NetworkManager::NetworkManager()
{
	SteamNetworkingMessagesErrorCallback.Register(this, &NetworkManager::HandleNetworkingMessagesErrors);
	SteamNetworkingMessagesSessionRequestCallback.Register(this, &NetworkManager::HandleNetworkingMessagesSessionRequest);
}

NetworkManager::~NetworkManager()
{
	SteamNetworkingMessagesErrorCallback.Unregister();
	SteamNetworkingMessagesSessionRequestCallback.Unregister();
}

void NetworkManager::Update()
{
	for (int channel = 0; channel < DataStructuresChannelEnum::ChannelCount; channel++)
	{
		SteamNetworkingMessage_t** ppOutMessages = static_cast<SteamNetworkingMessage_t**>(malloc(sizeof(SteamNetworkingMessage_t*) * NUMBER_MESSAGE_HANDLED_PER_UPDATE));
		int ppOutMessagesCount = SteamNetworkingMessages()->ReceiveMessagesOnChannel(channel, ppOutMessages, NUMBER_MESSAGE_HANDLED_PER_UPDATE);

		for (int index = 0; index < min(ppOutMessagesCount, NUMBER_MESSAGE_HANDLED_PER_UPDATE); index++)
		{
			if (ppOutMessages[index] != nullptr)
			{
				HandleIncomingMessage(ppOutMessages[index], channel);
			}
		}
		free(ppOutMessages);
	}
}

void NetworkManager::HandleIncomingMessage(SteamNetworkingMessage_t* ppOutMessage, int channel)
{
	switch (channel)
	{
	case DataStructuresChannelEnum::MessageDataChannel:
	{
		char* convertedData = (char*)ppOutMessage->GetData();
		m_incomingMessageQueue.push_back(std::make_pair(ppOutMessage->m_identityPeer.GetSteamID(), std::string(convertedData, ppOutMessage->GetSize()/sizeof(char))));
		break;
	}

	case DataStructuresChannelEnum::BackgroundColorDataChannel:
	{
		BackgroundColorData convertedData = *(BackgroundColorData*)ppOutMessage->GetData();
		popGetGameManager()->SetBackgroundData(convertedData);
		break;
	}

	case DataStructuresChannelEnum::PlayerPositionDataChannel:
	{
		int length = ppOutMessage->GetSize()/sizeof(PlayerPositionData);
		PlayerPositionData* posList = (PlayerPositionData*)ppOutMessage->GetData();
		for (int index = 0; index < length; index++)
		{
			popGetGameManager()->SetPlayerPosition(posList[index]);
		}
		break;
	}

	default:
		break;
	}
	ppOutMessage->Release();
}

void NetworkManager::SendDataToUser(CSteamID recipientId, std::string message)
{
	SteamNetworkingIdentity recipientNetworkingIdentity;
	recipientNetworkingIdentity.SetSteamID(recipientId);

	EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, message.c_str(), message.length(), k_EP2PSendReliable, DataStructuresChannelEnum::MessageDataChannel);
}

void NetworkManager::SendDataToAllLobby(BackgroundColorData data)
{
	std::vector<CSteamID>& recipients = popGetLobbyManager()->GetCurrentLobbyPlayerList();
	for (const CSteamID id : recipients)
	{
		if (id != SteamUser()->GetSteamID())
		{
			SteamNetworkingIdentity recipientNetworkingIdentity;
			recipientNetworkingIdentity.SetSteamID(id);
			EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, &data, sizeof(BackgroundColorData), k_EP2PSendUnreliable, DataStructuresChannelEnum::BackgroundColorDataChannel);
		}
	}
}

void NetworkManager::SendPlayerPositionDataToAllLobby(std::unordered_map<uint64, PlayerPositionData> data)
{
	const int length = data.size();
	
	PlayerPositionData* allocatedMemory = (PlayerPositionData*)malloc(sizeof(PlayerPositionData) * length);
	int counter = 0;
	for (const auto& pos : data)
	{
		allocatedMemory[counter] = pos.second;
		counter++;
	}

	int test = sizeof(PlayerPositionData);

	std::vector<CSteamID>& recipients = popGetLobbyManager()->GetCurrentLobbyPlayerList();
	for (const CSteamID id : recipients)
	{
		if (id != SteamUser()->GetSteamID())
		{
			SteamNetworkingIdentity recipientNetworkingIdentity;
			recipientNetworkingIdentity.SetSteamID(id);

			EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, allocatedMemory, sizeof(PlayerPositionData) * length, k_EP2PSendUnreliable, DataStructuresChannelEnum::PlayerPositionDataChannel);
		}
	}
	free(allocatedMemory);
}

void NetworkManager::SendCurrentPlayerPositionDataToLobbyOwner(PlayerPositionData data)
{
	SteamNetworkingIdentity recipientNetworkingIdentity;
	recipientNetworkingIdentity.SetSteamID(SteamMatchmaking()->GetLobbyOwner(popGetLobbyManager()->GetCurrentLobby()));

	EResult result = SteamNetworkingMessages()->SendMessageToUser(recipientNetworkingIdentity, &data, sizeof(PlayerPositionData), k_EP2PSendReliable, DataStructuresChannelEnum::PlayerPositionDataChannel);
}

void NetworkManager::HandleNetworkingMessagesErrors(SteamNetworkingMessagesSessionFailed_t *pParam)
{
	OutputDebugString("Message session failed\n");
	SteamNetworkingMessages()->CloseSessionWithUser(pParam->m_info.m_identityRemote);
}

void NetworkManager::HandleNetworkingMessagesSessionRequest(SteamNetworkingMessagesSessionRequest_t *pParam)
{
	OutputDebugString("Incoming session request\n");
	//Only accept sessions with friend or lobby members
	if (pParam != nullptr && (popGetFriendsManager()->IsFriend(pParam->m_identityRemote.GetSteamID()) || popGetLobbyManager()->IsPlayerInCurrentLobby(pParam->m_identityRemote.GetSteamID())))
	{
		SteamNetworkingMessages()->AcceptSessionWithUser(pParam->m_identityRemote);
	}
}