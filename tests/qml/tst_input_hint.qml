// Acceptance tests for task "Input hint row". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 40

TestCase {
    name: "InputHint"
    when: windowShown

    Component { id: hintC; InputHint {} }

    function col(c) { return String(c).toLowerCase() }
    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }

    function test_size() {
        var h = createTemporaryObject(hintC, stage)
        compare(h.width, 360)
        compare(h.height, 28)
    }
    function test_chevronsLight() {
        var h = createTemporaryObject(hintC, stage, { inputDir: 0 })
        var l = child(h, "chevronLeft"), r = child(h, "chevronRight")
        compare(l.lit, false)
        compare(r.lit, false)
        compare(col(l.color), col(Theme.chevronIdle))
        h.inputDir = -1
        compare(l.lit, true)
        compare(r.lit, false)
        compare(col(l.color), col(Theme.accent))
        h.inputDir = 1
        compare(l.lit, false)
        compare(r.lit, true)
        compare(col(r.color), col(Theme.accent))
    }
    function test_hintText_data() {
        return [
            { tag: "menu",          state: 0, mode: 0, text: "" },
            { tag: "ready",         state: 1, mode: 0, text: "SPACE LAUNCH" },
            { tag: "ready+gun",     state: 1, mode: 2, text: "SPACE LAUNCH" },
            { tag: "playing",       state: 2, mode: 0, text: "" },
            { tag: "playing+long",  state: 2, mode: 1, text: "" },
            { tag: "playing+gun",   state: 2, mode: 2, text: "SPACE FIRE" },
            { tag: "playing+laser", state: 2, mode: 3, text: "SPACE FIRE" },
            { tag: "paused",        state: 3, mode: 2, text: "" },
            { tag: "gameover",      state: 5, mode: 0, text: "" }
        ]
    }
    function test_hintText(data) {
        var h = createTemporaryObject(hintC, stage, { gameState: data.state, paddleMode: data.mode })
        compare(child(h, "hintText").text, data.text)
    }
}
}
