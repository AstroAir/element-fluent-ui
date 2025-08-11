#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <iostream>

// Test the accessibility fix
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Print platform information
    qDebug() << "Platform name from environment:" << qgetenv("QT_QPA_PLATFORM");
    qDebug() << "Application arguments:" << app.arguments();
    
    // Check if we're in offscreen mode
    QString platformName = qgetenv("QT_QPA_PLATFORM");
    if (platformName.isEmpty()) {
        QStringList args = QApplication::arguments();
        for (int i = 0; i < args.size() - 1; ++i) {
            if (args[i] == "-platform") {
                platformName = args[i + 1];
                break;
            }
        }
    }
    
    qDebug() << "Detected platform:" << platformName;
    
    if (platformName == "offscreen" || platformName == "minimal") {
        qDebug() << "SUCCESS: Detected offscreen/minimal platform - accessibility should be safe";
    } else {
        qDebug() << "INFO: Not in offscreen mode, normal accessibility initialization expected";
    }
    
    // Exit after a short delay to test if we hang
    QTimer::singleShot(2000, [&]() {
        qDebug() << "SUCCESS: Application completed without hanging";
        app.quit();
    });
    
    qDebug() << "Starting event loop...";
    return app.exec();
}
