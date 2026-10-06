// OWNER: task "Power-up rules". Phase 0 stub: compiles, behaviour missing.
#include "PowerUps.h"

namespace BB::PowerUps {

CapsuleEffect apply(PowerState &, CapsuleType) { return {}; }
bool resetForLifeLost(PowerState &) { return false; }

} // namespace BB::PowerUps
