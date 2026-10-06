// OWNER: Phase 0. FROZEN. Read-only for every task agent.
// Constructor, getters, the fixed-timestep loop, shared helpers and test hooks.
#include "GameEngine.h"

#include <QtGlobal>

namespace BB {

GameEngine::GameEngine(QObject *parent)
    : QObject(parent)
    , m_brickModel(new BrickModel(this))
    , m_ballModel(new BallModel(this))
    , m_capsuleModel(new CapsuleModel(this))
    , m_projectileModel(new ProjectileModel(this))
{
    m_capsules.setRandom(m_rng.fn());
    m_paddle.reset();
    m_brickModel->resetCells(m_grid.cells());
    loadPersistentData();
    m_lastBest = bestScore();
    m_lastPaddleX = m_paddle.x();
    m_lastBallCount = int(m_balls.size());
}

GameEngine::~GameEngine() = default;

// ---------------------------------------------------------------- getters
int GameEngine::gameState() const { return int(m_fsm.state()); }
int GameEngine::score() const { return m_score; }
int GameEngine::bestScore() const { return qMax(m_highScores.bestScore(), m_score); }
int GameEngine::lives() const { return m_power.lives; }
int GameEngine::level() const { return m_level; }
int GameEngine::round() const { return m_round; }
int GameEngine::speedState() const { return int(m_pace.speedState()); }
qreal GameEngine::paddleX() const { return m_paddle.x(); }
int GameEngine::paddleWidth() const { return m_paddle.width(); }
int GameEngine::paddleMode() const { return int(m_power.mode); }
int GameEngine::gunAmmo() const { return m_power.gunAmmo; }
int GameEngine::boardOffsetRows() const { return m_pace.boardOffsetRows(); }
BrickModel *GameEngine::bricks() const { return m_brickModel; }
BallModel *GameEngine::balls() const { return m_ballModel; }
CapsuleModel *GameEngine::capsules() const { return m_capsuleModel; }
ProjectileModel *GameEngine::projectiles() const { return m_projectileModel; }
int GameEngine::ballCount() const { return int(m_balls.size()); }
int GameEngine::inputDir() const { return m_paddle.inputDir(); }
int GameEngine::paddleSpeed() const { return m_paddle.speedSetting(); }
bool GameEngine::acceleration() const { return m_paddle.acceleration(); }
QVariantList GameEngine::highScores() const { return m_highScores.toVariantList(); }
bool GameEngine::highScorePending() const { return m_highScorePending; }
QString GameEngine::storagePath() const { return m_storagePath; }

void GameEngine::setStoragePath(const QString &path)
{
    if (path == m_storagePath)
        return;
    m_storagePath = path;
    emit storagePathChanged();
    loadPersistentData();
    emit highScoresChanged();
    addScore(0); // refresh bestScore notification
}

// ---------------------------------------------------------------- loop
void GameEngine::tick(qreal dt)
{
    dt = qBound<qreal>(0.0, dt, K::MaxTickDt);
    m_accumulator += dt;
    while (m_accumulator + 1e-9 >= K::FixedDt) {
        step(K::FixedDt);
        m_accumulator -= K::FixedDt;
    }
    if (m_accumulator < 0)
        m_accumulator = 0;
}

void GameEngine::step(qreal h)
{
    stepInput(h);
    stepStateTimers(h);
    if (m_fsm.state() == GameState::Playing) {
        stepBalls(h);
        if (m_fsm.state() == GameState::Playing)
            stepPowerUps(h);
    }
    syncModels();
}

// ---------------------------------------------------------------- helpers
void GameEngine::setState(GameState s)
{
    if (m_fsm.state() == s)
        return;
    m_fsm.forceState(s);
    emit gameStateChanged();
}

bool GameEngine::applyEvent(GameEvent e)
{
    const bool changed = m_fsm.handle(e);
    if (changed)
        emit gameStateChanged();
    return changed;
}

void GameEngine::addScore(int points)
{
    if (points != 0) {
        m_score += points;
        emit scoreChanged();
    }
    const int best = bestScore();
    if (best != m_lastBest) {
        m_lastBest = best;
        emit bestScoreChanged();
    }
}

void GameEngine::setScoreValue(int score)
{
    if (score != m_score) {
        m_score = score;
        emit scoreChanged();
    }
    addScore(0);
}

void GameEngine::setLevelNumber(int level)
{
    if (level != m_level) {
        m_level = level;
        emit levelChanged();
    }
}

void GameEngine::setRoundNumber(int round)
{
    if (round != m_round) {
        m_round = round;
        emit roundChanged();
    }
}

void GameEngine::syncPower()
{
    m_paddle.setWidth(m_power.paddleWidth());
    if (int(m_power.mode) != m_lastMode) {
        m_lastMode = int(m_power.mode);
        emit paddleModeChanged(m_lastMode);
    }
    if (m_power.gunAmmo != m_lastAmmo) {
        m_lastAmmo = m_power.gunAmmo;
        emit gunAmmoChanged();
    }
    if (m_power.lives != m_lastLives) {
        m_lastLives = m_power.lives;
        emit livesChanged();
    }
    if (m_paddle.width() != m_lastWidth) {
        m_lastWidth = m_paddle.width();
        emit paddleWidthChanged();
    }
}

void GameEngine::syncPace()
{
    const int speed = int(m_pace.speedState());
    if (speed != m_lastSpeedState) {
        m_lastSpeedState = speed;
        emit speedStateChanged(speed);
    }
    const int offset = m_pace.boardOffsetRows();
    if (offset != m_lastOffset) {
        const int delta = offset - m_lastOffset;
        m_lastOffset = offset;
        emit boardOffsetRowsChanged();
        if (delta > 0)
            emit boardShifted(delta);
    }
}

void GameEngine::placeBallOnPaddle()
{
    m_balls = {BallPhysics::restingBall(m_paddle.x())};
}

void GameEngine::loadRows(const QStringList &rows)
{
    m_grid.load(rows);
    m_brickModel->resetCells(m_grid.cells());
    m_pace.resetForLevel();
    syncPace();
    clearTransient();
}

void GameEngine::loadLevel(int levelNumber)
{
    loadRows(Levels::rows(levelNumber));
}

void GameEngine::clearTransient()
{
    m_capsules.clear();
    m_weapons.clear();
}

void GameEngine::updateBrick(int row, int col)
{
    m_brickModel->updateCell(row, col, m_grid.cell(row, col));
}

void GameEngine::enterLevelCleared()
{
    setState(GameState::LevelCleared);
    m_levelClearTimer = K::LevelClearDelay;
    m_balls.clear();
    clearTransient();
    syncModels();
    emit levelCleared();
}

void GameEngine::enterGameOver()
{
    setState(GameState::GameOver);
    m_balls.clear();
    clearTransient();
    const bool pending = m_highScores.qualifies(m_score);
    if (pending != m_highScorePending) {
        m_highScorePending = pending;
        emit highScorePendingChanged();
    }
    syncModels();
    emit gameOver();
}

void GameEngine::loseLife()
{
    m_power.lives = qMax(0, m_power.lives - 1);
    syncPower();
    emit lifeLost();

    PowerUps::resetForLifeLost(m_power);
    syncPower();
    m_pace.resetForLife();
    syncPace();
    clearTransient();

    if (m_power.lives <= 0) {
        enterGameOver();
    } else {
        placeBallOnPaddle();
        setState(GameState::Ready);
        syncModels();
    }
}

void GameEngine::checkLevelCleared()
{
    if (m_fsm.state() == GameState::Playing && m_grid.remainingBreakable() == 0)
        enterLevelCleared();
}

void GameEngine::syncModels()
{
    m_ballModel->setBalls(m_balls);
    m_capsuleModel->setCapsules(m_capsules.capsules());
    m_projectileModel->setProjectiles(m_weapons.projectiles());

    if (!qFuzzyCompare(1.0 + m_paddle.x(), 1.0 + m_lastPaddleX)) {
        m_lastPaddleX = m_paddle.x();
        emit paddleXChanged();
    }
    if (int(m_balls.size()) != m_lastBallCount) {
        m_lastBallCount = int(m_balls.size());
        emit ballCountChanged();
    }
    if (m_paddle.inputDir() != m_lastInputDir) {
        m_lastInputDir = m_paddle.inputDir();
        emit inputDirChanged();
    }
}

std::unique_ptr<QSettings> GameEngine::openSettings() const
{
    if (m_storagePath.isEmpty())
        return nullptr;
    if (m_storagePath == QLatin1String("native"))
        return std::make_unique<QSettings>();
    return std::make_unique<QSettings>(m_storagePath, QSettings::IniFormat);
}

// ---------------------------------------------------------------- test hooks
void GameEngine::debugSetState(GameState s) { setState(s); }

void GameEngine::debugLoadLevelRows(const QStringList &rows) { loadRows(rows); }

void GameEngine::debugSetBalls(const QVector<Ball> &balls)
{
    m_balls = balls;
    syncModels();
}

void GameEngine::debugSetPaddleX(qreal x)
{
    m_paddle.setPointerX(x);
    m_paddle.step(0); // clear the pointer direction
    syncModels();
}

void GameEngine::debugSetPower(PaddleMode mode, int gunAmmo, int lives)
{
    m_power.mode = mode;
    m_power.gunAmmo = gunAmmo;
    m_power.lives = lives;
    syncPower();
}

void GameEngine::debugSetRandom(RandFn rand) { m_capsules.setRandom(std::move(rand)); }

void GameEngine::debugAddCapsule(const Capsule &c)
{
    m_capsules.add(c);
    syncModels();
}

void GameEngine::debugSetScore(int score) { setScoreValue(score); }

void GameEngine::debugSetLevel(int level) { setLevelNumber(level); }

} // namespace BB
