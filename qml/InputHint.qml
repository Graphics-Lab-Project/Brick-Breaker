// OWNER: task "Input hint row". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Chevron row, 360 x 28. Required children: "chevronLeft", "chevronRight" (each with
// `property bool lit` and a `color`), "hintText" (Text).
Item {
    id: root
    property int inputDir: 0
    property int gameState: 0
    property int paddleMode: 0

    width: Theme.canvasW
    height: 28
}
