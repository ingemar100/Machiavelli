#pragma once
#include <string>

class Buildingcard
{
public:
	Buildingcard();
	~Buildingcard();

	std::string getName() { return _name; };
	int getPrice() { return _price; };
	std::string getColor() { return _color; };
	std::string getDescription() { return _description; };

	void setName(std::string name) { _name = name; };
	void setPrice(int price) { _price = price; };
	void setColor(std::string color) { _color = color; };
	void setProperty(std::string description) { _description = description; };

private:
	std::string _name = "";
	int _price = 0;
	std::string _color = "";
	std::string _description = "";
};

