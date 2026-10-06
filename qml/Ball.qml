// OWNER: task "Ball, capsule, projectile sprites". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Ball sprite. Required children: "core", "ghost0", "ghost1", "ghost2".
Item {
    id: root
    property real centerX: 0
    property real centerY: 0
    property bool fast: false
    property bool polish: Theme.polish

    width: Theme.ballSize
    height: Theme.ballSize
}
