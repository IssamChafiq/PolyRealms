#include "Abilities.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Champion.hpp"
#include "Card.hpp"
#include <limits>

bool Abilities::execute(Player* player,
                        AbilityName name,
                        Player* opponent,
                        int amount) {
    switch (name) {
        case AbilityName::GainGold:                return gainGold(player, amount); break;
        case AbilityName::GainCombat:              return gainCombat(player, amount); break;
        case AbilityName::GainAuthority:           return gainAuthority(player, amount); break;
        case AbilityName::DrawCards:               return drawCards(player, amount); break;
        case AbilityName::StunTargetChampion:      return stunTargetChampion(opponent); break;
        case AbilityName::PrepareFriendlyChampion: return prepareFriendlyChampion(player); break;
        case AbilityName::SacrificeCards:          return sacrificeCards(player, amount); break;
        case AbilityName::OpponentDiscard:     return opponentDiscard(opponent); break;
        case AbilityName::AddCombatPerChamp:                return addCombatPerChamp(player,amount); break;
        case AbilityName::AddCombatPerGuard:                return addCombatPerGuard(player,amount)  ; break;
        case AbilityName::AddHealthPerChamp:                return addHealthPerChamp(player,amount) ; break;
        case AbilityName::PutNextAcquiredCardInHand:        return putNextAcquiredCardInHand(player); break;
        case AbilityName::PutNextAcquiredActionCardOnDeck:  return putNextAcquiredActionCardOnDeck(player) ; break;
        case AbilityName::PutCardFromDiscardOnDeck:         return putCardFromDiscardOnDeck(player) ; break;
        case AbilityName::PutNextAcquiredCardOnDeck:        return putNextAcquiredCardOnDeck(player) ; break;
        case AbilityName::SacrificeForCombat:                 return sacrificeForCombat(player,amount); break;
        case AbilityName::DrawAndDiscard:                   return drawAndDiscard(player,amount) ; break;
        case AbilityName::MayDrawAndDiscard:         return mayDrawAndDiscard(player, amount) ; break;
        case AbilityName::PutChampFromDiscardOnDeck:        return putChampFromDiscardOnDeck(player) ; break;
        case AbilityName::AddCombatPerAlly:        return addCombatPerAlly(player, amount) ; break;
    }
}

bool Abilities::gainGold(Player* player, int amount) {
    player->setGold(player->getGold() + amount);
    return true;
}

bool Abilities::gainCombat(Player* player, int amount) {
    return true;
    player->setCombat(player->getCombat() + amount);
}

bool Abilities::gainAuthority(Player* player, int amount) {
    return true;
    player->setAuthority(player->getAuthority() + amount);
}

bool Abilities::drawCards(Player* player, int n) {
    return player->draw(n);
}

bool Abilities::stunTargetChampion(Player* opponent) {
    return opponent->stunChampion();
}

bool Abilities::prepareFriendlyChampion(Player* player) {
    return player->prepareFriendlyChampion();
}

bool Abilities::sacrificeCards(Player* player, int n) {
    return player->cardEffectSacrifice(n);
}

bool Abilities::opponentDiscard(Player* opponent) {
    return opponent->discard(1);
}

bool Abilities::addCombatPerChamp(Player* player,int amount){
    int n = 0;
    for (int i=0; i<(int)player->getChampions().size();i++) {
        n++;
    }
    player->setCombat(player->getCombat() + amount * n);
    return true;
}

bool Abilities::addCombatPerGuard(Player* player,int amount){
    int n = 0;
    for (Champion* c : player->getChampions()){
        if (c->isGuard()) {
            n++;
        }
    }
    player->setCombat(player->getCombat() + amount * n);
    return true;
}

bool Abilities::addHealthPerChamp(Player* player,int amount){
    int n = 0;
    for (int i=0; i<(int)player->getChampions().size();i++) n++;
    player->setAuthority(player->getAuthority() + amount * n);
    return true;
}

bool Abilities::putNextAcquiredCardInHand(Player* player){
    player->setNextBuyInHand(true);
    return true;
}

bool Abilities::putNextAcquiredActionCardOnDeck(Player* player){
    player->setNextActionBuyOnDeck(true);
    return true;
}

bool Abilities::putCardFromDiscardOnDeck(Player* player){
    player->getCardFromDiscardToDeck();
    return true;
}

bool Abilities::putNextAcquiredCardOnDeck(Player* player){
    player->setNextBuyTopOfDeck(true);
    return true;
}

bool Abilities::sacrificeForCombat(Player* player,int amount){
    if(player->cardEffectSacrifice(1)){
        player->setCombat(player->getCombat() + amount);
        return true;
    } else {
        return false;
    }
}

bool Abilities::mayDrawAndDiscard(Player* player,int amount){
    std::cout << "Do you want to draw " << amount << " card(s) ? (1. Yes /2. No)\n";
    int drawChoice;
    while(!(std::cin >> drawChoice) || drawChoice < 1 || drawChoice > 2){
        std::cout << "Invalid input. Please enter a valid choice: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    if(drawChoice == 1){
        return player->draw(amount) && player->discard(amount);
    } else if (drawChoice == 2){
        std::cout << "No cards drawn.\n";
        return false;
    }
}

bool Abilities::drawAndDiscard(Player* player,int amount){
    return player->draw(amount) && player->discard(amount);
}

bool Abilities::putChampFromDiscardOnDeck(Player* player){
    player->getChampionFromDiscardToDeck();
}

bool Abilities::addCombatPerAlly(Player* player, int amount) {
    // Ajoute du combat pour chaque autre carte alliée de la même faction (Wild dans la plupart des cas)
    // Parcourt les cartes actuellement en jeu (actions + champions)
    int alliesCount = 0;

    // On compte toutes les cartes "Wild" en jeu (hors la carte déclencheuse)
    for (Card* c : player->getInPlay()) {
        if (c->faction() == Faction::Wild)
            alliesCount++;
    }
    for (Champion* champ : player->getChampions()) {
        if (champ->faction() == Faction::Wild)
            alliesCount++;
    }

    int totalCombat = amount * alliesCount + 2; // On rajoute 2 parce que la carte a un effet 2 dégats de base et que le parseur n'arrive pas à le détecter. (pour l'instant)
    if (totalCombat > 0) {
        player->setCombat(player->getCombat()+totalCombat);
        std::cout << player->getName() << " gagne " << totalCombat
                  << " combat (" << amount << " par carte Wild en jeu)." << std::endl;
    } else {
        std::cout << "Aucun allié Wild en jeu, aucun bonus de combat." << std::endl;
    }
    return true;
}
