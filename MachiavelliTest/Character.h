#pragma once
#include <string>
#include <memory>

class Player;

class Character
{
public:
	Character();
	~Character();

	std::string getName() { return _name;  };
	std::string getColor() { return _color; };
	bool isDead() { return _dead; };
	bool isStolenFrom() { return _stolenFrom; };

	void setName(std::string name) { _name = name; }
	void setColor(std::string color) { _color = color; };
	void setDead(bool dead) { _dead = dead; };
	void setStolenFrom(bool stolenFrom) { _stolenFrom = stolenFrom; };


private:
	std::string _name = "";
	std::string _color = "";
	bool _dead = false;
	bool _stolenFrom = false;
	std::shared_ptr<Player> thief = nullptr;

	friend class Player;
};

