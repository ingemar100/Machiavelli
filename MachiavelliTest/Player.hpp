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

class Player {
public:
	Player() {}
	Player(const std::string& name, const int age) : name{ name }, age{ age } {}
	
	std::string get_name() const { return name; }
	int get_age() const { return age; }
	void set_name(const int& new_name) { name = new_name; }
	void set_age(const int& new_age) { age = new_age; }
	
private:
	std::string name;
	int age;
};

#endif /* Player_hpp */
