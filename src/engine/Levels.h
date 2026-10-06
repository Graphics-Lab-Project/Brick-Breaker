#pragma once
// OWNER: Phase 0 (interface FROZEN). Implementation: task "Brick grid + levels" -> Levels.cpp
#include <QStringList>

namespace BB::Levels {

// Number of distinct layouts (10).
int count();
// Layout for level n (1-based). Loops: n = 11 -> level 1, n = 12 -> level 2, ...
// n < 1 is treated as 1. Format as BrickGrid::load. Layouts: docs/DESIGN_HANDOFF.md section 7.
QStringList rows(int levelNumber);

} // namespace BB::Levels
