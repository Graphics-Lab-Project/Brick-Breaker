// OWNER: task "High-score table". Implementation of HighScoreTable.h.
#include "HighScoreTable.h"

#include <algorithm>

namespace BB {

namespace {
const char kArrayKey[] = "highscores";
}

QString HighScoreTable::normalizeInitials(const QString &raw)
{
    const QString s = raw.trimmed().toUpper();
    if (s.size() != 3)
        return {};
    for (const QChar c : s) {
        if (c < QLatin1Char('A') || c > QLatin1Char('Z'))
            return {};
    }
    return s;
}

bool HighScoreTable::qualifies(int score) const
{
    if (score <= 0)
        return false;
    if (m_entries.size() < K::HighScoreEntries)
        return true;
    return score > m_entries.last().score;
}

int HighScoreTable::insert(const QString &initials, int score, int level)
{
    const QString norm = normalizeInitials(initials);
    if (norm.isEmpty() || !qualifies(score))
        return -1;

    // First entry strictly lower than score: the new entry goes after equal scores.
    int rank = 0;
    while (rank < m_entries.size() && m_entries[rank].score >= score)
        ++rank;

    HighScoreEntry e;
    e.initials = norm;
    e.score = score;
    e.level = level;
    m_entries.insert(rank, e);

    while (m_entries.size() > K::HighScoreEntries)
        m_entries.removeLast();
    return rank;
}

int HighScoreTable::bestScore() const
{
    return m_entries.isEmpty() ? 0 : m_entries.first().score;
}

QVariantList HighScoreTable::toVariantList() const
{
    QVariantList out;
    for (const HighScoreEntry &e : m_entries) {
        QVariantMap m;
        m.insert(QStringLiteral("initials"), e.initials);
        m.insert(QStringLiteral("score"), e.score);
        m.insert(QStringLiteral("level"), e.level);
        out.append(m);
    }
    return out;
}

void HighScoreTable::load(QSettings &s)
{
    QVector<HighScoreEntry> loaded;
    const int count = s.beginReadArray(QString::fromLatin1(kArrayKey));
    for (int i = 0; i < count; ++i) {
        s.setArrayIndex(i);
        HighScoreEntry e;
        e.initials = normalizeInitials(s.value(QStringLiteral("initials")).toString());
        if (e.initials.isEmpty())
            continue;
        e.score = s.value(QStringLiteral("score")).toInt();
        e.level = s.value(QStringLiteral("level")).toInt();
        loaded.append(e);
    }
    s.endArray();

    // Defensive: keep rank order (descending score, stable) and at most K::HighScoreEntries.
    std::stable_sort(loaded.begin(), loaded.end(),
                     [](const HighScoreEntry &a, const HighScoreEntry &b) {
                         return a.score > b.score;
                     });
    while (loaded.size() > K::HighScoreEntries)
        loaded.removeLast();
    m_entries = loaded;
}

void HighScoreTable::save(QSettings &s) const
{
    const QString key = QString::fromLatin1(kArrayKey);
    s.remove(key);
    s.beginWriteArray(key, m_entries.size());
    for (int i = 0; i < m_entries.size(); ++i) {
        s.setArrayIndex(i);
        s.setValue(QStringLiteral("initials"), m_entries[i].initials);
        s.setValue(QStringLiteral("score"), m_entries[i].score);
        s.setValue(QStringLiteral("level"), m_entries[i].level);
    }
    s.endArray();
}

void HighScoreTable::clear()
{
    m_entries.clear();
}

} // namespace BB
