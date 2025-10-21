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
    Market(std::list<Card> cards);
    ~Market();
    void sell(Card card);
};
