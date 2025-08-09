// include/FluentQt/Performance/FluentSystemIntegration.h
#pragma once

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QMutex>
#include <QMap>
#include <QQueue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <memory>
#include <chrono>
#include <atomic>

#ifdef Q_OS_WIN
#include <windows.h>
#include <pdh.h>
#include <psapi.h>
#include <powerbase.h>
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#elif defined(Q_OS_MACOS)
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <IOKit/IOKitLib.h>
#endif

#include "FluentAdvancedPerformanceAnalytics.h"

namespace FluentQt::Performance {

// System resource types
enum class FluentSystemResource {
    CPU,                    // CPU usage and information
    Memory,                 // RAM usage and information
    Disk,                   // Disk usage and I/O
    Network,                // Network usage and latency
    GPU,                    // GPU usage and information
    Battery,                // Battery level and power state
    Thermal,                // Temperature and thermal state
    Process,                // Process information
    System,                 // General system information
    Custom                  // Custom resource type
};

// System monitoring levels
enum class FluentMonitoringLevel {
    Basic,                  // Basic system metrics
    Standard,               // Standard monitoring
    Detailed,               // Detailed monitoring
    Comprehensive,          // Comprehensive monitoring
    Debug,                  // Debug-level monitoring
    Custom                  // Custom monitoring level
};

// System information structure
struct FluentSystemInfo {
    // Hardware information
    QString cpuModel;
    int cpuCores{0};
    int cpuThreads{0};
    double cpuFrequency{0.0}; // GHz
    qint64 totalMemory{0};    // Bytes
    qint64 availableMemory{0}; // Bytes
    QString gpuModel;
    qint64 gpuMemory{0};      // Bytes
    
    // Operating system information
    QString osName;
    QString osVersion;
    QString osArchitecture;
    QString kernelVersion;
    
    // Display information
    QList<QSize> screenResolutions;
    QList<double> screenRefreshRates;
    int primaryScreenIndex{0};
    
    // Storage information
    QMap<QString, qint64> diskSizes;      // Drive -> Size in bytes
    QMap<QString, qint64> diskFreeSpace;  // Drive -> Free space in bytes
    QMap<QString, QString> diskTypes;     // Drive -> Type (SSD, HDD, etc.)
    
    // Network information
    QStringList networkInterfaces;
    QMap<QString, QString> networkTypes;  // Interface -> Type (WiFi, Ethernet, etc.)
    QMap<QString, bool> networkStatus;    // Interface -> Connected status
    
    // Power information
    bool hasBattery{false};
    double batteryLevel{100.0};
    bool isCharging{false};
    QString powerProfile;
    
    // Thermal information
    QMap<QString, double> temperatures;   // Sensor -> Temperature in Celsius
    QMap<QString, int> fanSpeeds;        // Fan -> Speed in RPM
    QString thermalState;
    
    // Performance capabilities
    bool supportsHardwareAcceleration{false};
    bool supportsMultithreading{true};
    bool supportsVirtualization{false};
    QStringList supportedInstructionSets;
    
    // System limits and capabilities
    int maxFileDescriptors{0};
    int maxProcesses{0};
    qint64 maxMemoryPerProcess{0};
    
    // Timestamp
    std::chrono::steady_clock::time_point lastUpdate;
};

// Real-time system metrics
struct FluentSystemMetrics {
    // CPU metrics
    double cpuUsageTotal{0.0};           // Overall CPU usage %
    QList<double> cpuUsagePerCore;       // Per-core CPU usage %
    double cpuFrequencyCurrent{0.0};     // Current CPU frequency
    double cpuTemperature{0.0};          // CPU temperature
    int cpuProcesses{0};                 // Number of processes
    int cpuThreads{0};                   // Number of threads
    
    // Memory metrics
    qint64 memoryUsed{0};                // Used memory in bytes
    qint64 memoryAvailable{0};           // Available memory in bytes
    qint64 memoryBuffered{0};            // Buffered memory in bytes
    qint64 memoryCached{0};              // Cached memory in bytes
    double memoryPressure{0.0};          // Memory pressure %
    int memoryPageFaults{0};             // Page faults per second
    
    // Disk metrics
    QMap<QString, double> diskUsage;     // Drive -> Usage %
    QMap<QString, double> diskReadRate;  // Drive -> Read rate MB/s
    QMap<QString, double> diskWriteRate; // Drive -> Write rate MB/s
    QMap<QString, int> diskIOPS;         // Drive -> IOPS
    QMap<QString, double> diskLatency;   // Drive -> Average latency ms
    
