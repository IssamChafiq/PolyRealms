#pragma once
#include <cstdint>
#include <string>
#include <algorithm>

enum class Faction : uint8_t {
    Imperial,
    Guild,
    Necros,
    Wild,
    Neutral,    
    Unknown
};

enum class CardType : uint8_t {
    Action,
    Champion
};

enum class Trigger : uint8_t {
    OnPlay,     
    Expend,     
    Sacrifice,  
    Ally,
    OnPlayChoice,
    ExpendChoice,
    SacrificeChoice
};

enum class AbilityName : uint8_t {
    GainGold,
    GainCombat,
    GainAuthority, // heal
    DrawCards,
    StunTargetChampion,
    PrepareFriendlyChampion,
    SacrificeCards,
    AcquireToTop,
    OpponentDiscard,

};

// helper for parsing factions from strings like "Imperial Ally"
inline Faction factionFromStringLoose(std::string s) {
    auto toLower = [](std::string x) {
        std::transform(x.begin(), x.end(), x.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return x;
    };
    s = toLower(s);

    if (s.find("imperial") != std::string::npos) return Faction::Imperial;
    if (s.find("guild")    != std::string::npos) return Faction::Guild;
    if (s.find("necros")   != std::string::npos) return Faction::Necros;
    if (s.find("wild")     != std::string::npos) return Faction::Wild;
    if (s.find("neutral")  != std::string::npos) return Faction::Neutral;

    return Faction::Unknown;
}
