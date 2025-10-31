#include "Game.hpp"
#include "Abilities.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>

// Déclaration de la sacrificePile_ statique afin de pouvoir y accéder dans Game::sacrifice
std::vector<Card*> Game::sacrificePile_ = {};
std::vector<Player*> Game::playerList_ = {};
int Game::fireGemStack_ = 16;

Game::Game(Market market, std::vector<Card*> startingDeck, Card* fireGem): market_(market), startingDeck_(startingDeck), fireGem_(fireGem) {}

Game::~Game() {
    for (Player* player : playerList_) {
        delete player;
    }
    playerList_.clear();
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

    std::cout << "Which gamemode would you like to play ?\n1. Free For All (2+ players).\n";
    int modeChoice;
    while(!(std::cin >> modeChoice) || modeChoice < 1 || modeChoice > 1){
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
            std::cout << "\n\nIt's " << player->getName() << "'s turn.\n";
            // On réutilise les capacités onPlay des champions à chaque tour
            for(Champion* champion : player->getChampions()){
                for(auto& ab : champion->abilities()){
                    if(ab.trigger == Trigger::OnPlay){
                        smartAbilityExecute(player,ab);
                    }
                }
            }
            while(!turnOver){
                std::cout << "Choose an action:\n\n1. Look at something on the board - 2. Buy Card - 3. Play Card - 4. Use a spell - 5. Attack - 6. End Turn.\n";
                std::cout << "Type 'godmode' to activate godmode or access its features.\n";
                std::cout << "Type 'exit' or 'quit' to exit the game.\n";
                
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
                        std::cout << "What would you like to do ?\n1. Set a player's health to 1 - 2. Deactivate godmode - 3. Return\n";
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
                            std::cout << playerList_[healthChoice-1] << " now has 1 health point.\n";
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
                            std::cout << " - 1. Fire Gem ( " << fireGemStack_ << " left):\n";
                            fireGem_->printCardInfo();
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
                            std::cout << "You have " << player->getGold() << " gold.\n";
                            int godmodeBuyChoice;
                            while(!(std::cin >> godmodeBuyChoice) || godmodeBuyChoice < 1 || godmodeBuyChoice > (int)market_.getMarketRow().size()+(int)market_.getMarketDeck().getDeckContents().size()+1){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }
                            if(godmodeBuyChoice == 1){
                                if(fireGemStack_ > 0){
                                    if(player->godmodeBuy(fireGem_,market_)){
                                        fireGemStack_ -= 1;
                                    }
                                } else {
                                    std::cout << "Not any fire gems left.\n";
                                }
                            } else if(godmodeBuyChoice <= (int)market_.getMarketRow().size()+1){
                                player->godmodeBuy(market_.getMarketRow()[godmodeBuyChoice-2],market_);
                            } else {
                                player->godmodeBuy(market_.getMarketDeck().getDeckContents()[godmodeBuyChoice-(int)market_.getMarketRow().size()-2],market_);
                            }
                        } else {
                            std::cout << "Which card do you want to buy ? :\n";
                            std::cout << " - 1. Fire Gem ( " << fireGemStack_ << " left):\n";
                            fireGem_->printCardInfo();
                            for (int i=0;i<(int)market_.getMarketRow().size();i++){
                                std::cout << " - " << i+2 << ":\n";
                                market_.getMarketRow()[i]->printCardInfo();
                            }
                            int buyChoice;
                            while(!(std::cin >> buyChoice) || buyChoice < 1 || buyChoice > (int)market_.getMarketRow().size()+1){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }
                            if(buyChoice == 1){
                                if(fireGemStack_ > 0){
                                    if(player->buy(fireGem_,market_)){
                                        fireGemStack_ -= 1;
                                    }
                                } else {
                                    std::cout << "Not any fire gems left.\n";
                                }
                            } else {
                                player->buy(market_.getMarketRow()[buyChoice-2],market_);
                            }
                        }
                        break;
                    case 3:
                        /* 
                            PLAYING A CARD CASE
                        */
                        std::cout << "Which card do you want to play ? :\n";
                            for (int i=0;i<(int)player->getHand().size();i++){
                                std::cout << " - " << i+1 << ":\n";
                                player->getHand()[i]->printCardInfo();
                            }
                            int playChoice;
                            while(!(std::cin >> playChoice) || playChoice < 1 || playChoice > (int)player->getHand().size()){
                                std::cout << "Invalid input. Please enter a valid choice: ";
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            }

                            // Utilisation des capacités onPlay et Ally de la carte
                            for(auto& ab : player->getHand()[playChoice-1]->abilities()){
                                if(ab.trigger == Trigger::OnPlay){
                                    smartAbilityExecute(player,ab);
                                }
                            }

                            // On enlève la carte de la main et on la place au bon endroit sur le plateau de jeu
                            player->play(player->getHand()[playChoice-1]);
                        break;
                    case 4:
                        /* 
                            ATTACK A PLAYER CASE
                        */
                        std::cout << "Who do you want to attack ? : (Combat power : " << player->getCombat() << ")\n";
                        for (int i=0;i<(int)playerList_.size();i++){
                            std::cout << " - " << i+1 << ". " << playerList_[i]->getName();
                        }
                        std::cout << "\n";
                        int attackChoice;
                        while(!(std::cin >> attackChoice) || attackChoice < 1 || attackChoice > (int)playerList_.size()){
                            std::cout << "Invalid input. Please enter a valid choice: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                        if (player == playerList_[attackChoice-1]){
                            std::cout << "You cannot attack yourself\n";
                        } else {
                            std::cout << "Attacking " << playerList_[attackChoice-1]->getName() << "\n";
                            player->attack(playerList_[attackChoice-1]);
                        }
                        break;
                    case 5:
                        /* 
                            USE AN ABILITY CASE
                        */
                        std::cout << "Which card's ability do you want to use ?\n";
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

                        // Return case
                        if(cardChoice == (int)player->getChampions().size()+(int)player->getInPlay().size()+1){
                            break;
                        }
                        player->useAbility(cardChoice);
                        break; 
                    case 6:
                        // Logique de fin de tour : phases de défausse et de pioche, on regarde si il y a fin de partie.
                        for (Player* p : playerList_){
                            if(p->getAuthority() <= 0){
                                std::cout << "The player " << p->getName() << " has died.\n";
                                for (std::vector<Player*>::iterator it = playerList_.begin(); it != playerList_.end();)
                                {
                                    // *it sert à récupérer l'objet Card pointé par l'itérateur
                                    if (p->getName() == (*it)->getName()){
                                        it = playerList_.erase(it);
                                        break;
                                    } else {
                                            ++it;
                                    }
                                }
                            }
                        }
                        if((int)playerList_.size() == 1){
                            std::cout << "Congratulations ! The player " << playerList_[0]->getName() << " has won the game !\n";
                            gameOver = true;
                        }
                        player->cleanup(); // phase de défausse
                        player->draw(5); // phase de pioche
                        turnOver = true;
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
    std::cout << player->getName() << ":\nHealth : " << player->getAuthority() << "\nGold : " << player->getGold() << "\nCombat : " << player->getCombat() << ".\n";
    std::cout << "Which part of " << player->getName() << "'s board do you want to look at ?\n 1. Hand - 2. Played cards - 3. Active champions - 4. Discard pile - 5. Deck - 6. Return\n";
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
    if(card->name() == "Fire Gem"){
        fireGemStack_ += 1;
        // Les fire gems ont toutes le même id dans cette implémentation du jeu.
        // Je m'assure donc ici qu'il n'y ait jamais de problème où un joueur ne peut pas utiliser sa fire gem car un autre l'a utilisé avant en remettant toujours la capacité à non used après un sacrifice.
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
void Game::smartAbilityExecute(Player* player, Card::CardAbility& ab){
    // Je regarde ici si la capacité est parmi celles qui ont besoin d'un opponent (il n'y en a que deux, donc je me retiens de faire quelque chose de plus compliqué que ça...)
    if(Card::abilityNameToString(ab.ability) == "OpponentDiscard" || Card::abilityNameToString(ab.ability) == "StunTargetChampion"){
        std::cout << "Which player do you want to use " << Card::abilityNameToString(ab.ability) << " on ?\n";
        for (int i=0;i<(int)playerList_.size();i++){
            std::cout << " - " << i+1 << ". " << playerList_[i]->getName();
        }
        std::cout << "\n";
        int spellChoice;
        while(!(std::cin >> spellChoice) || spellChoice < 1 || spellChoice > (int)playerList_.size()){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if (playerList_[spellChoice-1] == player){
            std::cout << "You cannot target yourself with this ability.\n";
        } else {
            Abilities::execute(player,ab.ability,playerList_[spellChoice-1],ab.amount);
            std::cout << "Ability used :\n";
            ab.printAbility();
            ab.used = true;
        }
    } else {
        Abilities::execute(player,ab.ability,player,ab.amount);
        std::cout << "Ability used :\n";
        ab.printAbility();
        ab.used = true;
    }
}