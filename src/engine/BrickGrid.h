#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "Brick grid + levels" -> BrickGrid.cpp
#include "Constants.h"
#include "Types.h"
#include <QStringList>
#include <QVector>
#include <array>

namespace BB {

struct BrickHitResult {
    bool hit = false;          // an alive brick was there
    bool broken = false;       // it died from this call
    int hitsLeft = 0;          // after the call
    bool unbreakable = false;
    int tier = 0;              // the brick's tier (load-time hits; 0 for silver)
};

class BrickGrid {
public:
    // Level format: up to 14 strings, each exactly 7 chars from ". 1 2 3 S".
    // '.' empty, '1'/'2'/'3' = hits (tier), 'S' = silver (alive, unbreakable, hitsLeft 0, tier 0).
    // Missing rows are empty. Invalid input -> returns false and leaves the grid empty.
    bool load(const QStringList &rows);
    void clear();

    // Out-of-range -> a default (dead) BrickCell.
    BrickCell cell(int row, int col) const;
    bool isAlive(int row, int col) const;

    // Damage an alive brick. Silver: hit=true, nothing changes. Breakable: hitsLeft -= damage;
    // at <= 0 it becomes dead with hitsLeft 0 and broken=true. Dead/out-of-range: hit=false.
    BrickHitResult hit(int row, int col, int damage = 1);
    // Gun bullet: kills any alive brick, silver included (broken=true).
    BrickHitResult destroy(int row, int col);

    int remainingBreakable() const;   // alive && !unbreakable
    int aliveCount() const;           // includes silver
    QVector<BrickCell> cells() const; // 98 cells, row-major (index = row*7 + col)

    static int index(int row, int col) { return row * K::Cols + col; }
    static bool inRange(int row, int col) { return row >= 0 && row < K::Rows && col >= 0 && col < K::Cols; }

private:
    std::array<BrickCell, K::Rows * K::Cols> m_cells{};
};

} // namespace BB
