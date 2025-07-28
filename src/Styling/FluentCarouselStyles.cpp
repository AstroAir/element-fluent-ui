// src/Styling/FluentCarouselStyles.cpp
#include "FluentQt/Styling/FluentCarouselStyles.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QLinearGradient>
#include <QPixmap>
#include <algorithm>

namespace FluentQt::Styling {

FluentCarouselStyles::FluentCarouselStyles() {
    initializeStyles();
    connectToTheme();
}

FluentCarouselStyles& FluentCarouselStyles::instance() {
    static FluentCarouselStyles instance;
    return instance;
}

void FluentCarouselStyles::initializeStyles() {
    auto& theme = FluentTheme::instance();
    
    // Initialize base carousel styles
    m_baseStyles = {
        {"background", theme.color("surface")},
        {"border", theme.color("neutralQuaternary")},
        {"borderRadius", theme.borderRadius("medium")},
        {"padding", theme.spacing("m")},
        {"elevation", theme.elevation("card")},
        {"focusColor", theme.color("accent")},
        {"focusWidth", 2}
    };
    
    // Initialize navigation button styles
    m_navigationStyles = {
        {"background", theme.color("neutralLighter")},
        {"backgroundHover", theme.color("neutralLight")},
        {"backgroundPressed", theme.color("neutralTertiaryAlt")},
        {"backgroundDisabled", theme.color("neutralQuaternary")},
        {"foreground", theme.color("neutralPrimary")},
        {"foregroundHover", theme.color("neutralDark")},
        {"foregroundPressed", theme.color("neutralDark")},
        {"foregroundDisabled", theme.color("neutralTertiary")},
        {"borderRadius", theme.borderRadius("small")},
        {"size", 32},
        {"iconSize", theme.iconSize("medium")},
        {"elevation", theme.elevation("button")},
        {"animationDuration", 150}
    };
    
    // Initialize indicator styles
    m_indicatorStyles = {
        {"activeColor", theme.color("accent")},
        {"inactiveColor", theme.color("neutralTertiary")},
        {"hoverColor", theme.color("accentLight")},
        {"pressedColor", theme.color("accentDark")},
        {"disabledColor", theme.color("neutralQuaternary")},
        {"dotSize", 8},
        {"lineWidth", 24},
        {"lineHeight", 4},
        {"numberSize", 24},
        {"thumbnailSize", QSize(48, 32)},
        {"spacing", theme.spacing("m")},
        {"borderRadius", theme.borderRadius("small")},
        {"animationDuration", 200}
    };
    
    // Initialize progress styles
    m_progressStyles = {
        {"backgroundColor", theme.color("neutralLighter")},
        {"foregroundColor", theme.color("accent")},
        {"height", 4},
        {"borderRadius", 2},
        {"animationDuration", 300}
    };
    
    // Initialize touch feedback styles
    m_touchStyles = {
        {"rippleColor", theme.color("accent")},
        {"rippleOpacity", 0.3},
        {"rippleSize", 40},
        {"rippleDuration", 300},
        {"dragIndicatorColor", theme.color("accent")},
        {"dragIndicatorOpacity", 0.1},
        {"edgeGlowColor", theme.color("accent")},
        {"edgeGlowOpacity", 0.5}
    };
    
    // Initialize transition styles
    m_transitionStyles = {
        {"slideEasing", "cubic-bezier(0.25, 0.46, 0.45, 0.94)"},
        {"fadeEasing", "cubic-bezier(0.25, 0.46, 0.45, 0.94)"},
        {"scaleEasing", "cubic-bezier(0.68, -0.55, 0.265, 1.55)"},
        {"flipEasing", "cubic-bezier(0.175, 0.885, 0.32, 1.275)"},
        {"cubeEasing", "cubic-bezier(0.645, 0.045, 0.355, 1)"},
        {"coverflowEasing", "cubic-bezier(0.23, 1, 0.32, 1)"},
        {"defaultDuration", 300},
        {"fastDuration", 150},
        {"slowDuration", 500}
    };
}

void FluentCarouselStyles::connectToTheme() {
    auto& theme = FluentTheme::instance();
    connect(&theme, &FluentTheme::themeChanged,
            this, &FluentCarouselStyles::onThemeChanged);
}

void FluentCarouselStyles::onThemeChanged() {
    initializeStyles();
    emit stylesChanged();
}

// Base carousel styling
void FluentCarouselStyles::paintCarouselBackground(QPainter& painter, const QRect& rect, 
                                                  Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Get colors based on state
    QColor backgroundColor = getBaseStyle("background").value<QColor>();
    QColor borderColor = getBaseStyle("border").value<QColor>();
    int borderRadius = getBaseStyle("borderRadius").toInt();
    
    // Adjust colors for state
    switch (state) {
    case Core::FluentState::Hovered:
        backgroundColor = backgroundColor.lighter(105);
        break;
    case Core::FluentState::Pressed:
        backgroundColor = backgroundColor.darker(105);
        break;
    case Core::FluentState::Disabled:
        backgroundColor = backgroundColor.lighter(110);
        borderColor = borderColor.lighter(120);
        break;
    default:
        break;
    }
    
    // Paint background
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, borderRadius, borderRadius);
    
