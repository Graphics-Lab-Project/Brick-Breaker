// Acceptance tests for task "Game over + initials". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 336; height: 336

TestCase {
    name: "GameOver"
    when: windowShown

    Component { id: initialsC; InitialsEntry {} }
    Component { id: overC; GameOverOverlay {} }
    Component { id: spyC; SignalSpy {} }

    readonly property var sampleEntries: [
        { initials: "BNY", score: 1240, level: 5 },
        { initials: "RIM", score: 830, level: 3 },
        { initials: "JUI", score: 610, level: 2 }
    ]

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function spy(target, sig) {
        return createTemporaryObject(spyC, stage, { target: target, signalName: sig })
    }

    function test_initialsDefaults() {
        var i = createTemporaryObject(initialsC, stage)
        compare(i.initials, "AAA")
        compare(i.cursor, 0)
        compare(child(i, "slot0").active, true)
        compare(child(i, "slot1").active, false)
    }
    function test_initialsLetters() {
        var i = createTemporaryObject(initialsC, stage)
        i.forceActiveFocus()
        keyClick(Qt.Key_Up)
        compare(i.initials, "BAA")
        keyClick(Qt.Key_Down)
        keyClick(Qt.Key_Down)
        compare(i.initials, "ZAA")             // wraps A -> Z
        keyClick(Qt.Key_Up)
        compare(i.initials, "AAA")             // wraps Z -> A
    }
    function test_initialsCursor() {
        var i = createTemporaryObject(initialsC, stage)
        i.forceActiveFocus()
        keyClick(Qt.Key_Left)
        compare(i.cursor, 0)
        keyClick(Qt.Key_Right)
        keyClick(Qt.Key_Up)
        compare(i.initials, "ABA")
        keyClick(Qt.Key_Right)
        keyClick(Qt.Key_Right)
        compare(i.cursor, 2)
        compare(child(i, "slot2").active, true)
    }
    function test_initialsSubmit() {
        var i = createTemporaryObject(initialsC, stage)
        i.forceActiveFocus()
        var s = spy(i, "submitted")
        keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Return)
        compare(s.count, 1)
        compare(s.signalArguments[0][0], "BAA")
    }

    function test_overlayTexts() {
        var o = createTemporaryObject(overC, stage, { score: 1240, newBest: false, entries: sampleEntries, polish: false })
        compare(child(o, "titleText").text, "GAME OVER")
        compare(child(o, "scoreText").text, "01240")
        compare(child(o, "newBestText").visible, false)
        compare(child(o, "initialsEntry").visible, false)
    }
    function test_tableRows() {
        var o = createTemporaryObject(overC, stage, { entries: sampleEntries, polish: false })
        compare(child(o, "hsInitials0").text, "BNY")
        compare(child(o, "hsScore0").text, "01240")
        compare(child(o, "hsLevel0").text, "05")
        compare(child(o, "hsInitials2").text, "JUI")
        compare(child(o, "hsInitials3").text, "---")
        compare(child(o, "hsScore4").text, "-----")
        compare(child(o, "hsLevel4").text, "--")
    }
    function test_newBestShowsEntryAndForwards() {
        var o = createTemporaryObject(overC, stage, { score: 1240, newBest: true, entries: [], polish: false })
        compare(child(o, "newBestText").visible, true)
        var entry = child(o, "initialsEntry")
        compare(entry.visible, true)
        o.forceActiveFocus()
        verify(entry.activeFocus, "initials entry must get focus when newBest")
        var s = spy(o, "initialsSubmitted")
        keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Return)
        compare(s.count, 1)
        compare(s.signalArguments[0][0], "CAA")
    }
    function test_buttons() {
        var o = createTemporaryObject(overC, stage, { newBest: false, polish: false })
        var again = spy(o, "playAgain")
        var menu = spy(o, "menu")
        mouseClick(child(o, "btnPlayAgain"))
        mouseClick(child(o, "btnMenu"))
        compare(again.count, 1)
        compare(menu.count, 1)
    }
    function test_keysWhenNoNewBest() {
        var o = createTemporaryObject(overC, stage, { newBest: false, polish: false })
        o.forceActiveFocus()
        var again = spy(o, "playAgain")
        var menu = spy(o, "menu")
        keyClick(Qt.Key_Return)
        keyClick(Qt.Key_Escape)
        compare(again.count, 1)
        compare(menu.count, 1)
    }
}
}
