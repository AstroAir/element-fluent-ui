// tests/Performance/FluentPerformanceBenchmark.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QElapsedTimer>
#include <QThread>
#include <chrono>
#include <vector>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Core/FluentBenchmark.h"
#include "FluentQt/Core/FluentMemoryPool.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentPerformanceBenchmark : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Component creation benchmarks
    void benchmarkButtonCreation();
    void benchmarkButtonCreation_data();
    void benchmarkTextInputCreation();
    void benchmarkCardCreation();
    void benchmarkProgressBarCreation();

    // Memory management benchmarks
    void benchmarkMemoryPoolAllocation();
    void benchmarkStandardAllocation();
    void benchmarkMemoryPoolVsStandard();

    // Rendering benchmarks
    void benchmarkComponentRendering();
    void benchmarkComplexLayoutRendering();
    void benchmarkAnimationPerformance();

    // Theme switching benchmarks
    void benchmarkThemeSwitching();
    void benchmarkStyleSheetApplication();

    // Stress tests
    void stressTestManyComponents();
    void stressTestRapidUpdates();
    void stressTestMemoryUsage();

    // Real-world scenarios
    void benchmarkFormCreation();
    void benchmarkListViewPerformance();
    void benchmarkComplexUICreation();

private:
    void createTestComponents(int count);
    void measureMemoryUsage(const QString& testName);
    double calculateOperationsPerSecond(int operations, std::chrono::milliseconds duration);

private:
    QWidget* m_testWidget;
    FluentPerformanceMonitor* m_performanceMonitor;
    FluentBenchmarkSuite* m_benchmarkSuite;
    std::vector<QWidget*> m_testComponents;
};

void FluentPerformanceBenchmark::initTestCase() {
    // Initialize performance monitoring
    m_performanceMonitor = &FluentPerformanceMonitor::instance();
    m_performanceMonitor->startFrameRateMonitoring();
    m_performanceMonitor->enableLowPerformanceMode(false);
    
    // Create benchmark suite
    m_benchmarkSuite = new FluentBenchmarkSuite("FluentQt Performance Tests", this);
    
    // Create test widget
    m_testWidget = new QWidget();
    m_testWidget->setFixedSize(800, 600);
    
    qDebug() << "Performance benchmark initialized";
    qDebug() << "Initial frame rate:" << m_performanceMonitor->currentFrameRate();
    qDebug() << "Initial memory usage:" << m_performanceMonitor->currentMemoryUsage() / (1024*1024) << "MB";
}

void FluentPerformanceBenchmark::cleanupTestCase() {
    delete m_testWidget;
    m_performanceMonitor->stopFrameRateMonitoring();
    
    // Print final benchmark results
    auto results = m_benchmarkSuite->getResults();
    qDebug() << "\n=== Performance Benchmark Results ===";
    for (const auto& result : results) {
        qDebug() << result.testName << ":" 
                 << result.averageTime.count() << "ms avg,"
                 << result.operationsPerSecond << "ops/sec";
    }
}

void FluentPerformanceBenchmark::init() {
    // Clear any existing components
    qDeleteAll(m_testComponents);
    m_testComponents.clear();
    
    // Reset performance counters
    m_performanceMonitor->clearProfileData();
}

void FluentPerformanceBenchmark::cleanup() {
    // Clean up test components
    qDeleteAll(m_testComponents);
    m_testComponents.clear();
}

void FluentPerformanceBenchmark::benchmarkButtonCreation_data() {
    QTest::addColumn<int>("count");
    QTest::addColumn<QString>("description");
    
    QTest::newRow("10 buttons") << 10 << "Small scale";
    QTest::newRow("100 buttons") << 100 << "Medium scale";
    QTest::newRow("1000 buttons") << 1000 << "Large scale";
}

