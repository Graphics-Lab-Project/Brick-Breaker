#pragma once
// OWNER: Phase 0. Read-only. Shared helpers for engine tests.
#include <QtTest>
#include <cmath>

#define BB_NEAR(actual, expected, tol)                                                             \
    do {                                                                                           \
        const double a_ = double(actual), e_ = double(expected);                                   \
        if (!(std::abs(a_ - e_) <= double(tol)))                                                   \
            QFAIL(qPrintable(QStringLiteral("%1 = %2, expected %3 (+/- %4)")                       \
                                 .arg(QStringLiteral(#actual)).arg(a_, 0, 'g', 10)                 \
                                 .arg(e_, 0, 'g', 10).arg(double(tol))));                          \
    } while (0)

namespace TestUtil {
inline double deg(double d) { return d * M_PI / 180.0; }
inline double len(double x, double y) { return std::sqrt(x * x + y * y); }
}

// Fails the test (instead of crashing) when a container is too small to index.
#define BB_REQUIRE_INDEX(container, idx)                                                           \
    QVERIFY2(int((container).size()) > int(idx),                                                   \
             qPrintable(QStringLiteral("%1 has %2 element(s); index %3 needed")                    \
                            .arg(QStringLiteral(#container)).arg(int((container).size()))          \
                            .arg(int(idx))))
