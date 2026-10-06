// Acceptance tests for task "High-score table". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "HighScoreTable.h"
#include <QTemporaryDir>

using namespace BB;

class TstHighScoreTable : public QObject {
    Q_OBJECT
private slots:
    void normalize()
    {
        QCOMPARE(HighScoreTable::normalizeInitials("bny"), QStringLiteral("BNY"));
        QCOMPARE(HighScoreTable::normalizeInitials("  AbC "), QStringLiteral("ABC"));
        QCOMPARE(HighScoreTable::normalizeInitials("AB"), QString());
        QCOMPARE(HighScoreTable::normalizeInitials("ABCD"), QString());
        QCOMPARE(HighScoreTable::normalizeInitials("A1C"), QString());
        QCOMPARE(HighScoreTable::normalizeInitials(""), QString());
    }
    void emptyTable()
    {
        HighScoreTable t;
        QCOMPARE(t.bestScore(), 0);
        QVERIFY(t.qualifies(10));
        QVERIFY(!t.qualifies(0));
        QVERIFY(t.toVariantList().isEmpty());
    }
    void insertSorted()
    {
        HighScoreTable t;
        QCOMPARE(t.insert("AAA", 500, 2), 0);
        QCOMPARE(t.insert("BBB", 900, 3), 0);
        QCOMPARE(t.insert("CCC", 700, 3), 1);
        QCOMPARE(t.entries().size(), 3);
        BB_REQUIRE_INDEX(t.entries(), 0);
        QCOMPARE(t.entries()[0].initials, QStringLiteral("BBB"));
        BB_REQUIRE_INDEX(t.entries(), 1);
        QCOMPARE(t.entries()[1].score, 700);
        BB_REQUIRE_INDEX(t.entries(), 2);
        QCOMPARE(t.entries()[2].level, 2);
        QCOMPARE(t.bestScore(), 900);
    }
    void tieGoesAfter()
    {
        HighScoreTable t;
        t.insert("AAA", 500, 1);
        QCOMPARE(t.insert("BBB", 500, 1), 1);
        BB_REQUIRE_INDEX(t.entries(), 0);
        QCOMPARE(t.entries()[0].initials, QStringLiteral("AAA"));
    }
    void keepsFive()
    {
        HighScoreTable t;
        for (int i = 1; i <= 5; ++i)
            t.insert("AAA", i * 100, 1);
        QVERIFY(!t.qualifies(100));      // equal to lowest: no
        QVERIFY(t.qualifies(101));
        QCOMPARE(t.insert("ZZZ", 50, 1), -1);
        QCOMPARE(t.insert("NEW", 250, 1), 3);
        QCOMPARE(t.entries().size(), 5);
        QCOMPARE(t.entries().last().score, 200);
    }
    void rejectsBadInitials()
    {
        HighScoreTable t;
        QCOMPARE(t.insert("X", 500, 1), -1);
        QVERIFY(t.entries().isEmpty());
        QCOMPARE(t.insert("xyz", 500, 1), 0);
        BB_REQUIRE_INDEX(t.entries(), 0);
        QCOMPARE(t.entries()[0].initials, QStringLiteral("XYZ"));
    }
    void variantList()
    {
        HighScoreTable t;
        t.insert("BNY", 1240, 5);
        const QVariantList l = t.toVariantList();
        QCOMPARE(l.size(), 1);
        const QVariantMap m = l[0].toMap();
        QCOMPARE(m.value("initials").toString(), QStringLiteral("BNY"));
        QCOMPARE(m.value("score").toInt(), 1240);
        QCOMPARE(m.value("level").toInt(), 5);
    }
    void saveLoadRoundTrip()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("hs.ini");
        {
            HighScoreTable t;
            t.insert("AAA", 300, 1);
            t.insert("BBB", 800, 4);
            QSettings s(path, QSettings::IniFormat);
            t.save(s);
            s.sync();
        }
        HighScoreTable u;
        u.insert("OLD", 5, 1);           // load replaces contents
        QSettings s(path, QSettings::IniFormat);
        u.load(s);
        QCOMPARE(u.entries().size(), 2);
        BB_REQUIRE_INDEX(u.entries(), 0);
        QCOMPARE(u.entries()[0].initials, QStringLiteral("BBB"));
        BB_REQUIRE_INDEX(u.entries(), 0);
        QCOMPARE(u.entries()[0].score, 800);
        BB_REQUIRE_INDEX(u.entries(), 0);
        QCOMPARE(u.entries()[0].level, 4);
        BB_REQUIRE_INDEX(u.entries(), 1);
        QCOMPARE(u.entries()[1].initials, QStringLiteral("AAA"));
    }
    void loadMissingIsEmpty()
    {
        QTemporaryDir dir;
        QSettings s(dir.filePath("none.ini"), QSettings::IniFormat);
        HighScoreTable t;
        t.insert("AAA", 10, 1);
        t.load(s);
        QVERIFY(t.entries().isEmpty());
    }
    void clear()
    {
        HighScoreTable t;
        t.insert("AAA", 10, 1);
        t.clear();
        QVERIFY(t.entries().isEmpty());
    }
};

QTEST_GUILESS_MAIN(TstHighScoreTable)
#include "tst_high_score_table.moc"
