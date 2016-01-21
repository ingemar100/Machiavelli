//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//
#pragma once
#ifndef Player_hpp
#define Player_hpp

#include <string>
#include <memory>
#include <vector>

#include "Socket.h"
#include "Buildingcard.h"
#include "Character.h"

class Game;

class Dialogue;

class Player : public std::enable_shared_from_this<Player> {
public:
	Player() {}
	Player(const std::string& name, const int age, std::shared_ptr<Socket> socket, std::shared_ptr<Game> game) : name{ name }, age{ age }, socket{ socket }, game{ game } {}

	std::string get_name() const { return name; }
	int get_age() const { return age; }
	std::shared_ptr<Socket> get_socket() const { return socket; }
	void set_name(const int& new_name) { name = new_name; }
	void set_age(const int& new_age) { age = new_age; }
	void set_socket(std::shared_ptr<Socket> new_socket) { socket = new_socket; }

	void addGold(int amount) { goldPieces += amount; }
	void addBuildingCard(std::shared_ptr<Buildingcard> buildingCard);
	void handleCommand(std::string cmd);
	void addCharacter(std::shared_ptr<Character> character);
	void clearCharacters() { characters.clear(); };
	bool isCharacter(std::shared_ptr<Character> character);
	void act(std::shared_ptr<Character> character);
	std::vector<std::shared_ptr<Buildingcard>> getBuildingsBuilt() { return buildingsBuilt; };

	std::vector<std::shared_ptr<Character>> pickCharacters(std::vector<std::shared_ptr<Character>> options);
	void setTotalPoints(int _points) { totalPoints = _points; };
	void setPointsForBUildings(int _points) { pointsForBuildings = _points; };
	int getTotalPoints() { return totalPoints; };
	int getPointsForBuildings() { return pointsForBuildings; };
	void clearBuildingCards() { buildingCards.clear(); };

	const Player & operator<<(const char c) const;
	const Player & operator<<(const char * message) const;
	const Player & operator<<(const std::string & message) const;
private:
	std::string name;
	int age;
	std::shared_ptr<Socket> socket;
	int buildingsAllowed = 0;

	int goldPieces = 0;
	std::vector<std::shared_ptr<Buildingcard>> buildingCards;
	std::vector<std::shared_ptr<Buildingcard>> buildingsBuilt;
	std::shared_ptr<Dialogue> activeDialog;
	std::vector<std::shared_ptr<Character>> characters;
	std::shared_ptr<Game> game;
	void takeGoldOrCards();
	bool build();
	void useSpecial(std::shared_ptr<Character> character);
	void getIncome(std::shared_ptr<Character> character);
	int totalPoints = 0;
	int pointsForBuildings = 0;
};

#endif /* Player_hpp */
