// tests/Performance/FluentMemoryManagementTest.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QElapsedTimer>
#include "FluentQt/Core/FluentAdvancedMemoryManager.h"
#include "FluentQt/Core/FluentMemoryPool.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Core/FluentPerformance.h"

using namespace FluentQt::Core;
using namespace FluentQt::Components;
using FluentQt::Core::FluentPerformanceMonitor;

class FluentMemoryManagementTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Memory management tests
    void testMemoryPoolPerformance();
    void testAdvancedMemoryManagerEfficiency();
    void testComponentCaching();
    void testMemoryStrategies();
    void testMemoryLimitEnforcement();
    void testMemoryLeakPrevention();
    void testConcurrentMemoryOperations();
    void testMemoryOptimization();

private:
    QWidget* m_testWidget{nullptr};
    FluentPerformanceMonitor* m_monitor{nullptr};
    FluentAdvancedMemoryManager* m_memoryManager{nullptr};
    
    // Helper methods
    void stressTestMemoryAllocations(int count);
    void measureAllocationPerformance(int iterations);
    size_t getCurrentMemoryUsage();
};

void FluentMemoryManagementTest::initTestCase() {
    m_monitor = &FluentPerformanceMonitor::instance();
    m_monitor->startFrameRateMonitoring();
    
    m_memoryManager = &FluentAdvancedMemoryManager::instance();
    m_memoryManager->enablePerformanceMonitoring(true);
}

void FluentMemoryManagementTest::cleanupTestCase() {
    m_monitor->stopFrameRateMonitoring();
}

void FluentMemoryManagementTest::init() {
    m_testWidget = new QWidget;
    m_testWidget->resize(800, 600);
}

void FluentMemoryManagementTest::cleanup() {
    delete m_testWidget;
    m_testWidget = nullptr;
    
    // Clean up any remaining cached components
    m_memoryManager->cleanupUnusedMemory();
}

void FluentMemoryManagementTest::testMemoryPoolPerformance() {
    const int allocationCount = 10000;
    
    // Test standard allocation
    QElapsedTimer timer;
    timer.start();
    
    QList<FluentButton*> standardButtons;
    for (int i = 0; i < allocationCount; ++i) {
        auto* button = new FluentButton(QString("Button %1").arg(i), m_testWidget);
        standardButtons.append(button);
    }
    
    qint64 standardAllocationTime = timer.elapsed();
    
    // Clean up
    qDeleteAll(standardButtons);
    standardButtons.clear();
    
    // Test pool allocation
    FluentMemoryPool<FluentButton> buttonPool;
    
    timer.restart();
    QList<FluentButton*> pooledButtons;
    for (int i = 0; i < allocationCount; ++i) {
        auto* button = buttonPool.construct(QString("Button %1").arg(i), m_testWidget);
        pooledButtons.append(button);
    }
    
    qint64 pooledAllocationTime = timer.elapsed();
    
    // Test deallocation performance
    timer.restart();
    for (auto* button : pooledButtons) {
        buttonPool.destroy(button);
    }
    qint64 pooledDeallocationTime = timer.elapsed();
    
    qDebug() << "Memory pool performance (" << allocationCount << "allocations):";
    qDebug() << "  Standard allocation:" << standardAllocationTime << "ms";
    qDebug() << "  Pooled allocation:" << pooledAllocationTime << "ms";
    qDebug() << "  Pooled deallocation:" << pooledDeallocationTime << "ms";
    
    if (pooledAllocationTime < standardAllocationTime) {
        double improvement = (double)standardAllocationTime / pooledAllocationTime;
        qDebug() << "  Pool improvement:" << QString::number(improvement, 'f', 2) << "x";
        QVERIFY(improvement >= 1.2); // At least 20% improvement
    }
    
    // Verify pool statistics
    QVERIFY(buttonPool.allocatedCount() == 0); // All should be deallocated
    QVERIFY(buttonPool.totalBlocks() > 0); // Should have allocated blocks
}

