// OWNER: Phase 0. Read-only. Stand-in for GameEngine in UI tests (same property/signal/method names).
import QtQuick

QtObject {
    id: mock
    property int gameState: 1
    property int score: 0
    property int bestScore: 0
    property int lives: 3
    property int level: 1
    property int round: 1
    property int speedState: 0
    property real paddleX: 168
    property int paddleWidth: 64
    property int paddleMode: 0
    property int gunAmmo: 0
    property int boardOffsetRows: 0
    property int ballCount: balls.count
    property int inputDir: 0
    property int paddleSpeed: 3
    property bool acceleration: false
    property var highScores: []
    property bool highScorePending: false
    property int levelCount: 20
    property int unlockedLevel: 1
    property bool hasProgress: false

    property ListModel bricks: ListModel {}
    property ListModel balls: ListModel { ListElement { x: 168; y: 319 } }
    property ListModel capsules: ListModel {}
    property ListModel projectiles: ListModel {}

    signal brickHit(int row, int col, int hitsLeft, bool unbreakable)
    signal brickBroken(int row, int col, int tier)
    signal capsuleSpawned(int type, real x, real y)
    signal capsuleCaught(int type)
    signal capsuleLost()
    signal boardShifted(int steps)
    signal projectileFired(int kind, real x, real y)
    signal multiBallActivated()
    signal lifeLost()
    signal lifeGained()
    signal levelCleared()
    signal gameOver()

    // every invokable call is recorded as "name" or "name:arg"
    property var calls: []
    function record(s) { calls = calls.concat([s]) }
    function called(s) { return calls.indexOf(s) >= 0 }
    function clearCalls() { calls = [] }

    function startGame() { record("startGame") }
    function startLevel(level) { record("startLevel:" + level) }
    function launchOrFire() { record("launchOrFire") }
    function togglePause() { record("togglePause") }
    function quitToMenu() { record("quitToMenu") }
    function moveLeft(pressed) { record("moveLeft:" + pressed) }
    function moveRight(pressed) { record("moveRight:" + pressed) }
    function setPointerX(x) { record("setPointerX:" + Math.round(x)) }
    function setPaddleSpeed(v) { record("setPaddleSpeed:" + v) }
    function setAcceleration(on) { record("setAcceleration:" + on) }
    function submitInitials(s) { record("submitInitials:" + s) }
    function tick(dt) {}

    // Fill `bricks` from level strings (same format as the engine).
    function fillBricks(rows) {
        bricks.clear()
        for (var r = 0; r < 14; ++r) {
            for (var c = 0; c < 7; ++c) {
                var ch = (r < rows.length) ? rows[r].charAt(c) : "."
                var silver = ch === "S"
                var hits = (ch >= "1" && ch <= "3") ? Number(ch) : 0
                bricks.append({ row: r, col: c, hitsLeft: hits, unbreakable: silver,
                                alive: silver || hits > 0, tier: hits })
            }
        }
    }
}
