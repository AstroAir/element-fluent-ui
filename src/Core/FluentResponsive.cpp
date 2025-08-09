// src/Core/FluentResponsive.cpp
#include "FluentQt/Core/FluentResponsive.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QMutexLocker>
#include <QTimer>

namespace FluentQt::Core {

FluentResponsiveManager& FluentResponsiveManager::instance() {
    static FluentResponsiveManager instance;
    return instance;
}

FluentResponsiveManager::FluentResponsiveManager() {
    // Initialize timers
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(100); // 100ms debounce
    connect(m_updateTimer, &QTimer::timeout, this, &FluentResponsiveManager::updateResponsiveState);
    
    m_containerUpdateTimer = new QTimer(this);
    m_containerUpdateTimer->setSingleShot(true);
    m_containerUpdateTimer->setInterval(50); // 50ms debounce for container queries
    
    // Initialize default configuration
    m_config.enableFluidLayouts = true;
    m_config.enableScalableText = true;
    m_config.enableFluidTypography = true;
    m_config.enableAccessibilityScaling = true;
    m_config.mode = FluentResponsiveMode::Hybrid;
    
    // Connect to screen changes (using QGuiApplication for screen signals)
    if (auto* guiApp = qobject_cast<QGuiApplication*>(QApplication::instance())) {
        connect(guiApp, &QGuiApplication::screenAdded, this, [this](QScreen*) { onScreenChanged(); });
        connect(guiApp, &QGuiApplication::screenRemoved, this, [this](QScreen*) { onScreenChanged(); });
    }
    
    // Initialize current state
    detectDeviceType();
    detectOrientation();
    updateBreakpoint();
    
    qDebug() << "FluentResponsiveManager initialized";
}

void FluentResponsiveManager::setResponsiveConfig(const ResponsiveConfig& config) {
    m_config = config;
}

FluentBreakpoint FluentResponsiveManager::getBreakpointForSize(const QSize& size) const {
    int width = size.width();

    if (width >= BREAKPOINT_XXL) return FluentBreakpoint::XXLarge;
    if (width >= BREAKPOINT_XL) return FluentBreakpoint::XLarge;
    if (width >= BREAKPOINT_LG) return FluentBreakpoint::Large;
    if (width >= BREAKPOINT_MD) return FluentBreakpoint::Medium;
    if (width >= BREAKPOINT_SM) return FluentBreakpoint::Small;
    return FluentBreakpoint::XSmall;
}

void FluentResponsiveManager::setScaleFactor(double factor) {
    if (m_scaleFactor != factor) {
        double oldFactor = m_scaleFactor;
        m_scaleFactor = factor;
        emit scaleFactorChanged(oldFactor, factor);
    }
}

void FluentResponsiveManager::registerResponsiveWidget(QWidget* widget) {
    if (!widget) return;

    QMutexLocker locker(&m_widgetsMutex);
    m_responsiveWidgets[widget] = ResponsiveProperties{};

    // Connect to widget destruction
    connect(widget, &QObject::destroyed, this, [this](QObject* obj) {
        QMutexLocker locker(&m_widgetsMutex);
        m_responsiveWidgets.erase(static_cast<QWidget*>(obj));
    });
}

// Private slot implementations
void FluentResponsiveManager::onScreenChanged() {
    detectDeviceType();
    updateBreakpoint();
    emit screenChanged(QApplication::primaryScreen());
}

void FluentResponsiveManager::onScreenResized() {
    updateBreakpoint();
    m_updateTimer->start();
}

void FluentResponsiveManager::onOrientationChanged() {
    detectOrientation();
}

void FluentResponsiveManager::updateResponsiveState() {
    // Update all registered responsive widgets
    QMutexLocker locker(&m_widgetsMutex);
    for (auto& pair : m_responsiveWidgets) {
        QWidget* widget = pair.first;
        const ResponsiveProperties& properties = pair.second;
        if (widget) {
            applyResponsiveProperties(widget, properties);
        }
    }
}

// Private helper methods
void FluentResponsiveManager::detectDeviceType() {
    // Simplified device type detection
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return;
    
    QSize screenSize = screen->size();
    double dpi = screen->logicalDotsPerInch();
    
    // Detect touch capability (simplified)
    m_isTouchDevice = false; // Would use platform-specific detection
    
    // Detect high DPI
    m_isHighDPI = dpi > 120;
    
    // Determine device type based on screen size
    FluentDeviceType newDeviceType;
    if (screenSize.width() < 768) {
        newDeviceType = FluentDeviceType::Mobile;
    } else if (screenSize.width() < 1024) {
        newDeviceType = FluentDeviceType::Tablet;
    } else {
        newDeviceType = FluentDeviceType::Desktop;
    }
    
    if (newDeviceType != m_deviceType) {
        FluentDeviceType oldDeviceType = m_deviceType;
        m_deviceType = newDeviceType;
        emit deviceTypeChanged(oldDeviceType, newDeviceType);
    }
}

void FluentResponsiveManager::detectOrientation() {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return;
    
    QSize screenSize = screen->size();
    FluentOrientation newOrientation = (screenSize.width() > screenSize.height()) 
                                      ? FluentOrientation::Landscape 
                                      : FluentOrientation::Portrait;
    
    if (newOrientation != m_orientation) {
        FluentOrientation oldOrientation = m_orientation;
        m_orientation = newOrientation;
        emit orientationChanged(oldOrientation, newOrientation);
    }
}

void FluentResponsiveManager::updateBreakpoint() {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return;

    QSize screenSize = screen->size();
    FluentBreakpoint newBreakpoint = getBreakpointForSize(screenSize);
    if (newBreakpoint != m_currentBreakpoint) {
        FluentBreakpoint oldBreakpoint = m_currentBreakpoint;
        m_currentBreakpoint = newBreakpoint;
        emit breakpointChanged(oldBreakpoint, newBreakpoint);
    }
}

void FluentResponsiveManager::applyResponsiveProperties(QWidget* widget, const ResponsiveProperties& properties) {
    if (!widget) return;
    
    // Apply size for current breakpoint
    auto sizeIt = properties.sizes.find(m_currentBreakpoint);
    if (sizeIt != properties.sizes.end()) {
        widget->resize(sizeIt->second);
    }
    
    // Apply visibility for current breakpoint
    auto visibilityIt = properties.visibility.find(m_currentBreakpoint);
    if (visibilityIt != properties.visibility.end()) {
        widget->setVisible(visibilityIt->second);
    }
}

} // namespace FluentQt::Core
