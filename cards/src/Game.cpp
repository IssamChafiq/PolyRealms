#include "Game.hpp"
#include "Abilities.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <thread>
#include <chrono>
#include <stdlib.h>

// Déclaration des variables statiques
std::vector<Card*> Game::sacrificePile_ = {};
std::vector<Player*> Game::playerList_ = {};
std::vector<Player*> Game::opponentList_ = {};
std::vector<Card*> Game::fireGems_ = {};

Game::Game(Market market, std::vector<Card*> startingDeck,std::vector<Card*> fireGems): market_(market), startingDeck_(startingDeck) {
    fireGems_ = fireGems;
}

Game::~Game() {
    // Nettoyage de la mémoire
    for (Player* player : playerList_) {
        delete player;
    }
    playerList_.clear();
    
    for (Card* c : sacrificePile_) delete c;
    sacrificePile_.clear();

    for (Card* c : fireGems_) delete c;
    fireGems_.clear();
};

void Game::initialize(){
    int numPlayers;

    std::cout << "Enter number of players (max 4 players): ";
    while(!(std::cin >> numPlayers) || numPlayers < 1 || numPlayers > 4){
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
        Deck deck = Deck(startingDeck_);
        deck.shuffle();
        Player* newPlayer = new Player(playerName, 50, deck);
        playerList_.push_back(newPlayer);
    }

    std::cout << "Which gamemode would you like to play ?\n1. Free For All (2+ players)\n2. First blood (3+ players)\n3. Last standing (3+ players)\n";
    int modeChoice;
    while(!(std::cin >> modeChoice) || modeChoice < 1 || modeChoice > 3){
        std::cout << "Invalid input. Please enter a valid choice: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    if(modeChoice == 1){
        if(numPlayers < 2){
            std::cout << "Free For All requires at least 2 players. Exiting.\n";
            return;
        } else {
            std::cout << "Starting Free For All mode with " << numPlayers << " players.\n";
            std::cout << "The last player alive wins !\n";
            startGame(1);
        }
    } else if (modeChoice == 2){
        if(numPlayers < 3){
            std::cout << "First Blood requires at least 3 players. Exiting.\n";
            return;
        } else {
            std::cout << "Starting First Blood mode with " << numPlayers << " players.\n";
            std::cout << "The player to the right of the first player to die wins !\n";
            startGame(2);
        }
    } else if (modeChoice == 3){
        std::cout << "Starting Last Standing mode with " << numPlayers << " players.\n";
        std::cout << "Just like First Blood, but every time a player dies, the player to its right gains 10 health points and draws a card, and the game ends when there is only one player left!\n";
        startGame(3);
    }
}

