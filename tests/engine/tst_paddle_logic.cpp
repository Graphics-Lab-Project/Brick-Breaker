// Acceptance tests for task "Paddle logic". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "Constants.h"
#include "PaddleLogic.h"

using namespace BB;

namespace {
void run(PaddleLogic &p, double seconds)
{
    const int steps = int(std::lround(seconds / K::FixedDt));
    for (int i = 0; i < steps; ++i)
        p.step(K::FixedDt);
}
}

class TstPaddleLogic : public QObject {
    Q_OBJECT
private slots:
    void defaults()
    {
        PaddleLogic p;
        p.reset();
        QCOMPARE(p.x(), 168.0);
        QCOMPARE(p.width(), 64);
        QCOMPARE(p.speedSetting(), 3);
        QCOMPARE(p.acceleration(), false);
        QCOMPARE(p.inputDir(), 0);
        QCOMPARE(p.motionDir(), 0);
    }
    void rect()
    {
        PaddleLogic p;
        p.reset();
        const Rect r = p.rect();
        QCOMPARE(r.x, 136.0);
        QCOMPARE(r.y, 322.0);
        QCOMPARE(r.w, 64.0);
        QCOMPARE(r.h, 6.0);
    }
    void movesAtSettingSpeed()
    {
        PaddleLogic p;
        p.reset();
        p.setInputDir(+1);
        run(p, 0.5);                   // setting 3 = 240 px/s -> 120 px
        BB_NEAR(p.x(), 288.0, 0.01);
        BB_NEAR(p.currentSpeed(), 240.0, 0.01);
    }
    void speedSettingTable()
    {
        const double expected[5] = {120, 180, 240, 300, 360};
        for (int s = 1; s <= 5; ++s) {
            PaddleLogic p;
            p.reset();
            p.setSpeedSetting(s);
            p.setInputDir(-1);
            run(p, 0.25);
            BB_NEAR(168.0 - p.x(), expected[s - 1] * 0.25, 0.01);
        }
    }
    void speedSettingClamped()
    {
        PaddleLogic p;
        p.setSpeedSetting(9);
        QCOMPARE(p.speedSetting(), 5);
        p.setSpeedSetting(0);
        QCOMPARE(p.speedSetting(), 1);
        p.setSpeedSetting(-3);
        QCOMPARE(p.speedSetting(), 1);
    }
    void inputDirIsSign()
    {
        PaddleLogic p;
        p.setInputDir(7);
        QCOMPARE(p.inputDir(), 1);
        p.setInputDir(-4);
        QCOMPARE(p.inputDir(), -1);
        p.setInputDir(0);
        QCOMPARE(p.inputDir(), 0);
    }
    void clampsLeft()
    {
        PaddleLogic p;
        p.reset();
        p.setInputDir(-1);
        run(p, 3.0);
        QCOMPARE(p.x(), 32.0);
    }
    void clampsRight()
    {
        PaddleLogic p;
        p.reset();
        p.setInputDir(+1);
        run(p, 3.0);
        QCOMPARE(p.x(), 304.0);
    }
    void widthChangeReclamps()
    {
        PaddleLogic p;
        p.reset();
        p.setInputDir(+1);
        run(p, 3.0);                   // at 304 with width 64
        p.setWidth(96);
        QCOMPARE(p.width(), 96);
        QCOMPARE(p.x(), 288.0);        // 336 - 48
    }
    void zeroDtDoesNothing()
    {
        PaddleLogic p;
        p.reset();
        p.setInputDir(+1);
        p.step(0);
        QCOMPARE(p.x(), 168.0);
    }
    void noInputNoMovement()
    {
        PaddleLogic p;
        p.reset();
        run(p, 1.0);
        QCOMPARE(p.x(), 168.0);
        QCOMPARE(p.currentSpeed(), 0.0);
    }
    void accelerationRamps()
    {
        PaddleLogic p;
        p.reset();
        p.setAcceleration(true);
        QCOMPARE(p.acceleration(), true);
        p.setInputDir(+1);
        p.step(K::FixedDt);
        const double first = p.x() - 168.0;
        QVERIFY2(first > 0.0 && first < 240.0 * K::FixedDt * 0.5, "first step must be well below full speed");
        run(p, 0.3);                    // well past the 0.15 s ramp
        const double before = p.x();
        p.step(K::FixedDt);
        BB_NEAR(p.x() - before, 240.0 * K::FixedDt, 1e-6);
    }
    void accelerationRestartsOnDirectionChange()
    {
        PaddleLogic p;
        p.reset();
        p.setAcceleration(true);
        p.setInputDir(+1);
        run(p, 0.3);
        p.setInputDir(-1);
        const double before = p.x();
        p.step(K::FixedDt);
        QVERIFY2(before - p.x() < 240.0 * K::FixedDt * 0.5, "ramp must restart after reversing");
    }
    void accelerationOffIsInstant()
    {
        PaddleLogic p;
        p.reset();
        p.setInputDir(+1);
        p.step(K::FixedDt);
        BB_NEAR(p.x() - 168.0, 240.0 * K::FixedDt, 1e-9);
    }
    void pointerAbsoluteAndClamped()
    {
        PaddleLogic p;
        p.reset();
        p.setPointerX(200);
        QCOMPARE(p.x(), 200.0);
        p.setPointerX(-50);
        QCOMPARE(p.x(), 32.0);
        p.setPointerX(1000);
        QCOMPARE(p.x(), 304.0);
    }
    void motionDir_keysThenPointer()
    {
        PaddleLogic p;
        p.reset();
        p.setInputDir(-1);
        QCOMPARE(p.motionDir(), -1);
        p.setInputDir(0);
        QCOMPARE(p.motionDir(), 0);
        p.setPointerX(200);             // moved right
        QCOMPARE(p.motionDir(), 1);
        p.step(K::FixedDt);             // pointer direction cleared by step
        QCOMPARE(p.motionDir(), 0);
        p.setPointerX(100);
        QCOMPARE(p.motionDir(), -1);
        p.setPointerX(100);             // no movement -> keeps -1 until next step
        QCOMPARE(p.motionDir(), -1);
    }
    void resetRestoresStart()
    {
        PaddleLogic p;
        p.setPointerX(50);
        p.setInputDir(1);
        p.reset();
        QCOMPARE(p.x(), 168.0);
        QCOMPARE(p.inputDir(), 0);
        QCOMPARE(p.motionDir(), 0);
    }
};

QTEST_APPLESS_MAIN(TstPaddleLogic)
#include "tst_paddle_logic.moc"
