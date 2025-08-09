// examples/ResponsiveDesignExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSlider>
#include <QProgressBar>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QTabWidget>
#include <QTimer>
#include <QResizeEvent>

#include "FluentQt/Core/FluentResponsive.h"
#include "FluentQt/Core/FluentFluidTypography.h"
#include "FluentQt/Core/FluentResponsiveComponent.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

using namespace FluentQt;

class ResponsiveCard : public Core::FluentResponsiveWidget {
    Q_OBJECT

public:
    explicit ResponsiveCard(const QString& title, const QString& content, QWidget* parent = nullptr)
        : FluentResponsiveWidget(parent), m_title(title), m_content(content) {
        setupUI();
        setupResponsive();
    }

protected:
    void onBreakpointChanged(Core::FluentBreakpoint breakpoint) override {
        updateLayoutForBreakpoint(breakpoint);
    }
    
    void onVariantChanged(Core::FluentComponentVariant variant) override {
        updateStyleForVariant(variant);
    }

private:
    void setupUI() {
        setMinimumSize(200, 150);
        
        auto* layout = new QVBoxLayout(this);
        
        // Title with fluid typography
        m_titleLabel = new QLabel(m_title, this);
        m_titleLabel->setObjectName("CardTitle");
        
        // Content
        m_contentLabel = new QLabel(m_content, this);
        m_contentLabel->setObjectName("CardContent");
        m_contentLabel->setWordWrap(true);
        
        // Action button
        m_actionButton = new QPushButton("Action", this);
        m_actionButton->setObjectName("CardAction");
        
        layout->addWidget(m_titleLabel);
        layout->addWidget(m_contentLabel);
        layout->addStretch();
        layout->addWidget(m_actionButton);
        
        // Apply initial styling
        setStyleSheet(
            "ResponsiveCard { "
            "  background-color: white; "
            "  border: 1px solid #e0e0e0; "
            "  border-radius: 8px; "
            "  padding: 16px; "
            "} "
            "#CardTitle { "
            "  font-weight: bold; "
            "  color: #333; "
            "} "
            "#CardContent { "
            "  color: #666; "
            "  margin: 8px 0; "
            "} "
            "#CardAction { "
            "  background-color: #0078d4; "
            "  color: white; "
            "  border: none; "
            "  border-radius: 4px; "
            "  padding: 8px 16px; "
            "}"
        );
    }
    
    void setupResponsive() {
        Core::FluentResponsiveComponentConfig config;
        config.componentType = "ResponsiveCard";
        config.enableFluidSizing = true;
        config.enableAccessibilityAdaptation = true;
        
        // Configure states for different breakpoints
        Core::FluentComponentState compactState;
        compactState.breakpoint = Core::FluentBreakpoint::XSmall;
        compactState.variant = Core::FluentComponentVariant::Compact;
        compactState.size = QSize(280, 120);
        compactState.margins = QMargins(8, 8, 8, 8);
        compactState.spacing = 4;
        config.states[Core::FluentBreakpoint::XSmall] = compactState;
        
        Core::FluentComponentState comfortableState;
        comfortableState.breakpoint = Core::FluentBreakpoint::Medium;
        comfortableState.variant = Core::FluentComponentVariant::Comfortable;
        comfortableState.size = QSize(320, 180);
        comfortableState.margins = QMargins(16, 16, 16, 16);
        comfortableState.spacing = 8;
        config.states[Core::FluentBreakpoint::Medium] = comfortableState;
        
        Core::FluentComponentState spaciousState;
        spaciousState.breakpoint = Core::FluentBreakpoint::Large;
        spaciousState.variant = Core::FluentComponentVariant::Spacious;
        spaciousState.size = QSize(400, 220);
        spaciousState.margins = QMargins(24, 24, 24, 24);
        spaciousState.spacing = 12;
        config.states[Core::FluentBreakpoint::Large] = spaciousState;
        
        setResponsiveConfig(config);
        enablePerformanceOptimization(true);
    }
    
