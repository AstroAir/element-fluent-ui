// src/Accessibility/FluentScreenReaderManager.cpp
#include "FluentQt/Accessibility/FluentScreenReaderManager.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QProcess>
#include <QThread>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace FluentQt::Accessibility {

FluentScreenReaderManager& FluentScreenReaderManager::instance() {
    static FluentScreenReaderManager instance;
    return instance;
}

FluentScreenReaderManager::FluentScreenReaderManager() {
    // Initialize announcement timer only if we have a proper event loop
    if (QApplication::instance() &&
        QThread::currentThread() == QApplication::instance()->thread()) {
        m_announcementTimer = new QTimer(this);
        m_announcementTimer->setSingleShot(true);
        connect(m_announcementTimer, &QTimer::timeout, this,
                &FluentScreenReaderManager::processAnnouncementQueue);
    } else {
        // Defer timer creation until we're in the main thread
        QTimer::singleShot(0, this, [this]() {
            if (!m_announcementTimer) {
                m_announcementTimer = new QTimer(this);
                m_announcementTimer->setSingleShot(true);
                connect(m_announcementTimer, &QTimer::timeout, this,
                        &FluentScreenReaderManager::processAnnouncementQueue);
            }
        });
    }

    // Defer screen reader detection to avoid blocking UI thread during startup
    // Check for skip environment variables
    if (qEnvironmentVariableIsSet("FLUENTQT_SKIP_PROCESS_DETECTION") ||
        qEnvironmentVariableIsSet("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION")) {
        qDebug() << "Skipping screen reader detection (env override)";
    } else {
        QTimer::singleShot(300, this, [this]() { detectScreenReader(); });
    }

    qDebug() << "FluentScreenReaderManager initialized";
}

FluentScreenReaderInfo FluentScreenReaderManager::detectScreenReader() {
    FluentScreenReaderInfo info;

#ifdef Q_OS_WIN
    info.type = detectWindowsScreenReader();
#elif defined(Q_OS_MACOS)
    info.type = detectMacScreenReader();
#elif defined(Q_OS_LINUX)
    info.type = detectLinuxScreenReader();
#endif

    switch (info.type) {
        case FluentScreenReaderType::NVDA:
            info.name = "NVDA";
            info.supportsAria = true;
            info.supportsLiveRegions = true;
            info.supportsLandmarks = true;
            break;
        case FluentScreenReaderType::JAWS:
            info.name = "JAWS";
            info.supportsAria = true;
            info.supportsLiveRegions = true;
            info.supportsLandmarks = true;
            break;
        case FluentScreenReaderType::WindowsNarrator:
            info.name = "Windows Narrator";
            info.supportsAria = true;
            info.supportsLiveRegions = true;
            info.supportsLandmarks = true;
            break;
        case FluentScreenReaderType::VoiceOver:
            info.name = "VoiceOver";
            info.supportsAria = true;
            info.supportsLiveRegions = true;
            info.supportsLandmarks = true;
            break;
        case FluentScreenReaderType::Orca:
            info.name = "Orca";
            info.supportsAria = true;
            info.supportsLiveRegions = true;
            info.supportsLandmarks = true;
            break;
        default:
            info.name = "Unknown";
            info.supportsAria = false;
            info.supportsLiveRegions = false;
            info.supportsLandmarks = false;
            break;
    }

    m_activeScreenReader = info.type;
    m_screenReaderInfo = info;
    m_screenReaderActive = (info.type != FluentScreenReaderType::Unknown);

    if (m_screenReaderActive) {
        emit screenReaderDetected(info.type, info.name);
        qDebug() << "Detected screen reader:" << info.name;
    }

    return info;
}

void FluentScreenReaderManager::setScreenReaderActive(bool active) {
    if (m_screenReaderActive != active) {
        m_screenReaderActive = active;
        emit screenReaderActivated(active);

        if (active) {
            announce("Screen reader support activated",
                     FluentAnnouncementPriority::High);
        }
    }
}

void FluentScreenReaderManager::announce(const QString& message,
                                         FluentAnnouncementPriority priority,
                                         FluentLiveRegionType liveType) {
    if (!m_screenReaderActive || message.isEmpty()) {
        return;
    }

    FluentAnnouncement announcement;
    announcement.message = message;
    announcement.priority = priority;
    announcement.liveType = liveType;
    announcement.timestamp = std::chrono::steady_clock::now();
    announcement.interrupt =
        (priority == FluentAnnouncementPriority::Interrupt);

    queueAnnouncement(announcement);
}

