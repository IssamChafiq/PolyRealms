#include "Player.hpp"
#include "Game.hpp"
#include <iostream>

Player::Player(std::string name,
        int authority,
        Deck deck)
    : name_(name),
      authority_(authority),
      deck_(deck) {}

Player::~Player() = default;

void Player::draw(int n){
    int cardsLeft = deck_.getDeckContents().size();
    if(n >= cardsLeft){
        // Je tire toutes les cartes restantes dans le deck
        std::vector<Card*> listLeft = deck_.draw(cardsLeft);
        hand_.insert(hand_.end(), listLeft.cbegin(), listLeft.cend());
        // Je remélange la défausse dans le deck et je continue à piocher
        deck_.setDeck(discardPile_);
        deck_.shuffle();
        // Je vide la défausse
        discardPile_.clear();
        // Je pioche le reste des cartes
        std::vector<Card*> listNew = deck_.draw(n-cardsLeft);
        hand_.insert(hand_.end(), listNew.cbegin(), listNew.cend());
    } else {
        // Je pioche normalement
        std::vector<Card*> drawn = deck_.draw(n);
        hand_.insert(hand_.end(), drawn.cbegin(), drawn.cend());
    }
}

void Player::play(Card* card){
    bool found = false;
    for (std::vector<Card*>::iterator it = hand_.begin(); it != hand_.end();)
    {
        if (card->id() == (*it)->id()){
            it = hand_.erase(it);
            found = true;
            break;
        }
        else {
            ++it;
        }
    }
    // Ne devrait pas arriver car idéalement on ne sera proposé que des cartes en main, mais par sécurité on vérifie quand même
    if(!found){
        std::cout << "Card not found in hand, cannot play.\n";
        return;
    }
    if(card->isChampion()){
        champions_.push_back(static_cast<Champion*>(card));
    } else {
        inPlay_.push_back(card);
    }
}

void Player::buy(Card* card, Market market){
    if(gold_ >= card->cost()){
        gold_ -= card->cost();
        discardPile_.push_back(card);
        market.sell(card);
    } else {
        std::cout << "Pas assez d'or pour acheter cette carte : elle coûte " << card->cost() << " or et vous en avez " << gold_ << "\n";
        // Out : pas assez de golds, il faut modifier le player.hpp j'ai mis 15 golds de départ pour que ça marche
    }  
}

void Player::godmodeBuy(Card* card, Market market){
    if(gold_ >= card->cost()){
        gold_ -= card->cost();
        hand_.push_back(card);
        market.godmodeSell(card);
    } else {
        std::cout << "Pas assez d'or pour acheter cette carte : elle coûte " << card->cost() << " or et vous en avez " << gold_ << "\n";
        // Out : pas assez de golds, il faut modifier le player.hpp j'ai mis 15 golds de départ pour que ça marche
    }  
}

void Player::attack(Player player, int amount){
    // Refaire cela pour pouvoir faire des dégats à un joueur et à un ennemi.
    player.setAuthority(player.getAuthority()-amount);
    // Si hp <= 0, game over (géré différement par player et champion, méthode redéfinie par targetable)
}

