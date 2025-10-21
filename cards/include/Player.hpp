#pragma once
#include <string>
#include <list>
#include "Deck.hpp"
#include "Champion.hpp"

class Player {
    std::String name_;
    int authority_;
    int gold_ = 0;
    int combat_ = 0;
    Deck deck_;
    std::list<Card> hand_ = {};
    std::list<Card> discardPile_ = {};
    std::list<Card> inPlay_ = {};
    std::list<Champion> champions_ = {};
public:
    Player(std::string name,
        int authority,
        Deck deck);
    ~Player();
    int getAuthority() const {return authority_};
    void setAuthority(int authority) {authority_ = authority};
    void draw(int n);
    void play(Card card);
    void buy(Card card, market Market);
    void attack(Player player, amount int);
    void cleanup();
};
