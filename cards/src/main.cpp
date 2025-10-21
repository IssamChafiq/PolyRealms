#include <iostream>
#include "ActionCard.hpp"
#include "Champion.hpp"
#include "Card.hpp"
#include "Enums.hpp"

int main() {
    std::cout << "=== Hero Realms Card Test ===\n\n";

    // Champion (Guard)
    Champion guardian("guardian", "Guardian of the Keep", 5, Faction::Imperial, 6, true);

    // Action (Fire Gem: sacrificeable)
    ActionCard fireGem("firegem", "Fire Gem", 2, Faction::Neutral, false, true);

    // Another Champion (non-Guard) to test gradual damage and healing
    Champion thief("thief", "Silent Thief", 3, Faction::Guild, 4, false);

    // Print details (uses Card::printCardInfo)
    Card::printCardInfo(guardian);
    Card::printCardInfo(fireGem);
    Card::printCardInfo(thief);

    // Combat tests to show guard vs non-guard behavior
    std::cout << "\n=== Combat Simulation ===\n";
    std::cout << "[Attack 3] -> Guardian (guard, shield 6)\n";
    guardian.takeDamage(3);   // should block (guard logic)

    std::cout << "[Attack 6] -> Guardian (guard, shield 6)\n";
    guardian.takeDamage(6);   // should be defeated

    std::cout << "\n[Attack 2] -> Thief (non-guard, shield 4)\n";
    thief.takeDamage(2);    

    std::cout << "[Heal 3] -> Thief (should heal to 4, not exceed max)\n";
    thief.heal();            

    std::cout << "[Attack 3] -> Thief (non-guard, shield 4)\n";
    thief.takeDamage(3);      

    std::cout << "[Heal 2] -> Thief (should heal to 3)\n";
    thief.heal();

    std::cout << "[Attack 5] -> Thief (should be defeated)\n";
    thief.takeDamage(5);

    std::cout << "[Heal 2] -> Thief (should fail — stunned)\n";
    thief.heal();

    std::cout << "\n=== End ===\n";
    return 0;
}
