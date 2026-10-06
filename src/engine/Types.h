#pragma once
// OWNER: Phase 0. FROZEN. Read-only for every task agent.
#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace BB {
Q_NAMESPACE
QML_NAMED_ELEMENT(BB)

// Integer values are part of the QML contract (QML side may compare ints).
enum class GameState { Menu = 0, Ready = 1, Playing = 2, Paused = 3, LevelCleared = 4, GameOver = 5 };
Q_ENUM_NS(GameState)
enum class SpeedState { Slow = 0, Fast = 1 };
Q_ENUM_NS(SpeedState)
enum class PaddleMode { Normal = 0, Long = 1, Gun = 2, Laser = 3 };
Q_ENUM_NS(PaddleMode)
enum class CapsuleType { Life = 0, Multi = 1, Long = 2, Gun = 3, Laser = 4 };
Q_ENUM_NS(CapsuleType)
enum class ProjectileKind { Bullet = 0, Laser = 1 };
Q_ENUM_NS(ProjectileKind)

struct Vec2 {
    qreal x = 0;
    qreal y = 0;
};

// Axis-aligned rectangle, top-left + size.
struct Rect {
    qreal x = 0;
    qreal y = 0;
    qreal w = 0;
    qreal h = 0;
    qreal left() const { return x; }
    qreal right() const { return x + w; }
    qreal top() const { return y; }
    qreal bottom() const { return y + h; }
    // Strict overlap: rectangles that only touch along an edge do NOT intersect.
    bool intersects(const Rect &o) const
    {
        return left() < o.right() && right() > o.left() && top() < o.bottom() && bottom() > o.top();
    }
};

struct Ball {
    Vec2 pos;   // centre
    Vec2 vel;   // px/s
};

struct BrickCell {
    int hitsLeft = 0;          // 0 for empty / broken / silver
    int tier = 0;              // hits at level load (1..3); 0 for silver/empty
    bool unbreakable = false;  // silver
    bool alive = false;
};

struct Capsule {
    CapsuleType type = CapsuleType::Life;
    Vec2 pos;                  // top-left
};

struct Projectile {
    ProjectileKind kind = ProjectileKind::Bullet;
    Vec2 pos;                  // top-left
};

} // namespace BB
