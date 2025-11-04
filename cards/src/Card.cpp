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
        case Faction::Imperial: 
            return "\033[1;33mImperial\033[0m"; 

        case Faction::Guild:    
            return "\033[1;34mGuild\033[0m";    

        case Faction::Necros:   
            return "\033[1;31mNecros\033[0m";   

        case Faction::Wild:     return "\033[0;32mWild\033[0m";    
        case Faction::Neutral:  return "Neutral";
        case Faction::Unknown:  return "Unknown";
    }
    return "Unknown";
}

std::string Card::typeToString(CardType t) {
    switch (t) {
        case CardType::Action:   return "\033[1;35mAction\033[0m";  
        case CardType::Champion:  return "\033[1;36mChampion\033[0m";  

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
        case AbilityName::GainGold:                      return "\033[1;32mGain x gold.\033[0m";
        case AbilityName::GainCombat:                    
            return "\033[1;32mGain x combat.\033[0m";

        case AbilityName::GainAuthority:                 
            return "\033[1;32mGain x health.\033[0m";

        case AbilityName::DrawCards:                     
            return "\033[1;32mDraw x card(s).\033[0m";

        case AbilityName::StunTargetChampion:            
            return "\033[1;32mStun target champion.\033[0m";

        case AbilityName::PrepareFriendlyChampion:       
            return "\033[1;32mPrepare a champion.\033[0m";

        case AbilityName::SacrificeCards:                
            return "\033[1;32mYou may sacrifice x card(s) in your hand or discard pile.\033[0m";

        case AbilityName::OpponentDiscard:               
            return "\033[1;32mTarget opponent discards a card.\033[0m";

        case AbilityName::AddCombatPerChamp:             
            return "\033[1;32mGain x combat for each champion you have in play.\033[0m";

        case AbilityName::AddCombatPerOtherGuard:        
            return "\033[1;32mGain x combat for each other guard you have in play.\033[0m";

        case AbilityName::AddCombatPerOtherChamp:        
            return "\033[1;32mGain x combat for each other champion you have in play.\033[0m";

        case AbilityName::AddHealthPerChamp:             
            return "\033[1;32mGain x health for each champion you have in play.\033[0m";

        case AbilityName::PutNextAcquiredCardInHand:     
            return "\033[1;32mPut the next card you acquire this turn into your hand.\033[0m";

        case AbilityName::PutNextAcquiredActionCardOnDeck:
            return "\033[1;32mPut the next action you acquire this turn on top of your deck.\033[0m";

        case AbilityName::PutCardFromDiscardOnDeck:      
            return "\033[1;32mYou may put a card from your discard pile on top of your deck.\033[0m";

        case AbilityName::PutNextAcquiredCardOnDeck:     
            return "\033[1;32mPut the next card you acquire this turn on top of your deck.\033[0m";

        case AbilityName::SacrificeForCombat:            
            return "\033[1;32mYou may sacrifice a card in your hand or discard pile. If you do, gain an additional x combat.\033[0m";

        case AbilityName::MayDrawAndDiscard:             
            return "\033[1;32mYou may draw up to x card(s). If you do, discard x card(s).\033[0m";

        case AbilityName::DrawAndDiscard:                
            return "\033[1;32mDraw a card, then discard a card.\033[0m";

        case AbilityName::PutChampFromDiscardOnDeck:     
            return "\033[1;32mTake a champion from your discard pile and put it on top of your deck.\033[0m";

        case AbilityName::AddCombatPerAlly:              
            return "\033[1;32mGain 2 combat + x combat for each other Wild card you have in play.\033[0m";

    
    }
    return "UnknownAbility";
}

// print with abilities
void Card::printCardInfo() const {
    std::cout << "\033[0m"; // reset colors
    std::cout << "\n+====================================================================+\n";
    std::cout << "|                         \033[1;35mCARD DETAILS\033[0m                              |\n";
    std::cout << "+====================================================================+\n";

    std::cout << "|  Name:     \033[1;36m" << name_ << "\033[0m\n";
    std::cout << "|  ID:       " << id_ << "\n";
    std::cout << "|  Cost:     \033[1;93m" << cost_ << "\033[0m\n";
    std::cout << "|  Faction:  " << factionToString(faction_) << "\n";
    std::cout << "|  Type:     " << typeToString(type_) << "\n";

    std::cout << "+--------------------------------------------------------------------+\n";

    if (!abilities().empty()) {
        std::cout << "|  \033[1mAbilities:\033[0m\n";
        for (const auto& ab : abilities()) {
            ab.printAbility();
        }
    } else {
        std::cout << "|  Abilities: (none parsed)\n";
    }

    std::cout << "+====================================================================+\n\n";
}


void Card::CardAbility::printAbility() const {
    std::cout << "|   ────────────────────────────────────────────────────────────────\n";
    std::cout << "|    • Trigger: \033[1;36m" << Card::triggerToString(trigger) << "\033[0m\n";
    std::cout << "|    • Effect:  \033[1;33m" << Card::abilityNameToString(ability) << "\033[0m";
    if (amount > 0) {
        std::cout << " (\033[1;32m+" << amount << "\033[0m)";
    }
    std::cout << "\n";
    if (requiresAlly) {
        std::cout << "|    • Requires Ally: " << Card::factionToString(requiredAllyFaction) << "\n";
    }
    std::cout << "|    • Used: " << (used ? "\033[1;31mYes\033[0m" : "\033[1;32mNo\033[0m") << "\n";
}

