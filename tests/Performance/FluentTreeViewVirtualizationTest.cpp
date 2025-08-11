// tests/Performance/FluentTreeViewVirtualizationTest.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QElapsedTimer>
#include <QTreeWidgetItem>
#include <QScrollBar>
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Core/FluentPerformance.h"

using namespace FluentQt::Components;
using FluentQt::Core::FluentPerformanceMonitor;

class FluentTreeViewVirtualizationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Virtualization performance tests
    void testLargeDatasetPerformance();
    void testScrollingPerformance();
    void testColumnVirtualization();
    void testMemoryEfficiency();
    void testVirtualizationMetrics();
    void testChunkProcessing();
    void testCacheEffectiveness();
    void testFilteringWithVirtualization();

private:
    QWidget* m_testWidget{nullptr};
    FluentPerformanceMonitor* m_monitor{nullptr};
    
    // Helper methods
    void populateTreeWithLargeDataset(FluentTreeView* treeView, int itemCount);
    void createHierarchicalData(FluentTreeView* treeView, int depth, int childrenPerNode);
    void measureScrollPerformance(FluentTreeView* treeView, int scrollDistance);
};

void FluentTreeViewVirtualizationTest::initTestCase() {
    m_monitor = &FluentPerformanceMonitor::instance();
    m_monitor->startFrameRateMonitoring();
}

void FluentTreeViewVirtualizationTest::cleanupTestCase() {
    m_monitor->stopFrameRateMonitoring();
}

void FluentTreeViewVirtualizationTest::init() {
    m_testWidget = new QWidget;
    m_testWidget->resize(800, 600);
}

void FluentTreeViewVirtualizationTest::cleanup() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentTreeViewVirtualizationTest::testLargeDatasetPerformance() {
    auto* treeView = new FluentTreeView(m_testWidget);
    treeView->resize(600, 400);
    
    // Test without virtualization
    treeView->setVirtualizationEnabled(false);
    
    QElapsedTimer timer;
    timer.start();
    populateTreeWithLargeDataset(treeView, 10000);
    qint64 nonVirtualizedTime = timer.elapsed();
    
    treeView->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Measure rendering time without virtualization
    timer.restart();
    for (int i = 0; i < 10; ++i) {
        treeView->update();
        QApplication::processEvents();
    }
    qint64 nonVirtualizedRenderTime = timer.elapsed();
    
    // Clear and test with virtualization
    treeView->clear();
    treeView->setVirtualizationEnabled(true);
    treeView->setVirtualizationChunkSize(100);
    
    timer.restart();
    populateTreeWithLargeDataset(treeView, 10000);
    qint64 virtualizedTime = timer.elapsed();
    
    // Measure rendering time with virtualization
    timer.restart();
    for (int i = 0; i < 10; ++i) {
        treeView->update();
        QApplication::processEvents();
    }
    qint64 virtualizedRenderTime = timer.elapsed();
    
    qDebug() << "Large dataset performance (10,000 items):";
    qDebug() << "  Population - Non-virtualized:" << nonVirtualizedTime << "ms";
    qDebug() << "  Population - Virtualized:" << virtualizedTime << "ms";
    qDebug() << "  Rendering - Non-virtualized:" << nonVirtualizedRenderTime << "ms";
    qDebug() << "  Rendering - Virtualized:" << virtualizedRenderTime << "ms";
    
    // Virtualization should significantly improve rendering performance
    QVERIFY(virtualizedRenderTime < nonVirtualizedRenderTime);
    
    // Population time might be similar, but rendering should be much faster
    double renderingImprovement = (double)nonVirtualizedRenderTime / virtualizedRenderTime;
    QVERIFY(renderingImprovement >= 2.0); // At least 2x improvement
    
    // Check virtualization metrics
    auto metrics = treeView->getVirtualizationMetrics();
    QVERIFY(metrics.totalItems == 10000);
    QVERIFY(metrics.visibleItems < metrics.totalItems);
    QVERIFY(metrics.renderedItems <= metrics.visibleItems);
    
    qDebug() << "  Virtualization metrics:";
    qDebug() << "    Total items:" << metrics.totalItems;
    qDebug() << "    Visible items:" << metrics.visibleItems;
    qDebug() << "    Rendered items:" << metrics.renderedItems;
    qDebug() << "    Memory usage:" << metrics.memoryUsage << "bytes";
}

