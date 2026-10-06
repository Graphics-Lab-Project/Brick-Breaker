#pragma once
// OWNER: Phase 0. FROZEN. Read-only for every task agent.
// All tuning values approved by the team (see docs/PLAN.md "Gameplay numbers").
// Units: logical pixels (field coordinates, origin = playfield top-left) and seconds.
#include <QtGlobal>

namespace BB::K {

// Field and grid
constexpr qreal FieldW = 336.0;
constexpr qreal FieldH = 336.0;
constexpr int   Cols = 7;
constexpr int   Rows = 14;
constexpr qreal CellW = 48.0;
constexpr qreal CellH = 24.0;

// Fixed timestep
constexpr qreal FixedDt = 1.0 / 120.0;   // engine always steps in these increments
constexpr qreal MaxTickDt = 0.05;        // tick(dt) clamps dt to [0, MaxTickDt]

// Paddle
constexpr qreal PaddleTopY = 322.0;
constexpr qreal PaddleH = 6.0;
constexpr int   PaddleNormalW = 64;
constexpr int   PaddleLongW = 96;
constexpr qreal PaddleStartX = 168.0;    // centre
constexpr int   MinPaddleSpeedSetting = 1;
constexpr int   MaxPaddleSpeedSetting = 5;
constexpr int   DefaultPaddleSpeedSetting = 3;
constexpr qreal PaddleSpeeds[5] = {120.0, 180.0, 240.0, 300.0, 360.0}; // px/s for settings 1..5
constexpr qreal AccelRampSec = 0.15;     // keyboard acceleration ramp 0 -> full speed
constexpr qreal TurretH = 4.0;           // gun turret / laser nubs sit this far above the paddle top

// Ball
constexpr qreal BallSize = 6.0;
constexpr qreal BallHalf = 3.0;
constexpr qreal BallRestY = 319.0;       // centre y while resting on the paddle
constexpr qreal SlowSpeed = 180.0;
constexpr qreal FastSpeed = 260.0;
constexpr int   FastAfterPaddleHits = 50;
constexpr qreal MaxBounceAngleDeg = 60.0; // from vertical, at the paddle edge
constexpr qreal LaunchAngleDeg = 30.0;    // from vertical
constexpr qreal MultiAnglesDeg[4] = {-50.0, -20.0, 20.0, 50.0}; // from vertical, upward

// Capsules
constexpr int   CapsuleDropOneIn = 6;    // rand(6) == 0 -> drop
constexpr int   CapsuleTypeCount = 5;
constexpr qreal CapsuleW = 20.0;
constexpr qreal CapsuleH = 10.0;
constexpr qreal CapsuleFallSpeed = 80.0;

// Weapons
constexpr int   GunAmmo = 3;
constexpr qreal ProjectileSpeed = 400.0;
constexpr qreal BulletW = 2.0;
constexpr qreal BulletH = 6.0;
constexpr qreal LaserBoltW = 2.0;
constexpr qreal LaserBoltH = 10.0;
constexpr qreal LaserCooldown = 0.25;

// Scoring
constexpr int BrickHitPoints = 10;
constexpr int CapsulePoints = 50;
constexpr int GunKillPoints = 50;
constexpr int LaserHitPoints = 10;

// Lives, levels, wall
constexpr int   StartLives = 3;
constexpr int   MaxLives = 9;
constexpr int   LevelCount = 10;
constexpr qreal LevelClearDelay = 1.5;
constexpr int   MaxDescentRows = 4;

// High scores
constexpr int HighScoreEntries = 5;

} // namespace BB::K
