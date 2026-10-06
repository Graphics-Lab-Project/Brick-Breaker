// Acceptance tests for task "List models". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "Models.h"
#include <QAbstractItemModelTester>
#include <QSignalSpy>

using namespace BB;

namespace {
QVariant roleValue(const QAbstractListModel &m, int row, const char *roleName)
{
    const auto names = m.roleNames();
    const int role = names.key(QByteArray(roleName), -1);
    if (role < 0)
        return QVariant();
    return m.data(m.index(row, 0), role);
}
QVector<BrickCell> cells98()
{
    return QVector<BrickCell>(98);
}
}

class TstModels : public QObject {
    Q_OBJECT
private slots:
    void brick_roleNames()
    {
        BrickModel m;
        const auto names = m.roleNames().values();
        for (const char *n : {"row", "col", "hitsLeft", "unbreakable", "alive", "tier"})
            QVERIFY2(names.contains(QByteArray(n)), n);
    }
    void brick_resetAndData()
    {
        BrickModel m;
        QAbstractItemModelTester tester(&m, QAbstractItemModelTester::FailureReportingMode::QtTest);
        QVector<BrickCell> c = cells98();
        c[10] = BrickCell{2, 2, false, true};   // row 1, col 3
        c[0] = BrickCell{0, 0, true, true};
        QSignalSpy reset(&m, &QAbstractItemModel::modelReset);
        m.resetCells(c);
        QCOMPARE(reset.count(), 1);
        QCOMPARE(m.rowCount(), 98);
        QCOMPARE(roleValue(m, 10, "row").toInt(), 1);
        QCOMPARE(roleValue(m, 10, "col").toInt(), 3);
        QCOMPARE(roleValue(m, 10, "hitsLeft").toInt(), 2);
        QCOMPARE(roleValue(m, 10, "tier").toInt(), 2);
        QCOMPARE(roleValue(m, 10, "alive").toBool(), true);
        QCOMPARE(roleValue(m, 0, "unbreakable").toBool(), true);
        QCOMPARE(roleValue(m, 5, "alive").toBool(), false);
    }
    void brick_wrongSizeIgnored()
    {
        BrickModel m;
        m.resetCells(QVector<BrickCell>(5));
        QCOMPARE(m.rowCount(), 0);
    }
    void brick_updateCellEmitsOneDataChanged()
    {
        BrickModel m;
        m.resetCells(cells98());
        QSignalSpy changed(&m, &QAbstractItemModel::dataChanged);
        m.updateCell(2, 4, BrickCell{1, 2, false, true});
        QCOMPARE(changed.count(), 1);
        const QModelIndex tl = changed[0][0].value<QModelIndex>();
        const QModelIndex br = changed[0][1].value<QModelIndex>();
        QCOMPARE(tl.row(), 18);
        QCOMPARE(br.row(), 18);
        QCOMPARE(roleValue(m, 18, "hitsLeft").toInt(), 1);
        m.updateCell(20, 0, BrickCell{});           // out of range
        QCOMPARE(changed.count(), 1);
    }
    void ball_setAndUpdateInPlace()
    {
        BallModel m;
        QAbstractItemModelTester tester(&m, QAbstractItemModelTester::FailureReportingMode::QtTest);
        m.setBalls({Ball{{10, 20}, {}}, Ball{{30, 40}, {}}});
        QCOMPARE(m.rowCount(), 2);
        QCOMPARE(roleValue(m, 1, "x").toDouble(), 30.0);
        QCOMPARE(roleValue(m, 1, "y").toDouble(), 40.0);

        QSignalSpy reset(&m, &QAbstractItemModel::modelReset);
        QSignalSpy inserted(&m, &QAbstractItemModel::rowsInserted);
        QSignalSpy removed(&m, &QAbstractItemModel::rowsRemoved);
        QSignalSpy changed(&m, &QAbstractItemModel::dataChanged);
        m.setBalls({Ball{{11, 21}, {}}, Ball{{31, 41}, {}}});
        QCOMPARE(reset.count(), 0);
        QCOMPARE(inserted.count(), 0);
        QCOMPARE(removed.count(), 0);
        QVERIFY(changed.count() >= 1);
        QCOMPARE(roleValue(m, 0, "x").toDouble(), 11.0);
    }
    void ball_countChanges()
    {
        BallModel m;
        QAbstractItemModelTester tester(&m, QAbstractItemModelTester::FailureReportingMode::QtTest);
        m.setBalls({Ball{{10, 20}, {}}});
        m.setBalls({Ball{{1, 1}, {}}, Ball{{2, 2}, {}}, Ball{{3, 3}, {}}, Ball{{4, 4}, {}}});
        QCOMPARE(m.rowCount(), 4);
        QCOMPARE(roleValue(m, 3, "x").toDouble(), 4.0);
        m.setBalls({});
        QCOMPARE(m.rowCount(), 0);
    }
    void capsule_roles()
    {
        CapsuleModel m;
        QAbstractItemModelTester tester(&m, QAbstractItemModelTester::FailureReportingMode::QtTest);
        m.setCapsules({Capsule{CapsuleType::Gun, {5, 6}}});
        QCOMPARE(m.rowCount(), 1);
        QCOMPARE(roleValue(m, 0, "type").toInt(), 3);
        QCOMPARE(roleValue(m, 0, "x").toDouble(), 5.0);
        QCOMPARE(roleValue(m, 0, "y").toDouble(), 6.0);
        QSignalSpy reset(&m, &QAbstractItemModel::modelReset);
        m.setCapsules({Capsule{CapsuleType::Gun, {5, 9}}});
        QCOMPARE(reset.count(), 0);
        QCOMPARE(roleValue(m, 0, "y").toDouble(), 9.0);
    }
    void projectile_roles()
    {
        ProjectileModel m;
        QAbstractItemModelTester tester(&m, QAbstractItemModelTester::FailureReportingMode::QtTest);
        m.setProjectiles({Projectile{ProjectileKind::Laser, {7, 8}}, Projectile{ProjectileKind::Bullet, {1, 2}}});
        QCOMPARE(m.rowCount(), 2);
        QCOMPARE(roleValue(m, 0, "kind").toInt(), 1);
        QCOMPARE(roleValue(m, 1, "kind").toInt(), 0);
        QCOMPARE(roleValue(m, 0, "x").toDouble(), 7.0);
        m.setProjectiles({});
        QCOMPARE(m.rowCount(), 0);
    }
    void invalidIndex()
    {
        BallModel m;
        m.setBalls({Ball{{1, 2}, {}}});
        QVERIFY(!m.data(m.index(5, 0), BallModel::XRole).isValid());
    }
};

QTEST_GUILESS_MAIN(TstModels)
#include "tst_models.moc"
