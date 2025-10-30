#include "Card.hpp"
#include "Champion.hpp"
#include "Abilities.hpp"
#include <iostream>

// ctor / dtor
Card::Card(std::string id,
           std::string name,
           int cost,
           Faction faction,
           CardType type,
           bool expendable,
           bool sacrificeable)
    : id_(std::move(id)),
      name_(std::move(name)),
      cost_(cost),
      faction_(faction),
      type_(type),
      expendable_(expendable),
      expended_(false),
      sacrificeable_(sacrificeable),
      sacrificed_(false) {}

Card::~Card() = default;

// getters
const std::string& Card::id() const { return id_; }
const std::string& Card::name() const { return name_; }
int Card::cost() const { return cost_; }
Faction Card::faction() const { return faction_; }
CardType Card::type() const { return type_; }

bool Card::isExpendable() const { return expendable_; }
bool Card::isSacrificeable() const { return sacrificeable_; }
bool Card::isExpended() const { return expended_; }
bool Card::isSacrificed() const { return sacrificed_; }

void Card::setOwnerId(std::string owner) { ownerId_ = std::move(owner); }
void Card::setOpponentId(std::string opp) { opponentId_ = std::move(opp); }

// gameplay hooks (stubs for now)
void Card::onPlay(CardController& ctrl) {
    (void)ctrl;
}
void Card::onExpend(CardController& ctrl) {
    (void)ctrl;
}
void Card::onSacrifice(CardController& ctrl) {
    (void)ctrl;
}
void Card::onNewTurn(CardController& ctrl) {
    (void)ctrl;
}

std::vector<Card::CardAbility>& Card::abilities() { return abilities_; }
const std::vector<Card::CardAbility>& Card::abilities() const { return abilities_; }

bool Card::isChampion() const { return false; }

std::string Card::factionToString(Faction f) {
    switch (f) {
        case Faction::Imperial: return "Imperial";
        case Faction::Guild:    return "Guild";
        case Faction::Necros:   return "Necros";
        case Faction::Wild:     return "Wild";
        case Faction::Neutral:  return "Neutral";
        case Faction::Unknown:  return "Unknown";
    }
    return "Unknown";
}

std::string Card::typeToString(CardType t) {
    switch (t) {
        case CardType::Action:   return "Action";
        case CardType::Champion: return "Champion";
    }
    return "Unknown";
}

std::string Card::triggerToString(Trigger tr) {
    switch (tr) {
        case Trigger::OnPlay:    return "OnPlay";
        case Trigger::Expend:    return "Expend";
        case Trigger::Sacrifice: return "Sacrifice";
        case Trigger::NewTurn:   return "NewTurn";
    }
    return "UnknownTrigger";
}

std::string Card::abilityNameToString(AbilityName a) {
    switch (a) {
        case AbilityName::GainGold:                 return "GainGold";
        case AbilityName::GainCombat:               return "GainCombat";
        case AbilityName::GainAuthority:            return "GainAuthority";
        case AbilityName::DrawCards:                return "DrawCards";
        case AbilityName::StunTargetChampion:       return "StunTargetChampion";
        case AbilityName::PrepareFriendlyChampion:  return "PrepareFriendlyChampion";
        case AbilityName::SacrificeCards:            return "SacrificeCards";
        case AbilityName::AcquireToTop:             return "AcquireToTop";
        case AbilityName::OpponentDiscardsOne:      return "OpponentDiscardsOne";
    }
    return "UnknownAbility";
}

bool Card::hasAllySameFaction() {
    // faut créer la classe joueur pour avoir la liste de cartes et vérifier l'existence de ally
    return false;
}

void Card::removeSelfFromPlayerZones() {
    // faut créer la classe joueur pour avoir la liste de cartes / deck et enlever cette carte
}

// print with abilities
void Card::printCardInfo() const {
    std::cout << "----------------------------------------\n";
    std::cout << "Card Name: " << name_ << "\n";
    std::cout << "Card ID:   " << id_ << "\n";
    std::cout << "Cost:      " << cost_ << "\n";
    std::cout << "Faction:   " << factionToString(faction_) << "\n";
    std::cout << "Type:      " << typeToString(type_) << "\n";

    std::cout << "Expendable: " << (expendable_ ? "Yes" : "No") << "\n";
    std::cout << "Sacrificeable: " << (sacrificeable_ ? "Yes" : "No") << "\n";

    if (!abilities().empty()) {
        std::cout << "Abilities:\n";
        for (const auto& ab : abilities()) {
            std::cout << "  - Trigger=" << triggerToString(ab.trigger)
                      << ", Name=" << abilityNameToString(ab.ability)
                      << ", Amount=" << ab.amount
                      << ", RequiresAlly=" << (ab.requiresAlly ? "Yes" : "No");
            if (ab.requiresAlly) {
                std::cout << " (" << factionToString(ab.requiredAllyFaction) << " Ally)";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "Abilities: (none parsed)\n";
    }

    std::cout << "----------------------------------------\n\n";
}
