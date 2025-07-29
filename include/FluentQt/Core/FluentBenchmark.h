// include/FluentQt/Core/FluentBenchmark.h
#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QList>
#include <QString>
#include <QVariant>
#include <chrono>
#include <functional>
#include <memory>

namespace FluentQt::Core {

// Benchmark test case
struct BenchmarkTestCase {
    QString name;
    QString description;
    std::function<void()> testFunction;
    int iterations{1000};
    std::chrono::milliseconds timeout{5000};
    bool enabled{true};
};

// Benchmark result
struct BenchmarkTestResult {
    QString testName;
    bool passed{false};
    std::chrono::milliseconds totalTime{0};
    std::chrono::milliseconds averageTime{0};
    std::chrono::milliseconds minTime{std::chrono::milliseconds::max()};
    std::chrono::milliseconds maxTime{0};
    double operationsPerSecond{0.0};
    size_t memoryUsed{0};
    QString errorMessage;
    QVariantMap additionalData;
};

// Benchmark suite for component performance testing
class FluentBenchmarkSuite : public QObject {
    Q_OBJECT

public:
    explicit FluentBenchmarkSuite(const QString& suiteName, QObject* parent = nullptr);
    
    // Test case management
    void addTestCase(const BenchmarkTestCase& testCase);
    void addTestCase(const QString& name, std::function<void()> testFunction, 
                     int iterations = 1000, const QString& description = "");
    void removeTestCase(const QString& name);
    void clearTestCases();
    
    // Execution
    void runAllTests();
    void runTest(const QString& testName);
    void runTestsMatching(const QString& pattern);
    
    // Results
    QList<BenchmarkTestResult> getResults() const { return m_results; }
    BenchmarkTestResult getResult(const QString& testName) const;
    void clearResults();
    
    // Configuration
    void setDefaultIterations(int iterations) { m_defaultIterations = iterations; }
    void setDefaultTimeout(std::chrono::milliseconds timeout) { m_defaultTimeout = timeout; }
    void setWarmupIterations(int warmup) { m_warmupIterations = warmup; }
    
    // Reporting
    QString generateReport() const;
    QString generateCsvReport() const;
    void saveReport(const QString& filename) const;

signals:
    void testStarted(const QString& testName);
    void testCompleted(const QString& testName, const BenchmarkTestResult& result);
    void suiteCompleted(const QList<BenchmarkTestResult>& results);
    void progressUpdated(int current, int total);

private:
    void runSingleTest(const BenchmarkTestCase& testCase);
    void warmupTest(const std::function<void()>& testFunction);
    size_t measureMemoryUsage() const;

private:
    QString m_suiteName;
    QList<BenchmarkTestCase> m_testCases;
    QList<BenchmarkTestResult> m_results;
    
    int m_defaultIterations{1000};
    std::chrono::milliseconds m_defaultTimeout{5000};
    int m_warmupIterations{100};
};

// Component-specific benchmark utilities
class FluentComponentBenchmark {
public:
    // Widget creation benchmarks
    static BenchmarkTestCase createWidgetCreationTest(const QString& componentName,
                                                      std::function<QWidget*()> factory);
    
    // Rendering benchmarks
    static BenchmarkTestCase createRenderingTest(const QString& componentName,
                                                 QWidget* widget);
    
    // Animation benchmarks
    static BenchmarkTestCase createAnimationTest(const QString& componentName,
                                                 std::function<void()> animationTrigger);
    
    // Memory usage benchmarks
    static BenchmarkTestCase createMemoryTest(const QString& componentName,
                                              std::function<void()> operation);
    
    // Event handling benchmarks
    static BenchmarkTestCase createEventTest(const QString& componentName,
                                             QWidget* widget,
                                             std::function<void()> eventTrigger);
};

// Global benchmark registry
class FluentBenchmarkRegistry : public QObject {
    Q_OBJECT

public:
    static FluentBenchmarkRegistry& instance();
    
    // Suite management
    void registerSuite(std::shared_ptr<FluentBenchmarkSuite> suite);
    void unregisterSuite(const QString& suiteName);
    std::shared_ptr<FluentBenchmarkSuite> getSuite(const QString& suiteName) const;
    QStringList getSuiteNames() const;
    
    // Global execution
    void runAllSuites();
    void runSuite(const QString& suiteName);
    void runSuitesMatching(const QString& pattern);
    
    // Global reporting
    QString generateGlobalReport() const;
    void saveGlobalReport(const QString& filename) const;
    
    // Built-in benchmark suites
    void registerBuiltInSuites();

signals:
    void suiteStarted(const QString& suiteName);
    void suiteCompleted(const QString& suiteName, const QList<BenchmarkTestResult>& results);
    void allSuitesCompleted();

private:
    FluentBenchmarkRegistry() = default;
    void createCoreBenchmarks();
    void createComponentBenchmarks();
    void createAnimationBenchmarks();
    void createThemeBenchmarks();

private:
    QList<std::shared_ptr<FluentBenchmarkSuite>> m_suites;
};

// Convenience macros for benchmark creation
#define FLUENT_BENCHMARK_SUITE(suiteName) \
    auto suite = std::make_shared<FluentQt::Core::FluentBenchmarkSuite>(suiteName); \
    FluentQt::Core::FluentBenchmarkRegistry::instance().registerSuite(suite)

#define FLUENT_BENCHMARK_TEST(suite, testName, iterations, code) \
    suite->addTestCase(testName, [=]() { code; }, iterations)

#define FLUENT_BENCHMARK_SIMPLE(suite, testName, code) \
    suite->addTestCase(testName, [=]() { code; })

} // namespace FluentQt::Core
