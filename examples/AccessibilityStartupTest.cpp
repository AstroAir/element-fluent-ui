// examples/AccessibilityStartupTest.cpp
// Test program to verify accessibility initialization doesn't freeze during startup

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QVBoxLayout>

#include "FluentQt/Accessibility/FluentAccessibilityManager.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class AccessibilityStartupTest : public QMainWindow {
    Q_OBJECT

public:
    AccessibilityStartupTest(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupAccessibilityTest();
        
        setWindowTitle("FluentQt Accessibility Startup Test");
        resize(600, 400);
        
        // Log startup progress
        qDebug() << "AccessibilityStartupTest window created";
    }

private slots:
    void onTestAccessibility() {
        qDebug() << "Testing accessibility manager...";
        
        auto& manager = Accessibility::FluentAccessibilityManager::instance();
        
        // Test basic functionality
        manager.setHighContrastMode(true);
        manager.setReducedMotionMode(false);
        manager.setScreenReaderMode(false);
        
        qDebug() << "Accessibility settings:";
        qDebug() << "  High contrast:" << manager.isHighContrastMode();
        qDebug() << "  Reduced motion:" << manager.isReducedMotionMode();
        qDebug() << "  Screen reader:" << manager.isScreenReaderMode();
        
        m_statusLabel->setText("Accessibility test completed successfully!");
        m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
    }
    
    void onSkipTest() {
        qDebug() << "Skipping accessibility test";
        m_statusLabel->setText("Accessibility test skipped");
        m_statusLabel->setStyleSheet("color: orange;");
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* layout = new QVBoxLayout(centralWidget);
        
        // Title
        auto* titleLabel = new QLabel("FluentQt Accessibility Startup Test");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 20px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);
        
        // Description
        auto* descLabel = new QLabel(
            "This test verifies that the FluentQt accessibility system "
            "initializes without freezing during startup.\n\n"
            "If you can see this window and interact with the buttons below, "
            "the accessibility initialization is working correctly.");
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("margin: 10px; padding: 10px;");
        layout->addWidget(descLabel);
        
        // Status label
        m_statusLabel = new QLabel("Ready to test accessibility...");
        m_statusLabel->setStyleSheet("margin: 10px; padding: 10px; border: 1px solid gray;");
        m_statusLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(m_statusLabel);
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout();
        
        auto* testButton = new Components::FluentButton("Test Accessibility", this);
        testButton->setButtonStyle(Components::FluentButtonStyle::Accent);
        connect(testButton, &Components::FluentButton::clicked, this, 
                &AccessibilityStartupTest::onTestAccessibility);
        buttonLayout->addWidget(testButton);
        
        auto* skipButton = new Components::FluentButton("Skip Test", this);
        connect(skipButton, &Components::FluentButton::clicked, this, 
                &AccessibilityStartupTest::onSkipTest);
        buttonLayout->addWidget(skipButton);
        
        layout->addLayout(buttonLayout);
        layout->addStretch();
    }
    
    void setupAccessibilityTest() {
        // Set up a timer to automatically test after a short delay
        QTimer::singleShot(2000, [this]() {
            qDebug() << "Auto-testing accessibility after 2 second delay...";
            onTestAccessibility();
        });
    }

private:
    QLabel* m_statusLabel;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "Starting FluentQt Accessibility Startup Test...";
    
    // Set application properties
    app.setApplicationName("FluentQt Accessibility Startup Test");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt Tests");
    
    // Initialize FluentQt theme
    qDebug() << "Initializing FluentQt theme...";
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::Light);
    qDebug() << "FluentQt theme initialized";
    
    // Create and show test window
    qDebug() << "Creating test window...";
    AccessibilityStartupTest testWindow;
    testWindow.show();
    qDebug() << "Test window shown - startup completed successfully!";
    
    return app.exec();
}

#include "AccessibilityStartupTest.moc"