    void updateLayoutForBreakpoint(Core::FluentBreakpoint breakpoint) {
        // Update typography based on breakpoint
        auto& typographyManager = Core::FluentFluidTypographyManager::instance();
        QSize viewportSize = parentWidget() ? parentWidget()->size() : QSize(1024, 768);
        
        // Update title font
        QFont titleFont = typographyManager.calculateFluidFont(Core::FluentTypographyStyle::Title, viewportSize);
        m_titleLabel->setFont(titleFont);
        
        // Update content font
        QFont contentFont = typographyManager.calculateFluidFont(Core::FluentTypographyStyle::Body, viewportSize);
        m_contentLabel->setFont(contentFont);
        
        // Adjust layout based on breakpoint
        switch (breakpoint) {
            case Core::FluentBreakpoint::XSmall:
                // Stack elements more tightly
                layout()->setSpacing(4);
                m_actionButton->setText("Go");
                break;
            case Core::FluentBreakpoint::Small:
                layout()->setSpacing(6);
                m_actionButton->setText("Action");
                break;
            case Core::FluentBreakpoint::Medium:
                layout()->setSpacing(8);
                m_actionButton->setText("Take Action");
                break;
            case Core::FluentBreakpoint::Large:
            case Core::FluentBreakpoint::XLarge:
                layout()->setSpacing(12);
                m_actionButton->setText("Take Action");
                break;
            default:
                break;
        }
    }
    
    void updateStyleForVariant(Core::FluentComponentVariant variant) {
        QString variantClass;
        
        switch (variant) {
            case Core::FluentComponentVariant::Compact:
                variantClass = "compact";
                setContentsMargins(8, 8, 8, 8);
                break;
            case Core::FluentComponentVariant::Comfortable:
                variantClass = "comfortable";
                setContentsMargins(16, 16, 16, 16);
                break;
            case Core::FluentComponentVariant::Spacious:
                variantClass = "spacious";
                setContentsMargins(24, 24, 24, 24);
                break;
            case Core::FluentComponentVariant::Touch:
                variantClass = "touch";
                m_actionButton->setMinimumSize(44, 44);
                break;
            case Core::FluentComponentVariant::Dense:
                variantClass = "dense";
                setContentsMargins(4, 4, 4, 4);
                break;
            case Core::FluentComponentVariant::Accessible:
                variantClass = "accessible";
                setContentsMargins(20, 20, 20, 20);
                break;
            default:
                variantClass = "default";
                break;
        }
        
        setProperty("variant", variantClass);
        style()->polish(this);
    }

private:
    QString m_title;
    QString m_content;
    QLabel* m_titleLabel;
    QLabel* m_contentLabel;
    QPushButton* m_actionButton;
};

class ResponsiveGridWidget : public QWidget {
    Q_OBJECT

public:
    explicit ResponsiveGridWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupResponsive();
    }

