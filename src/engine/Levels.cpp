// OWNER: task "Brick grid + levels". Phase 0 stub: compiles, behaviour missing.
#include "Levels.h"

#include <array>

namespace BB::Levels {

namespace {
// Layouts from docs/DESIGN_HANDOFF.md section 7. Levels 2-10 are approved placeholders.
const std::array<QStringList, 10> kLayouts = {{
    {".......", ".11.11.", ".11.21.", ".......", ".21.21.", ".11.11.", ".......", ".12.11.", ".11.11."},
    {".......", "1111111", ".......", "2222222", ".......", "1111111"},
    {".......", "...2...", "..212..", ".11211.", "1112111"},
    {".......", ".1.1.1.", "1.1.1.1", ".2.2.2.", "1.1.1.1", ".1.1.1."},
    {".......", "2.2.2.2", "1.1.1.1", "1.1.1.1", "1.1.1.1", "2.2.2.2"},
    {"...2...", "..121..", ".12221.", "..121..", "...2..."},
    {"2222222", "2.....2", "2.111.2", "2.....2", "2222222"},
    {"3333333", "1111111", "2222222", "1111111"},
    {".......", ".33333.", "S22122S", "S12321S", "S11111S", "SSS.SSS"},
    {".......", "1S1S1S1", "2121212", "2S2S2S2", "1111111", ".S.S.S.", "3.3.3.3"},
}};
} // namespace

int count()
{
    return static_cast<int>(kLayouts.size());
}

QStringList rows(int levelNumber)
{
    const int n = levelNumber < 1 ? 1 : levelNumber;
    return kLayouts[static_cast<std::size_t>((n - 1) % count())];
}

} // namespace BB::Levels
