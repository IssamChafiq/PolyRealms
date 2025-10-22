#pragma once
#include <string>
#include <iostream>
#include <list>
#include "Card.hpp"
#include "Deck.hpp"

class Market {
    std::list<Card> tradeRow_;
    int fireGemStack_ = 16;
    Deck tradeDeck_;
public:
    Market(Deck tradeDeck);
    ~Market();
    Deck getMarketDeck() const;
    void setMarketDeck(Deck deck);
    std::list<Card> getMarketRow() const;
    void setMarketRow(std::list<Card> row);
    void sell(Card card);
};
