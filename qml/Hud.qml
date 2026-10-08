// OWNER: task "HUD + pause button". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Top HUD, 344 x 60. Required children: "leftPanel", "rightPanel", "scoreText", "bestText",
// "levelText", "ammoText", "ammo0", "ammo1", "ammo2", "livesText".
Item {
    id: root
    property int score: 0
    property int bestScore: 0
    property int level: 1
    property int gunAmmo: 0
    property int lives: 3
    property bool polish: Theme.polish

    width: 344
    height: 60

    // 0..1 effect drivers (only ever non-zero when polish is on)
    property real lossFlash: 0
    property real gainPulse: 0

    function flashLifeLost() {
        if (!polish) return
        gainAnim.stop()
        root.gainPulse = 0
        lossAnim.restart()
    }
    function pulseLifeGained() {
        if (!polish) return
        lossAnim.stop()
        root.lossFlash = 0
        gainAnim.restart()
    }

    NumberAnimation { id: lossAnim; target: root; property: "lossFlash"; from: 1; to: 0; duration: 300 }
    SequentialAnimation {
        id: gainAnim
        NumberAnimation { target: root; property: "gainPulse"; from: 0; to: 1; duration: 100; easing.type: Easing.OutQuad }
        NumberAnimation { target: root; property: "gainPulse"; from: 1; to: 0; duration: 100; easing.type: Easing.InQuad }
    }

    function mix(a, b, t) {
        return Qt.rgba(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, 1)
    }

    component Panel: Rectangle {
        width: 168; height: 60
        color: Theme.bg
        border.color: Theme.line
        border.width: 1
        radius: 2
    }
    component Label: Text {
        font.family: Theme.fontFamily
        color: Theme.text
        renderType: Text.NativeRendering
    }

    Panel {
        objectName: "leftPanel"
        x: 0; y: 0
        Label {
            x: 8; y: 8
            font.pixelSize: Theme.fontS
            color: Theme.brickAmber
            text: "\u2605"
        }
        Label {
            objectName: "bestText"
            x: 20; y: 8
            font.pixelSize: Theme.fontS
            color: Theme.brickAmber
            text: Theme.pad(root.bestScore, 5)
        }
        Label {
            objectName: "levelText"
            anchors.right: parent.right
            anchors.rightMargin: 8
            y: 8
            font.pixelSize: Theme.fontS
            color: Theme.textDim
            text: "LV " + Theme.pad(root.level, 2)
        }
        Label {
            objectName: "scoreText"
            x: 8; y: 28
            font.pixelSize: Theme.fontL
            text: Theme.pad(root.score, 5)
        }
    }

    Panel {
        objectName: "rightPanel"
        x: 176; y: 0
        Label {
            objectName: "ammoText"
            x: 8; y: 6
            font.pixelSize: Theme.fontM
            text: String(root.gunAmmo)
        }
        Row {
            x: 40; y: 10
            spacing: 4
            Repeater {
                model: 3
                Rectangle {
                    objectName: "ammo" + index
                    width: 6; height: 10
                    color: index < root.gunAmmo ? Theme.brickAmber : Theme.ammoEmpty
                }
            }
        }
        Rectangle {
            x: 1; y: 29
            width: parent.width - 2; height: 1
            color: Theme.line
        }
        Label {
            objectName: "livesText"
            x: 8; y: 36
            font.pixelSize: Theme.fontM
            text: String(root.lives)
            color: root.mix(root.mix(Theme.text, Theme.brickRed, root.lossFlash),
                            Theme.capLife, root.gainPulse)
            scale: 1 + 0.3 * root.gainPulse
            transformOrigin: Item.Left
        }
        Rectangle {
            x: 40; y: 42
            width: 24; height: 6
            color: Theme.paddle
        }
    }
}
