// OWNER: task "Pace controller". Phase 0 stub: compiles, behaviour missing.
#include "PaceController.h"

namespace BB {

void PaceController::resetForLevel()
{
    m_hits = 0;
    m_state = SpeedState::Slow;
    m_offset = 0;
}

void PaceController::resetForLife()
{
    m_hits = 0;
    m_state = SpeedState::Slow;
}

PaddleHitOutcome PaceController::onPaddleHit()
{
    PaddleHitOutcome outcome;
    const bool wasFast = (m_state == SpeedState::Fast);

    m_hits += 1;

    if (!wasFast && m_hits >= K::FastAfterPaddleHits) {
        m_state = SpeedState::Fast;
        outcome.becameFast = true;
    }

    if (wasFast && m_offset < K::MaxDescentRows) {
        m_offset += 1;
        outcome.descended = true;
    }

    return outcome;
}

qreal PaceController::ballSpeed() const
{
    return m_state == SpeedState::Fast ? K::FastSpeed : K::SlowSpeed;
}

} // namespace BB
