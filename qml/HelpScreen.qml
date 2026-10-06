// OWNER: task "Help screen". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// 360 x 480. Required children: "capsuleName0".."capsuleName4", "capsuleDesc0".."capsuleDesc4",
// "scoreBrickHit", "scoreCapsule", "scoreGunKill", "scoreLaserHit".
FocusScope {
    id: root
    signal back()

    width: Theme.canvasW
    height: Theme.canvasH
}
