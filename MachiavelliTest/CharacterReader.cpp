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
			characters.push_back(tokens[1]);
		}
	}
}


CharacterReader::~CharacterReader()
{
	characters.clear();
}

void CharacterReader::randomCharacters()
{
	int randIndex = rand() % characters.size();
	std::vector<std::string>characterscopy(characters);
	std::random_shuffle(characterscopy.begin(), characterscopy.end());
	characterscopy.erase(characterscopy.begin());
}

std::vector<std::string> CharacterReader::getCharacters()
{
	return characters;
}


