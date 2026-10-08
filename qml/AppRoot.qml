import QtQuick
import BrickBreaker

// Screen switcher, 360 x 480. Required children: "menuScreen", "optionsScreen", "helpScreen", "gameScreen".
FocusScope {
    id: root
    property var engine: null
    property string screen: "menu"      // "menu" | "options" | "help" | "levels"

    width: Theme.canvasW
    height: Theme.canvasH

    // The game is shown whenever a run is active and no other screen is on top.
    readonly property bool inGame: root.screen === "menu" && root.engine !== null
                                   && root.engine.gameState !== Theme.stateMenu

    MenuScreen {
        objectName: "menuScreen"
        id: menuScreen
        visible: root.screen === "menu" && !root.inGame
        focus: visible
        bestScore: root.engine ? root.engine.bestScore : 0
        hasProgress: root.engine ? root.engine.hasProgress : false
        onStartGame: root.screen = "levels"
        onOpenOptions: root.screen = "options"
        onOpenHelp: root.screen = "help"
        onQuit: Qt.quit()
    }

    GameScreen {
        objectName: "gameScreen"
        id: gameScreen
        engine: root.engine
        visible: root.inGame
        focus: visible
        onOptionsRequested: root.screen = "options"
    }

    OptionsScreen {
        objectName: "optionsScreen"
        id: optionsScreen
        visible: root.screen === "options"
        focus: visible
        paddleSpeed: root.engine ? root.engine.paddleSpeed : 3
        acceleration: root.engine ? root.engine.acceleration : false
        onPaddleSpeedRequested: (value) => { if (root.engine) root.engine.setPaddleSpeed(value) }
        onAccelerationRequested: (on) => { if (root.engine) root.engine.setAcceleration(on) }
        onBack: root.screen = "menu"
    }

    HelpScreen {
        objectName: "helpScreen"
        id: helpScreen
        visible: root.screen === "help"
        focus: visible
        onBack: root.screen = "menu"
    }

    LevelSelectScreen {
        objectName: "levelSelectScreen"
        id: levelSelectScreen
        visible: root.screen === "levels"
        focus: visible
        unlockedLevel: root.engine ? root.engine.unlockedLevel : 1
        levelCount: root.engine ? root.engine.levelCount : 20
        onLevelChosen: (level) => {
            if (root.engine) root.engine.startLevel(level)
            root.screen = "menu"
        }
        onBack: root.screen = "menu"
    }
}
