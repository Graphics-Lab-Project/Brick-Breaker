// OWNER: task "Paddle logic". Phase 0 stub: compiles, behaviour missing.
#include "PaddleLogic.h"

#include <algorithm>

namespace BB {

namespace {

// Centre x clamped so the paddle stays fully inside the field.
qreal clampCentre(qreal x, int width)
{
    const qreal half = width / 2.0;
    return std::clamp(x, half, K::FieldW - half);
}

int signOf(qreal v)
{
    return (v > 0) - (v < 0);
}

} // namespace

void PaddleLogic::reset()
{
    m_x = clampCentre(K::PaddleStartX, m_width);
    m_dir = 0;
    m_held = 0;
    m_pointerDir = 0;
    m_curSpeed = 0;
}

void PaddleLogic::setWidth(int w)
{
    m_width = w;
    m_x = clampCentre(m_x, m_width);
}

void PaddleLogic::setSpeedSetting(int s)
{
    m_speedSetting = std::clamp(s, K::MinPaddleSpeedSetting, K::MaxPaddleSpeedSetting);
}

void PaddleLogic::setAcceleration(bool on)
{
    m_accel = on;
}

void PaddleLogic::setInputDir(int dir)
{
    const int s = signOf(qreal(dir));
    if (s != m_dir)
        m_held = 0; // a change of direction restarts the acceleration ramp
    m_dir = s;
}

void PaddleLogic::setPointerX(qreal fieldX)
{
    const qreal before = m_x;
    m_x = clampCentre(fieldX, m_width);
    const int moved = signOf(m_x - before);
    if (moved != 0)
        m_pointerDir = moved; // no movement keeps the previous pointer direction
}

void PaddleLogic::step(qreal dt)
{
    m_held += dt;
    if (m_dir == 0) {
        m_curSpeed = 0;
    } else {
        qreal speed = K::PaddleSpeeds[m_speedSetting - 1];
        if (m_accel)
            speed *= std::min<qreal>(1.0, m_held / K::AccelRampSec);
        m_curSpeed = speed;
        m_x = clampCentre(m_x + m_dir * speed * dt, m_width);
    }
    m_pointerDir = 0;
}

int PaddleLogic::motionDir() const
{
    return m_dir != 0 ? m_dir : m_pointerDir;
}

Rect PaddleLogic::rect() const
{
    return {m_x - m_width / 2.0, K::PaddleTopY, qreal(m_width), K::PaddleH};
}

} // namespace BB
