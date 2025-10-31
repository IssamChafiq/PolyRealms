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
        shield_ = 0;
        stunned_ = true;
        std::cout << name_ << " has been defeated and removed from play.\n";
        return true;
    }
}


void Champion::heal() {
    if (stunned_) {
        std::cout << name_ << " is stunned and cannot be healed.\n";
        return;
    }

    int before = shield_;
    shield_ = maxShield_;

    std::cout << name_ << " heals " << (maxShield_ - before)
              << " shield (now " << shield_ << "/" << maxShield_ << ").\n";
}

void Champion::printCardInfo() const{
    std::cout << "----------------------------------------\n";
    std::cout << "Card Name: " << name_ << "\n";
    std::cout << "Card ID:   " << id_ << "\n";
    std::cout << "Cost:      " << cost_ << "\n";
    std::cout << "Faction:   " << factionToString(faction_) << "\n";
    std::cout << "Type:      " << typeToString(type_) << "\n";

    std::cout << "Shield:    " << shield_ << "\n";
    std::cout << "Guard:     " << (isGuard_ ? "Yes" : "No") << "\n";
    std::cout << "Stunned:   " << (stunned_ ? "Yes" : "No") << "\n";

    std::cout << "Expendable: " << (expendable_ ? "Yes" : "No") << "\n";
    std::cout << "Sacrificeable: " << (sacrificeable_ ? "Yes" : "No") << "\n";

    if (!abilities().empty()) {
        std::cout << "Abilities:\n";
        for (const auto& ab : abilities()) {
            ab.printAbility();
        }
    } else {
        std::cout << "Abilities: (none parsed)\n";
    }
    
    std::cout << "----------------------------------------\n\n";
}
