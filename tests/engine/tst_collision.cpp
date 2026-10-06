// Acceptance tests for task "Collision helpers". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "Collision.h"
#include "Constants.h"

using namespace BB;
using namespace BB::Collision;

class TstCollision : public QObject {
    Q_OBJECT
private slots:
    void cellRect_noOffset()
    {
        const Rect r = cellRect(2, 3, 0);
        QCOMPARE(r.x, 144.0);
        QCOMPARE(r.y, 48.0);
        QCOMPARE(r.w, 48.0);
        QCOMPARE(r.h, 24.0);
    }
    void cellRect_withOffset()
    {
        const Rect r = cellRect(0, 0, 2);
        QCOMPARE(r.x, 0.0);
        QCOMPARE(r.y, 48.0);
    }
    void ballRect_fromCentre()
    {
        const Rect r = ballRect(Vec2{100, 50});
        QCOMPARE(r.x, 97.0);
        QCOMPARE(r.y, 47.0);
        QCOMPARE(r.w, 6.0);
        QCOMPARE(r.h, 6.0);
    }
    void cellsOverlapping_single()
    {
        const auto cells = cellsOverlapping(Rect{150, 50, 6, 6}, 0);
        QCOMPARE(cells.size(), 1);
        QCOMPARE(cells[0], QPoint(3, 2));   // (col, row)
    }
    void cellsOverlapping_cornerOfFour_rowMajor()
    {
        const auto cells = cellsOverlapping(Rect{45, 21, 6, 6}, 0);
        QCOMPARE(cells.size(), 4);
        QCOMPARE(cells[0], QPoint(0, 0));
        QCOMPARE(cells[1], QPoint(1, 0));
        QCOMPARE(cells[2], QPoint(0, 1));
        QCOMPARE(cells[3], QPoint(1, 1));
    }
    void cellsOverlapping_touchingEdgeIsNotOverlap()
    {
        // right edge exactly at x = 48 touches column 1 but does not overlap it
        const auto cells = cellsOverlapping(Rect{42, 30, 6, 6}, 0);
        QCOMPARE(cells.size(), 1);
        QCOMPARE(cells[0], QPoint(0, 1));
    }
    void cellsOverlapping_respectsOffset()
    {
        // y 50..56 with offset 1 -> grid row 1 (drawn at 48..72)
        const auto cells = cellsOverlapping(Rect{10, 50, 6, 6}, 1);
        QCOMPARE(cells.size(), 1);
        QCOMPARE(cells[0], QPoint(0, 1));
    }
    void cellsOverlapping_clipsToGrid()
    {
        QVERIFY(cellsOverlapping(Rect{-20, -20, 6, 6}, 0).isEmpty());
        QVERIFY(cellsOverlapping(Rect{400, 10, 6, 6}, 0).isEmpty());
        QVERIFY(cellsOverlapping(Rect{10, 340, 6, 4}, 0).isEmpty());   // below row 13
        // with offset 2 the wall starts at y 48: a rect at y 10 maps to grid row -2 -> nothing
        QVERIFY(cellsOverlapping(Rect{10, 10, 6, 6}, 2).isEmpty());
    }
    void hitAxis_none()
    {
        const Rect t{48, 48, 48, 24};
        QCOMPARE(ballHitAxis(Rect{10, 10, 6, 6}, Rect{12, 12, 6, 6}, t), HitAxis::None);
    }
    void hitAxis_fromBelow_isY()
    {
        const Rect t{48, 48, 48, 24};          // bottom = 72
        const Rect prev{60, 73, 6, 6};
        const Rect cur{60, 70, 6, 6};
        QCOMPARE(ballHitAxis(prev, cur, t), HitAxis::Y);
    }
    void hitAxis_fromAbove_isY()
    {
        const Rect t{48, 48, 48, 24};
        QCOMPARE(ballHitAxis(Rect{60, 41, 6, 6}, Rect{60, 44, 6, 6}, t), HitAxis::Y);
    }
    void hitAxis_fromSide_isX()
    {
        const Rect t{48, 48, 48, 24};          // left = 48
        QCOMPARE(ballHitAxis(Rect{41, 55, 6, 6}, Rect{44, 55, 6, 6}, t), HitAxis::X);
        QCOMPARE(ballHitAxis(Rect{97, 55, 6, 6}, Rect{94, 55, 6, 6}, t), HitAxis::X);
    }
    void hitAxis_corner_isBoth()
    {
        const Rect t{48, 48, 48, 24};
        QCOMPARE(ballHitAxis(Rect{41, 41, 6, 6}, Rect{44, 44, 6, 6}, t), HitAxis::Both);
    }
    void hitAxis_alreadyInside_isY()
    {
        const Rect t{48, 48, 48, 24};
        QCOMPARE(ballHitAxis(Rect{60, 55, 6, 6}, Rect{61, 56, 6, 6}, t), HitAxis::Y);
    }
    void reflect_axes()
    {
        const Vec2 v{30, -40};
        Vec2 r = reflect(v, HitAxis::X);
        QCOMPARE(r.x, -30.0); QCOMPARE(r.y, -40.0);
        r = reflect(v, HitAxis::Y);
        QCOMPARE(r.x, 30.0); QCOMPARE(r.y, 40.0);
        r = reflect(v, HitAxis::Both);
        QCOMPARE(r.x, -30.0); QCOMPARE(r.y, 40.0);
        r = reflect(v, HitAxis::None);
        QCOMPARE(r.x, 30.0); QCOMPARE(r.y, -40.0);
    }
    void reflect_zeroVelocity()
    {
        const Vec2 r = reflect(Vec2{0, 0}, HitAxis::Both);
        QCOMPARE(r.x, 0.0);
        QCOMPARE(r.y, 0.0);
    }
};

QTEST_APPLESS_MAIN(TstCollision)
#include "tst_collision.moc"
