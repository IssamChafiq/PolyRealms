#pragma once
#include "Enums.hpp"
#include "Player.hpp"
#include "Card.hpp"

class Abilities {
public:
    static bool execute(Player* player, AbilityName name, Player* opponent, int amount = 0);
    static bool gainGold(Player* player, int amount);
    static bool gainCombat(Player* player, int amount);
    static bool gainAuthority(Player* player, int amount);
    static bool drawCards(Player* player, int n);
    static bool stunTargetChampion(Player* opponent);
    static bool prepareFriendlyChampion(Player* player);
    static bool sacrificeCards(Player* player, int num);
    static bool opponentDiscard(Player* opponent);
    static bool addCombatPerChamp(Player* player,int amount);
    static bool addCombatPerOtherGuard(Player* player,int amount);
    static bool addCombatPerOtherChamp(Player* player,int amount);
    static bool addHealthPerChamp(Player* player,int amount);
    static bool putNextAcquiredCardInHand(Player* player);
    static bool putNextAcquiredActionCardOnDeck(Player* player);
    static bool putCardFromDiscardOnDeck(Player* player);
    static bool putNextAcquiredCardOnDeck(Player* player);
    static bool sacrificeForCombat(Player* player,int amount);
    static bool mayDrawAndDiscard(Player* player,int amount);
    static bool drawAndDiscard(Player* player,int amount);
    static bool putChampFromDiscardOnDeck(Player* player);
    static bool addCombatPerAlly(Player* player, int amount);
};




// Santé ou atatack en fonction du nombre de champions
// manage the acquire to top
// Manage the choice add them in enum

//TO add to abilities : 
//{2 combat} for each champion you have in play.X
//Gain {2 health} for each champion you have in play.X
//{1 combat} for each other guard you have in play.X
//{1 combat} for each other champion you have in play.X
//Put the next card you acquire this turn into your hand.X
//Put the next action you acquire this turn on top of your deck.X
//Gain {1 health} for each champion you have in play.X
//{1 health} for each champion you have in play.X
//You may put a card from your discard pile on top of your deck.X
//Put the next card you acquire this turn on top of your deck.X
//You may sacrifice a card in your hand or discard pile.\nIf you do, gain an additional {2 combat}.X
//You may draw up to two cards, then discard that many cards.
//You may draw a card. If you do, discard a card.
//Take a champion from your discard pile and put it on top of your deck.
//Draw a card, then discard a card

//TO add to card creator : 
//{2 combat} for each champion you have in play.
//Gain {2 health} for each champion you have in play.
//{1 combat} for each other guard you have in play.
//{1 combat} for each other champion you have in play.
//Put the next card you acquire this turn into your hand.
//Put the next action you acquire this turn on top of your deck.
//Gain {1 health} for each champion you have in play.
//{1 health} for each champion you have in play.
//You may put a card from your discard pile on top of your deck.
//Put the next card you acquire this turn on top of your deck.
//You may sacrifice a card in your hand or discard pile.\nIf you do, gain an additional {2 combat}.
//You may sacrifice a card in your hand or discard pile.
//You may sacrifice up to two cards in your hand and/or discard pile.
//You may draw up to two cards, then discard that many cards.
//You may draw a card. If you do, discard a card.
//You may draw a card. If you do, discard a card.
//Take a champion from your discard pile and put it on top of your deck.