// OWNER: task "Capsule system".
#include "CapsuleSystem.h"

namespace BB {

std::optional<Capsule> CapsuleSystem::maybeSpawn(const Vec2 &brickCentre)
{
    if (!m_rand)
        return std::nullopt;
    if (m_rand(K::CapsuleDropOneIn) != 0)
        return std::nullopt;
    const auto type = CapsuleType(m_rand(K::CapsuleTypeCount));
    Capsule c;
    c.type = type;
    c.pos = Vec2{brickCentre.x - K::CapsuleW / 2, brickCentre.y - K::CapsuleH / 2};
    m_caps.append(c);
    return c;
}

void CapsuleSystem::step(qreal dt)
{
    for (Capsule &c : m_caps)
        c.pos.y += K::CapsuleFallSpeed * dt;
}

QVector<CapsuleType> CapsuleSystem::collectCaught(const Rect &paddle)
{
    QVector<CapsuleType> caught;
    QVector<Capsule> rest;
    for (const Capsule &c : m_caps) {
        if (rectOf(c).intersects(paddle))
            caught.append(c.type);
        else
            rest.append(c);
    }
    m_caps = rest;
    return caught;
}

int CapsuleSystem::removeLost()
{
    int removed = 0;
    QVector<Capsule> rest;
    for (const Capsule &c : m_caps) {
        if (c.pos.y > K::FieldH)
            ++removed;
        else
            rest.append(c);
    }
    m_caps = rest;
    return removed;
}

void CapsuleSystem::clear() { m_caps.clear(); }

void CapsuleSystem::add(const Capsule &c) { m_caps.append(c); }

} // namespace BB