void FluentScreenReaderManager::announceFromWidget(
    QWidget* widget, const QString& message,
    FluentAnnouncementPriority priority) {
    if (!widget) {
        announce(message, priority);
        return;
    }

    FluentAnnouncement announcement;
    announcement.message = message;
    announcement.priority = priority;
    announcement.source = widget;
    announcement.timestamp = std::chrono::steady_clock::now();
    announcement.context = widget->objectName();

    queueAnnouncement(announcement);
}

void FluentScreenReaderManager::interruptAndAnnounce(const QString& message) {
    clearAnnouncementQueue();
    announce(message, FluentAnnouncementPriority::Interrupt);
}

void FluentScreenReaderManager::clearAnnouncementQueue() {
    QMutexLocker locker(&m_mutex);
    m_announcementQueue.clear();
}

void FluentScreenReaderManager::setAriaRole(QWidget* widget,
                                            FluentAriaRole role) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);
    m_ariaRoles[widget] = role;

    // Update accessible interface
    updateAccessibleInterface(widget);

    emit ariaPropertyChanged(widget, "role", ariaRoleToString(role));
}

void FluentScreenReaderManager::setAriaProperties(
    QWidget* widget, const FluentAriaProperties& properties) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);
    m_ariaProperties[widget] = properties;

    // Update accessible interface
    updateAccessibleInterface(widget);
}

void FluentScreenReaderManager::setAriaLabel(QWidget* widget,
                                             const QString& label) {
    if (!widget)
        return;

    widget->setAccessibleName(label);

    QMutexLocker locker(&m_mutex);
    auto& props = m_ariaProperties[widget];
    props.label = label;

    emit ariaPropertyChanged(widget, "aria-label", label);
}

void FluentScreenReaderManager::setAriaDescription(QWidget* widget,
                                                   const QString& description) {
    if (!widget)
        return;

    widget->setAccessibleDescription(description);

    QMutexLocker locker(&m_mutex);
    auto& props = m_ariaProperties[widget];
    props.describedBy = description;

    emit ariaPropertyChanged(widget, "aria-describedby", description);
}

void FluentScreenReaderManager::setLiveRegion(QWidget* widget,
                                              FluentLiveRegionType type,
                                              bool atomic) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);
    m_liveRegions[widget] = type;

    auto& props = m_ariaProperties[widget];
    props.live = type;
    props.atomic = atomic;

    emit ariaPropertyChanged(widget, "aria-live",
                             type == FluentLiveRegionType::Polite ? "polite"
                             : type == FluentLiveRegionType::Assertive
                                 ? "assertive"
                                 : "off");
}

void FluentScreenReaderManager::updateLiveRegion(QWidget* widget,
                                                 const QString& content) {
    if (!widget)
        return;

    auto it = m_liveRegions.find(widget);
    if (it != m_liveRegions.end() && it->second != FluentLiveRegionType::Off) {
        announce(content, FluentAnnouncementPriority::Medium, it->second);
        emit liveRegionUpdated(widget, content);
    }
}

void FluentScreenReaderManager::setLandmark(QWidget* widget,
                                            FluentAriaRole landmarkRole,
                                            const QString& label) {
    if (!widget)
        return;

    setAriaRole(widget, landmarkRole);
    if (!label.isEmpty()) {
        setAriaLabel(widget, label);
    }

    QMutexLocker locker(&m_mutex);
    if (!m_landmarks.contains(widget)) {
        m_landmarks.append(widget);
    }

    emit landmarkAdded(widget, landmarkRole, label);
}

void FluentScreenReaderManager::removeLandmark(QWidget* widget) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);
    m_landmarks.removeAll(widget);
}

QList<QWidget*> FluentScreenReaderManager::getLandmarks() const {
    QMutexLocker locker(&m_mutex);
    return m_landmarks;
}

void FluentScreenReaderManager::setHeading(QWidget* widget, int level,
                                           const QString& text) {
    if (!widget)
        return;

    setAriaRole(widget, FluentAriaRole::Heading);
    setAriaLabel(widget, text);

    QMutexLocker locker(&m_mutex);
    auto& props = m_ariaProperties[widget];
    props.level = level;

    if (!m_headings.contains(widget)) {
        m_headings.append(widget);
    }

    emit ariaPropertyChanged(widget, "aria-level", QString::number(level));
}

QList<QWidget*> FluentScreenReaderManager::getHeadings() const {
    QMutexLocker locker(&m_mutex);
    return m_headings;
}

