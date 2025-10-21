#pragma once
#include <string>
#include "Enums.hpp"
#include "CardController.hpp"

class Abilities {
    public:
    static void execute(CardController& ctrl,
                        AbilityName name,
                        const std::string& selfId,
                        const std::string& opponentId,
                        const std::string& sourceCardId,
                        int amount = 0);

private:
    static void gainGold(CardController& ctrl, const std::string& selfId, int amount);
    static void gainCombat(CardController& ctrl, const std::string& selfId, int amount);
    static void gainAuthority(CardController& ctrl, const std::string& selfId, int amount);
    static void drawCards(CardController& ctrl, const std::string& selfId, int n);
    static void stunTargetChampion(CardController& ctrl, const std::string& selfId, const std::string& opponentId);
    static void prepareFriendlyChampion(CardController& ctrl, const std::string& selfId);
    static void sacrificeSelf(CardController& ctrl, const std::string& selfId, const std::string& cardId);
    static void acquireToTop(CardController& ctrl, const std::string& selfId, bool enable);
    static void opponentDiscardsOne(CardController& ctrl, const std::string& selfId, const std::string& opponentId);
};