private slots:
    void onBreakpointChanged(Core::FluentBreakpoint breakpoint) {
        updateGridLayout(breakpoint);
    }
    
    void onVariantChanged() {
        auto variant = static_cast<Core::FluentComponentVariant>(m_variantCombo->currentData().toInt());
        applyVariantToAllCards(variant);
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Controls
        auto* controlsGroup = new QGroupBox("Responsive Controls", this);
        auto* controlsLayout = new QHBoxLayout(controlsGroup);
        
        // Breakpoint display
        m_breakpointLabel = new QLabel("Current: Medium", this);
        
        // Variant selector
        m_variantCombo = new QComboBox(this);
        m_variantCombo->addItem("Compact", static_cast<int>(Core::FluentComponentVariant::Compact));
        m_variantCombo->addItem("Comfortable", static_cast<int>(Core::FluentComponentVariant::Comfortable));
        m_variantCombo->addItem("Spacious", static_cast<int>(Core::FluentComponentVariant::Spacious));
        m_variantCombo->addItem("Touch", static_cast<int>(Core::FluentComponentVariant::Touch));
        m_variantCombo->addItem("Dense", static_cast<int>(Core::FluentComponentVariant::Dense));
        m_variantCombo->addItem("Accessible", static_cast<int>(Core::FluentComponentVariant::Accessible));
        m_variantCombo->setCurrentIndex(1); // Comfortable
        
        connect(m_variantCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &ResponsiveGridWidget::onVariantChanged);
        
        controlsLayout->addWidget(new QLabel("Breakpoint:", this));
        controlsLayout->addWidget(m_breakpointLabel);
        controlsLayout->addStretch();
        controlsLayout->addWidget(new QLabel("Variant:", this));
        controlsLayout->addWidget(m_variantCombo);
        
        // Scrollable grid area
        auto* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        m_gridWidget = new QWidget();
        m_gridLayout = new QGridLayout(m_gridWidget);
        m_gridLayout->setSpacing(16);
        
        // Create sample cards
        createSampleCards();
        
        scrollArea->setWidget(m_gridWidget);
        
        layout->addWidget(controlsGroup);
        layout->addWidget(scrollArea);
    }
    
    void setupResponsive() {
        // Connect to responsive manager
        auto& responsiveManager = Core::FluentResponsiveManager::instance();
        connect(&responsiveManager, &Core::FluentResponsiveManager::breakpointChanged,
                this, &ResponsiveGridWidget::onBreakpointChanged);
        
        // Set up fluid typography
        auto& typographyManager = Core::FluentFluidTypographyManager::instance();
        Core::FluentFluidTypographyConfig typographyConfig;
        typographyConfig.enableFluidScaling = true;
        typographyConfig.enableResponsiveLineHeight = true;
        typographyConfig.enableResponsiveLetterSpacing = true;
        typographyManager.setConfig(typographyConfig);
    }
    
    void createSampleCards() {
        QStringList titles = {
            "Performance Monitoring", "Accessibility Features", "Responsive Design",
            "Fluid Typography", "Component Variants", "Layout Systems",
            "Animation Framework", "Theming System", "Internationalization"
        };
        
        QStringList contents = {
            "Real-time performance tracking with automated optimization suggestions.",
            "WCAG 2.1 AA compliance with screen reader support and keyboard navigation.",
            "Adaptive layouts that work seamlessly across all device sizes.",
            "Typography that scales beautifully from mobile to desktop.",
            "Multiple component variants for different use cases and contexts.",
            "Flexible layout systems including Grid, Flexbox, and Masonry.",
            "Smooth animations with reduced motion support.",
            "Comprehensive theming with dark mode and high contrast support.",
            "Full internationalization with RTL support and locale-aware formatting."
        };
        
        for (int i = 0; i < titles.size(); ++i) {
            auto* card = new ResponsiveCard(titles[i], contents[i], this);
            m_cards.append(card);
            
            // Register with responsive component manager
            Core::FluentResponsiveComponentManager::instance().registerComponent(card, {});
        }
        
        updateGridLayout(Core::FluentBreakpoint::Medium);
    }
    
    void updateGridLayout(Core::FluentBreakpoint breakpoint) {
        // Update breakpoint display
        QString breakpointName;
        int columns = 3;
        
        switch (breakpoint) {
            case Core::FluentBreakpoint::XSmall:
                breakpointName = "XSmall (< 576px)";
                columns = 1;
                break;
            case Core::FluentBreakpoint::Small:
                breakpointName = "Small (576px - 768px)";
                columns = 2;
                break;
            case Core::FluentBreakpoint::Medium:
                breakpointName = "Medium (768px - 992px)";
                columns = 2;
                break;
            case Core::FluentBreakpoint::Large:
                breakpointName = "Large (992px - 1200px)";
                columns = 3;
                break;
            case Core::FluentBreakpoint::XLarge:
                breakpointName = "XLarge (1200px - 1400px)";
                columns = 4;
                break;
            case Core::FluentBreakpoint::XXLarge:
                breakpointName = "XXLarge (> 1400px)";
                columns = 4;
                break;
            default:
                breakpointName = "Unknown";
                columns = 3;
                break;
        }
        
        m_breakpointLabel->setText(QString("Current: %1").arg(breakpointName));
        
        // Rearrange cards in grid
        for (int i = 0; i < m_cards.size(); ++i) {
            int row = i / columns;
            int col = i % columns;
            m_gridLayout->addWidget(m_cards[i], row, col);
        }
        
        // Update grid spacing based on breakpoint
        int spacing = (breakpoint <= Core::FluentBreakpoint::Small) ? 8 : 16;
        m_gridLayout->setSpacing(spacing);
    }
    
    void applyVariantToAllCards(Core::FluentComponentVariant variant) {
        auto& componentManager = Core::FluentResponsiveComponentManager::instance();
        
        for (auto* card : m_cards) {
            componentManager.setComponentVariant(card, variant);
        }
    }

private:
    QLabel* m_breakpointLabel;
    QComboBox* m_variantCombo;
    QWidget* m_gridWidget;
    QGridLayout* m_gridLayout;
    QList<ResponsiveCard*> m_cards;
};

class ResponsiveMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ResponsiveMainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupResponsive();
        startResizeTimer();
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QMainWindow::resizeEvent(event);
        
        // Update responsive manager with new size
        auto& responsiveManager = Core::FluentResponsiveManager::instance();
        responsiveManager.updateSize(event->size());
        
        // Restart timer to debounce rapid resize events
        m_resizeTimer->start();
    }

private slots:
    void onResizeTimer() {
        // Update fluid typography for new viewport size
        auto& typographyManager = Core::FluentFluidTypographyManager::instance();
        
        // Update window title font
        QFont titleFont = typographyManager.calculateFluidFont(Core::FluentTypographyStyle::Headline, size());
        // Would apply to window title if accessible
        
        qDebug() << "Window resized to:" << size() << "- Typography updated";
    }

private:
    void setupUI() {
        setWindowTitle("FluentQt Responsive Design Demo");
        setMinimumSize(320, 240);
        resize(1024, 768);
        
        auto* centralWidget = new ResponsiveGridWidget(this);
        setCentralWidget(centralWidget);
        
        // Create menu bar
        auto* viewMenu = menuBar()->addMenu("&View");
        
        auto* compactAction = viewMenu->addAction("&Compact View");
        connect(compactAction, &QAction::triggered, this, [this]() {
            setVariantForAllComponents(Core::FluentComponentVariant::Compact);
        });
        
        auto* touchAction = viewMenu->addAction("&Touch Friendly");
        connect(touchAction, &QAction::triggered, this, [this]() {
            setVariantForAllComponents(Core::FluentComponentVariant::Touch);
        });
        
        auto* accessibleAction = viewMenu->addAction("&Accessible Mode");
        connect(accessibleAction, &QAction::triggered, this, [this]() {
            setVariantForAllComponents(Core::FluentComponentVariant::Accessible);
        });
        
        // Status bar with responsive info
        auto* statusLabel = new QLabel("Ready", this);
        statusBar()->addWidget(statusLabel);
        
        // Update status bar with current breakpoint
        auto& responsiveManager = Core::FluentResponsiveManager::instance();
        connect(&responsiveManager, &Core::FluentResponsiveManager::breakpointChanged,
                this, [statusLabel](Core::FluentBreakpoint breakpoint) {
                    QString breakpointName = "Unknown";
                    switch (breakpoint) {
                        case Core::FluentBreakpoint::XSmall: breakpointName = "XSmall"; break;
                        case Core::FluentBreakpoint::Small: breakpointName = "Small"; break;
                        case Core::FluentBreakpoint::Medium: breakpointName = "Medium"; break;
                        case Core::FluentBreakpoint::Large: breakpointName = "Large"; break;
                        case Core::FluentBreakpoint::XLarge: breakpointName = "XLarge"; break;
                        case Core::FluentBreakpoint::XXLarge: breakpointName = "XXLarge"; break;
                        default: break;
                    }
                    statusLabel->setText(QString("Breakpoint: %1").arg(breakpointName));
                });
    }
    
    void setupResponsive() {
        // Configure responsive manager
        auto& responsiveManager = Core::FluentResponsiveManager::instance();
        Core::ResponsiveConfig config;
        config.enableFluidLayouts = true;
        config.enableScalableText = true;
        config.enableFluidTypography = true;
        config.enableAccessibilityScaling = true;
        config.mode = Core::FluentResponsiveMode::Hybrid;
        responsiveManager.setResponsiveConfig(config);
        
        // Initialize with current size
        responsiveManager.updateSize(size());
    }
    
    void startResizeTimer() {
        m_resizeTimer = new QTimer(this);
        m_resizeTimer->setSingleShot(true);
        m_resizeTimer->setInterval(100); // 100ms debounce
        connect(m_resizeTimer, &QTimer::timeout, this, &ResponsiveMainWindow::onResizeTimer);
    }
    
    void setVariantForAllComponents(Core::FluentComponentVariant variant) {
        auto& componentManager = Core::FluentResponsiveComponentManager::instance();
        componentManager.applyVariantToAll(variant);
    }

private:
    QTimer* m_resizeTimer;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize responsive system
    auto& responsiveManager = Core::FluentResponsiveManager::instance();
    responsiveManager.initialize();
    
    ResponsiveMainWindow window;
    window.show();
    
    return app.exec();
}

#include "ResponsiveDesignExample.moc"
