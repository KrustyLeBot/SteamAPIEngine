#include "GameManager.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::Update()
{
}

void GameManager::DrawStaticElements()
{
	if (m_renderWindow != nullptr)
	{
		sf::RectangleShape rectangle(sf::Vector2f(1920, 1080));
		rectangle.setFillColor(sf::Color(m_bgData.R, m_bgData.G, m_bgData.B));

		m_renderWindow->draw(rectangle);
	}
}