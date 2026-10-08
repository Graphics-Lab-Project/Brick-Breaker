// OWNER: task "Main menu screen". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 360 x 480. Required children: "itemStart", "itemOptions", "itemHelp", "itemQuit"
// (each with `property bool selected`), "bestText".
FocusScope {
    id: root
    property int bestScore: 0
    property int currentIndex: 0

    signal startGame()
    signal openOptions()
    signal openHelp()
    signal quit()

    width: Theme.canvasW
    height: Theme.canvasH

    function activate(i) {
        if (i === 0) startGame()
        else if (i === 1) openOptions()
        else if (i === 2) openHelp()
        else if (i === 3) quit()
    }

    component MenuItem: Item {
        id: item
        property bool selected: false
        property string label: ""
        signal clicked()
        width: 200
        height: 32
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: item.selected ? Theme.accent : Theme.lineFaint
            border.width: 1
        }
        Text {
            anchors.centerIn: parent
            text: item.label
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontM
            color: item.selected ? Theme.accent : Theme.textDim
        }
        MouseArea {
            anchors.fill: parent
            onClicked: item.clicked()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
    }

    // logo: brick row + title
    Row {
        x: (root.width - width) / 2
        y: 56
        spacing: 2
        Repeater {
            model: 6
            Rectangle {
                width: 46
                height: 22
                color: index % 2 === 0 ? Theme.brickRed : Theme.brickAmber
            }
        }
    }
    Text {
        x: 0
        y: 90
        width: root.width
        horizontalAlignment: Text.AlignHCenter
        text: "BRICK BREAKER"
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontL
        color: Theme.text
    }

    Column {
        x: (root.width - width) / 2
        y: 180
        spacing: 12
        MenuItem {
            objectName: "itemStart"
            label: "START GAME"
            selected: root.currentIndex === 0
            onClicked: { root.currentIndex = 0; root.activate(0) }
        }
        MenuItem {
            objectName: "itemOptions"
            label: "OPTIONS"
            selected: root.currentIndex === 1
            onClicked: { root.currentIndex = 1; root.activate(1) }
        }
        MenuItem {
            objectName: "itemHelp"
            label: "HELP"
            selected: root.currentIndex === 2
            onClicked: { root.currentIndex = 2; root.activate(2) }
        }
        MenuItem {
            objectName: "itemQuit"
            label: "QUIT"
            selected: root.currentIndex === 3
            onClicked: { root.currentIndex = 3; root.activate(3) }
        }
    }

    Text {
        objectName: "bestText"
        x: 0
        y: 420
        width: root.width
        horizontalAlignment: Text.AlignHCenter
        text: "BEST " + Theme.pad(root.bestScore, 5)
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontM
        color: Theme.textDim
    }

    focus: true
    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Up) {
            currentIndex = (currentIndex + 3) % 4
            event.accepted = true
        } else if (event.key === Qt.Key_Down) {
            currentIndex = (currentIndex + 1) % 4
            event.accepted = true
        } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter
                   || event.key === Qt.Key_Space) {
            activate(currentIndex)
            event.accepted = true
        }
    }
}