void Player::discard(int amount){
    std::cout << name_ << ", you have to discard " << amount << " cards :\n";
    for (int i=0;i<amount;i++){
        if ((int)hand_.size() > 0){

            for (int i=0;i<(int)hand_.size();i++){
                std::cout << " - " << i+1 << "\n";
                hand_[i]->printCardInfo();
            }

            int discardChoice;
            while(!(std::cin >> discardChoice) || discardChoice < 1 || discardChoice > (int)hand_.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            // Je rajoute la carte dans la pile de discard avant juste pour être sur de pas avoir de problème en supprimant la carte 
            discardPile_.push_back(hand_[discardChoice-1]);
            std::cout << name_ << " has discarded the following card :\n";
            hand_[discardChoice-1]->printCardInfo();

            // Supprime la carte
            for (std::vector<Card*>::iterator it = hand_.begin(); it != hand_.end();)
            {
                // *it sert à récupérer l'objet Card pointé par l'itérateur
                if (hand_[discardChoice-1]->id() == (*it)->id()){
                    it = hand_.erase(it);
                    break;
                } else {
                    ++it;
                }
            }
        } else {
            std::cout << "No cards left in hand : cannot discard anymore";
            break;
        } 
    }
}

void Player::cardEffectSacrifice(int amount){
    std::cout << "You can sacrifice " << amount << " cards :\n";
    for (int i=0;i<amount;i++){
        std::cout << "What do you want to do ? 1. Sacrifice from hand - 2. Sacrifice from discard pile - 3. Do nothing\n";

        int choice;
            while(!(std::cin >> choice) || choice < 1 || choice > 3){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (choice == 3){
            return;
        }

        switch(choice){
            case 1:
                if ((int)hand_.size() > 0){
                    for (int i=0;i<(int)hand_.size();i++){
                        std::cout << " - " << i+1 << "\n";
                        hand_[i]->printCardInfo();
                    }

                    int handSacrificeChoice;
                    while(!(std::cin >> handSacrificeChoice) || handSacrificeChoice < 1 || handSacrificeChoice > (int)hand_.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }

                    // Je rajoute la carte dans la pile de discard avant juste pour être sur de pas avoir de problème en supprimant la carte
                    Game::sacrifice(hand_[handSacrificeChoice-1]); 
                    std::cout << name_ << " has sacrificed the following card :\n";
                    hand_[handSacrificeChoice-1]->printCardInfo();

                    // Supprime la carte
                    for (std::vector<Card*>::iterator it = hand_.begin(); it != hand_.end();)
                    {
                        // *it sert à récupérer l'objet Card pointé par l'itérateur
                        if (hand_[handSacrificeChoice-1]->id() == (*it)->id()){
                            it = hand_.erase(it);
                            break;
                        } else {
                            ++it;
                        }
                    }
                } else {
                    std::cout << "No cards left in hand : cannot sacrifice";
                }
                break;
            case 2:
                if ((int)discardPile_.size() > 0){
                    for (int i=0;i<(int)discardPile_.size();i++){
                        std::cout << " - " << i+1 << "\n";
                        discardPile_[i]->printCardInfo();
                    }

                    int discardSacrificeChoice;
                    while(!(std::cin >> discardSacrificeChoice) || discardSacrificeChoice < 1 || discardSacrificeChoice > (int)hand_.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }

                    // Je rajoute la carte dans la pile de discard avant juste pour être sur de pas avoir de problème en supprimant la carte 
                    Game::sacrifice(discardPile_[discardSacrificeChoice-1]);
                    std::cout << name_ << " has sacrificed the following card :\n";
                    discardPile_[discardSacrificeChoice-1]->printCardInfo();

                    // Supprime la carte
                    for (std::vector<Card*>::iterator it = discardPile_.begin(); it != discardPile_.end();)
                    {
                        // *it sert à récupérer l'objet Card pointé par l'itérateur
                        if (discardPile_[discardSacrificeChoice-1]->id() == (*it)->id()){
                            it = discardPile_.erase(it);
                            break;
                        } else {
                            ++it;
                        }
                    }
                } else {
                    std::cout << "No cards left in hand : cannot sacrifice";
                }
                break;
            default: break;
        }
    }
}

bool Player::isFactionInPlay(Faction faction){
    bool factionInPlay = false;
    for (Card* card : inPlay_){
        if(card->faction() == faction){
            factionInPlay = true;
        }
    }
    for (Champion* champion : champions_){
        if(champion->faction() == faction){
            factionInPlay = true;
        }
    }
    return factionInPlay;
}

void Player::cleanup(){
    for (Champion* champion : champions_){
        champion->heal();
    }
    discardPile_.insert(discardPile_.end(), hand_.cbegin(), hand_.cend());
    discardPile_.insert(discardPile_.end(), inPlay_.cbegin(), inPlay_.cend());
    hand_.clear();
    inPlay_.clear();
    gold_ = 0;
    combat_ = 0;
    // Il faudra aussi réinitialiser les sorts et capacités utilisées pendant le tour
}