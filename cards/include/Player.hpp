#pragma once
#include <string>
#include <list>
#include "Deck.hpp"
#include "Champion.hpp"
#include "Market.hpp"

class Player {
    std::string name_;
    int authority_;
    int gold_ = 0;
    int combat_ = 0;
    Deck deck_;
    std::vector<Card*> hand_ = {};
    std::vector<Card*> discardPile_ = {};
    std::vector<Card*> inPlay_ = {};
    std::vector<Champion*> champions_ = {};
    bool nextBuyTopOfDeck = false;
    bool nextBuyInHand = false;
    bool nextActionBuyOnDeck = false;
public:
    Player(std::string name,
        int authority,
        Deck deck);
    ~Player();

    std::string getName() const {return name_;};
    int getAuthority() const {return authority_;};
    int getGold() const {return gold_;};
    int getCombat() const {return combat_;};

    void setName(std::string name) {name_ = name;};
    void setAuthority(int authority) {authority_ = authority;};
    void setGold(int gold) {gold_ = gold;};
    void setCombat(int combat) {combat_ = combat;};
    void setNextBuyTopOfDeck(bool state) {nextBuyTopOfDeck = state;};
    void setNextBuyInHand(bool state) {nextBuyInHand = state;};
    void setNextActionBuyOnDeck(bool state) {nextActionBuyOnDeck = state;};

    std::vector<Card*> getHand() const {return hand_;};
    std::vector<Card*> getDiscardPile() const {return discardPile_;};
    std::vector<Card*> getInPlay() const {return inPlay_;};
    std::vector<Champion*> getChampions() const {return champions_;};
    std::vector<Champion*> getGuards() const;
    Deck getDeck() const {return deck_;};

    void setHand(std::vector<Card*> hand) {hand_ = hand;};
    void setDiscardPile(std::vector<Card*> discardPile) {discardPile_ = discardPile;};
    void setInPlay(std::vector<Card*> inPlay) {inPlay_ = inPlay;};
    void setChampions(std::vector<Champion*> champions) {champions_ = champions;};
    void removeChampion(Champion* champion);

    bool draw(int n);
    void play(Card* card);
    bool buy(Card* card, Market& market);
    bool godmodeBuy(Card* card, Market& market);

    bool discard(int amount);
    bool cardEffectSacrifice(int amount);
    bool prepareFriendlyChampion();
    bool getChampionFromDiscardToDeck();
    void getCardFromDiscardToDeck();
    bool isFactionInPlay(Faction faction);
    bool isFactionInPlayExclude(Faction faction, Card* card);
    bool isGuarded();
    bool stunChampion();
    void attack(Player* player, bool toRight);
    void useAbility(int cardChoice);

    void cleanup();
};
