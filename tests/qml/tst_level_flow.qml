// Acceptance tests for task "Level select wiring" (MenuScreen + AppRoot). READ-ONLY for agents (AGENTS.md R4).
// Uses the REAL GameEngine.
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "LevelFlow"
    when: windowShown

    Component { id: engineC; GameEngine {} }
    Component { id: rootC; AppRoot {} }
    Component { id: menuC; MenuScreen {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }
    function make(engineProps) {
        var e = createTemporaryObject(engineC, stage, engineProps || {})
        verify(e !== null)
        var r = createTemporaryObject(rootC, stage, { engine: e })
        verify(r !== null)
        r.forceActiveFocus()
        return { e: e, r: r }
    }

    function test_menuLabel() {
        var m = createTemporaryObject(menuC, stage, {})
        compare(m.hasProgress, false)
        compare(child(m, "itemStart").label, "START GAME")
        m.hasProgress = true
        compare(child(m, "itemStart").label, "CONTINUE")
    }
    function test_startOpensLevelSelect() {
        var t = make()
        compare(child(t.r, "levelSelectScreen").visible, false)
        keyClick(Qt.Key_Return)
        compare(child(t.r, "levelSelectScreen").visible, true)
        compare(child(t.r, "menuScreen").visible, false)
        compare(child(t.r, "gameScreen").visible, false)
        compare(t.e.gameState, 0)
        verify(child(t.r, "levelSelectScreen").activeFocus)
    }
    function test_escapeBackToMenu() {
        var t = make()
        keyClick(Qt.Key_Return)
        keyClick(Qt.Key_Escape)
        compare(child(t.r, "levelSelectScreen").visible, false)
        compare(child(t.r, "menuScreen").visible, true)
        verify(child(t.r, "menuScreen").activeFocus)
        compare(t.e.gameState, 0)
    }
    function test_firstRunOnlyLevelOne() {
        var t = make()
        keyClick(Qt.Key_Return)
        var sel = child(t.r, "levelSelectScreen")
        compare(sel.unlockedLevel, 1)
        keyClick(Qt.Key_Down)                    // level 2 is locked
        keyClick(Qt.Key_Return)
        compare(t.e.gameState, 0)
        compare(sel.visible, true)
        keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Return)                  // level 1
        compare(t.e.gameState, 1)
        compare(t.e.level, 1)
        compare(t.e.hasProgress, true)
        compare(child(t.r, "gameScreen").visible, true)
        compare(sel.visible, false)
    }
    function test_continueLabelAfterFirstGame() {
        var t = make()
        compare(child(child(t.r, "menuScreen"), "itemStart").label, "START GAME")
        keyClick(Qt.Key_Return)
        keyClick(Qt.Key_Return)
        t.e.quitToMenu()
        compare(child(t.r, "menuScreen").visible, true)
        compare(child(child(t.r, "menuScreen"), "itemStart").label, "CONTINUE")
    }
    function test_chooseAnUnlockedLevel() {
        var t = make({ unlockAll: true })
        keyClick(Qt.Key_Return)
        var sel = child(t.r, "levelSelectScreen")
        compare(sel.unlockedLevel, 20)
        compare(sel.levelCount, 20)
        child(sel, "levelItem20")                // all 20 rows exist
        for (var i = 0; i < 3; ++i)
            keyClick(Qt.Key_Up)
        keyClick(Qt.Key_Return)                  // level 17
        compare(t.e.gameState, 1)
        compare(t.e.level, 17)
        compare(child(t.r, "gameScreen").visible, true)
    }
}
}