    // Paint border
    if (state == Core::FluentState::Focused) {
        QColor focusColor = getBaseStyle("focusColor").value<QColor>();
        int focusWidth = getBaseStyle("focusWidth").toInt();
        
        QPen focusPen(focusColor, focusWidth);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), borderRadius, borderRadius);
    } else {
        QPen borderPen(borderColor, 1);
        painter.setPen(borderPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect, borderRadius, borderRadius);
    }
}

void FluentCarouselStyles::applyCarouselElevation(QWidget* widget, Core::FluentState state) const {
    if (!widget) return;
    
    // Remove existing shadow effect
    if (auto* effect = widget->graphicsEffect()) {
        if (qobject_cast<QGraphicsDropShadowEffect*>(effect)) {
            widget->setGraphicsEffect(nullptr);
        }
    }
    
    // Apply elevation based on state
    int elevation = getBaseStyle("elevation").toInt();
    if (state == Core::FluentState::Hovered) {
        elevation += 2;
    } else if (state == Core::FluentState::Pressed) {
        elevation = std::max(0, elevation - 1);
    } else if (state == Core::FluentState::Disabled) {
        elevation = 0;
    }
    
    if (elevation > 0) {
        auto* shadowEffect = new QGraphicsDropShadowEffect(widget);
        shadowEffect->setBlurRadius(elevation);
        shadowEffect->setOffset(0, elevation / 2);
        shadowEffect->setColor(QColor(0, 0, 0, 40));
        widget->setGraphicsEffect(shadowEffect);
    }
}

