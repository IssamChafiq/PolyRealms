#pragma once
#include <string>

struct CardController {
    virtual ~CardController() = default;

    // Core ressources operations
    virtual void addGold(const std::string& whoId, int amount) = 0;
    virtual void addCombat(const std::string& whoId, int amount) = 0;
    virtual void addAuthority(const std::string& whoId, int amount) = 0;
    virtual void drawCards(const std::string& whoId, int n) = 0;

    // Champion interactions
    virtual void prepareChampion(const std::string& whoId, const std::string& championId) = 0;
    virtual void stunChampion(const std::string& attackerId,
                              const std::string& defenderId,
                              const std::string& championId) = 0;

    // Sacrifice/removal
    virtual void sacrificeCard(const std::string& whoId, const std::string& cardId) = 0;

    // Acquire
    virtual void setAcquireToTopThisTurn(const std::string& whoId, bool enabled) = 0;
    virtual std::string chooseEnemyChampion(const std::string& chooserId,
                                            const std::string& enemyId,
                                            bool guardsFirst) = 0;
};
