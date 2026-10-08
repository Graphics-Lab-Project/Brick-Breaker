import QtQuick
import BrickBreaker

// Application window: fixed 720 x 960, the 360 x 480 canvas scaled 2x.
Window {
    width: 720
    height: 960
    minimumWidth: 720
    minimumHeight: 960
    maximumWidth: 720
    maximumHeight: 960
    visible: true
    color: "black"
    title: "Brick Breaker"

    FontLoader { source: "qrc:/fonts/Silkscreen-Regular.ttf" }
    FontLoader { source: "qrc:/fonts/Silkscreen-Bold.ttf" }

    GameEngine {
        id: engine
        storagePath: "native"
    }

    FrameAnimation {
        running: true
        onTriggered: engine.tick(frameTime)
    }

    Item {
        width: Theme.canvasW
        height: Theme.canvasH
        scale: 2
        transformOrigin: Item.TopLeft

        AppRoot {
            id: appRoot
            anchors.fill: parent
            engine: engine
            focus: true
        }
    }
}