void FluentPerformanceBenchmark::benchmarkButtonCreation() {
    QFETCH(int, count);
    QFETCH(QString, description);
    
    QElapsedTimer timer;
    size_t initialMemory = m_performanceMonitor->currentMemoryUsage();
    
    QBENCHMARK {
        timer.start();
        
        for (int i = 0; i < count; ++i) {
            auto* button = new FluentButton(QString("Button %1").arg(i), m_testWidget);
            m_testComponents.push_back(button);
        }
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        double opsPerSec = calculateOperationsPerSecond(count, elapsed);
        
        qDebug() << description << "- Created" << count << "buttons in" 
                 << elapsed.count() << "ms (" << opsPerSec << "ops/sec)";
    }
    
    size_t finalMemory = m_performanceMonitor->currentMemoryUsage();
    size_t memoryUsed = finalMemory - initialMemory;
    qDebug() << "Memory used:" << memoryUsed / 1024 << "KB";
    
    // Performance assertions
    QVERIFY2(m_performanceMonitor->currentFrameRate() > 30.0, "Frame rate should remain above 30 FPS");
}

void FluentPerformanceBenchmark::benchmarkTextInputCreation() {
    const int count = 500;
    
    QBENCHMARK {
        QElapsedTimer timer;
        timer.start();
        
        for (int i = 0; i < count; ++i) {
            auto* textInput = new FluentTextInput(m_testWidget);
            textInput->setPlaceholderText(QString("Input %1").arg(i));
            m_testComponents.push_back(textInput);
        }
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        qDebug() << "Created" << count << "text inputs in" << elapsed.count() << "ms";
    }
}

void FluentPerformanceBenchmark::benchmarkCardCreation() {
    const int count = 200;
    
    QBENCHMARK {
        QElapsedTimer timer;
        timer.start();
        
        for (int i = 0; i < count; ++i) {
            auto* card = new FluentCard(m_testWidget);
            card->setTitle(QString("Card %1").arg(i));
            card->setContent(QString("Content for card %1").arg(i));
            m_testComponents.push_back(card);
        }
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        qDebug() << "Created" << count << "cards in" << elapsed.count() << "ms";
    }
}

void FluentPerformanceBenchmark::benchmarkMemoryPoolAllocation() {
    const int iterations = 10000;
    FluentMemoryPool<FluentButton> pool;
    
    QBENCHMARK {
        QElapsedTimer timer;
        timer.start();
        
        std::vector<FluentButton*> buttons;
        buttons.reserve(iterations);
        
        // Allocate
        for (int i = 0; i < iterations; ++i) {
            auto* button = pool.construct(QString("Button %1").arg(i), nullptr);
            buttons.push_back(button);
        }
        
        // Deallocate
        for (auto* button : buttons) {
            pool.destroy(button);
        }
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        qDebug() << "Memory pool: " << iterations << "allocations in" << elapsed.count() << "ms";
    }
}

void FluentPerformanceBenchmark::benchmarkStandardAllocation() {
    const int iterations = 10000;
    
    QBENCHMARK {
        QElapsedTimer timer;
        timer.start();
        
        std::vector<FluentButton*> buttons;
        buttons.reserve(iterations);
        
        // Allocate
        for (int i = 0; i < iterations; ++i) {
            auto* button = new FluentButton(QString("Button %1").arg(i), nullptr);
            buttons.push_back(button);
        }
        
        // Deallocate
        for (auto* button : buttons) {
            delete button;
        }
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        qDebug() << "Standard allocation: " << iterations << "allocations in" << elapsed.count() << "ms";
    }
}

void FluentPerformanceBenchmark::benchmarkComponentRendering() {
    // Create components for rendering test
    createTestComponents(50);
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QBENCHMARK {
        QElapsedTimer timer;
        timer.start();
        
        // Force repaint of all components
        for (auto* component : m_testComponents) {
            component->update();
        }
        
        // Process paint events
        QApplication::processEvents();
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        qDebug() << "Rendered" << m_testComponents.size() << "components in" << elapsed.count() << "ms";
    }
    
    m_testWidget->hide();
}

void FluentPerformanceBenchmark::benchmarkAnimationPerformance() {
    // Create buttons for animation test
    const int buttonCount = 20;
    std::vector<FluentButton*> buttons;
    
    for (int i = 0; i < buttonCount; ++i) {
        auto* button = new FluentButton(QString("Animated %1").arg(i), m_testWidget);
        button->setGeometry(i * 30, i * 20, 100, 30);
        buttons.push_back(button);
        m_testComponents.push_back(button);
    }
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QBENCHMARK {
        QElapsedTimer timer;
        timer.start();
        
        // Trigger hover animations on all buttons
        for (auto* button : buttons) {
            QHoverEvent hoverEvent(QEvent::HoverEnter, QPointF(50, 15), QPointF(50, 15));
            QApplication::sendEvent(button, &hoverEvent);
        }
        
        // Wait for animations to complete
        QTest::qWait(300);
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        qDebug() << "Animated" << buttonCount << "buttons in" << elapsed.count() << "ms";
    }
    
    m_testWidget->hide();
}