    // Network metrics
    QMap<QString, double> networkReceiveRate; // Interface -> Receive rate MB/s
    QMap<QString, double> networkSendRate;    // Interface -> Send rate MB/s
    QMap<QString, int> networkConnections;    // Interface -> Active connections
    QMap<QString, double> networkLatency;     // Interface -> Latency ms
    QMap<QString, int> networkErrors;         // Interface -> Error count
    
    // GPU metrics
    double gpuUsage{0.0};                // GPU usage %
    double gpuMemoryUsage{0.0};          // GPU memory usage %
    double gpuTemperature{0.0};          // GPU temperature
    double gpuPowerUsage{0.0};           // GPU power usage watts
    int gpuFanSpeed{0};                  // GPU fan speed RPM
    
    // Power metrics
    double batteryLevel{100.0};          // Battery level %
    double powerConsumption{0.0};        // Power consumption watts
    bool isOnBattery{false};             // Running on battery
    std::chrono::minutes batteryTimeRemaining{0}; // Estimated time remaining
    QString powerProfile;                // Current power profile
    
    // Thermal metrics
    QMap<QString, double> temperatures;  // Sensor -> Temperature
    QMap<QString, int> fanSpeeds;       // Fan -> Speed RPM
    QString thermalState;                // Overall thermal state
    bool thermalThrottling{false};       // Is thermal throttling active
    
    // Process metrics
    int totalProcesses{0};               // Total number of processes
    int activeProcesses{0};              // Active processes
    int zombieProcesses{0};              // Zombie processes
    double systemLoadAverage{0.0};       // System load average
    
    // Timestamp and metadata
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::milliseconds collectionTime{0}; // Time taken to collect metrics
    bool isValid{true};                  // Are metrics valid
    QString errorMessage;                // Error message if invalid
};

// System integration configuration
struct FluentSystemIntegrationConfig {
    FluentMonitoringLevel monitoringLevel{FluentMonitoringLevel::Standard};
    std::chrono::milliseconds updateInterval{1000};
    
    // Resource monitoring flags
    bool monitorCPU{true};
    bool monitorMemory{true};
    bool monitorDisk{true};
    bool monitorNetwork{true};
    bool monitorGPU{true};
    bool monitorBattery{true};
    bool monitorThermal{true};
    bool monitorProcesses{true};
    
    // Advanced monitoring options
    bool enablePerCoreMonitoring{false};
    bool enablePerProcessMonitoring{false};
    bool enableNetworkTrafficAnalysis{false};
    bool enableDiskIOAnalysis{false};
    bool enableGPUMemoryTracking{false};
    bool enableThermalPrediction{false};
    
    // Performance optimization
    bool enableCaching{true};
    bool enableBatching{true};
    bool enableAsyncCollection{true};
    int maxCacheSize{1000};
    std::chrono::seconds cacheExpiry{60};
    
    // Platform-specific options
    bool useNativeAPIs{true};
    bool usePerfCounters{true};  // Windows
    bool useProcFS{true};        // Linux
    bool useIOKit{true};         // macOS
    
    // Alert thresholds
    double cpuAlertThreshold{80.0};
    double memoryAlertThreshold{85.0};
    double diskAlertThreshold{90.0};
    double temperatureAlertThreshold{80.0};
    double batteryAlertThreshold{20.0};
    
    // Custom settings
    QMap<QString, QVariant> customSettings;
};

// Advanced system integration manager
class FluentSystemIntegrationManager : public QObject {
    Q_OBJECT

public:
    static FluentSystemIntegrationManager& instance();
    
    // Configuration
    void setConfig(const FluentSystemIntegrationConfig& config);
    FluentSystemIntegrationConfig getConfig() const { return m_config; }
    void setMonitoringLevel(FluentMonitoringLevel level);
    FluentMonitoringLevel getMonitoringLevel() const { return m_config.monitoringLevel; }
    
    // System information
    FluentSystemInfo getSystemInfo();
    void refreshSystemInfo();
    bool isSystemInfoCached() const;
    void clearSystemInfoCache();
    
    // Real-time monitoring
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring();
    void resumeMonitoring();
    bool isMonitoring() const { return m_monitoringActive; }
    
    // Metrics collection
    FluentSystemMetrics getCurrentMetrics();
    QList<FluentSystemMetrics> getMetricsHistory(std::chrono::seconds duration);
    void clearMetricsHistory();
    
