// OWNER: task "Engine core". Invokables, input step, level-clear timer, persistence.
#include "GameEngine.h"
#include "SqliteScoreStore.h"

#include <QDateTime>
#include <QStandardPaths>

namespace BB {

namespace {
const QString kPaddleSpeedKey = QStringLiteral("paddleSpeed");
const QString kAccelerationKey = QStringLiteral("acceleration");
const QString kUnlockedLevelKey = QStringLiteral("unlockedLevel");
const QString kHasProgressKey = QStringLiteral("hasProgress");
}

void GameEngine::startGame()
{
    startLevel(1);
}

void GameEngine::startLevel(int level)
{
    if (level < 1 || level > K::LevelCount || level > unlockedLevel())
        return;
    if (!applyEvent(GameEvent::StartGame))
        return;
    m_power = PowerState();
    syncPower();
    m_runPlaySteps = 0;
    m_currentRunId = 0;
    setScoreValue(0);
    setLevelNumber(level);
    setRoundNumber(1);
    if (m_highScorePending) {
        m_highScorePending = false;
        emit highScorePendingChanged();
    }
    m_paddle.reset();
    loadLevel(level);
    placeBallOnPaddle();
    syncModels();
    if (!m_hasProgress) {
        m_hasProgress = true;
        emit hasProgressChanged();
    }
    savePersistentData();
}

int GameEngine::unlockedLevel() const { return m_unlockAll ? K::LevelCount : m_unlockedLevel; }
bool GameEngine::hasProgress() const { return m_hasProgress; }
bool GameEngine::unlockAll() const { return m_unlockAll; }

void GameEngine::setUnlockAll(bool on)
{
    if (on == m_unlockAll)
        return;
    const int before = unlockedLevel();
    m_unlockAll = on;
    emit unlockAllChanged();
    if (unlockedLevel() != before)
        emit unlockedLevelChanged();
}

void GameEngine::unlockThrough(int level)
{
    level = qMin(level, K::LevelCount);
    const int before = unlockedLevel();
    if (level > m_unlockedLevel)
        m_unlockedLevel = level;
    if (unlockedLevel() != before)
        emit unlockedLevelChanged();
    savePersistentData();
}

// ---- score persistence ----
namespace {
QVariantMap bestToMap(const LevelBest &b)
{
    QVariantMap m;
    m.insert(QStringLiteral("bestScore"), b.bestScore);
    m.insert(QStringLiteral("bestTimeMicros"), b.bestTimeMicros);
    m.insert(QStringLiteral("clears"), b.clears);
    return m;
}

QVariantMap levelClearToMap(const LevelClearRecord &r)
{
    QVariantMap m;
    m.insert(QStringLiteral("level"), r.level);
    m.insert(QStringLiteral("round"), r.round);
    m.insert(QStringLiteral("score"), r.score);
    m.insert(QStringLiteral("timeMicros"), r.timeMicros);
    m.insert(QStringLiteral("initials"), r.initials);
    m.insert(QStringLiteral("at"), r.atUtc.toString(Qt::ISODateWithMs));
    return m;
}

QVariantMap runToMap(const RunRecord &r)
{
    QVariantMap m;
    m.insert(QStringLiteral("initials"), r.initials);
    m.insert(QStringLiteral("score"), r.score);
    m.insert(QStringLiteral("level"), r.level);
    m.insert(QStringLiteral("round"), r.round);
    m.insert(QStringLiteral("timeMicros"), r.timeMicros);
    m.insert(QStringLiteral("at"), r.atUtc.toString(Qt::ISODateWithMs));
    return m;
}
}

qint64 GameEngine::runTimeMicros() const { return stepsToMicros(m_runPlaySteps); }
qint64 GameEngine::levelTimeMicros() const { return stepsToMicros(m_levelPlaySteps); }

QVariantMap GameEngine::levelBest(int level) const
{
    if (!m_scoreStore || level < 1 || level > K::LevelCount)
        return bestToMap(LevelBest());
    return bestToMap(m_scoreStore->levelBest(level));
}

QVariantList GameEngine::levelScores(int level, int limit) const
{
    QVariantList out;
    if (!m_scoreStore || level < 1 || level > K::LevelCount)
        return out;
    const auto rows = m_scoreStore->topLevelScores(level, limit);
    for (const auto &r : rows)
        out.append(levelClearToMap(r));
    return out;
}

QVariantList GameEngine::levelTimes(int level, int limit) const
{
    QVariantList out;
    if (!m_scoreStore || level < 1 || level > K::LevelCount)
        return out;
    const auto rows = m_scoreStore->fastestLevelTimes(level, limit);
    for (const auto &r : rows)
        out.append(levelClearToMap(r));
    return out;
}

QVariantList GameEngine::runScores(int limit) const
{
    QVariantList out;
    if (!m_scoreStore)
        return out;
    const auto rows = m_scoreStore->topRuns(limit);
    for (const auto &r : rows)
        out.append(runToMap(r));
    return out;
}

QVariantList GameEngine::levelBests() const
{
    if (m_levelBests.size() == K::LevelCount)
        return m_levelBests;
    QVariantList out;
    for (int i = 1; i <= K::LevelCount; ++i)
        out.append(levelBest(i));
    return out;
}

void GameEngine::setScoreStore(std::unique_ptr<ScoreStore> store)
{
    m_scoreStore = std::move(store);
    refreshLevelBests();
}

void GameEngine::recordLevelClear()
{
    if (!m_scoreStore)
        return;
    LevelClearRecord r;
    r.level = m_level;
    r.round = m_round;
    r.score = qMax(0, m_score - m_levelStartScore);
    r.timeMicros = levelTimeMicros();
    r.atUtc = QDateTime::currentDateTimeUtc();
    m_scoreStore->addLevelClear(r);
    refreshLevelBests();
}

void GameEngine::recordRun()
{
    if (!m_scoreStore) {
        m_currentRunId = 0;
        return;
    }
    RunRecord r;
    r.score = m_score;
    r.level = m_level;
    r.round = m_round;
    r.timeMicros = runTimeMicros();
    r.atUtc = QDateTime::currentDateTimeUtc();
    m_currentRunId = m_scoreStore->addRun(r);
}

void GameEngine::refreshLevelBests()
{
    QVariantList out;
    for (int i = 1; i <= K::LevelCount; ++i)
        out.append(levelBest(i));
    m_levelBests = out;
    emit levelBestsChanged();
}

void GameEngine::launchOrFire()
{
    if (m_fsm.state() == GameState::Ready) {
        if (!applyEvent(GameEvent::Launch))
            return;
        if (m_balls.isEmpty())
            placeBallOnPaddle();
        const int dir = m_paddle.motionDir() < 0 ? -1 : 1;
        m_balls[0].vel = BallPhysics::launchVelocity(dir, m_pace.ballSpeed());
        syncModels();
    } else if (m_fsm.state() == GameState::Playing) {
        fireWeapon();
        syncModels();
    }
}

void GameEngine::togglePause()
{
    applyEvent(GameEvent::TogglePause);
}

void GameEngine::quitToMenu()
{
    applyEvent(GameEvent::QuitToMenu);
    m_balls.clear();
    clearTransient();
    syncModels();
}

void GameEngine::moveLeft(bool pressed)
{
    m_leftHeld = pressed;
    m_paddle.setInputDir((m_rightHeld ? 1 : 0) - (m_leftHeld ? 1 : 0));
    syncModels();
}

void GameEngine::moveRight(bool pressed)
{
    m_rightHeld = pressed;
    m_paddle.setInputDir((m_rightHeld ? 1 : 0) - (m_leftHeld ? 1 : 0));
    syncModels();
}

void GameEngine::setPointerX(qreal fieldX)
{
    const GameState s = m_fsm.state();
    if (s != GameState::Ready && s != GameState::Playing)
        return;
    m_paddle.setPointerX(fieldX);
    if (s == GameState::Ready && !m_balls.isEmpty())
        placeBallOnPaddle();
    syncModels();
}

void GameEngine::setPaddleSpeed(int value)
{
    value = qBound(1, value, 5);
    if (value == m_paddle.speedSetting())
        return;
    m_paddle.setSpeedSetting(value);
    emit paddleSpeedChanged();
    savePersistentData();
}

void GameEngine::setAcceleration(bool on)
{
    if (on == m_paddle.acceleration())
        return;
    m_paddle.setAcceleration(on);
    emit accelerationChanged();
    savePersistentData();
}

void GameEngine::submitInitials(const QString &initials)
{
    if (m_fsm.state() != GameState::GameOver || !m_highScorePending)
        return;
    if (m_highScores.insert(initials, m_score, m_level) < 0)
        return;
    m_highScorePending = false;
    emit highScorePendingChanged();
    emit highScoresChanged();
    if (m_scoreStore && m_currentRunId > 0)
        m_scoreStore->setRunInitials(m_currentRunId, HighScoreTable::normalizeInitials(initials));
    addScore(0);
    savePersistentData();
}

void GameEngine::stepInput(qreal h)
{
    const GameState s = m_fsm.state();
    if (s != GameState::Ready && s != GameState::Playing)
        return;
    m_paddle.step(h);
    if (s == GameState::Ready && !m_balls.isEmpty())
        placeBallOnPaddle();
}

void GameEngine::stepStateTimers(qreal h)
{
    if (m_fsm.state() != GameState::LevelCleared)
        return;
    m_levelClearTimer -= h;
    if (m_levelClearTimer > 1e-9)
        return;

    if (m_level >= K::LevelCount) {
        setLevelNumber(1);
        setRoundNumber(m_round + 1);
    } else {
        setLevelNumber(m_level + 1);
    }
    const int lives = m_power.lives;
    m_power = PowerState();
    m_power.lives = lives;
    syncPower();
    m_paddle.reset();
    loadLevel(m_level);
    placeBallOnPaddle();
    applyEvent(GameEvent::NextLevel);
    syncModels();
}

void GameEngine::loadPersistentData()
{
    if (m_storagePath.isEmpty()) {
        m_scoreStore.reset();
    } else {
        const QString dbPath = m_storagePath == QLatin1String("native")
            ? QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/scores.sqlite")
            : m_storagePath + QStringLiteral(".scores.sqlite");
        m_scoreStore = std::make_unique<SqliteScoreStore>(dbPath);
    }
    refreshLevelBests();
    auto s = openSettings();
    if (!s)
        return;
    const int speed = qBound(1, s->value(kPaddleSpeedKey, K::DefaultPaddleSpeedSetting).toInt(), 5);
    const bool accel = s->value(kAccelerationKey, false).toBool();
    if (speed != m_paddle.speedSetting()) {
        m_paddle.setSpeedSetting(speed);
        emit paddleSpeedChanged();
    }
    if (accel != m_paddle.acceleration()) {
        m_paddle.setAcceleration(accel);
        emit accelerationChanged();
    }
    const int before = unlockedLevel();
    m_unlockedLevel = qBound(1, s->value(kUnlockedLevelKey, 1).toInt(), K::LevelCount);
    if (unlockedLevel() != before)
        emit unlockedLevelChanged();
    const bool progress = s->value(kHasProgressKey, false).toBool();
    if (progress != m_hasProgress) {
        m_hasProgress = progress;
        emit hasProgressChanged();
    }
    m_highScores.load(*s);
}

void GameEngine::savePersistentData()
{
    auto s = openSettings();
    if (!s)
        return;
    s->setValue(kPaddleSpeedKey, m_paddle.speedSetting());
    s->setValue(kAccelerationKey, m_paddle.acceleration());
    s->setValue(kUnlockedLevelKey, m_unlockedLevel);
    s->setValue(kHasProgressKey, m_hasProgress);
    m_highScores.save(*s);
    s->sync();
}

} // namespace BB
