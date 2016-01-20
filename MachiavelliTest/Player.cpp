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
		*socket << "Sorry: '" << name << ", but its not your turn to act.\r\n" << machiavelli_prompt;
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

	std::vector<std::string> options;
	options.push_back("Neem 2 goudstukken");
	options.push_back("Neem 2 bouwkaarten en leg er 1 af");
	activeDialog = make_shared<Dialogue>("Maak een keuze",options, socket);
	int keuze = activeDialog->activate();

	if (keuze == 0) {
		game->takeGold(shared_from_this(), 2);
	}
	else if (keuze == 1) {
		auto options = game->getBuildingcardReader()->peekTwo();
		std::vector<std::string> names;
		names.push_back(options[0]->getName());
		names.push_back(options[1]->getName());

		activeDialog = make_shared<Dialogue>("Pick a card (the other card will be discarded)", names, socket);
		int keuze = activeDialog->activate();

		if (keuze == 0) {
			game->takeCards(shared_from_this(), 1);
			game->getBuildingcardReader()->discardTop();
		}
		else if (keuze == 1) {
			game->getBuildingcardReader()->discardTop();
			game->takeCards(shared_from_this(), 1);
		}
	}
}

std::vector<std::shared_ptr<Character>> Player::pickCharacters(std::vector<std::shared_ptr<Character>> options)
{
	std::string message = "These character cards are available: ";
	std::vector<std::string> optionNames;
	for (auto option : options) {
		message += option->getName() + ",";
		optionNames.push_back(option->getName());
	}
	//1 wegleggen
	message += "\r\nPick a character to discard\r\n";

	activeDialog = make_shared<Dialogue>(message, optionNames, socket);
	int discardIndex = activeDialog->activate();

	options.erase(options.begin() + discardIndex);
	optionNames.erase(optionNames.begin() + discardIndex);

	//1 kiezen
	message = "\r\nPick a character to keep\r\n";

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