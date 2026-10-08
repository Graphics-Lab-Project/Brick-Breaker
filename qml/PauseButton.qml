import QtQuick
import BrickBreaker

// 24 x 20 pause button. Required children: "bar0", "bar1".
Item {
    id: root
    signal clicked()
    width: 24
    height: 20

    Rectangle {
        objectName: "bar0"
        x: 6; y: 3
        width: 4; height: 14
        color: Theme.text
    }
    Rectangle {
        objectName: "bar1"
        x: 14; y: 3
        width: 4; height: 14
        color: Theme.text
    }
    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
