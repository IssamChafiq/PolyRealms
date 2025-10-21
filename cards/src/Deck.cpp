#include "include/Deck.hpp"
#include <iostream>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <random>
#include <chrono>

Deck::Deck(std::list<Card> cards): cards_(cards) {}

Deck::~Deck() = default;

std::list<Card> getDeck() const {
    return cards;
}

void setDeck(std::list<Card> deck){
    std::copy(deck.begin(), deck.end(), cards_.begin());
}

void Deck::shuffle() {
    std::vector<Card> vec(cards_.begin(), cards_.end());
    
    // Create a random number generator
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    
    // Shuffle the vector
    std::shuffle(vec.begin(), vec.end(), gen);
    
    // Copy the shuffled elements back to the list
    std::copy(vec.begin(), vec.end(), cards_.begin());
}

std::list<Card> Deck::draw(int n) {
    std::list<Card> draws = {};
    for (int i=0,i<n,i++){
        draws.push_back(cards_.front());
        cards_.pop_front();
    }
    return cards;
}