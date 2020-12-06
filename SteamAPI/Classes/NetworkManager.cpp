#include "NetworkManager.h"
#include <windows.h>

#include "Classes/FriendsManager.h"
#include "Classes/GameManager.h"

#define NUMBER_MESSAGE_HANDLED_PER_UPDATE 5

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
	SteamNetworkingMessage_t** ppOutMessages = static_cast<SteamNetworkingMessage_t**>(malloc(sizeof(SteamNetworkingMessage_t*) * NUMBER_MESSAGE_HANDLED_PER_UPDATE));
	int ppOutMessagesCount = SteamNetworkingMessages()->ReceiveMessagesOnChannel(0, ppOutMessages, NUMBER_MESSAGE_HANDLED_PER_UPDATE);

	for (int index = 0; index < min(ppOutMessagesCount,NUMBER_MESSAGE_HANDLED_PER_UPDATE); index++)
	{
		if (ppOutMessages[index] != nullptr)
		{
			HandleIncomingMessage(ppOutMessages[index]);
		}
	}
	free(ppOutMessages);
}

void NetworkManager::HandleIncomingMessage(SteamNetworkingMessage_t* ppOutMessage)
{
	if (ppOutMessage->GetSize() == sizeof(SampleMessageDataStructure))
	{
		SampleMessageDataStructure * convertedData = (SampleMessageDataStructure*)ppOutMessage->GetData();
		m_incomingMessageQueue.push_back(std::make_pair(ppOutMessage->m_identityPeer.GetSteamID(), convertedData->m_message));
	}
	else if (ppOutMessage->GetSize() == sizeof(BackgroundColorData))
	{
		popGetGameManager()->SetBackgroundData(*(BackgroundColorData*)ppOutMessage->GetData());
	}
	ppOutMessage->Release();
}

void NetworkManager::HandleNetworkingMessagesErrors(SteamNetworkingMessagesSessionFailed_t *pParam)
{
	OutputDebugString("Message session failed\n");
}

void NetworkManager::HandleNetworkingMessagesSessionRequest(SteamNetworkingMessagesSessionRequest_t *pParam)
{
	OutputDebugString("Incoming session request\n");
	//Only accept sessions with friends
	if (pParam != nullptr && popGetFriendsManager()->IsFriend(pParam->m_identityRemote.GetSteamID()))
	{
		SteamNetworkingMessages()->AcceptSessionWithUser(pParam->m_identityRemote);
	}
}