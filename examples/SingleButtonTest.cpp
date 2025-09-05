/**
 * @file SingleButtonTest.cpp
 * @brief Test single FluentButton to isolate accessibility crash
 * 
 * This is the most minimal test to isolate exactly where the
 * accessibility crash occurs with FluentQt components.
 * 
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>

// Only include FluentQt theme and single component
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Components/FluentButton.h"

using namespace FluentQt;
using namespace FluentQt::Components;

int main(int argc, char* argv[]) {
    // Set environment variables BEFORE creating QApplication
    qputenv("FLUENTQT_SKIP_PROCESS_DETECTION", "1");
    qputenv("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION", "1");
    qputenv("QT_ACCESSIBILITY", "0");
    
    qDebug() << "Creating QApplication...";
    QApplication app(argc, argv);
    qDebug() << "QApplication created successfully";
    
    // Set application properties
    app.setApplicationName("FluentQt Single Button Test");
    app.setApplicationVersion("1.0");
    
    qDebug() << "Initializing FluentQt theme system...";
    try {
        auto& theme = Styling::FluentTheme::instance();
        qDebug() << "FluentTheme instance obtained";
        
        theme.setMode(Styling::FluentThemeMode::System);
        qDebug() << "FluentTheme mode set to System";
    } catch (const std::exception& e) {
        qCritical() << "Exception in theme initialization:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "Unknown exception in theme initialization";
        return 1;
    }
    
    qDebug() << "Creating main widget...";
    QWidget mainWidget;
    mainWidget.setWindowTitle("FluentQt Single Button Test");
    mainWidget.resize(300, 200);
    
    auto* layout = new QVBoxLayout(&mainWidget);
    
    qDebug() << "Creating FluentButton...";
    try {
        auto* button = new FluentButton("Test Button");
        qDebug() << "FluentButton created successfully";
        
        button->setButtonStyle(FluentButtonStyle::Primary);
        qDebug() << "FluentButton style set";
        
        layout->addWidget(button);
        qDebug() << "FluentButton added to layout";
        
    } catch (const std::exception& e) {
        qCritical() << "Exception creating FluentButton:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "Unknown exception creating FluentButton";
        return 1;
    }
    
    qDebug() << "Showing widget...";
    mainWidget.show();
    qDebug() << "Widget shown successfully";
    
    qDebug() << "Starting event loop...";
    return app.exec();
}
