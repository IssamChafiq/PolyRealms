#include "CardCreator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <regex>
#include <iostream>

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
        if (std::isalnum((unsigned char)c)) out.push_back(std::tolower((unsigned char)c));
        else if (std::isspace((unsigned char)c) || c=='-' || c=='_') out.push_back('-');
    }
    out.erase(std::unique(out.begin(), out.end(),
                          [](char a,char b){ return a=='-' && b=='-'; }),
              out.end());
    if (out.empty()) out = "card";
    return out;
}


// Type / Faction

bool CardCreator::isChampionType(const std::string& typeCell) {
    std::string t = lower(trim(typeCell));
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


// Gestion colonne defense

void CardCreator::parseDefenseField(const std::string& defenseCell,
                                    int& shieldOut,
                                    bool& guardOut) {
    shieldOut = 0;
    guardOut  = false;

    std::string lc = lower(defenseCell);
    int tmp = 0; bool seen = false;
    for (char c : lc) {
        if (std::isdigit((unsigned char)c)) { tmp = tmp*10 + (c-'0'); seen = true; }
        else if (seen) break;
    }
    shieldOut = tmp;
    if (lc.find("guard") != std::string::npos) guardOut = true;
}

// parse le csv
std::vector<std::string> CardCreator::parseCsvLine(const std::string& line) {
    std::vector<std::string> cells;
    std::string cur; bool inq = false;
    for (size_t i=0;i<line.size();++i){
        char c=line[i];
        if (c=='"'){
            if (inq && i+1<line.size() && line[i+1]=='"'){ cur.push_back('"'); ++i; }
            else inq=!inq;
        } else if (c==',' && !inq) {
            cells.push_back(cur); cur.clear();
        } else cur.push_back(c);
    }
    cells.push_back(cur);
    return cells;
}


// lecture csv

std::vector<CardRow> CardCreator::readCsv(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Cannot open CSV: " + path);

    std::string header;
    if (!std::getline(in, header)) throw std::runtime_error("CSV is empty: " + path);
    auto headers = parseCsvLine(header);

    auto findCol = [&](std::initializer_list<std::string> alts)->int {
        for (const auto& alt: alts){
            std::string want = lower(trim(alt));
            for (size_t i=0;i<headers.size();++i){
                if (lower(trim(headers[i]))==want) return (int)i;
            }
        }
        return -1;
    };

    int cSet      = findCol({"set","source set","pack","expansion"});
    int cQty      = findCol({"quantity","qty","#","count"});
    int cName     = findCol({"name","card name"});
    int cText     = findCol({"text","card text","rules text"});
    int cType     = findCol({"type","card type"});
    int cFaction  = findCol({"faction","faction / color","color","faction/color"});
    int cCost     = findCol({"cost","price"});
    int cDefense  = findCol({"defense","defence","shield","guard/defense"});
    int cRole     = findCol({"role","deck","location","where"});

    if (cQty<0||cName<0||cText<0||cType<0||cFaction<0||cCost<0||cDefense<0||cRole<0){
        throw std::runtime_error("CSV headers missing");
    }

    std::vector<CardRow> rows;
    std::string rowBuf;

    auto flushIfDone = [&](bool force)->bool{
        if (rowBuf.empty()) return false;
        bool q=false;
        for (size_t i=0;i<rowBuf.size();++i){
            if (rowBuf[i]=='"'){
                if (q && i+1<rowBuf.size() && rowBuf[i+1]=='"') { ++i; }
                else q=!q;
            }
        }
        if (!q || force){
            auto cells = parseCsvLine(rowBuf);
            auto get = [&](int idx)->std::string{
                if (idx>=0 && idx<(int)cells.size()) return trim(cells[idx]);
                return "";
            };
            CardRow r;
            r.set      = (cSet>=0?get(cSet):"");
            r.quantity = toIntSafe(get(cQty),0);
            r.name     = get(cName);
            r.text     = get(cText);
            r.type     = get(cType);
            r.faction  = get(cFaction);
            r.cost     = toIntSafe(get(cCost),0);
            r.defense  = get(cDefense);
            r.role     = get(cRole);
            if (!r.name.empty() && !r.text.empty()) rows.push_back(r);
            rowBuf.clear();
            return true;
        }
        return false;
    };

    std::string phys;
    while (std::getline(in, phys)){
        if (!rowBuf.empty()) rowBuf.push_back('\n');
        rowBuf += phys;
        flushIfDone(false);
    }
    flushIfDone(true);
    return rows;
}


// Abilities: détection trigger + choix

void CardCreator::handleAbilities(const std::string& fullText,
                                  Faction /*printedFaction*/,
                                  Card& cardOut)
{
    cardOut.abilities().clear();

    auto pushAbility = [&](Trigger trig, bool reqAlly, Faction allyFac,
                           AbilityName name, int amt){
        Card::CardAbility a;
        a.trigger             = trig;
        a.ability             = name;
        a.amount              = amt;
        a.requiresAlly        = reqAlly;
        a.requiredAllyFaction = allyFac;
        a.used                = false;
        cardOut.abilities().push_back(a);
    };

    // normalisation
    std::string norm = fullText;
    for (char& c : norm) if (c == '\r') c = '\n';
    std::string s = lower(norm);

    // rendre <hr> et <i>or</i> robustes aux espaces/retours
    s = std::regex_replace(s, std::regex(R"(\s*<\s*hr\s*>\s*)"), "|||HR|||");
    s = std::regex_replace(s, std::regex(R"(\s*<\s*i\s*>\s*or\s*<\s*/\s*i\s*>\s*)"), "|||OR|||");

    // split HR
    std::vector<std::string> hrBlocks;
    {
        size_t start = 0;
        while (true) {
            size_t p = s.find("|||HR|||", start);
            if (p == std::string::npos) {
                std::string piece = trim(s.substr(start));
                if (!piece.empty()) hrBlocks.push_back(piece);
                break;
            } else {
                std::string piece = trim(s.substr(start, p - start));
                if (!piece.empty()) hrBlocks.push_back(piece);
                start = p + 8;
            }
        }
    }

    auto detectAllyFaction = [&](const std::string& blk)->Faction{
        if (blk.find("{imperial ally}") != std::string::npos) return Faction::Imperial;
        if (blk.find("{guild ally}")    != std::string::npos) return Faction::Guild;
        if (blk.find("{necros ally}")   != std::string::npos) return Faction::Necros;
        if (blk.find("{wild ally}")     != std::string::npos) return Faction::Wild;
        return Faction::Unknown;
    };

    for (const std::string& block : hrBlocks) {
        bool hasExpend     = block.find("{expend}")    != std::string::npos;
        bool hasSacrificeT = block.find("{sacrifice}") != std::string::npos;
        Faction allyFac    = detectAllyFaction(block);
        bool isAllyBlock   = (allyFac != Faction::Unknown);

        Trigger baseTrig = Trigger::OnPlay;
        bool reqAlly = false;
        if (hasExpend) baseTrig = Trigger::Expend;
        else if (hasSacrificeT) baseTrig = Trigger::Sacrifice;
        else if (isAllyBlock) { baseTrig = Trigger::Ally; reqAlly = true; }

        // split OR
        std::vector<std::string> orSubs;
        {
            size_t start = 0;
            while (true) {
                size_t p = block.find("|||OR|||", start);
                if (p == std::string::npos) {
                    std::string piece = trim(block.substr(start));
                    if (!piece.empty()) orSubs.push_back(piece);
                    break;
                } else {
                    std::string piece = trim(block.substr(start, p - start));
                    if (!piece.empty()) orSubs.push_back(piece);
                    start = p + 8;
                }
            }
        }
        bool isChoice = (orSubs.size() > 1);

        auto withChoice = [&](Trigger t)->Trigger{
            if (!isChoice) return t;
            switch (t) {
                case Trigger::OnPlay:    return Trigger::OnPlayChoice;
                case Trigger::Expend:    return Trigger::ExpendChoice;
                case Trigger::Sacrifice: return Trigger::SacrificeChoice;
                default:                 return t; 
            }
        };

        for (const std::string& subRaw : orSubs) {
            std::string t = lower(trim(subRaw));
            Trigger trig = withChoice(baseTrig);

            bool suppressDrawOne   = false;
            bool suppressDrawTwo   = false;
            bool suppressFlatCombat= false;
            bool suppressFlatHealth= false;

            auto contains = [&](const char* needle)->bool {
                return t.find(needle) != std::string::npos;
            };

            // per champion / per guard
            if (t.find("for each champion you have in play") != std::string::npos) {
                if (contains("{1 combat}") || contains("+{1 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerChamp, 1), suppressFlatCombat = true;
                if (contains("{2 combat}") || contains("+{2 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerChamp, 2), suppressFlatCombat = true;
                if (contains("{3 combat}") || contains("+{3 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerChamp, 3), suppressFlatCombat = true;

                if (contains("{1 health}") || contains("gain {1 health}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddHealthPerChamp, 1), suppressFlatHealth = true;
                if (contains("{2 health}") || contains("gain {2 health}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddHealthPerChamp, 2), suppressFlatHealth = true;
                if (contains("{3 health}") || contains("gain {3 health}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddHealthPerChamp, 3), suppressFlatHealth = true;
            }
            if (t.find("for each other guard you have in play") != std::string::npos) {
                if (contains("{1 combat}") || contains("+{1 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerGuard, 1), suppressFlatCombat = true;
                if (contains("{2 combat}") || contains("+{2 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerGuard, 2), suppressFlatCombat = true;
            }
            if (t.find("for each other champion you have in play") != std::string::npos) {
                if (contains("{1 combat}") || contains("+{1 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerChamp, 1), suppressFlatCombat = true;
                if (contains("{2 combat}") || contains("+{2 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerChamp, 2), suppressFlatCombat = true;
            }

            // per ally  
            if (t.find("for each other {wild} card you have in play") != std::string::npos) {
                if (contains("{1 combat}") || contains("+{1 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerAlly, 1), suppressFlatCombat = true;
                if (contains("{2 combat}") || contains("+{2 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerAlly, 2), suppressFlatCombat = true;
                if (contains("{3 combat}") || contains("+{3 combat}"))
                    pushAbility(trig, reqAlly, allyFac, AbilityName::AddCombatPerAlly, 3), suppressFlatCombat = true;
            }

            // draw and discard 
            if (t.find("you may draw up to two cards, then discard that many cards") != std::string::npos) {
                pushAbility(trig, reqAlly, allyFac, AbilityName::DrawAndDiscard, 2);
                suppressDrawOne = true; suppressDrawTwo = true;
            } else if (t.find("you may draw a card. if you do, discard a card") != std::string::npos
                       || t.find("draw a card, then discard a card") != std::string::npos) {
                pushAbility(trig, reqAlly, allyFac, AbilityName::DrawAndDiscard, 1);
                suppressDrawOne = true;
            }

            // sacrifice -> bonus combat si “if you do” 
            bool hasMaySacrifice = (t.find("you may sacrifice a card in your hand or discard pile") != std::string::npos);
            if (hasMaySacrifice) {
                pushAbility(trig, reqAlly, allyFac, AbilityName::SacrificeCards, 1);
            }
            {
                int bonus = 0;
                if (t.find("if you do, gain an additional {3 combat}") != std::string::npos) bonus = 3;
                else if (t.find("if you do, gain an additional {2 combat}") != std::string::npos) bonus = 2;
                else if (t.find("if you do, gain an additional {1 combat}") != std::string::npos) bonus = 1;
                if (hasMaySacrifice && bonus > 0) {
                    pushAbility(trig, reqAlly, allyFac, AbilityName::SacrificeForCombat, bonus);
                }
            }

            // acquire/move effects 
            if (t.find("put the next card you acquire this turn into your hand") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::PutNextAcquiredCardInHand, 1);
            if (t.find("put the next action you acquire this turn on top of your deck") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::PutNextAcquiredActionCardInHand, 1);
            if (t.find("put the next card you acquire this turn on top of your deck") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::PutNextAcquiredCardOnDeck, 1);
            if (t.find("you may put a card from your discard pile on top of your deck") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::PutCardFromDiscardOnDeck, 1);
            if (t.find("take a champion from your discard pile and put it on top of your deck") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::PutChampFromDiscardOnDeck, 1);

            // opponent / stun / prepare 
            if (t.find("target opponent discards a card") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::OpponentDiscard, 1);
            if (t.find("stun target champion") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::StunTargetChampion, 1);
            if (t.find("prepare a champion") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::PrepareFriendlyChampion, 1);

            // gains plats (gold/combat/health/draw) avec suppressions locales 
            // gold
            if (t.find("{gain 1 gold}") != std::string::npos || t.find("gain {1 gold}") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::GainGold, 1);
            if (t.find("{gain 2 gold}") != std::string::npos || t.find("gain {2 gold}") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::GainGold, 2);
            if (t.find("{gain 3 gold}") != std::string::npos || t.find("gain {3 gold}") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::GainGold, 3);
            if (t.find("{gain 4 gold}") != std::string::npos || t.find("gain {4 gold}") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::GainGold, 4);
            if (t.find("{gain 5 gold}") != std::string::npos || t.find("gain {5 gold}") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::GainGold, 5);

            // combat plat (skip si per-*)
            if (!suppressFlatCombat) {
                if (t.find("{gain 1 combat}") != std::string::npos || t.find("gain {1 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 1);
                if (t.find("{gain 2 combat}") != std::string::npos || t.find("gain {2 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 2);
                if (t.find("{gain 3 combat}") != std::string::npos || t.find("gain {3 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 3);
                if (t.find("{gain 4 combat}") != std::string::npos || t.find("gain {4 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 4);
                if (t.find("{gain 5 combat}") != std::string::npos || t.find("gain {5 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 5);
                if (t.find("{gain 6 combat}") != std::string::npos || t.find("gain {6 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 6);
                if (t.find("{gain 7 combat}") != std::string::npos || t.find("gain {7 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 7);
                if (t.find("{gain 8 combat}") != std::string::npos || t.find("gain {8 combat}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainCombat, 8);
            }


            if (!suppressFlatHealth) {
                if (t.find("{gain 1 health}") != std::string::npos || t.find("gain {1 health}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainAuthority, 1);
                if (t.find("{gain 2 health}") != std::string::npos || t.find("gain {2 health}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainAuthority, 2);
                if (t.find("{gain 3 health}") != std::string::npos || t.find("gain {3 health}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainAuthority, 3);
                if (t.find("{gain 4 health}") != std::string::npos || t.find("gain {4 health}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainAuthority, 4);
                if (t.find("{gain 5 health}") != std::string::npos || t.find("gain {5 health}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainAuthority, 5);
                if (t.find("{gain 6 health}") != std::string::npos || t.find("gain {6 health}") != std::string::npos)
                    pushAbility(trig, reqAlly, allyFac, AbilityName::GainAuthority, 6);
            }


            if (!suppressDrawTwo && t.find("draw two cards") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::DrawCards, 2);
            if (!suppressDrawOne && t.find("draw a card") != std::string::npos)
                pushAbility(trig, reqAlly, allyFac, AbilityName::DrawCards, 1);
        }
    }
}





// Constructions carte
std::vector<Card*> CardCreator::buildCardsFromRow(const CardRow& row) {
    std::vector<Card*> out;

    bool isChamp = isChampionType(row.type);
    bool isActionLike = isActionType(row.type) || isItemType(row.type);
    if (!isChamp && !isActionLike) return out;

    Faction f = parseFaction(row.faction);

    int shield=0; bool guard=false;
    if (isChamp) parseDefenseField(row.defense, shield, guard);

    int copies = (row.quantity>0?row.quantity:1);
    std::string baseId = slugify(row.name);

    for (int i=1;i<=copies;++i){
        std::string id = (copies>1)? (baseId+"-"+std::to_string(i)) : baseId;
        if (isChamp){
            Champion* c = new Champion(id,row.name,row.cost,f,shield,guard,true);
            handleAbilities(row.text, f, *c);
            out.push_back(c);
        } else {
            ActionCard* a = new ActionCard(id,row.name,row.cost,f,false,false);
            handleAbilities(row.text, f, *a);
            out.push_back(a);
        }
    }
    return out;
}

std::vector<Card*> CardCreator::loadFromCsv(const std::string& path) {
    std::vector<Card*> result;
    auto rows = readCsv(path);
    for (const auto& r : rows){
        auto made = buildCardsFromRow(r);
        result.insert(result.end(), made.begin(), made.end());
    }
    return result;
}