void FluentMemoryManagementTest::testAdvancedMemoryManagerEfficiency() {
    size_t initialMemory = getCurrentMemoryUsage();
    
    const int componentCount = 1000;
    QList<FluentButton*> buttons;
    
    // Test allocation through advanced memory manager
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < componentCount; ++i) {
        auto* button = FLUENT_ALLOCATE(FluentButton);
        if (button) {
            new(button) FluentButton(QString("Button %1").arg(i), m_testWidget);
            buttons.append(button);
        }
    }
    
    qint64 allocationTime = timer.elapsed();
    size_t memoryAfterAllocation = getCurrentMemoryUsage();
    
    // Test deallocation
    timer.restart();
    for (auto* button : buttons) {
        button->~FluentButton();
        FLUENT_DEALLOCATE(button);
    }
    buttons.clear();
    
    qint64 deallocationTime = timer.elapsed();
    
    // Force cleanup and measure final memory
    m_memoryManager->optimizeMemoryUsage();
    QTest::qWait(100); // Allow cleanup to complete
    size_t finalMemory = getCurrentMemoryUsage();
    
    qDebug() << "Advanced memory manager efficiency:";
    qDebug() << "  Allocation time:" << allocationTime << "ms";
    qDebug() << "  Deallocation time:" << deallocationTime << "ms";
    qDebug() << "  Initial memory:" << initialMemory << "bytes";
    qDebug() << "  Memory after allocation:" << memoryAfterAllocation << "bytes";
    qDebug() << "  Final memory:" << finalMemory << "bytes";
    
    size_t memoryOverhead = memoryAfterAllocation - initialMemory;
    size_t memoryPerComponent = memoryOverhead / componentCount;
    
    qDebug() << "  Memory per component:" << memoryPerComponent << "bytes";
    
    // Memory usage should be reasonable
    QVERIFY(memoryPerComponent < 10000); // Less than 10KB per component
    
    // Memory should be mostly cleaned up
    size_t memoryLeak = finalMemory - initialMemory;
    QVERIFY(memoryLeak < memoryOverhead * 0.2); // Less than 20% leak
    
    // Get memory manager statistics
    auto stats = m_memoryManager->getMemoryStats();
    qDebug() << "  Total allocated:" << stats.totalAllocated << "bytes";
    qDebug() << "  Total deallocated:" << stats.totalDeallocated << "bytes";
    qDebug() << "  Current usage:" << stats.currentUsage << "bytes";
    qDebug() << "  Peak usage:" << stats.peakUsage << "bytes";
}

void FluentMemoryManagementTest::testComponentCaching() {
    // Configure caching for FluentButton
    FluentComponentMemoryConfig config;
    config.strategy = FluentMemoryStrategy::Cached;
    config.maxCachedObjects = 20;
    config.cacheTimeout = std::chrono::milliseconds(5000); // 5 seconds
    
    m_memoryManager->registerComponentType("FluentButton", config);
    
    const int testCount = 50;
    QElapsedTimer timer;
    
    // First allocation (cache miss)
    timer.start();
    QList<FluentButton*> buttons;
    for (int i = 0; i < testCount; ++i) {
        auto* button = m_memoryManager->allocateComponent<FluentButton>("FluentButton");
        if (button) {
            new(button) FluentButton(QString("Button %1").arg(i), m_testWidget);
            buttons.append(button);
        }
    }
    qint64 cacheMissTime = timer.elapsed();
    
    // Deallocate (should go to cache)
    for (auto* button : buttons) {
        button->~FluentButton();
        m_memoryManager->deallocateComponent(button, "FluentButton");
    }
    buttons.clear();
    
    // Second allocation (cache hit)
    timer.restart();
    for (int i = 0; i < testCount; ++i) {
        auto* button = m_memoryManager->allocateComponent<FluentButton>("FluentButton");
        if (button) {
            new(button) FluentButton(QString("Button %1").arg(i), m_testWidget);
            buttons.append(button);
        }
    }
    qint64 cacheHitTime = timer.elapsed();
    
    qDebug() << "Component caching performance:";
    qDebug() << "  Cache miss time:" << cacheMissTime << "ms";
    qDebug() << "  Cache hit time:" << cacheHitTime << "ms";
    
    if (cacheHitTime < cacheMissTime) {
        double improvement = (double)cacheMissTime / cacheHitTime;
        qDebug() << "  Cache improvement:" << QString::number(improvement, 'f', 2) << "x";
        QVERIFY(improvement >= 1.5); // At least 50% improvement
    }
    
    // Clean up
    for (auto* button : buttons) {
        button->~FluentButton();
        m_memoryManager->deallocateComponent(button, "FluentButton");
    }
    
    auto componentStats = m_memoryManager->getComponentStats("FluentButton");
    qDebug() << "  Component allocations:" << componentStats.totalAllocated;
    qDebug() << "  Component deallocations:" << componentStats.totalDeallocated;
}

