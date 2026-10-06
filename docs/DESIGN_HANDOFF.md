# DESIGN_HANDOFF — Brick Breaker clone (Qt 6.8 / QML + C++)

Visual direction: **"Phosphor"**. This is a modern-retro, minimal skin for AMOLED screens, chosen by the project owner instead of the screenshot skin.
Layout, HUD placement and proportions come from the screenshots. Colours and art are new, so every visual value below is **ASSUMED** unless it says otherwise.
The design canvas holds every mockup, the component sheet, the motion storyboards, the tokens and the manifest. This file is the implementation contract.

---

## 1. Canvas, scaling, crispness

| Item | Value |
|---|---|
| Logical canvas | **360 × 480** (3:4 portrait) |
| Window | **720 × 960**, fixed (`minimumWidth = maximumWidth = 720`, same for height) |
| Scale | Root `Item { width: 360; height: 480; scale: 2; transformOrigin: Item.TopLeft }` |
| Crisp pixels | `Image { smooth: false; mipmap: false }` on every sprite; `layer.smooth: false`; sprite positions rounded to integers (`Math.round`) in QML bindings |
| Text | One font: **Silkscreen** (OFL), bundled `.ttf` through `FontLoader`. Sizes 8/16/24/32 only. `font.hintingPreference: Font.PreferNoHinting`, `renderType: Text.QtRendering` |
| HiDPI | Ship `name.png` (1x) and `name@2x.png`; Qt picks `@2x` automatically |

All coordinates in this document are logical px. All game coordinates are relative to the **playfield origin (12, 104)**.

## 2. Tokens (`Theme.qml`, singleton)

```qml
pragma Singleton
import QtQuick
QtObject {
    readonly property color bg:          "#000000"
    readonly property color line:        "#2A2A2A"
    readonly property color lineFaint:   "#1A1A1A"
    readonly property color text:        "#EDEDED"
    readonly property color textDim:     "#7A7A7A"
    readonly property color accent:      "#46E6FF"   // selection, paddle core, Long capsule
    readonly property color paddle:      "#3D7BFF"
    readonly property color ball:        "#F5F5F5"
    readonly property color brickRed:    "#FF4A33"   // tier 2 fill, tier 1 edge, GAME OVER
    readonly property color brickRedDeep:"#2A0B07"   // tier 1 fill
    readonly property color brickAmber:  "#FFB020"   // tier 3, Gun, hazard, best score
    readonly property color silverFace:  "#7E858E"
    readonly property color silverHi:    "#E3E7EC"
    readonly property color silverLo:    "#4A5058"
    readonly property color capLife:     "#5BE38A"
    readonly property color capMulti:    "#F5F5F5"
    readonly property color capLong:     "#46E6FF"
    readonly property color capGun:      "#FFB020"
    readonly property color capLaser:    "#FF4FD8"
    readonly property color chevronIdle: "#1E5F6B"
    readonly property color ammoEmpty:   "#4A4A4A"
    readonly property real  flashAlpha:  0.18        // catch-flash field = capsule colour at 18% over black
    readonly property int   unit: 8
    readonly property bool  polish: true             // T1 on/off. T0 must work with this false
}
```

Spacing: base unit 8. Screen gutter is 8 in-game and 24 on menus. Panel padding is 8. Menu items are 24 tall with a 6 gap. Borders are 1 px, with radius 2 on HUD panels only.

## 3. Geometry (ASSUMED portrait adaptation of S02)

