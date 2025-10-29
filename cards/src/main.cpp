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
    std::string csvFile = "BaseSet_1stKickstarter_Cards_clean.csv";

    std::cout << "=== DEBUG CARD CREATION START ===\n";

    // this will now spam diagnostics for every row
    auto cards = CardCreator::loadFromCsv(csvFile);

    std::cout << "\n=== FINAL CARD LIST (" << cards.size() << " physical cards) ===\n\n";

    for (const Card* c : cards) {
        c->printCardInfo();
    }

    std::cout << "=== DEBUG CARD CREATION END ===\n";
    return 0;
}
