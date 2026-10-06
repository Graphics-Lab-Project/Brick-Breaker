// OWNER: task "T1 effects layer". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Transient T1 effects over the playfield (336 x 336), spawned from engine signals.
// Effect items use objectNames "breakFx", "scorePop", "splitFlash", "muzzle".
Item {
    id: root
    property var engine: null
    property bool polish: Theme.polish
    property point ballPosition: Qt.point(0, 0)   // field coords of the first ball (for the split flash)
    property int activeEffects: 0                 // number of live effect items

    width: Theme.fieldW
    height: Theme.fieldH
}