// Navigation button styling
void FluentCarouselStyles::paintNavigationButton(QPainter& painter, const QRect& rect,
                                                Core::FluentState state, const QIcon& icon) const {
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Get colors based on state
    QColor backgroundColor, foregroundColor;
    
    switch (state) {
    case Core::FluentState::Normal:
        backgroundColor = getNavigationStyle("background").value<QColor>();
        foregroundColor = getNavigationStyle("foreground").value<QColor>();
        break;
    case Core::FluentState::Hovered:
        backgroundColor = getNavigationStyle("backgroundHover").value<QColor>();
        foregroundColor = getNavigationStyle("foregroundHover").value<QColor>();
        break;
    case Core::FluentState::Pressed:
        backgroundColor = getNavigationStyle("backgroundPressed").value<QColor>();
        foregroundColor = getNavigationStyle("foregroundPressed").value<QColor>();
        break;
    case Core::FluentState::Disabled:
        backgroundColor = getNavigationStyle("backgroundDisabled").value<QColor>();
        foregroundColor = getNavigationStyle("foregroundDisabled").value<QColor>();
        break;
    default:
        backgroundColor = getNavigationStyle("background").value<QColor>();
        foregroundColor = getNavigationStyle("foreground").value<QColor>();
        break;
    }
    
    int borderRadius = getNavigationStyle("borderRadius").toInt();
    QSize iconSize = getNavigationStyle("iconSize").value<QSize>();
    
    // Paint background
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, borderRadius, borderRadius);
    
    // Paint icon
    if (!icon.isNull()) {
        QRect iconRect = QRect(0, 0, iconSize.width(), iconSize.height());
        iconRect.moveCenter(rect.center());
        
        QPixmap pixmap = icon.pixmap(iconSize, 
                                   state == Core::FluentState::Disabled ? QIcon::Disabled : QIcon::Normal);
        
        // Apply color overlay for non-disabled states
        if (state != Core::FluentState::Disabled) {
            QPainter pixmapPainter(&pixmap);
            pixmapPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            pixmapPainter.fillRect(pixmap.rect(), foregroundColor);
        }
        
        painter.drawPixmap(iconRect, pixmap);
    }
    
    // Paint focus ring
    if (state == Core::FluentState::Focused) {
        auto& theme = FluentTheme::instance();
        QPen focusPen(theme.color("accent"), 2);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), borderRadius + 2, borderRadius + 2);
    }
}

// Indicator styling
void FluentCarouselStyles::paintDotIndicator(QPainter& painter, const QRect& rect,
                                           bool active, Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor color = active ? getIndicatorStyle("activeColor").value<QColor>() 
                          : getIndicatorStyle("inactiveColor").value<QColor>();
    
    // Adjust color for state
    switch (state) {
    case Core::FluentState::Hovered:
        color = active ? getIndicatorStyle("hoverColor").value<QColor>() : color.lighter(120);
        break;
    case Core::FluentState::Pressed:
        color = getIndicatorStyle("pressedColor").value<QColor>();
        break;
    case Core::FluentState::Disabled:
        color = getIndicatorStyle("disabledColor").value<QColor>();
        break;
    default:
        break;
    }
    
    // Paint dot
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    
    int dotSize = getIndicatorStyle("dotSize").toInt();
    QRect dotRect = QRect(0, 0, dotSize, dotSize);
    dotRect.moveCenter(rect.center());
    
    painter.drawEllipse(dotRect);
    
    // Paint focus ring
    if (state == Core::FluentState::Focused) {
        auto& theme = FluentTheme::instance();
        QPen focusPen(theme.color("accent"), 1);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(dotRect.adjusted(-2, -2, 2, 2));
    }
}

void FluentCarouselStyles::paintLineIndicator(QPainter& painter, const QRect& rect,
                                            bool active, Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor color = active ? getIndicatorStyle("activeColor").value<QColor>() 
                          : getIndicatorStyle("inactiveColor").value<QColor>();
    
    // Adjust color for state
    switch (state) {
    case Core::FluentState::Hovered:
        color = active ? getIndicatorStyle("hoverColor").value<QColor>() : color.lighter(120);
        break;
    case Core::FluentState::Pressed:
        color = getIndicatorStyle("pressedColor").value<QColor>();
        break;
    case Core::FluentState::Disabled:
        color = getIndicatorStyle("disabledColor").value<QColor>();
        break;
    default:
        break;
    }
    
    // Paint line
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    
    int borderRadius = getIndicatorStyle("borderRadius").toInt();
    painter.drawRoundedRect(rect, borderRadius, borderRadius);
    
    // Paint focus ring
    if (state == Core::FluentState::Focused) {
        auto& theme = FluentTheme::instance();
        QPen focusPen(theme.color("accent"), 1);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-1, -1, 1, 1), borderRadius, borderRadius);
    }
}

