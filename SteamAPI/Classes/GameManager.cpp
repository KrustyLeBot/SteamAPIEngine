#include "GameManager.h"
#include "Classes/LobbyManager.h"
#include <chrono>
#include <sstream>
#include <windows.h>

static const char* GAME_STARTING_METADATA_KEY = "game_starting";
static const char* GAME_START_TIMESTAMP_METADATA_KEY = "game_start_timestamp";
static const int GAME_START_TIMER_SECONDS = 10;
static const int TICK_RATE_SERVER = 35;

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
	if (m_position.playerId == CSteamID())
	{
		m_position.playerId = SteamUser()->GetSteamID();
		m_position.sprite = 0;
	}
	
	SetGameStarting(popGetLobbyManager()->GetLobbyMetadata(GAME_STARTING_METADATA_KEY));
	UpdatePlayerPosition();

	if (m_gameStarting)
	{
		int startTimestamp = std::stoi(popGetLobbyManager()->GetLobbyMetadata(GAME_START_TIMESTAMP_METADATA_KEY));
		const auto now = std::chrono::system_clock::now();
		__int64 nowTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
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
		//If we have more position than players, clean the map
		if (m_lobbyPlayerPositions.size() > popGetLobbyManager()->GetCurrentLobbyPlayerList().size())
		{
			std::vector<CSteamID>& lobbyPlayerList = popGetLobbyManager()->GetCurrentLobbyPlayerList();
			std::vector<uint64> keyToErase;
			for (const auto playerPosition : m_lobbyPlayerPositions)
			{
				const auto found = std::find_if(lobbyPlayerList.begin(), lobbyPlayerList.end(), [playerPosition](const CSteamID x) { return playerPosition.first == x.ConvertToUint64(); });
				if (found == lobbyPlayerList.end())
				{
					//Remove position of players that are no longer in the lobby
					keyToErase.push_back(playerPosition.first);
				}
			}

			for (const auto key : keyToErase)
			{
				m_lobbyPlayerPositions.erase(key);
			}
		}

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

	// Handle Draw items at the end of the update
	DrawStaticElements();

	if (m_gameStarted)
	{
		DrawDynamicElements();
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

void GameManager::UpdatePlayerPosition()
{
	sf::Vector2i pos = sf::Mouse::getPosition(*(sf::Window*)m_renderWindow);
	m_position.posX = pos.x;
	m_position.posY = pos.y;

	SetPlayerPosition(m_position);
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

void GameManager::DrawDynamicElements()
{
	if (m_renderWindow != nullptr)
	{
		//Draw local player position
		sf::RectangleShape rectangle1(sf::Vector2f(20, 20));
		rectangle1.setFillColor(sf::Color(150, 150, 150));
		rectangle1.setPosition(m_position.posX, m_position.posY);
		m_renderWindow->draw(rectangle1);

		//Draw lobby player positions
		for (const auto playerPostion : m_lobbyPlayerPositions)
		{
			if (playerPostion.first != SteamUser()->GetSteamID().ConvertToUint64())
			{
				sf::RectangleShape rectangle2(sf::Vector2f(20, 20));
				rectangle2.setFillColor(sf::Color(150, 150, 150));
				rectangle2.setPosition(playerPostion.second.posX, playerPostion.second.posY);
				m_renderWindow->draw(rectangle2);
			}
		}
	}
}