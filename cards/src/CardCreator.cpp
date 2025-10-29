#include "CardCreator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <regex>
#include <iostream>

// -------------------------------------------------
// small helpers
// -------------------------------------------------
std::string CardCreator::trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace((unsigned char)s[a])) a++;
    size_t b = s.size();
    while (b > a && std::isspace((unsigned char)s[b-1])) b--;
    return s.substr(a, b-a);
}

std::string CardCreator::lower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}

int CardCreator::toIntSafe(const std::string& s, int fallback) {
    try {
        if (s.empty()) return fallback;
        return std::stoi(s);
    } catch (...) {
        return fallback;
    }
}

std::string CardCreator::slugify(const std::string& name) {
    std::string out;
    out.reserve(name.size());
    for (char c : name) {
        if (std::isalnum((unsigned char)c)) {
            out.push_back(std::tolower((unsigned char)c));
        } else if (std::isspace((unsigned char)c) || c == '-' || c == '_') {
            out.push_back('-');
        }
        // else skip weird chars
    }
    // collapse duplicate '-'
    out.erase(std::unique(out.begin(), out.end(),
                          [](char a,char b){ return a=='-' && b=='-'; }),
              out.end());
    if (out.empty()) out = "card";
    return out;
}

// figure out type
bool CardCreator::isChampionType(const std::string& typeCell) {
    std::string t = lower(trim(typeCell));
    return t.rfind("champion", 0) == 0; // starts with "champion"
}
bool CardCreator::isActionType(const std::string& typeCell) {
    std::string t = lower(trim(typeCell));
    return t.rfind("action", 0) == 0;
}
bool CardCreator::isItemType(const std::string& typeCell) {
    std::string t = lower(trim(typeCell));
    return t.rfind("item", 0) == 0;
}

// faction parser
Faction CardCreator::parseFaction(const std::string& factionCell) {
    std::string f = lower(trim(factionCell));
    if (f.find("imperial") != std::string::npos) return Faction::Imperial;
    if (f.find("guild")    != std::string::npos) return Faction::Guild;
    if (f.find("necros")   != std::string::npos) return Faction::Necros;
    if (f.find("wild")     != std::string::npos) return Faction::Wild;
    if (f.find("favored")  != std::string::npos) return Faction::Unknown;
    if (f.find("blue")     != std::string::npos) return Faction::Unknown;
    if (f.empty() || f.find("neutral") != std::string::npos) return Faction::Neutral;
    return Faction::Unknown;
}

// parse "Defense" column like "6 Guard" / "5" / "".
void CardCreator::parseDefenseField(const std::string& defenseCell,
                                    int& shieldOut,
                                    bool& guardOut) {
    shieldOut = 0;
    guardOut  = false;

    std::string lc = lower(defenseCell);

    // get first int
    {
        int tmp = 0;
        bool seenDigit = false;
        for (char c : lc) {
            if (std::isdigit((unsigned char)c)) {
                tmp = tmp * 10 + (c - '0');
                seenDigit = true;
            } else if (seenDigit) {
                break;
            }
        }
        shieldOut = tmp;
    }

    if (lc.find("guard") != std::string::npos) {
        guardOut = true;
    }
}

// -------------------------------------------------
// CSV parsing helpers
// -------------------------------------------------

// low-level splitter: splits a *complete logical row* into cells
// handles commas, quotes, and "" escapes
std::vector<std::string> CardCreator::parseCsvLine(const std::string& line) {
    std::vector<std::string> cells;
    std::string cur;
    bool inq = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inq && i+1 < line.size() && line[i+1] == '"') {
                // escaped quote ("")
                cur.push_back('"');
                ++i;
            } else {
                inq = !inq;
            }
        } else if (c == ',' && !inq) {
            cells.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    cells.push_back(cur);
    return cells;
}

