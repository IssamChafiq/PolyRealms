#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Card.hpp"

class Deck {
    std::vector<Card*> cards_;
public:
    Deck(std::vector<Card*> cards);
    ~Deck();
    std::vector<Card*> getDeckContents() const;
    void setDeck(std::vector<Card*>);
    void shuffle();
    std::vector<Card*> draw(int n);
    void removeCardFromDeck(Card* card);
};
