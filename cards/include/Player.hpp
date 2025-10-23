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
    std::vector<Card*> hand_ = {};
    std::vector<Card*> discardPile_ = {};
    std::vector<Card*> inPlay_ = {};
    std::vector<Champion*> champions_ = {};
public:
    Player(std::string name,
        int authority,
        Deck deck);
    ~Player();
    int getAuthority() const {return authority_;};
    void setAuthority(int authority) {authority_ = authority;};
    
    std::vector<Card*> getHand() const {return hand_;};
    std::vector<Card*> getDiscardPile() const {return discardPile_;};
    std::vector<Card*> getInPlay() const {return inPlay_;};
    std::vector<Champion*> getChampions() const {return champions_;};

    void setHand(std::vector<Card*> hand) {hand_ = hand;};
    void setDiscardPile(std::vector<Card*> discardPile) {discardPile_ = discardPile;};
    void setInPlay(std::vector<Card*> inPlay) {inPlay_ = inPlay;};
    void setChampions(std::vector<Champion*> champions) {champions_ = champions;};

    void draw(int n);
    void play(Card* card);
    void buy(Card* card, Market market);
    void attack(Player player, int amount);
    void cleanup();
};
