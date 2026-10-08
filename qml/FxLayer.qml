// OWNER: task "T1 effects layer". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Transient T1 effects over the playfield (336 x 336), spawned from engine signals.
// Effect items use objectNames "breakFx", "scorePop", "splitFlash", "muzzle".
Item {
    id: root
    property var engine: null
    property bool polish: Theme.polish
    property point ballPosition: Qt.point(0, 0)   // field coords of the first ball (for the split flash)
    property int activeEffects: 0                 // number of live effect items

    width: Theme.fieldW
    height: Theme.fieldH

    // Each effect emits done() when finished; the layer then destroys it.
    component BreakFx: Item {
        id: bfx
        property color tint: Theme.brickRed
        signal done()
        objectName: "breakFx"
        width: Theme.brickW
        height: Theme.brickH
        property real t: 0                         // seconds since spawn, drives shards

        Rectangle {
            id: body
            anchors.fill: parent
            color: bfx.tint
            transformOrigin: Item.Center
            NumberAnimation on scale { from: 1; to: 0.6; duration: 150; easing.type: Easing.OutQuad }
            NumberAnimation on opacity { from: 1; to: 0; duration: 150; easing.type: Easing.OutQuad }
        }
        Repeater {
            model: 6
            Rectangle {
                required property int index
                readonly property real vx: (index % 2 === 0 ? 1 : -1) * (20 + 8 * index)
                width: 2; height: 2
                color: bfx.tint
                x: Theme.brickW / 2 + vx * bfx.t
                y: Theme.brickH / 2 - 40 * bfx.t + 300 * bfx.t * bfx.t
                opacity: Math.max(0, 1 - bfx.t / 0.18)
            }
        }
        NumberAnimation on t { from: 0; to: 0.18; duration: 180; onFinished: bfx.done() }
    }

    component ScorePop: Text {
        id: pop
        signal done()
        objectName: "scorePop"
        text: "+50"
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontS
        property real startY: 0
        y: startY
        NumberAnimation on y { from: pop.startY; to: pop.startY - 24; duration: 500; easing.type: Easing.OutQuad }
        SequentialAnimation on opacity {
            NumberAnimation { from: 1; to: 0; duration: 500; easing.type: Easing.OutQuad }
            ScriptAction { script: pop.done() }
        }
    }

    component Flash: Rectangle {
        id: fl
        signal done()
        property real startOpacity: 1
        color: "white"
        SequentialAnimation on opacity {
            NumberAnimation { from: fl.startOpacity; to: 0; duration: fl.duration; easing.type: Easing.Linear }
            ScriptAction { script: fl.done() }
        }
        property int duration: 60
    }

    function spawn(comp, props) {
        var item = comp.createObject(root, props)
        if (!item)
            return
        activeEffects++
        item.done.connect(function () {
            activeEffects--
            item.destroy()
        })
    }

    Component { id: breakC; BreakFx {} }
    Component { id: popC; ScorePop {} }
    Component { id: flashC; Flash {} }

    Connections {
        target: root.polish ? root.engine : null
        function onBrickBroken(row, col, tier) {
            var off = root.engine.boardOffsetRows || 0
            root.spawn(breakC, { x: col * Theme.cellW + 1, y: (row + off) * Theme.cellH + 1,
                                 tint: Theme.tierColor(tier) })
        }
        function onCapsuleCaught(type) {
            root.spawn(popC, { x: root.engine.paddleX - 8, startY: Theme.paddleTopY - 10 })
        }
        function onProjectileFired(kind, x, y) {
            root.spawn(flashC, { objectName: "muzzle", x: x - 3, y: y - 4, width: 6, height: 6,
                                 duration: 60, startOpacity: 1, color: Theme.capGun })
        }
        function onMultiBallActivated() {
            root.spawn(flashC, { objectName: "splitFlash", x: root.ballPosition.x - 5,
                                 y: root.ballPosition.y - 5, width: 10, height: 10,
                                 duration: 80, startOpacity: 0.7 })
        }
    }
}
