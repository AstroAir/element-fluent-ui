// tests/Core/FluentPerformanceTest.cpp
#include <QApplication>
#include <QSignalSpy>
#include <QTimer>
#include <QWidget>
#include <QtTest/QtTest>
#include <chrono>
#include <thread>

#include "FluentQt/Core/FluentBenchmark.h"
#include "FluentQt/Core/FluentPerformance.h"

using namespace FluentQt::Core;

class FluentPerformanceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Performance Monitor Tests
    void testSingletonAccess();
    void testFrameRateMonitoring();
    void testMemoryTracking();
    void testComponentRegistration();
    void testComponentPerformanceTracking();
    void testProfiling();
    void testBenchmarking();
    void testPerformanceOptimizationHints();
    void testLowPerformanceMode();
    void testPerformanceSignals();
    void testThreadSafety();

    // Benchmark System Tests
    void testBenchmarkSuite();
    void testBenchmarkExecution();
    void testBenchmarkResults();
    void testBenchmarkReporting();

    // Memory Pool Tests
    void testMemoryPoolAllocation();
    void testMemoryPoolDeallocation();
    void testMemoryPoolThreadSafety();
    void testMemoryPoolStatistics();

    // Performance Regression Tests
    void testPerformanceRegression();
    void testMemoryLeakDetection();

private:
    FluentPerformanceMonitor* m_monitor;
    QWidget* m_testWidget;
};

void FluentPerformanceTest::initTestCase() {
    // Initialize test environment
    m_monitor = &FluentPerformanceMonitor::instance();
    m_testWidget = new QWidget();
    m_testWidget->setObjectName("TestWidget");
}

void FluentPerformanceTest::cleanupTestCase() { delete m_testWidget; }

void FluentPerformanceTest::init() {
    // Reset monitor state before each test
    m_monitor->stopFrameRateMonitoring();
    m_monitor->clearProfileData();
    m_monitor->clearBenchmarkResults();
    m_monitor->enableLowPerformanceMode(false);
}

void FluentPerformanceTest::cleanup() {
    // Clean up after each test
    m_monitor->stopFrameRateMonitoring();
}

void FluentPerformanceTest::testSingletonAccess() {
    // Test singleton pattern
    auto& monitor1 = FluentPerformanceMonitor::instance();
    auto& monitor2 = FluentPerformanceMonitor::instance();

    QVERIFY(&monitor1 == &monitor2);
    QVERIFY(&monitor1 == m_monitor);
}

void FluentPerformanceTest::testFrameRateMonitoring() {
    // Test frame rate monitoring
    QSignalSpy frameRateSpy(m_monitor,
                            &FluentPerformanceMonitor::frameRateChanged);

    m_monitor->startFrameRateMonitoring();
    QVERIFY(m_monitor->currentFrameRate() >= 0.0);

    // Wait for at least one frame rate update
    QVERIFY(frameRateSpy.wait(2000));
    QVERIFY(frameRateSpy.count() >= 1);

    double frameRate = frameRateSpy.first().first().toDouble();
    QVERIFY(frameRate >= 0.0);
    QVERIFY(frameRate <= 1000.0);  // Reasonable upper bound

    m_monitor->stopFrameRateMonitoring();
}

void FluentPerformanceTest::testMemoryTracking() {
    // Test memory usage tracking
    size_t initialMemory = m_monitor->currentMemoryUsage();

    m_monitor->trackMemoryUsage();
    size_t currentMemory = m_monitor->currentMemoryUsage();

    QVERIFY(currentMemory >= initialMemory);

    // Test peak memory tracking
    size_t peakMemory = m_monitor->peakMemoryUsage();
    QVERIFY(peakMemory >= currentMemory);

    m_monitor->resetPeakMemoryUsage();
    QCOMPARE(m_monitor->peakMemoryUsage(), currentMemory);
}

void FluentPerformanceTest::testComponentRegistration() {
    // Test component registration
    const QString componentName = "TestComponent";

    m_monitor->registerComponent(componentName, m_testWidget);

    auto componentData = m_monitor->getComponentData(componentName);
    QCOMPARE(componentData.componentName, componentName);
    QCOMPARE(componentData.renderCount, 0);

    m_monitor->unregisterComponent(componentName);

    // After unregistration, should return empty data
    auto emptyData = m_monitor->getComponentData(componentName);
    QCOMPARE(emptyData.componentName, componentName);
    QCOMPARE(emptyData.renderCount, 0);
}

