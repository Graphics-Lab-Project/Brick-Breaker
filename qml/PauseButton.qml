// OWNER: task "HUD + pause button". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 24 x 20 pause button. Required children: "bar0", "bar1".
Item {
    id: root
    signal clicked()
    width: 24
    height: 20
}
