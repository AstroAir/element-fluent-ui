// include/FluentQt/Styling/FluentCarouselStyles.h
#pragma once

#include <QColor>
#include <QIcon>
#include <QObject>
#include <QPixmap>
#include <QRect>
#include <QSize>
#include <QVariant>
#include <string>
#include <unordered_map>
#include "FluentQt/Core/FluentState.h"

class QPainter;
class QWidget;

namespace FluentQt::Styling {

/**
 * @brief Centralized styling system for FluentCarousel components
 *
 * This class provides a comprehensive styling system for all carousel variants,
 * following Fluent Design System principles and integrating with FluentTheme.
 * It handles visual styling for base carousel, navigation buttons, indicators,
 * progress bars, touch feedback, and transitions.
 */
class FluentCarouselStyles : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     */
    static FluentCarouselStyles& instance();

    // Style getters
    QVariant getBaseStyle(const std::string& key) const;
    QVariant getNavigationStyle(const std::string& key) const;
    QVariant getIndicatorStyle(const std::string& key) const;
    QVariant getProgressStyle(const std::string& key) const;
    QVariant getTouchStyle(const std::string& key) const;
    QVariant getTransitionStyle(const std::string& key) const;

    // Base carousel styling
    void paintCarouselBackground(
        QPainter& painter, const QRect& rect,
        Core::FluentState state = Core::FluentState::Normal) const;
    void applyCarouselElevation(
        QWidget* widget,
        Core::FluentState state = Core::FluentState::Normal) const;

    // Navigation button styling
    void paintNavigationButton(QPainter& painter, const QRect& rect,
                               Core::FluentState state,
                               const QIcon& icon) const;
    void applyNavigationButtonElevation(QWidget* button,
                                        Core::FluentState state) const;

    // Indicator styling
    void paintDotIndicator(
        QPainter& painter, const QRect& rect, bool active,
        Core::FluentState state = Core::FluentState::Normal) const;
    void paintLineIndicator(
        QPainter& painter, const QRect& rect, bool active,
        Core::FluentState state = Core::FluentState::Normal) const;
    void paintNumberIndicator(
        QPainter& painter, const QRect& rect, int number, bool active,
        Core::FluentState state = Core::FluentState::Normal) const;
    void paintThumbnailIndicator(
        QPainter& painter, const QRect& rect, const QPixmap& thumbnail,
        bool active, Core::FluentState state = Core::FluentState::Normal) const;
    void paintProgressIndicator(
        QPainter& painter, const QRect& rect, qreal progress, bool active,
        Core::FluentState state = Core::FluentState::Normal) const;

    // Progress bar styling
    void paintProgressBar(
        QPainter& painter, const QRect& rect, qreal progress,
        Core::FluentState state = Core::FluentState::Normal) const;
    void paintCircularProgress(
        QPainter& painter, const QRect& rect, qreal progress,
        Core::FluentState state = Core::FluentState::Normal) const;

    // Touch feedback styling
    void paintRippleEffect(QPainter& painter, const QPoint& center,
                           qreal radius, qreal opacity = 1.0) const;
    void paintDragIndicator(QPainter& painter, const QRect& rect,
                            qreal offset) const;
    void paintEdgeGlow(QPainter& painter, const QRect& rect, bool leftEdge,
                       bool rightEdge) const;

    // Transition effects
    void paintSlideTransition(QPainter& painter, const QRect& rect,
                              qreal progress, bool horizontal = true) const;
    void paintFadeTransition(QPainter& painter, const QRect& rect,
                             qreal opacity) const;
    void paintScaleTransition(QPainter& painter, const QRect& rect,
                              qreal scale) const;

    // Accessibility styling
    void paintFocusRing(QPainter& painter, const QRect& rect,
                        int borderRadius = 4) const;
    void paintHighContrastBorder(QPainter& painter, const QRect& rect,
                                 int borderRadius = 4) const;

    // Animation curves and timing
    QString getEasingCurve(const std::string& transitionType) const;
    int getAnimationDuration(const std::string& durationType) const;

    // Responsive design helpers
    int getResponsiveSize(const std::string& sizeKey, int screenWidth) const;
    QSize getResponsiveIconSize(int screenWidth) const;
    int getResponsiveSpacing(int screenWidth) const;

    // Theme integration
    void updateFromTheme();
    bool isDarkMode() const;
    bool isHighContrast() const;
    bool isReducedMotion() const;

signals:
    /**
     * @brief Emitted when styles are updated due to theme changes
     */
    void stylesChanged();

private slots:
    void onThemeChanged();

private:
    FluentCarouselStyles();
    void initializeStyles();
    void connectToTheme();

    // Style maps
    std::unordered_map<std::string, QVariant> m_baseStyles;
    std::unordered_map<std::string, QVariant> m_navigationStyles;
    std::unordered_map<std::string, QVariant> m_indicatorStyles;
    std::unordered_map<std::string, QVariant> m_progressStyles;
    std::unordered_map<std::string, QVariant> m_touchStyles;
    std::unordered_map<std::string, QVariant> m_transitionStyles;

    // Responsive breakpoints
    struct ResponsiveBreakpoints {
        int mobile = 480;
        int tablet = 768;
        int desktop = 1024;
        int large = 1440;
    } m_breakpoints;
};

// Inline implementations for frequently used getters
inline QVariant FluentCarouselStyles::getBaseStyle(
    const std::string& key) const {
    auto it = m_baseStyles.find(key);
    return (it != m_baseStyles.end()) ? it->second : QVariant();
}

inline QVariant FluentCarouselStyles::getNavigationStyle(
    const std::string& key) const {
    auto it = m_navigationStyles.find(key);
    return (it != m_navigationStyles.end()) ? it->second : QVariant();
}

inline QVariant FluentCarouselStyles::getIndicatorStyle(
    const std::string& key) const {
    auto it = m_indicatorStyles.find(key);
    return (it != m_indicatorStyles.end()) ? it->second : QVariant();
}

inline QVariant FluentCarouselStyles::getProgressStyle(
    const std::string& key) const {
    auto it = m_progressStyles.find(key);
    return (it != m_progressStyles.end()) ? it->second : QVariant();
}

inline QVariant FluentCarouselStyles::getTouchStyle(
    const std::string& key) const {
    auto it = m_touchStyles.find(key);
    return (it != m_touchStyles.end()) ? it->second : QVariant();
}

inline QVariant FluentCarouselStyles::getTransitionStyle(
    const std::string& key) const {
    auto it = m_transitionStyles.find(key);
    return (it != m_transitionStyles.end()) ? it->second : QVariant();
}

}  // namespace FluentQt::Styling
