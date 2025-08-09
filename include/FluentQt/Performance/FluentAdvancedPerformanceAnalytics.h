// include/FluentQt/Performance/FluentAdvancedPerformanceAnalytics.h
#pragma once

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QMap>
#include <QQueue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <memory>
#include <chrono>
#include <functional>
#include <atomic>

#include "FluentQt/Core/FluentPerformance.h"

namespace FluentQt::Performance {

// Advanced performance metrics with predictive capabilities
struct FluentAdvancedMetrics {
    // Core metrics (inherited from base)
    Core::PerformanceMetrics baseMetrics;
    
    // Predictive metrics
    double predictedFrameRate{60.0};
    double predictedMemoryUsage{0.0};
    double predictedCpuUsage{0.0};
    std::chrono::milliseconds predictedResponseTime{0};
    
    // Trend analysis
    double frameRateTrend{0.0};        // Positive = improving, negative = degrading
    double memoryTrend{0.0};
    double cpuTrend{0.0};
    double responseTrend{0.0};
    
    // Performance patterns
    QString dominantPattern{"stable"};  // stable, increasing, decreasing, oscillating, chaotic
    double patternConfidence{0.0};      // 0.0 to 1.0
    std::chrono::seconds patternDuration{0};
    
    // System health indicators
    double systemStability{100.0};     // 0-100
    double performanceReliability{100.0}; // 0-100
    double userExperienceScore{100.0}; // 0-100
    double energyEfficiency{100.0};    // 0-100
    
    // Anomaly detection
    bool hasAnomalies{false};
    QStringList detectedAnomalies;
    double anomalySeverity{0.0};       // 0.0 to 1.0
    
    // Optimization opportunities
    QMap<QString, double> optimizationPotential; // Component -> potential improvement %
    QStringList recommendedActions;
    double estimatedImprovement{0.0};  // Expected performance gain %
    
    // Contextual information
    QString currentWorkload{"normal"};  // light, normal, heavy, extreme
    QString systemState{"optimal"};     // optimal, degraded, critical, recovery
    QMap<QString, QVariant> contextualData;
    
    // Timestamp and metadata
    std::chrono::steady_clock::time_point timestamp;
    QString sessionId;
    int sampleCount{0};
};

// Performance prediction models
enum class FluentPredictionModel {
    Linear,                 // Linear regression
    Exponential,            // Exponential smoothing
    ARIMA,                  // AutoRegressive Integrated Moving Average
    NeuralNetwork,          // Simple neural network
    Ensemble,               // Combination of multiple models
    MachineLearning,        // Advanced ML model
    Custom                  // Custom prediction function
};

// Analytics configuration
struct FluentAnalyticsConfig {
    // Prediction settings
    FluentPredictionModel predictionModel{FluentPredictionModel::Ensemble};
    std::chrono::seconds predictionHorizon{60};
    int historySizeForPrediction{100};
    double predictionConfidenceThreshold{0.7};
    
    // Anomaly detection
    bool enableAnomalyDetection{true};
    double anomalyThreshold{2.0};      // Standard deviations
    int anomalyWindowSize{20};
    bool enableRealTimeAnomalyDetection{true};
    
    // Pattern recognition
    bool enablePatternRecognition{true};
    int patternAnalysisWindow{50};
    double patternMatchThreshold{0.8};
    
    // Optimization analysis
    bool enableOptimizationAnalysis{true};
    std::chrono::seconds optimizationAnalysisInterval{30};
    double optimizationThreshold{5.0}; // Minimum improvement % to recommend
    
    // Data collection
    std::chrono::milliseconds samplingInterval{100};
    int maxHistorySize{1000};
    bool enableDataPersistence{true};
    QString dataStoragePath;
    
    // Machine learning
    bool enableMachineLearning{false};
    QString modelPath;
    bool enableOnlineLearning{false};
    double learningRate{0.01};
    
    // Reporting and alerts
    bool enableRealTimeReporting{true};
    bool enablePerformanceAlerts{true};
    std::chrono::seconds alertCooldown{300}; // 5 minutes
    
