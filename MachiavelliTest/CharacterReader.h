#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <time.h>

class CharacterReader
{
private:
	std::vector<std::string> characters;

public:
	CharacterReader();
	~CharacterReader();
	void randomCharacters();

	std::vector<std::string> getCharacters();
};

