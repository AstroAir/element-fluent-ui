#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentOptimizedEasing.h"
#include "FluentQt/Components/FluentSidebar.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Animation;
using namespace FluentQt::Styling;

class EnhancedSidebarDemo : public QMainWindow {
    Q_OBJECT

public:
    EnhancedSidebarDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupSidebar();
        setupAnimations();
        setupMenus();

        setWindowTitle("Enhanced Sidebar & Animation Demo");
        resize(1200, 800);

        // Apply Fluent theme
        FluentTheme::instance().setTheme(FluentTheme::Light);
    }

private slots:
    void onSidebarItemClicked(const QString& id) {
        m_contentLabel->setText(QString("Selected: %1").arg(id));

        // Demonstrate smooth content transitions
        auto fadeOut = FluentAnimator::fadeOut(m_contentArea);
        connect(fadeOut.get(), &QPropertyAnimation::finished, [this, id]() {
            updateContent(id);
            auto fadeIn = FluentAnimator::fadeIn(m_contentArea);
            fadeIn->start();
        });
        fadeOut->start();
    }

    void toggleSidebar() { m_sidebar->toggle(); }

    void setSidebarMode(FluentSidebarMode mode) { m_sidebar->setMode(mode); }

    void demonstrateAnimations() {
        // Demonstrate various optimized animations
        auto config = FluentAnimationConfig{};
        config.duration = std::chrono::milliseconds(300);
        config.easing = FluentEasing::EaseOut;
        config.useHardwareAcceleration = true;

        // Stagger animation for sidebar items
        QList<QWidget*> widgets;
        for (int i = 0; i < m_sidebar->itemCount(); ++i) {
            // This would need access to internal widgets
            // widgets.append(m_sidebar->itemWidget(i));
        }

        // Demonstrate pulse effect on content
        auto pulse = FluentAnimator::pulseEffect(m_contentArea, config);
        pulse->start();
    }

    void onSidebarModeChanged(FluentSidebarMode mode) {
        QString modeText;
        switch (mode) {
            case FluentSidebarMode::Expanded:
                modeText = "Expanded";
                break;
            case FluentSidebarMode::Compact:
                modeText = "Compact";
                break;
            case FluentSidebarMode::Hidden:
                modeText = "Hidden";
                break;
            case FluentSidebarMode::Overlay:
                modeText = "Overlay";
                break;
            case FluentSidebarMode::Auto:
                modeText = "Auto";
                break;
        }

        statusBar()->showMessage(QString("Sidebar mode: %1").arg(modeText));
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget();
        setCentralWidget(centralWidget);

        auto* mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Create sidebar
        m_sidebar = new FluentSidebar();
        m_sidebar->setCollapsible(true);
        m_sidebar->setAutoHide(true);
        m_sidebar->enableResponsiveBehavior(true);
        m_sidebar->setResponsiveBreakpoints(768, 480);

        // Create content area
        m_contentArea = new QWidget();
        auto* contentLayout = new QVBoxLayout(m_contentArea);

        m_contentLabel = new QLabel("Welcome to Enhanced Sidebar Demo");
        m_contentLabel->setAlignment(Qt::AlignCenter);
        m_contentLabel->setStyleSheet(
            "font-size: 24px; font-weight: bold; margin: 20px;");

        auto* textEdit = new QTextEdit();
        textEdit->setPlainText(
            "This demo showcases the enhanced sidebar component with:\n\n"
            "• Smooth animations with GPU acceleration\n"
            "• Responsive behavior for different screen sizes\n"
            "• Comprehensive accessibility support\n"
            "• Keyboard navigation (Up/Down arrows, Enter, Escape)\n"
            "• Multiple display modes (Expanded, Compact, Overlay, Hidden)\n"
            "• Optimized easing curves for smooth transitions\n"
            "• Performance monitoring and adaptive quality\n\n"
            "Try resizing the window to see responsive behavior!\n"
            "Use keyboard navigation to navigate the sidebar items.");

        contentLayout->addWidget(m_contentLabel);
        contentLayout->addWidget(textEdit);

        // Add to main layout
        mainLayout->addWidget(m_sidebar);
        mainLayout->addWidget(m_contentArea, 1);

        // Connect signals
        connect(m_sidebar, &FluentSidebar::itemClicked, this,
                &EnhancedSidebarDemo::onSidebarItemClicked);
        connect(m_sidebar, &FluentSidebar::modeChanged, this,
                &EnhancedSidebarDemo::onSidebarModeChanged);
    }

    void setupSidebar() {
        // Add header
        auto* header = new QLabel("Navigation");
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet(
            "font-weight: bold; padding: 16px; background: #f0f0f0;");
        m_sidebar->setHeader(header);

        // Add navigation items
        FluentSidebarItem homeItem;
        homeItem.id = "home";
        homeItem.text = "Home";
        homeItem.icon = QIcon(":/icons/home.png");
        homeItem.tooltip = "Go to home page";
        homeItem.accessibleName = "Home navigation item";
        homeItem.accessibleDescription = "Navigate to the home page";
        m_sidebar->addItem(homeItem);

        FluentSidebarItem documentsItem;
        documentsItem.id = "documents";
        documentsItem.text = "Documents";
        documentsItem.icon = QIcon(":/icons/documents.png");
        documentsItem.tooltip = "View documents";
        documentsItem.accessibleName = "Documents navigation item";
        m_sidebar->addItem(documentsItem);

        FluentSidebarItem settingsItem;
        settingsItem.id = "settings";
        settingsItem.text = "Settings";
        settingsItem.icon = QIcon(":/icons/settings.png");
        settingsItem.tooltip = "Application settings";
        settingsItem.accessibleName = "Settings navigation item";
        m_sidebar->addItem(settingsItem);

        // Add separator
        FluentSidebarItem separator;
        separator.separator = true;
        m_sidebar->addItem(separator);

        FluentSidebarItem helpItem;
        helpItem.id = "help";
        helpItem.text = "Help";
        helpItem.icon = QIcon(":/icons/help.png");
        helpItem.tooltip = "Get help";
        helpItem.accessibleName = "Help navigation item";
        m_sidebar->addItem(helpItem);

        // Set initial selection
        m_sidebar->setSelectedItem("home");

        // Add footer with toggle button
        auto* footer = new QWidget();
        auto* footerLayout = new QVBoxLayout(footer);
        auto* toggleButton = new QPushButton("Toggle");
        toggleButton->setToolTip("Toggle sidebar mode");
        connect(toggleButton, &QPushButton::clicked, this,
                &EnhancedSidebarDemo::toggleSidebar);
        footerLayout->addWidget(toggleButton);
        m_sidebar->setFooter(footer);
    }

    void setupAnimations() {
        // Configure optimized easing for better performance
        FluentOptimizedEasing::OptimizationConfig config;
        config.targetFPS = 60.0;
        config.enableAntiJank = true;
        config.enablePerceptualOptimization = true;
        config.respectReducedMotion = true;

        // Set animation properties for sidebar
        m_sidebar->setAnimationDuration(250);
        m_sidebar->setAnimationEasing(QEasingCurve::OutCubic);
    }

    void setupMenus() {
        auto* viewMenu = menuBar()->addMenu("&View");

        auto* expandAction = viewMenu->addAction("&Expanded");
        connect(expandAction, &QAction::triggered,
                [this]() { setSidebarMode(FluentSidebarMode::Expanded); });

        auto* compactAction = viewMenu->addAction("&Compact");
        connect(compactAction, &QAction::triggered,
                [this]() { setSidebarMode(FluentSidebarMode::Compact); });

        auto* overlayAction = viewMenu->addAction("&Overlay");
        connect(overlayAction, &QAction::triggered,
                [this]() { setSidebarMode(FluentSidebarMode::Overlay); });

        auto* autoAction = viewMenu->addAction("&Auto");
        connect(autoAction, &QAction::triggered,
                [this]() { setSidebarMode(FluentSidebarMode::Auto); });

        viewMenu->addSeparator();

        auto* animationAction = viewMenu->addAction("Demo &Animations");
        connect(animationAction, &QAction::triggered, this,
                &EnhancedSidebarDemo::demonstrateAnimations);

        statusBar()->showMessage(
            "Ready - Try resizing the window or using keyboard navigation!");
    }

    void updateContent(const QString& id) {
        QString content;
        if (id == "home") {
            content = "Home Page Content";
        } else if (id == "documents") {
            content = "Documents Page Content";
        } else if (id == "settings") {
            content = "Settings Page Content";
        } else if (id == "help") {
            content = "Help Page Content";
        } else {
            content = "Unknown Page";
        }

        m_contentLabel->setText(content);
    }

    FluentSidebar* m_sidebar = nullptr;
    QWidget* m_contentArea = nullptr;
    QLabel* m_contentLabel = nullptr;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt
    FluentTheme::instance().initialize(&app);

    EnhancedSidebarDemo demo;
    demo.show();

    return app.exec();
}

#include "EnhancedSidebarExample.moc"
