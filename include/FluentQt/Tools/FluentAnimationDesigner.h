// include/FluentQt/Tools/FluentAnimationDesigner.h
#ifndef FLUENTANIMATIONDESIGNER_H
#define FLUENTANIMATIONDESIGNER_H

#include <QComboBox>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMainWindow>
#include <QPropertyBrowser>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentGPUAnimator.h"
#include "FluentQt/Animation/FluentPhysicsAnimator.h"

namespace FluentQt::Tools {

// Animation preset definitions
struct FluentAnimationPreset {
    QString name;
    QString description;
    QString category;
    Animation::FluentAnimationConfig config;
    QVariantMap customProperties;
    QString codeTemplate;

    // Predefined presets
    static QList<FluentAnimationPreset> getBuiltInPresets();
    static FluentAnimationPreset createCustomPreset(
        const QString& name, const Animation::FluentAnimationConfig& config);
};

// Animation timeline item
struct FluentTimelineItem {
    QString name;
    QString property;
    QVariant startValue;
    QVariant endValue;
    int startTime;
    int duration;
    Animation::FluentEasing easing;
    bool enabled{true};
};

// Visual animation designer widget
class FluentAnimationDesigner : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentAnimationDesigner(QWidget* parent = nullptr);
    ~FluentAnimationDesigner() = default;

    // Project management
    void newProject();
    void openProject(const QString& filePath);
    void saveProject(const QString& filePath);
    void exportAnimation(const QString& format = "cpp");

    // Animation management
    void addAnimationLayer(const QString& name);
    void removeAnimationLayer(const QString& name);
    void setCurrentLayer(const QString& name);

    // Preview and testing
    void playAnimation();
    void pauseAnimation();
    void stopAnimation();
    void setPreviewWidget(QWidget* widget);

    // Preset management
    void loadPreset(const FluentAnimationPreset& preset);
    void saveAsPreset(const QString& name);
    QList<FluentAnimationPreset> getAvailablePresets() const;

signals:
    void animationChanged();
    void previewRequested();
    void codeGenerated(const QString& code);

private slots:
    void onPropertyChanged();
    void onTimelineChanged();
    void onPresetSelected();
    void onPreviewUpdate();
    void onExportRequested();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupPropertyPanel();
    void setupTimelinePanel();
    void setupPreviewPanel();
    void setupPresetPanel();
    void setupCodePanel();

    void updatePropertyBrowser();
    void updateTimeline();
    void updatePreview();
    void generateCode();

    // UI Components
    QSplitter* m_mainSplitter;
    QSplitter* m_rightSplitter;

    // Property panel
    QWidget* m_propertyPanel;
    QTreeWidget* m_layerTree;
    QWidget* m_propertyBrowser;
    QGroupBox* m_easingGroup;
    QComboBox* m_easingCombo;
    QSlider* m_durationSlider;
    QSpinBox* m_durationSpin;
    QSlider* m_delaySlider;
    QSpinBox* m_delaySpin;

    // Timeline panel
    QWidget* m_timelinePanel;
    QWidget* m_timelineView;
    QPushButton* m_playButton;
    QPushButton* m_pauseButton;
    QPushButton* m_stopButton;
    QSlider* m_timelineSlider;
    QLabel* m_timeLabel;

    // Preview panel
    QWidget* m_previewPanel;
    QGraphicsView* m_previewView;
    QGraphicsScene* m_previewScene;
    QWidget* m_previewWidget;
    QPushButton* m_previewButton;

    // Preset panel
    QWidget* m_presetPanel;
    QTreeWidget* m_presetTree;
    QPushButton* m_loadPresetButton;
    QPushButton* m_savePresetButton;
    QPushButton* m_deletePresetButton;

    // Code panel
    QTabWidget* m_codeTabWidget;
    QTextEdit* m_cppCodeEdit;
    QTextEdit* m_qmlCodeEdit;
    QTextEdit* m_jsonCodeEdit;
    QPushButton* m_exportButton;
    QPushButton* m_copyButton;

    // Animation data
    QList<FluentTimelineItem> m_timelineItems;
    QList<FluentAnimationPreset> m_presets;
    QString m_currentLayer;
    QTimer* m_previewTimer;
    int m_currentTime{0};
    int m_totalDuration{1000};

    // Animation objects
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    std::unique_ptr<Animation::FluentPhysicsAnimator> m_physicsAnimator;
    std::unique_ptr<Animation::FluentGPUAnimator> m_gpuAnimator;
};

// Animation debugging and profiling tool
class FluentAnimationProfiler : public QWidget {
    Q_OBJECT

public:
    explicit FluentAnimationProfiler(QWidget* parent = nullptr);
    ~FluentAnimationProfiler() = default;

    // Profiling control
    void startProfiling();
    void stopProfiling();
    void clearData();

    // Data access
    QList<QVariantMap> getPerformanceData() const;
    QVariantMap getCurrentMetrics() const;

