// OWNER: task "Ball, capsule, projectile sprites". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Falling capsule, 20 x 10, label to the right. Required children: "body", "label".
Item {
    id: root
    property int capsuleType: 0
    property bool polish: Theme.polish

    width: Theme.capsuleW
    height: Theme.capsuleH
}
