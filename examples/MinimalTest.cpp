/**
 * @file MinimalTest.cpp
 * @brief Minimal test to isolate FluentQt accessibility crash
 * 
 * This is the most basic possible FluentQt application to test
 * where exactly the crash occurs.
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

int main(int argc, char* argv[]) {
    // Set environment variables BEFORE creating QApplication
    qputenv("FLUENTQT_SKIP_PROCESS_DETECTION", "1");
    qputenv("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION", "1");
    qputenv("QT_ACCESSIBILITY", "0");
    
    qDebug() << "Creating QApplication...";
    QApplication app(argc, argv);
    qDebug() << "QApplication created successfully";
    
    // Set application properties
    app.setApplicationName("FluentQt Minimal Test");
    app.setApplicationVersion("1.0");
    
    qDebug() << "Creating main widget...";
    QWidget mainWidget;
    mainWidget.setWindowTitle("FluentQt Minimal Test");
    mainWidget.resize(400, 300);
    
    auto* layout = new QVBoxLayout(&mainWidget);
    
    qDebug() << "Creating QLabel...";
    auto* label = new QLabel("This is a standard Qt label");
    layout->addWidget(label);
    
    qDebug() << "Creating QPushButton...";
    auto* button = new QPushButton("Standard Qt Button");
    layout->addWidget(button);
    
    qDebug() << "Showing widget...";
    mainWidget.show();
    
    qDebug() << "Starting event loop...";
    return app.exec();
}
