#include "Abilities.hpp"
#include <iostream>

void Abilities::execute(CardController& ctrl,
                        AbilityName name,
                        const std::string& selfId,
                        const std::string& opponentId,
                        const std::string& sourceCardId,
                        int amount) {
    switch (name) {
        case AbilityName::GainGold:               gainGold(ctrl, selfId, amount); break;
        case AbilityName::GainCombat:             gainCombat(ctrl, selfId, amount); break;
        case AbilityName::GainAuthority:          gainAuthority(ctrl, selfId, amount); break;
        case AbilityName::DrawCards:              drawCards(ctrl, selfId, amount); break;
        case AbilityName::StunTargetChampion:     stunTargetChampion(ctrl, selfId, opponentId); break;
        case AbilityName::PrepareFriendlyChampion:prepareFriendlyChampion(ctrl, selfId); break;
        case AbilityName::SacrificeSelf:          sacrificeSelf(ctrl, selfId, sourceCardId); break;
        case AbilityName::AcquireToTop:           acquireToTop(ctrl, selfId, true); break;
        case AbilityName::OpponentDiscardsOne:    opponentDiscardsOne(ctrl, selfId, opponentId); break;
    }
}

void Abilities::opponentDiscardsOne(CardController& ,
                                    const std::string& /*selfId*/,
                                    const std::string& /*opponentId*/) {
    // faut créer la classe joeur pour avoir la liste de cartes / deck et  enlever la carte
}


void Abilities::gainGold(CardController& ctrl, const std::string& selfId, int amount) {
    ctrl.addGold(selfId, amount);
}

void Abilities::gainCombat(CardController& ctrl, const std::string& selfId, int amount) {
    ctrl.addCombat(selfId, amount);
}

void Abilities::gainAuthority(CardController& ctrl, const std::string& selfId, int amount) {
    ctrl.addAuthority(selfId, amount);
}

void Abilities::drawCards(CardController& ctrl, const std::string& selfId, int n) {
    ctrl.drawCards(selfId, n);
}

void Abilities::stunTargetChampion(CardController& ctrl,
                                   const std::string& selfId,
                                   const std::string& opponentId) {
    // faut créer la classe joeur pour avoir la liste de cartes / deck et vérifier l'existence de ally/heroes/ enlever la carte
    std::string target = ctrl.chooseEnemyChampion(selfId, opponentId, true);
    if (!target.empty()) ctrl.stunChampion(selfId, opponentId, target);
}

void Abilities::prepareFriendlyChampion(CardController& ctrl, const std::string& selfId) {
    (void)ctrl;
    (void)selfId;
    // faut créer la classe joeur pour avoir la liste de cartes / deck et vérifier l'existence de ally/heroes/ enlever la carte
}

void Abilities::sacrificeSelf(CardController& ctrl,
                              const std::string& selfId,
                              const std::string& cardId) {
    ctrl.sacrificeCard(selfId, cardId);
    // faut créer la classe joeur pour avoir la liste de cartes / deck et vérifier l'existence de ally/heroes/ enlever la carte
}

void Abilities::acquireToTop(CardController& ctrl,
                             const std::string& selfId,
                             bool enable) {
    ctrl.setAcquireToTopThisTurn(selfId, enable);
}

