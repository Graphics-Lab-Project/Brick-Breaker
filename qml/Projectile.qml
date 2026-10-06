// OWNER: task "Ball, capsule, projectile sprites". Phase 0 stub: interface FROZEN (properties, signals, functions below);
// implement the body. Spec: the task issue + docs/DESIGN_HANDOFF.md. Use 'component X: ...' for helpers.
import QtQuick
import BrickBreaker

// Bullet (2 x 6) or laser bolt (2 x 10). Required child: "body".
Item {
    id: root
    property int kind: 0                // Theme.projectileBullet / projectileLaser
}
