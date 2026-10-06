// OWNER: task "Paddle component". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Paddle in field coordinates. Required children: "body", "turret", "nubLeft", "nubRight".
Item {
    id: root
    property real paddleX: 168          // centre
    property int paddleWidth: 64
    property int paddleMode: 0          // Theme.modeNormal/Long/Gun/Laser
    property bool polish: Theme.polish

    height: Theme.paddleH

    // T1 life-lost blink (no-op when polish is false)
    function blink() {}
}
