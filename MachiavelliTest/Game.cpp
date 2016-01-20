#pragma once
#include "Game.h"
#include "CharacterReader.h"
#include <iostream>
#include "machiavelli.h"
#include <thread>

Game::Game()
{
}


Game::~Game()
{
}

void Game::setUp()
{
	//check if enough players

	messageAll("Voorbereiden spel...");

	bg = std::make_shared<BuildingcardReader>();
	cr = std::make_shared<CharacterReader>();

	goldPieces = 30;

	//oldest player is king
	int oldest = 0;
	for (auto player : players) {
		if (player->get_age() > oldest) {
			king = player;
		}

		//each player takes two goldpieces
		takeGold(player, 2);
		takeCards(player, 4);
	}

	//king->get_socket << "Koning: " << king->get_name() << "\r\n";
	messageAll(king->get_name() + " is de oudste speler, dus die wordt de koning.");
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

void Game::startRound()
{
	messageAll("Begin volgende ronde");

	pickCharacters();

	handleTurns();
}

void Game::handleCommand(ClientCommand command)
{
	try {
		std::string cmd = command.get_cmd();
		if (command.get_cmd() == "start") {
			setUp();

			while (true) {
				startRound();
			}
		}
		else if (command.get_cmd() == "help") {
			*command.get_client() << showHelp();
		}
		else if(std::all_of(cmd.begin(), cmd.end(), ::isdigit)){
			command.get_player()->handleCommand(command.get_cmd());
			//check if players turn
		}
		else {
			*command.get_client() << command.get_player()->get_name() << ", you wrote: '" << command.get_cmd() << "', but I'll ignore that for now.\r\n" << machiavelli_prompt;
		}
	}
	catch (const exception& ex) {
		cerr << "*** exception in consumer thread for player " << command.get_player()->get_name() << '\n';
		if (command.get_client()->is_open()) {
			command.get_client()->write("Sorry, something went wrong during handling of your request.\r\n");
		}
	}
	catch(...) {
		cerr << "*** exception in consumer thread for player " << command.get_player()->get_name() << '\n';
		if (command.get_client()->is_open()) {
			command.get_client()->write("Sorry, something went wrong during handling of your request.\r\n");
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

void Game::takeCards(std::shared_ptr<Player> player, int amount)
{
	*player << "Je hebt de volgende bouwkaarten gepakt:";
	for (int i = 0; i < amount; i++) {
		std::shared_ptr<Buildingcard> buildingcardToBeTaken = bg->takeCard();
		player->addBuildingCard(buildingcardToBeTaken);
		*player << "\r\n\t" << buildingcardToBeTaken->toString() << "\r\n";
		if (buildingcardToBeTaken->getDescription() != "") {
			*player << "\t\t" << buildingcardToBeTaken->getDescription() << "\r\n";
		}
	}
}

void Game::pickCharacters()
{
	//clear characters from players
	for (auto player : players) {
		player->clearCharacters();
	}

	std::vector<std::shared_ptr<Character>> characters = cr->getShuffledCharacters();
	std::shared_ptr<Player> toPick = king;

	while (characters.size() > 0) {
		//koning kiest eerst kaarten
		//daarna steeds volgende
		characters = toPick->pickCharacters(characters);
		messageAllExcept(toPick->get_name() + " is karakters aan het kiezen.\r\n", toPick);

		bool next = false;
		bool picked = false;
		int index = 0;
		while (!picked){
			auto player = players[index];
			if (next) {
				toPick = player;
				break;
			}
			else if (player == toPick) {
				next = true;
			}
			index++;
			if (index >= players.size()) {
				index = 0;
			}
		}
	}
}

void Game::handleTurns() {
	auto characters = cr->getCharactersInOrder();

	for (auto character : characters) {
		if (!character->isDead()){
			bool isPicked = false;
			//check welke player karakter is
			for (auto player : players) {
				if (player->isCharacter(character)) {
					messageAllExcept(player->get_name() + " is aan de beurt als " + character->getName() + ".\r\n", player);
					player->act(character);
					isPicked = true;

					if (character->getName() == "Koning") {
						king = player;
					}
				}

			}

			if (!isPicked) {
				messageAll("\r\n" + character->getName() + " is door niemand gekozen\r\n");
			}
		}
		else {
			messageAll("\r\n" + character->getName() + " komt niet aan de beurt omdat hij vermoord is\r\n");
			character->setDead(false);
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
	info += "8 - Condottiere(rood)\r\n";
	info += "\tVernietigt een gebouw\r\n";
	info += "\tOntvangt van alle militaire gebouwen\r\n";

	info += machiavelli_prompt;

	return info;
}