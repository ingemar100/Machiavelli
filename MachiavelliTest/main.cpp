//
//  main.cpp
//  socketexample
//
//  Created by Bob Polis on 16/09/14.
//  Copyright (c) 2014 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//
#pragma once
#include <thread>
#include <iostream>
#include <exception>
#include <memory>
#include <utility>
#include <vector>
using namespace std;

#include "Socket.h"
#include "Sync_queue.h"
#include "ClientCommand.h"
#include "Player.hpp"
#include "Game.h"
#include "machiavelli.h"


namespace machiavelli {
	const int tcp_port{ 1080 };
	std::shared_ptr<Game> game = nullptr;
}
static Sync_queue<ClientCommand> queue;

void consume_command() // runs in its own thread
{
    try {
        while (true) {
			ClientCommand command {queue.get()}; // will block here unless there are still command objects in the queue
			shared_ptr<Socket> client {command.get_client()};
			shared_ptr<Player> player {command.get_player()};
			try {
				// TODO handle command here
				//machiavelli::game->handleCommand(command);
				thread commandThread{ &Game::handleCommand, machiavelli::game, command };
				commandThread.detach();
			} catch (const exception& ex) {
				cerr << "*** exception in consumer thread for player " << player->get_name() << ": " << ex.what() << '\n';
				if (client->is_open()) {
					client->write("Sorry, something went wrong during handling of your request.\r\n");
				}
			} catch (...) {
				cerr << "*** exception in consumer thread for player " << player->get_name() << '\n';
				if (client->is_open()) {
					client->write("Sorry, something went wrong during handling of your request.\r\n");
				}
			}
        }
    } catch (...) {
        cerr << "consume_command crashed\n";
    }
}

void handle_client(shared_ptr<Socket> client) // this function runs in a separate thread
{
    try {
        client->write("Welcome to Server 1.0! To quit, type 'quit'.\r\n");
		client->write("What's your name?\r\n");
		client->write(machiavelli_prompt);
		string name {client->readline()};

		client->write("How old are you?\r\n");

		bool valid = false;
		int age = 0;
		while (!valid) {
			client->write(machiavelli_prompt);
			string input{ client->readline() };
			if (isdigit(input[0])) {
				valid = true;
				age = std::stoi(input);
			}
		};

		shared_ptr<Player> player {new Player {name, age, client, machiavelli::game}};

		machiavelli::game->addPlayer(player);
		*client << "Welcome, " << name << ", use the 'start' command to begin playing our game!\r\n" << machiavelli_prompt;

        while (true) { // game loop
            try {
				string cmd;
                // read first line of request
				cmd = client->readline();

				cerr << '[' << client->get_dotted_ip() << " (" << client->get_socket() << ") " << player->get_name() << "] " << cmd << '\n';

				if (cmd == "quit") {
					client->write("Bye!\r\n");
					//remove player
					machiavelli::game->removePlayer(player);
					break;
				}

				ClientCommand command{ cmd, client, player };
				queue.put(command);

            } catch (const exception& ex) {
				cerr << "*** exception in client handler thread for player " << player->get_name() << ": " << ex.what() << '\n';
				if (client->is_open()) {
					*client << "ERROR: " << ex.what() << "\r\n";
				}
            } catch (...) {
				cerr << "*** exception in client handler thread for player " << player->get_name() << '\n';
				if (client->is_open()) {
					client->write("ERROR: something went wrong during handling of your request. Sorry!\r\n");
				}
            }
        }
		if (client->is_open()) client->close();
	} catch (...) {
        cerr << "handle_client crashed\n";
    }
}

int main(int argc, const char * argv[])
{
    // start command consumer thread
    thread consumer {consume_command};

    // keep client threads here, so we don't need to detach them
    vector<thread> handlers;
    
	// create a server socket
	ServerSocket server {machiavelli::tcp_port};

	machiavelli::game = make_shared<Game>();
	
	while (true) {
		try {
			while (true) {
				// wait for connection from client; will create new socket
				cerr << "server listening" << '\n';
				unique_ptr<Socket> client {server.accept()};

				// communicate with client over new socket in separate thread
                thread handler {handle_client, move(client)};
				handlers.push_back(move(handler));
			}
		} catch (const exception& ex) {
			cerr << ex.what() << ", resuming..." << '\n';
        } catch (...) {
            cerr << "problems, problems, but: keep calm and carry on!\n";
        }
	}
    return 0;
}