| Element | Rect (x, y, w × h) | Notes |
|---|---|---|
| HUD left panel | 8, 8, 168 × 60 | Trophy + best (8 px, amber) top-left. `LV nn` (8 px, dim) top-right. Score (24 px) at 8, 28 |
| HUD right panel | 184, 8, 168 × 60 | Row 1: gun ammo count (16 px) + ammo icon. Row 2: lives count (16 px) + paddle icon. Divider at y 29 |
| Pause button | 328, 74, 24 × 20 | Sits under the right panel, as in S02 |
| Hazard strip | 8, 98, 344 × 2 | Tiled `frame_hazard.png` (nod to S02's frame) |
| Field frame | 8, 100, 344 × 344 | 1 px `line` |
| **Playfield** | **12, 104, 336 × 336** | `clip: true` |
| Brick grid | **7 cols × 14 rows, cell 48 × 24** | Sprite 46 × 22 inset 1 px. 7 columns measured from S02 (2-brick clusters in cols 1–2 and 4–5) |
| Paddle | top at field y 322 | 64 × 6 normal, 96 × 6 long. Gun/laser add parts 4 px above |
| Ball | 6 × 6 | Resting on paddle: centre (paddleX, 319) |
| Capsule | 20 × 10 + label | Label is 8 px text in the capsule colour, 3 px to the right (from S03's "Long" label) |
| Bullet / laser bolt | 2 × 6 / 2 × 10 | |
| Chevron row | 0, 448, 360 × 28 | Replaces S02's touch arrows. Chevrons light while input is held. The centre shows a key hint |

Brick tiers (by `hitsLeft`): **3 = solid amber (ASSUMED, late levels)**, **2 = solid red (S02's bright smooth red)**, **1 = hollow red outline on deep-red fill (S02's cracked red)**. Silver = bevelled grey. Damage therefore reads as "fill drains away".

## 4. Engine → UI contract

### Properties on `GameEngine` (QObject, registered as a QML singleton or a context object)

| Property | Type | Notes |
|---|---|---|
| `gameState` | enum `GameState { Menu, Ready, Playing, Paused, LevelCleared, GameOver }` | `Ready` = ball on paddle, waiting for launch |
| `score`, `bestScore` | int | HUD pads to 5 digits |
| `lives` | int | Starts at 3 |
| `level` | int | 1..10 (v1), loops |
| `round` | int | Pass number (1 = first pass) |
| `speedState` | enum `SpeedState { Slow, Fast }` | |
| `paddleX` | real | Centre, field coords |
| `paddleWidth` | int | 64 or 96 |
| `paddleMode` | enum `PaddleMode { Normal, Long, Gun, Laser }` | |
| `gunAmmo` | int | 0..3 |
| `boardOffsetRows` | int | Wall descent steps (0..maxDescent) |
| `bricks` | `BrickModel` (QAbstractListModel) | Roles: `row`, `col`, `hitsLeft`, `unbreakable`, `alive` |
| `balls` | `BallModel` | Roles: `x`, `y` (centre) |
| `capsules` | `CapsuleModel` | Roles: `type` (enum `CapsuleType { Life, Multi, Long, Gun, Laser }`), `x`, `y` (top-left) |
| `projectiles` | `ProjectileModel` | Roles: `kind` (enum `Bullet, Laser`), `x`, `y` |
| `ballCount` | int | Convenience: `balls.rowCount()` |
| `inputDir` | int | −1 / 0 / +1, drives the chevrons |

### Signals (exactly what the motion layer listens to)

`brickHit(int r, int c, int hitsLeft, bool unbreakable)` · `brickBroken(int r, int c, int tier)` · `capsuleSpawned(int type, real x, real y)` · `capsuleCaught(int type)` · `capsuleLost()` · `boardShifted(int steps)` · `paddleModeChanged(int mode)` · `projectileFired(int kind, real x, real y)` · `multiBallActivated()` · `lifeLost()` · `lifeGained()` · `levelCleared()` · `gameOver()` · `speedStateChanged(int state)`

### Invokables (QML → engine; input only)

`moveLeft(bool pressed)`, `moveRight(bool pressed)`, `setPointerX(real fieldX)`, `launchOrFire()`, `togglePause()`, `startGame()`, `setPaddleSpeed(int 1..5)`, `setAcceleration(bool)`, `submitInitials(string)`.

QML never decides collisions, scoring, timing of gameplay or capsule effects. QML timers exist only for presentation (flash hold, banners, effects).

## 5. QML component tree

```
main.qml                      Window 720×960, FontLoader ×2, root Item scale 2
└─ AppRoot.qml                StackLayout / Loader on gameState
   ├─ MenuScreen.qml          consumes bestScore → startGame(), opens Options/Help
   ├─ OptionsScreen.qml       paddle speed 1..5, acceleration ON/OFF (QSettings)
   ├─ HelpScreen.qml          capsule legend (5), brick legend, score, controls
   └─ GameScreen.qml
      ├─ Hud.qml              score, bestScore, level, gunAmmo, lives  (+ T1 counter flash/pulse)
      ├─ PauseButton.qml      togglePause()
      ├─ Playfield.qml        336×336, clip; catch-flash tint + name (capsuleCaught)
      │  ├─ Board.qml         y = boardOffsetRows*24 (Behavior when Theme.polish)
      │  │  └─ Repeater{bricks} → Brick.qml       hitsLeft, unbreakable, alive
      │  ├─ Repeater{capsules}    → Capsule.qml   type, x, y
      │  ├─ Repeater{projectiles} → Projectile.qml kind, x, y
      │  ├─ Repeater{balls}       → Ball.qml      x, y, speedState (T1 trail)
      │  ├─ Paddle.qml        paddleX, paddleWidth, paddleMode
      │  ├─ FxLayer.qml       T1 only: BreakFx, ScorePop, SplitFlash, Muzzle (spawned on signals)
      │  ├─ PauseOverlay.qml  gameState == Paused
      │  ├─ LevelBanner.qml   gameState == LevelCleared
      │  └─ GameOverOverlay.qml gameState == GameOver (+ InitialsEntry.qml, HighScoreTable.qml)
      └─ InputHint.qml        inputDir, gameState (hint text: SPACE LAUNCH / SPACE FIRE / empty)
```

Keyboard: a `Keys` handler on `GameScreen` forwards to the invokables. Mouse: `MouseArea` over the playfield calls `setPointerX` and `launchOrFire`.

## 6. Motion spec

All T1 work is gated by `Theme.polish`. With it false the game must look exactly like T0.

### T0 (faithful)

| Effect | Trigger | QML target | ms | Easing |
|---|---|---|---|---|
| Brick hit | `brickHit` | `Brick` image source from `hitsLeft` | 0 | swap |
| Silver hit | `brickHit(unbreakable)` | — | 0 | none |
| Brick break | `brickBroken` | delegate hidden (`alive=false`) | 0 | same frame |
| Capsule spawn / fall | `capsuleSpawned` | `Capsule.y` bound to model | engine | constant vy |
| **Capsule caught** (from S03) | `capsuleCaught` | `Playfield.flashType` → field colour = capsule × 0.18, 24 px name at field top; HUD score +50 | hold **500** (tune) | on/off |
| Capsule missed | `capsuleLost` | removed from model | engine | — |
| Wall descent | `boardShifted` | `Board.y` | 0 | 24 px jump |
| Paddle Long | `paddleModeChanged` | `Paddle.width` 64 → 96 | 0 | swap |
| Gun / Laser fire | `projectileFired` | `Projectile.y` bound | engine | constant vy |
| Multi | `multiBallActivated` | Repeater gets 4 balls at the old ball's position | 0 | — |
| Life lost / gained | `lifeLost` / `lifeGained` | `Hud.lives` | 0 | — |
| Level cleared | `levelCleared` | `LevelBanner` visible, next level appears after **1500** (engine, tune) | 0 | — |
| Game over | `gameOver` | `GameOverOverlay` visible (opacity 0.9) | 0 | — |
| Speed change | `speedStateChanged` | none | — | — |

### T1 (polish, in v1)

| Effect | QML implementation | ms | Easing |
|---|---|---|---|
| Brick hit flash | white `Rectangle` over brick: opacity 1 → 0; brick `height` 22 → 20 → 22 (2 px squash on the hit axis) | 60 | Linear |
| Silver glint | 3 × 22 `silver_glint.png`, x −8 → 54, clipped to the brick | 80 | Linear |
| Brick break | `BreakFx` at (r, c): `scale` 1 → 0.6, `opacity` 1 → 0. Six 2 × 2 `Rectangle` shards in the tier colour, vx ±(20..60) px/s, vy −40 px/s, g = 600 px/s², fade over 180 ms | 150 (+30 shards) | OutQuad |
| Capsule pop-in | `scale` 0.6 → 1 | 100 | OutBack (overshoot 1.2) |
| +50 float | `ScorePop` at the paddle: y −24, opacity 1 → 0 | 500 | OutQuad |
| Wall slide | `Behavior on y { NumberAnimation }` on `Board` | 120 | Linear |
| Long tween | `Behavior on width` (paddle is a `BorderImage`, border 4) | 150 | OutCubic |
| Muzzle flash | `muzzle.png` at the turret or nubs, opacity 1 → 0 | 60 | Linear |
| Multi split flash | 10 × 10 white rect at the ball, opacity 0.7 → 0 | 80 | Linear |
| Life lost | paddle opacity 1/0/1/0 (75 ms steps); `Hud.lives` colour red → text | 300 | Step |
| Life gained | `Hud.lives` scale 1 → 1.3 → 1, colour `capLife` at the peak | 200 | OutQuad/InQuad |
| Level banner | banner width 0 → 336 (600 ms, OutCubic), then next bricks fade in, 40 ms stagger per row, 160 ms each (≤ 400 total) | 600 + 400 | OutCubic; Linear |
| Game over fade | overlay opacity 0 → 0.9 | 400 | InOutQuad |
| Fast trail | 3 ghost `Image`s at the last 3 sampled ball positions, opacity 0.55 / 0.3 / 0.15, only while `speedState == Fast` | continuous | — |

Note: the T1 Long tween lags the engine's collision width by 150 ms. That is acceptable, but flagged (see open question 15).

## 7. Levels (v1 = 10)

Format: one string per row, 7 chars. `.` empty, `1`/`2`/`3` = hits, `S` = silver. Rows start at grid row 0. Level 1 comes from S02. **Levels 2–10 are ASSUMED placeholders** until the original layouts are supplied.

```
L01 (S02)   L02         L03         L04         L05
.......     .......     .......     .......     .......
.11.11.     1111111     ...2...     .1.1.1.     2.2.2.2
.11.21.     .......     ..212..     1.1.1.1     1.1.1.1
.......     2222222     .11211.     .2.2.2.     1.1.1.1
.21.21.     .......     1112111     1.1.1.1     1.1.1.1
.11.11.     1111111                 .1.1.1.     2.2.2.2
.......
.12.11.
.11.11.

L06         L07         L08         L09 (≈orig 13, silver)   L10 (≈orig 16, silver)
...2...     2222222     3333333     .......                  .......
..121..     2.....2     1111111     .33333.                  1S1S1S1
.12221.     2.111.2     2222222     S22122S                  2121212
..121..     2.....2     1111111     S12321S                  2S2S2S2
...2...     2222222                 S11111S                  1111111
                                    SSS.SSS                  .S.S.S.
                                                             3.3.3.3
```

Wall descent: `maxDescentRows = 4` (tune). The descent step happens on each paddle hit while `speedState == Fast` (pass 1).

## 8. Asset manifest

| File | Size | Frames | Used by |
|---|---|---|---|
| brick_t3.png / brick_t2.png / brick_t1.png | 46×22 | 1 each | Brick.qml |
| brick_silver.png | 46×22 | 1 | Brick.qml |
| silver_glint.png | 3×22 | 1 | Brick.qml (T1) |
| paddle_body.png | 12×6 | 1 | Paddle.qml (BorderImage, border 4) |
| paddle_gun_turret.png | 6×4 | 1 | Paddle.qml |
| paddle_laser_nub.png | 4×4 | 1 | Paddle.qml (×2) |
| muzzle.png | 6×4 | 1 | FxLayer.qml (T1) |
| ball.png | 6×6 | 1 | Ball.qml |
| capsule_{life,multi,long,gun,laser}.png | 20×10 | 1 each | Capsule.qml, HelpScreen.qml |
| bullet.png / laser_bolt.png | 2×6 / 2×10 | 1 | Projectile.qml |
| frame_hazard.png | 6×2 | tile | Playfield.qml (`fillMode: Image.Tile`) |
| icon_trophy.png | 8×8 | 1 | Hud.qml, MenuScreen.qml |
| icon_ammo.png | sheet 32×6 | 2 (grey, amber) | Hud.qml |
| icon_life.png | 20×4 | 1 | Hud.qml |
| chevron.png | sheet 48×20 | 4 (L idle, L held, R idle, R held) | InputHint.qml |
| silkscreen-regular.ttf, silkscreen-bold.ttf | — | — | main.qml |

Drawn as `Rectangle`s (no asset): hit flash, shards, overlays, panels, buttons, the pause icon, the menu logo brick row, the catch-flash tint.

## 9. Assumptions (all ASSUMED)

1. The skin is new ("Phosphor"), by request. Only hues and layout come from the screenshots.
2. 7 × 14 grid with 48 × 24 cells. The column count is measured from S02. The row count fits the 336 px square field.
3. S02's bright smooth red = 2 hits, cracked red = 1 hit. The amber 3-hit tier exists only for later levels.
4. S02's missile counter = Gun ammo. The paddle counter = lives. Lives start at 3 (the screenshot's 5 is mid-game).
5. The level number appears as `LV nn` inside the score panel.
6. The catch flash (S03) is used for all five capsules, holding for 500 ms.
7. The touch arrows become input chevrons with a key hint between them.
8. High scores: 5 entries, 3-letter initials, stored in QSettings.
9. Levels 2–10 are placeholders.
10. Main menu, pause, level cleared, game over, options and help are **EXTRAPOLATED** (no screenshots).

## 10. Open questions

1. Can you supply the original layouts for levels 2–8, 13 and 16? Otherwise, are the placeholders in §7 approved?
2. Do any bricks need 3–4 hits? (The amber tier is assumed.)
3. Is the laser hit worth 5 or 10 points?
4. Does the catch flash fire for every capsule, and how long does it hold?
5. What are the tune values: slow/fast ball speed, capsule fall speed, the ~50-bounce threshold, and max descent?
6. Is the missile counter definitely Gun ammo?
7. Is the `LV nn` label in the score panel acceptable?
8. Is a high-score table of 5 entries with 3-letter initials right?
9. Launch direction: straight up, or angled by paddle velocity?
10. Mouse: does the paddle follow absolute X? Does acceleration apply to the mouse?
11. Should the chevrons be clickable (hold to move)?
12. Multi caught while the ball is low: what launch angles?
13. Can the player open Options from the Pause menu?
14. Skip pass 2/3 speed behaviour in v1?
15. Is a 150 ms visual lag on the Long tween acceptable, or should the engine also tween collision width?
