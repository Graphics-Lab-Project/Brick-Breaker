// OWNER: task "Engine core". Phase 0 stub: compiles, behaviour missing.
// Implement: all Q_INVOKABLEs, stepInput, stepStateTimers, loadPersistentData, savePersistentData.
#include "GameEngine.h"

namespace BB {

void GameEngine::startGame() {}
void GameEngine::launchOrFire() {}
void GameEngine::togglePause() {}
void GameEngine::quitToMenu() {}
void GameEngine::moveLeft(bool) {}
void GameEngine::moveRight(bool) {}
void GameEngine::setPointerX(qreal) {}
void GameEngine::setPaddleSpeed(int) {}
void GameEngine::setAcceleration(bool) {}
void GameEngine::submitInitials(const QString &) {}

void GameEngine::stepInput(qreal) {}
void GameEngine::stepStateTimers(qreal) {}
void GameEngine::loadPersistentData() {}
void GameEngine::savePersistentData() {}

} // namespace BB
