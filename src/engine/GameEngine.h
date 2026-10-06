#pragma once
// OWNER: Phase 0. FROZEN (properties, signals, invokables, private helpers, data members).
// Implementation is split by file so tasks never share a file:
//   GameEngine_loop.cpp     Phase 0 (frozen): ctor, getters, tick/step loop, shared helpers, test hooks
//   GameEngine.cpp          task "Engine core"      : invokables, input step, level-clear timer, persistence
//   GameEngine_balls.cpp    task "Engine ball step" : stepBalls (walls, paddle, bricks, losing balls)
//   GameEngine_powerups.cpp task "Engine power-ups" : capsules, weapons, projectile hits
// UI contract: docs/DESIGN_HANDOFF.md section 4. QML never decides gameplay.
#include "BallPhysics.h"
#include "BrickGrid.h"
#include "CapsuleSystem.h"
#include "Collision.h"
#include "Constants.h"
#include "GameStateMachine.h"
#include "HighScoreTable.h"
#include "Levels.h"
#include "Models.h"
#include "PaceController.h"
#include "PaddleLogic.h"
#include "PowerUps.h"
#include "Rng.h"
#include "Types.h"
#include "WeaponSystem.h"

#include <QObject>
#include <QSettings>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>
#include <memory>

namespace BB {

class GameEngine : public QObject {
    Q_OBJECT
    QML_ELEMENT

