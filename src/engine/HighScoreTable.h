#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "High-score table" -> HighScoreTable.cpp
#include "Constants.h"
#include <QSettings>
#include <QString>
#include <QVariantList>
#include <QVector>

namespace BB {

struct HighScoreEntry {
    QString initials;
    int score = 0;
    int level = 0;
};

class HighScoreTable {
public:
    // Trim + upper-case. Returns the result if it is exactly 3 letters A-Z, else "".
    static QString normalizeInitials(const QString &raw);
    // score > 0 and (fewer than HighScoreEntries entries, or score > lowest entry's score).
    bool qualifies(int score) const;
    // Inserts sorted by score (descending); a new entry goes AFTER existing equal scores.
    // Keeps at most HighScoreEntries. Returns the 0-based rank, or -1 if rejected
    // (invalid initials, or it does not qualify).
    int insert(const QString &initials, int score, int level);
    int bestScore() const;                       // 0 when empty
    const QVector<HighScoreEntry> &entries() const { return m_entries; }
    // [{ "initials": "BNY", "score": 1240, "level": 5 }, ...] in rank order
    QVariantList toVariantList() const;
    // QSettings array "highscores" with keys initials / score / level.
    // load() replaces the current contents (and re-sorts / truncates defensively).
    void load(QSettings &s);
    void save(QSettings &s) const;
    void clear();

private:
    QVector<HighScoreEntry> m_entries;
};

} // namespace BB
