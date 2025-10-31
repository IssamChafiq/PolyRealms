#include "Abilities.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Champion.hpp"
#include "Card.hpp"
#include <limits>

void Abilities::execute(Player* player,
                        AbilityName name,
                        Player* opponent,
                        int amount) {
    switch (name) {
        case AbilityName::GainGold:                gainGold(player, amount); break;
        case AbilityName::GainCombat:              gainCombat(player, amount); break;
        case AbilityName::GainAuthority:           gainAuthority(player, amount); break;
        case AbilityName::DrawCards:               drawCards(player, amount); break;
        case AbilityName::StunTargetChampion:      stunTargetChampion(opponent); break;
        case AbilityName::PrepareFriendlyChampion: prepareFriendlyChampion(player); break;
        case AbilityName::SacrificeCards:          sacrificeCards(player, amount); break;
        case AbilityName::OpponentDiscard:     opponentDiscard(opponent); break;
        case AbilityName::AddCombatPerChamp:                addCombatPerChamp(player,amount); break;
        case AbilityName::AddCombatPerGuard:                addCombatPerGuard(player,amount)  ; break;
        case AbilityName::AddHealthPerChamp:                addHealthPerChamp(player,amount) ; break;
        case AbilityName::PutNextAcquiredCardInHand:        putNextAcquiredCardInHand(player); break;
        case AbilityName::PutNextAcquiredActionCardOnDeck:  putNextAcquiredActionCardOnDeck(player) ; break;
        case AbilityName::PutCardFromDiscardOnDeck:         putCardFromDiscardOnDeck(player) ; break;
        case AbilityName::PutNextAcquiredCardOnDeck:        putNextAcquiredCardOnDeck(player) ; break;
        case AbilityName::SacrificeForCombat:                 sacrificeForCombat(player,amount); break;
        case AbilityName::DrawAndDiscard:                   drawAndDiscard(player,amount) ; break;
        case AbilityName::MayDrawAndDiscard:        mayDrawAndDiscard(player, amount) ; break;
        case AbilityName::PutChampFromDiscardOnDeck:        putChampFromDiscardOnDeck(player) ; break;
        case AbilityName::AddCombatPerAlly:        addCombatPerAlly(player, amount) ; break;
    }
}

void Abilities::gainGold(Player* player, int amount) {
    player->setGold(player->getGold() + amount);
}

void Abilities::gainCombat(Player* player, int amount) {
    player->setCombat(player->getCombat() + amount);
}

void Abilities::gainAuthority(Player* player, int amount) {
    player->setAuthority(player->getAuthority() + amount);
}

void Abilities::drawCards(Player* player, int n) {
    player->draw(n);
}

void Abilities::stunTargetChampion(Player* opponent) {
    opponent->stunChampion();
}

void Abilities::prepareFriendlyChampion(Player* player) {
    player->prepareFriendlyChampion();
}

void Abilities::sacrificeCards(Player* player, int n) {
    player->cardEffectSacrifice(n);
}

void Abilities::opponentDiscard(Player* opponent) {
    opponent->discard(1);
}

void Abilities::addCombatPerChamp(Player* player,int amount){
    int n = 0;
    for (int i=0; i<(int)player->getChampions().size();i++) {
        n++;
    }
    player->setCombat(player->getCombat() + amount * n);
}

void Abilities::addCombatPerGuard(Player* player,int amount){
    int n = 0;
    for (Champion* c : player->getChampions()){
        if (c->isGuard()) {
            n++;
        }
    }
    player->setCombat(player->getCombat() + amount * n);
}

void Abilities::addHealthPerChamp(Player* player,int amount){
    int n = 0;
    for (int i=0; i<(int)player->getChampions().size();i++) n++;
    player->setAuthority(player->getAuthority() + amount * n);
}

void Abilities::putNextAcquiredCardInHand(Player* player){
    player->setNextBuyInHand(true);
}

void Abilities::putNextAcquiredActionCardOnDeck(Player* player){
    player->setNextActionBuyOnDeck(true); 
}

void Abilities::putCardFromDiscardOnDeck(Player* player){
    player->getCardFromDiscardToDeck();
}

void Abilities::putNextAcquiredCardOnDeck(Player* player){
    player->setNextBuyTopOfDeck(true);
}

void Abilities::sacrificeForCombat(Player* player,int amount){
    if(player->cardEffectSacrifice(1)){
        player->setCombat(player->getCombat() + amount);
    }
}

void Abilities::mayDrawAndDiscard(Player* player,int amount){
    std::cout << "Do you want to draw " << amount << " card(s) ? (1. Yes /2. No)\n";
    int drawChoice;
    while(!(std::cin >> drawChoice) || drawChoice < 1 || drawChoice > 2){
        std::cout << "Invalid input. Please enter a valid choice: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    if(drawChoice == 1){
        player->draw(amount);
        player->discard(amount);
    } else if (drawChoice == 2){
        std::cout << "No cards drawn.\n";
    }
}

void Abilities::drawAndDiscard(Player* player,int amount){
    player->draw(amount);
    player->discard(amount);
}

void Abilities::putChampFromDiscardOnDeck(Player* player){
    player->getChampionFromDiscardToDeck();
}

void Abilities::addCombatPerAlly(Player* player, int amount) {
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
}
