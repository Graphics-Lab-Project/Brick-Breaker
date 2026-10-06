#pragma once
// OWNER: Phase 0 (interface FROZEN). Implementation: task "Collision helpers" -> Collision.cpp
#include "Types.h"
#include <QPoint>
#include <QVector>

namespace BB::Collision {

enum class HitAxis { None, X, Y, Both };

// Rect of grid cell (row, col) in field coordinates, with the wall shifted down by
// boardOffsetRows: x = col*CellW, y = (row + boardOffsetRows)*CellH, size CellW x CellH.
Rect cellRect(int row, int col, int boardOffsetRows);

// All grid cells whose cellRect strictly intersects r (Rect::intersects), clipped to the
// 7x14 grid. Returned as QPoint(col, row), sorted by row, then col (row-major).
QVector<QPoint> cellsOverlapping(const Rect &r, int boardOffsetRows);

// Which velocity component to flip when the moving ball (prev -> cur) hits target.
//  - cur does not intersect target                       -> None
//  - prev already overlapped target on x (prev.right > t.left && prev.left < t.right)
//    but not on y                                         -> Y  (top/bottom face)
//  - prev overlapped on y but not on x                    -> X  (side face)
//  - prev overlapped on neither axis                      -> Both (corner)
//  - prev overlapped on both (already inside)             -> Y
HitAxis ballHitAxis(const Rect &prevBall, const Rect &curBall, const Rect &target);

// Flip vx for X, vy for Y, both for Both, nothing for None.
Vec2 reflect(const Vec2 &vel, HitAxis axis);

// Square ball rect from its centre (BallSize x BallSize).
Rect ballRect(const Vec2 &centre);

} // namespace BB::Collision
