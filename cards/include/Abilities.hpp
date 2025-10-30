#pragma once
#include "Enums.hpp"
#include "Player.hpp"
#include "Card.hpp"

class Abilities {
public:
    static void execute(Player& player, AbilityName name, Player& opponent, int amount = 0);
    static void gainGold(Player& player, int amount);
    static void gainCombat(Player& player, int amount);
    static void gainAuthority(Player& player, int amount);
    static void drawCards(Player& player, int n);
    static void stunTargetChampion(Player& opponent);
    static void prepareFriendlyChampion(Player& player);
    static void sacrificeCards(Player& player, int num);
    static void acquireToTop(Player& player);
    static void opponentDiscard(Player& opponent);
    static void addCombatPerChamp(Player& player,int amount);
    static void addCombatPerGuard(Player& player,int amount);
    static void addHealthPerChamp(Player& player,int amount);
    static void putNextAcquiredCardInHand(Player& player);
    static void putNextAcquiredActionCardInHand(Player& player);
    static void putCardFromDiscardOnDeck(Player& player);
    static void putNextAcquiredCardOnDeck(Player& player);
    static void sacrificeForCombat(Player& player,int amount);
    static void drawAndDiscard(Player& player,int amount);
    static void putChampFromDiscardOnDeck(Player& player);
    static void addCombatPerAlly(Player& player, int amount);

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