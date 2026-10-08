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

    y: boardOffsetRows * 24

    Behavior on y {
        enabled: root.polish
        NumberAnimation { duration: 120; easing.type: Easing.OutCubic }
    }

    Repeater {
        id: brickRepeater
        objectName: "brickRepeater"
        model: root.bricks
        delegate: Brick {
            required property var model
            x: model.col * 48 + 1
            y: model.row * 24 + 1
            hitsLeft: model.hitsLeft
            unbreakable: model.unbreakable
            alive: model.alive
            polish: root.polish
        }
    }

    function brickAt(row, col) {
        if (row < 0 || row >= 14 || col < 0 || col >= 7)
            return null
        return brickRepeater.itemAt(row * 7 + col)
    }
    function flashBrick(row, col) {
        var b = brickAt(row, col)
        if (b)
            b.flash()
    }
}
