#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "Weapon system" -> WeaponSystem.cpp
#include "Constants.h"
#include "PowerUps.h"
#include "Types.h"
#include <QVector>

namespace BB {

class WeaponSystem {
public:
    // Fire according to s.mode. Returns how many projectiles were spawned (0, 1 or 2).
    //  Gun  : if gunAmmo > 0 -> one Bullet at top-left (paddleX - BulletW/2, PaddleTopY - TurretH - BulletH);
    //         gunAmmo -= 1; when it reaches 0, mode -> Normal. No cooldown.
    //  Laser: if cooldown() <= 0 -> two Laser bolts at top-left
    //         (paddleX - paddleWidth/2 + 1, PaddleTopY - TurretH - LaserBoltH) and
    //         (paddleX + paddleWidth/2 - 1 - LaserBoltW, same y); cooldown = LaserCooldown.
    //  Normal / Long: nothing.
    int fire(PowerState &s, qreal paddleX, qreal paddleWidth);
    // Moves every projectile up by ProjectileSpeed * dt, reduces the cooldown by dt (not below 0),
    // and removes projectiles whose bottom (pos.y + height) is above the field top (< 0).
    void step(qreal dt);
    void clear();                    // removes projectiles and resets cooldown to 0
    void remove(int index);          // no-op if out of range
    const QVector<Projectile> &projectiles() const { return m_list; }
    qreal cooldown() const { return m_cooldown; }
    static Rect rectOf(const Projectile &p)
    {
        return p.kind == ProjectileKind::Bullet ? Rect{p.pos.x, p.pos.y, K::BulletW, K::BulletH}
                                                : Rect{p.pos.x, p.pos.y, K::LaserBoltW, K::LaserBoltH};
    }

private:
    QVector<Projectile> m_list;
    qreal m_cooldown = 0;
};

} // namespace BB