    // Privacy and security
    bool enableDataAnonymization{true};
    bool enableLocalProcessingOnly{true};
    bool enableTelemetry{false};
    
    // Custom settings
    QMap<QString, QVariant> customSettings;
};

// Performance prediction result
struct FluentPredictionResult {
    double predictedValue{0.0};
    double confidence{0.0};           // 0.0 to 1.0
    std::chrono::seconds timeHorizon{0};
    QString predictionModel;
    QMap<QString, double> modelParameters;
    QString explanation;
    bool isReliable{true};
};

// Advanced performance analytics engine
class FluentAdvancedPerformanceAnalytics : public QObject {
    Q_OBJECT

public:
    static FluentAdvancedPerformanceAnalytics& instance();
    
    // Configuration
    void setAnalyticsConfig(const FluentAnalyticsConfig& config);
    FluentAnalyticsConfig getAnalyticsConfig() const { return m_config; }
    void enableAnalytics(bool enabled = true);
    bool isAnalyticsEnabled() const { return m_analyticsEnabled; }
    
    // Data collection and analysis
    void startAnalytics();
    void stopAnalytics();
    void collectMetrics();
    FluentAdvancedMetrics getCurrentAnalytics() const;
    QList<FluentAdvancedMetrics> getAnalyticsHistory(std::chrono::seconds duration) const;
    
    // Predictive analytics
    FluentPredictionResult predictFrameRate(std::chrono::seconds horizon = std::chrono::seconds(60));
    FluentPredictionResult predictMemoryUsage(std::chrono::seconds horizon = std::chrono::seconds(60));
    FluentPredictionResult predictCpuUsage(std::chrono::seconds horizon = std::chrono::seconds(60));
    FluentPredictionResult predictResponseTime(std::chrono::seconds horizon = std::chrono::seconds(60));
    FluentPredictionResult predictSystemLoad(std::chrono::seconds horizon = std::chrono::seconds(60));
    
    // Anomaly detection
    void enableAnomalyDetection(bool enabled = true);
    bool isAnomalyDetectionEnabled() const { return m_config.enableAnomalyDetection; }
    QStringList detectAnomalies(const FluentAdvancedMetrics& metrics);
    double calculateAnomalyScore(const FluentAdvancedMetrics& metrics);
    void setAnomalyThreshold(double threshold);
    
    // Pattern recognition
    void enablePatternRecognition(bool enabled = true);
    QString recognizePerformancePattern(const QList<FluentAdvancedMetrics>& history);
    double calculatePatternConfidence(const QString& pattern, const QList<FluentAdvancedMetrics>& history);
    QStringList getKnownPatterns() const;
    void registerCustomPattern(const QString& name, std::function<bool(const QList<FluentAdvancedMetrics>&)> detector);
    
    // Optimization analysis
    QMap<QString, double> analyzeOptimizationOpportunities();
    QStringList generateOptimizationRecommendations();
    double estimateOptimizationImpact(const QString& optimization);
    void applyOptimizationRecommendation(const QString& recommendation);
    
    // Trend analysis
    double calculateTrend(const QString& metric, std::chrono::seconds window = std::chrono::seconds(300));
    QString analyzeTrendDirection(const QString& metric);
    double predictTrendContinuation(const QString& metric, std::chrono::seconds horizon);
    
    // System health assessment
    double calculateSystemStability();
    double calculatePerformanceReliability();
    double calculateUserExperienceScore();
    double calculateEnergyEfficiency();
    QString assessSystemHealth();
    
    // Machine learning integration
    void enableMachineLearning(bool enabled = true);
    void trainPredictionModel(const QList<FluentAdvancedMetrics>& trainingData);
    void updateModelWithNewData(const FluentAdvancedMetrics& newData);
    void saveModel(const QString& filePath);
    void loadModel(const QString& filePath);
    