// -------------------------------------------------
// readCsv with multiline row support
// -------------------------------------------------
std::vector<CardRow> CardCreator::readCsv(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Cannot open CSV: " + path);
    }

    std::cout << "[readCsv] OPEN " << path << "\n";

    // read header (assume header is 1 physical line)
    std::string headerPhysical;
    if (!std::getline(in, headerPhysical)) {
        throw std::runtime_error("CSV is empty: " + path);
    }

    auto headers = parseCsvLine(headerPhysical);

    auto findColMulti = [&](std::initializer_list<std::string> alts) -> int {
        for (const auto& alt : alts) {
            std::string want = lower(trim(alt));
            for (size_t i = 0; i < headers.size(); ++i) {
                std::string got = lower(trim(headers[i]));
                if (got == want) {
                    return (int)i;
                }
            }
        }
        return -1;
    };

    int cSet      = findColMulti({"set","source set","pack","expansion"});
    int cQty      = findColMulti({"quantity","qty","#","count","qty"});
    int cName     = findColMulti({"name","card name"});
    int cText     = findColMulti({"text","card text","rules text"});
    int cType     = findColMulti({"type","card type"});
    int cFaction  = findColMulti({"faction","faction / color","color","faction/color"});
    int cCost     = findColMulti({"cost","price"});
    int cDefense  = findColMulti({"defense","defence","shield","guard/defense"});
    int cRole     = findColMulti({"role","deck","location","where"});

    if (cQty < 0 || cName < 0 || cText < 0 || cType < 0 ||
        cFaction < 0 || cCost < 0 || cDefense < 0 || cRole < 0) {
        throw std::runtime_error(
            "CSV headers missing. Need Qty,Name,Text,Type,Faction/Color,Cost,Defense,Role"
        );
    }

    std::vector<CardRow> rows;

    // We'll accumulate physical lines into rowBuffer until quotes balance.
    std::string rowBuffer;
    int physicalLineNum = 1; // header consumed, so next getline is line 2

    auto flushRowBufferIfComplete = [&](bool force=false) {
        if (rowBuffer.empty()) return false;

        // check quote balance
        bool q = false; // 'in quotes' state as we scan the buffer
        for (size_t i = 0; i < rowBuffer.size(); ++i) {
            char c = rowBuffer[i];
            if (c == '"') {
                if (q && i+1 < rowBuffer.size() && rowBuffer[i+1] == '"') {
                    // escaped double quote, skip next
                    ++i;
                } else {
                    q = !q;
                }
            }
        }

        // if balanced (q == false) OR we're forcing at EOF, then parse logical row
        if (!q || force) {
            auto cells = parseCsvLine(rowBuffer);

            auto get = [&](int idx) -> std::string {
                if (idx >= 0 && idx < (int)cells.size()) return trim(cells[idx]);
                return "";
            };

            CardRow r;
            r.set      = (cSet >= 0 ? get(cSet) : "");
            r.quantity = toIntSafe(get(cQty), 0);
            r.name     = get(cName);
            r.text     = get(cText);   // note: text may contain '\n' because we join with '\n' below
            r.type     = get(cType);
            r.faction  = get(cFaction);
            r.cost     = toIntSafe(get(cCost), 0);
            r.defense  = get(cDefense);
            r.role     = get(cRole);

            // DEBUG
            std::cout << "[readCsv LOGICAL ROW] name='" << r.name
                      << "' qty=" << r.quantity
                      << " type='" << r.type
                      << "' faction='" << r.faction
                      << "' cost=" << r.cost
                      << " defense='" << r.defense
                      << "' role='" << r.role << "'\n";

            if (!r.name.empty() && !r.text.empty()) {
                rows.push_back(r);
            } else {
                std::cout << "  -> SKIP logical row because missing name or text\n";
            }

            rowBuffer.clear();
            return true;
        }
        return false;
    };

    std::string physical;
    while (std::getline(in, physical)) {
        ++physicalLineNum;

        // append newline between physical lines so card text keeps its line breaks
        if (!rowBuffer.empty()) {
            rowBuffer.push_back('\n');
        }
        rowBuffer += physical;

        // attempt flush if rowBuffer is now a full logical row
        flushRowBufferIfComplete(false);
    }
    // force flush at end of file
    flushRowBufferIfComplete(true);

    std::cout << "[readCsv] built " << rows.size() << " logical rows.\n";
    for (size_t i = 0; i < rows.size(); ++i) {
        std::cout << "  ROW " << (i+1)
                  << " name='" << rows[i].name
                  << "' qty=" << rows[i].quantity
                  << " type='" << rows[i].type
                  << "' faction='" << rows[i].faction
                  << "' cost=" << rows[i].cost
                  << " def='" << rows[i].defense
                  << "' role='" << rows[i].role
                  << "'\n";
    }

    return rows;
}

