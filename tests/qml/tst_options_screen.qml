// Acceptance tests for task "Options screen". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "OptionsScreen"
    when: windowShown

    Component { id: optionsC; OptionsScreen {} }
    Component { id: spyC; SignalSpy {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function make(props) {
        var o = createTemporaryObject(optionsC, stage, props)
        verify(o !== null)
        o.forceActiveFocus()
        return o
    }
    function spy(target, sig) {
        return createTemporaryObject(spyC, stage, { target: target, signalName: sig })
    }

    function test_speedDisplay() {
        var o = make({ paddleSpeed: 3 })
        compare(child(o, "speedValue").text, "3")
        compare(child(o, "seg0").filled, true)
        compare(child(o, "seg2").filled, true)
        compare(child(o, "seg3").filled, false)
        compare(child(o, "seg4").filled, false)
        o.paddleSpeed = 5
        compare(child(o, "seg4").filled, true)
    }
    function test_accelDisplay() {
        var o = make({ acceleration: true })
        compare(child(o, "accelOn").selected, true)
        compare(child(o, "accelOff").selected, false)
        o.acceleration = false
        compare(child(o, "accelOn").selected, false)
        compare(child(o, "accelOff").selected, true)
    }
    function test_rows() {
        var o = make({})
        compare(o.currentRow, 0)
        compare(child(o, "rowSpeed").current, true)
        keyClick(Qt.Key_Down)
        compare(o.currentRow, 1)
        compare(child(o, "rowAccel").current, true)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Down)                 // clamped, no wrap
        compare(o.currentRow, 2)
        compare(child(o, "rowBack").current, true)
        keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Up)
        compare(o.currentRow, 0)
    }
    function test_speedLeftRight() {
        var o = make({ paddleSpeed: 3 })
        var s = spy(o, "paddleSpeedRequested")
        keyClick(Qt.Key_Right)
        compare(s.count, 1)
        compare(s.signalArguments[0][0], 4)
        keyClick(Qt.Key_Left)
        compare(s.signalArguments[1][0], 2)
    }
    function test_speedClampedNoSignal() {
        var o = make({ paddleSpeed: 5 })
        var s = spy(o, "paddleSpeedRequested")
        keyClick(Qt.Key_Right)
        compare(s.count, 0)
        o.paddleSpeed = 1
        keyClick(Qt.Key_Left)
        compare(s.count, 0)
    }
    function test_accelKeys() {
        var o = make({ acceleration: false })
        var s = spy(o, "accelerationRequested")
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Right)                // already OFF -> nothing
        compare(s.count, 0)
        keyClick(Qt.Key_Left)                 // ON
        compare(s.count, 1)
        compare(s.signalArguments[0][0], true)
        keyClick(Qt.Key_Return)               // toggle
        compare(s.count, 2)
        compare(s.signalArguments[1][0], true)  // property not updated by parent yet -> still requests ON
    }
    function test_backRowAndEscape() {
        var o = make({})
        var s = spy(o, "back")
        keyClick(Qt.Key_Escape)
        compare(s.count, 1)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)
        compare(s.count, 2)
    }
    function test_mouseAccel() {
        var o = make({ acceleration: false })
        var s = spy(o, "accelerationRequested")
        mouseClick(child(o, "accelOn"))
        compare(s.count, 1)
        compare(s.signalArguments[0][0], true)
    }
}
}
