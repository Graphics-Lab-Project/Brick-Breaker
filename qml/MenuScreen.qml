// OWNER: task "Main menu screen". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 360 x 480. Required children: "itemStart", "itemOptions", "itemHelp", "itemQuit"
// (each with `property bool selected`), "bestText".
FocusScope {
    id: root
    property int bestScore: 0
    property int currentIndex: 0

    signal startGame()
    signal openOptions()
    signal openHelp()
    signal quit()

    width: Theme.canvasW
    height: Theme.canvasH
}
