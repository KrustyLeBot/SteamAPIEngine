//Standard libs
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>

//Steamworks API
#include "Steam/steam_api.h"

//Dear ImGui
#include "imgui.h"
#include "imgui-SFML.h"

//SFML
#include "SFML/Graphics.hpp"

//Custom Classes
#include "Classes/FriendsManager.h"
#include "Classes/LobbyManager.h"
#include "Classes/NetworkManager.h"
#include "Classes/GameManager.h"
#include "Classes/DebugMenu.h"

#define TARGET_WINDOW_NAME "SteamAPIEngine"
#define TARGET_WINDOW_LENGTH 1920
#define TARGET_WINDOW_HEIGHT 1080
#define TARGET_FRAMERATE 60

int32 APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int32 nCmdShow)
{
	OutputDebugString("Start application\n");

	if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
	{
		OutputDebugString("Application was not started with steam\n");
		return EXIT_FAILURE;
	}

	sf::RenderWindow window(sf::VideoMode(TARGET_WINDOW_LENGTH, TARGET_WINDOW_HEIGHT), TARGET_WINDOW_NAME, sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(TARGET_FRAMERATE);
	popGetGameManager()->SetRenderWindow(&window);

	ImGui::SFML::Init(window);

	if (!SteamAPI_Init())
	{
		OutputDebugString("SteamAPI_Init() failed\n");
		return EXIT_FAILURE;
	}

	if (!SteamUser()->BLoggedOn())
	{
		OutputDebugString("Steam user is not logged in\n");
		return EXIT_FAILURE;
	}

	sf::Clock deltaClock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		//Steam update
		SteamAPI_RunCallbacks();

		//Manager updates
		popGetNetworkManager()->Update();
		popGetLobbyManager()->Update();
		popGetFriendsManager()->Update();
		
		//Draw ImGui
		ImGui::SFML::Update(window, deltaClock.restart());
		DrawMainDebugMenu();
		DrawLobbyDebugMenu();
		DrawMessageQueueDebugMenu();
		
		//Clear old frame
		window.clear();

		//Draw Game
		popGetGameManager()->Update();

		//Render ImGui and draw the frame
		ImGui::SFML::Render(window);
		window.display();
	}

	Singleton::DestoryAllManagers();
	ImGui::SFML::Shutdown();
	SteamAPI_Shutdown();

	return EXIT_SUCCESS;
}