void FluentCarouselStyles::paintNumberIndicator(QPainter& painter, const QRect& rect,
                                              int number, bool active, Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
    QColor backgroundColor = active ? getIndicatorStyle("activeColor").value<QColor>() 
                                   : Qt::transparent;
    QColor textColor = active ? Qt::white : getIndicatorStyle("inactiveColor").value<QColor>();
    QColor borderColor = getIndicatorStyle("inactiveColor").value<QColor>();
    
    // Adjust colors for state
    switch (state) {
    case Core::FluentState::Hovered:
        if (active) {
            backgroundColor = getIndicatorStyle("hoverColor").value<QColor>();
        } else {
            borderColor = borderColor.lighter(120);
            textColor = textColor.lighter(120);
        }
        break;
    case Core::FluentState::Pressed:
        backgroundColor = getIndicatorStyle("pressedColor").value<QColor>();
        textColor = Qt::white;
        break;
    case Core::FluentState::Disabled:
        backgroundColor = getIndicatorStyle("disabledColor").value<QColor>();
        textColor = getIndicatorStyle("disabledColor").value<QColor>();
        borderColor = getIndicatorStyle("disabledColor").value<QColor>();
        break;
    default:
        break;
    }
    
    int borderRadius = getIndicatorStyle("borderRadius").toInt();
    
    // Paint background
    if (active) {
        painter.setBrush(backgroundColor);
        painter.setPen(Qt::NoPen);
    } else {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(borderColor, 1));
    }
    
    painter.drawRoundedRect(rect, borderRadius, borderRadius);
    
    // Paint number
    painter.setPen(textColor);
    QFont font = QApplication::font();
    font.setPointSize(10);
    font.setBold(active);
    painter.setFont(font);
    
    painter.drawText(rect, Qt::AlignCenter, QString::number(number));

    // Paint focus ring
    if (state == Core::FluentState::Focused) {
        auto& theme = FluentTheme::instance();
        QPen focusPen(theme.color("accent"), 2);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), borderRadius + 2, borderRadius + 2);
    }
}

void FluentCarouselStyles::paintThumbnailIndicator(QPainter& painter, const QRect& rect,
                                                 const QPixmap& thumbnail, bool active,
                                                 Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);

    QColor borderColor = active ? getIndicatorStyle("activeColor").value<QColor>()
                                : getIndicatorStyle("inactiveColor").value<QColor>();

    // Adjust color for state
    switch (state) {
    case Core::FluentState::Hovered:
        borderColor = active ? getIndicatorStyle("hoverColor").value<QColor>() : borderColor.lighter(120);
        break;
    case Core::FluentState::Pressed:
        borderColor = getIndicatorStyle("pressedColor").value<QColor>();
        break;
    case Core::FluentState::Disabled:
        borderColor = getIndicatorStyle("disabledColor").value<QColor>();
        break;
    default:
        break;
    }

    int borderRadius = getIndicatorStyle("borderRadius").toInt();

    // Paint thumbnail if available
    if (!thumbnail.isNull()) {
        QPixmap scaled = thumbnail.scaled(rect.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        // Create rounded pixmap
        QPixmap roundedPixmap(rect.size());
        roundedPixmap.fill(Qt::transparent);

        QPainter pixmapPainter(&roundedPixmap);
        pixmapPainter.setRenderHint(QPainter::Antialiasing);
        pixmapPainter.setBrush(QBrush(scaled));
        pixmapPainter.setPen(Qt::NoPen);
        pixmapPainter.drawRoundedRect(roundedPixmap.rect(), borderRadius, borderRadius);

        painter.drawPixmap(rect, roundedPixmap);
    } else {
        // Draw placeholder
        painter.setBrush(QColor(200, 200, 200));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect, borderRadius, borderRadius);

        // Draw placeholder icon or text
        painter.setPen(QColor(120, 120, 120));
        painter.drawText(rect, Qt::AlignCenter, "?");
    }

    // Paint border
    int borderWidth = active ? 3 : 1;
    QPen borderPen(borderColor, borderWidth);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, borderRadius, borderRadius);

    // Paint focus ring
    if (state == Core::FluentState::Focused) {
        auto& theme = FluentTheme::instance();
        QPen focusPen(theme.color("accent"), 2);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), borderRadius + 2, borderRadius + 2);
    }
}

