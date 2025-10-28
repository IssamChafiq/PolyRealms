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


int main() {
    std::cout << "=== Hero Realms Card Test ===\n\n";

    // Champion (Guard)
    Champion guardian("guardian", "Guardian of the Keep", 5, Faction::Imperial, 6, true);

    // Action (Fire Gem: sacrificeable)
    ActionCard fireGem("firegem", "Fire Gem", 2, Faction::Neutral, false, true);

    // Another Champion (non-Guard) to test gradual damage and healing
    Champion thief("thief", "Silent Thief", 3, Faction::Guild, 4, false);

    // Print details (uses Card::printCardInfo)
    guardian.printCardInfo();
    fireGem.printCardInfo();
    thief.printCardInfo();

    // Combat tests to show guard vs non-guard behavior
    std::cout << "=== Combat Simulation ===\n";
    std::cout << "[Attack 3] -> Guardian (guard, shield 6)\n";
    guardian.takeDamage(3);   // should block (guard logic)

    std::cout << "[Attack 6] -> Guardian (guard, shield 6)\n";
    guardian.takeDamage(6);   // should be defeated

    std::cout << "\n[Attack 2] -> Thief (non-guard, shield 4)\n";
    thief.takeDamage(2);    

    std::cout << "[Heal] -> Thief (should heal to full, not exceed max)\n";
    thief.heal();            

    std::cout << "[Attack 3] -> Thief (non-guard, shield 4)\n";
    thief.takeDamage(3);      

    std::cout << "[Heal] -> Thief (should heal to full)\n";
    thief.heal();

    std::cout << "[Attack 5] -> Thief (should be defeated)\n";
    thief.takeDamage(5);

    std::cout << "[Heal 2] -> Thief (should fail - stunned)\n";
    thief.heal();

    std::cout << "\n=== End ===\n";

    // Table layer (market/player/deck) tests :
    std::cout << "=== Table Simulation ===\n";
    std::vector<Card*> deckCards = {&guardian, &fireGem, &thief, &thief, &thief, &fireGem, &guardian, &guardian};
    std::vector<Card*> startingHand = {&fireGem, &thief, &guardian, &fireGem, &thief};
    //Deck marketDeck(deckCards);
    //Pour une raison qui m'échappe, je peux construire un market en passant une liste de cartes mais pas un deck
    //Je crois ça vient du fait que market crée automatiqument un deck avec deckCards ?
    Market market(deckCards);
    Player player("Alice", 50, Deck(startingHand));

    std::cout << "Market sell row contents:\n";
    for (const Card* card : market.getMarketRow()) {
        card->printCardInfo();
    }
    std::cout << "\nMarket deck contents:\n";
    for (const Card* card : market.getMarketDeck().getDeckContents()) {
        card->printCardInfo();
    }

    std::cout << "\nPlayer initial hand:\n";
    for (const Card* card : player.getHand()) {
        card->printCardInfo();
    }
    std::cout << "\nPlayer draws 3 cards from deck\n";
    player.draw(3);
    std::cout << "Player hand after drawing:\n";
    for (const Card* card : player.getHand()) {
        card->printCardInfo();
    }

    std::cout << "Player plays first card from hand\n";
    player.play(player.getHand().front());

    std::cout << "Player buys the first card in the market\n";
    player.buy(market.getMarketRow().front(), market);

    std::cout << "Player plays first card from hand\n";
    player.play(player.getHand().front());

    std::cout << "\nPlayer's in-play cards:\n";
    for (const Card* card : player.getInPlay()) {
        card->printCardInfo();
    }

    // Here, the thief should be present, but typeid comparison in player.play doesnt work for class recognition
    // We need to find a solution to properly handle the fact that a card can be of derived type Champion
    std::cout << "\nPlayer's champions:\n";
    for (const Champion* champ : player.getChampions()) {
        champ->printCardInfo();
    }

    std::cout << "\nPlayer performs cleanup phase\n";
    player.cleanup();

    std::cout << "\nPlayer's hand after cleanup:\n";
    for (const Card* card : player.getHand()) {
        card->printCardInfo();
    }

    std::cout << "\nPlayer's discard pile after cleanup:\n";
    for (const Card* card : player.getDiscardPile()) {
        card->printCardInfo();
    }

    std::cout << "\nPlayer's in-play cards after cleanup:\n";
    for (const Card* card : player.getInPlay()) {
        card->printCardInfo();
    }
    
    std::cout << "\nPlayer's champions after cleanup:\n";
    for (const Champion* champ : player.getChampions()) {
        champ->printCardInfo();
    }

    std::cout << "Loading cards from CSV ... \n";

    // Load et create the cards, j'ai mis auto car flemme de trouver le vrai type ici
    auto cards = CardCreator::loadFromCsv("BaseSet_1stKickstarter_Cards.csv");

    std::cout << "Created " << cards.size() << " card objects.\n\n";

    // Print every physical card created, with abilities (currently none)
    for (const auto& c : cards) {
        c->printCardInfo();
    }

    std::cout << "=== Done ===\n";
    return 0;
}
