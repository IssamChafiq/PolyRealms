#pragma once
#include <cstdint>

enum class Faction : uint8_t {
    Imperial,
    Guild,
    Necros,
    Wild,
    Neutral
};

enum class CardType : uint8_t {
    Action,
    Champion
};

enum class Trigger : uint8_t {
    OnPlay,
    Expend,
    Sacrifice,
    NewTurn
};

enum class AbilityName : uint8_t {
    GainGold,
    GainCombat,
    GainAuthority,
    DrawCards,
    StunTargetChampion,
    PrepareFriendlyChampion,
    SacrificeSelf,
    AcquireToTop,
    OpponentDiscardsOne
};

