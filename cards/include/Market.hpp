#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Card.hpp"
#include "Deck.hpp"

class Market {
    std::vector<Card*> tradeRow_;
    Deck tradeDeck_;
public:
    Market(Deck tradeDeck);
    ~Market();
    Deck getMarketDeck() const;
    void setMarketDeck(Deck deck);
    std::vector<Card*> getMarketRow() const;
    void setMarketRow(std::vector<Card*> row);
    void godmodeSell(Card* card);
    void sell(Card* card);
};
