// LEVEL CLEAR banner.
import QtQuick
import BrickBreaker

// Covers the playfield, 336 x 336. Required children: "levelText", "clearText", "nextText", "bar".
Item {
    id: root
    property int level: 1               // the level just cleared
    property int levelCount: 20         // levels wrap to 1 after this one (K::LevelCount)
    property bool polish: Theme.polish

    width: Theme.fieldW
    height: Theme.fieldH

    function play() {
        anim.stop()
        if (polish) {
            bar.width = 0
            anim.start()
        } else {
            bar.width = Theme.fieldW
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
        opacity: 0.85
    }

    Text {
        objectName: "levelText"
        y: 90
        anchors.horizontalCenter: parent.horizontalCenter
        text: "LEVEL " + Theme.pad(root.level, 2)
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontL
        color: Theme.text
    }
    Text {
        objectName: "clearText"
        y: 126
        anchors.horizontalCenter: parent.horizontalCenter
        text: "CLEAR"
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontXL
        color: Theme.accent
    }
    Rectangle {
        id: bar
        objectName: "bar"
        y: 174
        height: 4
        width: Theme.fieldW
        color: Theme.accent
    }
    Text {
        objectName: "nextText"
        y: 194
        anchors.horizontalCenter: parent.horizontalCenter
        text: "NEXT LEVEL " + Theme.pad(root.level % 10 + 1, 2)
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontM
        color: Theme.textDim
    }

    NumberAnimation {
        id: anim
        target: bar
        property: "width"
        from: 0
        to: Theme.fieldW
        duration: 600
        easing.type: Easing.OutCubic
    }
}
