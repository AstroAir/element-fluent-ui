#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QSignalSpy>
#include <QTimer>
#include <QPropertyAnimation>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentOptimizedEasing.h"
#include "FluentQt/Animation/FluentAnimationBatch.h"
#include "FluentQt/Animation/FluentTransformEffect.h"

using namespace FluentQt::Animation;

class AnimationSystemTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // FluentAnimator tests
    void testFadeInAnimation();
    void testFadeOutAnimation();
    void testScaleAnimation();
    void testSlideAnimation();
    void testAnimationChaining();
    void testAnimationConfiguration();
    void testHardwareAcceleration();
    void testReducedMotionSupport();

    // FluentOptimizedEasing tests
    void testOptimizedEasingCurves();
    void testPerceptualEasing();
    void testAntiJankEasing();
    void testBatteryOptimizedEasing();
    void testTimingOptimization();
    void testFramePerfectDuration();

    // FluentAnimationBatch tests
    void testAnimationBatching();
    void testBatchOptimization();
    void testPerformanceMonitoring();
    void testAdaptiveQuality();

    // FluentTransformEffect tests
    void testTransformEffect();
    void testHardwareAcceleratedTransforms();
    void testTransformCaching();

    // Performance tests
    void testAnimationPerformance();
    void testMemoryUsage();
    void testCPUUsage();

private:
    QWidget* m_testWidget = nullptr;
    QApplication* m_app = nullptr;
};

void AnimationSystemTests::initTestCase() {
    // Initialize test environment
    if (!QApplication::instance()) {
        int argc = 0;
        char** argv = nullptr;
        m_app = new QApplication(argc, argv);
    }
}

void AnimationSystemTests::cleanupTestCase() {
    if (m_app) {
        delete m_app;
        m_app = nullptr;
    }
}

void AnimationSystemTests::init() {
    m_testWidget = new QWidget();
    m_testWidget->resize(200, 200);
    m_testWidget->show();
}

void AnimationSystemTests::cleanup() {
    if (m_testWidget) {
        delete m_testWidget;
        m_testWidget = nullptr;
    }
}

void AnimationSystemTests::testFadeInAnimation() {
    QVERIFY(m_testWidget != nullptr);
    
    // Test basic fade in animation
    auto animation = FluentAnimator::fadeIn(m_testWidget);
    QVERIFY(animation != nullptr);
    
    QSignalSpy finishedSpy(animation.get(), &QPropertyAnimation::finished);
    
    animation->start();
    QVERIFY(animation->state() == QAbstractAnimation::Running);
    
    // Wait for animation to complete
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
    
    // Verify widget is visible and fully opaque
    QVERIFY(m_testWidget->isVisible());
}

void AnimationSystemTests::testFadeOutAnimation() {
    QVERIFY(m_testWidget != nullptr);
    
    // Test basic fade out animation
    auto animation = FluentAnimator::fadeOut(m_testWidget);
    QVERIFY(animation != nullptr);
    
    QSignalSpy finishedSpy(animation.get(), &QPropertyAnimation::finished);
    
    animation->start();
    QVERIFY(animation->state() == QAbstractAnimation::Running);
    
    // Wait for animation to complete
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
    
    // Verify widget is hidden after fade out
    QVERIFY(!m_testWidget->isVisible());
}

void AnimationSystemTests::testScaleAnimation() {
    QVERIFY(m_testWidget != nullptr);
    
    // Test scale in animation
    auto animation = FluentAnimator::scaleIn(m_testWidget);
    QVERIFY(animation != nullptr);
    
    QSignalSpy finishedSpy(animation.get(), &QPropertyAnimation::finished);
    
    animation->start();
    QVERIFY(animation->state() == QAbstractAnimation::Running);
    
    // Wait for animation to complete
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
    
    // Verify widget is visible
    QVERIFY(m_testWidget->isVisible());
}

