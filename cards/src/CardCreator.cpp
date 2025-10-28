#include "CardCreator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>

std::string CardCreator::trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace((unsigned char)s[a])) a++;
    size_t b = s.size();
    while (b > a && std::isspace((unsigned char)s[b-1])) b--;
    return s.substr(a, b-a);
}

std::string CardCreator::lower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
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
            out.push_back(std::tolower(c));
        } else if (std::isspace((unsigned char)c) || c=='-' || c=='_') {
            out.push_back('-');
        }
    }
    // collapse duplicate '-'
    out.erase(std::unique(out.begin(), out.end(),
                          [](char a,char b){ return a=='-' && b=='-'; }),
              out.end());
    if (out.empty()) out = "card";
    return out;
}

bool CardCreator::isChampionType(const std::string& typeCell) {
    std::string t = lower(trim(typeCell));
    // Champion au début
    return t.rfind("champion", 0) == 0;
}

bool CardCreator::isActionType(const std::string& typeCell) {
    std::string t = lower(trim(typeCell));
    return t.rfind("action", 0) == 0;
}

bool CardCreator::isItemType(const std::string& typeCell) {
    std::string t = lower(trim(typeCell));
    return t.rfind("item", 0) == 0;
}

Faction CardCreator::parseFaction(const std::string& factionCell) {
    std::string f = lower(trim(factionCell));
    if (f.find("imperial") != std::string::npos) return Faction::Imperial;
    if (f.find("guild")    != std::string::npos) return Faction::Guild;
    if (f.find("necros")   != std::string::npos) return Faction::Necros;
    if (f.find("wild")     != std::string::npos) return Faction::Wild;
    if (f.empty() || f.find("neutral") != std::string::npos) return Faction::Neutral;
    return Faction::Unknown;
}

// Defense column comme "6 Guard" or "5" or ""
void CardCreator::parseDefenseField(const std::string& defenseCell,
                                    int& shieldOut,
                                    bool& guardOut) {
    shieldOut = 0;
    guardOut  = false;

    std::regex numRe(R"((\d+))");
    std::smatch m;
    if (std::regex_search(defenseCell, m, numRe)) {
        shieldOut = toIntSafe(m[1].str(), 0);
    }

    std::string lc = lower(defenseCell);
    if (lc.find("guard") != std::string::npos) {
        guardOut = true;
    }
}


// traitement csv
std::vector<std::string> CardCreator::parseCsvLine(const std::string& line) {
    std::vector<std::string> cells;
    std::string cur;
    bool inq = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inq && i+1 < line.size() && line[i+1] == '"') {
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

std::vector<CardRow> CardCreator::readCsv(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Cannot open CSV: " + path);
    }

    std::string headerLine;
    if (!std::getline(in, headerLine)) {
        throw std::runtime_error("CSV is empty: " + path);
    }

    auto headers = parseCsvLine(headerLine);

    // Resolve column indexes, with aliases
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
    int cQty      = findColMulti({"quantity","qty","#","count"});
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
            "CSV is missing one or more required headers. "
            "Need: Qty, Name, Text, Type, Faction/Color, Cost, Defense, Role"
        );
    }

    std::vector<CardRow> rows;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        auto cells = parseCsvLine(line);

        auto get = [&](int idx) -> std::string {
            if (idx >= 0 && idx < (int)cells.size()) return cells[idx];
            return "";
        };

        CardRow r;
        r.set      = (cSet >= 0 ? trim(get(cSet)) : "");
        r.quantity = toIntSafe(get(cQty), 0);
        r.name     = trim(get(cName));
        r.text     = trim(get(cText));
        r.type     = trim(get(cType));
        r.faction  = trim(get(cFaction));
        r.cost     = toIntSafe(get(cCost), 0);
        r.defense  = trim(get(cDefense));
        r.role     = trim(get(cRole));

        if (!r.name.empty() && r.quantity > 0) {
            rows.push_back(std::move(r));
        }
    }

    return rows;
}

