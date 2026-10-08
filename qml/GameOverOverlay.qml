import QtQuick
import BrickBreaker

// Covers the playfield, 336 x 336. Required children: "titleText", "scoreText", "newBestText",
// "initialsEntry" (InitialsEntry), "hsInitials0..4", "hsScore0..4", "hsLevel0..4",
// "btnPlayAgain", "btnMenu".
FocusScope {
    id: root
    property int score: 0
    property bool newBest: false
    property var entries: []            // [{initials, score, level}] like GameEngine.highScores
    property int highlightRank: -1
    property bool polish: Theme.polish

    signal initialsSubmitted(string initials)
    signal playAgain()
    signal menu()

    width: Theme.fieldW
    height: Theme.fieldH

    opacity: 0.9
    NumberAnimation on opacity {
        running: root.polish
        from: 0; to: 0.9; duration: 400
        easing.type: Easing.InOutQuad
    }

    focus: true
    Keys.onReturnPressed: if (!root.newBest) root.playAgain()
    Keys.onEnterPressed: if (!root.newBest) root.playAgain()
    Keys.onEscapePressed: if (!root.newBest) root.menu()

    function entryAt(i) {
        return (entries && i < entries.length) ? entries[i] : null
    }

    component Btn: Rectangle {
        id: btn
        property alias label: lbl.text
        signal clicked()
        width: 96
        height: 24
        color: "transparent"
        border.width: 1
        border.color: Theme.accent
        Text {
            id: lbl
            anchors.centerIn: parent
            color: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontS
        }
        MouseArea {
            anchors.fill: parent
            onClicked: btn.clicked()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
    }

    Text {
        objectName: "titleText"
        x: 0; y: 16; width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: "GAME OVER"
        color: Theme.brickRed
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontL
    }

    Text {
        objectName: "scoreText"
        x: 0; y: 52; width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: Theme.pad(root.score, 5)
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontM
    }

    Text {
        objectName: "newBestText"
        x: 0; y: 76; width: parent.width
        horizontalAlignment: Text.AlignHCenter
        visible: root.newBest
        text: "NEW BEST"
        color: Theme.brickAmber
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontS
    }

    InitialsEntry {
        objectName: "initialsEntry"
        x: (parent.width - width) / 2
        y: 92
        visible: root.newBest
        focus: root.newBest
        onSubmitted: (initials) => root.initialsSubmitted(initials)
    }

    Column {
        x: 48; y: 128
        width: parent.width - 96
        spacing: 4
        Repeater {
            model: 5
            Item {
                id: row
                required property int index
                readonly property var e: root.entryAt(index)
                width: parent.width
                height: 16
                readonly property color col: root.highlightRank === index ? Theme.accent : Theme.text
                Text {
                    x: 0
                    text: Theme.pad(row.index + 1, 1)
                    color: Theme.textDim
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontS
                }
                Text {
                    objectName: "hsInitials" + row.index
                    x: 16
                    text: row.e ? row.e.initials : "---"
                    color: row.col
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontS
                }
                Text {
                    objectName: "hsScore" + row.index
                    x: 64
                    text: row.e ? Theme.pad(row.e.score, 5) : "-----"
                    color: row.col
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontS
                }
                Text {
                    objectName: "hsLevel" + row.index
                    x: 128
                    text: row.e ? Theme.pad(row.e.level, 2) : "--"
                    color: row.col
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontS
                }
            }
        }
    }

    Btn {
        objectName: "btnPlayAgain"
        x: 32; y: 296
        label: "PLAY AGAIN"
        onClicked: root.playAgain()
    }
    Btn {
        objectName: "btnMenu"
        x: parent.width - width - 32; y: 296
        label: "MENU"
        onClicked: root.menu()
    }
}
