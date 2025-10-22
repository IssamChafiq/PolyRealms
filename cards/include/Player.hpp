#pragma once
#include <string>
#include <list>
#include "Deck.hpp"
#include "Champion.hpp"
#include "Market.hpp"

class Player {
    std::string name_;
    int authority_;
    int gold_ = 15;
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
    int getAuthority() const {return authority_;};
    void setAuthority(int authority) {authority_ = authority;};
    
    std::list<Card> getHand() const {return hand_;};
    std::list<Card> getDiscardPile() const {return discardPile_;};
    std::list<Card> getInPlay() const {return inPlay_;};
    std::list<Champion> getChampions() const {return champions_;};

    void setHand(std::list<Card> hand) {hand_ = hand;};
    void setDiscardPile(std::list<Card> discardPile) {discardPile_ = discardPile;};
    void setInPlay(std::list<Card> inPlay) {inPlay_ = inPlay;};
    void setChampions(std::list<Champion> champions) {champions_ = champions;};

    void draw(int n);
    void play(Card card);
    void buy(Card card, Market market);
    void attack(Player player, int amount);
    void cleanup();
};
