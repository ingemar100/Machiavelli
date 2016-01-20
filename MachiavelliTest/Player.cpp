//
//  Player.cpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//
#pragma once
#include "Player.hpp"
#include "Dialogue.h"
#include "Game.h"

using namespace std;

void Player::addBuildingCard(std::shared_ptr<Buildingcard> buildingCard)
{
	buildingCards.push_back(buildingCard);
}

void Player::handleCommand(std::string cmd)
{
	if (activeDialog) {
		if (activeDialog->pick(cmd)) {
			activeDialog = nullptr;
		}
	}
	else {
		*socket << "Sorry: '" << name << ", maar je bent niet aan de beurt.\r\n" << machiavelli_prompt;
	}
}

void Player::addCharacter(std::shared_ptr<Character> character)
{
	characters.push_back(character);
}

bool Player::isCharacter(std::shared_ptr<Character> character)
{
	for (auto c : characters) {
		if (c == character) {
			return true;
		}
	}
	return false;
}

void Player::act(std::shared_ptr<Character> character)
{
	*socket << "Je bent nu aan de beurt als: " << character->getName() << "\r\n";

	bool tookGoldOrCards = false;
	bool builtBuildings = false;
	bool usedSpecialPower = false;
	bool endTurn = false;

	while (!endTurn) {
		std::vector<std::string> options;

		//status bekijken
		int choices = 0;
		int takeChoice = -1;
		int buildChoice = -1;
		int specialChoice = -1;

		if (!tookGoldOrCards) {
			options.push_back("Pak goud of kaarten");
			takeChoice = choices;
			choices++;
		}
		if (!builtBuildings) {
			options.push_back("Bouw gebouwen");
			buildChoice = choices;
			choices++;
		}
		if (!usedSpecialPower) {
			options.push_back("Gebruik de karaktereigenschap van de " + character->getName());
			specialChoice = choices;
			choices++;
		}
		options.push_back("Beeindig je beurt");
		activeDialog = make_shared<Dialogue>("Maak een keuze", options, socket);
		int keuze = activeDialog->activate();

		if (keuze == takeChoice) {
			takeGoldOrCards();
			tookGoldOrCards = true;
		}
		else if (keuze == buildChoice) {
			builtBuildings = build();
		}
		else if (keuze == specialChoice) {
			//
		}
		else if (keuze == choices) {
			endTurn = true;
		}
	}
}

std::vector<std::shared_ptr<Character>> Player::pickCharacters(std::vector<std::shared_ptr<Character>> options)
{
	std::string message = "Deze karakterkaarten zijn nog beschikbaar: ";
	std::vector<std::string> optionNames;
	for (auto option : options) {
		message += option->getName() + ",";
		optionNames.push_back(option->getName());
	}
	//1 wegleggen
	message += "\r\nKies welk karakter je wilt afleggen\r\n";

	activeDialog = make_shared<Dialogue>(message, optionNames, socket);
	int discardIndex = activeDialog->activate();

	options.erase(options.begin() + discardIndex);
	optionNames.erase(optionNames.begin() + discardIndex);

	//1 kiezen
	message = "\r\nKies welk karakter je wilt houden\r\n";

	activeDialog = make_shared<Dialogue>(message, optionNames, socket);
	int pickIndex = activeDialog->activate();

	addCharacter(options[pickIndex]);
	options.erase(options.begin() + pickIndex);
	optionNames.erase(optionNames.begin() + pickIndex);
	

	//overgebleven karakters doorgeven
	return options;
}

const Player& Player::operator<<(const char c) const
{
	socket->write(c);
	return *this;
}

const Player& Player::operator<<(const char* message) const
{
	socket->write(std::string{ message });
	return *this;
}

const Player& Player::operator<<(const std::string& message) const
{
	socket->write(message);
	return *this;
}

void Player::takeGoldOrCards()
{
	std::vector<std::string> options;
	options.push_back("Neem 2 goudstukken");
	options.push_back("Neem 2 bouwkaarten en leg er 1 af");
	activeDialog = make_shared<Dialogue>("Maak een keuze", options, socket);
	int keuze = activeDialog->activate();

	if (keuze == 0) {
		game->takeGold(shared_from_this(), 2);
		game->messageAllExcept(name + " heeft twee goudstukken gepakt.", shared_from_this());
	}
	else if (keuze == 1) {
		auto options = game->getBuildingcardReader()->peekTwo();
		std::vector<std::string> names;
		names.push_back(options[0]->toString());
		names.push_back(options[1]->toString());

		activeDialog = make_shared<Dialogue>("Kies een kaart (de andere kaart wordt afgelegd)", names, socket);
		int keuze = activeDialog->activate();

		if (keuze == 0) {
			game->takeCards(shared_from_this(), 1);
			game->getBuildingcardReader()->discardTop();
		}
		else if (keuze == 1) {
			game->getBuildingcardReader()->discardTop();
			game->takeCards(shared_from_this(), 1);
		}

		game->messageAllExcept(name + " heeft twee kaarten gepakt en een afgelegd.", shared_from_this());
	}
}

bool Player::build()
{
	//geef lijst van gebouwen of terug
	std::vector<std::string> names;
	for (auto card : buildingCards) {
		names.push_back(card->toString());
	}
	names.push_back("Annuleren");

	activeDialog = make_shared<Dialogue>("Welk gebouw wil je bouwen? Aantal goudstukken: " + std::to_string(goldPieces), names, socket);
	int choice = activeDialog->activate();

	if (choice == names.size()) {
		return false;
	}
	else {
		auto toBuild = buildingCards[choice];
		if (toBuild->getPrice() > goldPieces) {
			*socket << "Je hebt niet genoeg goudstukken om dit gebouw te kunnen bouwen\r\n";
			return build();
		}
		else {
			buildingsBuilt.push_back(toBuild);
			buildingCards.erase(buildingCards.begin() + choice);
			*socket << "Je hebt een " << toBuild->getName() << " gebouwd\r\n";
			game->messageAllExcept(name + " heeft een " + toBuild->getName() + " gebouwd\r\n", shared_from_this());
		}
	}

	return true;
}
