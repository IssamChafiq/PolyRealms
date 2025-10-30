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

    bool isExpendable() const;
    bool isSacrificeable() const;
    bool isExpended() const;
    bool isSacrificed() const;

    // Pour preparer le champion
    void prepare(){expended_ = false;}

    void setOwnerId(std::string owner);
    void setOpponentId(std::string opp);

    // gameplay hooks
    virtual void onPlay(CardController& ctrl);
    virtual void onExpend(CardController& ctrl);
    virtual void onSacrifice(CardController& ctrl);
    virtual void onNewTurn(CardController& ctrl);

    // abilities access
    std::vector<CardAbility>& abilities();
    const std::vector<CardAbility>& abilities() const;

    virtual bool isChampion() const;

    // pretty-print helpers
    static std::string factionToString(Faction f);
    static std::string typeToString(CardType t);
    static std::string triggerToString(Trigger tr);
    static std::string abilityNameToString(AbilityName a);

    virtual void printCardInfo() const;

protected:
    bool hasAllySameFaction();
    void removeSelfFromPlayerZones();

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

    std::string ownerId_;
    std::string opponentId_;

    std::vector<CardAbility> abilities_;
};
