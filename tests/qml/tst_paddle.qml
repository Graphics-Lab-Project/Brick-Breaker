// Acceptance tests for task "Paddle component". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 336; height: 336

TestCase {
    name: "Paddle"
    when: windowShown

    Component { id: paddleC; Paddle {} }

    function make(props) {
        var p = createTemporaryObject(paddleC, stage, props)
        verify(p !== null)
        return p
    }
    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }

    function test_geometry() {
        var p = make({ paddleX: 100, paddleWidth: 64, polish: false })
        compare(p.x, 68)
        compare(p.y, 322)
        compare(p.width, 64)
        compare(p.height, 6)
        compare(String(child(p, "body").color).toLowerCase(), String(Theme.paddle).toLowerCase())
    }
    function test_xIsRounded() {
        var p = make({ paddleX: 100.6, paddleWidth: 64, polish: false })
        compare(p.x, 69)
    }
    function test_longInstantWithoutPolish() {
        var p = make({ paddleX: 168, paddleWidth: 64, polish: false })
        p.paddleWidth = 96
        compare(p.width, 96)
        compare(p.x, 120)
    }
    function test_longTweensWithPolish() {
        var p = make({ paddleX: 168, paddleWidth: 64, polish: true })
        p.paddleWidth = 96
        tryCompare(p, "width", 96, 600)
        tryCompare(p, "x", 120, 600)
    }
    function test_partsByMode() {
        var p = make({ paddleMode: 0, polish: false })
        compare(child(p, "turret").visible, false)
        compare(child(p, "nubLeft").visible, false)
        compare(child(p, "nubRight").visible, false)
        p.paddleMode = 2      // Gun
        compare(child(p, "turret").visible, true)
        compare(child(p, "nubLeft").visible, false)
        p.paddleMode = 3      // Laser
        compare(child(p, "turret").visible, false)
        compare(child(p, "nubLeft").visible, true)
        compare(child(p, "nubRight").visible, true)
        p.paddleMode = 1      // Long
        compare(child(p, "turret").visible, false)
        compare(child(p, "nubRight").visible, false)
    }
    function test_partsSitAbovePaddle() {
        var p = make({ paddleMode: 2, polish: false })
        var t = child(p, "turret")
        compare(t.y, -4)
        compare(t.height, 4)
        compare(t.x + t.width / 2, p.width / 2)
    }
    function test_blinkNoopWithoutPolish() {
        var p = make({ polish: false })
        p.blink()
        compare(p.opacity, 1)
        wait(100)
        compare(p.opacity, 1)
    }
    function test_blinkWithPolish() {
        var p = make({ polish: true })
        p.blink()
        tryCompare(p, "opacity", 0, 200)
        tryCompare(p, "opacity", 1, 600)
    }
}
}
