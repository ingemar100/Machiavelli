#pragma once
#include <vector>
#include "Player.hpp"
#include <memory>

class Game
{
public:
	Game();
	~Game();

	void setUp();
	void addPlayer(std::shared_ptr<Player> player);
	void removePlayer(std::shared_ptr<Player> player);
	std::string showHelp();
private:
	std::vector<std::shared_ptr<Player>> players;
	void messageAll(std::string message);
	void messageAllExcept(std::string message, std::shared_ptr<Player> except);
	int goldPieces;
	void takeGold(std::shared_ptr<Player> player, int amount);

};

