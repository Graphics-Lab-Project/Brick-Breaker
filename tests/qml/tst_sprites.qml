// Acceptance tests for task "Ball, capsule, projectile sprites". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 336; height: 336

TestCase {
    name: "Sprites"
    when: windowShown

    Component { id: ballC; Ball {} }
    Component { id: capsuleC; Capsule {} }
    Component { id: projectileC; Projectile {} }

    function col(c) { return String(c).toLowerCase() }
    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }

    function test_ballGeometry() {
        var b = createTemporaryObject(ballC, stage, { centerX: 168, centerY: 319, polish: false })
        compare(b.x, 165)
        compare(b.y, 316)
        compare(b.width, 6)
        compare(b.height, 6)
        compare(col(child(b, "core").color), col(Theme.ball))
    }
    function test_ballRounds() {
        var b = createTemporaryObject(ballC, stage, { centerX: 100.7, centerY: 50.2, polish: false })
        compare(b.x, 98)
        compare(b.y, 47)
    }
    function test_trailHiddenWhenSlowOrNoPolish() {
        var b = createTemporaryObject(ballC, stage, { fast: false, polish: true })
        compare(child(b, "ghost0").visible, false)
        b.fast = true
        b.polish = false
        compare(child(b, "ghost0").visible, false)
        compare(child(b, "ghost2").visible, false)
    }
    function test_trailWhenFast() {
        var b = createTemporaryObject(ballC, stage, { fast: true, polish: true })
        compare(child(b, "ghost0").visible, true)
        compare(child(b, "ghost1").visible, true)
        compare(child(b, "ghost2").visible, true)
        fuzzyCompare(child(b, "ghost0").opacity, 0.55, 0.001)
        fuzzyCompare(child(b, "ghost1").opacity, 0.30, 0.001)
        fuzzyCompare(child(b, "ghost2").opacity, 0.15, 0.001)
    }

    function test_capsule_data() {
        return [
            { tag: "life",  type: 0, name: "LIFE",  color: Theme.capLife },
            { tag: "multi", type: 1, name: "MULTI", color: Theme.capMulti },
            { tag: "long",  type: 2, name: "LONG",  color: Theme.capLong },
            { tag: "gun",   type: 3, name: "GUN",   color: Theme.capGun },
            { tag: "laser", type: 4, name: "LASER", color: Theme.capLaser }
        ]
    }
    function test_capsule(data) {
        var c = createTemporaryObject(capsuleC, stage, { capsuleType: data.type, polish: false })
        compare(c.width, 20)
        compare(c.height, 10)
        compare(col(child(c, "body").color), col(data.color))
        var label = child(c, "label")
        compare(label.text, data.name)
        compare(col(label.color), col(data.color))
        compare(label.x, 23)
        compare(label.font.pixelSize, 8)
    }
    function test_capsulePopInSettles() {
        var c = createTemporaryObject(capsuleC, stage, { capsuleType: 2, polish: true })
        tryCompare(c, "scale", 1, 500)
    }

    function test_bullet() {
        var p = createTemporaryObject(projectileC, stage, { kind: 0 })
        compare(p.width, 2)
        compare(p.height, 6)
        compare(col(child(p, "body").color), col(Theme.capGun))
    }
    function test_laserBolt() {
        var p = createTemporaryObject(projectileC, stage, { kind: 1 })
        compare(p.width, 2)
        compare(p.height, 10)
        compare(col(child(p, "body").color), col(Theme.capLaser))
    }
}
}
