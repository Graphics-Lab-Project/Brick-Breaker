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

    component Row_: Rectangle {
        property bool current: false
        width: 296
        height: 56
        color: "transparent"
        border.width: 1
        border.color: current ? Theme.accent : Theme.lineFaint
    }
    component Label_: Text {
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontM
    }

    function changeSpeed(delta) {
        var v = Math.max(1, Math.min(5, paddleSpeed + delta))
        if (v !== paddleSpeed) paddleSpeedRequested(v)
    }
    function requestAccel(on) {
        if (on !== acceleration) accelerationRequested(on)
    }

    Rectangle { anchors.fill: parent; color: Theme.bg }

    Label_ {
        x: 0; y: 40; width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: "OPTIONS"
        color: Theme.accent
        font.pixelSize: Theme.fontL
    }

    Row_ {
        objectName: "rowSpeed"
        x: 32; y: 120
        current: root.currentRow === 0
        Label_ { x: 12; y: 8; text: "PADDLE SPEED"; font.pixelSize: Theme.fontS
            color: parent.current ? Theme.text : Theme.textDim }
        Row {
            x: 12; y: 30; spacing: 4
            Repeater {
                model: 5
                Rectangle {
                    required property int index
                    objectName: "seg" + index
                    property bool filled: index < root.paddleSpeed
                    width: 24; height: 12
                    color: filled ? Theme.accent : "transparent"
                    border.width: 1
                    border.color: filled ? Theme.accent : Theme.line
                }
            }
        }
        Label_ {
            objectName: "speedValue"
            x: 248; y: 24
            width: 36
            horizontalAlignment: Text.AlignRight
            text: String(root.paddleSpeed)
        }
    }

    Row_ {
        objectName: "rowAccel"
        x: 32; y: 200
        current: root.currentRow === 1
        Label_ { x: 12; y: 8; text: "ACCELERATION"; font.pixelSize: Theme.fontS
            color: parent.current ? Theme.text : Theme.textDim }
        Rectangle {
            objectName: "accelOn"
            property bool selected: root.acceleration
            x: 12; y: 28; width: 60; height: 22
            color: selected ? Theme.accent : "transparent"
            border.width: 1
            border.color: selected ? Theme.accent : Theme.line
            Label_ {
                anchors.centerIn: parent
                text: "ON"
                font.pixelSize: Theme.fontS
                color: parent.selected ? Theme.bg : Theme.textDim
            }
            MouseArea {
                anchors.fill: parent
                onClicked: { root.currentRow = 1; root.requestAccel(true) }
            }
        }
        Rectangle {
            objectName: "accelOff"
            property bool selected: !root.acceleration
            x: 80; y: 28; width: 60; height: 22
            color: selected ? Theme.accent : "transparent"
            border.width: 1
            border.color: selected ? Theme.accent : Theme.line
            Label_ {
                anchors.centerIn: parent
                text: "OFF"
                font.pixelSize: Theme.fontS
                color: parent.selected ? Theme.bg : Theme.textDim
            }
            MouseArea {
                anchors.fill: parent
                onClicked: { root.currentRow = 1; root.requestAccel(false) }
            }
        }
    }

    Row_ {
        objectName: "rowBack"
        x: 32; y: 280; height: 40
        current: root.currentRow === 2
        Label_ {
            anchors.centerIn: parent
            text: "BACK"
            color: parent.current ? Theme.text : Theme.textDim
        }
        MouseArea {
            anchors.fill: parent
            onClicked: { root.currentRow = 2; root.back() }
        }
    }

    Keys.onPressed: (event) => {
        event.accepted = true
        switch (event.key) {
        case Qt.Key_Up:
            currentRow = Math.max(0, currentRow - 1)
            break
        case Qt.Key_Down:
            currentRow = Math.min(2, currentRow + 1)
            break
        case Qt.Key_Left:
            if (currentRow === 0) changeSpeed(-1)
            else if (currentRow === 1) requestAccel(true)
            break
        case Qt.Key_Right:
            if (currentRow === 0) changeSpeed(1)
            else if (currentRow === 1) requestAccel(false)
            break
        case Qt.Key_Return:
        case Qt.Key_Enter:
            if (currentRow === 1) accelerationRequested(!acceleration)
            else if (currentRow === 2) back()
            break
        case Qt.Key_Escape:
            back()
            break
        default:
            event.accepted = false
        }
    }
}
