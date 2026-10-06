// OWNER: task "App root + main window". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Screen switcher, 360 x 480. Required children: "menuScreen", "optionsScreen", "helpScreen", "gameScreen".
FocusScope {
    id: root
    property var engine: null
    property string screen: "menu"      // "menu" | "options" | "help"

    width: Theme.canvasW
    height: Theme.canvasH
}
