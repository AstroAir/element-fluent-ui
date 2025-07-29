// test_accessibility_fix.cpp
// Simple demonstration program to verify the accessibility fix

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QAccessible>
#include <QDebug>
#include <QTimer>
#include <memory>

#include "FluentQt/Accessibility/FluentAccessible.h"

using namespace FluentQt::Accessibility;

class TestWidget : public QWidget {
    Q_OBJECT

public:
    TestWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupAccessibility();
    }

public slots:
    void testAccessibilityWithDestroyedWidget() {
        qDebug() << "=== Testing Accessibility with Destroyed Widget ===";
        
        // Create a widget and accessibility interface
        auto testWidget = std::make_unique<QWidget>();
        testWidget->setObjectName("TestWidget");
        testWidget->setProperty("text", "Test Button");
        
        auto accessibleInterface = std::make_unique<FluentAccessibleInterface>(testWidget.get());
        
        // Test normal operation
        qDebug() << "Normal operation:";
        qDebug() << "  Name:" << accessibleInterface->text(QAccessible::Name);
        qDebug() << "  Role:" << accessibleInterface->role();
        qDebug() << "  Description:" << accessibleInterface->description();
        
        // Destroy the widget
        qDebug() << "\nDestroying widget...";
        testWidget.reset();
        
        // Test operations with destroyed widget - should not crash
        qDebug() << "Operations with destroyed widget (should not crash):";
        
        try {
            QString name = accessibleInterface->text(QAccessible::Name);
            qDebug() << "  Name (should be empty):" << name;
            
            QAccessible::Role role = accessibleInterface->role();
            qDebug() << "  Role (should be default):" << role;
            
            QString description = accessibleInterface->description();
            qDebug() << "  Description (should be empty):" << description;
            
            QString help = accessibleInterface->help();
            qDebug() << "  Help (should be empty):" << help;
            
            // Test setText - should not crash
            accessibleInterface->setText(QAccessible::Name, "NewName");
            qDebug() << "  setText completed without crash";
            
            qDebug() << "\n✅ All operations completed without segmentation fault!";
            
        } catch (const std::exception& e) {
            qCritical() << "❌ Exception caught:" << e.what();
        } catch (...) {
            qCritical() << "❌ Unknown exception caught";
        }
    }

private:
    void setupUI() {
        auto layout = new QVBoxLayout(this);
        
        auto label = new QLabel("Accessibility Fix Test", this);
        label->setObjectName("TitleLabel");
        layout->addWidget(label);
        
        auto testButton = new QPushButton("Test Accessibility Fix", this);
        testButton->setObjectName("TestButton");
        connect(testButton, &QPushButton::clicked, this, &TestWidget::testAccessibilityWithDestroyedWidget);
        layout->addWidget(testButton);
        
        auto infoLabel = new QLabel(
            "Click the button to test the accessibility fix.\n"
            "Check the console output for results.", this);
        infoLabel->setWordWrap(true);
        layout->addWidget(infoLabel);
        
        setLayout(layout);
        setWindowTitle("FluentQt Accessibility Fix Test");
        resize(400, 200);
    }
    
    void setupAccessibility() {
        // Set up accessibility for this widget
        setAccessibleName("Accessibility Test Window");
        setAccessibleDescription("Window for testing FluentQt accessibility fixes");
        
        // Enable accessibility
        QAccessible::setActive(true);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "FluentQt Accessibility Fix Test";
    qDebug() << "==============================";
    
    // Initialize accessibility
    QAccessible::setActive(true);
    
    TestWidget window;
    window.show();
    
    // Run automatic test
    QTimer::singleShot(1000, [&window]() {
        window.testAccessibilityWithDestroyedWidget();
    });
    
    return app.exec();
}

#include "test_accessibility_fix.moc"
