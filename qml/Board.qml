// OWNER: task "Board + playfield". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// The brick wall. Required child: "brickRepeater" (Repeater over `bricks`).
Item {
    id: root
    property var bricks: null           // BrickModel (roles row, col, hitsLeft, unbreakable, alive, tier)
    property int boardOffsetRows: 0
    property bool polish: Theme.polish

    width: Theme.fieldW
    height: Theme.fieldH

    function brickAt(row, col) { return null }
    function flashBrick(row, col) {}
}
