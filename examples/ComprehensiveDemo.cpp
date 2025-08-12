// examples/ComprehensiveDemo.cpp
// Comprehensive demonstration of all enhanced FluentQt components and systems

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>

// FluentQt includes
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentBreadcrumb.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class ComprehensiveDemoWindow : public QMainWindow {
    Q_OBJECT

public:
    ComprehensiveDemoWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupAnimations();
        setupConnections();
        setupTheme();

        setWindowTitle("FluentQt Comprehensive Demo - Enhanced Components");
        resize(1200, 800);
    }

private slots:
    void onThemeToggle() {
        auto& theme = Styling::FluentTheme::instance();
        theme.setDarkMode(!theme.isDarkMode());
        updateThemeUI();
    }

    void onAccentColorChanged() {
        auto& theme = Styling::FluentTheme::instance();

        // Cycle through accent colors
        static int colorIndex = 0;
        const QList<QColor> colors = {
            QColor(0, 120, 215),   // Windows Blue
            QColor(16, 124, 16),   // Green
            QColor(196, 43, 28),   // Red
            QColor(136, 23, 152),  // Purple
            QColor(0, 153, 188),   // Teal
            QColor(255, 140, 0)    // Orange
        };

        theme.setAccentColor(colors[colorIndex % colors.size()]);
        colorIndex++;

        m_statusLabel->setText(QString("Accent color changed to: %1")
                                   .arg(theme.accentColor().name()));
    }

    void onAnimationDemo() {
        // Demonstrate various animations
        static int animIndex = 0;
        const QList<std::function<void()>> animations = {
            [this]() {
                m_animator->fadeIn(m_demoWidget, 500,
                                   Animation::FluentEasing::CubicOut);
            },
            [this]() {
                m_animator->slideUp(m_demoWidget, 400,
                                    Animation::FluentEasing::ElasticOut);
            },
            [this]() {
                m_animator->scaleIn(m_demoWidget, 300,
                                    Animation::FluentEasing::BackOut);
            },
            [this]() { m_animator->pulseEffect(m_demoWidget); },
            [this]() { m_animator->shakeEffect(m_demoWidget); },
            [this]() { m_animator->hoverEffect(m_demoWidget); }};

        animations[animIndex % animations.size()]();
        animIndex++;
    }

    void onFormValidation() {
        bool isValid = true;
        QString errors;

        // Validate email
        if (!m_emailInput->isValid()) {
            isValid = false;
            errors += "• Invalid email address\n";
            m_animator->shakeEffect(m_emailInput);
        }

        // Validate name
        if (m_nameInput->text().length() < 2) {
            isValid = false;
            errors += "• Name must be at least 2 characters\n";
            m_animator->shakeEffect(m_nameInput);
        }

        // Check agreement
        if (!m_agreeCheckBox->isChecked()) {
            isValid = false;
            errors += "• You must agree to the terms\n";
            m_animator->shakeEffect(m_agreeCheckBox);
        }

        if (isValid) {
            m_statusLabel->setText("Form validation successful!");
            m_animator->pulseEffect(m_submitButton);

            // Show success feedback
            m_progressBar->setType(Components::FluentProgressType::Determinate);
            m_progressBar->setValue(0);
            m_progressBar->show();

            // Simulate processing
            auto* timer = new QTimer(this);
            connect(timer, &QTimer::timeout, [this, timer]() {
                int value = m_progressBar->value() + 10;
                m_progressBar->setValue(value);

                if (value >= 100) {
                    timer->stop();
                    timer->deleteLater();
                    m_progressBar->hide();
                    m_statusLabel->setText("Form submitted successfully!");
                }
            });
            timer->start(100);

        } else {
            m_statusLabel->setText("Form validation failed:\n" + errors);
            m_animator->shakeEffect(m_formPanel);
        }
    }

    void onLoadingDemo() {
        static bool isLoading = false;

        if (!isLoading) {
            isLoading = true;
            m_loadingIndicator->start();
            m_loadingButton->setText("Stop Loading");
            m_statusLabel->setText("Loading demonstration started...");

            // Auto-stop after 3 seconds
            QTimer::singleShot(3000, [this]() { onLoadingDemo(); });
        } else {
            isLoading = false;
            m_loadingIndicator->stop();
            m_loadingButton->setText("Start Loading");
            m_statusLabel->setText("Loading demonstration stopped.");
        }
    }

    void onPanelToggle() {
        bool isExpanded = m_collapsiblePanel->isExpanded();
        m_collapsiblePanel->setExpanded(!isExpanded);

        m_statusLabel->setText(
            QString("Panel %1").arg(isExpanded ? "collapsed" : "expanded"));
    }

    void onBreadcrumbNavigation(int index) {
        m_statusLabel->setText(
            QString("Navigated to: %1").arg(m_breadcrumb->itemText(index)));

        // Animate navigation feedback
        m_animator->pulseEffect(m_breadcrumb);
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // Setup menu bar
        setupMenuBar();

        // Title and breadcrumb
        setupHeader(mainLayout);

        // Main content area
        auto* splitter = new QSplitter(Qt::Horizontal, this);

        // Left panel - Controls and demos
        setupControlsPanel(splitter);

        // Right panel - Form and feedback demos
        setupDemoPanel(splitter);

        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);
        splitter->setSizes({600, 600});

        mainLayout->addWidget(splitter, 1);

        // Status bar
        m_statusLabel = new QLabel("FluentQt Comprehensive Demo Ready", this);
        statusBar()->addWidget(m_statusLabel);
    }

    void setupMenuBar() {
        auto* themeMenu = menuBar()->addMenu("Theme");

        auto* toggleThemeAction = themeMenu->addAction("Toggle Dark Mode");
        connect(toggleThemeAction, &QAction::triggered, this,
                &ComprehensiveDemoWindow::onThemeToggle);

        auto* changeAccentAction = themeMenu->addAction("Change Accent Color");
        connect(changeAccentAction, &QAction::triggered, this,
                &ComprehensiveDemoWindow::onAccentColorChanged);

        auto* animationMenu = menuBar()->addMenu("Animation");

        auto* demoAnimationAction = animationMenu->addAction("Demo Animation");
        connect(demoAnimationAction, &QAction::triggered, this,
                &ComprehensiveDemoWindow::onAnimationDemo);

        auto* helpMenu = menuBar()->addMenu("Help");
        helpMenu->addAction("About FluentQt");
    }

    void setupHeader(QVBoxLayout* mainLayout) {
        // Title
        auto* titleLabel =
            new QLabel("FluentQt Enhanced Components Demo", this);
        auto& theme = Styling::FluentTheme::instance();
        titleLabel->setFont(theme.displayFont());
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Breadcrumb navigation
        m_breadcrumb = new Components::FluentBreadcrumb(this);
        m_breadcrumb->addItem(QIcon(":/icons/home"), "Home");
        m_breadcrumb->addItem("Components");
        m_breadcrumb->addItem("Demo");
        m_breadcrumb->addItem("Comprehensive");
        m_breadcrumb->setCurrentIndex(3);
        m_breadcrumb->setStyle(Components::FluentBreadcrumbStyle::Pills);

        connect(m_breadcrumb, &Components::FluentBreadcrumb::itemClicked, this,
                &ComprehensiveDemoWindow::onBreadcrumbNavigation);

        mainLayout->addWidget(m_breadcrumb);
    }

    void setupControlsPanel(QSplitter* splitter) {
        auto* controlsArea = new QScrollArea(this);
        auto* controlsWidget = new QWidget();
        auto* controlsLayout = new QVBoxLayout(controlsWidget);
        controlsLayout->setSpacing(16);

        // Animation demo panel
        auto* animPanel = new Components::FluentPanel("Animation Demos", this);
        animPanel->setPanelType(Components::FluentPanelType::Card);
        animPanel->setElevation(Components::FluentPanelElevation::Medium);

        auto* animLayout = animPanel->createVerticalLayout();

        m_demoWidget = new QLabel("Animation Target", this);
        m_demoWidget->setAlignment(Qt::AlignCenter);
        m_demoWidget->setMinimumSize(200, 100);
        m_demoWidget->setStyleSheet(
            "background-color: lightblue; border-radius: 8px;");
        animLayout->addWidget(m_demoWidget);

        auto* animButton = new QPushButton("Demo Animation", this);
        connect(animButton, &QPushButton::clicked, this,
                &ComprehensiveDemoWindow::onAnimationDemo);
        animLayout->addWidget(animButton);

        controlsLayout->addWidget(animPanel);

        // Loading demo panel
        auto* loadingPanel =
            new Components::FluentPanel("Loading Indicators", this);
        loadingPanel->setPanelType(Components::FluentPanelType::Surface);

        auto* loadingLayout = loadingPanel->createVerticalLayout();

        m_loadingIndicator = new Components::FluentLoadingIndicator(this);
        m_loadingIndicator->setType(Components::FluentLoadingType::Spinner);
        m_loadingIndicator->setSize(Components::FluentLoadingSize::Large);
        m_loadingIndicator->setText("Processing...");
        loadingLayout->addWidget(m_loadingIndicator);

        m_loadingButton = new QPushButton("Start Loading", this);
        connect(m_loadingButton, &QPushButton::clicked, this,
                &ComprehensiveDemoWindow::onLoadingDemo);
        loadingLayout->addWidget(m_loadingButton);

        controlsLayout->addWidget(loadingPanel);

        // Progress demo panel
        auto* progressPanel =
            new Components::FluentPanel("Progress Indicators", this);
        auto* progressLayout = progressPanel->createVerticalLayout();

        m_progressBar = new Components::FluentProgressBar(this);
        m_progressBar->setType(Components::FluentProgressType::Determinate);
        m_progressBar->setShowText(true);
        m_progressBar->hide();
        progressLayout->addWidget(m_progressBar);

        controlsLayout->addWidget(progressPanel);

        // Collapsible panel demo
        m_collapsiblePanel =
            new Components::FluentPanel("Collapsible Panel", this);
        m_collapsiblePanel->setCollapsible(true);
        m_collapsiblePanel->setExpanded(true);

        auto* collapsibleLayout = m_collapsiblePanel->createVerticalLayout();
        collapsibleLayout->addWidget(
            new QLabel("This panel can be collapsed", this));

        auto* toggleButton = new QPushButton("Toggle Panel", this);
        connect(toggleButton, &QPushButton::clicked, this,
                &ComprehensiveDemoWindow::onPanelToggle);
        collapsibleLayout->addWidget(toggleButton);

        controlsLayout->addWidget(m_collapsiblePanel);

        controlsLayout->addStretch();

        controlsArea->setWidget(controlsWidget);
        controlsArea->setWidgetResizable(true);
        controlsArea->setMinimumWidth(350);

        splitter->addWidget(controlsArea);
    }

    void setupDemoPanel(QSplitter* splitter) {
        auto* demoArea = new QScrollArea(this);
        auto* demoWidget = new QWidget();
        auto* demoLayout = new QVBoxLayout(demoWidget);
        demoLayout->setSpacing(20);

        // Form demo panel
        m_formPanel =
            new Components::FluentPanel("Enhanced Form Components", this);
        m_formPanel->setPanelType(Components::FluentPanelType::Card);
        m_formPanel->setElevation(Components::FluentPanelElevation::High);

        auto* formLayout = m_formPanel->createVerticalLayout();

        // Name input
        m_nameInput = new Components::FluentTextInput("Full Name", this);
        m_nameInput->setValidationType(
            Components::FluentTextInputValidation::None);
        formLayout->addWidget(m_nameInput);

        // Email input with validation
        m_emailInput = new Components::FluentTextInput("Email Address", this);
        m_emailInput->setValidationType(
            Components::FluentTextInputValidation::Email);
        formLayout->addWidget(m_emailInput);

        // Password input
        auto* passwordInput = new Components::FluentTextInput("Password", this);
        passwordInput->setPasswordMode(true);
        formLayout->addWidget(passwordInput);

        // Agreement checkbox
        m_agreeCheckBox = new Components::FluentCheckBox(
            "I agree to the terms and conditions", this);
        formLayout->addWidget(m_agreeCheckBox);

        // Submit button
        m_submitButton = new QPushButton("Submit Form", this);
        connect(m_submitButton, &QPushButton::clicked, this,
                &ComprehensiveDemoWindow::onFormValidation);
        formLayout->addWidget(m_submitButton);

        demoLayout->addWidget(m_formPanel);

        // Theme controls panel
        auto* themePanel = new Components::FluentPanel("Theme Controls", this);
        auto* themeLayout = themePanel->createHorizontalLayout();

        auto* themeButton = new QPushButton("Toggle Theme", this);
        connect(themeButton, &QPushButton::clicked, this,
                &ComprehensiveDemoWindow::onThemeToggle);
        themeLayout->addWidget(themeButton);

        auto* accentButton = new QPushButton("Change Accent", this);
        connect(accentButton, &QPushButton::clicked, this,
                &ComprehensiveDemoWindow::onAccentColorChanged);
        themeLayout->addWidget(accentButton);

        demoLayout->addWidget(themePanel);

        demoLayout->addStretch();

        demoArea->setWidget(demoWidget);
        demoArea->setWidgetResizable(true);

        splitter->addWidget(demoArea);
    }

    void setupAnimations() { m_animator = new Animation::FluentAnimator(this); }

    void setupConnections() {
        // Connect theme changes to UI updates
        auto& theme = Styling::FluentTheme::instance();
        connect(&theme, &Styling::FluentTheme::themeChanged, this,
                &ComprehensiveDemoWindow::updateThemeUI);
        connect(&theme, &Styling::FluentTheme::accentColorChanged, this,
                &ComprehensiveDemoWindow::updateThemeUI);

        // Connect form validation
        connect(
            m_emailInput, &Components::FluentTextInput::validationStateChanged,
            [this](bool isValid) {
                m_statusLabel->setText(QString("Email validation: %1")
                                           .arg(isValid ? "Valid" : "Invalid"));
            });
    }

    void setupTheme() {
        auto& theme = Styling::FluentTheme::instance();
        theme.setAccentColor(QColor(0, 120, 215));  // Windows blue
        updateThemeUI();
    }

    void updateThemeUI() {
        auto& theme = Styling::FluentTheme::instance();

        // Update main window background
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                          .arg(theme.color("backgroundPrimary").name()));

        // Update status
        m_statusLabel->setText(QString("Theme: %1 mode, Accent: %2")
                                   .arg(theme.isDarkMode() ? "Dark" : "Light")
                                   .arg(theme.accentColor().name()));
    }

private:
    // Animation system
    Animation::FluentAnimator* m_animator{nullptr};

    // Navigation
    Components::FluentBreadcrumb* m_breadcrumb{nullptr};

    // Form components
    Components::FluentPanel* m_formPanel{nullptr};
    Components::FluentTextInput* m_nameInput{nullptr};
    Components::FluentTextInput* m_emailInput{nullptr};
    Components::FluentCheckBox* m_agreeCheckBox{nullptr};
    QPushButton* m_submitButton{nullptr};

    // Feedback components
    Components::FluentProgressBar* m_progressBar{nullptr};
    Components::FluentLoadingIndicator* m_loadingIndicator{nullptr};
    QPushButton* m_loadingButton{nullptr};

    // Layout components
    Components::FluentPanel* m_collapsiblePanel{nullptr};

    // Demo widgets
    QLabel* m_demoWidget{nullptr};

    // Status
    QLabel* m_statusLabel{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(QColor(0, 120, 215));

    ComprehensiveDemoWindow window;
    window.show();

    return app.exec();
}

#include "ComprehensiveDemo.moc"
