// OWNER: task "Level select screen". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: tasks/L2.md + docs/LEVEL_SELECT.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 360 x 480. Required children: "levelFlick" (Flickable), "levelItem1".."levelItem10" (each with
// `int level`, `bool locked`, `bool selected`; locked items also show a Text "lockText"; unlocked items with a
// recorded best show Texts "bestText" and "timeText"), "backButton".
FocusScope {
    id: root
    property int unlockedLevel: 1       // highest playable level
    property int levelCount: 20
    // Personal bests, index 0 = level 1: [{ bestScore, bestTimeMicros, clears }, ...] (engine.levelBests).
    // Missing / shorter list / clears === 0 -> no best shown for that level.
    property var bests: []
    property int currentIndex: 0        // 0-based; Component.onCompleted / on becoming visible -> unlockedLevel - 1

    signal levelChosen(int level)       // 1-based, only ever emitted for unlocked levels
    signal back()

    width: Theme.canvasW
    height: Theme.canvasH

    readonly property int rowH: 40
    readonly property int rowGap: 6

    // microseconds -> "M:SS.uuuuuu" (minutes unbounded), e.g. 42318457 -> "0:42.318457"
    function formatTime(us) { return "" }

    function activate(i) {
        if (i >= 0 && i < levelCount && i + 1 <= unlockedLevel)
            levelChosen(i + 1)
    }
    function ensureVisible() {
        var top = currentIndex * (rowH + rowGap)
        var y = levelFlick.contentY
        if (top < y) y = top
        if (top + rowH > y + levelFlick.height) y = top + rowH - levelFlick.height
        levelFlick.contentY = Math.max(0, Math.min(y, levelFlick.contentHeight - levelFlick.height))
    }
    function selectHighest() {
        currentIndex = Math.max(0, Math.min(levelCount - 1, unlockedLevel - 1))
        ensureVisible()
    }

    Component.onCompleted: selectHighest()
    onVisibleChanged: if (visible) selectHighest()

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
    }

    Text {
        x: 0; y: 24; width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: "SELECT LEVEL"
        color: Theme.accent
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontL
    }

    Flickable {
        id: levelFlick
        objectName: "levelFlick"
        x: 32; y: 70
        width: 296; height: 350
        interactive: true
        clip: true
        contentWidth: width
        contentHeight: levelColumn.height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: levelColumn
            spacing: root.rowGap
            Repeater {
                model: root.levelCount
                Rectangle {
                    id: row
                    required property int index
                    objectName: "levelItem" + level
                    property int level: index + 1
                    property bool locked: level > root.unlockedLevel
                    property bool selected: index === root.currentIndex
                    width: 296
                    height: root.rowH
                    color: "transparent"
                    border.width: 1
                    border.color: selected ? Theme.accent : Theme.lineFaint
                    Text {
                        x: 12
                        anchors.verticalCenter: parent.verticalCenter
                        text: "LEVEL " + Theme.pad(row.level, 2)
                        color: row.selected ? Theme.accent : (row.locked ? Theme.textDim : Theme.text)
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontM
                    }
                    Text {
                        objectName: "lockText"
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        visible: row.locked
                        text: "LOCKED"
                        color: Theme.textDim
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontS
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.currentIndex = row.index
                            root.ensureVisible()
                            root.activate(row.index)
                        }
                    }
                }
            }
        }
    }

    Item {
        objectName: "backButton"
        x: 32; y: 436; width: 60; height: 28
        Text {
            anchors.centerIn: parent
            text: "BACK"
            color: Theme.textDim
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontS
        }
        MouseArea {
            anchors.fill: parent
            onClicked: root.back()
        }
    }

    Text {
        anchors.right: parent.right
        anchors.rightMargin: 32
        y: 442
        text: "ESC BACK"
        color: Theme.lineFaint
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontS
    }

    Keys.onPressed: (event) => {
        event.accepted = true
        switch (event.key) {
        case Qt.Key_Up:
            currentIndex = Math.max(0, currentIndex - 1)
            ensureVisible()
            break
        case Qt.Key_Down:
            currentIndex = Math.min(levelCount - 1, currentIndex + 1)
            ensureVisible()
            break
        case Qt.Key_Return:
        case Qt.Key_Enter:
        case Qt.Key_Space:
            activate(currentIndex)
            break
        case Qt.Key_Escape:
            back()
            break
        default:
            event.accepted = false
        }
    }
}
