#include "Game.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>

Game::Game(Market market): market_(market) {}

Game::~Game() = default;

void Game::initialize(){
    int numPlayers;
    std::cout << "Enter number of players: ";
    std::cin >> numPlayers;
    for(int i = 0; i < numPlayers; i++){
        std::string playerName;
        std::cout << "Enter name for player " << (i + 1) << ": ";
        std::cin >> playerName;
        Deck initialDeck = Deck::createInitialDeck();
        Player* newPlayer = new Player(playerName, 50, initialDeck);
        playerList_.push_back(newPlayer);
    }

    std::cout << "Which gamemode would you like to play ?\n1. Free For All (2+ players)\n";
    int modeChoice;
    while(!(std::cin >> modeChoice)){
        std::cout << "Invalid input. Please enter a valid choice: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    if(modeChoice == 1){
        if(numPlayers < 2){
            std::cout << "Free For All requires at least 2 players. Exiting.\n";
            return;
        }
        startFFA();
    } else {
        std::cout << "Invalid choice. Exiting.\n";
    }
}

void Game::startFFA(){
    bool gameOver = false;

    if(playerList_.size() == 2){
        playerList_[0]->draw(3);
        playerList_[1]->draw(5);
    } else {
        playerList_[0]->draw(3);
        playerList_[1]->draw(4);
        for(int i = 2; i < (int)playerList_.size(); i++){
            playerList_[i]->draw(5);
        }
    }

    while(!gameOver){
        for(Player* player : playerList_){
            bool turnOver = false;
            std::cout << "It's " << player->getName() << "'s turn.\n";
            while(!turnOver){
                std::cout << "Choose an action:\n1. Look at something on the board - 2. Buy Card - 3. Play Card - 4. Use a spell - 5. Attack - 6. End Turn\n";
                std::cout << "Or type 'godmode' to activate godmode or access its features\n";
                
                std::string input;
                std::cin >> input;
                
                if(input == "godmode"){
                    std::cout << "Activate godmode ? (y/n)\n";
                    std::string response;
                    std::cin >> response;
                    if(response == "y"){
                        std::cout << "Godmode activated!\n";
                    }
                    continue;
                } else if(input == "quit" || input == "exit"){
                    std::cout << "Exiting game...\n";
                    gameOver = true;
                    turnOver = true;
                    break;
                }
                
                int actionChoice;
                std::stringstream ss(input);
                if(!(ss >> actionChoice) || actionChoice < 1 || actionChoice > 6){
                    std::cout << "Invalid input. Please enter a number between 1 and 6: ";
                    continue;
                }

                switch(actionChoice){
                    case 1:
                        std::cout << "What do you want to look at ? :\n1. Market row - 2. Sacrifice pile";
                        for (int i=0;i<(int)playerList_.size();i++){
                            std::cout << " - " << i+3 << ". " << playerList_[i] << "'s board";
                        }
                        std::cout << "\n";
                        int choice;
                        while(!(std::cin >> choice) || choice < 1 || choice > (int)playerList_.size()+2){
                            std::cout << "Invalid input. Please enter a valid choice: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                        if(choice == 1){
                            std::cout << "Market row :\n";
                            for (Card* card : market_.getMarketRow()){
                                card->printCardInfo();
                            }
                        } else if(choice == 2){
                            for (Card* card : sacrificePile_){
                                card->printCardInfo();
                            }
                        } else {
                            lookAt(playerList_[choice-3]);
                        }
                        break;
                    case 2:
                        // Buy Card logic
                        break;
                    case 3:
                        // Attack Player logic
                        break;
                    case 4:
                        turnOver = true;
                        break;
                    default:
                        std::cout << "Invalid choice. Please try again.\n";
                }
            }
        }
    }
}

void Game::lookAt(Player* player){
    std::cout << player->getName() << ":\nHealth : " << player->getAuthority() << "\nGold : " << player->getGold() << "\nCombat : " << player->getCombat();
    std::cout << "Which part of " << player->getName() << "'s board do you want to look at ?\n 1. Hand - 2. Played cards - 3. Active champions - 4. Discard pile - 5. Deck";
    int choice;
    while(!(std::cin >> choice) || choice < 1 || choice > 5){
        std::cout << "Invalid input. Please enter a valid choice: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    switch(choice){
        case 1:
            std::cout << player->getName() << "'s hand:\n";
            for (Card* card : player->getHand()){
                card->printCardInfo();
            }
            break;
        case 2:
            std::cout << player->getName() << "'s played cards:\n";
            for (Card* card : player->getInPlay()){
                card->printCardInfo();
            }
            break;
        case 3:
            std::cout << player->getName() << "'s active champions:\n";
            for (Champion* champion : player->getChampions()){
                champion->printCardInfo();
            }
            break;
        case 4:
            std::cout << player->getName() << "'s discard pile:\n";
            for (Card* card : player->getDiscardPile()){
                card->printCardInfo();
            }
            break;
        case 5:
            std::cout << player->getName() << "'s deck:\n";
            for (Card* card : player->getDeck().getDeckContents()){
                card->printCardInfo();
            }
            break;
    }
}