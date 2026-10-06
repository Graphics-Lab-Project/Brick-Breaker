// OWNER: task "High-score table". Phase 0 stub: compiles, behaviour missing.
#include "HighScoreTable.h"

namespace BB {

QString HighScoreTable::normalizeInitials(const QString &) { return {}; }
bool HighScoreTable::qualifies(int) const { return false; }
int HighScoreTable::insert(const QString &, int, int) { return -1; }
int HighScoreTable::bestScore() const { return 0; }
QVariantList HighScoreTable::toVariantList() const { return {}; }
void HighScoreTable::load(QSettings &) {}
void HighScoreTable::save(QSettings &) const {}
void HighScoreTable::clear() {}

} // namespace BB
