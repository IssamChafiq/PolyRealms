#pragma once
#include <string>
#include <iostream>
#include <list>
#include "Card.hpp"

class Deck {
    std::list<Card> cards_;
public:
    Deck(std::list<Card> cards);
    ~Deck();
    std::list<Card> getDeck() const;
    void setDeck(std::list<Card>);
    void shuffle();
    std::list<Card> draw(int n);
};
