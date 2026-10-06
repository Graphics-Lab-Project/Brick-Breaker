// Acceptance tests for task "Board + playfield". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 336; height: 336

TestCase {
    name: "Playfield"
    when: windowShown

    Component { id: mockC; MockEngine {} }
    Component { id: playfieldC; Playfield {} }

    function col(c) { return String(c).toLowerCase() }
    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function make() {
        var m = createTemporaryObject(mockC, stage)
        m.fillBricks([".......", ".12.S3."])
        var p = createTemporaryObject(playfieldC, stage, { engine: m, polish: false })
        verify(p !== null)
        return { m: m, p: p }
    }

    function test_sizeAndClip() {
        var t = make()
        compare(t.p.width, 336)
        compare(t.p.height, 336)
        compare(t.p.clip, true)
    }
    function test_boardDelegates() {
        var t = make()
        var board = child(t.p, "board")
        compare(child(board, "brickRepeater").count, 98)
        var b = board.brickAt(1, 1)
        verify(b !== null)
        compare(b.x, 49)
        compare(b.y, 25)
        compare(b.hitsLeft, 1)
        compare(b.visible, true)
        compare(board.brickAt(1, 4).unbreakable, true)
        compare(board.brickAt(1, 5).hitsLeft, 3)
        compare(board.brickAt(0, 0).visible, false)
    }
    function test_boardFollowsModel() {
        var t = make()
        var board = child(t.p, "board")
        t.m.bricks.setProperty(8, "hitsLeft", 0)
        t.m.bricks.setProperty(8, "alive", false)
        compare(board.brickAt(1, 1).visible, false)
    }
    function test_boardOffset() {
        var t = make()
        t.m.boardOffsetRows = 2
        compare(child(t.p, "board").y, 48)
    }
    function test_paddle() {
        var t = make()
        var paddle = child(t.p, "paddle")
        compare(paddle.x, 136)
        compare(paddle.y, 322)
        t.m.paddleWidth = 96
        t.m.paddleX = 100
        compare(paddle.width, 96)
        compare(paddle.x, 52)
        t.m.paddleMode = 2
        compare(paddle.paddleMode, 2)
    }
    function test_balls() {
        var t = make()
        var rep = child(t.p, "ballRepeater")
        compare(rep.count, 1)
        compare(rep.itemAt(0).x, 165)
        compare(rep.itemAt(0).y, 316)
        t.m.balls.append({ x: 50, y: 60 })
        compare(rep.count, 2)
        compare(rep.itemAt(1).x, 47)
        t.m.balls.setProperty(0, "x", 200)
        compare(rep.itemAt(0).x, 197)
    }
    function test_ballFastFollowsSpeedState() {
        var t = make()
        var rep = child(t.p, "ballRepeater")
        compare(rep.itemAt(0).fast, false)
        t.m.speedState = 1
        compare(rep.itemAt(0).fast, true)
    }
    function test_capsulesAndProjectiles() {
        var t = make()
        t.m.capsules.append({ type: 3, x: 158, y: 127 })
        t.m.projectiles.append({ kind: 1, x: 137, y: 308 })
        var c = child(t.p, "capsuleRepeater").itemAt(0)
        verify(c !== null)
        compare(c.x, 158)
        compare(c.y, 127)
        compare(c.capsuleType, 3)
        var pr = child(t.p, "projectileRepeater").itemAt(0)
        verify(pr !== null)
        compare(pr.x, 137)
        compare(pr.y, 308)
        compare(pr.kind, 1)
    }
    function test_catchFlash() {
        var t = make()
        var tint = child(t.p, "flashTint")
        var name = child(t.p, "flashName")
        compare(tint.visible, false)
        compare(t.p.flashType, -1)
        t.m.capsuleCaught(2)
        compare(t.p.flashType, 2)
        compare(tint.visible, true)
        compare(col(tint.color), col(Theme.capLong))
        fuzzyCompare(tint.opacity, 0.18, 0.001)
        compare(name.text, "LONG")
        compare(name.visible, true)
        wait(300)
        compare(tint.visible, true)          // holds ~500 ms
        tryCompare(tint, "visible", false, 1000)
        compare(t.p.flashType, -1)
    }
}
}
