#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "Paddle logic" -> PaddleLogic.cpp
#include "Constants.h"
#include "Types.h"

namespace BB {

class PaddleLogic {
public:
    // Centre x in field coordinates; always within [width/2, FieldW - width/2].
    qreal x() const { return m_x; }
    int width() const { return m_width; }
    int inputDir() const { return m_dir; }
    int speedSetting() const { return m_speedSetting; }
    bool acceleration() const { return m_accel; }
    qreal currentSpeed() const { return m_curSpeed; } // px/s used in the last step()

    // Back to PaddleStartX (clamped), input released, ramp reset, pointer dir cleared.
    void reset();
    // Sets width (64 or 96 in practice) and re-clamps x.
    void setWidth(int w);
    // Clamped to [1, 5].
    void setSpeedSetting(int s);
    void setAcceleration(bool on);
    // dir is reduced to its sign (-1, 0, +1). A change of direction restarts the ramp.
    void setInputDir(int dir);
    // Absolute pointer control: x jumps to fieldX (clamped). Records the sign of the
    // movement as the pointer direction until the next step().
    void setPointerX(qreal fieldX);
    // Moves by inputDir * speed * dt, then clamps. speed = PaddleSpeeds[setting-1],
    // scaled by min(1, heldTime / AccelRampSec) when acceleration is on (heldTime is
    // increased by dt BEFORE computing speed). Clears the pointer direction at the end.
    void step(qreal dt);
    // inputDir if non-zero, else the pointer direction recorded since the last step(), else 0.
    int motionDir() const;
    // (x - width/2, PaddleTopY, width, PaddleH)
    Rect rect() const;

private:
    qreal m_x = K::PaddleStartX;
    int m_width = K::PaddleNormalW;
    int m_speedSetting = K::DefaultPaddleSpeedSetting;
    bool m_accel = false;
    int m_dir = 0;
    qreal m_held = 0;
    int m_pointerDir = 0;
    qreal m_curSpeed = 0;
};

} // namespace BB