void FluentMemoryManagementTest::testMemoryStrategies() {
    const QList<FluentMemoryStrategy> strategies = {
        FluentMemoryStrategy::Standard,
        FluentMemoryStrategy::Pooled,
        FluentMemoryStrategy::Cached,
        FluentMemoryStrategy::Adaptive
    };
    
    const int allocationCount = 500;
    
    for (FluentMemoryStrategy strategy : strategies) {
        // Configure strategy
        FluentComponentMemoryConfig config;
        config.strategy = strategy;
        config.maxCachedObjects = 30;
        config.poolBlockSize = 4096;
        
        QString strategyName = QString("TestStrategy_%1").arg(static_cast<int>(strategy));
        m_memoryManager->registerComponentType(strategyName, config);
        
        QElapsedTimer timer;
        timer.start();
        
        // Allocate components
        QList<FluentButton*> buttons;
        for (int i = 0; i < allocationCount; ++i) {
            auto* button = m_memoryManager->allocateComponent<FluentButton>(strategyName);
            if (button) {
                new(button) FluentButton(QString("Button %1").arg(i), m_testWidget);
                buttons.append(button);
            }
        }
        
        qint64 allocationTime = timer.elapsed();
        
        // Deallocate components
        timer.restart();
        for (auto* button : buttons) {
            button->~FluentButton();
            m_memoryManager->deallocateComponent(button, strategyName);
        }
        qint64 deallocationTime = timer.elapsed();
        
        qDebug() << "Strategy" << static_cast<int>(strategy) << "performance:";
        qDebug() << "  Allocation time:" << allocationTime << "ms";
        qDebug() << "  Deallocation time:" << deallocationTime << "ms";
        
        // All strategies should complete in reasonable time
        QVERIFY(allocationTime < 1000); // Less than 1 second
        QVERIFY(deallocationTime < 1000); // Less than 1 second
        
        m_memoryManager->unregisterComponentType(strategyName);
    }
}

void FluentMemoryManagementTest::testMemoryLimitEnforcement() {
    // Set a low memory limit for testing
    size_t originalLimit = m_memoryManager->memoryLimit();
    m_memoryManager->setMemoryLimit(1024 * 1024); // 1MB
    
    bool limitExceeded = false;
    connect(m_memoryManager, &FluentAdvancedMemoryManager::memoryLimitExceeded,
            [&limitExceeded](size_t current, size_t limit) {
                Q_UNUSED(current)
                Q_UNUSED(limit)
                limitExceeded = true;
            });
    
    // Allocate many components to exceed limit
    QList<FluentButton*> buttons;
    for (int i = 0; i < 1000; ++i) {
        auto* button = FLUENT_ALLOCATE(FluentButton);
        if (button) {
            new(button) FluentButton(QString("Button %1").arg(i), m_testWidget);
            buttons.append(button);
        }
        
        // Check if limit was exceeded
        if (limitExceeded) {
            break;
        }
    }
    
    qDebug() << "Memory limit enforcement:";
    qDebug() << "  Allocated" << buttons.size() << "components before limit exceeded";
    qDebug() << "  Limit exceeded:" << (limitExceeded ? "Yes" : "No");
    
    // Should have triggered limit exceeded signal
    QVERIFY(limitExceeded);
    
    // Clean up
    for (auto* button : buttons) {
        button->~FluentButton();
        FLUENT_DEALLOCATE(button);
    }
    
    // Restore original limit
    m_memoryManager->setMemoryLimit(originalLimit);
}

size_t FluentMemoryManagementTest::getCurrentMemoryUsage() {
    return m_monitor->currentMemoryUsage();
}

QTEST_MAIN(FluentMemoryManagementTest)
#include "FluentMemoryManagementTest.moc"
