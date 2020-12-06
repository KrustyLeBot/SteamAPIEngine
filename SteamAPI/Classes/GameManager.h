#pragma once
#include <vector>
#include "Classes/OnlineManager.h"
#include "Classes/NetworkManager.h"
#include "SFML/Graphics.hpp"
#include "Steam/steam_api.h"

class GameManager : public OnlineManager
{
public:
	GameManager();
	~GameManager();

	void Update() override;
	void SetRenderWindow(sf::RenderWindow* renderWindow) { m_renderWindow = renderWindow; }
	void DrawStaticElements();

	void SetBackgroundData(NetworkManager::BackgroundColorData data) { m_bgData = data; }


private:
	sf::RenderWindow* m_renderWindow;
	NetworkManager::BackgroundColorData m_bgData;
};

#define popGetGameManager() GameManager::GetInstancePtr<GameManager>()