    // Visualization
    void setRealTimeMode(bool enabled);
    void setUpdateInterval(int ms);

signals:
    void performanceAlert(const QString& message);
    void frameDropDetected(int droppedFrames);

private slots:
    void updateMetrics();
    void analyzePerformance();

private:
    void setupUI();
    void setupMetricsDisplay();
    void setupPerformanceGraph();
    void setupAlertSystem();

    // UI Components
    QVBoxLayout* m_layout;
    QGroupBox* m_metricsGroup;
    QLabel* m_fpsLabel;
    QLabel* m_frameTimeLabel;
    QLabel* m_memoryLabel;
    QLabel* m_animationCountLabel;

    QGroupBox* m_graphGroup;
    QWidget* m_performanceGraph;

    QGroupBox* m_alertGroup;
    QTextEdit* m_alertLog;

    QPushButton* m_startButton;
    QPushButton* m_stopButton;
    QPushButton* m_clearButton;
    QPushButton* m_exportButton;

    // Profiling data
    QTimer* m_updateTimer;
    QList<QVariantMap> m_performanceHistory;
    bool m_profiling{false};
    bool m_realTimeMode{true};
    int m_updateInterval{100};

    // Performance thresholds
    double m_fpsThreshold{30.0};
    double m_frameTimeThreshold{33.0};
    int m_memoryThreshold{100};  // MB
};

// Animation cookbook and documentation
class FluentAnimationCookbook : public QWidget {
    Q_OBJECT

public:
    explicit FluentAnimationCookbook(QWidget* parent = nullptr);
    ~FluentAnimationCookbook() = default;

    // Content management
    void addRecipe(const QString& category, const QString& name,
                   const QString& description, const QString& code);
    void removeRecipe(const QString& category, const QString& name);
    void updateRecipe(const QString& category, const QString& name,
                      const QString& description, const QString& code);

    // Search and filtering
    void setSearchFilter(const QString& filter);
    void setCategoryFilter(const QString& category);
    void setDifficultyFilter(const QString& difficulty);

signals:
    void recipeSelected(const QString& code);
    void recipeExecuted(const QString& code);

private slots:
    void onRecipeClicked();
    void onSearchChanged();
    void onCategoryChanged();
    void onRunExample();

private:
    void setupUI();
    void setupSearchPanel();
    void setupCategoryTree();
    void setupContentPanel();
    void loadBuiltInRecipes();

    struct AnimationRecipe {
        QString category;
        QString name;
        QString description;
        QString difficulty;
        QString code;
        QStringList tags;
        QString preview;
    };

    // UI Components
    QHBoxLayout* m_layout;
    QSplitter* m_splitter;

    QWidget* m_leftPanel;
    QVBoxLayout* m_leftLayout;
    QLineEdit* m_searchEdit;
    QComboBox* m_categoryCombo;
    QComboBox* m_difficultyCombo;
    QTreeWidget* m_recipeTree;

    QWidget* m_rightPanel;
    QVBoxLayout* m_rightLayout;
    QLabel* m_titleLabel;
    QLabel* m_descriptionLabel;
    QTextEdit* m_codeEdit;
    QPushButton* m_runButton;
    QPushButton* m_copyButton;
    QPushButton* m_favoriteButton;

    // Data
    QList<AnimationRecipe> m_recipes;
    QStringList m_categories;
    QString m_currentFilter;
    QString m_currentCategory;
    QString m_currentDifficulty;
};

// Animation template generator
class FluentAnimationTemplateGenerator : public QObject {
    Q_OBJECT

public:
    explicit FluentAnimationTemplateGenerator(QObject* parent = nullptr);
    ~FluentAnimationTemplateGenerator() = default;

    // Template generation
    QString generateCppTemplate(const Animation::FluentAnimationConfig& config,
                                const QString& targetProperty,
                                const QString& className = "MyWidget");

    QString generateQmlTemplate(const Animation::FluentAnimationConfig& config,
                                const QString& targetProperty);

    QString generateJsonTemplate(
        const Animation::FluentAnimationConfig& config);

    // Batch generation
    QStringList generateComponentTemplates(const QStringList& components);
    QString generateAnimationSequence(
        const QList<Animation::FluentAnimationConfig>& configs);

    // Utility methods
    static QString easingToString(Animation::FluentEasing easing);
    static QString durationToString(std::chrono::milliseconds duration);
    static QString configToJson(const Animation::FluentAnimationConfig& config);

signals:
    void templateGenerated(const QString& template_);
    void batchCompleted(const QStringList& templates);

private:
    QString formatCppCode(const QString& code) const;
    QString formatQmlCode(const QString& code) const;
    QString formatJsonCode(const QString& code) const;

    QStringList m_supportedComponents;
    QStringList m_supportedProperties;
};

}  // namespace FluentQt::Tools

#endif  // FLUENTANIMATIONDESIGNER_H
