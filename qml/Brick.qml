// OWNER: task "Brick component". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// One brick sprite, 46 x 22 (the Board positions it in its 48 x 24 cell).
// Required children (objectName): "face" (Rectangle), "hitFlash" (Rectangle, opacity 0 at rest).
Item {
    id: root
    property int hitsLeft: 1
    property bool unbreakable: false
    property bool alive: true
    property bool polish: Theme.polish

    width: Theme.brickW
    height: Theme.brickH

    visible: alive

    property real squash: 0

    Rectangle {
        id: face
        objectName: "face"
        width: parent.width
        height: parent.height - root.squash
        anchors.verticalCenter: parent.verticalCenter
        color: root.unbreakable ? Theme.silverFace
             : root.hitsLeft >= 3 ? Theme.brickAmber
             : root.hitsLeft === 2 ? Theme.brickRed
             : Theme.brickRedDeep
        border.width: root.unbreakable ? 1 : (root.hitsLeft === 1 ? 1 : 0)
        border.color: root.unbreakable ? Theme.silverHi : Theme.brickRed

        Rectangle {
            id: hitFlash
            objectName: "hitFlash"
            anchors.fill: parent
            color: "white"
            opacity: 0
        }
    }

    ParallelAnimation {
        id: flashAnim
        NumberAnimation { target: hitFlash; property: "opacity"; from: 1; to: 0; duration: 60; easing.type: Easing.Linear }
        SequentialAnimation {
            NumberAnimation { target: root; property: "squash"; to: 2; duration: 30 }
            NumberAnimation { target: root; property: "squash"; to: 0; duration: 30 }
        }
    }

    // T1 hit flash (no-op when polish is false)
    function flash() {
        if (!polish)
            return
        flashAnim.stop()
        hitFlash.opacity = 1
        flashAnim.restart()
    }
}
