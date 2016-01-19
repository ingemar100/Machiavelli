#pragma once
#include <string>
class Character
{
public:
	Character();
	~Character();

	std::string getName() { return _name;  };
	std::string getColor() { return _color; };
	
	void setName(std::string name) { _name = name; }
	void setColor(std::string color) { _color = color; };

private:
	std::string _name = "";
	std::string _color = "";
};

