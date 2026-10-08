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

    // Presentation-only trail: the last three sampled centres (newest in g0).
    property real g0x: 0
    property real g0y: 0
    property real g1x: 0
    property real g1y: 0
    property real g2x: 0
    property real g2y: 0

    function sample() {
        g2x = g1x; g2y = g1y
        g1x = g0x; g1y = g0y
        g0x = centerX; g0y = centerY
    }

    Component.onCompleted: {
        g0x = g1x = g2x = centerX
        g0y = g1y = g2y = centerY
    }
    onCenterXChanged: sample()
    onCenterYChanged: sample()

    x: Math.round(centerX - 3)
    y: Math.round(centerY - 3)

    Rectangle {
        id: ghost2
        objectName: "ghost2"
        width: Theme.ballSize
        height: Theme.ballSize
        x: Math.round(root.g2x - 3) - root.x
        y: Math.round(root.g2y - 3) - root.y
        radius: width / 2
        color: Theme.ball
        opacity: 0.15
        visible: root.fast && root.polish
    }
    Rectangle {
        id: ghost1
        objectName: "ghost1"
        width: Theme.ballSize
        height: Theme.ballSize
        x: Math.round(root.g1x - 3) - root.x
        y: Math.round(root.g1y - 3) - root.y
        radius: width / 2
        color: Theme.ball
        opacity: 0.30
        visible: root.fast && root.polish
    }
    Rectangle {
        id: ghost0
        objectName: "ghost0"
        width: Theme.ballSize
        height: Theme.ballSize
        x: Math.round(root.g0x - 3) - root.x
        y: Math.round(root.g0y - 3) - root.y
        radius: width / 2
        color: Theme.ball
        opacity: 0.55
        visible: root.fast && root.polish
    }
    Rectangle {
        id: core
        objectName: "core"
        anchors.fill: parent
        radius: width / 2
        color: Theme.ball
    }
}
