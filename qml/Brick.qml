// OWNER: task "Brick component". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// One brick sprite, 46 x 22 (the Board positions it in its 48 x 24 cell).
// Required children (objectName): "face" (Rectangle), "hitFlash" (Rectangle, opacity 0 at rest).
Item {
    id: root
    property int hitsLeft: 1
    property bool unbreakable: false
    property bool alive: true
    property bool polish: Theme.polish

    width: Theme.brickW
    height: Theme.brickH

    // T1 hit flash (no-op when polish is false)
    function flash() {}
}
