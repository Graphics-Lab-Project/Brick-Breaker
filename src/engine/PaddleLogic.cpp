// OWNER: task "Paddle logic". Phase 0 stub: compiles, behaviour missing.
#include "PaddleLogic.h"

namespace BB {

void PaddleLogic::reset() {}
void PaddleLogic::setWidth(int) {}
void PaddleLogic::setSpeedSetting(int) {}
void PaddleLogic::setAcceleration(bool) {}
void PaddleLogic::setInputDir(int) {}
void PaddleLogic::setPointerX(qreal) {}
void PaddleLogic::step(qreal) {}
int PaddleLogic::motionDir() const { return 0; }
Rect PaddleLogic::rect() const { return {}; }

} // namespace BB
