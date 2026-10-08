// Acceptance tests for task "Pause overlay + level banner". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 336; height: 336

TestCase {
    name: "Overlays"
    when: windowShown

    Component { id: pauseC; PauseOverlay {} }
    Component { id: bannerC; LevelBanner {} }
    Component { id: spyC; SignalSpy {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function spy(target, sig) {
        return createTemporaryObject(spyC, stage, { target: target, signalName: sig })
    }
    function makePause() {
        var p = createTemporaryObject(pauseC, stage)
        verify(p !== null)
        p.forceActiveFocus()
        return p
    }

    function test_pauseLayout() {
        var p = makePause()
        compare(p.width, 336)
        compare(p.height, 336)
        compare(child(p, "title").text, "PAUSED")
        verify(child(p, "dim").opacity >= 0.8)
    }
    function test_pauseSelection() {
        var p = makePause()
        compare(child(p, "itemResume").selected, true)
        keyClick(Qt.Key_Down)
        compare(p.currentIndex, 1)
        compare(child(p, "itemOptions").selected, true)
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Down)                    // wraps
        compare(p.currentIndex, 0)
        keyClick(Qt.Key_Up)
        compare(p.currentIndex, 2)
    }
    function test_pauseActivate_data() {
        return [
            { tag: "resume",  downs: 0, sig: "resume" },
            { tag: "options", downs: 1, sig: "openOptions" },
            { tag: "quit",    downs: 2, sig: "quitToMenu" }
        ]
    }
    function test_pauseActivate(data) {
        var p = makePause()
        var s = spy(p, data.sig)
        for (var i = 0; i < data.downs; ++i)
            keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Return)
        compare(s.count, 1)
    }
    function test_pauseShortcuts() {
        var p = makePause()
        var s = spy(p, "resume")
        keyClick(Qt.Key_P)
        keyClick(Qt.Key_Escape)
        compare(s.count, 2)
    }
    function test_pauseMouse() {
        var p = makePause()
        var s = spy(p, "quitToMenu")
        mouseClick(child(p, "itemQuit"))
        compare(s.count, 1)
    }

    function test_bannerTexts() {
        var b = createTemporaryObject(bannerC, stage, { level: 1, polish: false })
        compare(child(b, "levelText").text, "LEVEL 01")
        compare(child(b, "clearText").text, "CLEAR")
        compare(child(b, "nextText").text, "NEXT LEVEL 02")
        b.level = 10
        compare(child(b, "nextText").text, "NEXT LEVEL 11")
        b.level = 20
        compare(child(b, "nextText").text, "NEXT LEVEL 01")
    }
    function test_bannerBarWithoutPolish() {
        var b = createTemporaryObject(bannerC, stage, { level: 1, polish: false })
        b.play()
        compare(child(b, "bar").width, 336)
    }
    function test_bannerBarWithPolish() {
        var b = createTemporaryObject(bannerC, stage, { level: 1, polish: true })
        b.play()
        var bar = child(b, "bar")
        verify(bar.width < 336, "bar must grow from 0")
        tryCompare(bar, "width", 336, 1500)
    }
}
}
