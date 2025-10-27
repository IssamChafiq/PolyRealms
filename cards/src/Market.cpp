#include "Market.hpp"
#include <iostream>
#include <string>
#include <vector>

Market::Market(Deck tradeDeck): tradeDeck_(tradeDeck) {
    tradeDeck_.shuffle();
    tradeRow_ = tradeDeck_.draw(5);
}

Market::~Market() = default;

Deck Market::getMarketDeck() const {
    return tradeDeck_;
}

void Market::setMarketDeck(Deck deck){
    tradeDeck_ = deck;
}

std::vector<Card*> Market::getMarketRow() const {
    return tradeRow_;
}

void Market::setMarketRow(std::vector<Card*> row){
    tradeRow_ = row;
}

void Market::sell(Card* card){
    // J'itère sur la tradeRow pour trouver la carte à vendre
    for (std::vector<Card*>::iterator it = tradeRow_.begin(); it != tradeRow_.end();)
    {
        // *it sert à récupérer l'objet Card pointé par l'itérateur
        if (card->id() == (*it)->id()){
            it = tradeRow_.erase(it);
            break;
        }
        else {
            ++it;
        }
    }
    // On remplit la tradeRow en piochant une carte du tradeDeck
    if(tradeDeck_.draw(1).empty()){
        std::cout << "Le deck du marché est vide, impossible de remplir la ligne";
    } else {
        tradeRow_.push_back(tradeDeck_.draw(1)[0]);
    }
}