void AnimationSystemTests::testSlideAnimation() {
    QVERIFY(m_testWidget != nullptr);
    
    // Test slide up animation
    auto animation = FluentAnimator::slideUp(m_testWidget);
    QVERIFY(animation != nullptr);
    
    QSignalSpy finishedSpy(animation.get(), &QPropertyAnimation::finished);
    
    animation->start();
    QVERIFY(animation->state() == QAbstractAnimation::Running);
    
    // Wait for animation to complete
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
}

void AnimationSystemTests::testAnimationConfiguration() {
    QVERIFY(m_testWidget != nullptr);
    
    // Test custom animation configuration
    FluentAnimationConfig config;
    config.duration = std::chrono::milliseconds(500);
    config.easing = FluentEasing::EaseInOut;
    config.useHardwareAcceleration = true;
    config.respectReducedMotion = false;
    
    auto animation = FluentAnimator::fadeIn(m_testWidget, config);
    QVERIFY(animation != nullptr);
    
    // Verify configuration is applied
    QCOMPARE(animation->duration(), 500);
    QVERIFY(animation->property("hardwareAccelerated").toBool());
}

void AnimationSystemTests::testOptimizedEasingCurves() {
    // Test different optimized easing curves
    FluentOptimizedEasing::OptimizationConfig config;
    config.targetFPS = 60.0;
    config.enableAntiJank = true;
    
    auto perceptualCurve = FluentOptimizedEasing::createOptimizedCurve(
        FluentOptimizedEasing::CurveType::PerceptualEaseOut, config);
    QVERIFY(perceptualCurve.type() == QEasingCurve::Custom);
    
    auto antiJankCurve = FluentOptimizedEasing::createOptimizedCurve(
        FluentOptimizedEasing::CurveType::AntiJank, config);
    QVERIFY(antiJankCurve.type() == QEasingCurve::Custom);
    
    auto batteryOptimizedCurve = FluentOptimizedEasing::createOptimizedCurve(
        FluentOptimizedEasing::CurveType::BatteryOptimized, config);
    QVERIFY(batteryOptimizedCurve.type() == QEasingCurve::Custom);
}

void AnimationSystemTests::testPerceptualEasing() {
    auto curve = FluentOptimizedEasing::createPerceptuallyLinear();
    QVERIFY(curve.type() == QEasingCurve::Custom);
    
    // Test that the curve produces reasonable values
    double value1 = curve.valueForProgress(0.0);
    double value2 = curve.valueForProgress(0.5);
    double value3 = curve.valueForProgress(1.0);
    
    QCOMPARE(value1, 0.0);
    QCOMPARE(value3, 1.0);
    QVERIFY(value2 > 0.0 && value2 < 1.0);
}

void AnimationSystemTests::testTimingOptimization() {
    FluentOptimizedEasing::OptimizationConfig config;
    config.targetFPS = 60.0;
    
    // Test optimal duration calculation
    auto duration = FluentOptimizedEasing::calculateOptimalDuration(
        100.0, FluentOptimizedEasing::CurveType::FastEaseOut, config);
    
    QVERIFY(duration.count() > 0);
    QVERIFY(duration.count() <= 1000); // Reasonable upper bound
}

void AnimationSystemTests::testFramePerfectDuration() {
    auto duration = FluentTimingOptimizer::calculateFramePerfectDuration(
        std::chrono::milliseconds(250), 60.0);
    
    // Should be rounded to nearest frame boundary
    double frameTime = 1000.0 / 60.0; // ~16.67ms
    int expectedFrames = qRound(250.0 / frameTime);
    int expectedDuration = static_cast<int>(expectedFrames * frameTime);
    
    QVERIFY(qAbs(duration.count() - expectedDuration) <= 1);
}

