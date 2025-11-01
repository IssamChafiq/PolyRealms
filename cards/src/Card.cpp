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
        case AbilityName::GainGold:                      return "Gain x gold.";
        case AbilityName::GainCombat:                    return "Gain x combat.";
        case AbilityName::GainAuthority:                 return "Gain x health.";
        case AbilityName::DrawCards:                     return "Draw x card(s).";
        case AbilityName::StunTargetChampion:            return "Stun target champion.";
        case AbilityName::PrepareFriendlyChampion:       return "Prepare a champion.";
        case AbilityName::SacrificeCards:                return "You may sacrifice x card(s) in your hand or discard pile.";
        case AbilityName::OpponentDiscard:               return "Target opponent discards a card.";
        case AbilityName::AddCombatPerChamp:             return "Gain x combat for each champion you have in play.";
        case AbilityName::AddCombatPerOtherGuard:        return "Gain x combat for each other guard you have in play.";
        case AbilityName::AddCombatPerOtherChamp:        return "Gain x combat for each other champion you have in play.";
        case AbilityName::AddHealthPerChamp:             return "Gain x health for each champion you have in play.";
        case AbilityName::PutNextAcquiredCardInHand:     return "Put the next card you acquire this turn into your hand.";
        case AbilityName::PutNextAcquiredActionCardOnDeck:return "Put the next action you acquire this turn on top of your deck.";
        case AbilityName::PutCardFromDiscardOnDeck:      return "You may put a card from your discard pile on top of your deck";
        case AbilityName::PutNextAcquiredCardOnDeck:     return "Put the next card you acquire this turn on top of your deck.";
        case AbilityName::SacrificeForCombat:            return "You may sacrifice a card in your hand or discard pile. If you do, gain an additional x combat.";
        case AbilityName::MayDrawAndDiscard:             return "You may draw up to x card(s). If you do, discard x card(s).";
        case AbilityName::DrawAndDiscard:                return "Draw a card, then discard a card.";
        case AbilityName::PutChampFromDiscardOnDeck:     return "Take a champion from your discard pile and put it on top of your deck.";
        case AbilityName::AddCombatPerAlly:              return "Gain 2 combat + x combat for each other Wild card you have in play.";
    
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
