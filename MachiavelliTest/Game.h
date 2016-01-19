#pragma once
#include <vector>
#include <memory>

#include "Player.hpp"
#include "BuildingcardReader.h"
#include "CharacterReader.h"
#include "ClientCommand.h"

class Game
{
public:
	Game();
	~Game();

	void setUp();
	void addPlayer(std::shared_ptr<Player> player);
	void removePlayer(std::shared_ptr<Player> player);
	std::string showHelp();
	void startRound();
	void handleCommand(ClientCommand command);

private:
	std::vector<std::shared_ptr<Player>> players;
	void messageAll(std::string message);
	void messageAllExcept(std::string message, std::shared_ptr<Player> except);
	int goldPieces;
	void takeGold(std::shared_ptr<Player> player, int amount);
	void takeCards(std::shared_ptr<Player> player, int amount);
	std::shared_ptr<BuildingcardReader> bg;
	std::shared_ptr<CharacterReader> cr;
	std::shared_ptr<Player> king;
	void pickCharacters();
	void handleTurns();
};

