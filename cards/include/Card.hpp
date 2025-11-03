#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Enums.hpp"

// forward declarations
class CardController;
class Champion;

class Card {
public:
    struct CardAbility {
        Trigger trigger;            
        AbilityName ability;        
        int amount;                 
        bool requiresAlly;          
        Faction requiredAllyFaction;
        bool used;
        void printAbility() const;
    };

    Card(std::string id,
         std::string name,
         int cost,
         Faction faction,
         CardType type,
         bool expendable,
         bool sacrificeable);

    virtual ~Card();

    // getters
    const std::string& id() const;
    const std::string& name() const;
    int cost() const;
    Faction faction() const;
    CardType type() const;

    // abilities access
    std::vector<CardAbility>& abilities();
    const std::vector<CardAbility>& abilities() const;
    bool hasAbilitiesWithTrigger(Trigger t);

    virtual bool isChampion() const;

    // pretty-print helpers
    static std::string factionToString(Faction f);
    static std::string typeToString(CardType t);
    static std::string triggerToString(Trigger tr);
    static std::string abilityNameToString(AbilityName a);

    virtual void printCardInfo() const;

protected:
    std::string id_;
    std::string name_;
    int cost_;
    Faction faction_;
    CardType type_;

    bool expendable_;
    bool expended_;
    bool sacrificeable_;
    bool sacrificed_;

    std::vector<CardAbility> abilities_;
};
