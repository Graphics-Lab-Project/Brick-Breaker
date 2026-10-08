import QtQuick
import BrickBreaker

// 360 x 480 in-game screen. Required children: "hud", "pauseButton", "playfield", "pointerArea",
// "fxLayer", "inputHint", "pauseOverlay", "levelBanner", "gameOverOverlay".
FocusScope {
    id: root
    property var engine: null
    property bool polish: Theme.polish

    signal optionsRequested()

    width: Theme.canvasW
    height: Theme.canvasH
    focus: true

    readonly property int gs: engine ? engine.gameState : 0

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
    }

    Keys.onPressed: (event) => {
        if (!root.engine) return
        switch (event.key) {
        case Qt.Key_Left:
            if (!event.isAutoRepeat) root.engine.moveLeft(true)
            break
        case Qt.Key_Right:
            if (!event.isAutoRepeat) root.engine.moveRight(true)
            break
        case Qt.Key_Space:
            if (!event.isAutoRepeat) root.engine.launchOrFire()
            break
        case Qt.Key_P:
        case Qt.Key_Escape:
            if (!event.isAutoRepeat) root.engine.togglePause()
            break
        default:
            return
        }
        event.accepted = true
    }
    Keys.onReleased: (event) => {
        if (!root.engine || event.isAutoRepeat) return
        if (event.key === Qt.Key_Left) root.engine.moveLeft(false)
        else if (event.key === Qt.Key_Right) root.engine.moveRight(false)
        else return
        event.accepted = true
    }

    Hud {
        objectName: "hud"
        x: 8; y: 8
        score: root.engine ? root.engine.score : 0
        bestScore: root.engine ? root.engine.bestScore : 0
        level: root.engine ? root.engine.level : 1
        gunAmmo: root.engine ? root.engine.gunAmmo : 0
        lives: root.engine ? root.engine.lives : 3
        polish: root.polish
        id: hud
    }

    PauseButton {
        objectName: "pauseButton"
        x: 328; y: 74
        onClicked: if (root.engine) root.engine.togglePause()
    }

    Rectangle {
        x: 8; y: 98
        width: 344; height: 2
        color: Theme.brickAmber
    }

    Rectangle {
        x: 8; y: 100
        width: 344; height: 344
        color: "transparent"
        border.width: 1
        border.color: Theme.line
    }

    Playfield {
        objectName: "playfield"
        id: playfield
        x: 12; y: 104
        engine: root.engine
        polish: root.polish

        FxLayer {
            objectName: "fxLayer"
            anchors.fill: parent
            engine: root.engine
            polish: root.polish
            ballPosition: (root.engine && root.engine.balls && root.engine.balls.count > 0)
                          ? Qt.point(root.engine.balls.get(0).x, root.engine.balls.get(0).y)
                          : Qt.point(0, 0)
        }

        MouseArea {
            objectName: "pointerArea"
            anchors.fill: parent
            hoverEnabled: true
            onPositionChanged: (mouse) => { if (root.engine) root.engine.setPointerX(mouse.x) }
            onClicked: if (root.engine) root.engine.launchOrFire()
        }

        PauseOverlay {
            objectName: "pauseOverlay"
            visible: root.gs === Theme.statePaused
            focus: visible
            onResume: if (root.engine) root.engine.togglePause()
            onOpenOptions: root.optionsRequested()
            onQuitToMenu: if (root.engine) root.engine.quitToMenu()
        }

        LevelBanner {
            objectName: "levelBanner"
            id: levelBanner
            visible: root.gs === Theme.stateLevelCleared
            level: root.engine ? root.engine.level : 1
            polish: root.polish
        }

        GameOverOverlay {
            objectName: "gameOverOverlay"
            visible: root.gs === Theme.stateGameOver
            focus: visible
            score: root.engine ? root.engine.score : 0
            newBest: root.engine ? root.engine.highScorePending : false
            entries: root.engine ? root.engine.highScores : []
            polish: root.polish
            onInitialsSubmitted: (initials) => { if (root.engine) root.engine.submitInitials(initials) }
            onPlayAgain: if (root.engine) root.engine.startGame()
            onMenu: if (root.engine) root.engine.quitToMenu()
        }
    }

    InputHint {
        objectName: "inputHint"
        y: 448
        inputDir: root.engine ? root.engine.inputDir : 0
        gameState: root.gs
        paddleMode: root.engine ? root.engine.paddleMode : 0
    }

    Connections {
        target: root.engine
        ignoreUnknownSignals: true
        function onLifeLost() { hud.flashLifeLost() }
        function onLifeGained() { hud.pulseLifeGained() }
        function onLevelCleared() { levelBanner.play() }
    }
}
