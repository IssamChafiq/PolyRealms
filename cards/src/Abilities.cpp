#include "Abilities.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Champion.hpp"
#include "Card.hpp"

void Abilities::execute(Player& player,
                        AbilityName name,
                        Player& opponent,
                        int amount) {
    switch (name) {
        case AbilityName::GainGold:                gainGold(player, amount); break;
        case AbilityName::GainCombat:              gainCombat(player, amount); break;
        case AbilityName::GainAuthority:           gainAuthority(player, amount); break;
        case AbilityName::DrawCards:               drawCards(player, amount ); break;
        case AbilityName::StunTargetChampion:      stunTargetChampion(opponent); break;
        case AbilityName::PrepareFriendlyChampion: prepareFriendlyChampion(player); break;
        case AbilityName::SacrificeCards:           sacrificeCards(player, amount); break;
        case AbilityName::AcquireToTop:            acquireToTop(player); break;
        case AbilityName::OpponentDiscard:     opponentDiscard(opponent); break;
        case AbilityName::AddCombatPerChamp:                addCombatPerChamp(player,amount); break;
        case AbilityName::AddCombatPerGuard:                addCombatPerGuard(player,amount)  ; break;
        case AbilityName::AddHealthPerChamp:                addHealthPerChamp(player,amount) ; break;
        case AbilityName::PutNextAcquiredCardInHand:        putNextAcquiredCardInHand(player); break;
        case AbilityName::PutNextAcquiredActionCardInHand:  putNextAcquiredActionCardInHand(player) ; break;
        case AbilityName::PutCardFromDiscardOnDeck:         putCardFromDiscardOnDeck(player) ; break;
        case AbilityName::PutNextAcquiredCardOnDeck:        putNextAcquiredCardOnDeck(player) ; break;
        case AbilityName::SacrificeForCombat:                 sacrificeForCombat(player,amount); break;
        case AbilityName::DrawAndDiscard:                   drawAndDiscard(player,amount) ; break;
        case AbilityName::PutChampFromDiscardOnDeck:        putChampFromDiscardOnDeck(player) ; break;
        case AbilityName::AddCombatPerAlly:        addCombatPerAlly(player, amount) ; break;
    }
}

void Abilities::gainGold(Player& player, int amount) {
    player.setGold(player.getGold() + amount);
}

void Abilities::gainCombat(Player& player, int amount) {
    player.setCombat(player.getCombat() + amount);
}

void Abilities::gainAuthority(Player& player, int amount) {
    player.setAuthority(player.getAuthority() + amount);
}

void Abilities::drawCards(Player& player, int n) {
    player.draw(n);
}

void Abilities::stunTargetChampion(Player& opponent) {
    auto champs = opponent.getChampions();
    if (!champs.empty()) {
        // Il faut permettre au joueur de choisir le champion ennemi à stun
        //championOfChoice->stun();
    }
}

void Abilities::prepareFriendlyChampion(Player& player) {
    auto champs = player.getChampions();
    if (!champs.empty() ) {
        // Same thing here faut donner le choix du champion à stun
        // championOfChoice->prepare();
    }
}

void Abilities::sacrificeCards(Player player, int n) {
    player.cardEffectSacrifice(n);
}

void Abilities::acquireToTop(Player& player) {
    // Ici il y a des conditions sur le type carte à mettre so maybe i'll need to break it down
    // into more abilities for each type of card
    // For example : acquire chapion from discard to top, acquire action from discard to top ...
}

void Abilities::opponentDiscard(Player& opponent) {
    opponent.discard(1);
}


void Abilities::addCombatPerChamp(Player& player,int amount){
    int n = 0;
    for (Champion* c : player.getChampions()) {
        n++;
    }
    player.setCombat(player.getCombat() + amount * n);
}

void Abilities::addCombatPerGuard(Player& player,int amount){
    int n = 0;
    for (Champion* c : player.getChampions()){
        if (c->isGuard()) {
            n++;
        }
    }
    player.setCombat(player.getCombat() + amount * n);
}

void Abilities::addHealthPerChamp(Player& player,int amount){
    int n = 0;
    for (Champion* c : player.getChampions()) n++;
    player.setAuthority(player.getAuthority() + amount * n);
}

void Abilities::putNextAcquiredCardInHand(Player& player){
    
    // marc needed here 
}

void Abilities::putNextAcquiredActionCardInHand(Player& player){
    // marc needed here 
}

void Abilities::putCardFromDiscardOnDeck(Player& player){
    // marc needed here 
}

void Abilities::putNextAcquiredCardOnDeck(Player& player){
    // marc needed here 
}

void Abilities::sacrificeForCombat(Player& player,int amount){
    player.cardEffectSacrifice(1);
    player.setCombat(player.getCombat() + amount);
}

void Abilities::drawAndDiscard(Player& player,int amount){
    player.draw(amount);
    player.discard(amount);
}

void Abilities::putChampFromDiscardOnDeck(Player& player){
    // marc needed here 
}

void Abilities::addCombatPerAlly(Player& player, int amount) {
    // Ajoute du combat pour chaque autre carte alliée de la même faction (Wild dans la plupart des cas)
    // Parcourt les cartes actuellement en jeu (actions + champions)
    int alliesCount = 0;

    // On compte toutes les cartes "Wild" en jeu (hors la carte déclencheuse)
    for (Card* c : player.getInPlay()) {
        if (c->faction() == Faction::Wild)
            alliesCount++;
    }
    for (Champion* champ : player.getChampions()) {
        if (champ->faction() == Faction::Wild)
            alliesCount++;
    }

    int totalCombat = amount * alliesCount;
    if (totalCombat > 0) {
        player.setCombat(player.getCombat()+totalCombat);
        std::cout << player.getName() << " gagne " << totalCombat
                  << " combat (" << amount << " par carte Wild en jeu)." << std::endl;
    } else {
        std::cout << "Aucun allié Wild en jeu, aucun bonus de combat." << std::endl;
    }
}
