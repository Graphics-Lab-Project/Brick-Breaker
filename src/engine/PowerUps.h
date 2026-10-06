#pragma once
// OWNER: Phase 0 (interface FROZEN). Implementation: task "Power-up rules" -> PowerUps.cpp
#include "Constants.h"
#include "Types.h"

namespace BB {

struct PowerState {
    PaddleMode mode = PaddleMode::Normal;
    int gunAmmo = 0;
    int lives = K::StartLives;
    int paddleWidth() const { return mode == PaddleMode::Long ? K::PaddleLongW : K::PaddleNormalW; }
};

struct CapsuleEffect {
    bool modeChanged = false;   // s.mode differs from before the call
    bool lifeGained = false;    // lives actually increased
    bool multiBall = false;     // caller must split balls
};

namespace PowerUps {
// Rules:
//  Life : lives = min(MaxLives, lives + 1); mode -> Normal; gunAmmo -> 0
//  Multi: multiBall = true; nothing else changes
//  Long : mode -> Long;  gunAmmo -> 0
//  Gun  : mode -> Gun;   gunAmmo -> GunAmmo (refills if already Gun)
//  Laser: mode -> Laser; gunAmmo -> 0
CapsuleEffect apply(PowerState &s, CapsuleType type);
// After a life is lost: mode -> Normal, gunAmmo -> 0 (lives untouched). Returns modeChanged.
bool resetForLifeLost(PowerState &s);
} // namespace PowerUps

} // namespace BB
