// Acceptance tests for task "Level select screen". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "LevelSelectScreen"
    when: windowShown

    Component { id: screenC; LevelSelectScreen {} }
    Component { id: spyC; SignalSpy {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function make(props) {
        if (props.levelCount === undefined)
            props.levelCount = 10               // most tests use a 10-row list; the component is generic
        var m = createTemporaryObject(screenC, stage, props)
        verify(m !== null)
        m.forceActiveFocus()
        return m
    }
    function spy(target, sig) {
        return createTemporaryObject(spyC, stage, { target: target, signalName: sig })
    }
    function item(m, n) { return child(m, "levelItem" + n) }

    function test_tenItemsLockedByProgress() {
        var m = make({ unlockedLevel: 3 })
        for (var n = 1; n <= 10; ++n) {
            compare(item(m, n).level, n)
            compare(item(m, n).locked, n > 3, "level " + n)
        }
    }
    function test_lockTextOnlyOnLockedItems() {
        var m = make({ unlockedLevel: 2 })
        compare(child(item(m, 2), "lockText").visible, false)
        compare(child(item(m, 3), "lockText").visible, true)
    }
    function test_followsUnlockedLevel() {
        var m = make({ unlockedLevel: 1 })
        compare(item(m, 5).locked, true)
        m.unlockedLevel = 5
        compare(item(m, 5).locked, false)
        compare(item(m, 6).locked, true)
    }
    function test_opensOnHighestUnlocked() {
        var m = make({ unlockedLevel: 3 })
        compare(m.currentIndex, 2)
        compare(item(m, 3).selected, true)
        compare(item(m, 1).selected, false)
    }
    function test_reselectsWhenShownAgain() {
        var m = make({ unlockedLevel: 3 })
        keyClick(Qt.Key_Up)
        compare(m.currentIndex, 1)
        m.visible = false
        m.visible = true
        compare(m.currentIndex, 2)
    }
    function test_upDownClamp() {
        var m = make({ unlockedLevel: 10 })
        compare(m.currentIndex, 9)
        keyClick(Qt.Key_Down)
        compare(m.currentIndex, 9)
        for (var i = 0; i < 12; ++i)
            keyClick(Qt.Key_Up)
        compare(m.currentIndex, 0)
        keyClick(Qt.Key_Up)
        compare(m.currentIndex, 0)
        keyClick(Qt.Key_Down)
        compare(m.currentIndex, 1)
    }
    function test_enterStartsUnlockedLevel_data() {
        return [
            { tag: "return", key: Qt.Key_Return },
            { tag: "enter",  key: Qt.Key_Enter },
            { tag: "space",  key: Qt.Key_Space }
        ]
    }
    function test_enterStartsUnlockedLevel(data) {
        var m = make({ unlockedLevel: 4 })
        var s = spy(m, "levelChosen")
        keyClick(Qt.Key_Up)                      // level 3
        keyClick(data.key)
        compare(s.count, 1)
        compare(s.signalArguments[0][0], 3)
    }
    function test_lockedLevelCannotStart() {
        var m = make({ unlockedLevel: 3 })
        var s = spy(m, "levelChosen")
        keyClick(Qt.Key_Down)                    // level 4, locked
        compare(m.currentIndex, 3)
        keyClick(Qt.Key_Return)
        keyClick(Qt.Key_Space)
        compare(s.count, 0)
    }
    function test_escapeAndBackButton() {
        var m = make({ unlockedLevel: 3 })
        var s = spy(m, "back")
        keyClick(Qt.Key_Escape)
        compare(s.count, 1)
        mouseClick(child(m, "backButton"))
        compare(s.count, 2)
    }
    function test_mouseClick() {
        var m = make({ unlockedLevel: 3 })
        var s = spy(m, "levelChosen")
        mouseClick(item(m, 2))
        compare(s.count, 1)
        compare(s.signalArguments[0][0], 2)
        compare(m.currentIndex, 1)
        mouseClick(item(m, 3))
        compare(s.count, 2)
        mouseClick(item(m, 4))                   // locked: selects, never starts
        compare(s.count, 2)
        compare(m.currentIndex, 3)
    }
    function test_listScrollsAndKeepsSelectionVisible() {
        var m = make({ unlockedLevel: 10 })
        var flick = child(m, "levelFlick")
        verify(flick.interactive)
        verify(flick.contentHeight > flick.height, "10 rows must not fit without scrolling")
        for (var i = 0; i < 9; ++i)
            keyClick(Qt.Key_Up)
        compare(m.currentIndex, 0)
        tryCompare(flick, "contentY", 0)
        for (var j = 0; j < 9; ++j)
            keyClick(Qt.Key_Down)
        compare(m.currentIndex, 9)
        var last = item(m, 10)
        tryVerify(function () {
            var y = last.mapToItem(flick, 0, 0).y
            return y >= -1 && y + last.height <= flick.height + 1
        })
        verify(flick.contentY > 0)
        for (var k = 0; k < 9; ++k)
            keyClick(Qt.Key_Up)
        tryCompare(flick, "contentY", 0)
    }
    function test_twentyLevels() {
        var m = make({ unlockedLevel: 14, levelCount: 20 })
        for (var n = 1; n <= 20; ++n) {
            compare(item(m, n).level, n)
            compare(item(m, n).locked, n > 14, "level " + n)
        }
        compare(m.currentIndex, 13)
        var flick = child(m, "levelFlick")
        tryVerify(function () {
            var y = item(m, 14).mapToItem(flick, 0, 0).y
            return y >= -1 && y + item(m, 14).height <= flick.height + 1
        })
        var s = spy(m, "levelChosen")
        keyClick(Qt.Key_Down)                    // level 15 is locked
        keyClick(Qt.Key_Return)
        compare(s.count, 0)
        for (var i = 0; i < 20; ++i)
            keyClick(Qt.Key_Up)
        compare(m.currentIndex, 0)
        keyClick(Qt.Key_Return)
        compare(s.signalArguments[0][0], 1)
    }
}
}
