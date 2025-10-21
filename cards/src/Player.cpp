#include "include/Player.hpp"
#include <iostream>

Player::Player(std::string name,
        int authority,
        Deck deck)
    : name_(name),
      authority_(authority),
      deck_(deck) {}

Player::~Player() = default;

void Player::draw(int n){
    int cardsLeft = deck_.getDeck.size();
    if(n >= cardsLeft){
        hand_.append_range(deck_.draw(cardsLeft));
        deck_.setDeck(discardPile_);
        deck_.shuffle();
        discardPile_.clear();
        hand_.append_range(deck_.draw(n-cardsLeft));
    } else {
        hand_.append_range(deck_.draw(n));
    }
}

void Player::play(Card card){
    for (std::list<int>::iterator it = hand_.begin(); it != hand_.end();)
    {
        if (card.id() = *it.id())
            it = hand_.erase(it);
            break;
        else
            ++it;
    }
    inPlay_.push_back(card);
}

void Player::buy(Card card, Market market){
    if(gold_ >= card.cost()){
        gold_ -= card.cost();
        discardPile_.push_back(card);
        market.sell(card)
    } else {
        // Out : pas assez de golds
    }  
}

void Player::attack(Player player, int amount){
    // Refaire cela pour pouvoir faire des dégats à un joueur et à un ennemi.
    player.setAuthority(player.getAuthority()-amount);
    // Si hp <= 0, game over (géré différement par player et champion, méthode redéfinie par targetable)
}

void Player::cleanup(){
    for (champion : champions_){
        champion.heal();
    }
    discardPile_.append_range(hand_);
    hand_.clear();
    inPlay_.clear();
    gold_ = 0;
    combat_ = 0;
}