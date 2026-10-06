// OWNER: task "HUD + pause button". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Top HUD, 344 x 60. Required children: "leftPanel", "rightPanel", "scoreText", "bestText",
// "levelText", "ammoText", "ammo0", "ammo1", "ammo2", "livesText".
Item {
    id: root
    property int score: 0
    property int bestScore: 0
    property int level: 1
    property int gunAmmo: 0
    property int lives: 3
    property bool polish: Theme.polish

    width: 344
    height: 60

    function flashLifeLost() {}     // T1
    function pulseLifeGained() {}   // T1
}
