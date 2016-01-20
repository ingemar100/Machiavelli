#include "Buildingcard.h"



Buildingcard::Buildingcard()
{
}


Buildingcard::~Buildingcard()
{
}

std::string Buildingcard::toString()
{
	return _name + " (" + _color + ", " + std::to_string(_price) + ")";
}
