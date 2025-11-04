#include "Champion.hpp"
#include <iostream>
#include <algorithm>

bool Champion::takeDamage(int attackValue) {
    if (stunned_) {
        std::cout << name_ << " is already stunned.\n";
        return false;
    }

    if (attackValue < maxShield_) {
        std::cout << name_ << " blocks the attack ("
                << maxShield_ << " defense).\n";
        return false;
    } else {
        stunned_ = true;
        std::cout << name_ << " has been defeated and removed from play.\n";
        return true;
    }
}

void Champion::printCardInfo() const {
    std::cout << "\033[0m"; // reset colors
    std::cout << "\n+====================================================================+\n";
    std::cout << "|                         \033[1;35mCHAMPION DETAILS\033[0m                           |\n";
    std::cout << "+====================================================================+\n";

    std::cout << "|  Name:        \033[1;36m" << name_ << "\033[0m\n";
    std::cout << "|  ID:          " << id_ << "\n";
    std::cout << "|  Cost:        \033[1;93m" << cost_ << "\033[0m\n";
    std::cout << "|  Faction:     " << factionToString(faction_) << "\n";
    std::cout << "|  Type:        " << typeToString(type_) << "\n";
    std::cout << "+--------------------------------------------------------------------+\n";

    std::cout << "|  Shield:      \033[1;36m" << maxShield_ << "\033[0m\n";
    std::cout << "|  Guard:       " << (isGuard_ ? "\033[1;32mYes\033[0m" : "\033[1;31mNo\033[0m") << "\n";
    std::cout << "|  Stunned:     " << (stunned_ ? "\033[1;31mYes\033[0m" : "\033[1;32mNo\033[0m") << "\n";
    std::cout << "+--------------------------------------------------------------------+\n";

    std::cout << "|  Expendable:   " << (expendable_ ? "\033[1;33mYes\033[0m" : "No") << "\n";
    std::cout << "|  Sacrificeable:" << (sacrificeable_ ? "\033[1;33mYes\033[0m" : "No") << "\n";
    std::cout << "+--------------------------------------------------------------------+\n";

    if (!abilities().empty()) {
        std::cout << "|  \033[1mAbilities:\033[0m\n";
        for (const auto& ab : abilities()) {
            ab.printAbility();
        }
    } else {
        std::cout << "|  Abilities: (none parsed)\n";
    }

    std::cout << "+====================================================================+\n\n";
}

