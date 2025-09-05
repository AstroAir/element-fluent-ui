/**
 * @file ThemeOnlyTest.cpp
 * @brief Test FluentQt theme system without components
 * 
 * This test only uses the FluentQt theme system to isolate
 * whether the crash is in the theme or in the components.
 * 
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

// Only include FluentQt theme system
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

int main(int argc, char* argv[]) {
    // Set environment variables BEFORE creating QApplication
    qputenv("FLUENTQT_SKIP_PROCESS_DETECTION", "1");
    qputenv("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION", "1");
    qputenv("QT_ACCESSIBILITY", "0");
    
    qDebug() << "Creating QApplication...";
    QApplication app(argc, argv);
    qDebug() << "QApplication created successfully";
    
    // Set application properties
    app.setApplicationName("FluentQt Theme Only Test");
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
    mainWidget.setWindowTitle("FluentQt Theme Only Test");
    mainWidget.resize(400, 300);
    
    auto* layout = new QVBoxLayout(&mainWidget);
    
    qDebug() << "Creating QLabel...";
    auto* label = new QLabel("This is a standard Qt label with FluentQt theme");
    layout->addWidget(label);
    
    qDebug() << "Creating QPushButton...";
    auto* button = new QPushButton("Standard Qt Button with FluentQt theme");
    layout->addWidget(button);
    
    // Add theme toggle button
    auto* themeButton = new QPushButton("Toggle Theme");
    QObject::connect(themeButton, &QPushButton::clicked, [&]() {
        qDebug() << "Theme toggle clicked";
        try {
            auto& theme = Styling::FluentTheme::instance();
            auto currentMode = theme.mode();
            if (currentMode == Styling::FluentThemeMode::Light) {
                theme.setMode(Styling::FluentThemeMode::Dark);
                qDebug() << "Switched to Dark mode";
            } else {
                theme.setMode(Styling::FluentThemeMode::Light);
                qDebug() << "Switched to Light mode";
            }
        } catch (const std::exception& e) {
            qCritical() << "Exception in theme toggle:" << e.what();
        } catch (...) {
            qCritical() << "Unknown exception in theme toggle";
        }
    });
    layout->addWidget(themeButton);
    
    qDebug() << "Showing widget...";
    mainWidget.show();
    
    qDebug() << "Starting event loop...";
    return app.exec();
}
