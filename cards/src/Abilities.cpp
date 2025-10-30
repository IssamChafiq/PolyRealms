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
        case AbilityName::OpponentDiscardsOne:     opponentDiscardsOne(player, opponent); break;
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

void Abilities::sacrificeCards(Player& player, int n) {
    std::vector<Card*> hand = player.getHand();
    std::vector<Card*> discard = player.getDiscardPile();
    for(int i=0; i<n; i+=1)
    {
        // Faut donner le choix parmis ces cartes pour savoir laquelle sacrifier
    }

}

void Abilities::acquireToTop(Player& player) {
    // Ici il y a des conditions sur le type carte à mettre so maybe i'll need to break it down
    // into more abilities for each type of card
    // For example : acquire chapion from discard to top, acquire action from discard to top ...
}

void Abilities::opponentDiscardsOne(Player& /*player*/, Player& opponent) {
    opponent.discard(1);
}
