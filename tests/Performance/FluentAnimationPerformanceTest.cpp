// tests/Performance/FluentAnimationPerformanceTest.cpp
#include <QApplication>
#include <QElapsedTimer>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QtTest/QtTest>
#include "FluentQt/Animation/FluentAnimationPerformanceManager.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentGPUAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Core/FluentPerformance.h"

using namespace FluentQt::Animation;
using namespace FluentQt::Components;
using FluentQt::Core::FluentPerformanceMonitor;

class FluentAnimationPerformanceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Animation performance tests
    void testConcurrentAnimationPerformance();
    void testAnimationBatchingEfficiency();
    void testGPUAccelerationPerformance();
    void testAnimationMemoryUsage();
    void testFrameRateOptimization();
    void testAnimationQueueManagement();
    void testPerformanceAdaptation();
    void testAnimationCaching();

private:
    QWidget* m_testWidget{nullptr};
    FluentPerformanceMonitor* m_monitor{nullptr};
    FluentAnimationPerformanceManager* m_animationManager{nullptr};

    // Helper methods
    void createTestButtons(int count, QList<FluentButton*>& buttons);
    void measureAnimationPerformance(
        const QList<QAbstractAnimation*>& animations);
    void simulateHighLoadScenario();
};

void FluentAnimationPerformanceTest::initTestCase() {
    m_monitor = &FluentPerformanceMonitor::instance();
    m_monitor->startFrameRateMonitoring();

    m_animationManager = &FluentAnimationPerformanceManager::instance();
    m_animationManager->startPerformanceMonitoring();
}

void FluentAnimationPerformanceTest::cleanupTestCase() {
    m_monitor->stopFrameRateMonitoring();
    m_animationManager->stopPerformanceMonitoring();
}

void FluentAnimationPerformanceTest::init() {
    m_testWidget = new QWidget;
    m_testWidget->resize(1000, 800);
}

void FluentAnimationPerformanceTest::cleanup() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentAnimationPerformanceTest::testConcurrentAnimationPerformance() {
    const int buttonCount = 50;
    QList<FluentButton*> buttons;
    createTestButtons(buttonCount, buttons);

    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);

    // Test without batching
    m_animationManager->setOptimizationStrategy(
        FluentOptimizationStrategy::BatchAnimations, false);

    QElapsedTimer timer;
    timer.start();

    QList<QAbstractAnimation*> animations;
    for (auto* button : buttons) {
        auto animation = FluentAnimator::fadeIn(button);
        animations.append(animation.release());
    }

    // Start all animations simultaneously
    for (auto* animation : animations) {
        animation->start();
    }

    // Wait for all animations to complete
    QTest::qWait(500);
    qint64 unbatchedTime = timer.elapsed();

    // Clean up
    qDeleteAll(animations);
    animations.clear();

    // Test with batching enabled
    m_animationManager->setOptimizationStrategy(
        FluentOptimizationStrategy::BatchAnimations, true);

    timer.restart();
    for (auto* button : buttons) {
        auto animation = FluentAnimator::fadeIn(button);
        animations.append(animation.release());
    }

    // Start all animations simultaneously
    for (auto* animation : animations) {
        animation->start();
    }

    QTest::qWait(500);
    qint64 batchedTime = timer.elapsed();

    qDebug() << "Concurrent animation performance:";
    qDebug() << "  Unbatched:" << unbatchedTime << "ms";
    qDebug() << "  Batched:" << batchedTime << "ms";
    qDebug() << "  Improvement:"
             << QString::number((double)unbatchedTime / batchedTime, 'f', 2)
             << "x";

    // Batching should improve performance
    QVERIFY(batchedTime <= unbatchedTime);

    // Clean up
    qDeleteAll(animations);
    qDeleteAll(buttons);
}