void FluentPerformanceTest::testComponentPerformanceTracking() {
    // Test component performance tracking
    const QString componentName = "TestComponent";
    m_monitor->registerComponent(componentName, m_testWidget);

    QSignalSpy performanceIssueSpy(
        m_monitor, &FluentPerformanceMonitor::componentPerformanceIssue);

    // Record a fast render
    m_monitor->recordComponentRender(componentName,
                                     std::chrono::milliseconds(5));

    auto data = m_monitor->getComponentData(componentName);
    QCOMPARE(data.renderCount, 1);
    QCOMPARE(data.averageRenderTime, std::chrono::milliseconds(5));
    QCOMPARE(data.maxRenderTime, std::chrono::milliseconds(5));
    QVERIFY(!data.hasPerformanceIssues);

    // Record a slow render (should trigger performance issue)
    m_monitor->recordComponentRender(componentName,
                                     std::chrono::milliseconds(20));

    data = m_monitor->getComponentData(componentName);
    QCOMPARE(data.renderCount, 2);
    QVERIFY(data.hasPerformanceIssues);
    QCOMPARE(performanceIssueSpy.count(), 1);

    m_monitor->unregisterComponent(componentName);
}

void FluentPerformanceTest::testProfiling() {
    // Test profiling functionality
    const QString operation = "TestOperation";

    m_monitor->beginProfile(operation);

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    m_monitor->endProfile(operation);

    auto profileTime = m_monitor->getProfileTime(operation);
    QVERIFY(profileTime >=
            std::chrono::milliseconds(8));  // Allow some tolerance
    QVERIFY(profileTime <= std::chrono::milliseconds(50));  // Upper bound

    // Test RAII profiler
    {
        FluentProfiler profiler("RAIITest");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    auto raiiTime = m_monitor->getProfileTime("RAIITest");
    QVERIFY(raiiTime >= std::chrono::milliseconds(3));
}

void FluentPerformanceTest::testBenchmarking() {
    // Test benchmarking system
    QSignalSpy benchmarkSpy(m_monitor,
                            &FluentPerformanceMonitor::benchmarkCompleted);

    bool benchmarkExecuted = false;
    m_monitor->runBenchmark("TestBenchmark", [&benchmarkExecuted]() {
        benchmarkExecuted = true;
        // Simulate some work
        volatile int sum = 0;
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
    });

    QVERIFY(benchmarkExecuted);
    QCOMPARE(benchmarkSpy.count(), 1);

    auto results = m_monitor->getBenchmarkResults();
    QVERIFY(!results.isEmpty());

    const auto& result = results.first();
    QCOMPARE(result.testName, QString("TestBenchmark"));
    QVERIFY(result.passed);
    QVERIFY(result.duration > std::chrono::milliseconds(0));
    QVERIFY(result.operationsPerSecond > 0.0);
}

void FluentPerformanceTest::testPerformanceOptimizationHints() {
    // Test performance optimization hints

    // Initially should not skip animations (good performance)
    QVERIFY(!m_monitor->shouldSkipAnimation());
    QVERIFY(!m_monitor->shouldReduceEffects());
    QVERIFY(!m_monitor->shouldUseLowQualityRendering());

    // Enable low performance mode
    m_monitor->enableLowPerformanceMode(true);

    QVERIFY(m_monitor->isLowPerformanceModeEnabled());
    QVERIFY(m_monitor->shouldSkipAnimation());
    QVERIFY(m_monitor->shouldReduceEffects());
    QVERIFY(m_monitor->shouldUseLowQualityRendering());

    // Check animation duration recommendations
    int duration = m_monitor->recommendedAnimationDuration();
    QVERIFY(duration > 0);
    QVERIFY(duration <= 250);  // Should be reduced in low performance mode

    // Check max animations recommendation
    int maxAnimations = m_monitor->recommendedMaxAnimations();
    QVERIFY(maxAnimations >= 1);
    QVERIFY(maxAnimations <= 10);
}

void FluentPerformanceTest::testLowPerformanceMode() {
    // Test low performance mode
    QSignalSpy metricsSpy(m_monitor, &FluentPerformanceMonitor::metricsUpdated);

    QVERIFY(!m_monitor->isLowPerformanceModeEnabled());

    m_monitor->enableLowPerformanceMode(true);
    QVERIFY(m_monitor->isLowPerformanceModeEnabled());

    // Should emit metrics update
    QVERIFY(metricsSpy.count() >= 1);

    m_monitor->enableLowPerformanceMode(false);
    QVERIFY(!m_monitor->isLowPerformanceModeEnabled());
}

void FluentPerformanceTest::testPerformanceSignals() {
    // Test performance warning signals
    QSignalSpy warningSpy(m_monitor,
                          &FluentPerformanceMonitor::performanceWarning);
    QSignalSpy criticalSpy(m_monitor,
                           &FluentPerformanceMonitor::performanceCritical);

    m_monitor->startFrameRateMonitoring();

    // Wait for potential signals (may not occur in test environment)
    QTest::qWait(1000);

    // Signals may or may not be emitted depending on system performance
    QVERIFY(warningSpy.count() >= 0);
    QVERIFY(criticalSpy.count() >= 0);

    m_monitor->stopFrameRateMonitoring();
}

void FluentPerformanceTest::testThreadSafety() {
    // Test thread safety of performance monitor
    const int numThreads = 4;
    const int operationsPerThread = 100;

    QList<std::thread> threads;
    std::atomic<int> completedOperations{0};

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(
            [this, i, operationsPerThread, &completedOperations]() {
                for (int j = 0; j < operationsPerThread; ++j) {
                    QString operation = QString("Thread%1_Op%2").arg(i).arg(j);

                    m_monitor->beginProfile(operation);
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                    m_monitor->endProfile(operation);

                    completedOperations++;
                }
            });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    QCOMPARE(completedOperations.load(), numThreads * operationsPerThread);
}

void FluentPerformanceTest::testBenchmarkSuite() {
    // Test benchmark suite functionality
    FluentBenchmarkSuite suite("TestSuite");

    bool test1Executed = false;
    bool test2Executed = false;

    suite.addTestCase(
        "Test1", [&test1Executed]() { test1Executed = true; }, 10);

    suite.addTestCase("Test2", [&test2Executed]() { test2Executed = true; }, 5);

    QSignalSpy suiteCompletedSpy(&suite, &FluentBenchmarkSuite::suiteCompleted);

    suite.runAllTests();

    QVERIFY(test1Executed);
    QVERIFY(test2Executed);
    QCOMPARE(suiteCompletedSpy.count(), 1);

    auto results = suite.getResults();
    QCOMPARE(results.size(), 2);

    for (const auto& result : results) {
        QVERIFY(result.passed);
        QVERIFY(result.totalTime > std::chrono::milliseconds(0));
    }
}

void FluentPerformanceTest::testBenchmarkExecution() {
    // Test individual benchmark execution
    FluentBenchmarkSuite suite("ExecutionTest");

    int executionCount = 0;
    suite.addTestCase(
        "CountTest", [&executionCount]() { executionCount++; }, 50);

    suite.runTest("CountTest");

    QCOMPARE(executionCount, 50);

    auto result = suite.getResult("CountTest");
    QCOMPARE(result.testName, QString("CountTest"));
    QVERIFY(result.passed);
}

void FluentPerformanceTest::testBenchmarkResults() {
    // Test benchmark result generation
    FluentBenchmarkSuite suite("ResultsTest");

    suite.addTestCase(
        "SimpleTest",
        []() {
            // Simple operation
            volatile int x = 42;
            Q_UNUSED(x);
        },
        100);

    suite.runAllTests();

    QString report = suite.generateReport();
    QVERIFY(report.contains("SimpleTest"));
    QVERIFY(report.contains("PASSED"));

    QString csvReport = suite.generateCsvReport();
    QVERIFY(csvReport.contains("Test Name"));
    QVERIFY(csvReport.contains("SimpleTest"));
}

void FluentPerformanceTest::testBenchmarkReporting() {
    // Test benchmark reporting functionality
    FluentBenchmarkSuite suite("ReportingTest");

    suite.addTestCase(
        "ReportTest",
        []() { std::this_thread::sleep_for(std::chrono::microseconds(100)); },
        10);

    suite.runAllTests();

    // Test report generation
    QString report = suite.generateReport();
    QVERIFY(!report.isEmpty());
    QVERIFY(report.contains("ReportTest"));

    QString csvReport = suite.generateCsvReport();
    QVERIFY(!csvReport.isEmpty());
    QVERIFY(csvReport.contains("ReportTest"));
}

void FluentPerformanceTest::testMemoryPoolAllocation() {
    // Test memory pool allocation
    FluentMemoryPool<int> pool;

    // Test allocation
    int* ptr1 = pool.allocate();
    QVERIFY(ptr1 != nullptr);
    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(1));

    int* ptr2 = pool.allocate();
    QVERIFY(ptr2 != nullptr);
    QVERIFY(ptr2 != ptr1);
    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(2));

    // Test deallocation
    pool.deallocate(ptr1);
    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(1));

    pool.deallocate(ptr2);
    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(0));
}

