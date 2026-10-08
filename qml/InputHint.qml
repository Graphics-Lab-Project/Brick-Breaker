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

    // One chevron glyph. Lit (accent) while its direction is held, idle otherwise.
    component Chevron: Text {
        property bool lit: false
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontL
        font.bold: true
        color: lit ? Theme.accent : Theme.chevronIdle
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        width: 24
        height: root.height
        anchors.verticalCenter: parent.verticalCenter
    }

    Chevron {
        objectName: "chevronLeft"
        text: "<"
        lit: root.inputDir === -1
        anchors.left: parent.left
        anchors.leftMargin: Theme.unit
    }

    Chevron {
        objectName: "chevronRight"
        text: ">"
        lit: root.inputDir === 1
        anchors.right: parent.right
        anchors.rightMargin: Theme.unit
    }

    Text {
        objectName: "hintText"
        anchors.centerIn: parent
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontS
        color: Theme.textDim
        text: {
            if (root.gameState === Theme.stateReady)
                return "SPACE LAUNCH"
            if (root.gameState === Theme.statePlaying
                    && (root.paddleMode === Theme.modeGun || root.paddleMode === Theme.modeLaser))
                return "SPACE FIRE"
            return ""
        }
    }
}
