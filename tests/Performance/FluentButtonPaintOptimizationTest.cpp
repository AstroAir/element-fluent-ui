// tests/Performance/FluentButtonPaintOptimizationTest.cpp
#include <QApplication>
#include <QElapsedTimer>
#include <QPainter>
#include <QPixmap>
#include <QtTest/QtTest>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Core/FluentPerformance.h"

using namespace FluentQt::Components;
using FluentQt::Core::FluentPerformanceMonitor;

class FluentButtonPaintOptimizationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Performance tests
    void testPaintEventPerformance();
    void testCacheEffectiveness();
    void testMemoryUsage();
    void testConcurrentButtonRendering();
    void testCacheInvalidation();
    void testStyleChangePerformance();

private:
    QWidget* m_testWidget{nullptr};
    FluentPerformanceMonitor* m_monitor{nullptr};

    // Helper methods
    void createTestButtons(int count, QList<FluentButton*>& buttons);
    void measurePaintTime(FluentButton* button, int iterations = 100);
    void simulateUserInteraction(FluentButton* button);
};

void FluentButtonPaintOptimizationTest::initTestCase() {
    m_monitor = &FluentPerformanceMonitor::instance();
    m_monitor->startFrameRateMonitoring();
}

void FluentButtonPaintOptimizationTest::cleanupTestCase() {
    m_monitor->stopFrameRateMonitoring();
}

void FluentButtonPaintOptimizationTest::init() {
    m_testWidget = new QWidget;
    m_testWidget->resize(800, 600);
}

void FluentButtonPaintOptimizationTest::cleanup() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentButtonPaintOptimizationTest::testPaintEventPerformance() {
    // Create a button with complex styling
    auto* button = new FluentButton("Performance Test Button", m_testWidget);
    button->setButtonStyle(FluentButtonStyle::Primary);
    button->setIcon(QIcon(":/icons/test.png"));
    button->resize(200, 40);
    button->show();

    QTest::qWaitForWindowExposed(m_testWidget);

    // Measure paint performance with caching
    QElapsedTimer timer;
    const int iterations = 1000;

    timer.start();
    for (int i = 0; i < iterations; ++i) {
        // Force a paint event
        button->update();
        QApplication::processEvents();
    }
    qint64 cachedTime = timer.elapsed();

    // Disable caching by invalidating cache on each paint
    timer.restart();
    for (int i = 0; i < iterations; ++i) {
        button->invalidateCache();  // Force cache miss
        button->update();
        QApplication::processEvents();
    }
    qint64 uncachedTime = timer.elapsed();

    qDebug() << "Paint performance comparison:";
    qDebug() << "  Cached rendering:" << cachedTime << "ms for" << iterations
             << "iterations";
    qDebug() << "  Uncached rendering:" << uncachedTime << "ms for"
             << iterations << "iterations";
    qDebug() << "  Performance improvement:"
             << QString::number((double)uncachedTime / cachedTime, 'f', 2)
             << "x";

    // Cached rendering should be significantly faster
    QVERIFY(cachedTime < uncachedTime);

    // We expect at least 2x improvement with caching
    double improvement = (double)uncachedTime / cachedTime;
    QVERIFY(improvement >= 2.0);
}

void FluentButtonPaintOptimizationTest::testCacheEffectiveness() {
    auto* button = new FluentButton("Cache Test", m_testWidget);
    button->setButtonStyle(FluentButtonStyle::Accent);
    button->resize(150, 35);
    button->show();

    // Initial paint should populate cache
    button->update();
    QApplication::processEvents();

    // Subsequent paints should use cache (measure time)
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 100; ++i) {
        button->update();
        QApplication::processEvents();
    }

    qint64 cacheHitTime = timer.elapsed();

    // Now invalidate cache and measure again
    timer.restart();
    for (int i = 0; i < 100; ++i) {
        button->invalidateCache();
        button->update();
        QApplication::processEvents();
    }

    qint64 cacheMissTime = timer.elapsed();

    qDebug() << "Cache effectiveness:";
    qDebug() << "  Cache hits:" << cacheHitTime << "ms";
    qDebug() << "  Cache misses:" << cacheMissTime << "ms";

    // Cache hits should be faster
    QVERIFY(cacheHitTime < cacheMissTime);
}

void FluentButtonPaintOptimizationTest::testMemoryUsage() {
    const int buttonCount = 50;
    QList<FluentButton*> buttons;

    // Measure initial memory
    size_t initialMemory = m_monitor->currentMemoryUsage();

    // Create buttons
    createTestButtons(buttonCount, buttons);

    // Show all buttons to trigger cache creation
    for (auto* button : buttons) {
        button->show();
        button->update();
    }
    QApplication::processEvents();

    size_t memoryWithCache = m_monitor->currentMemoryUsage();

    // Clear all caches
    for (auto* button : buttons) {
        button->invalidateCache();
    }

    size_t memoryAfterClear = m_monitor->currentMemoryUsage();

    qDebug() << "Memory usage analysis:";
    qDebug() << "  Initial memory:" << initialMemory << "bytes";
    qDebug() << "  Memory with cache:" << memoryWithCache << "bytes";
    qDebug() << "  Memory after cache clear:" << memoryAfterClear << "bytes";
    qDebug() << "  Cache overhead per button:"
             << (memoryWithCache - initialMemory) / buttonCount << "bytes";

    // Cache should use reasonable memory (less than 10KB per button)
    size_t cacheOverhead = (memoryWithCache - initialMemory) / buttonCount;
    QVERIFY(cacheOverhead < 10240);  // 10KB per button

    // Cleanup
    qDeleteAll(buttons);
}

