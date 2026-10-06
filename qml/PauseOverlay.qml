// OWNER: task "Pause overlay + level banner". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Covers the playfield, 336 x 336. Required children: "dim", "title",
// "itemResume", "itemOptions", "itemQuit" (each `property bool selected`).
FocusScope {
    id: root
    property int currentIndex: 0

    signal resume()
    signal openOptions()
    signal quitToMenu()

    width: Theme.fieldW
    height: Theme.fieldH
}
