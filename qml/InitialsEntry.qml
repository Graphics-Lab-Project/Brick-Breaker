// OWNER: task "Game over + initials". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Three-letter initials picker. Required children: "slot0", "slot1", "slot2" (each `property bool active`).
FocusScope {
    id: root
    property string initials: "AAA"
    property int cursor: 0

    signal submitted(string initials)

    width: 96
    height: 24
}
