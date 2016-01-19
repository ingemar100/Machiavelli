#include "CharacterReader.h"



CharacterReader::CharacterReader()
{
	const std::string textfile("karakterkaarten.csv");
	std::ifstream input_file(textfile); // Dze constructie opent de file voor het lezen;
	std::string line;
	while (getline(input_file, line)) {

		std::string buffer;
		std::stringstream ss(line);

		std::vector<std::string>tokens;

		while (std::getline(ss, buffer, ';')) {
			tokens.push_back(buffer);
		}
		if (tokens.size() > 1) {
			std::shared_ptr<Character> character(new Character());
			character->setName(tokens[1]);
			if (tokens.size() > 2) {
				character->setColor(tokens[2]);
			}
			charactersInOrder.push_back(character);
		}
	}
}


CharacterReader::~CharacterReader()
{
	charactersInOrder.clear();
}

std::vector<std::shared_ptr<Character>> CharacterReader::getShuffledCharacters()
{
	int randIndex = rand() % charactersInOrder.size();
	std::vector<std::shared_ptr<Character>>shuffledCharacters(charactersInOrder);
	std::random_shuffle(shuffledCharacters.begin(), shuffledCharacters.end());
	//shuffledCharacters.erase(shuffledCharacters.begin());

	return  shuffledCharacters;
}

std::vector<std::shared_ptr<Character>> CharacterReader::getCharactersInOrder()
{
	return charactersInOrder;
}


