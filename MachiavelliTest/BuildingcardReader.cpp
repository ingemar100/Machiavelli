#include "BuildingcardReader.h"



BuildingcardReader::BuildingcardReader()
{
	srand(time(0));
	const std::string textfile("Bouwkaarten.csv");
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
			std::shared_ptr<Buildingcard> bg(new Buildingcard());
			bg->setName(tokens[0]);
			bg->setPrice(std::stoi(tokens[1]));
			bg->setColor(tokens[2]);
			if (tokens.size() > 3) {
				bg->setProperty(tokens[3]);
			}
			buildingCards.push_back(bg);
		}
	}
	std::random_shuffle(buildingCards.begin(), buildingCards.end());
}


BuildingcardReader::~BuildingcardReader()
{

}

std::vector<std::shared_ptr<Buildingcard>> BuildingcardReader::getBuildingCards()
{
	return buildingCards;
}

std::shared_ptr<Buildingcard> BuildingcardReader::takeCard()
{
	std::shared_ptr<Buildingcard> cardToBeTaken = buildingCards[0];
	buildingCards.erase(buildingCards.begin());
	return cardToBeTaken;
}


