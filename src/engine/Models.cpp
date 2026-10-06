// OWNER: task "List models". Phase 0 stub: compiles, behaviour missing.
#include "Models.h"

namespace BB {

int BrickModel::rowCount(const QModelIndex &) const { return 0; }
QVariant BrickModel::data(const QModelIndex &, int) const { return {}; }
QHash<int, QByteArray> BrickModel::roleNames() const { return {}; }
void BrickModel::resetCells(const QVector<BrickCell> &) {}
void BrickModel::updateCell(int, int, const BrickCell &) {}

int BallModel::rowCount(const QModelIndex &) const { return 0; }
QVariant BallModel::data(const QModelIndex &, int) const { return {}; }
QHash<int, QByteArray> BallModel::roleNames() const { return {}; }
void BallModel::setBalls(const QVector<Ball> &) {}

int CapsuleModel::rowCount(const QModelIndex &) const { return 0; }
QVariant CapsuleModel::data(const QModelIndex &, int) const { return {}; }
QHash<int, QByteArray> CapsuleModel::roleNames() const { return {}; }
void CapsuleModel::setCapsules(const QVector<Capsule> &) {}

int ProjectileModel::rowCount(const QModelIndex &) const { return 0; }
QVariant ProjectileModel::data(const QModelIndex &, int) const { return {}; }
QHash<int, QByteArray> ProjectileModel::roleNames() const { return {}; }
void ProjectileModel::setProjectiles(const QVector<Projectile> &) {}

} // namespace BB
