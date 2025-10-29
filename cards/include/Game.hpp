#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Player.hpp"
#include "Market.hpp"

class Game {
    std::vector<Player*> playerList_;
    Market market_;
    static std::vector<Card*> sacrificePile_;
    bool godmode_ = false;
public:
    Game(Market market);
    ~Game();
    void initialize();
    void startFFA();
    static void lookAt(Player* player);
    static void sacrifice(Card* card);
};
