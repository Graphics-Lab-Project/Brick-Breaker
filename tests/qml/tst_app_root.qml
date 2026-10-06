// Acceptance tests for task "App root + main window". READ-ONLY for agents (AGENTS.md R4).
// Uses the REAL GameEngine (needs the engine tasks merged).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "AppRoot"
    when: windowShown

    Component { id: engineC; GameEngine {} }
    Component { id: rootC; AppRoot {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function make() {
        var e = createTemporaryObject(engineC, stage)
        verify(e !== null)
        var r = createTemporaryObject(rootC, stage, { engine: e })
        verify(r !== null)
        r.forceActiveFocus()
        return { e: e, r: r }
    }

    function test_startsOnMenu() {
        var t = make()
        compare(child(t.r, "menuScreen").visible, true)
        compare(child(t.r, "gameScreen").visible, false)
        compare(child(t.r, "optionsScreen").visible, false)
        compare(child(t.r, "helpScreen").visible, false)
        verify(child(t.r, "menuScreen").activeFocus)
    }
    function test_startGame() {
        var t = make()
        keyClick(Qt.Key_Return)
        compare(t.e.gameState, 1)
        compare(child(t.r, "gameScreen").visible, true)
        compare(child(t.r, "menuScreen").visible, false)
        verify(child(t.r, "gameScreen").activeFocus)
        keyClick(Qt.Key_Space)
        compare(t.e.gameState, 2)
    }
    function test_helpAndBack() {
        var t = make()
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)
        compare(child(t.r, "helpScreen").visible, true)
        compare(child(t.r, "menuScreen").visible, false)
        keyClick(Qt.Key_Escape)
        compare(child(t.r, "menuScreen").visible, true)
    }
    function test_optionsApplyToEngine() {
        var t = make()
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)
        var opts = child(t.r, "optionsScreen")
        compare(opts.visible, true)
        compare(opts.paddleSpeed, 3)
        keyClick(Qt.Key_Right)
        compare(t.e.paddleSpeed, 4)
        compare(opts.paddleSpeed, 4)
        keyClick(Qt.Key_Escape)
        compare(child(t.r, "menuScreen").visible, true)
    }
    function test_optionsFromPause() {
        var t = make()
        keyClick(Qt.Key_Return)                 // start -> Ready
        keyClick(Qt.Key_P)                      // pause
        compare(t.e.gameState, 3)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)                 // OPTIONS
        compare(child(t.r, "optionsScreen").visible, true)
        keyClick(Qt.Key_Escape)
        compare(child(t.r, "optionsScreen").visible, false)
        compare(child(t.r, "gameScreen").visible, true)
        compare(t.e.gameState, 3)               // still paused
    }
    function test_quitToMenuFromPause() {
        var t = make()
        keyClick(Qt.Key_Return)
        keyClick(Qt.Key_P)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)                 // QUIT TO MENU
        compare(t.e.gameState, 0)
        compare(child(t.r, "menuScreen").visible, true)
        verify(child(t.r, "menuScreen").activeFocus)
    }
}
}
