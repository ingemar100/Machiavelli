#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iterator>
#include <memory>
#include <algorithm>
#include <time.h>

#include "Buildingcard.h"

class BuildingcardReader
{
public:
	BuildingcardReader();
	~BuildingcardReader();

	std::vector<std::shared_ptr<Buildingcard>> getBuildingCards();
	std::shared_ptr<Buildingcard> takeCard();

private:
	std::vector<std::shared_ptr<Buildingcard>> buildingCards;
};