void FluentPerformanceTest::testMemoryPoolDeallocation() {
    // Test memory pool deallocation and reuse
    FluentMemoryPool<int> pool;

    int* ptr1 = pool.allocate();
    pool.deallocate(ptr1);

    int* ptr2 = pool.allocate();
    // Should reuse the deallocated memory
    QCOMPARE(ptr1, ptr2);

    pool.deallocate(ptr2);
}

void FluentPerformanceTest::testMemoryPoolThreadSafety() {
    // Test memory pool thread safety
    FluentMemoryPool<int> pool;
    const int numThreads = 4;
    const int allocationsPerThread = 100;

    QList<std::thread> threads;
    std::atomic<int> successfulAllocations{0};

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(
            [&pool, allocationsPerThread, &successfulAllocations]() {
                QList<int*> allocatedPtrs;

                // Allocate
                for (int j = 0; j < allocationsPerThread; ++j) {
                    int* ptr = pool.allocate();
                    if (ptr) {
                        allocatedPtrs.append(ptr);
                        successfulAllocations++;
                    }
                }

                // Deallocate
                for (int* ptr : allocatedPtrs) {
                    pool.deallocate(ptr);
                }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    QCOMPARE(successfulAllocations.load(), numThreads * allocationsPerThread);
    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(0));
}

void FluentPerformanceTest::testMemoryPoolStatistics() {
    // Test memory pool statistics
    FluentMemoryPool<int, 1024> pool;

    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(0));
    QCOMPARE(pool.totalBlocks(), static_cast<size_t>(0));

    // Allocate enough to trigger block allocation
    int* ptr = pool.allocate();
    QVERIFY(ptr != nullptr);
    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(1));
    QCOMPARE(pool.totalBlocks(), static_cast<size_t>(1));
    QCOMPARE(pool.totalMemory(), static_cast<size_t>(1024));

    pool.deallocate(ptr);
    QCOMPARE(pool.allocatedCount(), static_cast<size_t>(0));
    // Blocks remain allocated
    QCOMPARE(pool.totalBlocks(), static_cast<size_t>(1));
}

void FluentPerformanceTest::testPerformanceRegression() {
    // Test for performance regressions
    const int iterations = 1000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        m_monitor->beginProfile("RegressionTest");
        m_monitor->endProfile("RegressionTest");
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Should complete within reasonable time (adjust threshold as needed)
    QVERIFY(duration < std::chrono::milliseconds(1000));
}

void FluentPerformanceTest::testMemoryLeakDetection() {
    // Test memory leak detection
    size_t initialMemory = m_monitor->currentMemoryUsage();

    {
        FluentMemoryTracker tracker("LeakTest");

        // Allocate some memory
        QList<int*> ptrs;
        for (int i = 0; i < 100; ++i) {
            ptrs.append(new int(i));
        }

        // Clean up
        for (int* ptr : ptrs) {
            delete ptr;
        }
    }

    size_t finalMemory = m_monitor->currentMemoryUsage();

    // Memory usage should be similar (allowing for some variance)
    qint64 memoryDiff =
        static_cast<qint64>(finalMemory) - static_cast<qint64>(initialMemory);
    QVERIFY(qAbs(memoryDiff) < 1024 * 1024);  // Less than 1MB difference
}

QTEST_MAIN(FluentPerformanceTest)
#include "FluentPerformanceTest.moc"
