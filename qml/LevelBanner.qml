// OWNER: task "Pause overlay + level banner". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Covers the playfield, 336 x 336. Required children: "levelText", "clearText", "nextText", "bar".
Item {
    id: root
    property int level: 1               // the level just cleared
    property bool polish: Theme.polish

    width: Theme.fieldW
    height: Theme.fieldH

    function play() {}
}
