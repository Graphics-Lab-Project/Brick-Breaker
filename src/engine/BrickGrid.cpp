// OWNER: task "Brick grid + levels". Phase 0 stub: compiles, behaviour missing.
#include "BrickGrid.h"

namespace BB {

namespace {
// Parses one level character into a cell. Returns false for characters outside ". 1 2 3 S".
bool parseCell(QChar ch, BrickCell &cell)
{
    cell = BrickCell{};
    switch (ch.unicode()) {
    case '.':
        return true;
    case 'S':
        cell.alive = true;
        cell.unbreakable = true;
        return true;
    case '1':
    case '2':
    case '3':
        cell.alive = true;
        cell.tier = ch.unicode() - '0';
        cell.hitsLeft = cell.tier;
        return true;
    default:
        return false;
    }
}
} // namespace

bool BrickGrid::load(const QStringList &rows)
{
    clear();
    if (rows.size() > K::Rows)
        return false;

    std::array<BrickCell, K::Rows * K::Cols> cells{};
    for (int r = 0; r < rows.size(); ++r) {
        const QString &row = rows.at(r);
        if (row.size() != K::Cols)
            return false;
        for (int c = 0; c < K::Cols; ++c) {
            if (!parseCell(row.at(c), cells[index(r, c)]))
                return false;
        }
    }
    m_cells = cells;
    return true;
}

void BrickGrid::clear()
{
    m_cells = {};
}

BrickCell BrickGrid::cell(int row, int col) const
{
    if (!inRange(row, col))
        return {};
    return m_cells[index(row, col)];
}

bool BrickGrid::isAlive(int row, int col) const
{
    return cell(row, col).alive;
}

BrickHitResult BrickGrid::hit(int row, int col, int damage)
{
    BrickHitResult result;
    if (!inRange(row, col))
        return result;

    BrickCell &c = m_cells[index(row, col)];
    if (!c.alive)
        return result;

    result.hit = true;
    result.unbreakable = c.unbreakable;
    result.tier = c.tier;
    if (c.unbreakable) {
        result.hitsLeft = c.hitsLeft;
        return result;
    }

    c.hitsLeft -= damage;
    if (c.hitsLeft <= 0) {
        c = BrickCell{};
        result.broken = true;
    }
    result.hitsLeft = c.hitsLeft;
    return result;
}

BrickHitResult BrickGrid::destroy(int row, int col)
{
    BrickHitResult result;
    if (!inRange(row, col))
        return result;

    BrickCell &c = m_cells[index(row, col)];
    if (!c.alive)
        return result;

    result.hit = true;
    result.broken = true;
    result.unbreakable = c.unbreakable;
    result.tier = c.tier;
    c = BrickCell{};
    return result;
}

int BrickGrid::remainingBreakable() const
{
    int n = 0;
    for (const BrickCell &c : m_cells) {
        if (c.alive && !c.unbreakable)
            ++n;
    }
    return n;
}

int BrickGrid::aliveCount() const
{
    int n = 0;
    for (const BrickCell &c : m_cells) {
        if (c.alive)
            ++n;
    }
    return n;
}

QVector<BrickCell> BrickGrid::cells() const
{
    QVector<BrickCell> out;
    out.reserve(K::Rows * K::Cols);
    for (const BrickCell &c : m_cells)
        out.append(c);
    return out;
}

} // namespace BB
