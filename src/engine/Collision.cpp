// OWNER: task "Collision helpers". Phase 0 stub: compiles, behaviour missing.
#include "Collision.h"
#include "Constants.h"

namespace BB::Collision {

Rect cellRect(int, int, int) { return {}; }
QVector<QPoint> cellsOverlapping(const Rect &, int) { return {}; }
HitAxis ballHitAxis(const Rect &, const Rect &, const Rect &) { return HitAxis::None; }
Vec2 reflect(const Vec2 &vel, HitAxis) { return vel; }
Rect ballRect(const Vec2 &) { return {}; }

} // namespace BB::Collision
