#pragma once
#include <string>
#include <vector>
#include "Enums.hpp"
#include "CardController.hpp"
#include "Abilities.hpp"
#include <utility>


class Card {
public:
    struct CardAbility {
        Trigger trigger;
        AbilityName ability;
        int amount;
        bool requiresAlly;
    };

    Card(std::string id,
         std::string name,
         int cost,
         Faction faction,
         CardType type,
         bool expendable = false,
         bool sacrificeable = false);

    virtual ~Card();

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    int cost() const { return cost_; }
    Faction faction() const { return faction_; }
    CardType type() const { return type_; }


    bool isExpendable() const { return expendable_; }
    bool isSacrificeable() const { return sacrificeable_; }
    bool isExpended() const { return expended_; }
    bool isSacrificed() const { return sacrificed_; }

    void setOwnerId(std::string owner) { ownerId_ = std::move(owner); }
    void setOpponentId(std::string opp) { opponentId_ = std::move(opp); }

    virtual void onPlay(CardController& ctrl);
    virtual void onExpend(CardController& ctrl);
    virtual void onSacrifice(CardController& ctrl);
    virtual void onNewTurn(CardController& ctrl);

    std::vector<CardAbility>& abilities() { return abilities_; } // Pour création de cartes
    const std::vector<CardAbility>& abilities() const { return abilities_; } // Pour lire les abilities

    virtual bool isChampion() const { return false; }

    // Affichage cartes 
    static std::string factionToString(Faction f);
    static std::string typeToString(CardType t) ;
    static void printCardInfo(const Card& card);



protected:
    bool hasAllySameFaction() ; // faut créer la classe joeur pour avoir la liste de cartes et vérifier l'existence de ally
    void removeSelfFromPlayerZones(); // faut créer la classe joeur pour avoir la liste de cartes / deck et vérifier l'existence de ally/heroes/ enlever la carte

protected:
    std::string id_;
    std::string name_;
    int cost_;
    Faction faction_;
    CardType type_;
    bool expendable_;
    bool expended_{false};
    bool sacrificeable_;
    bool sacrificed_{false};
    std::string ownerId_;
    std::string opponentId_;
    std::vector<CardAbility> abilities_;
};
