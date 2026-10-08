import QtQuick
import BrickBreaker

// Three-letter initials picker. Required children: "slot0", "slot1", "slot2" (each `property bool active`).
FocusScope {
    id: root
    property string initials: "AAA"
    property int cursor: 0

    signal submitted(string initials)

    width: 96
    height: 24

    function changeLetter(delta) {
        var code = initials.charCodeAt(cursor) - 65
        code = ((code + delta) % 26 + 26) % 26
        initials = initials.substring(0, cursor) + String.fromCharCode(65 + code)
                   + initials.substring(cursor + 1)
    }

    Keys.onUpPressed: changeLetter(1)
    Keys.onDownPressed: changeLetter(-1)
    Keys.onLeftPressed: cursor = Math.max(0, cursor - 1)
    Keys.onRightPressed: cursor = Math.min(2, cursor + 1)
    Keys.onReturnPressed: root.submitted(root.initials)
    Keys.onEnterPressed: root.submitted(root.initials)

    component Slot: Rectangle {
        id: slot
        required property int index
        property bool active: root.cursor === index
        x: index * 32
        width: 32
        height: root.height
        color: "transparent"
        border.width: active ? 2 : 1
        border.color: active ? Theme.accent : Theme.line

        Text {
            anchors.centerIn: parent
            text: root.initials.charAt(slot.index)
            color: slot.active ? Theme.accent : Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontM
        }
    }

    Slot { objectName: "slot0"; index: 0 }
    Slot { objectName: "slot1"; index: 1 }
    Slot { objectName: "slot2"; index: 2 }
}
