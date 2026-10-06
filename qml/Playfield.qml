// OWNER: task "Board + playfield". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 336 x 336 playfield, clip. Required children: "board" (Board), "paddle" (Paddle),
// "ballRepeater", "capsuleRepeater", "projectileRepeater", "flashTint", "flashName".
Item {
    id: root
    property var engine: null
    property bool polish: Theme.polish
    property int flashType: -1          // capsule type being flashed, -1 when idle

    width: Theme.fieldW
    height: Theme.fieldH
}
