// OWNER: task "List models".
#include "Models.h"
#include "Constants.h"

namespace BB {

namespace {
constexpr int kBrickCount = K::Rows * K::Cols;

// Same count -> dataChanged only; otherwise insert/remove rows at the end, then update values.
// Model begin/end calls are protected, so the model passes them in as callables.
template <typename T, typename Ins, typename Rem, typename Changed>
void syncList(QVector<T> &store, const QVector<T> &next, Ins ins, Rem rem, Changed changed)
{
    const int oldN = store.size();
    const int newN = next.size();
    if (newN > oldN) {
        ins(oldN, newN - 1, [&] { store = next; });
    } else if (newN < oldN) {
        rem(newN, oldN - 1, [&] { store = next; });
    } else {
        store = next;
    }
    const int common = qMin(oldN, newN);
    if (common > 0)
        changed(common - 1);
}
}

int BrickModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_cells.size();
}

QVariant BrickModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_cells.size())
        return {};
    const BrickCell &c = m_cells.at(index.row());
    switch (role) {
    case RowRole: return index.row() / K::Cols;
    case ColRole: return index.row() % K::Cols;
    case HitsLeftRole: return c.hitsLeft;
    case UnbreakableRole: return c.unbreakable;
    case AliveRole: return c.alive;
    case TierRole: return c.tier;
    }
    return {};
}

QHash<int, QByteArray> BrickModel::roleNames() const
{
    return {{RowRole, "row"}, {ColRole, "col"}, {HitsLeftRole, "hitsLeft"},
            {UnbreakableRole, "unbreakable"}, {AliveRole, "alive"}, {TierRole, "tier"}};
}

void BrickModel::resetCells(const QVector<BrickCell> &cells)
{
    if (cells.size() != kBrickCount)
        return;
    beginResetModel();
    m_cells = cells;
    endResetModel();
}

void BrickModel::updateCell(int row, int col, const BrickCell &cell)
{
    if (row < 0 || row >= K::Rows || col < 0 || col >= K::Cols)
        return;
    const int i = row * K::Cols + col;
    if (i >= m_cells.size())
        return;
    m_cells[i] = cell;
    const QModelIndex idx = index(i);
    emit dataChanged(idx, idx);
}

int BallModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_balls.size();
}

QVariant BallModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_balls.size())
        return {};
    const Ball &b = m_balls.at(index.row());
    switch (role) {
    case XRole: return b.pos.x;
    case YRole: return b.pos.y;
    }
    return {};
}

QHash<int, QByteArray> BallModel::roleNames() const
{
    return {{XRole, "x"}, {YRole, "y"}};
}

void BallModel::setBalls(const QVector<Ball> &balls) { syncList(m_balls, balls,
             [this](int f, int l, auto set) { beginInsertRows({}, f, l); set(); endInsertRows(); },
             [this](int f, int l, auto set) { beginRemoveRows({}, f, l); set(); endRemoveRows(); },
             [this](int l) { emit dataChanged(index(0), index(l)); }); }

int CapsuleModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_caps.size();
}

QVariant CapsuleModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_caps.size())
        return {};
    const Capsule &c = m_caps.at(index.row());
    switch (role) {
    case TypeRole: return static_cast<int>(c.type);
    case XRole: return c.pos.x;
    case YRole: return c.pos.y;
    }
    return {};
}

QHash<int, QByteArray> CapsuleModel::roleNames() const
{
    return {{TypeRole, "type"}, {XRole, "x"}, {YRole, "y"}};
}

void CapsuleModel::setCapsules(const QVector<Capsule> &capsules) { syncList(m_caps, capsules,
             [this](int f, int l, auto set) { beginInsertRows({}, f, l); set(); endInsertRows(); },
             [this](int f, int l, auto set) { beginRemoveRows({}, f, l); set(); endRemoveRows(); },
             [this](int l) { emit dataChanged(index(0), index(l)); }); }

int ProjectileModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_list.size();
}

QVariant ProjectileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
        return {};
    const Projectile &p = m_list.at(index.row());
    switch (role) {
    case KindRole: return static_cast<int>(p.kind);
    case XRole: return p.pos.x;
    case YRole: return p.pos.y;
    }
    return {};
}

QHash<int, QByteArray> ProjectileModel::roleNames() const
{
    return {{KindRole, "kind"}, {XRole, "x"}, {YRole, "y"}};
}

void ProjectileModel::setProjectiles(const QVector<Projectile> &projectiles)
{
    syncList(m_list, projectiles,
             [this](int f, int l, auto set) { beginInsertRows({}, f, l); set(); endInsertRows(); },
             [this](int f, int l, auto set) { beginRemoveRows({}, f, l); set(); endRemoveRows(); },
             [this](int l) { emit dataChanged(index(0), index(l)); });
}

} // namespace BB