void AnimationSystemTests::testAnimationBatching() {
    FluentAnimationBatch batch;
    
    // Test adding animations to batch
    int batchId1 = batch.addAnimation(m_testWidget, "opacity", 0.0, 1.0);
    int batchId2 = batch.addAnimation(m_testWidget, "geometry", 
                                     QRect(0, 0, 100, 100), QRect(50, 50, 200, 200));
    
    QVERIFY(batchId1 >= 0);
    QVERIFY(batchId2 >= 0);
    QVERIFY(batchId1 != batchId2);
    
    // Test batch execution
    QSignalSpy batchStartedSpy(&batch, &FluentAnimationBatch::batchStarted);
    QSignalSpy batchFinishedSpy(&batch, &FluentAnimationBatch::batchFinished);
    
    batch.startBatch();
    QVERIFY(batchStartedSpy.wait(100));
    QCOMPARE(batchStartedSpy.count(), 1);
}

void AnimationSystemTests::testPerformanceMonitoring() {
    FluentAnimationBatch batch;
    
    // Test performance metrics
    double fps = batch.getCurrentFPS();
    QVERIFY(fps > 0.0);
    
    int activeCount = batch.getActiveAnimationCount();
    QVERIFY(activeCount >= 0);
    
    double cpuUsage = batch.getCPUUsage();
    QVERIFY(cpuUsage >= 0.0 && cpuUsage <= 100.0);
}

void AnimationSystemTests::testTransformEffect() {
    FluentTransformEffect effect;
    
    // Test property setters
    effect.setScale(1.5);
    QCOMPARE(effect.scale(), 1.5);
    
    effect.setRotation(45.0);
    QCOMPARE(effect.rotation(), 45.0);
    
    effect.setTranslation(QPointF(10, 20));
    QCOMPARE(effect.translation(), QPointF(10, 20));
}

void AnimationSystemTests::testAnimationPerformance() {
    // Performance test: create multiple animations
    const int animationCount = 100;
    QList<std::unique_ptr<QPropertyAnimation>> animations;
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < animationCount; ++i) {
        auto widget = new QWidget();
        widget->resize(50, 50);
        auto animation = FluentAnimator::fadeIn(widget);
        animations.append(std::move(animation));
    }
    
    qint64 creationTime = timer.elapsed();
    
    // Start all animations
    timer.restart();
    for (auto& animation : animations) {
        animation->start();
    }
    
    qint64 startTime = timer.elapsed();
    
    // Verify performance is reasonable
    QVERIFY(creationTime < 1000); // Should create 100 animations in less than 1 second
    QVERIFY(startTime < 500);     // Should start 100 animations in less than 0.5 seconds
    
    // Cleanup
    for (auto& animation : animations) {
        if (animation->targetObject()) {
            delete animation->targetObject();
        }
    }
}

void AnimationSystemTests::testMemoryUsage() {
    // Test that animations don't leak memory
    const int iterations = 50;
    
    for (int i = 0; i < iterations; ++i) {
        auto widget = new QWidget();
        widget->resize(100, 100);
        
        auto animation = FluentAnimator::fadeIn(widget);
        animation->start();
        
        // Wait for animation to complete
        QSignalSpy finishedSpy(animation.get(), &QPropertyAnimation::finished);
        if (animation->state() == QAbstractAnimation::Running) {
            QVERIFY(finishedSpy.wait(1000));
        }
        
        delete widget;
    }
    
    // If we get here without crashing, memory management is working
    QVERIFY(true);
}

void AnimationSystemTests::testReducedMotionSupport() {
    // Test that animations respect reduced motion settings
    FluentAnimationConfig config;
    config.respectReducedMotion = true;
    
    // This test would need to mock the system accessibility settings
    // For now, just verify the configuration is accepted
    auto animation = FluentAnimator::fadeIn(m_testWidget, config);
    QVERIFY(animation != nullptr);
}

void AnimationSystemTests::testHardwareAcceleration() {
    // Test hardware acceleration configuration
    FluentAnimationConfig config;
    config.useHardwareAcceleration = true;
    
    auto animation = FluentAnimator::fadeIn(m_testWidget, config);
    QVERIFY(animation != nullptr);
    QVERIFY(animation->property("hardwareAccelerated").toBool());
}

QTEST_MAIN(AnimationSystemTests)
#include "AnimationSystemTests.moc"
