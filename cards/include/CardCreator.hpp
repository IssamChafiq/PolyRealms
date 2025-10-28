#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Card.hpp"
#include "ActionCard.hpp"
#include "Champion.hpp"
#include "Enums.hpp"

struct CardRow {
    std::string set;        
    int quantity;           
    std::string name;       
    std::string text;       
    std::string type;       
    std::string faction;    
    int cost;               
    std::string defense;    
    std::string role;       
};

class CardCreator {
public:
    static std::vector<std::unique_ptr<Card>> loadFromCsv(const std::string& path);
    static int toIntSafe(const std::string& s, int fallback = 0);
private:
    static std::vector<CardRow> readCsv(const std::string& path);
    static std::vector<std::unique_ptr<Card>> buildCardsFromRow(const CardRow& row);

    // NEW: parse text into abilities and attach to card
    static void parseTextIntoAbilities(const std::string& rulesText,
                                       Faction cardFaction,
                                       Card& cardOut);

    // helpers
    static std::vector<std::string> parseCsvLine(const std::string& line);

    static std::string trim(const std::string& s);
    static std::string lower(const std::string& s);

    

    static std::string slugify(const std::string& name);

    static bool isChampionType(const std::string& typeCell);
    static bool isActionType(const std::string& typeCell);
    static bool isItemType(const std::string& typeCell);

    static Faction parseFaction(const std::string& factionCell);

    static void parseDefenseField(const std::string& defenseCell,
                                  int& shieldOut,
                                  bool& guardOut);
};
