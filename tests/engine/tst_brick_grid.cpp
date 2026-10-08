// Acceptance tests for task "Brick grid + levels". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "BrickGrid.h"
#include "Levels.h"

using namespace BB;

namespace {
// Expected layouts (docs/LEVEL_SELECT.md, docs/DESIGN_HANDOFF.md section 7): 20 levels, at most 4 silver 'S' each.
const QList<QStringList> kLevels = {
    {".......", ".11.11.", ".11.21.", ".......", ".21.21.", ".11.11.", ".......", ".12.11.", ".11.11."},
    {".......", "1111111", ".......", "2222222", ".......", "1111111"},
    {".......", "...2...", "..212..", ".11211.", "1112111"},
    {".......", ".1.1.1.", "1.1.1.1", ".2.2.2.", "1.1.1.1", ".1.1.1."},
    {".......", "2.2.2.2", "1.1.1.1", "1.1.1.1", "1.1.1.1", "2.2.2.2"},
    {"...2...", "..121..", ".12221.", "..121..", "...2..."},
    {"2222222", "2.....2", "2.111.2", "2.....2", "2222222"},
    {"3333333", "1111111", "2222222", "1111111"},
    {".......", ".33333.", ".22122.", "S12321S", ".11111.", "S11.11S"},
    {".......", "1S1.1S1", "2121212", "2S2.2S2", "1111111", ".......", "3.3.3.3"},
    {".......", ".1...1.", "..111..", ".11111.", "11.1.11", "1222221", "1.111.1", "..1.1.."},
    {".......", ".11.11.", "1221221", "1223221", ".12221.", "..121..", "...1..."},
    {".......", "...3...", "..111..", ".11211.", "..111..", ".11211.", "1121211", "...2..."},
    {".......", "...3...", "..222..", "..212..", "..212..", ".22222.", "1.222.1", "..1.1.."},
    {".......", "111...S", ".222...", "..111..", "...222.", "..111..", ".222...", "111...S"},
    {".......", "3333333", ".22222.", "..111..", "...S...", "..111..", ".22222.", "2222222"},
    {".......", "3.S.S.3", "22.1.22", "2221222", "2221222", ".2.1.2.", "..212.."},
    {".......", "1111111", "1.....1", "1.222.1", "1.232.1", "1.222.1", "1.....1", "1111111"},
    {".......", "2.2.2.2", "2222222", "3S3S3S3", "1111111"},
    {".......", ".22222.", "3333333", "33S3S33", "3333333", ".11111.", ".1.1.1."},
};
}

