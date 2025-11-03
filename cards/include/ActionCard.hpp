#pragma once
#include "Card.hpp"

class ActionCard : public Card {
public:
    ActionCard(std::string id,
               std::string name,
               int cost,
               Faction faction,
               bool expendable = false,
               bool sacrificeable = false)
        : Card(std::move(id),
               std::move(name),
               cost,
               faction,
               CardType::Action,
               expendable,
               sacrificeable) {}
};