void FluentCarouselStyles::paintProgressIndicator(QPainter& painter, const QRect& rect,
                                                qreal progress, bool active, Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);

    QColor backgroundColor = getIndicatorStyle("inactiveColor").value<QColor>().lighter(150);
    QColor foregroundColor = active ? getIndicatorStyle("activeColor").value<QColor>()
                                   : getIndicatorStyle("inactiveColor").value<QColor>();

    // Adjust colors for state
    switch (state) {
    case Core::FluentState::Hovered:
        foregroundColor = active ? getIndicatorStyle("hoverColor").value<QColor>() : foregroundColor.lighter(120);
        break;
    case Core::FluentState::Pressed:
        foregroundColor = getIndicatorStyle("pressedColor").value<QColor>();
        break;
    case Core::FluentState::Disabled:
        foregroundColor = getIndicatorStyle("disabledColor").value<QColor>();
        backgroundColor = backgroundColor.lighter(120);
        break;
    default:
        break;
    }

    int borderRadius = getIndicatorStyle("borderRadius").toInt();

    // Paint background
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, borderRadius, borderRadius);

    // Paint progress
    if (progress > 0.0) {
        QRect progressRect = rect;
        progressRect.setWidth(static_cast<int>(rect.width() * std::clamp(progress, 0.0, 1.0)));

        painter.setBrush(foregroundColor);
        painter.drawRoundedRect(progressRect, borderRadius, borderRadius);
    }

    // Paint focus ring
    if (state == Core::FluentState::Focused) {
        auto& theme = FluentTheme::instance();
        QPen focusPen(theme.color("accent"), 1);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-1, -1, 1, 1), borderRadius, borderRadius);
    }
}

// Progress bar styling
void FluentCarouselStyles::paintProgressBar(QPainter& painter, const QRect& rect,
                                          qreal progress, Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);

    QColor backgroundColor = getProgressStyle("backgroundColor").value<QColor>();
    QColor foregroundColor = getProgressStyle("foregroundColor").value<QColor>();

    // Adjust colors for state
    switch (state) {
    case Core::FluentState::Hovered:
        foregroundColor = foregroundColor.lighter(110);
        break;
    case Core::FluentState::Disabled:
        foregroundColor = foregroundColor.lighter(150);
        backgroundColor = backgroundColor.lighter(120);
        break;
    default:
        break;
    }

    int borderRadius = getProgressStyle("borderRadius").toInt();

    // Paint background
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, borderRadius, borderRadius);

    // Paint progress
    if (progress > 0.0) {
        QRect progressRect = rect;
        progressRect.setWidth(static_cast<int>(rect.width() * std::clamp(progress, 0.0, 1.0)));

        painter.setBrush(foregroundColor);
        painter.drawRoundedRect(progressRect, borderRadius, borderRadius);
    }
}

void FluentCarouselStyles::paintCircularProgress(QPainter& painter, const QRect& rect,
                                               qreal progress, Core::FluentState state) const {
    painter.setRenderHint(QPainter::Antialiasing);

    QColor backgroundColor = getProgressStyle("backgroundColor").value<QColor>();
    QColor foregroundColor = getProgressStyle("foregroundColor").value<QColor>();

    // Adjust colors for state
    switch (state) {
    case Core::FluentState::Hovered:
        foregroundColor = foregroundColor.lighter(110);
        break;
    case Core::FluentState::Disabled:
        foregroundColor = foregroundColor.lighter(150);
        backgroundColor = backgroundColor.lighter(120);
        break;
    default:
        break;
    }

    int penWidth = 4;
    QRect circleRect = rect.adjusted(penWidth/2, penWidth/2, -penWidth/2, -penWidth/2);

    // Paint background circle
    painter.setPen(QPen(backgroundColor, penWidth));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(circleRect);

    // Paint progress arc
    if (progress > 0.0) {
        painter.setPen(QPen(foregroundColor, penWidth, Qt::SolidLine, Qt::RoundCap));

        int startAngle = -90 * 16; // Start from top
        int spanAngle = static_cast<int>(360 * 16 * std::clamp(progress, 0.0, 1.0));

        painter.drawArc(circleRect, startAngle, spanAngle);
    }
}