void FluentScreenReaderManager::announceStatus(const QString& status,
                                               bool polite) {
    FluentLiveRegionType type =
        polite ? FluentLiveRegionType::Polite : FluentLiveRegionType::Assertive;
    announce(status, FluentAnnouncementPriority::Medium, type);
}

void FluentScreenReaderManager::announceAlert(const QString& alert) {
    announce(alert, FluentAnnouncementPriority::High,
             FluentLiveRegionType::Assertive);
}

void FluentScreenReaderManager::announceError(const QString& error,
                                              QWidget* source) {
    QString message = "Error: " + error;
    if (source) {
        announceFromWidget(source, message, FluentAnnouncementPriority::High);
    } else {
        announce(message, FluentAnnouncementPriority::High,
                 FluentLiveRegionType::Assertive);
    }
}

void FluentScreenReaderManager::announceSuccess(const QString& success) {
    announce("Success: " + success, FluentAnnouncementPriority::Medium,
             FluentLiveRegionType::Polite);
}

void FluentScreenReaderManager::announceProgress(int percentage,
                                                 const QString& description) {
    QString message = QString("Progress: %1%").arg(percentage);
    if (!description.isEmpty()) {
        message += " - " + description;
    }
    announce(message, FluentAnnouncementPriority::Low,
             FluentLiveRegionType::Polite);
}

// Private implementation methods
FluentScreenReaderType FluentScreenReaderManager::detectWindowsScreenReader() {
#ifdef Q_OS_WIN
    if (isProcessRunning("nvda.exe")) {
        return FluentScreenReaderType::NVDA;
    }
    if (isProcessRunning("jfw.exe")) {
        return FluentScreenReaderType::JAWS;
    }

    // Check for Windows Narrator
    BOOL isNarratorRunning = FALSE;
    SystemParametersInfo(SPI_GETSCREENREADER, 0, &isNarratorRunning, 0);
    if (isNarratorRunning) {
        return FluentScreenReaderType::WindowsNarrator;
    }
#endif

    return FluentScreenReaderType::Unknown;
}

FluentScreenReaderType FluentScreenReaderManager::detectMacScreenReader() {
#ifdef Q_OS_MACOS
    // VoiceOver detection would go here
    // This is a simplified implementation
    return FluentScreenReaderType::VoiceOver;
#endif

    return FluentScreenReaderType::Unknown;
}

FluentScreenReaderType FluentScreenReaderManager::detectLinuxScreenReader() {
#ifdef Q_OS_LINUX
    if (isProcessRunning("orca")) {
        return FluentScreenReaderType::Orca;
    }
#endif

    return FluentScreenReaderType::Unknown;
}

bool FluentScreenReaderManager::isProcessRunning(const QString& processName) {
    // Allow tests or headless environments to skip process spawning entirely
    if (qEnvironmentVariableIsSet("FLUENTQT_SKIP_PROCESS_DETECTION")) {
        qDebug() << "Skipping process detection for" << processName
                 << "due to FLUENTQT_SKIP_PROCESS_DETECTION";
        return false;
    }
    // Check if we're in offscreen mode - if so, skip process detection to avoid
    // hanging
    if (QApplication::instance()) {
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

        if (platformName == "offscreen" || platformName == "minimal") {
            qDebug() << "Skipping process detection for" << processName << "in"
                     << platformName << "mode";
            return false;
        }
    }

    QProcess process;

#ifdef Q_OS_WIN
    process.start("tasklist",
                  QStringList()
                      << "/FI" << QString("IMAGENAME eq %1").arg(processName));
#else
    process.start("pgrep", QStringList() << processName);
#endif

    // Use shorter timeout to prevent hanging, especially in headless
    // environments
    bool finished =
        process.waitForFinished(1000);  // Reduced from 3000ms to 1000ms
    if (!finished) {
        qDebug() << "Process detection timed out for" << processName;
        process.kill();
        process.waitForFinished(500);
        return false;
    }

    QString output = process.readAllStandardOutput();

    return output.contains(processName, Qt::CaseInsensitive);
}

void FluentScreenReaderManager::queueAnnouncement(
    const FluentAnnouncement& announcement) {
    QMutexLocker locker(&m_mutex);

    if (announcement.interrupt) {
        m_announcementQueue.clear();
    }

    m_announcementQueue.enqueue(announcement);
    emit announcementQueued(announcement);

    if (!m_announcementTimer->isActive()) {
        m_announcementTimer->start(m_announcementDelay);
    }
}

