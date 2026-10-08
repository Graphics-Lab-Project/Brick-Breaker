// OWNER: task "Engine power-ups". Capsules, weapons, projectile hits.
#include "GameEngine.h"

namespace BB {

void GameEngine::stepPowerUps(qreal h)
{
    // capsules
    m_capsules.step(h);
    const Rect paddle{m_paddle.x() - m_paddle.width() / 2.0, K::PaddleTopY, qreal(m_paddle.width()), K::PaddleH};
    const QVector<CapsuleType> caught = m_capsules.collectCaught(paddle);
    for (CapsuleType t : caught) {
        addScore(K::CapsulePoints);
        emit capsuleCaught(int(t));
        applyCapsule(t);
    }
    const int lost = m_capsules.removeLost();
    for (int i = 0; i < lost; ++i)
        emit capsuleLost();

    // weapons
    m_weapons.step(h);
    const int offset = m_pace.boardOffsetRows();
    for (int i = int(m_weapons.projectiles().size()) - 1; i >= 0; --i) {
        const Projectile p = m_weapons.projectiles()[i];
        const Rect pr = WeaponSystem::rectOf(p);
        int hitRow = -1;
        int hitCol = -1;
        for (int row = K::Rows - 1; row >= 0 && hitRow < 0; --row) {
            for (int col = 0; col < K::Cols; ++col) {
                if (!m_grid.isAlive(row, col))
                    continue;
                const Rect cr{col * K::CellW, (row + offset) * K::CellH, K::CellW, K::CellH};
                if (pr.intersects(cr)) {
                    hitRow = row;
                    hitCol = col;
                    break;
                }
            }
        }
        if (hitRow < 0)
            continue;
        m_weapons.remove(i);
        if (p.kind == ProjectileKind::Bullet) {
            const BrickHitResult r = m_grid.destroy(hitRow, hitCol);
            updateBrick(hitRow, hitCol);
            addScore(K::GunKillPoints);
            emit brickBroken(hitRow, hitCol, r.tier);
            onBrickBroken(hitRow, hitCol);
        } else {
            if (m_grid.cell(hitRow, hitCol).unbreakable)
                continue;
            const BrickHitResult r = m_grid.hit(hitRow, hitCol);
            updateBrick(hitRow, hitCol);
            addScore(K::LaserHitPoints);
            emit brickHit(hitRow, hitCol, r.hitsLeft, r.unbreakable);
            if (r.broken) {
                emit brickBroken(hitRow, hitCol, r.tier);
                onBrickBroken(hitRow, hitCol);
            }
        }
    }
    checkLevelCleared();
}

void GameEngine::onBrickBroken(int row, int col)
{
    const Vec2 centre{col * K::CellW + K::CellW / 2.0,
                      (row + m_pace.boardOffsetRows()) * K::CellH + K::CellH / 2.0};
    if (const auto c = m_capsules.maybeSpawn(centre))
        emit capsuleSpawned(int(c->type), c->pos.x, c->pos.y);
}

void GameEngine::fireWeapon()
{
    const int before = int(m_weapons.projectiles().size());
    const int n = m_weapons.fire(m_power, m_paddle.x(), m_paddle.width());
    for (int i = 0; i < n; ++i) {
        const int idx = before + i;
        if (idx >= m_weapons.projectiles().size())
            break;
        const Projectile &p = m_weapons.projectiles()[idx];
        emit projectileFired(int(p.kind), p.pos.x, p.pos.y);
    }
    syncPower();
}

void GameEngine::applyCapsule(CapsuleType type)
{
    const CapsuleEffect fx = PowerUps::apply(m_power, type);
    if (fx.lifeGained)
        emit lifeGained();
    if (fx.multiBall && !m_balls.isEmpty()) {
        const Vec2 origin = m_balls[0].pos;
        const QVector<Vec2> vels = BallPhysics::multiBallVelocities(m_pace.ballSpeed());
        m_balls.clear();
        for (const Vec2 &v : vels)
            m_balls.push_back(Ball{origin, v});
        emit multiBallActivated();
    }
    syncPower();
}

} // namespace BB
