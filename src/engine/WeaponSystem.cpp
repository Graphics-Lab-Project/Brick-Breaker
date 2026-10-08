// OWNER: task "Weapon system". Implementation of the gun bullets and laser bolts.
#include "WeaponSystem.h"

#include <algorithm>

namespace BB {

int WeaponSystem::fire(PowerState &s, qreal paddleX, qreal paddleWidth)
{
    switch (s.mode) {
    case PaddleMode::Gun:
        if (s.gunAmmo <= 0)
            return 0;
        m_list.append(Projectile{ProjectileKind::Bullet,
                                 {paddleX - K::BulletW / 2, K::PaddleTopY - K::TurretH - K::BulletH}});
        s.gunAmmo -= 1;
        if (s.gunAmmo == 0)
            s.mode = PaddleMode::Normal;
        return 1;
    case PaddleMode::Laser:
        if (m_cooldown > 0)
            return 0;
        m_list.append(Projectile{ProjectileKind::Laser,
                                 {paddleX - paddleWidth / 2 + 1, K::PaddleTopY - K::TurretH - K::LaserBoltH}});
        m_list.append(Projectile{ProjectileKind::Laser,
                                 {paddleX + paddleWidth / 2 - 1 - K::LaserBoltW, K::PaddleTopY - K::TurretH - K::LaserBoltH}});
        m_cooldown = K::LaserCooldown;
        return 2;
    case PaddleMode::Normal:
    case PaddleMode::Long:
        break;
    }
    return 0;
}

void WeaponSystem::step(qreal dt)
{
    for (Projectile &p : m_list)
        p.pos.y -= K::ProjectileSpeed * dt;
    m_list.erase(std::remove_if(m_list.begin(), m_list.end(),
                                [](const Projectile &p) {
                                    return p.pos.y + rectOf(p).h < 0;
                                }),
                 m_list.end());
    m_cooldown = std::max<qreal>(0, m_cooldown - dt);
}

void WeaponSystem::clear()
{
    m_list.clear();
    m_cooldown = 0;
}

void WeaponSystem::remove(int index)
{
    if (index < 0 || index >= m_list.size())
        return;
    m_list.remove(index);
}

} // namespace BB
