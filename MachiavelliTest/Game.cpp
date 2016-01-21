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

	messageAll("Voorbereiden spel...");

	bg = std::make_shared<BuildingcardReader>();
	cr = std::make_shared<CharacterReader>();

	goldPieces = 30;

	//oldest player is king
	int oldest = 0;
	for (auto player : players) {
		player->clearBuildingCards();
		if (player->get_age() > oldest) {
			king = player;
			oldest = player->get_age();
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
			//check if enough players
			if (players.size() < 2) {
				messageAll("Er zijn twee spelers nodig om te kunnen spelen\r\n" + machiavelli_prompt);
				return;
			}

			setUp();

			while (!endGame) {
				startRound();
			}

			//game over
			pointCount();
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

void Game::setFirstToEight(std::shared_ptr<Player> _first)
{
	firstToEight = _first;
	messageAll(_first->get_name() + " heeft 8 gebouwen, dus het spel eindigt na deze ronde\r\n");
	endGame = true;
}

void Game::showStatus(std::shared_ptr<Socket> socket)
{
	for (auto player : players) {
		*socket << "--- Status van " << player->get_name() << " ---\r\n";
		*socket << "Goud: " << to_string(player->get_gold()) << "\r\n";
		*socket << "Gebouwde gebouwen: \r\n";
		for (auto building : player->getBuildingsBuilt()) {
			*socket << "\t" << building->toString() << "\r\n";
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

	//reset characters
	for (auto character : characters) {
		character->setStolenFrom(false);
		character->setDead(false);
	}

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

void Game::pointCount()
{
	messageAll("\r\nEinde spel\r\n\nPuntentelling:\r\n");
	for (auto player : players) {
		int totalPoints = 0;
		int pointsForBuildings = 0;
		bool hasRed = false;
		bool hasGreen = false;
		bool hasBlue = false;
		bool hasLila = false;
		bool hasYellow = false;

		for (auto building : player->getBuildingsBuilt()) {
			pointsForBuildings += building->getPrice();
			if (building->getColor() == "Groen") {
				hasGreen = true;
			}
			else if (building->getColor() == "Rood") {
				hasRed = true;
			}
			else if (building->getColor() == "Blauw") {
				hasBlue = true;
			}
			else if (building->getColor() == "Lila") {
				hasLila = true;
			}
			else if (building->getColor() == "Geel") {
				hasYellow = true;
			}
		}
		totalPoints += pointsForBuildings;

		if (hasRed && hasGreen && hasBlue && hasLila && hasYellow) {
			totalPoints += 3;
		}

		if (player == firstToEight) {
			totalPoints += 4;
		}
		else if (player->getBuildingsBuilt().size() >= 8) {
			totalPoints += 2;
		}

		player->setPointsForBUildings(pointsForBuildings);
		player->setTotalPoints(totalPoints);

		messageAll(player->get_name() + ": " + to_string(totalPoints));
	}

	determineWinner();

	
}

void Game::determineWinner()
{
	std::shared_ptr<Player> highestTotal = players[0];
	bool equalTotal = false;
	std::shared_ptr<Player> highestForBuildings = players[0];
	bool equalHighest = false;

	for (int i = 1; i < players.size(); i++) {
		auto player = players[i];
		if (highestTotal->getTotalPoints() < player->getTotalPoints()) {
			highestTotal = player;
			equalTotal = false;
		}
		else if (highestTotal->getTotalPoints() == player->getTotalPoints()) {
			equalTotal = true;
		}

		if (highestForBuildings->getPointsForBuildings() < player->getPointsForBuildings()) {
			highestTotal = player;
			equalHighest = false;
		}
		else if (highestForBuildings->getPointsForBuildings() == player->getPointsForBuildings()) {
			equalHighest = true;
		}
	}

	if (equalTotal && equalHighest) {
		messageAll("Het is een gelijkspel!");
	}
	else if (equalTotal) {
		messageAll("Winnaar " + highestForBuildings->get_name() + "!");
	}
	else {
		messageAll("Winnaar: " + highestTotal->get_name() + "!");
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