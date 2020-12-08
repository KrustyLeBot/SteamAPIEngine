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
	void DrawStaticElements();
	void SetBackgroundData(BackgroundColorData data) { m_bgData = data; }
	void StartGame();
	void StopGame();
	bool IsGameStarted() { return m_gameStarted; }
	bool IsGameStarting() { return m_gameStarting; }

private:
	void SetGameStarting(std::string value);

private:
	sf::RenderWindow* m_renderWindow;
	sf::Font m_font;
	BackgroundColorData m_bgData;
	bool m_gameStarted;
	bool m_gameStarting;
	CpuTimer m_startGameTimer;
	std::string m_remainingTime;
};

#define popGetGameManager() GameManager::GetInstancePtr<GameManager>()