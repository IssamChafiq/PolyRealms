#include "include/Market.hpp"
#include <iostream>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <random>
#include <chrono>

Market::Market(std::list<Card> cards): cards_(cards) {}

Market::~Market() = default;

void Market::sell(Card card){
    for (std::list<int>::iterator it = tradeRow_.begin(); it != tradeRow_.end();)
    {
        if (card.id() = *it.id())
            it = tradeRow_.erase(it);
            break;
        else
            ++it;
    }
    tradeRow_.append_range(tradeDeck_.draw(1));
}