void FluentTreeViewVirtualizationTest::testScrollingPerformance() {
    auto* treeView = new FluentTreeView(m_testWidget);
    treeView->setVirtualizationEnabled(true);
    treeView->resize(600, 400);
    
    populateTreeWithLargeDataset(treeView, 5000);
    treeView->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Measure scrolling performance
    auto* scrollBar = treeView->findChild<QScrollBar*>();
    QVERIFY(scrollBar != nullptr);
    
    QElapsedTimer timer;
    timer.start();
    
    // Simulate smooth scrolling
    const int scrollSteps = 100;
    const int maxScroll = scrollBar->maximum();
    const int stepSize = maxScroll / scrollSteps;
    
    for (int i = 0; i < scrollSteps; ++i) {
        scrollBar->setValue(i * stepSize);
        QApplication::processEvents();
    }
    
    qint64 scrollTime = timer.elapsed();
    
    qDebug() << "Scrolling performance:";
    qDebug() << "  Scrolled through 5,000 items in" << scrollTime << "ms";
    qDebug() << "  Average time per scroll step:" << (double)scrollTime / scrollSteps << "ms";
    
    // Scrolling should be smooth (less than 16ms per step for 60fps)
    double avgTimePerStep = (double)scrollTime / scrollSteps;
    QVERIFY(avgTimePerStep < 16.0);
    
    // Check that virtualization window updates correctly
    auto metrics = treeView->getVirtualizationMetrics();
    QVERIFY(metrics.visibleItems > 0);
    QVERIFY(metrics.visibleItems < metrics.totalItems);
}

void FluentTreeViewVirtualizationTest::testColumnVirtualization() {
    auto* treeView = new FluentTreeView(m_testWidget);
    treeView->setVirtualizationEnabled(true);
    treeView->setColumnVirtualizationEnabled(true);
    
    // Add many columns
    for (int i = 0; i < 50; ++i) {
        FluentTreeColumn column;
        column.header = QString("Column %1").arg(i);
        column.width = 100;
        treeView->addColumn(column);
    }
    
    // Add items with data in all columns
    for (int i = 0; i < 1000; ++i) {
        auto* item = treeView->addTopLevelItem(QString("Item %1").arg(i));
        for (int col = 1; col < 50; ++col) {
            item->setText(col, QString("Data %1-%2").arg(i).arg(col));
        }
    }
    
    treeView->resize(400, 300); // Small width to force horizontal scrolling
    treeView->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Test horizontal scrolling performance
    auto* hScrollBar = treeView->findChild<QScrollBar*>();
    // Find horizontal scroll bar specifically
    for (auto* scrollBar : treeView->findChildren<QScrollBar*>()) {
        if (scrollBar->orientation() == Qt::Horizontal) {
            hScrollBar = scrollBar;
            break;
        }
    }
    
    if (hScrollBar && hScrollBar->maximum() > 0) {
        QElapsedTimer timer;
        timer.start();
        
        // Scroll horizontally
        for (int i = 0; i <= hScrollBar->maximum(); i += 50) {
            hScrollBar->setValue(i);
            QApplication::processEvents();
        }
        
        qint64 hScrollTime = timer.elapsed();
        
        qDebug() << "Column virtualization performance:";
        qDebug() << "  Horizontal scroll time:" << hScrollTime << "ms";
        
        // Should handle column virtualization efficiently
        QVERIFY(hScrollTime < 1000); // Less than 1 second for full scroll
    }
}

