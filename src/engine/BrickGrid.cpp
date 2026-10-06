// OWNER: task "Brick grid + levels". Phase 0 stub: compiles, behaviour missing.
#include "BrickGrid.h"

namespace BB {

bool BrickGrid::load(const QStringList &) { return false; }
void BrickGrid::clear() {}
BrickCell BrickGrid::cell(int, int) const { return {}; }
bool BrickGrid::isAlive(int, int) const { return false; }
BrickHitResult BrickGrid::hit(int, int, int) { return {}; }
BrickHitResult BrickGrid::destroy(int, int) { return {}; }
int BrickGrid::remainingBreakable() const { return 0; }
int BrickGrid::aliveCount() const { return 0; }
QVector<BrickCell> BrickGrid::cells() const { return QVector<BrickCell>(K::Rows * K::Cols); }

} // namespace BB