class TstBrickGrid : public QObject {
    Q_OBJECT
private slots:
    void load_level1()
    {
        BrickGrid g;
        QVERIFY(g.load(kLevels[0]));
        QCOMPARE(g.aliveCount(), 24);
        QCOMPARE(g.remainingBreakable(), 24);
        const BrickCell c = g.cell(2, 4);       // ".11.21." -> col 4 = '2'
        QVERIFY(c.alive);
        QCOMPARE(c.hitsLeft, 2);
        QCOMPARE(c.tier, 2);
        QVERIFY(!c.unbreakable);
        QVERIFY(!g.isAlive(0, 0));
        QVERIFY(!g.isAlive(13, 6));            // rows past the layout are empty
    }
    void load_silver()
    {
        BrickGrid g;
        QVERIFY(g.load({"S.3...."}));
        const BrickCell s = g.cell(0, 0);
        QVERIFY(s.alive);
        QVERIFY(s.unbreakable);
        QCOMPARE(s.hitsLeft, 0);
        QCOMPARE(s.tier, 0);
        QCOMPARE(g.cell(0, 2).hitsLeft, 3);
        QCOMPARE(g.aliveCount(), 2);
        QCOMPARE(g.remainingBreakable(), 1);
    }
    void load_rejectsBadInput()
    {
        BrickGrid g;
        QVERIFY(g.load({"1111111"}));
        QVERIFY(!g.load({"11111"}));                 // short row
        QCOMPARE(g.aliveCount(), 0);                 // grid left empty
        QVERIFY(!g.load({"11x1111"}));               // bad char
        QStringList tooMany;
        for (int i = 0; i < 15; ++i)
            tooMany << ".......";
        QVERIFY(!g.load(tooMany));
    }
    void load_emptyIsValid()
    {
        BrickGrid g;
        QVERIFY(g.load({}));
        QCOMPARE(g.aliveCount(), 0);
    }
    void hit_damagesAndBreaks()
    {
        BrickGrid g;
        g.load({"2......"});
        BrickHitResult r = g.hit(0, 0);
        QVERIFY(r.hit);
        QVERIFY(!r.broken);
        QCOMPARE(r.hitsLeft, 1);
        QCOMPARE(r.tier, 2);
        r = g.hit(0, 0);
        QVERIFY(r.hit);
        QVERIFY(r.broken);
        QCOMPARE(r.hitsLeft, 0);
        QCOMPARE(r.tier, 2);
        QVERIFY(!g.isAlive(0, 0));
        QCOMPARE(g.remainingBreakable(), 0);
    }
    void hit_bigDamage()
    {
        BrickGrid g;
        g.load({"3......"});
        const BrickHitResult r = g.hit(0, 0, 5);
        QVERIFY(r.broken);
        QCOMPARE(r.hitsLeft, 0);
    }
    void hit_silverUnchanged()
    {
        BrickGrid g;
        g.load({"S......"});
        const BrickHitResult r = g.hit(0, 0);
        QVERIFY(r.hit);
        QVERIFY(!r.broken);
        QVERIFY(r.unbreakable);
        QVERIFY(g.isAlive(0, 0));
    }
    void hit_deadOrOutOfRange()
    {
        BrickGrid g;
        g.load({"1......"});
        QVERIFY(!g.hit(0, 1).hit);
        QVERIFY(!g.hit(-1, 0).hit);
        QVERIFY(!g.hit(0, 7).hit);
        QVERIFY(!g.hit(14, 0).hit);
        QCOMPARE(g.cell(-1, -1).alive, false);
    }
    void destroy_killsSilver()
    {
        BrickGrid g;
        g.load({"S3....."});
        BrickHitResult r = g.destroy(0, 0);
        QVERIFY(r.hit);
        QVERIFY(r.broken);
        QVERIFY(r.unbreakable);
        QVERIFY(!g.isAlive(0, 0));
        r = g.destroy(0, 1);
        QVERIFY(r.broken);
        QCOMPARE(r.tier, 3);
        QVERIFY(!g.destroy(0, 1).hit);
    }
    void cells_rowMajor()
    {
        BrickGrid g;
        g.load({".......", "...1..."});
        const QVector<BrickCell> cells = g.cells();
        QCOMPARE(cells.size(), 98);
        QVERIFY(cells[BrickGrid::index(1, 3)].alive);
        QCOMPARE(BrickGrid::index(1, 3), 10);
    }
    void clear()
    {
        BrickGrid g;
        g.load({"1111111"});
        g.clear();
        QCOMPARE(g.aliveCount(), 0);
    }
    void levels_count()
    {
        QCOMPARE(Levels::count(), 20);
    }
    void levels_exactLayouts()
    {
        for (int n = 1; n <= 20; ++n)
            QCOMPARE(Levels::rows(n), kLevels[n - 1]);
    }
    void levels_loop()
    {
        QCOMPARE(Levels::rows(21), kLevels[0]);
        QCOMPARE(Levels::rows(40), kLevels[19]);
        QCOMPARE(Levels::rows(23), kLevels[2]);
        QCOMPARE(Levels::rows(0), kLevels[0]);
    }
    void levels_allLoad()
    {
        for (int n = 1; n <= 20; ++n) {
            BrickGrid g;
            QVERIFY2(g.load(Levels::rows(n)), qPrintable(QString("level %1").arg(n)));
            QVERIFY(g.remainingBreakable() > 0);
        }
    }
    void levels_atMostFourSilver_andFitTheField()
    {
        for (int n = 1; n <= 20; ++n) {
            int silver = 0;
            for (const QString &row : Levels::rows(n))
                silver += row.count(QLatin1Char('S'));
            QVERIFY2(silver <= 4, qPrintable(QString("level %1 has %2 silver").arg(n).arg(silver)));
            QVERIFY2(Levels::rows(n).size() <= 9, qPrintable(QString("level %1 too tall").arg(n)));
        }
    }
};

QTEST_APPLESS_MAIN(TstBrickGrid)
#include "tst_brick_grid.moc"
