#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "Pace controller" -> PaceController.cpp
#include "Constants.h"
#include "Types.h"

namespace BB {

struct PaddleHitOutcome {
    bool becameFast = false;
    bool descended = false;
};

// Tracks paddle hits -> Slow/Fast ball speed, and the wall descent while Fast.
class PaceController {
public:
    void resetForLevel();   // hits 0, Slow, boardOffsetRows 0
    void resetForLife();    // hits 0, Slow, boardOffsetRows unchanged
    // hits += 1. If the state was Slow and hits reached FastAfterPaddleHits -> Fast (becameFast).
    // If the state was ALREADY Fast before this hit and offset < MaxDescentRows -> offset += 1 (descended).
    PaddleHitOutcome onPaddleHit();
    qreal ballSpeed() const;  // SlowSpeed or FastSpeed

    SpeedState speedState() const { return m_state; }
    int paddleHits() const { return m_hits; }
    int boardOffsetRows() const { return m_offset; }

private:
    int m_hits = 0;
    SpeedState m_state = SpeedState::Slow;
    int m_offset = 0;
};

} // namespace BB
