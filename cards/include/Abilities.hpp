#pragma once
#include "Enums.hpp"
#include "Player.hpp"
#include "Card.hpp"

class Abilities {
public:
    static void execute(Player& player, AbilityName name, Player& opponent, int amount = 0);
    static void gainGold(Player& player, int amount);
    static void gainCombat(Player& player, int amount);
    static void gainAuthority(Player& player, int amount);
    static void drawCards(Player& player, int n);
    static void stunTargetChampion(Player& opponent);
    static void prepareFriendlyChampion(Player& player);
    static void sacrificeCards(Player player, int num);
    static void acquireToTop(Player& player);
    static void opponentDiscard(Player& opponent);
};

// Santé ou atatack en fonction du nombre de champions
// Fix csv, remove kickstarter 1
// manage the acquire to top
// Manage the choice add them in enum