void FluentScreenReaderManager::processAnnouncementQueue() {
    QMutexLocker locker(&m_mutex);

    if (m_announcementQueue.isEmpty()) {
        return;
    }

    FluentAnnouncement announcement = m_announcementQueue.dequeue();
    locker.unlock();

    deliverAnnouncement(announcement);

    // Schedule next announcement if queue is not empty
    locker.relock();
    if (!m_announcementQueue.isEmpty()) {
        m_announcementTimer->start(m_announcementDelay);
    }
}

void FluentScreenReaderManager::deliverAnnouncement(
    const FluentAnnouncement& announcement) {
    if (!shouldAnnounce(announcement)) {
        return;
    }

    QString formattedMessage = formatAnnouncementForScreenReader(
        announcement.message, m_activeScreenReader);

    // In a real implementation, this would use platform-specific APIs
    // to send the announcement to the screen reader
    qDebug() << "Screen reader announcement:" << formattedMessage;

    emit announcementDelivered(announcement);
    m_lastAnnouncement = std::chrono::steady_clock::now();
    m_announcementCount++;
}

QString FluentScreenReaderManager::formatAnnouncementForScreenReader(
    const QString& message, FluentScreenReaderType type) {
    QString formatted = message;

    // Limit message length
    if (formatted.length() > m_maxAnnouncementLength) {
        formatted = formatted.left(m_maxAnnouncementLength - 3) + "...";
    }

    // Screen reader specific formatting
    switch (type) {
        case FluentScreenReaderType::NVDA:
        case FluentScreenReaderType::JAWS:
            // These screen readers handle punctuation well
            break;
        case FluentScreenReaderType::WindowsNarrator:
            // Narrator might need different formatting
            break;
        default:
            break;
    }

    return formatted;
}

bool FluentScreenReaderManager::shouldAnnounce(
    const FluentAnnouncement& announcement) {
    // Check if enough time has passed since last announcement
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastAnnouncement =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_lastAnnouncement);

    if (timeSinceLastAnnouncement.count() < m_announcementDelay &&
        announcement.priority != FluentAnnouncementPriority::Interrupt) {
        return false;
    }

    return true;
}

QString FluentScreenReaderManager::ariaRoleToString(FluentAriaRole role) {
    switch (role) {
        case FluentAriaRole::Button:
            return "button";
        case FluentAriaRole::Link:
            return "link";
        case FluentAriaRole::Heading:
            return "heading";
        case FluentAriaRole::List:
            return "list";
        case FluentAriaRole::ListItem:
            return "listitem";
        case FluentAriaRole::Main:
            return "main";
        case FluentAriaRole::Navigation:
            return "navigation";
        case FluentAriaRole::Banner:
            return "banner";
        case FluentAriaRole::ContentInfo:
            return "contentinfo";
        case FluentAriaRole::Form:
            return "form";
        case FluentAriaRole::Search:
            return "search";
        case FluentAriaRole::Alert:
            return "alert";
        case FluentAriaRole::Dialog:
            return "dialog";
        case FluentAriaRole::Status:
            return "status";
        default:
            return "generic";
    }
}

void FluentScreenReaderManager::updateAccessibleInterface(QWidget* widget) {
    if (!widget)
        return;

    // Update Qt's accessibility interface
    // This would typically involve updating QAccessibleInterface
    widget->update();
}

void FluentScreenReaderManager::onWidgetDestroyed(QObject* widget) {
    QWidget* w = qobject_cast<QWidget*>(widget);
    if (!w)
        return;

    QMutexLocker locker(&m_mutex);
    m_ariaProperties.erase(w);
    m_ariaRoles.erase(w);
    m_liveRegions.erase(w);
    m_landmarks.removeAll(w);
    m_headings.removeAll(w);
}

void FluentScreenReaderManager::onSystemAccessibilityChanged() {
    // Check for skip environment variables
    if (qEnvironmentVariableIsSet("FLUENTQT_SKIP_PROCESS_DETECTION") ||
        qEnvironmentVariableIsSet("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION")) {
        qDebug() << "Skipping screen reader re-detection (env override)";
        return;
    }

    // Re-detect screen reader when system accessibility settings change (async)
    QTimer::singleShot(0, this, [this]() { detectScreenReader(); });

    // Announce the change if a screen reader is active
    if (m_screenReaderActive) {
        announce("Accessibility settings changed",
                 FluentAnnouncementPriority::Medium);
    }
}

}  // namespace FluentQt::Accessibility
