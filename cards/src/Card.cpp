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

std::vector<Card::CardAbility>& Card::abilities() { return abilities_; }
const std::vector<Card::CardAbility>& Card::abilities() const { return abilities_; }

bool Card::hasAbilitiesWithTrigger(Trigger t){
    for (const auto& ab : abilities()){
        if(ab.trigger == t){
            return true;
        }
    }
    return false;
}

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
        case Trigger::SacrificeChoice: return "SacrificeChoice";
        case Trigger::ExpendChoice: return "ExpendChoice";
        case Trigger::OnPlayChoice: return "OnPlayChoice";
        case Trigger::Ally: return "Ally";
    }
    return "UnknownTrigger";
}

std::string Card::abilityNameToString(AbilityName a) {
    switch (a) {
        case AbilityName::GainGold:                      return "GainGold";
        case AbilityName::GainCombat:                    return "GainCombat";
        case AbilityName::GainAuthority:                 return "GainAuthority";
        case AbilityName::DrawCards:                     return "DrawCards";
        case AbilityName::StunTargetChampion:            return "StunTargetChampion";
        case AbilityName::PrepareFriendlyChampion:       return "PrepareFriendlyChampion";
        case AbilityName::SacrificeCards:                return "SacrificeCards";
        case AbilityName::OpponentDiscard:               return "OpponentDiscard";
        case AbilityName::AddCombatPerChamp:             return "AddCombatPerChamp";
        case AbilityName::AddCombatPerGuard:             return "AddCombatPerGuard";
        case AbilityName::AddHealthPerChamp:             return "AddHealthPerChamp";
        case AbilityName::PutNextAcquiredCardInHand:     return "PutNextAcquiredCardInHand";
        case AbilityName::PutNextAcquiredActionCardOnDeck:return "PutNextAcquiredActionCardOnDeck";
        case AbilityName::PutCardFromDiscardOnDeck:      return "PutCardFromDiscardOnDeck";
        case AbilityName::PutNextAcquiredCardOnDeck:     return "PutNextAcquiredCardOnDeck";
        case AbilityName::SacrificeForCombat:            return "SacrificeForCombat";
        case AbilityName::MayDrawAndDiscard:             return "MayDrawAndDiscard";
        case AbilityName::DrawAndDiscard:                return "DrawAndDiscard";
        case AbilityName::PutChampFromDiscardOnDeck:     return "PutChampFromDiscardOnDeck";
        case AbilityName::AddCombatPerAlly:              return "AddCombatPerAlly";
    
    }
    return "UnknownAbility";
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
            ab.printAbility();
        }
    } else {
        std::cout << "Abilities: (none parsed)\n";
    }

    std::cout << "----------------------------------------\n\n";
}

void Card::CardAbility::printAbility() const {
    std::cout << "---\n";
    std::cout << "  -Trigger: " << Card::triggerToString(trigger) << "\n";
    std::cout << "  -Ability: " << Card::abilityNameToString(ability);
    if (amount > 0) {
        std::cout << " (amount: " << amount << ")";
    }
    std::cout << "\n";
    if (requiresAlly) {
        std::cout << "  -Requires ally: " << Card::factionToString(requiredAllyFaction) << "\n";
    }
    std::cout << "  -Used: " << (used ? "Yes" : "No") << "\n";
    std::cout << "---\n";
}