// -------------------------------------------------
// handleAbilities: dumb, case-by-case
// -------------------------------------------------
void CardCreator::handleAbilities(const std::string& fullText,
                                  Faction printedFaction,
                                  Card& cardOut)
{
    cardOut.abilities().clear();

    auto addAbilitiesForTrigger = [&](Trigger trig,
                                      bool requiresAlly,
                                      Faction allyFac,
                                      const std::string& textBlockLower)
    {
        auto push = [&](AbilityName name, int amt) {
            Card::CardAbility a;
            a.trigger = trig;
            a.ability = name;
            a.amount = amt;
            a.requiresAlly = requiresAlly;
            a.requiredAllyFaction = allyFac;
            cardOut.abilities().push_back(a);
        };

        // Gain X gold
        if (textBlockLower.find("{gain 1 gold}") != std::string::npos) push(AbilityName::GainGold, 1);
        if (textBlockLower.find("{gain 2 gold}") != std::string::npos) push(AbilityName::GainGold, 2);
        if (textBlockLower.find("{gain 3 gold}") != std::string::npos) push(AbilityName::GainGold, 3);
        if (textBlockLower.find("{gain 4 gold}") != std::string::npos) push(AbilityName::GainGold, 4);
        if (textBlockLower.find("{gain 5 gold}") != std::string::npos) push(AbilityName::GainGold, 5);

        // Gain X combat
        if (textBlockLower.find("{gain 1 combat}") != std::string::npos) push(AbilityName::GainCombat, 1);
        if (textBlockLower.find("{gain 2 combat}") != std::string::npos) push(AbilityName::GainCombat, 2);
        if (textBlockLower.find("{gain 3 combat}") != std::string::npos) push(AbilityName::GainCombat, 3);
        if (textBlockLower.find("{gain 4 combat}") != std::string::npos) push(AbilityName::GainCombat, 4);
        if (textBlockLower.find("{gain 5 combat}") != std::string::npos) push(AbilityName::GainCombat, 5);
        if (textBlockLower.find("{gain 6 combat}") != std::string::npos) push(AbilityName::GainCombat, 6);
        if (textBlockLower.find("{gain 7 combat}") != std::string::npos) push(AbilityName::GainCombat, 7);
        if (textBlockLower.find("{gain 8 combat}") != std::string::npos) push(AbilityName::GainCombat, 8);

        // bonus +{X combat}
        if (textBlockLower.find("+{1 combat}") != std::string::npos) push(AbilityName::GainCombat, 1);
        if (textBlockLower.find("+{2 combat}") != std::string::npos) push(AbilityName::GainCombat, 2);
        if (textBlockLower.find("+{3 combat}") != std::string::npos) push(AbilityName::GainCombat, 3);

        // Gain X health (authority)
        if (textBlockLower.find("{gain 1 health}") != std::string::npos) push(AbilityName::GainAuthority, 1);
        if (textBlockLower.find("{gain 2 health}") != std::string::npos) push(AbilityName::GainAuthority, 2);
        if (textBlockLower.find("{gain 3 health}") != std::string::npos) push(AbilityName::GainAuthority, 3);
        if (textBlockLower.find("{gain 4 health}") != std::string::npos) push(AbilityName::GainAuthority, 4);
        if (textBlockLower.find("{gain 5 health}") != std::string::npos) push(AbilityName::GainAuthority, 5);
        if (textBlockLower.find("{gain 6 health}") != std::string::npos) push(AbilityName::GainAuthority, 6);

        // Draw cards
        if (textBlockLower.find("draw two cards") != std::string::npos) {
            push(AbilityName::DrawCards, 2);
        }
        if (textBlockLower.find("draw a card") != std::string::npos) {
            push(AbilityName::DrawCards, 1);
        }

        // Opponent discards
        if (textBlockLower.find("target opponent discards a card") != std::string::npos) {
            push(AbilityName::OpponentDiscardsOne, 1);
        }

        // Stun target champion
        if (textBlockLower.find("stun target champion") != std::string::npos) {
            push(AbilityName::StunTargetChampion, 1);
        }

        // Prepare a champion
        if (textBlockLower.find("prepare a champion") != std::string::npos) {
            push(AbilityName::PrepareFriendlyChampion, 1);
        }

        // Put/move something on top of your deck
        if (textBlockLower.find("on top of your deck") != std::string::npos) {
            push(AbilityName::AcquireToTop, 1);
        }
    };

    // normalize to lowercase once
    std::string s = fullText;
    // normalize CRLFs to spaces/newlines
    for (char& ch : s) {
        if (ch == '\r') ch = ' ';
    }

    std::string slow = lower(s);

    // split by "<hr>" = new trigger block
    std::vector<std::string> hrBlocks;
    {
        size_t start = 0;
        while (true) {
            size_t p = slow.find("<hr>", start);
            if (p == std::string::npos) {
                std::string piece = trim(slow.substr(start));
                if (!piece.empty()) hrBlocks.push_back(piece);
                break;
            } else {
                std::string piece = trim(slow.substr(start, p - start));
                if (!piece.empty()) hrBlocks.push_back(piece);
                start = p + 4;
            }
        }
    }

    // inside each hrBlock, split by <i>or</i> but keep same trigger for those subs
    for (const std::string& oneBlockRaw : hrBlocks) {
        std::vector<std::string> orSubs;
        {
            size_t start = 0;
            while (true) {
                size_t p = oneBlockRaw.find("<i>or</i>", start);
                if (p == std::string::npos) {
                    std::string piece = trim(oneBlockRaw.substr(start));
                    if (!piece.empty()) orSubs.push_back(piece);
                    break;
                } else {
                    std::string piece = trim(oneBlockRaw.substr(start, p - start));
                    if (!piece.empty()) orSubs.push_back(piece);
                    start = p + std::string("<i>or</i>").size();
                }
            }
        }

        for (const std::string& subRaw : orSubs) {
            std::string blkLow = lower(trim(subRaw));

            bool mentionsExpend       = blkLow.find("{expend}")     != std::string::npos;
            bool mentionsSacrifice    = blkLow.find("{sacrifice}")  != std::string::npos;
            bool mentionsImperialAlly = blkLow.find("{imperial ally}") != std::string::npos;
            bool mentionsGuildAlly    = blkLow.find("{guild ally}")    != std::string::npos;
            bool mentionsNecrosAlly   = blkLow.find("{necros ally}")   != std::string::npos;
            bool mentionsWildAlly     = blkLow.find("{wild ally}")     != std::string::npos;

            bool didSomething = false;

            // {Expend}:
            if (mentionsExpend) {
                addAbilitiesForTrigger(
                    Trigger::Expend,
                    /*requiresAlly=*/false,
                    /*allyFac=*/Faction::Unknown,
                    blkLow
                );
                didSomething = true;
            }

            // {Sacrifice}:
            if (mentionsSacrifice) {
                addAbilitiesForTrigger(
                    Trigger::Sacrifice,
                    /*requiresAlly=*/false,
                    /*allyFac=*/Faction::Unknown,
                    blkLow
                );
                didSomething = true;
            }

            // {X Ally}:
            if (mentionsImperialAlly) {
                addAbilitiesForTrigger(
                    Trigger::OnPlay,
                    /*requiresAlly=*/true,
                    /*allyFac=*/Faction::Imperial,
                    blkLow
                );
                didSomething = true;
            }
            if (mentionsGuildAlly) {
                addAbilitiesForTrigger(
                    Trigger::OnPlay,
                    /*requiresAlly=*/true,
                    /*allyFac=*/Faction::Guild,
                    blkLow
                );
                didSomething = true;
            }
            if (mentionsNecrosAlly) {
                addAbilitiesForTrigger(
                    Trigger::OnPlay,
                    /*requiresAlly=*/true,
                    /*allyFac=*/Faction::Necros,
                    blkLow
                );
                didSomething = true;
            }
            if (mentionsWildAlly) {
                addAbilitiesForTrigger(
                    Trigger::OnPlay,
                    /*requiresAlly=*/true,
                    /*allyFac=*/Faction::Wild,
                    blkLow
                );
                didSomething = true;
            }

            // if no trigger keyword, assume it's just "on play"
            if (!didSomething) {
                addAbilitiesForTrigger(
                    Trigger::OnPlay,
                    /*requiresAlly=*/false,
                    /*allyFac=*/Faction::Unknown,
                    blkLow
                );
            }
        }
    }
}

