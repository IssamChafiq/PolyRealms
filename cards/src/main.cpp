#include <iostream>
#include "ActionCard.hpp"
#include "Champion.hpp"
#include "Card.hpp"
#include "Enums.hpp"
#include "CardCreator.hpp"
#include "Card.hpp"


int main() {
    std::cout << "Loading cards from CSV ... \n";

    // Load et create the cards, j'ai mis auto car flemme de trouver le vrai type ici
    auto cards = CardCreator::loadFromCsv("BaseSet_1stKickstarter_Cards.csv");

    std::cout << "Created " << cards.size() << " card objects.\n\n";

    // Print every physical card created, with abilities (currently none)
    for (const auto& c : cards) {
        Card::printCardInfo(*c);
    }

    std::cout << "=== Done ===\n";
    return 0;
}