// Touch feedback styling
void FluentCarouselStyles::paintRippleEffect(QPainter& painter, const QPoint& center,
                                           qreal radius, qreal opacity) const {
    painter.setRenderHint(QPainter::Antialiasing);

    QColor rippleColor = getTouchStyle("rippleColor").value<QColor>();
    qreal baseOpacity = getTouchStyle("rippleOpacity").toReal();

    rippleColor.setAlphaF(baseOpacity * opacity);

    painter.setBrush(rippleColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, static_cast<int>(radius), static_cast<int>(radius));
}

void FluentCarouselStyles::paintDragIndicator(QPainter& painter, const QRect& rect, qreal offset) const {
    painter.setRenderHint(QPainter::Antialiasing);

    QColor indicatorColor = getTouchStyle("dragIndicatorColor").value<QColor>();
    qreal opacity = getTouchStyle("dragIndicatorOpacity").toReal();

    indicatorColor.setAlphaF(opacity);

    QRect offsetRect = rect;
    offsetRect.translate(static_cast<int>(offset), 0);

    painter.fillRect(offsetRect, indicatorColor);
}

void FluentCarouselStyles::paintEdgeGlow(QPainter& painter, const QRect& rect,
                                       bool leftEdge, bool rightEdge) const {
    painter.setRenderHint(QPainter::Antialiasing);

    QColor glowColor = getTouchStyle("edgeGlowColor").value<QColor>();
    qreal opacity = getTouchStyle("edgeGlowOpacity").toReal();

    glowColor.setAlphaF(opacity);

    int glowWidth = 20;

    if (leftEdge) {
        QLinearGradient gradient(rect.left(), 0, rect.left() + glowWidth, 0);
        gradient.setColorAt(0, glowColor);
        gradient.setColorAt(1, Qt::transparent);

        painter.fillRect(QRect(rect.left(), rect.top(), glowWidth, rect.height()), gradient);
    }

    if (rightEdge) {
        QLinearGradient gradient(rect.right() - glowWidth, 0, rect.right(), 0);
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, glowColor);

        painter.fillRect(QRect(rect.right() - glowWidth, rect.top(), glowWidth, rect.height()), gradient);
    }
}

// Transition effects
void FluentCarouselStyles::paintSlideTransition(QPainter& painter, const QRect& rect,
                                              qreal progress, bool horizontal) const {
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    Q_UNUSED(progress)
    Q_UNUSED(horizontal)
    // Slide transitions are typically handled by widget positioning
    // This method could be used for custom slide effects or overlays
}

void FluentCarouselStyles::paintFadeTransition(QPainter& painter, const QRect& rect, qreal opacity) const {
    Q_UNUSED(rect)
    // Set painter opacity for fade effect
    painter.setOpacity(std::clamp(opacity, 0.0, 1.0));
}

void FluentCarouselStyles::paintScaleTransition(QPainter& painter, const QRect& rect, qreal scale) const {
    Q_UNUSED(rect)
    // Apply scale transformation
    qreal clampedScale = std::clamp(scale, 0.1, 2.0);
    painter.scale(clampedScale, clampedScale);
}

// Accessibility styling
void FluentCarouselStyles::paintFocusRing(QPainter& painter, const QRect& rect, int borderRadius) const {
    painter.setRenderHint(QPainter::Antialiasing);

    auto& theme = FluentTheme::instance();
    QColor focusColor = theme.color("accent");

    QPen focusPen(focusColor, 2);
    painter.setPen(focusPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), borderRadius + 2, borderRadius + 2);
}