void Game::startGame(int mode){
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

            opponentList_.clear();

            if(mode == 1){
                // En Free For All, tous les autres joueurs sont des adversaires
                for (Player* p : playerList_){
                    if(p != player){
                        opponentList_.push_back(p);
                    }
                }
            } else if(mode == 2 || mode == 3){
                // On récupère ici les joueurs à gauche et à droite du joueur courant, si il n'y a que 2 joueurs on prend l'autre joueur comme adversaire unique pour éviter de le dédoubler.
                int id = getPlayerIndex(player);
                int leftId, rightId;
                if((int)playerList_.size() == 2){
                    opponentList_.push_back(playerList_[(id + 1) % 2]);
                } else {
                    if(id == 0){
                        leftId = (int)playerList_.size() - 1;
                    } else {
                        leftId = id - 1;
                    }
                    if(id == (int)playerList_.size() - 1){
                        rightId = 0;
                    } else {
                        rightId = id + 1;
                    }
                    opponentList_ = {playerList_[leftId], playerList_[rightId]};
                }
            }

            std::cout << "\nIt's " << player->getName() << "'s turn.\n";
            // On réutilise les capacités onPlay des champions à chaque tour
            for(Champion* champion : player->getChampions()){
                for(auto& ab : champion->abilities()){
                    if(ab.trigger == Trigger::OnPlay){
                        smartAbilityExecute(player,ab);
                    }
                }
            }
            while(!turnOver){
                // On temporise pour voir l'affichage du changement de joueur avant de clear.
                std::this_thread::sleep_for(std::chrono::seconds(2));
                system("clear||cls");
                std::cout << "\n" << player->getName() << ": Health: " << player->getAuthority() << ", Gold: " << player->getGold() << ", Combat: " << player->getCombat() << ".\n";
                std::cout << "\nChoose an action:\n\033[1;93m1\033[0m. Look at something on the board - \033[1;93m2\033[0m. Buy Card - \033[1;93m3\033[0m. Play Card - \033[1;93m4\033[0m. Attack - \033[1;93m5\033[0m. Use a spell - \033[1;93m6\033[0m. End Turn.\n";
                std::cout << "Type 'godmode' to activate godmode or access its features.\n";
                std::cout << "Type 'exit' or 'quit' to exit the game.\n";
                std::cout << "Your choice: ";
                
                std::string input;
                std::cin >> input;
                
                // Godmode
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
                        std::cout << "\nWhat would you like to do ?\n1. Set a player's health to 1 - 2. Deactivate godmode - 3. Return\n";
                        int godmodeChoice;
                        while(!(std::cin >> godmodeChoice) || godmodeChoice < 1 || godmodeChoice > 3){
                            std::cout << "Invalid input. Please enter a valid choice: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }

                        if(godmodeChoice == 1){
                            std::cout << "Which player do you want to bring down to 1 hp ? :";
                            for (int i=0;i<(int)playerList_.size();i++){
                                std::cout << " - " << i+1 << ". " << playerList_[i]->getName();
                            }
                            std::cout << "\n";
                            int healthChoice;
                            while(!(std::cin >> healthChoice) || healthChoice < 1 || healthChoice > (int)playerList_.size()){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }
                            playerList_[healthChoice-1]->setAuthority(1);
                            std::cout << playerList_[healthChoice-1]->getName() << " now has 1 health point.\n";
                        } else if(godmodeChoice == 2){
                            std::cout << "Godmode deactivated !";
                            godmode_ = false;
                        } else if(godmodeChoice == 3){
                            continue;
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
                    case 1:{
                        /* 
                            LOOKING AT SOMETHING CASE
                        */
                        bool lookOver = false;
                        while (!lookOver){
                            std::cout << "\nWhat do you want to look at ? :\n1. Market row - 2. Sacrifice pile";
                            for (int i=0;i<(int)playerList_.size();i++){
                                std::cout << " - " << i+3 << ". " << playerList_[i]->getName() << "'s board";
                            }
                            std::cout << " - " << (int)playerList_.size()+3 << ". Return.\n";
                            int lookChoice;
                            while(!(std::cin >> lookChoice) || lookChoice < 1 || lookChoice > (int)playerList_.size()+3){
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
                                std::cout << "Sacrifice pile :\n";
                                for (Card* card : sacrificePile_){
                                    card->printCardInfo();
                                }
                            } else if(lookChoice == (int)playerList_.size()+3) {
                                lookOver = true;
                            } else {
                                lookAt(playerList_[lookChoice-3]);
                            }
                        }
                        break;
                    }
                    case 2:{
                        /* 
                            BUYING A CARD CASE
                        */
                        bool buyOver = false;
                        while(!buyOver){
                            if(godmode_){
                                player->setGold(100); // On donne de l'or supplémentaire pour que le joueur puisse acheter librement en godmode (et pour des raisons de tests plus faciles aussi)
                                std::cout << "\nWhich card do you want to buy (godmode activated, you can buy from the market deck too) ? :\n";
                                std::cout << " - 1. Fire Gem ( " << fireGems_.size() << " left):\n";
                                if(fireGems_.size() > 0){
                                    fireGems_.front()->printCardInfo();
                                }
                                std::cout << "Market row cards :\n";
                                for (int i=0;i<(int)market_.getMarketRow().size();i++){
                                    std::cout << " - " << i+2 << ":\n";
                                    market_.getMarketRow()[i]->printCardInfo();
                                }
                                std::cout << "Market deck cards :\n";
                                for (int i=0;i<(int)market_.getMarketDeck().getDeckContents().size();i++){
                                    std::cout << " - " << i+(int)market_.getMarketRow().size()+2 << ":\n";
                                    market_.getMarketDeck().getDeckContents()[i]->printCardInfo();
                                }
                                std::cout << " - " << market_.getMarketRow().size()+market_.getMarketDeck().getDeckContents().size()+2 << ". Return.\n";
                                std::cout << "You have " << player->getGold() << " gold.\n";
                                int godmodeBuyChoice;
                                while(!(std::cin >> godmodeBuyChoice) || godmodeBuyChoice < 1 || godmodeBuyChoice > (int)market_.getMarketRow().size()+(int)market_.getMarketDeck().getDeckContents().size()+2){
                                    std::cout << "Invalid input. Please enter a valid choice: ";
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                }
                                if(godmodeBuyChoice == 1){
                                    if((int)fireGems_.size() > 0){
                                        if(player->godmodeBuy(fireGems_.front(),market_)){
                                            fireGems_.erase(fireGems_.begin());
                                            // On temporise l'affichage du marché à nouveau
                                            std::this_thread::sleep_for(std::chrono::seconds(2));
                                        }
                                    } else {
                                        std::cout << "Not any fire gems left.\n";
                                    }
                                } else if(godmodeBuyChoice == (int)market_.getMarketRow().size()+(int)market_.getMarketDeck().getDeckContents().size()+2){
                                    buyOver = true;
                                } else if(godmodeBuyChoice <= (int)market_.getMarketRow().size()+1){
                                    player->godmodeBuy(market_.getMarketRow()[godmodeBuyChoice-2],market_);
                                    // On temporise l'affichage du marché à nouveau
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                } else {
                                    player->godmodeBuy(market_.getMarketDeck().getDeckContents()[godmodeBuyChoice-(int)market_.getMarketRow().size()-2],market_);
                                    // On temporise l'affichage du marché à nouveau
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                }
                            } else {
                                std::cout << "\nWhich card do you want to buy ? :\n";
                                std::cout << " - 1. Fire Gem ( " << fireGems_.size() << " left):\n";
                                if(fireGems_.size() > 0){
                                    fireGems_.front()->printCardInfo();
                                }
                                for (int i=0;i<(int)market_.getMarketRow().size();i++){
                                    std::cout << " - " << i+2 << ":\n";
                                    market_.getMarketRow()[i]->printCardInfo();
                                }
                                std::cout << " - " << market_.getMarketRow().size()+2 << ". Return.\n";
                                std::cout << "You have " << player->getGold() << " gold.\n";
                                int buyChoice;
                                while(!(std::cin >> buyChoice) || buyChoice < 1 || buyChoice > (int)market_.getMarketRow().size()+2){
                                    std::cout << "Invalid input. Please enter a valid choice: ";
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                }
                                if(buyChoice == 1){
                                    if(fireGems_.size() > 0){
                                        if(player->buy(fireGems_.front(),market_)){
                                            fireGems_.erase(fireGems_.begin());
                                        }
                                    } else {
                                        std::cout << "Not any fire gems left.\n";
                                    }
                                } else if(buyChoice == (int)market_.getMarketRow().size()+2) {
                                    buyOver = true;
                                } else {
                                    player->buy(market_.getMarketRow()[buyChoice-2],market_);
                                    // On temporise l'affichage du marché à nouveau
                                    std::this_thread::sleep_for(std::chrono::seconds(1));
                                }
                            }
                        }
                        break;
                    }
                    case 3:{
                        /* 
                            PLAYING A CARD CASE
                        */
                        bool playOver = false;
                        while(!playOver){
                            std::cout << "\nWhich card do you want to play ? :\n";
                            for (int i=0;i<(int)player->getHand().size();i++){
                                std::cout << " - " << i+1 << ":\n";
                                player->getHand()[i]->printCardInfo();
                            }
                            std::cout << " - " << player->getHand().size()+1 << ". Return.\n";
                            int playChoice;
                            while(!(std::cin >> playChoice) || playChoice < 1 || playChoice > (int)player->getHand().size()+1){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }

                            if(playChoice == (int)player->getHand().size()+1){
                                playOver = true;
                            } else {
                                Card* chosenCard = player->getHand()[playChoice-1];

                                // On enlève la carte de la main et on la place au bon endroit sur le plateau de jeu
                                player->play(player->getHand()[playChoice-1]);

                                // Utilisation des capacités onPlay et Ally de la carte
                                for(auto& ab : chosenCard->abilities()){
                                    if(ab.trigger == Trigger::OnPlay){
                                        smartAbilityExecute(player,ab);
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case 4:{
                        /* 
                            ATTACK A PLAYER CASE
                        */
                        bool attackOver = false;
                        while(!attackOver){
                            if(mode == 2 || mode == 3){
                                std::cout << "In this mode, you can only attack the players to your left and right. The first player is to your left and the second is to your right.\n";
                            }
                            std::cout << "\nWho do you want to attack ? (Combat power : " << player->getCombat() << ") :";
                            for (int i=0;i<(int)opponentList_.size();i++){
                                std::cout << " - " << i+1 << ". " << opponentList_[i]->getName() << "'s board";
                            }
                            std::cout << " - " << (int)opponentList_.size()+1 << ". Return.\n";

                            int attackChoice;
                            while(!(std::cin >> attackChoice) || attackChoice < 1 || attackChoice > (int)opponentList_.size()+1){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }

                            if (attackChoice == (int)opponentList_.size()+1){
                                attackOver = true;
                            } else if (player == opponentList_[attackChoice-1]){
                                // Il est techniquement impossible d'attaquer soi-même avec la configuration actuelle, mais je garde cette vérification au cas où
                                std::cout << "You cannot attack yourself\n";
                            } else if((mode == 2 && attackChoice > 1) || (mode == 3 && attackChoice > 1)){
                                std::cout << "Attacking " << opponentList_[attackChoice-1]->getName() << "'s board.\n";
                                player->attack(opponentList_[attackChoice-1], true);
                            } else {
                                std::cout << "Attacking " << opponentList_[attackChoice-1]->getName() << "'s board.\n";
                                player->attack(opponentList_[attackChoice-1], false);

                                // On met à jour la liste des joueurs morts
                                std::vector<Player*> toRemove;
                                for (Player* p : opponentList_) {
                                    if (p->getAuthority() <= 0) {
                                        std::cout << "\nThe player " << p->getName() << " has died.\n";
                                        toRemove.push_back(p);
                                        if(mode == 3){
                                            int deadId = getPlayerIndex(p);
                                            int rightId;
                                            if(deadId == (int)playerList_.size() - 1){
                                                rightId = 0;
                                            } else {
                                                rightId = deadId + 1;
                                            }
                                            playerList_[rightId]->setAuthority(playerList_[rightId]->getAuthority() + 10);
                                            std::cout << playerList_[rightId]->getName() << " gains 10 health points and now has " << playerList_[rightId]->getAuthority() << " health points.\n";
                                            playerList_[rightId]->draw(1);
                                        }
                                    }
                                }

                                // Condition de victoire pour le mode First Blood
                                if(mode == 2 && toRemove.size() > 0){
                                    std::cout << "\nThe game is over ! The winner is ";
                                    int deadId = getPlayerIndex(toRemove[0]);
                                    int winnerId;
                                    if(deadId == (int)playerList_.size() - 1){
                                        winnerId = 0;
                                    } else {
                                        winnerId = deadId + 1;
                                    }
                                    std::cout << playerList_[winnerId]->getName() << " ! Congratulations !\n";
                                    gameOver = true;
                                    turnOver = true;
                                    attackOver = true;
                                }
                                
                                // On retire les joueurs morts de la partie
                                for (Player* dead : toRemove) {
                                    auto it = std::find(playerList_.begin(), playerList_.end(), dead);
                                    if (it != playerList_.end()) playerList_.erase(it);
                                }

                                // On vérifie si la partie est finie (dernier joueur debout)
                                if((int)playerList_.size() == 1){
                                    std::cout << "\nThe game is over ! The winner is " << playerList_[0]->getName() << " ! Congratulations !\n";
                                    turnOver = true;
                                    gameOver = true;
                                    attackOver = true;
                                }
                            }
                        }
                        break;
                    }
                    case 5:{
                        /* 
                            USE AN ABILITY CASE
                        */
                        bool abilityOver = false;
                        while(!abilityOver){
                            std::cout << "\nWhich card's ability do you want to use ?\n";
                            std::cout << "Champion cards :\n";
                            for (int i=0;i<(int)player->getChampions().size();i++){
                                std::cout << " - " << i+1 << "\n";
                                player->getChampions()[i]->printCardInfo();
                            }
                            std::cout << "In play cards :\n";
                            for (int i=0;i<(int)player->getInPlay().size();i++){
                                std::cout << " - " << i+(int)player->getChampions().size()+1 << "\n";
                                player->getInPlay()[i]->printCardInfo();
                            }
                            std::cout << " - " << (int)player->getChampions().size()+(int)player->getInPlay().size()+1 << ". Return.\n";
                            int cardChoice;
                            while(!(std::cin >> cardChoice) || cardChoice < 1 || cardChoice > (int)player->getChampions().size()+(int)player->getInPlay().size()+1){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }

                            if(cardChoice == (int)player->getChampions().size()+(int)player->getInPlay().size()+1){
                                abilityOver = true;
                            } else {
                                player->useAbility(cardChoice);
                                // On temporise un peu l'affichage pour que le joueur ait le temps de lire ce qui s'est passé
                                std::this_thread::sleep_for(std::chrono::seconds(2));
                            }
                        }
                        break; 
                    }
                    case 6:{
                        /* 
                            END TURN CASE
                        */
                        std::cout << "\nAre you sure you want to end your turn ? (y/n)\n";
                        // On rappelle au joueur ce qu'il lui reste avant de terminer son tour
                        std::cout << "You have " << player->getGold() << " gold and " << player->getCombat() << " combat power left.\n";

                        // Verification du combat restant
                        if(player->getCombat() > 0){
                            std::cout << "Remember that any unused combat power will be lost at the end of your turn.\n";
                        }

                        // Verification des cartes achetables dans le marché
                        bool buyableInMarket = false;
                        for(Card* card : market_.getMarketRow()){
                            if(card->cost() <= player->getGold()){
                                buyableInMarket = true;
                                break;
                            }
                        }
                        if(buyableInMarket || (fireGems_.size() > 0 && player->getGold() >= 2)){
                            std::cout << "There are still cards you can afford in the shop !\n";
                        }

                        // Verification des cartes jouables en main
                        if(player->getHand().size() > 0){
                            std::cout << "You still have unplayed cards in your hand !\n";
                        }

                        // Verification des capacités non utilisées
                        bool unusedAbilities = false;
                        for(Champion* champion : player->getChampions()){
                            for(auto& ab : champion->abilities()){
                                if(!ab.used){
                                    unusedAbilities = true;
                                    break;
                                }
                            }
                            if(unusedAbilities) break;
                        }
                        for(Card* card : player->getInPlay()){
                            for(auto& ab : card->abilities()){
                                if(!ab.used){
                                    unusedAbilities = true;
                                    break;
                                }
                            }
                            if(unusedAbilities) break;
                        }

                        if(unusedAbilities){
                            std::cout << "You have unused card abilities that you can still use this turn !\n";
                        }

                        std::string endResponse;
                        std::cin >> endResponse;
                        if(endResponse == "y" || endResponse == "Y"){
                            std::cout << "Ending " << player->getName() << "'s turn.\n";
                            player->cleanup(); // phase de défausse
                            player->draw(5); // phase de pioche
                            turnOver = true;
                        }
                        break;
                    } 
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
    std::cout << player->getName() << ":\nHealth : " << player->getAuthority() << "\nGold : " << player->getGold() << "\nCombat : " << player->getCombat() << ".\n";
    std::cout << "\nWhich part of " << player->getName() << "'s board do you want to look at ?\n 1. Hand - 2. Played cards - 3. Active champions - 4. Discard pile - 5. Deck - 6. Return\n";
    int choice;
    while(!(std::cin >> choice) || choice < 1 || choice > 6){
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
        case 6:
            break;
    }
}

// Sert juste à mettre des cartes dans la pile de sacrifices
void Game::sacrifice(Card* card){
    // On traite les gemmes de feu à part, puisqu'elles vont dans une pile dédiée
    if(card->name() == "Fire Gem"){
        fireGems_.push_back(card);
        for (auto& ab : card->abilities()){
            ab.used = false;
        }
    } else {
        sacrificePile_.push_back(card);
    }
    std::cout << "Card sacrificed :\n";
    card->printCardInfo();
}

// Sert à exécuter une capacité et à demander un ennemi à cibler si besoin.
bool Game::smartAbilityExecute(Player* player, Card::CardAbility& ab){
    // Je regarde ici si la capacité est parmi celles qui ont besoin d'un opponent (il n'y en a que deux, donc je me retiens de faire quelque chose de plus compliqué que ça...)
    if(ab.ability == AbilityName::OpponentDiscard || ab.ability == AbilityName::StunTargetChampion){
        std::cout << "\nWhich player do you want to use '" << Card::abilityNameToString(ab.ability) << "' on ?\n";
        for (int i=0;i<(int)opponentList_.size();i++){
            std::cout << " - " << i+1 << ". " << opponentList_[i]->getName();
        }
        std::cout << "\n";
        int spellChoice;
        while(!(std::cin >> spellChoice) || spellChoice < 1 || spellChoice > (int)opponentList_.size()){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if (opponentList_[spellChoice-1] == player){
            std::cout << "You cannot target yourself with this ability.\n";
        } else {
            if(Abilities::execute(player,ab.ability,opponentList_[spellChoice-1],ab.amount)){
                std::cout << "Ability used :\n";
                ab.printAbility();
                ab.used = true;
                return true;
            } else {
                std::cout << "Couldn't use the following ability, you may try to use it again after making sure you are able to use it :\n";
                ab.printAbility();
                return false;
            }
        }
    } else {
        if(Abilities::execute(player,ab.ability,player,ab.amount)){
            std::cout << "Ability used :\n";
            ab.printAbility();
            ab.used = true;
            return true;
        } else {
            std::cout << "Couldn't use the following ability, you may try to use it again after making sure you are able to use it :\n";
            ab.printAbility();
            return false;
        }
    }
    return false;
}

int Game::getPlayerIndex(Player* player){
    for (int i=0;i<(int)playerList_.size();i++){
        if (playerList_[i] == player){
            return i;
        }
    }
    return -1;
}