void FluentTreeViewVirtualizationTest::testMemoryEfficiency() {
    auto* treeView = new FluentTreeView(m_testWidget);
    
    // Measure memory without virtualization
    size_t initialMemory = m_monitor->currentMemoryUsage();
    
    treeView->setVirtualizationEnabled(false);
    populateTreeWithLargeDataset(treeView, 2000);
    treeView->show();
    QApplication::processEvents();
    
    size_t memoryWithoutVirtualization = m_monitor->currentMemoryUsage();
    
    // Clear and test with virtualization
    treeView->clear();
    treeView->setVirtualizationEnabled(true);
    populateTreeWithLargeDataset(treeView, 2000);
    QApplication::processEvents();
    
    size_t memoryWithVirtualization = m_monitor->currentMemoryUsage();
    
    qDebug() << "Memory efficiency:";
    qDebug() << "  Initial memory:" << initialMemory << "bytes";
    qDebug() << "  Without virtualization:" << memoryWithoutVirtualization << "bytes";
    qDebug() << "  With virtualization:" << memoryWithVirtualization << "bytes";
    
    size_t nonVirtualizedOverhead = memoryWithoutVirtualization - initialMemory;
    size_t virtualizedOverhead = memoryWithVirtualization - initialMemory;
    
    qDebug() << "  Memory overhead - Non-virtualized:" << nonVirtualizedOverhead << "bytes";
    qDebug() << "  Memory overhead - Virtualized:" << virtualizedOverhead << "bytes";
    
    // Virtualization might use slightly more memory for caching, but should be reasonable
    // The main benefit is in rendering performance, not necessarily memory usage
    QVERIFY(virtualizedOverhead < nonVirtualizedOverhead * 1.5); // At most 50% more memory
}

void FluentTreeViewVirtualizationTest::testVirtualizationMetrics() {
    auto* treeView = new FluentTreeView(m_testWidget);
    treeView->setVirtualizationEnabled(true);
    treeView->resize(400, 300);
    
    populateTreeWithLargeDataset(treeView, 1000);
    treeView->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    auto metrics = treeView->getVirtualizationMetrics();
    
    qDebug() << "Virtualization metrics validation:";
    qDebug() << "  Total items:" << metrics.totalItems;
    qDebug() << "  Visible items:" << metrics.visibleItems;
    qDebug() << "  Rendered items:" << metrics.renderedItems;
    qDebug() << "  Cached items:" << metrics.cachedItems;
    qDebug() << "  Render time:" << metrics.renderTime << "ms";
    qDebug() << "  Memory usage:" << metrics.memoryUsage << "bytes";
    
    // Validate metrics make sense
    QVERIFY(metrics.totalItems == 1000);
    QVERIFY(metrics.visibleItems > 0);
    QVERIFY(metrics.visibleItems <= metrics.totalItems);
    QVERIFY(metrics.renderedItems <= metrics.visibleItems);
    QVERIFY(metrics.renderTime >= 0.0);
}

void FluentTreeViewVirtualizationTest::testChunkProcessing() {
    auto* treeView = new FluentTreeView(m_testWidget);
    treeView->setVirtualizationEnabled(true);
    
    // Test different chunk sizes
    const QList<int> chunkSizes = {10, 50, 100, 200, 500};
    
    for (int chunkSize : chunkSizes) {
        treeView->clear();
        treeView->setVirtualizationChunkSize(chunkSize);
        
        QElapsedTimer timer;
        timer.start();
        
        populateTreeWithLargeDataset(treeView, 2000);
        treeView->show();
        QApplication::processEvents();
        
        qint64 processingTime = timer.elapsed();
        
        qDebug() << "Chunk size" << chunkSize << "processing time:" << processingTime << "ms";
        
        // All chunk sizes should complete in reasonable time
        QVERIFY(processingTime < 5000); // Less than 5 seconds
        
        auto metrics = treeView->getVirtualizationMetrics();
        QVERIFY(metrics.totalItems == 2000);
    }
}

void FluentTreeViewVirtualizationTest::populateTreeWithLargeDataset(FluentTreeView* treeView, int itemCount) {
    for (int i = 0; i < itemCount; ++i) {
        auto* item = treeView->addTopLevelItem(QString("Item %1").arg(i));
        
        // Add some child items for hierarchy
        if (i % 10 == 0) {
            for (int j = 0; j < 3; ++j) {
                treeView->addChildItem(item, QString("Child %1-%2").arg(i).arg(j));
            }
        }
    }
}

QTEST_MAIN(FluentTreeViewVirtualizationTest)
#include "FluentTreeViewVirtualizationTest.moc"
