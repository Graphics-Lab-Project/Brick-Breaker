// OWNER: Phase 0. Read-only. Passes at Phase 0: the module, Theme and every stub load.
import QtQuick
import QtTest
import BrickBreaker

Item {
    id: stage
    width: 360; height: 480

TestCase {
    name: "QmlSanity"
    when: windowShown

    Component { id: brickC; Brick {} }
    Component { id: paddleC; Paddle {} }
    Component { id: ballC; Ball {} }
    Component { id: capsuleC; Capsule {} }
    Component { id: projectileC; Projectile {} }
    Component { id: hudC; Hud {} }
    Component { id: pauseButtonC; PauseButton {} }
    Component { id: inputHintC; InputHint {} }
    Component { id: menuC; MenuScreen {} }
    Component { id: optionsC; OptionsScreen {} }
    Component { id: helpC; HelpScreen {} }
    Component { id: pauseC; PauseOverlay {} }
    Component { id: bannerC; LevelBanner {} }
    Component { id: initialsC; InitialsEntry {} }
    Component { id: gameOverC; GameOverOverlay {} }
    Component { id: fxC; FxLayer { engine: MockEngine {} } }
    Component { id: boardC; Board {} }
    Component { id: playfieldC; Playfield { engine: MockEngine {} } }
    Component { id: gameScreenC; GameScreen { engine: MockEngine {} } }
    Component { id: appRootC; AppRoot { engine: MockEngine { gameState: 0 } } }
    Component { id: mockC; MockEngine {} }

    function test_theme() {
        compare(Theme.pad(650, 5), "00650")
        compare(Theme.pad(123456, 5), "123456")
        compare(Theme.capsuleName(Theme.capsuleLaser), "LASER")
        compare(String(Theme.capsuleColor(Theme.capsuleLife)), "#5be38a")
        compare(Theme.fieldW, 336)
    }

    function test_allComponentsLoad_data() {
        return [
            { tag: "Brick", c: brickC }, { tag: "Paddle", c: paddleC }, { tag: "Ball", c: ballC },
            { tag: "Capsule", c: capsuleC }, { tag: "Projectile", c: projectileC }, { tag: "Hud", c: hudC },
            { tag: "PauseButton", c: pauseButtonC }, { tag: "InputHint", c: inputHintC },
            { tag: "MenuScreen", c: menuC }, { tag: "OptionsScreen", c: optionsC }, { tag: "HelpScreen", c: helpC },
            { tag: "PauseOverlay", c: pauseC }, { tag: "LevelBanner", c: bannerC },
            { tag: "InitialsEntry", c: initialsC }, { tag: "GameOverOverlay", c: gameOverC },
            { tag: "FxLayer", c: fxC }, { tag: "Board", c: boardC }, { tag: "Playfield", c: playfieldC },
            { tag: "GameScreen", c: gameScreenC }, { tag: "AppRoot", c: appRootC }
        ]
    }
    function test_allComponentsLoad(data) {
        var o = createTemporaryObject(data.c, stage)
        verify(o !== null, data.tag + " failed to load")
    }

    function test_mockEngine() {
        var m = createTemporaryObject(mockC, stage)
        m.fillBricks([".......", "...2..S"])
        compare(m.bricks.count, 98)
        compare(m.bricks.get(10).hitsLeft, 2)
        compare(m.bricks.get(13).unbreakable, true)
        m.moveLeft(true)
        verify(m.called("moveLeft:true"))
    }
}
}
