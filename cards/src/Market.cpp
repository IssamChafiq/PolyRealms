#include "Market.hpp"
#include <iostream>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <random>
#include <chrono>

Market::Market(Deck tradeDeck): tradeDeck_(tradeDeck) {
    tradeRow_ = tradeDeck_.draw(5);
}

Market::~Market() = default;

Deck Market::getMarketDeck() const {
    return tradeDeck_;
}

void Market::setMarketDeck(Deck deck){
    tradeDeck_ = deck;
}

std::list<Card> Market::getMarketRow() const {
    return tradeRow_;
}

void Market::setMarketRow(std::list<Card> row){
    tradeRow_ = row;
}

void Market::sell(Card card){
    // J'itère sur la tradeRow pour trouver la carte à vendre
    for (std::list<Card>::iterator it = tradeRow_.begin(); it != tradeRow_.end();)
    {
        // *it sert à récupérer l'objet Card pointé par l'itérateur
        if (card.id() == (*it).id()){
            it = tradeRow_.erase(it);
            break;
        }
        else {
            ++it;
        }
    }
    // On remplit la tradeRow en piochant une carte du tradeDeck
    tradeRow_.push_back(tradeDeck_.draw(1).front());
}
