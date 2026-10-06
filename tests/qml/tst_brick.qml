// Acceptance tests for task "Brick component". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 100; height: 100

TestCase {
    name: "Brick"
    when: windowShown

    Component { id: brickC; Brick {} }

    function col(c) { return String(c).toLowerCase() }
    function make(props) {
        var b = createTemporaryObject(brickC, stage, props)
        verify(b !== null)
        return b
    }
    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }

    function test_size() {
        var b = make({})
        compare(b.width, 46)
        compare(b.height, 22)
    }
    function test_tier3_amber() {
        var b = make({ hitsLeft: 3 })
        compare(col(child(b, "face").color), col(Theme.brickAmber))
    }
    function test_tier2_solidRed() {
        var b = make({ hitsLeft: 2 })
        compare(col(child(b, "face").color), col(Theme.brickRed))
    }
    function test_tier1_hollowRed() {
        var b = make({ hitsLeft: 1 })
        var face = child(b, "face")
        compare(col(face.color), col(Theme.brickRedDeep))
        compare(col(face.border.color), col(Theme.brickRed))
        compare(face.border.width, 1)
    }
    function test_tierUpdatesLive() {
        var b = make({ hitsLeft: 2 })
        b.hitsLeft = 1
        compare(col(child(b, "face").color), col(Theme.brickRedDeep))
    }
    function test_silver() {
        var b = make({ hitsLeft: 0, unbreakable: true })
        var face = child(b, "face")
        compare(col(face.color), col(Theme.silverFace))
        compare(col(face.border.color), col(Theme.silverHi))
    }
    function test_deadIsHidden() {
        var b = make({ hitsLeft: 1, alive: false })
        compare(b.visible, false)
        b.alive = true
        compare(b.visible, true)
    }
    function test_flashOffWithoutPolish() {
        var b = make({ hitsLeft: 2, polish: false })
        var f = child(b, "hitFlash")
        compare(f.opacity, 0)
        b.flash()
        compare(f.opacity, 0)
        wait(30)
        compare(f.opacity, 0)
    }
    function test_flashWithPolish() {
        var b = make({ hitsLeft: 2, polish: true })
        var f = child(b, "hitFlash")
        compare(f.opacity, 0)
        b.flash()
        verify(f.opacity > 0.5, "flash must start bright")
        tryCompare(f, "opacity", 0, 500)
        compare(child(b, "face").height, 22)   // squash (if any) restores
    }
}
}
