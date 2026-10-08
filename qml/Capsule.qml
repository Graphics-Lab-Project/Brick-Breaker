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

    // T1 pop-in (presentation only): scale 0.6 -> 1, 100 ms, OutBack.
    Component.onCompleted: {
        if (polish) {
            scale = 0.6
            popIn.start()
        }
    }
    NumberAnimation {
        id: popIn
        target: root
        property: "scale"
        to: 1
        duration: 100
        easing.type: Easing.OutBack
        easing.overshoot: 1.2
    }

    Rectangle {
        id: body
        objectName: "body"
        anchors.fill: parent
        color: Theme.capsuleColor(root.capsuleType)
    }

    Text {
        id: label
        objectName: "label"
        x: 23
        anchors.verticalCenter: parent.verticalCenter
        text: Theme.capsuleName(root.capsuleType)
        color: Theme.capsuleColor(root.capsuleType)
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontS
    }
}
