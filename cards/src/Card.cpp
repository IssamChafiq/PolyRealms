#include "include/Card.hpp"
#include <iostream>

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
      sacrificeable_(sacrificeable) {}

Card::~Card() = default;

void Card::onPlay(CardController& ctrl) {
    for (auto& ab : abilities_) {
        if (ab.trigger == Trigger::OnPlay && (!ab.requiresAlly || hasAllySameFaction())) {
            Abilities::execute(ctrl, ab.ability, ownerId_, opponentId_, id_, ab.amount);
        }
    }
}

void Card::onExpend(CardController& ctrl) {
    if (!expendable_) { std::cout << name_ << " n'est pas expendable.\n"; return; }
    if (expended_) { std::cout << name_ << " est déjà expendue.\n"; return; }

    for (auto& ab : abilities_) {
        if (ab.trigger == Trigger::Expend && (!ab.requiresAlly || hasAllySameFaction())) {
            Abilities::execute(ctrl, ab.ability, ownerId_, opponentId_, id_, ab.amount);
        }
    }
    expended_ = true;
}

void Card::onSacrifice(CardController& ctrl) {
    if (!sacrificeable_) { std::cout << name_ << " ne peut pas être sacrifiée.\n"; return; }
    if (sacrificed_) { std::cout << name_ << " a déjà été sacrifiée.\n"; return; }

    for (auto& ab : abilities_) {
        if (ab.trigger == Trigger::Sacrifice) {
            Abilities::execute(ctrl, ab.ability, ownerId_, opponentId_, id_, ab.amount);
        }
    }
    removeSelfFromPlayerZones(); // faut créer la classe joeur ...
    sacrificed_ = true;
}

void Card::onNewTurn(CardController& ctrl) {
    if (expendable_ && expended_) expended_ = false;
    for (auto& ab : abilities_) {
        if (ab.trigger == Trigger::NewTurn) {
            Abilities::execute(ctrl, ab.ability, ownerId_, opponentId_, id_, ab.amount);
        }
    }
}

bool Card::hasAllySameFaction() const {
    // faut créer la classe joeur pour avoir la liste de cartes et vérifier l'existence de ally
    return false;
}

void Card::removeSelfFromPlayerZones() {
    // faut créer la classe joeur pour avoir la liste de cartes / deck et vérifier l'existence de ally/heroes/ enlever la carte(this is depending on the method)
}
