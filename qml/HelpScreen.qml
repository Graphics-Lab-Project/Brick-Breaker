// OWNER: task "Help screen". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 360 x 480. Required children: "capsuleName0".."capsuleName4", "capsuleDesc0".."capsuleDesc4",
// "scoreBrickHit", "scoreCapsule", "scoreGunKill", "scoreLaserHit".
FocusScope {
    id: root
    signal back()

    width: Theme.canvasW
    height: Theme.canvasH
    focus: true

    readonly property var capsuleDescs: [
        "+1 LIFE, ENDS POWERS",
        "4 BALLS",
        "WIDER PADDLE",
        "3 SHOTS, BREAKS ALL",
        "TWIN BOLTS, NOT SILVER"
    ]

    component Label_: Text {
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontS
    }

    component Header_: Text {
        x: 32
        width: parent.width - 64
        color: Theme.accent
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontS
    }

    component BrickSwatch_: Rectangle {
        property alias label: swatchLabel.text
        property color fillColor: Theme.silverFace
        property color edgeColor: Theme.silverHi
        width: 46
        height: 22
        color: fillColor
        border.width: 1
        border.color: edgeColor
        Label_ {
            id: swatchLabel
            x: 0
            y: 26
            width: 72
            color: Theme.textDim
            font.pixelSize: Theme.fontS
        }
    }

    Rectangle { anchors.fill: parent; color: Theme.bg }

    Text {
        x: 0; y: 16; width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: "HELP"
        color: Theme.accent
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontL
    }

    // capsule legend
    Header_ { y: 56; text: "CAPSULES" }

    Repeater {
        model: 5
        Item {
            required property int index
            x: 32; y: 76 + index * 24
            width: 296; height: 20

            Rectangle {
                x: 0; y: 5
                width: Theme.capsuleW; height: Theme.capsuleH
                color: Theme.capsuleColor(index)
            }
            Label_ {
                objectName: "capsuleName" + index
                x: 32; y: 2
                width: 72
                text: Theme.capsuleName(index)
                color: Theme.capsuleColor(index)
            }
            Label_ {
                objectName: "capsuleDesc" + index
                x: 104; y: 2
                width: 192
                text: root.capsuleDescs[index]
                color: Theme.textDim
            }
        }
    }

    // brick legend
    Header_ { y: 210; text: "BRICKS" }

    BrickSwatch_ {
        x: 32; y: 228
        label: "SILVER"
        fillColor: Theme.silverFace
        edgeColor: Theme.silverHi
    }
    BrickSwatch_ {
        x: 112; y: 228
        label: "1 HIT"
        fillColor: Theme.brickRedDeep
        edgeColor: Theme.brickRed
    }
    BrickSwatch_ {
        x: 192; y: 228
        label: "2 HITS"
        fillColor: Theme.brickRed
        edgeColor: Theme.brickRed
    }
    BrickSwatch_ {
        x: 272; y: 228
        label: "3 HITS"
        fillColor: Theme.brickAmber
        edgeColor: Theme.brickAmber
    }

    // scoring
    Header_ { y: 284; text: "SCORING" }

    Label_ { x: 32; y: 302; text: "BRICK HIT" }
    Label_ {
        objectName: "scoreBrickHit"
        x: 260; y: 302; width: 68
        horizontalAlignment: Text.AlignRight
        text: "10"
        color: Theme.accent
    }
    Label_ { x: 32; y: 318; text: "CAPSULE" }
    Label_ {
        objectName: "scoreCapsule"
        x: 260; y: 318; width: 68
        horizontalAlignment: Text.AlignRight
        text: "50"
        color: Theme.accent
    }
    Label_ { x: 32; y: 334; text: "GUN KILL" }
    Label_ {
        objectName: "scoreGunKill"
        x: 260; y: 334; width: 68
        horizontalAlignment: Text.AlignRight
        text: "50"
        color: Theme.accent
    }
    Label_ { x: 32; y: 350; text: "LASER HIT" }
    Label_ {
        objectName: "scoreLaserHit"
        x: 260; y: 350; width: 68
        horizontalAlignment: Text.AlignRight
        text: "10"
        color: Theme.accent
    }

    // controls
    Header_ { y: 378; text: "CONTROLS" }

    Label_ { x: 32; y: 396; text: "LEFT / RIGHT   MOVE PADDLE"; color: Theme.textDim }
    Label_ { x: 32; y: 412; text: "SPACE           LAUNCH / FIRE"; color: Theme.textDim }
    Label_ { x: 32; y: 428; text: "ESC / ENTER     BACK"; color: Theme.textDim }

    Keys.onPressed: (event) => {
        switch (event.key) {
        case Qt.Key_Escape:
        case Qt.Key_Return:
        case Qt.Key_Enter:
            event.accepted = true
            root.back()
            break
        default:
            event.accepted = false
        }
    }
}
