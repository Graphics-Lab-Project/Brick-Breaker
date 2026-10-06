// OWNER: Phase 0. FROZEN. Design tokens (docs/DESIGN_HANDOFF.md section 2-3) + shared helpers.
pragma Singleton
import QtQuick

QtObject {
    // colours
    readonly property color bg:           "#000000"
    readonly property color line:         "#2A2A2A"
    readonly property color lineFaint:    "#1A1A1A"
    readonly property color text:         "#EDEDED"
    readonly property color textDim:      "#7A7A7A"
    readonly property color accent:       "#46E6FF"
    readonly property color paddle:       "#3D7BFF"
    readonly property color ball:         "#F5F5F5"
    readonly property color brickRed:     "#FF4A33"
    readonly property color brickRedDeep: "#2A0B07"
    readonly property color brickAmber:   "#FFB020"
    readonly property color silverFace:   "#7E858E"
    readonly property color silverHi:     "#E3E7EC"
    readonly property color silverLo:     "#4A5058"
    readonly property color capLife:      "#5BE38A"
    readonly property color capMulti:     "#F5F5F5"
    readonly property color capLong:      "#46E6FF"
    readonly property color capGun:       "#FFB020"
    readonly property color capLaser:     "#FF4FD8"
    readonly property color chevronIdle:  "#1E5F6B"
    readonly property color ammoEmpty:    "#4A4A4A"
    readonly property real  flashAlpha:   0.18
    readonly property int   unit: 8
    readonly property bool  polish: true      // T1 effects on/off. Everything must work with false.

    // typography (Silkscreen is bundled by the integration task; falls back if missing)
    readonly property string fontFamily: "Silkscreen"
    readonly property int fontS: 8
    readonly property int fontM: 16
    readonly property int fontL: 24
    readonly property int fontXL: 32

    // geometry (logical px, 360 x 480 canvas)
    readonly property int canvasW: 360
    readonly property int canvasH: 480
    readonly property int fieldX: 12          // playfield origin on the canvas
    readonly property int fieldY: 104
    readonly property int fieldW: 336
    readonly property int fieldH: 336
    readonly property int cellW: 48
    readonly property int cellH: 24
    readonly property int brickW: 46          // cell inset 1 px on each side
    readonly property int brickH: 22
    readonly property int paddleTopY: 322     // field coords
    readonly property int paddleH: 6
    readonly property int ballSize: 6
    readonly property int capsuleW: 20
    readonly property int capsuleH: 10
    readonly property int chevronRowY: 448

    // enum values (mirror of src/engine/Types.h; the engine exposes them as int)
    readonly property int stateMenu: 0
    readonly property int stateReady: 1
    readonly property int statePlaying: 2
    readonly property int statePaused: 3
    readonly property int stateLevelCleared: 4
    readonly property int stateGameOver: 5
    readonly property int modeNormal: 0
    readonly property int modeLong: 1
    readonly property int modeGun: 2
    readonly property int modeLaser: 3
    readonly property int capsuleLife: 0
    readonly property int capsuleMulti: 1
    readonly property int capsuleLong: 2
    readonly property int capsuleGun: 3
    readonly property int capsuleLaser: 4
    readonly property int projectileBullet: 0
    readonly property int projectileLaser: 1

    function capsuleColor(type) {
        switch (type) {
        case 0: return capLife
        case 1: return capMulti
        case 2: return capLong
        case 3: return capGun
        case 4: return capLaser
        }
        return text
    }
    function capsuleName(type) {
        return ["LIFE", "MULTI", "LONG", "GUN", "LASER"][type] || ""
    }
    // Brick tier colour (used for faces and break shards). 0 = silver.
    function tierColor(tier) {
        if (tier >= 3) return brickAmber
        if (tier === 2) return brickRed
        if (tier === 1) return brickRed
        return silverFace
    }
    // "650" -> "00650" (numbers wider than `digits` are shown in full)
    function pad(value, digits) {
        var s = String(Math.max(0, Math.floor(value)))
        while (s.length < digits) s = "0" + s
        return s
    }
}