    // Data management
    void exportAnalyticsData(const QString& filePath, const QString& format = "json");
    void importAnalyticsData(const QString& filePath);
    void clearAnalyticsHistory();
    void setDataRetentionPolicy(std::chrono::hours retentionPeriod);
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enabled = true);
    void setMonitoringInterval(std::chrono::milliseconds interval);
    void addCustomMetric(const QString& name, std::function<double()> collector);
    void removeCustomMetric(const QString& name);
    
    // Performance benchmarking
    void runPerformanceBenchmark(const QString& benchmarkName);
    void registerCustomBenchmark(const QString& name, std::function<double()> benchmark);
    QMap<QString, double> getBenchmarkResults() const;
    void compareBenchmarkResults(const QString& baseline, const QString& current);
    
    // Alerting and notifications
    void enablePerformanceAlerts(bool enabled = true);
    void setAlertThreshold(const QString& metric, double threshold);
    void addAlertCallback(std::function<void(const QString&, const FluentAdvancedMetrics&)> callback);
    void removeAlertCallback(int callbackId);

signals:
    void analyticsUpdated(const FluentAdvancedMetrics& metrics);
    void anomalyDetected(const QString& anomaly, double severity);
    void patternRecognized(const QString& pattern, double confidence);
    void optimizationOpportunityFound(const QString& opportunity, double potential);
    void predictionUpdated(const QString& metric, const FluentPredictionResult& prediction);
    void systemHealthChanged(const QString& status, double score);
    void performanceAlert(const QString& alert, const FluentAdvancedMetrics& metrics);
    void benchmarkCompleted(const QString& benchmark, double result);
    void modelTrainingCompleted(const QString& model, double accuracy);

private slots:
    void onAnalyticsTimer();
    void onPredictionTimer();
    void onAnomalyCheckTimer();
    void onOptimizationTimer();
    void onDataCleanupTimer();

private:
    FluentAdvancedPerformanceAnalytics();
    
    // Data collection
    void collectBaseMetrics();
    void calculateDerivedMetrics();
    void updateTrends();
    void updatePatterns();
    void updatePredictions();
    
    // Prediction algorithms
    double linearPrediction(const QList<double>& values, int horizon);
    double exponentialSmoothing(const QList<double>& values, double alpha = 0.3);
    double arimaPrediction(const QList<double>& values, int horizon);
    double neuralNetworkPrediction(const QList<double>& values, int horizon);
    double ensemblePrediction(const QList<double>& values, int horizon);
    
    // Anomaly detection algorithms
    bool isStatisticalAnomaly(double value, const QList<double>& baseline);
    bool isPatternAnomaly(const FluentAdvancedMetrics& metrics);
    bool isContextualAnomaly(const FluentAdvancedMetrics& metrics);
    
    // Pattern recognition algorithms
    bool isStablePattern(const QList<FluentAdvancedMetrics>& history);
    bool isIncreasingPattern(const QList<FluentAdvancedMetrics>& history);
    bool isDecreasingPattern(const QList<FluentAdvancedMetrics>& history);
    bool isOscillatingPattern(const QList<FluentAdvancedMetrics>& history);
    bool isChaoticPattern(const QList<FluentAdvancedMetrics>& history);
    
    // Optimization analysis
    double analyzeComponentOptimization(const QString& component);
    double analyzeMemoryOptimization();
    double analyzeCpuOptimization();
    double analyzeRenderingOptimization();
    double analyzeAnimationOptimization();
    
    // Machine learning helpers
    void initializeMachineLearning();
    void preprocessData(QList<FluentAdvancedMetrics>& data);
    void normalizeFeatures(QList<double>& features);
    double evaluateModel(const QList<FluentAdvancedMetrics>& testData);
    
    // Data management
    void persistData();
    void loadPersistedData();
    void cleanupOldData();
    void anonymizeData(FluentAdvancedMetrics& metrics);

private:
    FluentAnalyticsConfig m_config;
    bool m_analyticsEnabled{false};
    
    // Timers
    QTimer* m_analyticsTimer;
    QTimer* m_predictionTimer;
    QTimer* m_anomalyTimer;
    QTimer* m_optimizationTimer;
    QTimer* m_cleanupTimer;
    
