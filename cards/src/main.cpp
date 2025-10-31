#include <iostream>
#include <string>
#include <list>
#include <vector>
#include "ActionCard.hpp"
#include "Champion.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "Market.hpp"
#include "Enums.hpp"
#include "CardCreator.hpp"
#include "Game.hpp"


int main() {
    std::string csvFile = "BaseSet_Cards_clean.csv";

    std::cout << "=== DEBUG CARD CREATION START ===\n";

    // this will now spam diagnostics for every row
    std::vector<Card*> cards = CardCreator::loadFromCsv(csvFile);

    std::cout << "\n=== FINAL CARD LIST (" << cards.size() << " physical cards) ===\n\n";

    std::cout << "=== DEBUG CARD CREATION END ===\n";

    std::vector<Card*> starterDeck = {
        CardCreator::findCard("gold-1",cards),
        CardCreator::findCard("gold-2",cards),
        CardCreator::findCard("gold-3",cards),
        CardCreator::findCard("gold-4",cards),
        CardCreator::findCard("gold-5",cards),
        CardCreator::findCard("gold-6",cards),
        CardCreator::findCard("gold-7",cards),
        CardCreator::findCard("ruby-1",cards),
        CardCreator::findCard("dagger-1",cards),
        CardCreator::findCard("shortsword-1",cards)
    };

    std::vector<Card*> fireGems = {
        CardCreator::findCard("fire-gem-1",cards),
        CardCreator::findCard("fire-gem-2",cards),
        CardCreator::findCard("fire-gem-3",cards),
        CardCreator::findCard("fire-gem-4",cards),
        CardCreator::findCard("fire-gem-5",cards),
        CardCreator::findCard("fire-gem-6",cards),
        CardCreator::findCard("fire-gem-7",cards),
        CardCreator::findCard("fire-gem-8",cards),
        CardCreator::findCard("fire-gem-9",cards),
        CardCreator::findCard("fire-gem-10",cards),
        CardCreator::findCard("fire-gem-11",cards),
        CardCreator::findCard("fire-gem-12",cards),
        CardCreator::findCard("fire-gem-13",cards),
        CardCreator::findCard("fire-gem-14",cards),
        CardCreator::findCard("fire-gem-15",cards),
        CardCreator::findCard("fire-gem-16",cards),
    };

    // Je récupère toutes les cartes du deck du marché (elles sont toutes avant la première firegem) pour éviter de les chercher une par une et de faire des recherches à l'infini en boucle
    std::vector<Card*> marketCards = {};
    int i=0;
    Card* card = cards[0];
    while(card->id() != "fire-gem-1"){
        marketCards.push_back(card);
        i++;
        card = cards[i];
    }

    Market market = Market(Deck(marketCards));

    Game game = Game(market, starterDeck, fireGems);

    std::cout << "\n=== GAME START ===\n";

    game.initialize();

    return 0;
}
