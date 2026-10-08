// Pause menu over the playfield.
import QtQuick
import BrickBreaker

// Covers the playfield, 336 x 336. Required children: "dim", "title",
// "itemResume", "itemOptions", "itemQuit" (each `property bool selected`).
FocusScope {
    id: root
    property int currentIndex: 0

    signal resume()
    signal openOptions()
    signal quitToMenu()

    width: Theme.fieldW
    height: Theme.fieldH

    component MenuItem: Item {
        id: mi
        property bool selected: false
        property string label: ""
        signal activated()
        width: 200
        height: 32
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: Theme.accent
            border.width: mi.selected ? 2 : 0
        }
        Text {
            anchors.centerIn: parent
            text: mi.label
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontM
            color: mi.selected ? Theme.accent : Theme.textDim
        }
        MouseArea {
            anchors.fill: parent
            onClicked: mi.activated()
        }
    }

    function activate(index) {
        if (index === 0) resume()
        else if (index === 1) openOptions()
        else quitToMenu()
    }

    Keys.onPressed: (event) => {
        switch (event.key) {
        case Qt.Key_Down:
            currentIndex = (currentIndex + 1) % 3
            break
        case Qt.Key_Up:
            currentIndex = (currentIndex + 2) % 3
            break
        case Qt.Key_Return:
        case Qt.Key_Enter:
            activate(currentIndex)
            break
        case Qt.Key_P:
        case Qt.Key_Escape:
            resume()
            break
        default:
            return
        }
        event.accepted = true
    }

    Rectangle {
        objectName: "dim"
        anchors.fill: parent
        color: Theme.bg
        opacity: 0.85
    }

    Text {
        objectName: "title"
        y: 56
        anchors.horizontalCenter: parent.horizontalCenter
        text: "PAUSED"
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontXL
        color: Theme.text
    }

    Column {
        y: 130
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 12

        MenuItem {
            objectName: "itemResume"
            label: "RESUME"
            selected: root.currentIndex === 0
            onActivated: { root.currentIndex = 0; root.resume() }
        }
        MenuItem {
            objectName: "itemOptions"
            label: "OPTIONS"
            selected: root.currentIndex === 1
            onActivated: { root.currentIndex = 1; root.openOptions() }
        }
        MenuItem {
            objectName: "itemQuit"
            label: "QUIT TO MENU"
            selected: root.currentIndex === 2
            onActivated: { root.currentIndex = 2; root.quitToMenu() }
        }
    }
}
