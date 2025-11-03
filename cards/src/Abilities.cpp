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
        case AbilityName::AddCombatPerOtherGuard:                return addCombatPerOtherGuard(player,amount)  ; break;
        case AbilityName::AddCombatPerOtherChamp:                return addCombatPerOtherChamp(player,amount); break;
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
        default: return true;
    }
}

bool Abilities::gainGold(Player* player, int amount) {
    player->setGold(player->getGold() + amount);
    std::cout << player->getName() << " gains " << amount << " gold.\n";
    return true;
}

bool Abilities::gainCombat(Player* player, int amount) {
    player->setCombat(player->getCombat() + amount);
    std::cout << player->getName() << " gains " << amount << " combat.\n";
    return true;
}

bool Abilities::gainAuthority(Player* player, int amount) {
    player->setAuthority(player->getAuthority() + amount);
    std::cout << player->getName() << " gains " << amount << " health.\n";
    return true;
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
    std::cout << player->getName() << " gains " << amount * n << " combat (" << amount << " per champion in play).\n";
    return true;
}

bool Abilities::addCombatPerOtherGuard(Player* player,int amount){
    int n = -1; // On gagne du combat pour chaque AUTRE garde, donc on commence à -1 pour que la carte puisse se compter elle-même.
    for (Champion* c : player->getChampions()){
        if (c->isGuard()) {
            n++;
        }
    }
    player->setCombat(player->getCombat() + amount * n);
    std::cout << player->getName() << " gains " << amount * n << " combat (" << amount << " per other guard in play).\n";
    return true;
}

bool Abilities::addCombatPerOtherChamp(Player* player,int amount){
    int n = -1; // On gagne du combat pour chaque AUTRE champion, donc on commence à -1 pour que la carte puisse se compter elle-même.
    for (int i=0; i<(int)player->getChampions().size();i++) {
        n++;
    }
    player->setCombat(player->getCombat() + amount * n);
    std::cout << player->getName() << " gains " << amount * n << " combat (" << amount << " per other champion in play).\n";
    return true;
}

bool Abilities::addHealthPerChamp(Player* player,int amount){
    int n = 0;
    for (int i=0; i<(int)player->getChampions().size();i++) n++;
    player->setAuthority(player->getAuthority() + amount * n);
    std::cout << player->getName() << " gains " << amount * n << " health (" << amount << " per champion in play).\n";
    return true;
}

bool Abilities::putNextAcquiredCardInHand(Player* player){
    player->setNextBuyInHand(true);
    std::cout << "The next acquired card will be put in hand.\n";
    return true;
}

bool Abilities::putNextAcquiredActionCardOnDeck(Player* player){
    player->setNextActionBuyOnDeck(true);
    std::cout << "The next acquired action card will be put on top of the deck.\n";
    return true;
}

bool Abilities::putCardFromDiscardOnDeck(Player* player){
    player->getCardFromDiscardToDeck();
    return true;
}

bool Abilities::putNextAcquiredCardOnDeck(Player* player){
    player->setNextBuyTopOfDeck(true);
    std::cout << "The next acquired card will be put on top of the deck.\n";
    return true;
}

bool Abilities::sacrificeForCombat(Player* player,int amount){
    if(player->cardEffectSacrifice(1)){
        player->setCombat(player->getCombat() + amount);
        std::cout << player->getName() << " gains " << amount << " combat by sacrificing a card.\n";
        return true;
    } else {
        return false;
    }
}

// On donne au joueur la possibilité de piocher des cartes et de défausser ensuite
bool Abilities::mayDrawAndDiscard(Player* player,int amount){
    std::cout << "You may draw " << amount << " card(s)\n";
    for(int i=0; i<amount; i++){
        std::cout << "Do you want to draw a card ? You will have to discard one if you do. (1. Yes /2. No)\n";
        int drawChoice;
        while(!(std::cin >> drawChoice) || drawChoice < 1 || drawChoice > 2){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if(drawChoice == 1){
            if(!player->draw(1) || !player->discard(1)){
                return false;
            }
        } else if (drawChoice == 2){
            std::cout << "No card drawn.\n";
            return true;
        }
    }
    return true;
}

// On force le joueur à piocher des cartes et à défausser ensuite
bool Abilities::drawAndDiscard(Player* player,int amount){
    return player->draw(amount) && player->discard(amount);
}

bool Abilities::putChampFromDiscardOnDeck(Player* player){
    return player->getChampionFromDiscardToDeck();
}

bool Abilities::addCombatPerAlly(Player* player, int amount) {
    // Ajoute du combat pour chaque autre carte alliée de la même faction (Wild dans la plupart des cas)
    // Parcourt les cartes actuellement en jeu (actions + champions)
    int alliesCount = -1; // La carte se compte elle-même et je sais pas vraiment comment l'en empêcher donc je commence à -1 pour que ça fasse 0 en la comptant.

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