void FluentCarouselStyles::paintHighContrastBorder(QPainter& painter, const QRect& rect, int borderRadius) const {
    painter.setRenderHint(QPainter::Antialiasing);

    // Use high contrast colors
    QColor borderColor = isHighContrast() ? Qt::white : Qt::black;

    QPen borderPen(borderColor, 2);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, borderRadius, borderRadius);
}

// Navigation button elevation
void FluentCarouselStyles::applyNavigationButtonElevation(QWidget* button, Core::FluentState state) const {
    if (!button) return;

    // Remove existing shadow effect
    if (auto* effect = button->graphicsEffect()) {
        if (qobject_cast<QGraphicsDropShadowEffect*>(effect)) {
            button->setGraphicsEffect(nullptr);
        }
    }

    // Apply elevation based on state
    int elevation = getNavigationStyle("elevation").toInt();
    if (state == Core::FluentState::Hovered) {
        elevation += 1;
    } else if (state == Core::FluentState::Pressed) {
        elevation = std::max(0, elevation - 1);
    } else if (state == Core::FluentState::Disabled) {
        elevation = 0;
    }

    if (elevation > 0) {
        auto* shadowEffect = new QGraphicsDropShadowEffect(button);
        shadowEffect->setBlurRadius(elevation);
        shadowEffect->setOffset(0, elevation / 2);
        shadowEffect->setColor(QColor(0, 0, 0, 30));
        button->setGraphicsEffect(shadowEffect);
    }
}

// Animation curves and timing
QString FluentCarouselStyles::getEasingCurve(const std::string& transitionType) const {
    auto it = m_transitionStyles.find(transitionType + "Easing");
    return (it != m_transitionStyles.end()) ? it->second.toString() : "cubic-bezier(0.25, 0.46, 0.45, 0.94)";
}

int FluentCarouselStyles::getAnimationDuration(const std::string& durationType) const {
    auto it = m_transitionStyles.find(durationType + "Duration");
    return (it != m_transitionStyles.end()) ? it->second.toInt() : 300;
}

// Responsive design helpers
int FluentCarouselStyles::getResponsiveSize(const std::string& sizeKey, int screenWidth) const {
    Q_UNUSED(sizeKey)

    // Scale based on screen width
    if (screenWidth <= m_breakpoints.mobile) {
        return 0.8; // 80% of base size
    } else if (screenWidth <= m_breakpoints.tablet) {
        return 0.9; // 90% of base size
    } else if (screenWidth <= m_breakpoints.desktop) {
        return 1.0; // 100% of base size
    } else {
        return 1.1; // 110% of base size
    }
}

QSize FluentCarouselStyles::getResponsiveIconSize(int screenWidth) const {
    if (screenWidth <= m_breakpoints.mobile) {
        return QSize(14, 14);
    } else if (screenWidth <= m_breakpoints.tablet) {
        return QSize(16, 16);
    } else if (screenWidth <= m_breakpoints.desktop) {
        return QSize(18, 18);
    } else {
        return QSize(20, 20);
    }
}

int FluentCarouselStyles::getResponsiveSpacing(int screenWidth) const {
    if (screenWidth <= m_breakpoints.mobile) {
        return 4;
    } else if (screenWidth <= m_breakpoints.tablet) {
        return 6;
    } else if (screenWidth <= m_breakpoints.desktop) {
        return 8;
    } else {
        return 12;
    }
}

// Theme integration
void FluentCarouselStyles::updateFromTheme() {
    initializeStyles();
}

bool FluentCarouselStyles::isDarkMode() const {
    auto& theme = FluentTheme::instance();
    return theme.mode() == FluentThemeMode::Dark;
}

bool FluentCarouselStyles::isHighContrast() const {
    // Check system high contrast mode
    // This would need platform-specific implementation
    return false;
}

bool FluentCarouselStyles::isReducedMotion() const {
    // Check system reduced motion preference
    // This would need platform-specific implementation
    return false;
}

} // namespace FluentQt::Styling
