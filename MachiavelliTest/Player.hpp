//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <string>
#include <memory>
#include <vector>

#include "Socket.h"
#include "Buildingcard.h"

class Player {
public:
	Player() {}
	Player(const std::string& name, const int age, std::shared_ptr<Socket> socket) : name{ name }, age{ age }, socket{ socket } {}
	
	std::string get_name() const { return name; }
	int get_age() const { return age; }
	std::shared_ptr<Socket> get_socket() const { return socket; }
	void set_name(const int& new_name) { name = new_name; }
	void set_age(const int& new_age) { age = new_age; }
	void set_socket(std::shared_ptr<Socket> new_socket) { socket = new_socket; }

	void addGold(int amount) { goldPieces += amount; }
	void addBuildingCard(std::shared_ptr<Buildingcard> buildingCard);

	const Player & operator<<(const char c) const;
	const Player & operator<<(const char * message) const;
	const Player & operator<<(const std::string & message) const;
private:
	std::string name;
	int age;
	std::shared_ptr<Socket> socket;

	int goldPieces;
	std::vector<std::shared_ptr<Buildingcard>> buildingCards;
};

#endif /* Player_hpp */
