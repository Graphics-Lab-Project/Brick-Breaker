// Acceptance tests for task "HUD + pause button". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 100

TestCase {
    name: "Hud"
    when: windowShown

    Component { id: hudC; Hud {} }
    Component { id: buttonC; PauseButton {} }
    Component { id: spyC; SignalSpy {} }

    function col(c) { return String(c).toLowerCase() }
    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }

    function test_size() {
        var h = createTemporaryObject(hudC, stage)
        compare(h.width, 344)
        compare(h.height, 60)
        verify(findChild(h, "leftPanel") !== null)
        verify(findChild(h, "rightPanel") !== null)
    }
    function test_texts() {
        var h = createTemporaryObject(hudC, stage, { score: 650, bestScore: 830, level: 1, gunAmmo: 2, lives: 3, polish: false })
        compare(child(h, "scoreText").text, "00650")
        compare(child(h, "bestText").text, "00830")
        compare(child(h, "levelText").text, "LV 01")
        compare(child(h, "ammoText").text, "2")
        compare(child(h, "livesText").text, "3")
    }
    function test_bindingsUpdate() {
        var h = createTemporaryObject(hudC, stage, { polish: false })
        h.score = 123456
        h.level = 12
        h.lives = 5
        compare(child(h, "scoreText").text, "123456")
        compare(child(h, "levelText").text, "LV 12")
        compare(child(h, "livesText").text, "5")
    }
    function test_ammoIcons() {
        var h = createTemporaryObject(hudC, stage, { gunAmmo: 2, polish: false })
        compare(col(child(h, "ammo0").color), col(Theme.brickAmber))
        compare(col(child(h, "ammo1").color), col(Theme.brickAmber))
        compare(col(child(h, "ammo2").color), col(Theme.ammoEmpty))
        h.gunAmmo = 0
        compare(col(child(h, "ammo0").color), col(Theme.ammoEmpty))
    }
    function test_bestIsAmber() {
        var h = createTemporaryObject(hudC, stage, { polish: false })
        compare(col(child(h, "bestText").color), col(Theme.brickAmber))
    }
    function test_lifeEffectsNoopWithoutPolish() {
        var h = createTemporaryObject(hudC, stage, { polish: false })
        h.flashLifeLost()
        h.pulseLifeGained()
        compare(col(child(h, "livesText").color), col(Theme.text))
        compare(child(h, "livesText").scale, 1)
    }
    function test_lifeEffectsSettle() {
        var h = createTemporaryObject(hudC, stage, { polish: true })
        h.flashLifeLost()
        var t = child(h, "livesText")
        tryVerify(function() { return col(t.color) === col(Theme.text) }, 1000)
        h.pulseLifeGained()
        tryCompare(child(h, "livesText"), "scale", 1, 1000)
    }
    function test_pauseButton() {
        var b = createTemporaryObject(buttonC, stage)
        compare(b.width, 24)
        compare(b.height, 20)
        verify(findChild(b, "bar0") !== null)
        verify(findChild(b, "bar1") !== null)
        var spy = createTemporaryObject(spyC, stage, { target: b, signalName: "clicked" })
        mouseClick(b)
        compare(spy.count, 1)
    }
}
}