    // Data storage
    FluentAdvancedMetrics m_currentMetrics;
    QQueue<FluentAdvancedMetrics> m_metricsHistory;
    QMap<QString, QQueue<double>> m_metricTimeSeries;
    
    // Prediction models
    QMap<QString, QList<double>> m_predictionHistory;
    QMap<QString, FluentPredictionResult> m_latestPredictions;
    
    // Anomaly detection
    QMap<QString, QList<double>> m_baselineData;
    QStringList m_recentAnomalies;
    std::chrono::steady_clock::time_point m_lastAnomalyAlert;
    
    // Pattern recognition
    QMap<QString, std::function<bool(const QList<FluentAdvancedMetrics>&)>> m_customPatterns;
    QString m_currentPattern{"stable"};
    double m_currentPatternConfidence{0.0};
    
    // Optimization tracking
    QMap<QString, double> m_optimizationBaselines;
    QStringList m_appliedOptimizations;
    
    // Custom metrics and benchmarks
    QMap<QString, std::function<double()>> m_customMetrics;
    QMap<QString, std::function<double()>> m_customBenchmarks;
    QMap<QString, double> m_benchmarkResults;
    
    // Alerting
    QMap<QString, double> m_alertThresholds;
    QMap<int, std::function<void(const QString&, const FluentAdvancedMetrics&)>> m_alertCallbacks;
    int m_nextCallbackId{1};
    
    // Machine learning
    bool m_machineLearningEnabled{false};
    void* m_mlModel{nullptr}; // Platform-specific ML model handle
    QList<FluentAdvancedMetrics> m_trainingData;
    
    // Thread safety
    mutable QMutex m_analyticsMutex;
    mutable QMutex m_predictionMutex;
    mutable QMutex m_dataMutex;
    
    // Performance optimization
    std::atomic<bool> m_processingAnalytics{false};
    QThreadPool* m_analyticsThreadPool;
};

// Analytics utility functions
namespace FluentAnalyticsUtils {
    // Statistical functions
    double calculateMean(const QList<double>& values);
    double calculateStandardDeviation(const QList<double>& values);
    double calculateMedian(QList<double> values);
    double calculatePercentile(QList<double> values, double percentile);
    double calculateCorrelation(const QList<double>& x, const QList<double>& y);
    
    // Trend analysis
    double calculateLinearTrend(const QList<double>& values);
    double calculateTrendStrength(const QList<double>& values);
    QString classifyTrend(double trendValue);
    
    // Anomaly detection helpers
    bool isOutlier(double value, const QList<double>& dataset, double threshold = 2.0);
    QList<int> findOutliers(const QList<double>& values, double threshold = 2.0);
    double calculateAnomalyScore(double value, const QList<double>& baseline);
    
    // Pattern matching
    double calculatePatternSimilarity(const QList<double>& pattern1, const QList<double>& pattern2);
    QList<double> extractPattern(const QList<FluentAdvancedMetrics>& metrics, const QString& metricName);
    bool matchesKnownPattern(const QList<double>& data, const QList<double>& pattern, double threshold = 0.8);
    
    // Performance scoring
    double calculatePerformanceScore(const FluentAdvancedMetrics& metrics);
    double calculateStabilityScore(const QList<FluentAdvancedMetrics>& history);
    double calculateEfficiencyScore(const FluentAdvancedMetrics& metrics);
    
    // Data preprocessing
    QList<double> smoothData(const QList<double>& data, int windowSize = 5);
    QList<double> normalizeData(const QList<double>& data);
    QList<double> removeOutliers(const QList<double>& data, double threshold = 2.0);
    
    // Prediction helpers
    double extrapolateLinear(const QList<double>& values, int steps);
    double extrapolateExponential(const QList<double>& values, int steps, double alpha = 0.3);
    QList<double> generateForecast(const QList<double>& history, int horizon, const QString& method = "linear");
}

} // namespace FluentQt::Performance
