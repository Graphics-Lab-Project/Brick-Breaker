import QtQuick
import BrickBreaker

// 336 x 336 playfield, clip. Required children: "board" (Board), "paddle" (Paddle),
// "ballRepeater", "capsuleRepeater", "projectileRepeater", "flashTint", "flashName".
Item {
    id: root
    property var engine: null
    property bool polish: Theme.polish
    property int flashType: -1          // capsule type being flashed, -1 when idle

    width: Theme.fieldW
    height: Theme.fieldH
    clip: true

    Board {
        id: board
        objectName: "board"
        bricks: root.engine ? root.engine.bricks : null
        boardOffsetRows: root.engine ? root.engine.boardOffsetRows : 0
        polish: root.polish
    }

    Repeater {
        id: capsuleRepeater
        objectName: "capsuleRepeater"
        model: root.engine ? root.engine.capsules : null
        delegate: Capsule {
            required property var model
            x: model.x
            y: model.y
            capsuleType: model.type
            polish: root.polish
        }
    }

    Repeater {
        id: projectileRepeater
        objectName: "projectileRepeater"
        model: root.engine ? root.engine.projectiles : null
        delegate: Projectile {
            required property var model
            x: model.x
            y: model.y
            kind: model.kind
        }
    }

    Repeater {
        id: ballRepeater
        objectName: "ballRepeater"
        model: root.engine ? root.engine.balls : null
        delegate: Ball {
            required property var model
            centerX: model.x
            centerY: model.y
            fast: root.engine ? root.engine.speedState === 1 : false
            polish: root.polish
        }
    }

    Paddle {
        id: paddle
        objectName: "paddle"
        paddleX: root.engine ? root.engine.paddleX : 168
        paddleWidth: root.engine ? root.engine.paddleWidth : 64
        paddleMode: root.engine ? root.engine.paddleMode : 0
        polish: root.polish
    }

    Rectangle {
        id: flashTint
        objectName: "flashTint"
        anchors.fill: parent
        visible: root.flashType >= 0
        color: root.flashType >= 0 ? Theme.capsuleColor(root.flashType) : "transparent"
        opacity: 0.18
    }

    Text {
        id: flashName
        objectName: "flashName"
        anchors.centerIn: parent
        visible: root.flashType >= 0
        text: root.flashType >= 0 ? Theme.capsuleName(root.flashType) : ""
        color: root.flashType >= 0 ? Theme.capsuleColor(root.flashType) : "white"
        font.family: Theme.fontFamily
        font.pixelSize: 24
    }

    Timer {
        id: flashTimer
        interval: 500
        onTriggered: root.flashType = -1
    }

    Connections {
        target: root.engine
        ignoreUnknownSignals: true
        function onCapsuleCaught(type) {
            root.flashType = type
            flashTimer.restart()
        }
        function onBrickHit(row, col) {
            if (root.polish)
                board.flashBrick(row, col)
        }
        function onLifeLost() {
            if (root.polish)
                paddle.blink()
        }
    }
}
