#pragma once
#include "Classes/Singleton.h"
#include "Steam/steam_api.h"

#include "Classes/LobbyManager.h"

//Network Data Structures
////////////////////////////////////////////////////////////////
enum DataStructuresChannelEnum
{
	MessageDataChannel = 0,
	BackgroundColorDataChannel = 1,
	PlayerPositionDataChannel = 2,
	ChannelCount = 3
};

struct BackgroundColorData
{
	int R = 0;
	int G = 0;
	int B = 0;
};

struct PlayerPositionData
{
	CSteamID playerId = CSteamID();
	int sprite = 0;
	int posX = 0;
	int posY = 0;
	int posZ = 0;
};
////////////////////////////////////////////////////////////////

class NetworkManager : public Singleton
{
public:
	NetworkManager();
	~NetworkManager();

	void Update() override;

	void SendDataToUser(CSteamID recipientId, std::string message);
	void SendDataToAllLobby(BackgroundColorData data);
	void SendPlayerPositionDataToAllLobby(std::unordered_map<uint64, PlayerPositionData> data);
	void SendCurrentPlayerPositionDataToLobbyOwner(PlayerPositionData data);

	std::vector<std::pair<CSteamID, std::string>>& GetIncomingMessageQueue() { return m_incomingMessageQueue; }

private:
	void HandleIncomingMessage(SteamNetworkingMessage_t* ppOutMessage, int channel);

	STEAM_CALLBACK_MANUAL(NetworkManager, HandleNetworkingMessagesErrors, SteamNetworkingMessagesSessionFailed_t, SteamNetworkingMessagesErrorCallback);
	STEAM_CALLBACK_MANUAL(NetworkManager, HandleNetworkingMessagesSessionRequest, SteamNetworkingMessagesSessionRequest_t, SteamNetworkingMessagesSessionRequestCallback);

private:
	std::vector<std::pair<CSteamID, std::string>> m_incomingMessageQueue;
};

#define popGetNetworkManager() NetworkManager::GetInstancePtr<NetworkManager>()