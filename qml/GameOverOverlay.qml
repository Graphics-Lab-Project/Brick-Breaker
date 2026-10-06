// OWNER: task "Game over + initials". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Covers the playfield, 336 x 336. Required children: "titleText", "scoreText", "newBestText",
// "initialsEntry" (InitialsEntry), "hsInitials0..4", "hsScore0..4", "hsLevel0..4",
// "btnPlayAgain", "btnMenu".
FocusScope {
    id: root
    property int score: 0
    property bool newBest: false
    property var entries: []            // [{initials, score, level}] like GameEngine.highScores
    property int highlightRank: -1
    property bool polish: Theme.polish

    signal initialsSubmitted(string initials)
    signal playAgain()
    signal menu()

    width: Theme.fieldW
    height: Theme.fieldH
}
