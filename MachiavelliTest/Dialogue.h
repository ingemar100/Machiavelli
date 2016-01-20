#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "Socket.h"
#include "machiavelli.h"
#include <thread>

class Dialogue
{
private:
	std::string description;
	std::vector<std::string> choices;
	std::shared_ptr<Socket> socket;
	int userInput = -1;

public:
	int activate()
	{

		*socket << "\r\n" << description << "\r\n";

		// Output and number the choices
		for (int i = 0; i < this->choices.size(); ++i) {
			*socket << i << ": " << this->choices[i] << "\r\n";
		}

		*socket << machiavelli_prompt;
		// Repeatedly read input from stdin until a valid option is
		// chosen
		userInput = -1;
		while (true)
		{
			//std::string ui = socket->readline();
			//userInput = std::stoi(socket->readline());
			if (userInput >= 0 && userInput <= this->choices.size() - 1)
			{
				return userInput;
			}
		}
	}

	// Dialogue my_dialogue("Hello", {"Choice1", "Choice"});
	Dialogue(std::string description, std::vector<std::string> choices, std::shared_ptr<Socket> socket)
	{
		this->description = description;
		this->choices = choices;
		this->socket = socket;
	}

	void addChoice(std::string choice)
	{
		this->choices.push_back(choice);
	}

	unsigned int size()
	{
		return this->choices.size();
	}

	bool pick(std::string input) {
		int test = std::stoi(input);
		if (test > choices.size() - 1) {
			*socket << "You wrote: '" << input << "', but that option doesn't exist.\r\n" << machiavelli_prompt;
			return false;
		}
		else {
			userInput = test;
			return true;
		}
	}
};