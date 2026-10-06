// OWNER: task "Pace controller". Phase 0 stub: compiles, behaviour missing.
#include "PaceController.h"

namespace BB {

void PaceController::resetForLevel() {}
void PaceController::resetForLife() {}
PaddleHitOutcome PaceController::onPaddleHit() { return {}; }
qreal PaceController::ballSpeed() const { return 0; }

} // namespace BB