void FluentAnimationPerformanceTest::testAnimationBatchingEfficiency() {
    const int animationCount = 100;
    QList<FluentButton*> buttons;
    createTestButtons(animationCount / 4, buttons);  // 4 animations per button

    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);

    // Enable batching
    m_animationManager->setOptimizationStrategy(
        FluentOptimizationStrategy::BatchAnimations, true);

    QElapsedTimer timer;
    timer.start();

    // Create multiple animations for each button (should be batched)
    QList<QAbstractAnimation*> animations;
    for (auto* button : buttons) {
        animations.append(FluentAnimator::fadeIn(button).release());
        animations.append(FluentAnimator::scaleIn(button).release());
        animations.append(
            FluentAnimator::slideUp(button, QPoint(0, 20)).release());
        animations.append(FluentAnimator::rotateIn(button, 360).release());
    }

    // Start all animations
    for (auto* animation : animations) {
        animation->start();
    }

    QTest::qWait(600);
    qint64 batchingTime = timer.elapsed();

    auto metrics = m_animationManager->getCurrentMetrics();

    qDebug() << "Animation batching efficiency:";
    qDebug() << "  Total animations:" << animationCount;
    qDebug() << "  Execution time:" << batchingTime << "ms";
    qDebug() << "  Active animations:" << metrics.activeAnimations;
    qDebug() << "  Queued animations:" << metrics.queuedAnimations;
    qDebug() << "  Performance score:" << metrics.performanceScore;

    // Batching should keep active animations reasonable
    QVERIFY(metrics.activeAnimations <= 50);  // Should batch similar animations

    // Clean up
    qDeleteAll(animations);
    qDeleteAll(buttons);
}

void FluentAnimationPerformanceTest::testGPUAccelerationPerformance() {
    // Skip if GPU acceleration not available
    if (!FluentGPURenderer::isCapabilitySupported(
            FluentGPUCapability::OpenGL_3_3)) {
        QSKIP("GPU acceleration not available");
    }

    const int buttonCount = 20;
    QList<FluentButton*> buttons;
    createTestButtons(buttonCount, buttons);

    auto* gpuRenderer = new FluentGPURenderer(m_testWidget);
    auto* gpuAnimator = new FluentGPUAnimator(this);
    gpuAnimator->setRenderer(gpuRenderer);

    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);

    // Test CPU-based animations
    QElapsedTimer timer;
    timer.start();

    QList<QAbstractAnimation*> cpuAnimations;
    for (auto* button : buttons) {
        auto animation = FluentAnimator::fadeIn(button);
        cpuAnimations.append(animation.release());
    }

    for (auto* animation : cpuAnimations) {
        animation->start();
    }

    QTest::qWait(400);
    qint64 cpuTime = timer.elapsed();

    // Test GPU-accelerated animations
    timer.restart();

    for (auto* button : buttons) {
        FluentAnimationConfig config;
        config.useHardwareAcceleration = true;
        gpuAnimator->animateBlur(button, 0.0f, 5.0f, config);
    }

    QTest::qWait(400);
    qint64 gpuTime = timer.elapsed();

    qDebug() << "GPU acceleration performance:";
    qDebug() << "  CPU animations:" << cpuTime << "ms";
    qDebug() << "  GPU animations:" << gpuTime << "ms";

    if (gpuTime < cpuTime) {
        double improvement = (double)cpuTime / gpuTime;
        qDebug() << "  GPU improvement:" << QString::number(improvement, 'f', 2)
                 << "x";
        QVERIFY(improvement >= 1.2);  // At least 20% improvement
    }

    // Clean up
    qDeleteAll(cpuAnimations);
    qDeleteAll(buttons);
    delete gpuRenderer;
}