    // Resource-specific monitoring
    void enableResourceMonitoring(FluentSystemResource resource, bool enabled = true);
    bool isResourceMonitoringEnabled(FluentSystemResource resource) const;
    FluentSystemMetrics getResourceMetrics(FluentSystemResource resource);
    
    // Process monitoring
    void enableProcessMonitoring(bool enabled = true);
    QList<QMap<QString, QVariant>> getProcessList();
    QMap<QString, QVariant> getProcessInfo(qint64 pid);
    void monitorProcess(qint64 pid, bool monitor = true);
    void unmonitorProcess(qint64 pid);
    
    // Network monitoring
    void enableNetworkMonitoring(bool enabled = true);
    QStringList getNetworkInterfaces();
    QMap<QString, QVariant> getNetworkInterfaceInfo(const QString& interface);
    double measureNetworkLatency(const QString& host, int timeout = 5000);
    
    // Disk monitoring
    void enableDiskMonitoring(bool enabled = true);
    QStringList getDiskDrives();
    QMap<QString, QVariant> getDiskInfo(const QString& drive);
    double measureDiskPerformance(const QString& drive);
    
    // GPU monitoring
    void enableGPUMonitoring(bool enabled = true);
    QList<QMap<QString, QVariant>> getGPUList();
    QMap<QString, QVariant> getGPUInfo(int gpuIndex = 0);
    
    // Power and thermal monitoring
    void enablePowerMonitoring(bool enabled = true);
    void enableThermalMonitoring(bool enabled = true);
    QMap<QString, double> getThermalSensors();
    QMap<QString, int> getFanSpeeds();
    QString getPowerProfile();
    void setPowerProfile(const QString& profile);
    
    // System optimization
    void optimizeForPerformance();
    void optimizeForPowerSaving();
    void optimizeForBalance();
    void applyCustomOptimization(const QMap<QString, QVariant>& settings);
    
    // Alert and notification system
    void enableAlerts(bool enabled = true);
    void setAlertThreshold(FluentSystemResource resource, double threshold);
    void addAlertCallback(std::function<void(FluentSystemResource, const QString&)> callback);
    void removeAlertCallback(int callbackId);
    
    // Integration with analytics
    void connectToAnalytics(FluentAdvancedPerformanceAnalytics* analytics);
    void enableAnalyticsIntegration(bool enabled = true);
    void syncWithAnalytics();
    
    // Data export and logging
    void enableLogging(bool enabled = true);
    void setLogLevel(const QString& level);
    void exportMetrics(const QString& filePath, const QString& format = "json");
    void exportSystemInfo(const QString& filePath, const QString& format = "json");
    
    // Platform-specific features
    void enablePlatformSpecificMonitoring(bool enabled = true);
    QStringList getPlatformSpecificMetrics();
    QVariant getPlatformSpecificMetric(const QString& metric);

signals:
    void systemInfoUpdated(const FluentSystemInfo& info);
    void metricsUpdated(const FluentSystemMetrics& metrics);
    void resourceAlert(FluentSystemResource resource, const QString& message);
    void systemStateChanged(const QString& state);
    void thermalStateChanged(const QString& state);
    void powerStateChanged(const QString& state);
    void processStarted(qint64 pid, const QString& name);
    void processEnded(qint64 pid, const QString& name);
    void networkStateChanged(const QString& interface, bool connected);
    void diskStateChanged(const QString& drive, const QString& state);

private slots:
    void onMonitoringTimer();
    void onSystemInfoTimer();
    void onAlertCheckTimer();
    void onCacheCleanupTimer();

private:
    FluentSystemIntegrationManager();
    
    // System information collection
    void collectSystemInfo();
    void collectHardwareInfo();
    void collectOSInfo();
    void collectDisplayInfo();
    void collectStorageInfo();
    void collectNetworkInfo();
    void collectPowerInfo();
    void collectThermalInfo();
    
    // Metrics collection
    void collectAllMetrics();
    void collectCPUMetrics();
    void collectMemoryMetrics();
    void collectDiskMetrics();
    void collectNetworkMetrics();
    void collectGPUMetrics();
    void collectPowerMetrics();
    void collectThermalMetrics();
    void collectProcessMetrics();
    
    // Platform-specific implementations
    void initializePlatformMonitoring();
    void cleanupPlatformMonitoring();
    
#ifdef Q_OS_WIN
    void collectWindowsMetrics();
    void initializeWindowsCounters();
    void cleanupWindowsCounters();
#elif defined(Q_OS_LINUX)
    void collectLinuxMetrics();
    void readProcFS();
    void readSysFS();
#elif defined(Q_OS_MACOS)
    void collectMacOSMetrics();
    void queryIOKit();
    void querySysctl();
#endif
    
