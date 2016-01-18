#include "Game.h"
#include <iostream>


Game::Game()
{
}


Game::~Game()
{
}

void Game::setUp()
{
	//check if enough players

	messageAll("Setting up game");

	goldPieces = 30;

	//oldest player is king
	int oldest = 0;
	std::shared_ptr<Player> king;
	for (auto player : players) {
		if (player->get_age() > oldest) {
			king = player;
		}

		//each player takes two goldpieces
		takeGold(player, 2);
	}

	//king->get_socket << "Koning: " << king->get_name() << "\r\n";
	messageAll(king->get_name() + " is the oldest player, so becomes king.");


}

void Game::addPlayer(std::shared_ptr<Player> player)
{
	players.push_back(player);
}

void Game::removePlayer(std::shared_ptr<Player> player)
{
	//remove the player
	std::vector<std::shared_ptr<Player>>::iterator i = players.begin();
	while (i != players.end()) {
		if (*i == player) {
			i = players.erase(i);
		}
		else {
			i++;
		}
	}
}

void Game::messageAll(std::string message)
{
	for (auto player : players) {
		*player << message << "\r\n";
	}
}

void Game::messageAllExcept(std::string message, std::shared_ptr<Player> except)
{
	for (auto player : players) {
		if (player != except) {
			*player << message << "\r\n";
		}
	}
}

void Game::takeGold(std::shared_ptr<Player> player, int amount)
{
	goldPieces -= amount;
	if (goldPieces < 0) {
		amount += goldPieces;
	}
	player->addGold(amount);
}
