// OWNER: task "Power-up rules".
#include "PowerUps.h"

namespace BB::PowerUps {

CapsuleEffect apply(PowerState &s, CapsuleType type)
{
    CapsuleEffect e;
    const PaddleMode before = s.mode;

    switch (type) {
    case CapsuleType::Life:
        if (s.lives < K::MaxLives) {
            s.lives = s.lives + 1;
            e.lifeGained = true;
        }
        s.mode = PaddleMode::Normal;
        s.gunAmmo = 0;
        break;
    case CapsuleType::Multi:
        e.multiBall = true;
        break;
    case CapsuleType::Long:
        s.mode = PaddleMode::Long;
        s.gunAmmo = 0;
        break;
    case CapsuleType::Gun:
        s.mode = PaddleMode::Gun;
        s.gunAmmo = K::GunAmmo;
        break;
    case CapsuleType::Laser:
        s.mode = PaddleMode::Laser;
        s.gunAmmo = 0;
        break;
    }

    e.modeChanged = (s.mode != before);
    return e;
}

bool resetForLifeLost(PowerState &s)
{
    const bool changed = (s.mode != PaddleMode::Normal);
    s.mode = PaddleMode::Normal;
    s.gunAmmo = 0;
    return changed;
}

} // namespace BB::PowerUps