    // Alert processing
    void checkAlerts(const FluentSystemMetrics& metrics);
    void processAlert(FluentSystemResource resource, const QString& message);
    
    // Data management
    void cacheMetrics(const FluentSystemMetrics& metrics);
    void cleanupCache();
    void persistData();
    void loadPersistedData();
    
    // Optimization helpers
    void applyPerformanceOptimizations();
    void applyPowerSavingOptimizations();
    void applyBalancedOptimizations();

private:
    FluentSystemIntegrationConfig m_config;
    FluentAdvancedPerformanceAnalytics* m_analytics{nullptr};
    
    // Monitoring state
    bool m_monitoringActive{false};
    bool m_monitoringPaused{false};
    bool m_alertsEnabled{true};
    bool m_analyticsIntegrationEnabled{false};
    
    // Timers
    QTimer* m_monitoringTimer;
    QTimer* m_systemInfoTimer;
    QTimer* m_alertTimer;
    QTimer* m_cacheCleanupTimer;
    
    // Data storage
    FluentSystemInfo m_systemInfo;
    FluentSystemMetrics m_currentMetrics;
    QQueue<FluentSystemMetrics> m_metricsHistory;
    QMap<FluentSystemResource, QQueue<QVariant>> m_resourceHistory;
    
    // Process monitoring
    QMap<qint64, QMap<QString, QVariant>> m_monitoredProcesses;
    QSet<qint64> m_processMonitorList;
    
    // Alert system
    QMap<FluentSystemResource, double> m_alertThresholds;
    QMap<int, std::function<void(FluentSystemResource, const QString&)>> m_alertCallbacks;
    int m_nextCallbackId{1};
    std::chrono::steady_clock::time_point m_lastAlertTime;
    
    // Caching
    bool m_systemInfoCached{false};
    std::chrono::steady_clock::time_point m_systemInfoCacheTime;
    QMap<QString, QVariant> m_metricsCache;
    
    // Platform-specific handles
    void* m_platformHandle{nullptr};
    
#ifdef Q_OS_WIN
    HQUERY m_perfQuery{nullptr};
    QMap<QString, HCOUNTER> m_perfCounters;
#elif defined(Q_OS_LINUX)
    QMap<QString, QString> m_procPaths;
    QMap<QString, QString> m_sysPaths;
#elif defined(Q_OS_MACOS)
    io_iterator_t m_ioIterator{0};
    QMap<QString, io_object_t> m_ioObjects;
#endif
    
    // Thread safety
    mutable QMutex m_metricsMutex;
    mutable QMutex m_systemInfoMutex;
    mutable QMutex m_cacheMutex;
    
    // Performance optimization
    QThreadPool* m_collectionThreadPool;
    std::atomic<bool> m_collectionInProgress{false};
};

// System utility functions
namespace FluentSystemUtils {
    // System information helpers
    QString getOSName();
    QString getOSVersion();
    QString getCPUModel();
    int getCPUCoreCount();
    qint64 getTotalMemory();
    QStringList getNetworkInterfaces();
    QStringList getDiskDrives();
    
    // Performance measurement helpers
    double measureCPUUsage(std::chrono::milliseconds duration = std::chrono::milliseconds(1000));
    qint64 measureMemoryUsage();
    double measureDiskUsage(const QString& drive);
    double measureNetworkLatency(const QString& host);
    
    // System optimization helpers
    void setProcessPriority(qint64 pid, int priority);
    void setProcessAffinity(qint64 pid, const QList<int>& cores);
    void optimizeMemoryUsage();
    void clearSystemCache();
    
    // Platform detection
    bool isWindows();
    bool isLinux();
    bool isMacOS();
    bool isUnix();
    QString getPlatformName();
    QString getArchitecture();
    
    // Hardware detection
    bool hasGPU();
    bool hasBattery();
    bool hasWiFi();
    bool hasBluetooth();
    bool hasCamera();
    bool hasMicrophone();
    
    // Power management
    bool isOnBattery();
    double getBatteryLevel();
    QString getPowerProfile();
    void setPowerProfile(const QString& profile);
    
    // Thermal management
    QMap<QString, double> getThermalSensors();
    bool isThermalThrottling();
    QString getThermalState();
}

} // namespace FluentQt::Performance
