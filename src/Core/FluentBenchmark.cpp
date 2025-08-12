// src/Core/FluentBenchmark.cpp
#include "FluentQt/Core/FluentBenchmark.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPainter>
#include <QRegularExpression>
#include <QTextStream>
#include <QWidget>
#include <algorithm>
#include <numeric>
#include "FluentQt/Core/FluentPerformance.h"

namespace FluentQt::Core {

// FluentBenchmarkSuite Implementation
FluentBenchmarkSuite::FluentBenchmarkSuite(const QString& suiteName,
                                           QObject* parent)
    : QObject(parent), m_suiteName(suiteName) {}

void FluentBenchmarkSuite::addTestCase(const BenchmarkTestCase& testCase) {
    m_testCases.append(testCase);
}

void FluentBenchmarkSuite::addTestCase(const QString& name,
                                       std::function<void()> testFunction,
                                       int iterations,
                                       const QString& description) {
    BenchmarkTestCase testCase;
    testCase.name = name;
    testCase.description = description;
    testCase.testFunction = testFunction;
    testCase.iterations = iterations > 0 ? iterations : m_defaultIterations;
    testCase.timeout = m_defaultTimeout;
    testCase.enabled = true;

    addTestCase(testCase);
}

void FluentBenchmarkSuite::removeTestCase(const QString& name) {
    m_testCases.removeIf(
        [&name](const BenchmarkTestCase& test) { return test.name == name; });
}

void FluentBenchmarkSuite::clearTestCases() { m_testCases.clear(); }

void FluentBenchmarkSuite::runAllTests() {
    m_results.clear();

    int current = 0;
    for (const auto& testCase : m_testCases) {
        if (testCase.enabled) {
            emit testStarted(testCase.name);
            runSingleTest(testCase);
            emit progressUpdated(++current, m_testCases.size());
        }
    }

    emit suiteCompleted(m_results);
}

void FluentBenchmarkSuite::runTest(const QString& testName) {
    auto it = std::find_if(m_testCases.begin(), m_testCases.end(),
                           [&testName](const BenchmarkTestCase& test) {
                               return test.name == testName;
                           });

    if (it != m_testCases.end() && it->enabled) {
        emit testStarted(testName);
        runSingleTest(*it);
    }
}

void FluentBenchmarkSuite::runTestsMatching(const QString& pattern) {
    QRegularExpression regex(pattern);

    for (const auto& testCase : m_testCases) {
        if (testCase.enabled && regex.match(testCase.name).hasMatch()) {
            emit testStarted(testCase.name);
            runSingleTest(testCase);
        }
    }
}

void FluentBenchmarkSuite::runSingleTest(const BenchmarkTestCase& testCase) {
    BenchmarkTestResult result;
    result.testName = testCase.name;

    try {
        // Warmup
        if (m_warmupIterations > 0) {
            warmupTest(testCase.testFunction);
        }

        // Measure initial memory
        size_t initialMemory = measureMemoryUsage();

        // Run benchmark
        QElapsedTimer totalTimer;
        totalTimer.start();

        std::vector<std::chrono::milliseconds> iterationTimes;
        iterationTimes.reserve(testCase.iterations);

        for (int i = 0; i < testCase.iterations; ++i) {
            QElapsedTimer iterationTimer;
            iterationTimer.start();

            testCase.testFunction();

            auto elapsed = std::chrono::milliseconds(iterationTimer.elapsed());
            iterationTimes.push_back(elapsed);

            // Check timeout
            if (totalTimer.elapsed() > testCase.timeout.count()) {
                result.errorMessage = QString("Test timed out after %1ms")
                                          .arg(totalTimer.elapsed());
                break;
            }
        }

        // Calculate statistics
        if (!iterationTimes.empty()) {
            result.totalTime = std::chrono::milliseconds(totalTimer.elapsed());
            result.averageTime =
                std::accumulate(iterationTimes.begin(), iterationTimes.end(),
                                std::chrono::milliseconds(0)) /
                iterationTimes.size();
            result.minTime =
                *std::min_element(iterationTimes.begin(), iterationTimes.end());
            result.maxTime =
                *std::max_element(iterationTimes.begin(), iterationTimes.end());
            result.operationsPerSecond =
                (iterationTimes.size() * 1000.0) / result.totalTime.count();
            result.passed = result.errorMessage.isEmpty();
        }

        // Measure final memory
        size_t finalMemory = measureMemoryUsage();
        result.memoryUsed =
            (finalMemory > initialMemory) ? finalMemory - initialMemory : 0;

    } catch (const std::exception& e) {
        result.errorMessage = QString("Exception: %1").arg(e.what());
        result.passed = false;
    } catch (...) {
        result.errorMessage = "Unknown exception occurred";
        result.passed = false;
    }

    m_results.append(result);
    emit testCompleted(testCase.name, result);
}

void FluentBenchmarkSuite::warmupTest(
    const std::function<void()>& testFunction) {
    for (int i = 0; i < m_warmupIterations; ++i) {
        testFunction();
    }
}

size_t FluentBenchmarkSuite::measureMemoryUsage() const {
    return FluentPerformanceMonitor::instance().currentMemoryUsage();
}

BenchmarkTestResult FluentBenchmarkSuite::getResult(
    const QString& testName) const {
    auto it = std::find_if(m_results.begin(), m_results.end(),
                           [&testName](const BenchmarkTestResult& result) {
                               return result.testName == testName;
                           });

    return (it != m_results.end()) ? *it : BenchmarkTestResult{};
}

void FluentBenchmarkSuite::clearResults() { m_results.clear(); }

QString FluentBenchmarkSuite::generateReport() const {
    QString report;
    QTextStream stream(&report);

    stream << "=== Benchmark Suite: " << m_suiteName << " ===\n\n";

    for (const auto& result : m_results) {
        stream << "Test: " << result.testName << "\n";
        stream << "  Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";

        if (!result.passed && !result.errorMessage.isEmpty()) {
            stream << "  Error: " << result.errorMessage << "\n";
        } else {
            stream << "  Total Time: " << result.totalTime.count() << "ms\n";
            stream << "  Average Time: " << result.averageTime.count()
                   << "ms\n";
            stream << "  Min Time: " << result.minTime.count() << "ms\n";
            stream << "  Max Time: " << result.maxTime.count() << "ms\n";
            stream << "  Operations/sec: "
                   << QString::number(result.operationsPerSecond, 'f', 2)
                   << "\n";
            stream << "  Memory Used: " << result.memoryUsed << " bytes\n";
        }
        stream << "\n";
    }

    return report;
}

QString FluentBenchmarkSuite::generateCsvReport() const {
    QString csv;
    QTextStream stream(&csv);

    // Header
    stream << "Test Name,Status,Total Time (ms),Average Time (ms),Min Time "
              "(ms),Max Time (ms),Operations/sec,Memory Used (bytes),Error\n";

    // Data
    for (const auto& result : m_results) {
        stream << result.testName << ","
               << (result.passed ? "PASSED" : "FAILED") << ","
               << result.totalTime.count() << "," << result.averageTime.count()
               << "," << result.minTime.count() << "," << result.maxTime.count()
               << "," << QString::number(result.operationsPerSecond, 'f', 2)
               << "," << result.memoryUsed << "," << result.errorMessage
               << "\n";
    }

    return csv;
}

void FluentBenchmarkSuite::saveReport(const QString& filename) const {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        if (filename.endsWith(".csv")) {
            out << generateCsvReport();
        } else {
            out << generateReport();
        }
    }
}

}  // namespace FluentQt::Core
