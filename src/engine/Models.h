#pragma once
// OWNER: Phase 0 (interface FROZEN). Implementation: task "List models" -> Models.cpp
#include "Types.h"
#include <QAbstractListModel>
#include <QVector>
#include <QtQml/qqmlregistration.h>

namespace BB {

// 98 rows (row-major, index = row*7 + col). Roles: row, col, hitsLeft, unbreakable, alive, tier.
class BrickModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by GameEngine")
public:
    enum Roles { RowRole = Qt::UserRole + 1, ColRole, HitsLeftRole, UnbreakableRole, AliveRole, TierRole };
    using QAbstractListModel::QAbstractListModel;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    // Replaces everything (model reset). Expects 98 cells; any other size is ignored.
    void resetCells(const QVector<BrickCell> &cells);
    // Updates one cell and emits dataChanged for exactly that index. Out of range -> no-op.
    void updateCell(int row, int col, const BrickCell &cell);
private:
    QVector<BrickCell> m_cells;
};

// Roles: x, y (ball centre, field coords).
class BallModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by GameEngine")
public:
    enum Roles { XRole = Qt::UserRole + 1, YRole };
    using QAbstractListModel::QAbstractListModel;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    // Same count as before -> dataChanged over all rows only (no reset / insert / remove).
    // Different count -> rows inserted/removed at the end (or a reset); then values updated.
    void setBalls(const QVector<Ball> &balls);
private:
    QVector<Ball> m_balls;
};

// Roles: type (int, CapsuleType), x, y (top-left).
class CapsuleModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by GameEngine")
public:
    enum Roles { TypeRole = Qt::UserRole + 1, XRole, YRole };
    using QAbstractListModel::QAbstractListModel;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setCapsules(const QVector<Capsule> &capsules);  // same update rules as BallModel
private:
    QVector<Capsule> m_caps;
};

// Roles: kind (int, ProjectileKind), x, y (top-left).
class ProjectileModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by GameEngine")
public:
    enum Roles { KindRole = Qt::UserRole + 1, XRole, YRole };
    using QAbstractListModel::QAbstractListModel;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setProjectiles(const QVector<Projectile> &projectiles);  // same update rules as BallModel
private:
    QVector<Projectile> m_list;
};

} // namespace BB
