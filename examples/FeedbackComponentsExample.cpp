// examples/FeedbackComponentsExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QTimer>

#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class FeedbackComponentsWindow : public QMainWindow {
    Q_OBJECT

public:
    FeedbackComponentsWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupConnections();
        setupDemoTimer();
        
        // Apply Fluent theme
        auto& theme = Styling::FluentTheme::instance();
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                     .arg(theme.color("backgroundPrimary").name()));
    }

private slots:
    void onProgressValueChanged(int value) {
        m_determinateProgress->setValue(value);
        m_ringProgress->setValue(value);
        m_valueLabel->setText(QString("%1%").arg(value));
    }
    
    void onProgressTypeChanged(int index) {
        auto type = static_cast<Components::FluentProgressBarType>(index);
        m_mainProgress->setProgressType(type);
        
        // Start/stop based on type
        if (type == Components::FluentProgressBarType::Indeterminate ||
            type == Components::FluentProgressBarType::Ring ||
            type == Components::FluentProgressBarType::Dots) {
            m_mainProgress->start();
        } else {
            m_mainProgress->stop();
        }
    }
    
    void onLoadingTypeChanged(int index) {
        auto type = static_cast<Components::FluentLoadingType>(index);
        m_mainLoading->setLoadingType(type);
    }
    
    void onLoadingSizeChanged(int index) {
        auto size = static_cast<Components::FluentLoadingSize>(index);
        m_mainLoading->setLoadingSize(size);
        
        // Update all loading indicators
        for (auto* indicator : m_loadingIndicators) {
            indicator->setLoadingSize(size);
        }
    }
    
    void onToggleLoading() {
        if (m_mainLoading->isRunning()) {
            m_mainLoading->stop();
            m_toggleLoadingButton->setText("Start Loading");
        } else {
            m_mainLoading->start();
            m_toggleLoadingButton->setText("Stop Loading");
        }
    }
    
    void onThemeToggle() {
        auto& theme = Styling::FluentTheme::instance();
        theme.setDarkMode(!theme.isDarkMode());
        
        // Update window background
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                     .arg(theme.color("backgroundPrimary").name()));
    }
    
    void onDemoStep() {
        // Animate progress bars
        static int demoProgress = 0;
        demoProgress = (demoProgress + 2) % 101;
        
        m_demoProgress1->setValue(demoProgress);
        m_demoProgress2->setValue((demoProgress + 30) % 101);
        m_demoProgress3->setValue((demoProgress + 60) % 101);
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(40, 40, 40, 40);
        
        // Title
        auto* titleLabel = new QLabel("Fluent UI Feedback Components Demo", this);
        auto& theme = Styling::FluentTheme::instance();
        titleLabel->setFont(theme.titleFont());
        titleLabel->setStyleSheet(QString("color: %1;").arg(theme.color("textPrimary").name()));
        mainLayout->addWidget(titleLabel);
        
        // Progress Bar Section
        setupProgressBarSection(mainLayout);
        
        // Loading Indicator Section
        setupLoadingIndicatorSection(mainLayout);
        
        // Demo Section
        setupDemoSection(mainLayout);
        
        // Controls
        setupControls(mainLayout);
        
        mainLayout->addStretch();
        
        setWindowTitle("Fluent UI Feedback Components Example");
        resize(800, 900);
    }
    
    void setupProgressBarSection(QVBoxLayout* mainLayout) {
        auto* progressGroup = new QGroupBox("Progress Bars", this);
        auto& theme = Styling::FluentTheme::instance();
        progressGroup->setFont(theme.subtitleFont());
        auto* progressLayout = new QVBoxLayout(progressGroup);
        progressLayout->setSpacing(16);
        
        // Main configurable progress bar
        auto* mainProgressLayout = new QHBoxLayout();
        mainProgressLayout->addWidget(new QLabel("Main Progress:", this));
        
        m_mainProgress = new Components::FluentProgressBar(this);
        m_mainProgress->setValue(65);
        m_mainProgress->setText("Processing...");
        mainProgressLayout->addWidget(m_mainProgress, 1);
        
        progressLayout->addLayout(mainProgressLayout);
        
        // Different types of progress bars
        auto* typesLayout = new QGridLayout();
        
        // Determinate
        typesLayout->addWidget(new QLabel("Determinate:", this), 0, 0);
        m_determinateProgress = new Components::FluentProgressBar(Components::FluentProgressBarType::Determinate, this);
        m_determinateProgress->setValue(45);
        typesLayout->addWidget(m_determinateProgress, 0, 1);
        
        // Indeterminate
        typesLayout->addWidget(new QLabel("Indeterminate:", this), 1, 0);
        auto* indeterminateProgress = new Components::FluentProgressBar(Components::FluentProgressBarType::Indeterminate, this);
        indeterminateProgress->start();
        typesLayout->addWidget(indeterminateProgress, 1, 1);
        
        // Ring
        typesLayout->addWidget(new QLabel("Ring:", this), 2, 0);
        m_ringProgress = new Components::FluentProgressBar(Components::FluentProgressBarType::Ring, this);
        m_ringProgress->setValue(75);
        typesLayout->addWidget(m_ringProgress, 2, 1);
        
        // Dots
        typesLayout->addWidget(new QLabel("Dots:", this), 3, 0);
        auto* dotsProgress = new Components::FluentProgressBar(Components::FluentProgressBarType::Dots, this);
        dotsProgress->start();
        typesLayout->addWidget(dotsProgress, 3, 1);
        
        progressLayout->addLayout(typesLayout);
        mainLayout->addWidget(progressGroup);
    }
    
    void setupLoadingIndicatorSection(QVBoxLayout* mainLayout) {
        auto* loadingGroup = new QGroupBox("Loading Indicators", this);
        auto& theme = Styling::FluentTheme::instance();
        loadingGroup->setFont(theme.subtitleFont());
        auto* loadingLayout = new QVBoxLayout(loadingGroup);
        loadingLayout->setSpacing(16);
        
        // Main configurable loading indicator
        auto* mainLoadingLayout = new QHBoxLayout();
        mainLoadingLayout->addWidget(new QLabel("Main Loading:", this));
        
        m_mainLoading = new Components::FluentLoadingIndicator(this);
        m_mainLoading->setText("Loading...");
        m_mainLoading->start();
        mainLoadingLayout->addWidget(m_mainLoading);
        mainLoadingLayout->addStretch();
        
        loadingLayout->addLayout(mainLoadingLayout);
        
        // Different types of loading indicators
        auto* typesLayout = new QGridLayout();
        
        const QList<Components::FluentLoadingType> loadingTypes = {
            Components::FluentLoadingType::Spinner,
            Components::FluentLoadingType::Dots,
            Components::FluentLoadingType::Pulse,
            Components::FluentLoadingType::Bars,
            Components::FluentLoadingType::Ring,
            Components::FluentLoadingType::Wave
        };
        
        const QStringList typeNames = {
            "Spinner", "Dots", "Pulse", "Bars", "Ring", "Wave"
        };
        
        for (int i = 0; i < loadingTypes.size(); ++i) {
            const int row = i / 2;
            const int col = (i % 2) * 2;
            
            typesLayout->addWidget(new QLabel(typeNames[i] + ":", this), row, col);
            
            auto* indicator = new Components::FluentLoadingIndicator(loadingTypes[i], this);
            indicator->start();
            m_loadingIndicators.append(indicator);
            
            typesLayout->addWidget(indicator, row, col + 1);
        }
        
        loadingLayout->addLayout(typesLayout);
        mainLayout->addWidget(loadingGroup);
    }
    
    void setupDemoSection(QVBoxLayout* mainLayout) {
        auto* demoGroup = new QGroupBox("Live Demo", this);
        auto& theme = Styling::FluentTheme::instance();
        demoGroup->setFont(theme.subtitleFont());
        auto* demoLayout = new QVBoxLayout(demoGroup);
        demoLayout->setSpacing(12);
        
        // Animated progress bars
        m_demoProgress1 = new Components::FluentProgressBar(this);
        m_demoProgress1->setText("Task 1");
        m_demoProgress1->setProgressSize(Components::FluentProgressBarSize::Small);
        demoLayout->addWidget(m_demoProgress1);
        
        m_demoProgress2 = new Components::FluentProgressBar(this);
        m_demoProgress2->setText("Task 2");
        m_demoProgress2->setProgressSize(Components::FluentProgressBarSize::Medium);
        demoLayout->addWidget(m_demoProgress2);
        
        m_demoProgress3 = new Components::FluentProgressBar(this);
        m_demoProgress3->setText("Task 3");
        m_demoProgress3->setProgressSize(Components::FluentProgressBarSize::Large);
        demoLayout->addWidget(m_demoProgress3);
        
        mainLayout->addWidget(demoGroup);
    }
    
    void setupControls(QVBoxLayout* mainLayout) {
        auto* controlsGroup = new QGroupBox("Controls", this);
        auto& theme = Styling::FluentTheme::instance();
        controlsGroup->setFont(theme.subtitleFont());
        auto* controlsLayout = new QGridLayout(controlsGroup);
        
        // Progress value slider
        controlsLayout->addWidget(new QLabel("Progress Value:", this), 0, 0);
        m_progressSlider = new QSlider(Qt::Horizontal, this);
        m_progressSlider->setRange(0, 100);
        m_progressSlider->setValue(65);
        controlsLayout->addWidget(m_progressSlider, 0, 1);
        
        m_valueLabel = new QLabel("65%", this);
        controlsLayout->addWidget(m_valueLabel, 0, 2);
        
        // Progress type combo
        controlsLayout->addWidget(new QLabel("Progress Type:", this), 1, 0);
        m_progressTypeCombo = new QComboBox(this);
        m_progressTypeCombo->addItems({"Determinate", "Indeterminate", "Ring", "Dots"});
        controlsLayout->addWidget(m_progressTypeCombo, 1, 1, 1, 2);
        
        // Loading type combo
        controlsLayout->addWidget(new QLabel("Loading Type:", this), 2, 0);
        m_loadingTypeCombo = new QComboBox(this);
        m_loadingTypeCombo->addItems({"Spinner", "Dots", "Pulse", "Bars", "Ring", "Wave"});
        controlsLayout->addWidget(m_loadingTypeCombo, 2, 1, 1, 2);
        
        // Loading size combo
        controlsLayout->addWidget(new QLabel("Loading Size:", this), 3, 0);
        m_loadingSizeCombo = new QComboBox(this);
        m_loadingSizeCombo->addItems({"Small", "Medium", "Large", "Extra Large"});
        m_loadingSizeCombo->setCurrentIndex(1); // Medium
        controlsLayout->addWidget(m_loadingSizeCombo, 3, 1, 1, 2);
        
        // Action buttons
        auto* buttonLayout = new QHBoxLayout();
        
        m_toggleLoadingButton = new QPushButton("Stop Loading", this);
        buttonLayout->addWidget(m_toggleLoadingButton);
        
        auto* themeButton = new QPushButton("Toggle Theme", this);
        buttonLayout->addWidget(themeButton);
        
        buttonLayout->addStretch();
        controlsLayout->addLayout(buttonLayout, 4, 0, 1, 3);
        
        mainLayout->addWidget(controlsGroup);
    }
    
    void setupConnections() {
        connect(m_progressSlider, &QSlider::valueChanged, this, &FeedbackComponentsWindow::onProgressValueChanged);
        connect(m_progressTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &FeedbackComponentsWindow::onProgressTypeChanged);
        connect(m_loadingTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &FeedbackComponentsWindow::onLoadingTypeChanged);
        connect(m_loadingSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &FeedbackComponentsWindow::onLoadingSizeChanged);
        connect(m_toggleLoadingButton, &QPushButton::clicked, this, &FeedbackComponentsWindow::onToggleLoading);
        
        auto* themeButton = findChild<QPushButton*>();
        if (themeButton && themeButton->text() == "Toggle Theme") {
            connect(themeButton, &QPushButton::clicked, this, &FeedbackComponentsWindow::onThemeToggle);
        }
    }
    
    void setupDemoTimer() {
        m_demoTimer = new QTimer(this);
        m_demoTimer->setInterval(100); // Update every 100ms
        connect(m_demoTimer, &QTimer::timeout, this, &FeedbackComponentsWindow::onDemoStep);
        m_demoTimer->start();
    }

private:
    // Progress bars
    Components::FluentProgressBar* m_mainProgress{nullptr};
    Components::FluentProgressBar* m_determinateProgress{nullptr};
    Components::FluentProgressBar* m_ringProgress{nullptr};
    Components::FluentProgressBar* m_demoProgress1{nullptr};
    Components::FluentProgressBar* m_demoProgress2{nullptr};
    Components::FluentProgressBar* m_demoProgress3{nullptr};
    
    // Loading indicators
    Components::FluentLoadingIndicator* m_mainLoading{nullptr};
    QList<Components::FluentLoadingIndicator*> m_loadingIndicators;
    
    // Controls
    QSlider* m_progressSlider{nullptr};
    QComboBox* m_progressTypeCombo{nullptr};
    QComboBox* m_loadingTypeCombo{nullptr};
    QComboBox* m_loadingSizeCombo{nullptr};
    QPushButton* m_toggleLoadingButton{nullptr};
    QLabel* m_valueLabel{nullptr};
    
    // Demo timer
    QTimer* m_demoTimer{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Initialize Fluent theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(QColor(0, 120, 215)); // Windows blue
    
    FeedbackComponentsWindow window;
    window.show();
    
    return app.exec();
}

#include "FeedbackComponentsExample.moc"
