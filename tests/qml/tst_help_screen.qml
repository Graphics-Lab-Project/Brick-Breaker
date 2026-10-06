// Acceptance tests for task "Help screen". READ-ONLY for agents (AGENTS.md R4).
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "HelpScreen"
    when: windowShown

    Component { id: helpC; HelpScreen {} }
    Component { id: spyC; SignalSpy {} }

    function child(item, name) {
        var c = findChild(item, name)
        verify(c !== null, "missing child '" + name + "'")
        return c
    }

    function test_capsuleLegend() {
        var h = createTemporaryObject(helpC, stage)
        var names = ["LIFE", "MULTI", "LONG", "GUN", "LASER"]
        for (var i = 0; i < 5; ++i) {
            compare(child(h, "capsuleName" + i).text, names[i])
            verify(child(h, "capsuleDesc" + i).text.length > 0)
        }
    }
    function test_scoring() {
        var h = createTemporaryObject(helpC, stage)
        compare(child(h, "scoreBrickHit").text, "10")
        compare(child(h, "scoreCapsule").text, "50")
        compare(child(h, "scoreGunKill").text, "50")
        compare(child(h, "scoreLaserHit").text, "10")
    }
    function test_back() {
        var h = createTemporaryObject(helpC, stage)
        h.forceActiveFocus()
        var s = createTemporaryObject(spyC, stage, { target: h, signalName: "back" })
        keyClick(Qt.Key_Escape)
        keyClick(Qt.Key_Return)
        compare(s.count, 2)
    }
}
}
