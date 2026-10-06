// Acceptance tests for task "Main menu screen". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "MenuScreen"
    when: windowShown

    Component { id: menuC; MenuScreen {} }
    Component { id: spyC; SignalSpy {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function make(props) {
        var m = createTemporaryObject(menuC, stage, props)
        verify(m !== null)
        m.forceActiveFocus()
        return m
    }
    function spy(target, sig) {
        return createTemporaryObject(spyC, stage, { target: target, signalName: sig })
    }

    function test_bestText() {
        var m = make({ bestScore: 830 })
        compare(child(m, "bestText").text, "BEST 00830")
    }
    function test_selectionFollowsIndex() {
        var m = make({})
        compare(m.currentIndex, 0)
        compare(child(m, "itemStart").selected, true)
        compare(child(m, "itemOptions").selected, false)
        keyClick(Qt.Key_Down)
        compare(m.currentIndex, 1)
        compare(child(m, "itemStart").selected, false)
        compare(child(m, "itemOptions").selected, true)
    }
    function test_wraps() {
        var m = make({})
        keyClick(Qt.Key_Up)
        compare(m.currentIndex, 3)
        compare(child(m, "itemQuit").selected, true)
        keyClick(Qt.Key_Down)
        compare(m.currentIndex, 0)
    }
    function test_enterActivates_data() {
        return [
            { tag: "start",   downs: 0, sig: "startGame" },
            { tag: "options", downs: 1, sig: "openOptions" },
            { tag: "help",    downs: 2, sig: "openHelp" },
            { tag: "quit",    downs: 3, sig: "quit" }
        ]
    }
    function test_enterActivates(data) {
        var m = make({})
        var s = spy(m, data.sig)
        for (var i = 0; i < data.downs; ++i)
            keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)
        compare(s.count, 1)
    }
    function test_spaceAndEnterKeys() {
        var m = make({})
        var s = spy(m, "startGame")
        keyClick(Qt.Key_Enter)
        keyClick(Qt.Key_Space)
        compare(s.count, 2)
    }
    function test_mouseClick() {
        var m = make({})
        var s = spy(m, "openHelp")
        mouseClick(child(m, "itemHelp"))
        compare(s.count, 1)
    }
}
}
