#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Player.hpp"
#include "Market.hpp"

class Game {
    static std::vector<Player*> playerList_;
    Market market_;
    // J'ai mis les fireGems ici parce que sinon j'avais des problèmes pour le sacrifice car sacrifice est statique et ne pouvait pas accéder au marché (où les gemmes étaient de base).
    std::vector<Card*> startingDeck_;
    static int fireGemStack_;
    Card* fireGem_;
    static std::vector<Card*> sacrificePile_;
    bool godmode_ = false;
public:
    Game(Market market, std::vector<Card*> startingDeck, Card* fireGem);
    ~Game();
    void initialize();
    void startFFA();
    static void lookAt(Player* player);
    static void sacrifice(Card* card);
    static void smartAbilityExecute(Player* player, Card::CardAbility& ab);
};
