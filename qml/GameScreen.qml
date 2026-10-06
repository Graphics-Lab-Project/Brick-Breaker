// OWNER: task "Game screen". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 360 x 480 in-game screen. Required children: "hud", "pauseButton", "playfield", "pointerArea",
// "fxLayer", "inputHint", "pauseOverlay", "levelBanner", "gameOverOverlay".
FocusScope {
    id: root
    property var engine: null
    property bool polish: Theme.polish

    signal optionsRequested()

    width: Theme.canvasW
    height: Theme.canvasH
}
