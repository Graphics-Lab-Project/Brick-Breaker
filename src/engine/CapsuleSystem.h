#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "Capsule system" -> CapsuleSystem.cpp
#include "Constants.h"
#include "Rng.h"
#include "Types.h"
#include <QVector>
#include <optional>

namespace BB {

class CapsuleSystem {
public:
    explicit CapsuleSystem(RandFn rand = {}) : m_rand(std::move(rand)) {}
    void setRandom(RandFn rand) { m_rand = std::move(rand); }

    // Called when a brick breaks. Calls rand(CapsuleDropOneIn); only a result of 0 drops.
    // Then type = CapsuleType(rand(CapsuleTypeCount)). Spawned capsule's top-left is
    // brickCentre - (CapsuleW/2, CapsuleH/2). Adds it and returns it. No RandFn -> never drops.
    std::optional<Capsule> maybeSpawn(const Vec2 &brickCentre);
    // Moves every capsule down by CapsuleFallSpeed * dt.
    void step(qreal dt);
    // Removes and returns (in list order) every capsule whose rect strictly intersects paddle.
    QVector<CapsuleType> collectCaught(const Rect &paddle);
    // Removes capsules whose top (pos.y) is below FieldH. Returns how many.
    int removeLost();
    void clear();
    void add(const Capsule &c);
    const QVector<Capsule> &capsules() const { return m_caps; }
    static Rect rectOf(const Capsule &c) { return {c.pos.x, c.pos.y, K::CapsuleW, K::CapsuleH}; }

private:
    RandFn m_rand;
    QVector<Capsule> m_caps;
};

} // namespace BB
