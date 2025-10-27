#include "Player.hpp"
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
        if(!market.getMarketRow().empty()){
            gold_ -= card->cost();
            discardPile_.push_back(card);
            market.sell(card);
        } else {
            std::cout << "Le marché est vide";
        }
    } else {
        // Out : pas assez de golds, il faut modifier le player.hpp j'ai mis 15 golds de départ pour que ça marche
    }  
}

void Player::attack(Player player, int amount){
    // Refaire cela pour pouvoir faire des dégats à un joueur et à un ennemi.
    player.setAuthority(player.getAuthority()-amount);
    // Si hp <= 0, game over (géré différement par player et champion, méthode redéfinie par targetable)
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