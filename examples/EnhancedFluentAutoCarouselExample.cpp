// examples/EnhancedFluentAutoCarouselExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QScrollArea>
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>

#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

class EnhancedCarouselDemo : public QMainWindow {
    Q_OBJECT

public:
    explicit EnhancedCarouselDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Enhanced FluentAutoCarousel Demo - Element Fluent UI");
        setMinimumSize(1000, 700);
        
        setupUI();
        createCarouselExamples();
        connectSignals();
        
        // Apply Fluent theme
        auto& theme = FluentTheme::instance();
        theme.setMode(FluentThemeMode::Light);
    }

private slots:
    void onConfigurationChanged() {
        updateCarouselConfiguration();
    }
    
    void onThemeChanged() {
        auto& theme = FluentTheme::instance();
        auto mode = m_darkModeCheck->isChecked() ? FluentThemeMode::Dark : FluentThemeMode::Light;
        theme.setMode(mode);
    }
    
    void onCreateCustomCarousel() {
        createCustomConfiguredCarousel();
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QHBoxLayout(centralWidget);
        
        // Left panel for controls
        auto* controlPanel = createControlPanel();
        mainLayout->addWidget(controlPanel, 0);
        
        // Right panel for carousel examples
        auto* examplePanel = createExamplePanel();
        mainLayout->addWidget(examplePanel, 1);
    }
    
    QWidget* createControlPanel() {
        auto* panel = new QGroupBox("Configuration", this);
        panel->setMaximumWidth(300);
        auto* layout = new QVBoxLayout(panel);
        
        // Theme controls
        auto* themeGroup = new QGroupBox("Theme", panel);
        auto* themeLayout = new QVBoxLayout(themeGroup);
        
        m_darkModeCheck = new QCheckBox("Dark Mode", themeGroup);
        m_highContrastCheck = new QCheckBox("High Contrast", themeGroup);
        themeLayout->addWidget(m_darkModeCheck);
        themeLayout->addWidget(m_highContrastCheck);
        
        // Auto-play controls
        auto* autoPlayGroup = new QGroupBox("Auto-play", panel);
        auto* autoPlayLayout = new QVBoxLayout(autoPlayGroup);
        
        m_intervalSpin = new QSpinBox(autoPlayGroup);
        m_intervalSpin->setRange(100, 60000);
        m_intervalSpin->setValue(3000);
        m_intervalSpin->setSuffix(" ms");
        autoPlayLayout->addWidget(new QLabel("Interval:"));
        autoPlayLayout->addWidget(m_intervalSpin);
        
        m_directionCombo = new QComboBox(autoPlayGroup);
        m_directionCombo->addItems({"Forward", "Backward", "Ping-Pong"});
        autoPlayLayout->addWidget(new QLabel("Direction:"));
        autoPlayLayout->addWidget(m_directionCombo);
        
        // Progress style controls
        auto* progressGroup = new QGroupBox("Progress Style", panel);
        auto* progressLayout = new QVBoxLayout(progressGroup);
        
        m_progressStyleCombo = new QComboBox(progressGroup);
        m_progressStyleCombo->addItems({"Bar", "Circular", "Dots", "Line", "Ring", "Minimal", "Segmented"});
        progressLayout->addWidget(m_progressStyleCombo);
        
        // Button style controls
        auto* buttonGroup = new QGroupBox("Button Style", panel);
        auto* buttonLayout = new QVBoxLayout(buttonGroup);
        
        m_buttonStyleCombo = new QComboBox(buttonGroup);
        m_buttonStyleCombo->addItems({"Subtle", "Standard", "Accent"});
        buttonLayout->addWidget(m_buttonStyleCombo);
        
        // Enhanced features
        auto* enhancedGroup = new QGroupBox("Enhanced Features", panel);
        auto* enhancedLayout = new QVBoxLayout(enhancedGroup);
        
        m_elevationCheck = new QCheckBox("Elevation Effects", enhancedGroup);
        m_elevationCheck->setChecked(true);
        m_motionCheck = new QCheckBox("Fluent Motion", enhancedGroup);
        m_motionCheck->setChecked(true);
        m_accessibilityCheck = new QCheckBox("Enhanced Accessibility", enhancedGroup);
        m_accessibilityCheck->setChecked(true);
        
        enhancedLayout->addWidget(m_elevationCheck);
        enhancedLayout->addWidget(m_motionCheck);
        enhancedLayout->addWidget(m_accessibilityCheck);
        
        // Corner radius
        m_cornerRadiusSpin = new QSpinBox(enhancedGroup);
        m_cornerRadiusSpin->setRange(0, 20);
        m_cornerRadiusSpin->setValue(0);
        m_cornerRadiusSpin->setSpecialValueText("Theme Default");
        enhancedLayout->addWidget(new QLabel("Corner Radius:"));
        enhancedLayout->addWidget(m_cornerRadiusSpin);
        
        // Create button
        auto* createButton = new QPushButton("Create Custom Carousel", panel);
        
        // Add all groups to main layout
        layout->addWidget(themeGroup);
        layout->addWidget(autoPlayGroup);
        layout->addWidget(progressGroup);
        layout->addWidget(buttonGroup);
        layout->addWidget(enhancedGroup);
        layout->addWidget(createButton);
        layout->addStretch();
        
        // Store create button reference
        m_createButton = createButton;
        
        return panel;
    }
    
    QWidget* createExamplePanel() {
        auto* panel = new QWidget(this);
        auto* layout = new QVBoxLayout(panel);
        
        // Tab widget for different examples
        m_tabWidget = new QTabWidget(panel);
        layout->addWidget(m_tabWidget);
        
        return panel;
    }
    
    void createCarouselExamples() {
        // Basic example
        createBasicExample();
        
        // Touch optimized example
        createTouchOptimizedExample();
        
        // High contrast example
        createHighContrastExample();
        
        // Custom configuration example
        createCustomExample();
    }
    
    void createBasicExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        auto* carousel = FluentAutoCarousel::createWithInterval(2000, widget);
        addSampleContent(carousel);
        
        layout->addWidget(new QLabel("Basic Auto Carousel with 2-second intervals"));
        layout->addWidget(carousel);
        layout->addStretch();
        
        m_tabWidget->addTab(widget, "Basic");
    }
    
    void createTouchOptimizedExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        auto* carousel = FluentAutoCarousel::createTouchOptimized(3000, widget);
        addSampleContent(carousel);
        
        layout->addWidget(new QLabel("Touch-Optimized Carousel with dot progress"));
        layout->addWidget(carousel);
        layout->addStretch();
        
        m_tabWidget->addTab(widget, "Touch");
    }
    
    void createHighContrastExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        auto* carousel = FluentAutoCarousel::createHighContrast(4000, widget);
        addSampleContent(carousel);
        
        layout->addWidget(new QLabel("High Contrast Carousel for accessibility"));
        layout->addWidget(carousel);
        layout->addStretch();
        
        m_tabWidget->addTab(widget, "High Contrast");
    }
    
    void createCustomExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        // This will be populated by the custom configuration
        m_customCarouselWidget = widget;
        m_customCarouselLayout = layout;
        
        layout->addWidget(new QLabel("Custom configured carousel will appear here"));
        layout->addStretch();
        
        m_tabWidget->addTab(widget, "Custom");
    }
    
    void addSampleContent(FluentAutoCarousel* carousel) {
        // Create sample content with colored rectangles
        QStringList colors = {"#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4", "#FFEAA7", "#DDA0DD"};
        
        for (int i = 0; i < colors.size(); ++i) {
            auto* label = new QLabel();
            label->setMinimumSize(400, 200);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet(QString("background-color: %1; color: white; font-size: 18px; font-weight: bold;").arg(colors[i]));
            label->setText(QString("Slide %1").arg(i + 1));
            
            carousel->addItem(label);
        }
    }
    
    void connectSignals() {
        connect(m_darkModeCheck, &QCheckBox::toggled, this, &EnhancedCarouselDemo::onThemeChanged);
        connect(m_createButton, &QPushButton::clicked, this, &EnhancedCarouselDemo::onCreateCustomCarousel);
        
        // Connect configuration controls
        connect(m_intervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &EnhancedCarouselDemo::onConfigurationChanged);
        connect(m_directionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnhancedCarouselDemo::onConfigurationChanged);
        connect(m_progressStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnhancedCarouselDemo::onConfigurationChanged);
        connect(m_buttonStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnhancedCarouselDemo::onConfigurationChanged);
        connect(m_elevationCheck, &QCheckBox::toggled, this, &EnhancedCarouselDemo::onConfigurationChanged);
        connect(m_motionCheck, &QCheckBox::toggled, this, &EnhancedCarouselDemo::onConfigurationChanged);
        connect(m_accessibilityCheck, &QCheckBox::toggled, this, &EnhancedCarouselDemo::onConfigurationChanged);
        connect(m_cornerRadiusSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &EnhancedCarouselDemo::onConfigurationChanged);
    }
    
    void updateCarouselConfiguration() {
        // This will be called when configuration changes
        // For now, just enable the create button
        m_createButton->setEnabled(true);
        m_createButton->setText("Create Updated Carousel");
    }
    
    void createCustomConfiguredCarousel() {
        // Clear existing custom carousel
        if (m_customCarousel) {
            m_customCarousel->deleteLater();
        }
        
        // Create configuration from UI
        FluentAutoCarouselConfig config;
        config.autoPlayInterval = std::chrono::milliseconds(m_intervalSpin->value());
        
        // Set direction
        switch (m_directionCombo->currentIndex()) {
            case 0: config.autoPlayDirection = FluentCarouselAutoPlay::Forward; break;
            case 1: config.autoPlayDirection = FluentCarouselAutoPlay::Backward; break;
            case 2: config.autoPlayDirection = FluentCarouselAutoPlay::PingPong; break;
        }
        
        // Set progress style
        config.progressStyle = static_cast<FluentAutoCarouselProgressStyle>(m_progressStyleCombo->currentIndex());
        
        // Set button style
        config.controlButtonStyle = static_cast<FluentButtonStyle>(m_buttonStyleCombo->currentIndex());
        
        // Set enhanced features
        config.elevationEnabled = m_elevationCheck->isChecked();
        config.fluentMotionEnabled = m_motionCheck->isChecked();
        config.accessibilityAnnouncementsEnabled = m_accessibilityCheck->isChecked();
        config.cornerRadius = m_cornerRadiusSpin->value();
        
        // Set semantic color scheme based on theme
        if (m_highContrastCheck->isChecked()) {
            config.semanticColorScheme = FluentSemanticColorScheme::HighContrast;
        } else if (m_darkModeCheck->isChecked()) {
            config.semanticColorScheme = FluentSemanticColorScheme::Dark;
        } else {
            config.semanticColorScheme = FluentSemanticColorScheme::Light;
        }
        
        // Create carousel from configuration
        m_customCarousel = FluentAutoCarousel::createFromConfig(config, m_customCarouselWidget);
        addSampleContent(m_customCarousel);
        
        // Add to layout
        m_customCarouselLayout->insertWidget(1, m_customCarousel);
        
        // Update button
        m_createButton->setText("Carousel Created!");
        m_createButton->setEnabled(false);
        
        // Re-enable after a delay
        QTimer::singleShot(2000, [this]() {
            m_createButton->setText("Create Custom Carousel");
            m_createButton->setEnabled(true);
        });
    }

private:
    // UI Controls
    QTabWidget* m_tabWidget{nullptr};
    QCheckBox* m_darkModeCheck{nullptr};
    QCheckBox* m_highContrastCheck{nullptr};
    QSpinBox* m_intervalSpin{nullptr};
    QComboBox* m_directionCombo{nullptr};
    QComboBox* m_progressStyleCombo{nullptr};
    QComboBox* m_buttonStyleCombo{nullptr};
    QCheckBox* m_elevationCheck{nullptr};
    QCheckBox* m_motionCheck{nullptr};
    QCheckBox* m_accessibilityCheck{nullptr};
    QSpinBox* m_cornerRadiusSpin{nullptr};
    QPushButton* m_createButton{nullptr};
    
    // Custom carousel
    QWidget* m_customCarouselWidget{nullptr};
    QVBoxLayout* m_customCarouselLayout{nullptr};
    FluentAutoCarousel* m_customCarousel{nullptr};
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    EnhancedCarouselDemo demo;
    demo.show();
    
    return app.exec();
}

#include "EnhancedFluentAutoCarouselExample.moc"
