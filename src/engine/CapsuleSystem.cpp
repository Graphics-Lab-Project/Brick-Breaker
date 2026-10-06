// OWNER: task "Capsule system". Phase 0 stub: compiles, behaviour missing.
#include "CapsuleSystem.h"

namespace BB {

std::optional<Capsule> CapsuleSystem::maybeSpawn(const Vec2 &) { return std::nullopt; }
void CapsuleSystem::step(qreal) {}
QVector<CapsuleType> CapsuleSystem::collectCaught(const Rect &) { return {}; }
int CapsuleSystem::removeLost() { return 0; }
void CapsuleSystem::clear() {}
void CapsuleSystem::add(const Capsule &) {}

} // namespace BB
