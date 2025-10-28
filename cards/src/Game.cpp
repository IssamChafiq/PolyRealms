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
    while(!(std::cin >> numPlayers)){
        std::cout << "Invalid input. Please enter a valid choice: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Consommer le newline avant d'utiliser getline pour les noms
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for(int i = 0; i < numPlayers; i++){
        std::string playerName;
        std::cout << "Enter name for player " << (i + 1) << ": ";
        std::getline(std::cin, playerName);
        while(playerName.empty()){
            std::cout << "Name cannot be empty. Enter name for player " << (i + 1) << ": ";
            std::getline(std::cin, playerName);
        }
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
        } else {
            startFFA();
        }
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
                    if(godmode_ == false){
                        std::cout << "Activate godmode ? (y/n)\n";
                        std::string response;
                        std::cin >> response;
                        if(response == "y"){
                            std::cout << "Godmode activated! It allows you to reduce any player's health to 1 and to buy cards from anywhere in the market, putting them directly in your hand ! Type godmode to access the options.\n";
                            godmode_ = true;
                        } else if(response == "n") {
                            continue;
                        } else {
                            std::cout << "Invalid input. Please try again.";
                        } 
                    } else {
                        std::cout << "What would you like to do ?\n1. Set a player's health to 1 - 2. Deactivate godmode\n";
                        int godmodeChoice;
                        while(!(std::cin >> godmodeChoice) || godmodeChoice < 1 || godmodeChoice > 2){
                            std::cout << "Invalid input. Please enter a valid choice: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }

                        if(godmodeChoice == 1){
                            std::cout << "Which player do you want to bring down to 1 hp ? :";
                            for (int i=0;i<(int)playerList_.size();i++){
                                std::cout << " - " << i+1 << ". " << playerList_[i]->getName() << "'s board";
                            }
                            std::cout << "\n";
                            int healthChoice;
                            while(!(std::cin >> healthChoice) || healthChoice < 1 || healthChoice > (int)playerList_.size()){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }
                            playerList_[healthChoice-1]->setAuthority(1);
                        } else if(godmodeChoice == 2){
                            std::cout << "Godmode deactivated !";
                            godmode_ = false;
                        }
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
                        /* 
                            LOOKING AT SOMETHING CASE
                        */
                        std::cout << "What do you want to look at ? :\n1. Market row - 2. Sacrifice pile";
                        for (int i=0;i<(int)playerList_.size();i++){
                            std::cout << " - " << i+3 << ". " << playerList_[i]->getName() << "'s board";
                        }
                        std::cout << "\n";
                        int lookChoice;
                        while(!(std::cin >> lookChoice) || lookChoice < 1 || lookChoice > (int)playerList_.size()+2){
                            std::cout << "Invalid input. Please enter a valid choice: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                        if(lookChoice == 1){
                            std::cout << "Market row :\n";
                            for (Card* card : market_.getMarketRow()){
                                card->printCardInfo();
                            }
                        } else if(lookChoice == 2){
                            for (Card* card : sacrificePile_){
                                card->printCardInfo();
                            }
                        } else {
                            lookAt(playerList_[lookChoice-3]);
                        }
                        break;
                    case 2:
                        /* 
                            BUYING A CARD CASE
                        */
                        if(godmode_){
                            std::cout << "Which card do you want to buy (godmode activated, you can buy from the market deck too) ? :\n";
                            std::cout << "Market row cards :\n";
                            for (int i=0;i<(int)market_.getMarketRow().size();i++){
                                std::cout << " - " << i+1 << "\n";
                                market_.getMarketRow()[i]->printCardInfo();
                            }
                            std::cout << "Market deck cards :\n";
                            for (int i=0;i<(int)market_.getMarketDeck().getDeckContents().size();i++){
                                std::cout << " - " << i+(int)market_.getMarketRow().size()+1 << "\n";
                                market_.getMarketDeck().getDeckContents()[i]->printCardInfo();
                            }
                            int godmodeBuyChoice;
                            while(!(std::cin >> godmodeBuyChoice) || godmodeBuyChoice < 1 || godmodeBuyChoice > (int)market_.getMarketRow().size()+(int)market_.getMarketDeck().getDeckContents().size()){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }
                            if(godmodeBuyChoice <= (int)market_.getMarketRow().size()){
                                player->godmodeBuy(market_.getMarketRow()[godmodeBuyChoice-1],market_);
                            } else {
                                player->godmodeBuy(market_.getMarketDeck().getDeckContents()[godmodeBuyChoice-(int)market_.getMarketRow().size()-1],market_);
                            }
                        } else {
                            std::cout << "Which card do you want to buy ? :\n";
                            for (int i=0;i<(int)market_.getMarketRow().size();i++){
                                std::cout << " - " << i+1 << "\n";
                                market_.getMarketRow()[i]->printCardInfo();
                            }
                            int buyChoice;
                            while(!(std::cin >> buyChoice) || buyChoice < 1 || buyChoice > (int)market_.getMarketRow().size()){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }
                            player->buy(market_.getMarketRow()[buyChoice-1],market_);
                        }
                        break;
                    case 3:
                        /* 
                            PLAYING A CARD CASE
                        */
                        std::cout << "Which card do you want to play ? :\n";
                            for (int i=0;i<(int)player->getHand().size();i++){
                                std::cout << " - " << i+1 << "\n";
                                player->getHand()[i]->printCardInfo();
                            }
                            int playChoice;
                            while(!(std::cin >> playChoice) || playChoice < 1 || playChoice > (int)player->getHand().size()){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }
                            player->play(player->getHand()[playChoice-1]);
                        break;
                    case 4:
                        break;
                    case 5:
                        break; 
                    case 6:
                        turnOver = true;
                        // Logique de fin de tour : phases de défausse et de pioche, on regarde si il y a fin de partie.
                        break;
                }
            }
            if (gameOver){
                return;
            }
        }
    }
}

// Sert à regarder le plateau d'un joueur en particulier
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