void CardCreator::parseTextIntoAbilities(const std::string& rulesText,
                                         Faction cardFaction,
                                         Card& cardOut)
{
    (void)cardFaction; 
    // split on <hr>
    std::vector<std::string> chunks;
    {
        std::string temp = rulesText;
        size_t start = 0;
        while (true) {
            size_t pos = temp.find("<hr>", start);
            if (pos == std::string::npos) {
                std::string last = trim(temp.substr(start));
                if (!last.empty()) chunks.push_back(last);
                break;
            }
            std::string part = trim(temp.substr(start, pos - start));
            if (!part.empty()) chunks.push_back(part);
            start = pos + 4;
        }
    }

    // regex helpers for abilities inside a chunk
    auto addAbility = [&](Trigger trig,
                          AbilityName ability,
                          int amount,
                          bool requiresAlly,
                          Faction allyFaction)
    {
        Card::CardAbility ab;
        ab.trigger = trig;
        ab.ability = ability;
        ab.amount = amount;
        ab.requiresAlly = requiresAlly;
        ab.requiredAllyFaction = allyFaction;
        cardOut.abilities().push_back(ab);
    };

    auto parseSingleEffect = [&](const std::string& effectText,
                                 Trigger trig,
                                 bool requiresAlly,
                                 Faction allyFaction)
    {
        std::string low = lower(effectText);

        // Gain X gold
        {
            std::regex rgx(R"(\{?\s*gain\s+(\d+)\s+gold)");
            std::smatch m;
            if (std::regex_search(low, m, rgx)) {
                int amt = toIntSafe(m[1].str(), 0);
                addAbility(trig, AbilityName::GainGold, amt, requiresAlly, allyFaction);
            }
        }

        // Gain X combat
        {
            std::regex rgx(R"(\{?\s*gain\s+(\d+)\s+combat)");
            std::smatch m;
            if (std::regex_search(low, m, rgx)) {
                int amt = toIntSafe(m[1].str(), 0);
                addAbility(trig, AbilityName::GainCombat, amt, requiresAlly, allyFaction);
            }
        }

        // Gain X health 
        {
            std::regex rgx(R"(\{?\s*gain\s+(\d+)\s+health)");
            std::smatch m;
            if (std::regex_search(low, m, rgx)) {
                int amt = toIntSafe(m[1].str(), 0);
                addAbility(trig, AbilityName::GainAuthority, amt, requiresAlly, allyFaction);
            }
        }

        // Draw N cards 
        if (low.find("draw") != std::string::npos) {
            std::regex twoRe(R"(draw\s+two\s+cards)");
            std::regex oneRe(R"(draw\s+a\s+card)");
            std::regex nRe(R"(draw\s+(\d+)\s+cards?)");

            if (std::regex_search(low, twoRe)) {
                addAbility(trig, AbilityName::DrawCards, 2, requiresAlly, allyFaction);
            } else if (std::regex_search(low, oneRe)) {
                addAbility(trig, AbilityName::DrawCards, 1, requiresAlly, allyFaction);
            } else {
                std::smatch m;
                if (std::regex_search(low, m, nRe)) {
                    int amt = toIntSafe(m[1].str(), 1);
                    addAbility(trig, AbilityName::DrawCards, amt, requiresAlly, allyFaction);
                }
            }
        }

        // Target opponent discards a card.
        if (low.find("target opponent discards a card") != std::string::npos ||
            low.find("opponent discards a card") != std::string::npos) {
            addAbility(trig, AbilityName::OpponentDiscardsOne, 1, requiresAlly, allyFaction);
        }

        // Stun target champion.
        if (low.find("stun target champion") != std::string::npos ||
            low.find("you may stun target champion") != std::string::npos) {
            addAbility(trig, AbilityName::StunTargetChampion, 1, requiresAlly, allyFaction);
        }

        // Prepare a champion / Prepare a friendly champion.
        if (low.find("prepare a champion") != std::string::npos ||
            low.find("prepare a friendly champion") != std::string::npos) {
            addAbility(trig, AbilityName::PrepareFriendlyChampion, 1, requiresAlly, allyFaction);
        }

        // Put the next card you acquire this turn on top of your deck.
        // or "Put the next action you acquire this turn on top of your deck."
        if (low.find("put the next card you acquire this turn on top of your deck") != std::string::npos ||
            low.find("put the next action you acquire this turn on top of your deck") != std::string::npos ||
            low.find("put the next champion you acquire this turn on top of your deck") != std::string::npos) {
            addAbility(trig, AbilityName::AcquireToTop, 1, requiresAlly, allyFaction);
        }

        // Sacrifice to modify maybe
        // right now we just look for "{Sacrifice}:"
    };

    for (const std::string& rawChunk : chunks) {
        std::string chunk = trim(rawChunk);

        // Etape 1 : detect trigger / ally  / sacrifice 
        Trigger trig = Trigger::OnPlay;
        bool requiresAlly = false;
        Faction allyFaction = Faction::Unknown;
        bool isSacrificeHeader = false;

        // verifier si "{Expend}:"
        // Ally if "{xxx}:" / "{xxx Ally}:"
        // We'll do this in a loop because some text may start "{Expend}: {Gain...}"
        {
            // regex finds leading "{Stuff}:" at start
            std::regex headerRe(R"(^\s*\{([^}]+)\}\s*:\s*(.*)$)", std::regex::icase);
            std::smatch m;
            if (std::regex_match(chunk, m, headerRe)) {
                std::string header = m[1].str();   // e.g. "Expend" or "Necros Ally" or "Sacrifice"
                std::string rest   = m[2].str();   // rest of text after header
                std::string headerLow = lower(header);

                // classify header
                if (headerLow.find("expend") != std::string::npos) {
                    trig = Trigger::Expend;
                }
                else if (headerLow.find("sacrifice") != std::string::npos) {
                    trig = Trigger::Sacrifice;
                    isSacrificeHeader = true;
                }
                else if (headerLow.find("ally") != std::string::npos) {
                    // "{Wild Ally}" etc.
                    trig = Trigger::OnPlay;
                    requiresAlly = true;
                    allyFaction = factionFromStringLoose(headerLow); // e.g. "wild ally" -> Faction::Wild
                }
                else {
                    // fallback: any unknown header becomes OnPlay
                    trig = Trigger::OnPlay;
                }

                chunk = trim(rest);
            }
        }

        // Step 2: split  on "<i>or</i>" for choice
        std::vector<std::string> options;
        {
            std::string tmp = chunk;
            std::string sep = "<i>or</i>";
            size_t pos = 0, last = 0;
            while ((pos = lower(tmp).find(lower(sep), last)) != std::string::npos) {
                std::string left = trim(tmp.substr(last, pos - last));
                if (!left.empty()) options.push_back(left);
                last = pos + sep.size();
            }
            std::string tail = trim(tmp.substr(last));
            if (!tail.empty()) options.push_back(tail);

            if (options.empty()) {
                options.push_back(chunk);
            }
        }

        // Step 3: for each option, parse out all individual sub-effects
        for (const std::string& opt : options) {
            parseSingleEffect(opt, trig, requiresAlly, allyFaction);

            if (isSacrificeHeader) {
                // Je pose amount = 1 ici
                addAbility(trig,
                           AbilityName::SacrificeSelf,
                           1,
                           /*requiresAlly*/ false,
                           /*allyFaction*/ Faction::Unknown);
            }
        }
    }
}


std::vector<Card*> CardCreator::buildCardsFromRow(const CardRow& row) {
    std::vector<Card*> out;

    bool rowIsChampion   = isChampionType(row.type);
    bool rowIsActionLike = isActionType(row.type) || isItemType(row.type);

    // en cas d'erreur ou non reconnu ne pas créer
    if (!rowIsChampion && !rowIsActionLike) {
        return out;
    }

    Faction factionEnum = parseFaction(row.faction);

    int shieldVal = 0;
    bool guard = false;
    if (rowIsChampion) {
        parseDefenseField(row.defense, shieldVal, guard);
    }

    int copies = (row.quantity > 0 ? row.quantity : 1);
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

            // parse abilities from text and attach to this card
            parseTextIntoAbilities(row.text, factionEnum, *c);

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

            parseTextIntoAbilities(row.text, factionEnum, *a);

            out.push_back(a);
        }
    }

    return out;
}

std::vector<Card*> CardCreator::loadFromCsv(const std::string& path) {
    std::vector<Card*> result;
    auto rows = readCsv(path);

    for (const auto& r : rows) {
        std::vector<Card*> made = buildCardsFromRow(r);
        for (Card* ptr : made) {
            result.push_back(ptr);
        }
    }
    return result;
}
