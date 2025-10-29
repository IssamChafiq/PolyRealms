#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Card.hpp"
#include "Champion.hpp"
#include "ActionCard.hpp"
#include "Enums.hpp"

struct CardRow {
    std::string set;
    int         quantity;
    std::string name;
    std::string text;
    std::string type;
    std::string faction;
    int         cost;
    std::string defense;
    std::string role;
};

class CardCreator {
public:
    // helpers
    static std::string trim(const std::string& s);
    static std::string lower(const std::string& s);
    static int         toIntSafe(const std::string& s, int fallback = 0);
    static std::string slugify(const std::string& name);

    static bool isChampionType(const std::string& typeCell);
    static bool isActionType(const std::string& typeCell);
    static bool isItemType(const std::string& typeCell);

    static Faction parseFaction(const std::string& factionCell);

    static void parseDefenseField(const std::string& defenseCell,
                                  int& shieldOut,
                                  bool& guardOut);

    // CSV
    static std::vector<std::string> parseCsvLine(const std::string& line);
    static std::vector<CardRow>     readCsv(const std::string& path);

    // abilities
    static void handleAbilities(const std::string& fullText,
                                Faction printedFaction,
                                Card& cardOut);

    // build cards
    static std::vector<std::unique_ptr<Card>> buildCardsFromRow(const CardRow& row);
    static std::vector<std::unique_ptr<Card>> loadFromCsv(const std::string& path);
};