void FluentPerformanceBenchmark::stressTestManyComponents() {
    const int componentCount = 2000;
    
    QElapsedTimer timer;
    timer.start();
    
    size_t initialMemory = m_performanceMonitor->currentMemoryUsage();
    double initialFrameRate = m_performanceMonitor->currentFrameRate();
    
    // Create many components
    for (int i = 0; i < componentCount; ++i) {
        QWidget* component = nullptr;
        
        switch (i % 4) {
        case 0:
            component = new FluentButton(QString("Button %1").arg(i), m_testWidget);
            break;
        case 1:
            component = new FluentTextInput(m_testWidget);
            break;
        case 2:
            component = new FluentCard(m_testWidget);
            break;
        case 3:
            component = new FluentProgressBar(m_testWidget);
            break;
        }
        
        if (component) {
            m_testComponents.push_back(component);
        }
    }
    
    auto elapsed = std::chrono::milliseconds(timer.elapsed());
    size_t finalMemory = m_performanceMonitor->currentMemoryUsage();
    double finalFrameRate = m_performanceMonitor->currentFrameRate();
    
    qDebug() << "Stress test results:";
    qDebug() << "  Created" << componentCount << "components in" << elapsed.count() << "ms";
    qDebug() << "  Memory usage:" << (finalMemory - initialMemory) / (1024*1024) << "MB";
    qDebug() << "  Frame rate: " << initialFrameRate << "->" << finalFrameRate << "FPS";
    
    // Performance assertions
    QVERIFY2(finalFrameRate > 20.0, "Frame rate should remain above 20 FPS under stress");
    QVERIFY2((finalMemory - initialMemory) < 100 * 1024 * 1024, "Memory usage should be reasonable");
}

void FluentPerformanceBenchmark::benchmarkComplexUICreation() {
    QBENCHMARK {
        QElapsedTimer timer;
        timer.start();
        
        // Create a complex UI layout
        auto* mainLayout = new QVBoxLayout(m_testWidget);
        
        // Header with buttons
        auto* headerLayout = new QHBoxLayout();
        for (int i = 0; i < 5; ++i) {
            auto* button = new FluentButton(QString("Header %1").arg(i), m_testWidget);
            headerLayout->addWidget(button);
            m_testComponents.push_back(button);
        }
        mainLayout->addLayout(headerLayout);
        
        // Content area with cards
        for (int i = 0; i < 10; ++i) {
            auto* card = new FluentCard(m_testWidget);
            card->setTitle(QString("Card %1").arg(i));
            card->setContent("Complex content with multiple elements");
            
            // Add components to card
            auto* cardButton = new FluentButton("Action", card);
            auto* cardInput = new FluentTextInput(card);
            auto* cardProgress = new FluentProgressBar(card);
            cardProgress->setValue(i * 10);
            
            mainLayout->addWidget(card);
            m_testComponents.insert(m_testComponents.end(), {card, cardButton, cardInput, cardProgress});
        }
        
        auto elapsed = std::chrono::milliseconds(timer.elapsed());
        qDebug() << "Created complex UI in" << elapsed.count() << "ms";
    }
}

void FluentPerformanceBenchmark::createTestComponents(int count) {
    for (int i = 0; i < count; ++i) {
        auto* button = new FluentButton(QString("Test %1").arg(i), m_testWidget);
        button->setGeometry((i % 10) * 80, (i / 10) * 40, 75, 35);
        m_testComponents.push_back(button);
    }
}

void FluentPerformanceBenchmark::measureMemoryUsage(const QString& testName) {
    size_t currentUsage = m_performanceMonitor->currentMemoryUsage();
    qDebug() << testName << "memory usage:" << currentUsage / (1024*1024) << "MB";
}

double FluentPerformanceBenchmark::calculateOperationsPerSecond(int operations, std::chrono::milliseconds duration) {
    if (duration.count() == 0) return 0.0;
    return (operations * 1000.0) / duration.count();
}

QTEST_MAIN(FluentPerformanceBenchmark)
#include "FluentPerformanceBenchmark.moc"