// -------------------------------------------------
// buildCardsFromRow: create Champion or ActionCard copies, attach abilities
// -------------------------------------------------
std::vector<Card*> CardCreator::buildCardsFromRow(const CardRow& row) {
    std::cout << "[buildCardsFromRow] '" << row.name << "'\n";
    std::cout << "   typeCell='" << row.type << "'"
              << " -> rowIsChampion=" << (isChampionType(row.type)?"true":"false")
              << " rowIsActionLike=" << ((isActionType(row.type)||isItemType(row.type))?"true":"false")
              << "\n";

    std::vector<Card*> out;

    bool rowIsChampion   = isChampionType(row.type);
    bool rowIsActionLike = isActionType(row.type) || isItemType(row.type);

    if (!rowIsChampion && !rowIsActionLike) {
        std::cout << "   -> SKIP: type not recognized as Champion or Action/Item\n";
        std::cout << "   (NO CARDS CREATED FROM THIS ROW)\n\n";
        return out;
    }

    Faction factionEnum = parseFaction(row.faction);
    std::cout << "   factionParsed=" << (int)factionEnum
              << " from '" << row.faction << "'\n";

    int shieldVal = 0;
    bool guard = false;
    if (rowIsChampion) {
        parseDefenseField(row.defense, shieldVal, guard);
        std::cout << "   defenseCell='" << row.defense
                  << "' -> shield=" << shieldVal
                  << " guard=" << (guard?"true":"false") << "\n";
    }

    int copies = (row.quantity > 0 ? row.quantity : 1);
    std::cout << "   copies=" << copies << " (qty=" << row.quantity << ")\n";

    std::string baseId = slugify(row.name);

    for (int i = 1; i <= copies; ++i) {
        std::string id = (copies > 1)
            ? (baseId + "-" + std::to_string(i))
            : baseId;

        if (rowIsChampion) {
            Champion* c = new Champion(
                id,
                row.name,
                row.cost,
                factionEnum,
                shieldVal,
                guard,
                /*expendable=*/true
            );

            handleAbilities(row.text, factionEnum, *c);

            std::cout << "   -> MADE Champion id='" << id
                      << "' cost=" << row.cost
                      << " shield=" << shieldVal
                      << " guard=" << (guard?"true":"false")
                      << " abilities=" << c->abilities().size()
                      << "\n";

            out.push_back(c);
        } else {
            ActionCard* a = new ActionCard(
                id,
                row.name,
                row.cost,
                factionEnum,
                /*expendable=*/false,
                /*sacrificeable=*/false
            );

            handleAbilities(row.text, factionEnum, *a);

            std::cout << "   -> MADE Action/Item id='" << id
                      << "' cost=" << row.cost
                      << " abilities=" << a->abilities().size()
                      << "\n";

            out.push_back(a);
        }
    }

    std::cout << "\n";
    return out;
}

// -------------------------------------------------
// loadFromCsv: public entry
// -------------------------------------------------
std::vector<Card*> CardCreator::loadFromCsv(const std::string& path) {
    std::cout << "[loadFromCsv] reading '" << path << "'...\n";

    std::vector<Card*> result;
    auto rows = readCsv(path);

    std::cout << "[loadFromCsv] parsed " << rows.size()
              << " logical rows after multiline merge.\n\n";

    for (size_t i = 0; i < rows.size(); ++i) {
        std::cout << "[loadFromCsv] ROW " << (i+1)
                  << ": '" << rows[i].name << "'\n";
        auto made = buildCardsFromRow(rows[i]);
        if (made.empty()) {
            std::cout << "   (NO CARDS CREATED FROM THIS ROW)\n";
        }
        for (auto& ptr : made) {
            result.push_back(std::move(ptr));
        }
    }

    std::cout << "\n[loadFromCsv] TOTAL CREATED PHYSICAL CARDS = "
              << result.size() << "\n";

    return result;
}
