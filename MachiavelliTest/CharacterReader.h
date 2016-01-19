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
#include <memory>

#include "Character.h"

class CharacterReader
{
public:
	CharacterReader();
	~CharacterReader();

	std::vector<std::shared_ptr<Character>> getShuffledCharacters();

	std::vector<std::shared_ptr<Character>> getCharactersInOrder();

private:
	std::vector<std::shared_ptr<Character>> charactersInOrder;
};

