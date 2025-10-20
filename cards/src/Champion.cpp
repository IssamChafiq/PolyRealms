#include "include/Champion.hpp"

void Champion::applyDamage(int amount) {
    if (amount >= defense_) {
        const_cast<bool&>(stunned_) = true;
    }
}
