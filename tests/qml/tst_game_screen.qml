// Acceptance tests for task "Game screen". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "GameScreen"
    when: windowShown

    Component { id: mockC; MockEngine {} }
    Component { id: screenC; GameScreen {} }
    Component { id: spyC; SignalSpy {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function make(state) {
        var m = createTemporaryObject(mockC, stage, { gameState: state })
        m.fillBricks([".11.11."])
        var s = createTemporaryObject(screenC, stage, { engine: m, polish: false })
        verify(s !== null)
        s.forceActiveFocus()
        return { m: m, s: s }
    }

    function test_layout() {
        var t = make(1)
        var pf = child(t.s, "playfield")
        compare(pf.x, 12)
        compare(pf.y, 104)
        compare(child(t.s, "hud").x, 8)
        compare(child(t.s, "hud").y, 8)
        compare(child(t.s, "inputHint").y, 448)
        verify(findChild(t.s, "fxLayer") !== null)
        verify(findChild(t.s, "pauseButton") !== null)
    }
    function test_hudBound() {
        var t = make(2)
        t.m.score = 650
        t.m.lives = 2
        compare(child(t.s, "scoreText").text, "00650")
        compare(child(t.s, "livesText").text, "2")
    }
    function test_arrowKeys() {
        var t = make(2)
        keyPress(Qt.Key_Left)
        verify(t.m.called("moveLeft:true"))
        keyRelease(Qt.Key_Left)
        verify(t.m.called("moveLeft:false"))
        keyPress(Qt.Key_Right)
        keyRelease(Qt.Key_Right)
        verify(t.m.called("moveRight:true"))
        verify(t.m.called("moveRight:false"))
    }
    function test_spaceLaunches() {
        var t = make(1)
        keyClick(Qt.Key_Space)
        verify(t.m.called("launchOrFire"))
    }
    function test_pKeyPauses() {
        var t = make(2)
        keyClick(Qt.Key_P)
        verify(t.m.called("togglePause"))
    }
    function test_pauseButton() {
        var t = make(2)
        mouseClick(child(t.s, "pauseButton"))
        verify(t.m.called("togglePause"))
    }
    function test_pointer() {
        var t = make(2)
        var area = child(t.s, "pointerArea")
        mouseMove(area, 90, 50)
        mouseMove(area, 100, 50)
        verify(t.m.called("setPointerX:100"), JSON.stringify(t.m.calls))
        mouseClick(area, 100, 50)
        verify(t.m.called("launchOrFire"))
    }
    function test_overlayVisibility() {
        var t = make(2)
        compare(child(t.s, "pauseOverlay").visible, false)
        compare(child(t.s, "levelBanner").visible, false)
        compare(child(t.s, "gameOverOverlay").visible, false)
        t.m.gameState = 3
        compare(child(t.s, "pauseOverlay").visible, true)
        t.m.gameState = 4
        compare(child(t.s, "pauseOverlay").visible, false)
        compare(child(t.s, "levelBanner").visible, true)
        t.m.gameState = 5
        compare(child(t.s, "gameOverOverlay").visible, true)
    }
    function test_pauseOverlayHasFocusAndActs() {
        var t = make(3)
        var overlay = child(t.s, "pauseOverlay")
        verify(overlay.activeFocus, "pause overlay must have focus while paused")
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)                 // QUIT TO MENU
        verify(t.m.called("quitToMenu"))
    }
    function test_pauseOverlayResumeAndOptions() {
        var t = make(3)
        var opt = createTemporaryObject(spyC, stage, { target: t.s, signalName: "optionsRequested" })
        keyClick(Qt.Key_Return)                 // RESUME
        verify(t.m.called("togglePause"))
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)                 // OPTIONS
        compare(opt.count, 1)
    }
    function test_gameOverWiring() {
        var t = make(5)
        t.m.highScorePending = false
        mouseClick(child(t.s, "btnPlayAgain"))
        verify(t.m.called("startGame"))
        mouseClick(child(t.s, "btnMenu"))
        verify(t.m.called("quitToMenu"))
    }
    function test_gameOverInitials() {
        var t = make(5)
        t.m.highScorePending = true
        var overlay = child(t.s, "gameOverOverlay")
        compare(overlay.newBest, true)
        t.s.forceActiveFocus()
        keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Return)
        verify(t.m.called("submitInitials:BAA"), JSON.stringify(t.m.calls))
    }
}
}
