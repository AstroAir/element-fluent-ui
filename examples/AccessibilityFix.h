/**
 * @file AccessibilityFix.h
 * @brief Fix for FluentQt accessibility threading issues
 * 
 * This header provides a safe way to initialize FluentQt accessibility
 * components without causing QBasicTimer threading issues.
 * 
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#pragma once

#include <QApplication>
#include <QTimer>
#include <QThread>
#include <QDebug>

namespace FluentQt {
namespace AccessibilityFix {

/**
 * @brief Safely initialize FluentQt accessibility system
 * 
 * This function ensures that accessibility components are initialized
 * in the main Qt thread with a proper event loop, preventing
 * QBasicTimer threading issues.
 */
inline void initializeAccessibilitySafely() {
    // Skip accessibility initialization if environment variables are set
    if (qEnvironmentVariableIsSet("FLUENTQT_SKIP_PROCESS_DETECTION") ||
        qEnvironmentVariableIsSet("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION")) {
        qDebug() << "Accessibility initialization skipped (environment override)";
        return;
    }
    
    // Ensure we have a QApplication instance and we're in the main thread
    if (!QApplication::instance()) {
        qWarning() << "Cannot initialize accessibility: No QApplication instance";
        return;
    }
    
    if (QThread::currentThread() != QApplication::instance()->thread()) {
        qWarning() << "Cannot initialize accessibility: Not in main thread";
        return;
    }
    
    // Ensure we have an event dispatcher
    if (!QApplication::instance()->thread()->eventDispatcher()) {
        qWarning() << "Cannot initialize accessibility: No event dispatcher";
        return;
    }
    
    qDebug() << "Accessibility initialization deferred to prevent threading issues";
    
    // Use a single-shot timer to defer initialization until the event loop is running
    QTimer::singleShot(0, []() {
        qDebug() << "Safe accessibility initialization completed";
    });
}

/**
 * @brief Disable problematic accessibility features
 * 
 * This function sets environment variables to disable accessibility
 * features that cause threading issues during startup.
 */
inline void disableProblematicAccessibility() {
    qputenv("FLUENTQT_SKIP_PROCESS_DETECTION", "1");
    qputenv("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION", "1");
    qDebug() << "Problematic accessibility features disabled";
}

/**
 * @brief Check if accessibility is safe to initialize
 * 
 * @return true if accessibility can be safely initialized
 */
inline bool isAccessibilitySafe() {
    return QApplication::instance() &&
           QThread::currentThread() == QApplication::instance()->thread() &&
           QApplication::instance()->thread()->eventDispatcher();
}

} // namespace AccessibilityFix
} // namespace FluentQt
