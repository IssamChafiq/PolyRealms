#include "Deck.hpp"
#include <iostream>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

Deck::Deck(std::vector<Card*> cards): cards_(cards) {}

Deck::~Deck() = default;

std::vector<Card*> Deck::getDeck() const {
    return cards_;
}

void Deck::setDeck(std::vector<Card*> deck){
    std::copy(deck.begin(), deck.end(), cards_.begin());
}

void Deck::shuffle() {
    // Create a random number generator
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    
    // Shuffle the vector
    std::shuffle(cards_.begin(), cards_.end(), gen);
}

std::vector<Card*> Deck::draw(int n) {
    std::vector<Card*> draws = {};
    for (int i=0;i<n;i++){
        draws.push_back(cards_.front());
        cards_.erase(cards_.begin());
    }
    return draws;
}