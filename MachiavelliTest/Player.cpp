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
	*socket << "\r\nJe bent nu aan de beurt als: " << character->getName() << "\r\n";

	if (character->isStolenFrom()) {
		character->thief->addGold(goldPieces);
		goldPieces = 0;
	}

	getIncome(character);

	bool tookGoldOrCards = false;
	buildingsAllowed = 1;
	bool usedSpecialPower = false;
	bool endTurn = false;

	if (character->getName() == "Bouwmeester") {
		buildingsAllowed = 3;
	}

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
		if (buildingsAllowed > 0) {
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
			build();
		}
		else if (keuze == specialChoice) {
			useSpecial(character);
			usedSpecialPower = true;
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

	if (choice == names.size() - 1) {
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

			buildingsAllowed--;
			goldPieces -= toBuild->getPrice();
		}
	}

	return true;
}

void Player::useSpecial(std::shared_ptr<Character> character)
{
	if (character->getName() == "Moordenaar") {
		std::vector<std::shared_ptr<Character>>characters(game->getCharacterReader()->getCharactersInOrder());
		characters.erase(characters.begin());

		std::vector<std::string> names;
		for (auto c : characters) {
			names.push_back(c->getName());
		}

		activeDialog = make_shared<Dialogue>("Welk karakter wil je vermoorden: ", names, socket);
		int choice = activeDialog->activate();

		auto toDie = game->getCharacterReader()->getCharactersInOrder()[choice + 1];
		toDie->setDead(true);

		game->messageAll("\r\nDe " + toDie->getName() + " is vermoord\r\n");
	}
	else if (character->getName() == "Dief") {
		std::vector<std::shared_ptr<Character>>characters(game->getCharacterReader()->getCharactersInOrder());
		characters.erase(characters.begin());//moordenaar
		characters.erase(characters.begin());//dief

		std::vector<std::string> names;
		for (auto c : characters) {
			names.push_back(c->getName());
		}

		activeDialog = make_shared<Dialogue>("Van welk karakter wil je stelen: ", names, socket);
		int choice = activeDialog->activate();

		auto victim = game->getCharacterReader()->getCharactersInOrder()[choice + 2];
		victim->setStolenFrom(true);
		victim->thief = shared_from_this();
		game->messageAll("\r\nDe " + victim->getName() + " wordt door de dief bestolen\r\n");
	}
	else if (character->getName() == "Magier") {
		std::vector<std::string> optns = { "Kaarten ruilen met een andere speler", "Alle kaarten afleggen en een gelijk aantal kaarten trekken" };
		activeDialog = make_shared<Dialogue>("Wat wil je doen: ", optns, socket);
		int choice = activeDialog->activate();

		if (choice == 0) {
			auto players = game->getPlayers();
			std::vector<std::string> optns2;
			for (auto player : players) {
				optns2.push_back(player->get_name());
			}
			activeDialog = make_shared<Dialogue>("Welke speler?: ", optns2, socket);
			int choice = activeDialog->activate();
			auto victim = players[choice];

			auto tmp = victim->buildingCards;
			victim->buildingCards = this->buildingCards;
			this->buildingCards = tmp;
		}
		else if (choice == 1) {
			int size = buildingCards.size();
			buildingCards.clear();

			game->takeCards(shared_from_this(), size);
		}
	}
	else if (character->getName() == "Bouwmeester") {
		game->takeCards(shared_from_this(), 2);
	}
}

void Player::getIncome(std::shared_ptr<Character> character)
{
	if (character->getName() == "Koopman") {
		game->messageAllExcept(name + " ontvangt als koopman 1 goudstuk\r\n", shared_from_this());
		*socket << "Je ontvangt als koopman 1 goudstuk\r\n";
		goldPieces++;
	}
	int income = 0;

	for (auto building : buildingsBuilt) {
		if (building->getColor() == character->getColor()) {
			income++;
		}
	}

	goldPieces += income;
	game->messageAllExcept(name + " ontvangt " + to_string(income) + " goudstukken voor zijn gebouwen\r\n", shared_from_this());
	*socket << "Je ontvangt " << to_string(income) << " goudstukken voor je gebouwen\r\n";

}