void FluentButtonPaintOptimizationTest::testConcurrentButtonRendering() {
    const int buttonCount = 20;
    QList<FluentButton*> buttons;
    createTestButtons(buttonCount, buttons);

    // Show all buttons
    for (auto* button : buttons) {
        button->show();
    }
    QTest::qWaitForWindowExposed(m_testWidget);

    // Measure time to render all buttons simultaneously
    QElapsedTimer timer;
    timer.start();

    for (int iteration = 0; iteration < 10; ++iteration) {
        for (auto* button : buttons) {
            button->update();
        }
        QApplication::processEvents();
    }

    qint64 renderTime = timer.elapsed();

    qDebug() << "Concurrent rendering performance:";
    qDebug() << "  Rendered" << buttonCount << "buttons 10 times in"
             << renderTime << "ms";
    qDebug() << "  Average time per button per frame:"
             << (double)renderTime / (buttonCount * 10) << "ms";

    // Should render efficiently (less than 1ms per button per frame)
    double avgTimePerButton = (double)renderTime / (buttonCount * 10);
    QVERIFY(avgTimePerButton < 1.0);

    // Cleanup
    qDeleteAll(buttons);
}

void FluentButtonPaintOptimizationTest::testCacheInvalidation() {
    auto* button = new FluentButton("Cache Invalidation Test", m_testWidget);
    button->resize(180, 40);
    button->show();

    // Initial render to populate cache
    button->update();
    QApplication::processEvents();

    // Test that style changes invalidate appropriate cache regions
    QElapsedTimer timer;

    // Change text (should invalidate content cache)
    timer.start();
    button->setText("New Text");
    button->update();
    QApplication::processEvents();
    qint64 textChangeTime = timer.elapsed();

    // Change style (should invalidate all caches)
    timer.restart();
    button->setButtonStyle(FluentButtonStyle::Outline);
    button->update();
    QApplication::processEvents();
    qint64 styleChangeTime = timer.elapsed();

    // Change size (should invalidate all caches)
    timer.restart();
    button->resize(220, 45);
    button->update();
    QApplication::processEvents();
    qint64 sizeChangeTime = timer.elapsed();

    qDebug() << "Cache invalidation performance:";
    qDebug() << "  Text change:" << textChangeTime << "ms";
    qDebug() << "  Style change:" << styleChangeTime << "ms";
    qDebug() << "  Size change:" << sizeChangeTime << "ms";

    // All operations should complete quickly (less than 10ms)
    QVERIFY(textChangeTime < 10);
    QVERIFY(styleChangeTime < 10);
    QVERIFY(sizeChangeTime < 10);
}

void FluentButtonPaintOptimizationTest::testStyleChangePerformance() {
    auto* button = new FluentButton("Style Performance Test", m_testWidget);
    button->resize(160, 35);
    button->show();

    const QList<FluentButtonStyle> styles = {
        FluentButtonStyle::Default, FluentButtonStyle::Primary,
        FluentButtonStyle::Accent, FluentButtonStyle::Subtle,
        FluentButtonStyle::Outline};

    QElapsedTimer timer;
    timer.start();

    // Rapidly change styles
    for (int i = 0; i < 100; ++i) {
        FluentButtonStyle style = styles[i % styles.size()];
        button->setButtonStyle(style);
        button->update();
        QApplication::processEvents();
    }

    qint64 styleChangeTime = timer.elapsed();

    qDebug() << "Style change performance:";
    qDebug() << "  100 style changes in" << styleChangeTime << "ms";
    qDebug() << "  Average time per style change:" << styleChangeTime / 100.0
             << "ms";

    // Style changes should be fast (less than 2ms per change on average)
    double avgTimePerChange = styleChangeTime / 100.0;
    QVERIFY(avgTimePerChange < 2.0);
}

void FluentButtonPaintOptimizationTest::createTestButtons(
    int count, QList<FluentButton*>& buttons) {
    const QList<FluentButtonStyle> styles = {
        FluentButtonStyle::Default, FluentButtonStyle::Primary,
        FluentButtonStyle::Accent, FluentButtonStyle::Subtle};

    for (int i = 0; i < count; ++i) {
        auto* button =
            new FluentButton(QString("Button %1").arg(i), m_testWidget);
        button->setButtonStyle(styles[i % styles.size()]);
        button->setGeometry(10 + (i % 10) * 80, 10 + (i / 10) * 50, 75, 35);
        buttons.append(button);
    }
}

QTEST_MAIN(FluentButtonPaintOptimizationTest)
#include "FluentButtonPaintOptimizationTest.moc"
