// OWNER: task "Options screen". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 360 x 480. Required children: "rowSpeed", "rowAccel", "rowBack" (each `property bool current`),
// "speedValue", "seg0".."seg4" (each `property bool filled`), "accelOn", "accelOff"
// (each `property bool selected`).
FocusScope {
    id: root
    property int paddleSpeed: 3
    property bool acceleration: false
    property int currentRow: 0          // 0 speed, 1 acceleration, 2 back

    signal paddleSpeedRequested(int value)
    signal accelerationRequested(bool on)
    signal back()

    width: Theme.canvasW
    height: Theme.canvasH
}
