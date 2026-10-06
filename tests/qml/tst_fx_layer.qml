// Acceptance tests for task "T1 effects layer". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 336; height: 336

TestCase {
    name: "FxLayer"
    when: windowShown

    Component { id: mockC; MockEngine {} }
    Component { id: fxC; FxLayer {} }

    function make(polish) {
        var m = createTemporaryObject(mockC, stage)
        var fx = createTemporaryObject(fxC, stage, { engine: m, polish: polish })
        verify(fx !== null)
        return { m: m, fx: fx }
    }

    function test_size() {
        var t = make(true)
        compare(t.fx.width, 336)
        compare(t.fx.height, 336)
        compare(t.fx.activeEffects, 0)
    }
    function test_nothingWithoutPolish() {
        var t = make(false)
        t.m.brickBroken(2, 3, 2)
        t.m.capsuleCaught(2)
        t.m.projectileFired(0, 167, 312)
        t.m.multiBallActivated()
        compare(t.fx.activeEffects, 0)
        compare(findChild(t.fx, "breakFx"), null)
    }
    function test_breakFx() {
        var t = make(true)
        t.m.brickBroken(2, 3, 2)
        verify(t.fx.activeEffects >= 1)
        var b = findChild(t.fx, "breakFx")
        verify(b !== null, "breakFx item expected")
        compare(b.x, 145)          // col 3 * 48 + 1
        compare(b.y, 49)           // row 2 * 24 + 1
        tryCompare(t.fx, "activeEffects", 0, 1500)
    }
    function test_breakFxRespectsOffset() {
        var t = make(true)
        t.m.boardOffsetRows = 2
        t.m.brickBroken(0, 0, 1)
        var b = findChild(t.fx, "breakFx")
        verify(b !== null)
        compare(b.x, 1)
        compare(b.y, 49)           // (0 + 2) * 24 + 1
    }
    function test_scorePop() {
        var t = make(true)
        t.m.capsuleCaught(3)
        var p = findChild(t.fx, "scorePop")
        verify(p !== null, "scorePop expected")
        compare(p.text, "+50")
        tryCompare(t.fx, "activeEffects", 0, 1500)
    }
    function test_muzzleAndSplit() {
        var t = make(true)
        t.m.projectileFired(0, 167, 312)
        verify(findChild(t.fx, "muzzle") !== null)
        t.m.multiBallActivated()
        verify(findChild(t.fx, "splitFlash") !== null)
        verify(t.fx.activeEffects >= 2)
        tryCompare(t.fx, "activeEffects", 0, 1500)
    }
}
}
