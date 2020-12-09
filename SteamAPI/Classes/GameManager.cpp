#include "GameManager.h"
#include "Classes/LobbyManager.h"
#include <chrono>
#include <sstream>
#include <windows.h>

static const char* GAME_STARTING_METADATA_KEY = "game_starting";
static const char* GAME_START_TIMESTAMP_METADATA_KEY = "game_start_timestamp";
static const int GAME_START_TIMER_SECONDS = 10;
static const int TICK_RATE_SERVER = 64;

GameManager::GameManager()
	: m_gameStarted(false)
	, m_gameStarting(false)
{
	m_font.loadFromFile("Assets/fonts/cmunss.ttf");
}

GameManager::~GameManager()
{
}

void GameManager::Update()
{
	SetGameStarting(popGetLobbyManager()->GetLobbyMetadata(GAME_STARTING_METADATA_KEY));

	if (m_gameStarting)
	{
		int startTimestamp = std::stoi(popGetLobbyManager()->GetLobbyMetadata(GAME_START_TIMESTAMP_METADATA_KEY));
		const auto now = std::chrono::system_clock::now();
		int nowTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
		if (nowTimestamp >= startTimestamp)
		{
			m_gameStarted = true;
			m_gameStarting = false;
		}
		else
		{
			m_remainingTime = std::to_string(startTimestamp - nowTimestamp);
		}
	}

	if (m_gameStarted)
	{
		if (!m_timer.IsStarted() || m_timer.GetElapsedTimeMs() >= int(1000.0 / TICK_RATE_SERVER))
		{
			popGetNetworkManager()->SendDataToAllLobby(popGetGameManager()->GetBackgroundData());

			if (popGetLobbyManager()->IsLocalPlayerCurrentLobbyOwner())
			{
				//We are the server
				popGetNetworkManager()->SendPlayerPositionDataToAllLobby(m_lobbyPlayerPositions);
			}
			else
			{
				//We are a client
				popGetNetworkManager()->SendCurrentPlayerPositionDataToLobbyOwner(m_position);
			}

			m_timer.Reset();
			m_timer.Start();
		}
	}
}

void GameManager::DrawStaticElements()
{
	if (m_renderWindow != nullptr)
	{
		sf::RectangleShape rectangle(sf::Vector2f(1920, 1080));
		rectangle.setFillColor(sf::Color(m_bgData.R, m_bgData.G, m_bgData.B));

		m_renderWindow->draw(rectangle);
		
		if (m_gameStarting)
		{
			sf::Text text;
			text.setFont(m_font);
			text.setString(m_remainingTime);
			text.setCharacterSize(250);
			m_renderWindow->draw(text);
		}

		if (m_gameStarted)
		{
			sf::Text text;
			text.setFont(m_font);
			text.setString("Game Started");
			text.setCharacterSize(250);
			m_renderWindow->draw(text);
		}
	}
}

void GameManager::StartGame()
{
	popGetLobbyManager()->SetLobbyMetadata(GAME_STARTING_METADATA_KEY, "true");
	SetGameStarting("true");
	const auto now = std::chrono::system_clock::now();
	std::string startTimestamp = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() + GAME_START_TIMER_SECONDS);
	popGetLobbyManager()->SetLobbyMetadata(GAME_START_TIMESTAMP_METADATA_KEY, startTimestamp);
}

void GameManager::StopGame()
{
	popGetLobbyManager()->SetLobbyMetadata(GAME_STARTING_METADATA_KEY, "false");
	SetGameStarting("false");
	m_gameStarted = false;
}

void GameManager::SetGameStarting(std::string value)
{
	if (value == "true")
	{
		m_gameStarting = true;
	}
	else
	{
		m_gameStarting = false;
		m_gameStarted = false;
	}
}

void GameManager::SetPlayerPosition(PlayerPositionData data)
{
	m_lobbyPlayerPositions[data.playerId.ConvertToUint64()] = data;
}