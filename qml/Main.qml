import QtQuick
import BrickBreaker

// Application window: starts full screen; the 360 x 480 canvas is scaled to the largest size that fits
// and centred (black bars at the sides). F11 toggles windowed mode (resizable, 720 x 960 by default).
Window {
    id: win
    width: 720
    height: 960
    minimumWidth: 360
    minimumHeight: 480
    visibility: Window.FullScreen
    visible: true
    color: "black"
    title: "Brick Breaker"

    FontLoader { source: "qrc:/fonts/Silkscreen-Regular.ttf" }
    FontLoader { source: "qrc:/fonts/Silkscreen-Bold.ttf" }

    GameEngine {
        id: engine
        storagePath: "native"
        unlockAll: true     // DEV: every level unlocked for testing; delete this line to ship with real locking
    }

    Shortcut {
        sequence: "F11"
        onActivated: win.visibility = (win.visibility === Window.FullScreen) ? Window.Windowed
                                                                              : Window.FullScreen
    }

    FrameAnimation {
        running: true
        onTriggered: engine.tick(frameTime)
    }

    Item {
        readonly property real fit: Math.min(win.width / Theme.canvasW, win.height / Theme.canvasH)
        width: Theme.canvasW
        height: Theme.canvasH
        scale: fit
        x: (win.width - width * fit) / 2
        y: (win.height - height * fit) / 2
        transformOrigin: Item.TopLeft

        AppRoot {
            id: appRoot
            anchors.fill: parent
            engine: engine
            focus: true
        }
    }
}
