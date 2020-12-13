#pragma once
#include <vector>
#include "Classes/Singleton.h"
#include "Classes/NetworkManager.h"
#include "SFML/Graphics.hpp"
#include "Steam/steam_api.h"

class GameManager : public Singleton
{
public:
	GameManager();
	~GameManager();

	void Update() override;
	void SetRenderWindow(sf::RenderWindow* renderWindow) { m_renderWindow = renderWindow; }
	void SetBackgroundData(BackgroundColorData data) { m_bgData = data; }
	void SetBackgroundData(int R, int G, int B) { m_bgData.R = R; m_bgData.G = G; m_bgData.B = B; }
	void StartGame();
	void StopGame();
	bool IsGameStarted() { return m_gameStarted; }
	bool IsGameStarting() { return m_gameStarting; }
	void SetPlayerPosition(PlayerPositionData data);
	PlayerPositionData& GetPlayerPosition() { return m_position; }
	PlayerPositionData GetPlayerPosition(CSteamID player) { return m_lobbyPlayerPositions[player.ConvertToUint64()]; }
	std::unordered_map<uint64, PlayerPositionData>& GetLobbyPlayerPositions() { return m_lobbyPlayerPositions; }
	BackgroundColorData& GetBackgroundData() { return m_bgData; }

private:
	void SetGameStarting(std::string value);
	void UpdatePlayerPosition();
	void DrawStaticElements();
	void DrawDynamicElements();

private:
	sf::RenderWindow* m_renderWindow;
	sf::Font m_font;
	BackgroundColorData m_bgData;
	bool m_gameStarted;
	bool m_gameStarting;
	CpuTimer m_startGameTimer;
	std::string m_remainingTime;

	PlayerPositionData m_position;
	std::unordered_map<uint64, PlayerPositionData> m_lobbyPlayerPositions;
};

#define popGetGameManager() GameManager::GetInstancePtr<GameManager>()