void FluentAnimationPerformanceTest::testAnimationMemoryUsage() {
    size_t initialMemory = m_monitor->currentMemoryUsage();

    const int animationCount = 200;
    QList<FluentButton*> buttons;
    QList<QAbstractAnimation*> animations;

    createTestButtons(animationCount / 4, buttons);

    // Create many animations
    for (auto* button : buttons) {
        animations.append(FluentAnimator::fadeIn(button).release());
        animations.append(FluentAnimator::scaleIn(button).release());
        animations.append(
            FluentAnimator::slideUp(button, QPoint(0, 10)).release());
        animations.append(FluentAnimator::rotateIn(button, 180).release());
    }

    size_t memoryWithAnimations = m_monitor->currentMemoryUsage();

    // Start animations
    for (auto* animation : animations) {
        animation->start();
    }

    QTest::qWait(100);  // Let animations run briefly
    size_t memoryDuringAnimations = m_monitor->currentMemoryUsage();

    // Stop and clean up animations
    for (auto* animation : animations) {
        animation->stop();
        delete animation;
    }
    animations.clear();

    size_t memoryAfterCleanup = m_monitor->currentMemoryUsage();

    qDebug() << "Animation memory usage:";
    qDebug() << "  Initial memory:" << initialMemory << "bytes";
    qDebug() << "  With animations:" << memoryWithAnimations << "bytes";
    qDebug() << "  During animations:" << memoryDuringAnimations << "bytes";
    qDebug() << "  After cleanup:" << memoryAfterCleanup << "bytes";

    size_t animationOverhead = memoryWithAnimations - initialMemory;
    size_t memoryPerAnimation = animationOverhead / animationCount;

    qDebug() << "  Memory per animation:" << memoryPerAnimation << "bytes";

    // Memory usage should be reasonable
    QVERIFY(memoryPerAnimation < 5000);  // Less than 5KB per animation

    // Memory should be cleaned up properly
    size_t memoryLeak = memoryAfterCleanup - initialMemory;
    QVERIFY(memoryLeak < animationOverhead * 0.1);  // Less than 10% leak

    qDeleteAll(buttons);
}

void FluentAnimationPerformanceTest::testFrameRateOptimization() {
    const int buttonCount = 30;
    QList<FluentButton*> buttons;
    createTestButtons(buttonCount, buttons);

    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);

    // Enable adaptive frame rate optimization
    m_animationManager->setOptimizationStrategy(
        FluentOptimizationStrategy::AdaptiveFrameRate, true);

    double initialFrameRate = m_monitor->currentFrameRate();

    // Start many concurrent animations
    QList<QAbstractAnimation*> animations;
    for (auto* button : buttons) {
        animations.append(FluentAnimator::fadeIn(button).release());
        animations.append(FluentAnimator::scaleIn(button).release());
    }

    for (auto* animation : animations) {
        animation->start();
    }

    // Let animations run and measure frame rate
    QTest::qWait(200);
    double frameRateDuringAnimations = m_monitor->currentFrameRate();

    QTest::qWait(400);  // Let animations complete
    double finalFrameRate = m_monitor->currentFrameRate();

    qDebug() << "Frame rate optimization:";
    qDebug() << "  Initial frame rate:" << initialFrameRate << "fps";
    qDebug() << "  During animations:" << frameRateDuringAnimations << "fps";
    qDebug() << "  Final frame rate:" << finalFrameRate << "fps";

    // Frame rate should remain reasonable during animations
    QVERIFY(frameRateDuringAnimations >= 30.0);  // At least 30 FPS

    // Frame rate should recover after animations
    QVERIFY(finalFrameRate >= initialFrameRate * 0.9);  // Within 10% of initial

    // Clean up
    qDeleteAll(animations);
    qDeleteAll(buttons);
}

void FluentAnimationPerformanceTest::createTestButtons(
    int count, QList<FluentButton*>& buttons) {
    for (int i = 0; i < count; ++i) {
        auto* button =
            new FluentButton(QString("Button %1").arg(i), m_testWidget);
        button->setGeometry(10 + (i % 10) * 100, 10 + (i / 10) * 50, 90, 35);
        buttons.append(button);
    }
}

QTEST_MAIN(FluentAnimationPerformanceTest)
#include "FluentAnimationPerformanceTest.moc"
