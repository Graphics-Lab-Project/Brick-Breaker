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
    width: paddleWidth
    x: Math.round(paddleX - width / 2)
    y: Theme.paddleTopY

    Behavior on width {
        enabled: root.polish
        NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
    }

    Rectangle {
        objectName: "body"
        anchors.fill: parent
        color: Theme.paddle
    }
    Rectangle {
        objectName: "turret"
        visible: root.paddleMode === Theme.modeGun
        width: 6; height: 4
        x: Math.round((root.width - width) / 2)
        y: -4
        color: Theme.capGun
    }
    Rectangle {
        objectName: "nubLeft"
        visible: root.paddleMode === Theme.modeLaser
        width: 4; height: 4
        x: 0; y: -4
        color: Theme.capLaser
    }
    Rectangle {
        objectName: "nubRight"
        visible: root.paddleMode === Theme.modeLaser
        width: 4; height: 4
        x: root.width - 4; y: -4
        color: Theme.capLaser
    }

    // T1 life-lost blink (no-op when polish is false)
    function blink() {
        if (!polish)
            return
        blinkAnim.restart()
    }

    SequentialAnimation {
        id: blinkAnim
        PropertyAction { target: root; property: "opacity"; value: 0 }
        PauseAnimation { duration: 75 }
        PropertyAction { target: root; property: "opacity"; value: 1 }
        PauseAnimation { duration: 75 }
        PropertyAction { target: root; property: "opacity"; value: 0 }
        PauseAnimation { duration: 75 }
        PropertyAction { target: root; property: "opacity"; value: 1 }
    }
}