    // Enum-valued properties are exposed as int (values in Types.h) so QML mocks stay trivial.
    Q_PROPERTY(int gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(int bestScore READ bestScore NOTIFY bestScoreChanged)
    Q_PROPERTY(int lives READ lives NOTIFY livesChanged)
    Q_PROPERTY(int level READ level NOTIFY levelChanged)
    Q_PROPERTY(int round READ round NOTIFY roundChanged)
    Q_PROPERTY(int speedState READ speedState NOTIFY speedStateChanged)
    Q_PROPERTY(qreal paddleX READ paddleX NOTIFY paddleXChanged)
    Q_PROPERTY(int paddleWidth READ paddleWidth NOTIFY paddleWidthChanged)
    Q_PROPERTY(int paddleMode READ paddleMode NOTIFY paddleModeChanged)
    Q_PROPERTY(int gunAmmo READ gunAmmo NOTIFY gunAmmoChanged)
    Q_PROPERTY(int boardOffsetRows READ boardOffsetRows NOTIFY boardOffsetRowsChanged)
    Q_PROPERTY(BB::BrickModel *bricks READ bricks CONSTANT)
    Q_PROPERTY(BB::BallModel *balls READ balls CONSTANT)
    Q_PROPERTY(BB::CapsuleModel *capsules READ capsules CONSTANT)
    Q_PROPERTY(BB::ProjectileModel *projectiles READ projectiles CONSTANT)
    Q_PROPERTY(int ballCount READ ballCount NOTIFY ballCountChanged)
    Q_PROPERTY(int inputDir READ inputDir NOTIFY inputDirChanged)
    Q_PROPERTY(int paddleSpeed READ paddleSpeed NOTIFY paddleSpeedChanged)
    Q_PROPERTY(bool acceleration READ acceleration NOTIFY accelerationChanged)
    Q_PROPERTY(QVariantList highScores READ highScores NOTIFY highScoresChanged)
    Q_PROPERTY(bool highScorePending READ highScorePending NOTIFY highScorePendingChanged)
    // "" = no persistence (default, used by tests), "native" = QSettings(org, app),
    // anything else = path of an INI file. Setting it reloads persistent data.
    Q_PROPERTY(QString storagePath READ storagePath WRITE setStoragePath NOTIFY storagePathChanged)

public:
    explicit GameEngine(QObject *parent = nullptr);
    ~GameEngine() override;

    // ---- getters (Phase 0) ----
    int gameState() const;
    int score() const;
    int bestScore() const;          // max(top high score, current score)
    int lives() const;
    int level() const;
    int round() const;
    int speedState() const;
    qreal paddleX() const;
    int paddleWidth() const;
    int paddleMode() const;
    int gunAmmo() const;
    int boardOffsetRows() const;
    BrickModel *bricks() const;
    BallModel *balls() const;
    CapsuleModel *capsules() const;
    ProjectileModel *projectiles() const;
    int ballCount() const;
    int inputDir() const;
    int paddleSpeed() const;
    bool acceleration() const;
    QVariantList highScores() const;
    bool highScorePending() const;
    QString storagePath() const;
    void setStoragePath(const QString &path);

    // ---- QML invokables: task "Engine core" (GameEngine.cpp) ----
    // All of them update properties/models immediately (no tick needed).
    Q_INVOKABLE void startGame();            // Menu|GameOver -> Ready; score 0, lives 3, level 1, round 1, level 1 loaded, ball on paddle
    Q_INVOKABLE void launchOrFire();         // Ready -> Playing (launch); Playing -> fireWeapon()
    Q_INVOKABLE void togglePause();          // Ready|Playing <-> Paused
    Q_INVOKABLE void quitToMenu();           // any -> Menu; balls/capsules/projectiles cleared
    Q_INVOKABLE void moveLeft(bool pressed);
    Q_INVOKABLE void moveRight(bool pressed);
    Q_INVOKABLE void setPointerX(qreal fieldX);   // Ready/Playing only
    Q_INVOKABLE void setPaddleSpeed(int value);   // clamped 1..5, persisted
    Q_INVOKABLE void setAcceleration(bool on);    // persisted
    Q_INVOKABLE void submitInitials(const QString &initials); // GameOver + highScorePending only

    // ---- frame driver (Phase 0): QML calls this every frame with the frame time in seconds ----
    Q_INVOKABLE void tick(qreal dt);

    // ---- test hooks (Phase 0). Not for QML/gameplay code. ----
    void debugSetState(GameState s);                 // forces state, emits gameStateChanged
    void debugLoadLevelRows(const QStringList &rows); // grid + brick model, pace reset for level
    void debugSetBalls(const QVector<Ball> &balls);
    QVector<Ball> debugBalls() const { return m_balls; }
    void debugSetPaddleX(qreal x);                   // via PaddleLogic::setPointerX, then syncModels
    void debugSetPower(PaddleMode mode, int gunAmmo, int lives);
    void debugSetRandom(RandFn rand);                // capsule RNG
    void debugAddCapsule(const Capsule &c);
    QVector<Capsule> debugCapsules() const { return m_capsules.capsules(); }
    QVector<Projectile> debugProjectiles() const { return m_weapons.projectiles(); }
    void debugSetScore(int score);
    void debugSetLevel(int level);
    void debugEnterGameOver() { enterGameOver(); }
    void debugEnterLevelCleared() { enterLevelCleared(); }
    void debugNotifyBrickBroken(int row, int col) { onBrickBroken(row, col); }
    void debugFire() { fireWeapon(); }
    const BrickGrid &debugGrid() const { return m_grid; }
    PaceController &debugPace() { return m_pace; }
    void debugSyncPace() { syncPace(); }

signals:
    // property notifications
    void gameStateChanged();
    void scoreChanged();
    void bestScoreChanged();
    void livesChanged();
    void levelChanged();
    void roundChanged();
    void speedStateChanged(int state);
    void paddleXChanged();
    void paddleWidthChanged();
    void paddleModeChanged(int mode);
    void gunAmmoChanged();
    void boardOffsetRowsChanged();
    void ballCountChanged();
    void inputDirChanged();
    void paddleSpeedChanged();
    void accelerationChanged();
    void highScoresChanged();
    void highScorePendingChanged();
    void storagePathChanged();

    // gameplay events (the motion layer listens to these)
    void brickHit(int row, int col, int hitsLeft, bool unbreakable);
    void brickBroken(int row, int col, int tier);
    void capsuleSpawned(int type, qreal x, qreal y);
    void capsuleCaught(int type);
    void capsuleLost();
    void boardShifted(int steps);
    void projectileFired(int kind, qreal x, qreal y);
    void multiBallActivated();
    void lifeLost();
    void lifeGained();
    void levelCleared();
    void gameOver();

private:
    // ---- task "Engine core" (GameEngine.cpp) ----
    void stepInput(qreal h);          // paddle movement (Ready/Playing); resting ball follows paddle in Ready
    void stepStateTimers(qreal h);    // LevelCleared countdown -> next level
    void loadPersistentData();        // paddleSpeed, acceleration, high scores (defaults if absent)
    void savePersistentData();

    // ---- task "Engine ball step" (GameEngine_balls.cpp) ----
    void stepBalls(qreal h);

    // ---- task "Engine power-ups" (GameEngine_powerups.cpp) ----
    void stepPowerUps(qreal h);
    void onBrickBroken(int row, int col);
    void fireWeapon();
    void applyCapsule(CapsuleType type);

    // ---- Phase 0 helpers (GameEngine_loop.cpp). Use these; do not duplicate them. ----
    void step(qreal h);
    void setState(GameState s);       // no validation; emits gameStateChanged if changed
    bool applyEvent(GameEvent e);     // m_fsm.handle(e); emits gameStateChanged on transition
    void addScore(int points);        // emits scoreChanged (+ bestScoreChanged when it moves)
    void setScoreValue(int score);
    void setLevelNumber(int level);
    void setRoundNumber(int round);
    void syncPower();                 // pushes m_power into the paddle width; emits mode/ammo/width/lives signals that changed
    void syncPace();                  // emits speedStateChanged / boardOffsetRowsChanged / boardShifted(delta > 0)
    void placeBallOnPaddle();         // m_balls = { resting ball at the paddle }
    void loadLevel(int levelNumber);  // grid + brick model + pace.resetForLevel + clearTransient
    void loadRows(const QStringList &rows);
    void clearTransient();            // capsules + projectiles
    void updateBrick(int row, int col); // copy one grid cell into the brick model
    void enterLevelCleared();         // state LevelCleared, timer = LevelClearDelay, balls cleared, emit levelCleared
    void enterGameOver();             // state GameOver, highScorePending = qualifies(score), emit gameOver
    void loseLife();                  // lives-1, lifeLost, powers/pace/transients reset, then Ready+ball or GameOver
    void checkLevelCleared();         // Playing && remainingBreakable()==0 -> enterLevelCleared()
    void syncModels();                // balls/capsules/projectiles models + paddleX/ballCount/inputDir signals
    std::unique_ptr<QSettings> openSettings() const; // nullptr when storagePath is ""

    // ---- state (FROZEN) ----
    GameStateMachine m_fsm;
    PaddleLogic m_paddle;
    BrickGrid m_grid;
    Rng m_rng;
    CapsuleSystem m_capsules;
    WeaponSystem m_weapons;
    PaceController m_pace;
    PowerState m_power;
    HighScoreTable m_highScores;
    QVector<Ball> m_balls;

    int m_score = 0;
    int m_level = 1;
    int m_round = 1;
    bool m_leftHeld = false;
    bool m_rightHeld = false;
    bool m_highScorePending = false;
    qreal m_accumulator = 0;
    qreal m_levelClearTimer = 0;
    QString m_storagePath;

    BrickModel *m_brickModel = nullptr;
    BallModel *m_ballModel = nullptr;
    CapsuleModel *m_capsuleModel = nullptr;
    ProjectileModel *m_projectileModel = nullptr;

    // last-emitted values (change detection for notify signals)
    int m_lastBest = 0;
    qreal m_lastPaddleX = -1;
    int m_lastBallCount = -1;
    int m_lastInputDir = 0;
    int m_lastMode = 0;
    int m_lastAmmo = 0;
    int m_lastLives = K::StartLives;
    int m_lastWidth = K::PaddleNormalW;
    int m_lastSpeedState = 0;
    int m_lastOffset = 0;
};

} // namespace BB
