#include "Game.h"
#include "CharacterReader.h"
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
	messageAll(king->get_name() + "is the oldest player, so becomes king.");

	std::shared_ptr<CharacterReader> reader (new CharacterReader()); // smart pointer van maken
	for (auto character : reader->getShuffledCharacters()) {
		messageAll(character->getName());
	}
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

std::string Game::showHelp()
{
	std::string info = "Verloop van een speelbeurt: \r\n";
	info += "\t- Inkomsten: Neem 2 goudstukken of neem 2 kaarten en leg er 1 af\r\n";
	info += "\t- Bouwen: Leg 1 bouwkaart neer en betaal de waarde\r\n";
	info += "\tKaraktereigenschap: Op elk moment te gebruiken\r\n";
	info += "\r\n";

	info += "1 - Moordenaar\r\n";
	info += "\tVermoordt een ander karakter\r\n";
	info += "2 - Dief\r\n";
	info += "\tSteelt van een andere speler\r\n";
	info += "3 - Magier\r\n";
	info += "\tRuilt bouwkaarten om\r\n";
	info += "4 - Koning (geel)\r\n";
	info += "\tBegint de volgende beurt\r\n";
	info += "\tOntvangt van monumenten\r\n";
	info += "5 - Prediker (blauw)\r\n";
	info += "\tIs beschermd tegen de Condottiere\r\n";
	info += "\tOntvangt van kerkelijke gebouwen\r\n";
	info += "6 - Koopman (groen)\r\n";
	info += "\tOntvangt een extra goudstuk\r\n";
	info += "\tOntvangt van commerciele gebouwen\r\n";
	info += "7 - Bouwmeester\r\n";
	info += "\tTrekt twee extra kaarten\r\n";
	info += "\tMag drie gebouwen bouwen\r\n";
	info += "8 - Condottiere(oranje)\r\n";
	info += "\tVernietigt een gebouw\r\n";
	info += "\tOntvangt van alle militaire gebouwen\r\n";

	return info;
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
