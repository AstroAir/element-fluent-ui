// src/Components/FluentButton.cpp
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QStyleOption>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentButton::FluentButton(QWidget* parent)
    : Core::FluentComponent(parent),
      m_clickTimer(std::make_unique<QTimer>(this)) {
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover);

    // Single shot timer for click animation
    m_clickTimer->setSingleShot(true);
    m_clickTimer->setInterval(150);

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentButton::onThemeChanged);

    updateAccessibility();
    updateGeometry();
}

FluentButton::FluentButton(const QString& text, QWidget* parent)
    : FluentButton(parent) {
    setText(text);
}

FluentButton::FluentButton(const QIcon& icon, const QString& text,
                           QWidget* parent)
    : FluentButton(parent) {
    setIcon(icon);
    setText(text);
}

FluentButton::~FluentButton() = default;

void FluentButton::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        m_sizeHintValid = false;
        invalidateCache(FluentButtonDirtyRegion::Content);
        updateGeometry();
        updateAccessibility();
        update();
        emit textChanged(text);
    }
}

void FluentButton::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        m_sizeHintValid = false;
        invalidateCache(FluentButtonDirtyRegion::Content);
        updateGeometry();
        update();
        emit iconChanged(icon);
    }
}

void FluentButton::setIconPosition(FluentIconPosition position) {
    if (m_iconPosition != position) {
        m_iconPosition = position;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit iconPositionChanged(position);
    }
}

void FluentButton::setButtonStyle(FluentButtonStyle style) {
    if (m_buttonStyle != style) {
        m_buttonStyle = style;
        invalidateCache(FluentButtonDirtyRegion::All);
        updateStateStyle();
        emit styleChanged(style);
    }
}

void FluentButton::setButtonSize(FluentButtonSize size) {
    if (m_buttonSize != size) {
        m_buttonSize = size;
        m_sizeHintValid = false;
        updateGeometry();
        emit sizeChanged(size);
    }
}

void FluentButton::setFlat(bool flat) {
    if (m_flat != flat) {
        m_flat = flat;
        update();
    }
}

void FluentButton::setLoading(bool loading) {
    if (m_loading != loading) {
        m_loading = loading;

        if (loading) {
            startLoadingAnimation();
        } else {
            stopLoadingAnimation();
        }

        setEnabled(!loading);
        update();
        emit loadingChanged(loading);
    }
}

void FluentButton::setCheckable(bool checkable) {
    if (m_checkable != checkable) {
        m_checkable = checkable;
        if (!checkable) {
            setChecked(false);
        }
        updateAccessibility();
    }
}

void FluentButton::setChecked(bool checked) {
    if (m_checkable && m_checked != checked) {
        m_checked = checked;
        updateStateStyle();
        updateAccessibility();
        emit checkedChanged(checked);
        emit toggled(checked);
    }
}

void FluentButton::setBackgroundOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_backgroundOpacity, opacity)) {
        m_backgroundOpacity = qBound(0.0, opacity, 1.0);
        invalidateCache(FluentButtonDirtyRegion::Background);
        update();
    }
}

void FluentButton::setLoadingRotation(qreal rotation) {
    if (!qFuzzyCompare(m_loadingRotation, rotation)) {
        m_loadingRotation = rotation;
        if (m_loading) {
            update();
        }
    }
}

void FluentButton::setRevealProgress(qreal progress) {
    if (!qFuzzyCompare(m_revealProgress, progress)) {
        m_revealProgress = qBound(0.0, progress, 1.0);
        update();
    }
}

QSize FluentButton::sizeHint() const {
    FLUENT_PROFILE("FluentButton::sizeHint");

    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }

    const QSize iconSize = calculateIconSize();
    const QSize textSize = calculateTextSize();

    int contentWidth = 0;
    int contentHeight = 0;

    // Calculate content dimensions based on icon position
    switch (m_iconPosition) {
        case FluentIconPosition::Left:
        case FluentIconPosition::Right:
            contentWidth = iconSize.width() + textSize.width();
            if (!m_icon.isNull() && !m_text.isEmpty()) {
                contentWidth += getIconTextSpacing();
            }
            contentHeight = std::max(iconSize.height(), textSize.height());
            break;

        case FluentIconPosition::Top:
        case FluentIconPosition::Bottom:
            contentWidth = std::max(iconSize.width(), textSize.width());
            contentHeight = iconSize.height() + textSize.height();
            if (!m_icon.isNull() && !m_text.isEmpty()) {
                contentHeight += getIconTextSpacing();
            }
            break;
    }

    // Add padding
    const int horizontalPadding = 2 * getHorizontalPadding();
    const int verticalPadding = 2 * getVerticalPadding();

    // Minimum size constraints using design tokens
    int minWidth = 32;  // Minimum touch target
    int minHeight = 32;

    if (m_useDesignTokens) {
        QString sizeStr;
        switch (m_buttonSize) {
            case FluentButtonSize::Small:
                sizeStr = "small";
                break;
            case FluentButtonSize::Medium:
                sizeStr = "medium";
                break;
            case FluentButtonSize::Large:
                sizeStr = "large";
                break;
            case FluentButtonSize::ExtraLarge:
                sizeStr = "extraLarge";
                break;
            default:
                sizeStr = "medium";
        }

        int tokenHeight =
            getTokenSize(QString("button.size.%1.height").arg(sizeStr));
        int tokenWidth =
            getTokenSize(QString("button.size.%1.minWidth").arg(sizeStr));

        if (tokenHeight > 0)
            minHeight = tokenHeight;
        if (tokenWidth > 0)
            minWidth = tokenWidth;
    } else {
        // Fallback to hardcoded values
        switch (m_buttonSize) {
            case FluentButtonSize::Small:
                minHeight = 24;
                break;
            case FluentButtonSize::Medium:
                minHeight = 32;
                break;
            case FluentButtonSize::Large:
                minHeight = 40;
                break;
            case FluentButtonSize::ExtraLarge:
                minHeight = 48;
                break;
        }
    }

    const QSize result(std::max(minWidth, contentWidth + horizontalPadding),
                       std::max(minHeight, contentHeight + verticalPadding));

    m_cachedSizeHint = result;
    m_sizeHintValid = true;

    return result;
}

QSize FluentButton::minimumSizeHint() const {
    return QSize(32, 24);  // Minimum touch-friendly size
}

void FluentButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentButton::paintEvent");

    const QRect rect = this->rect().adjusted(0, 0, -1, -1);

    // Update cache if needed
    updateCacheIfNeeded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Use cached rendering when possible
    if (m_backgroundCacheValid &&
        (!m_flat || state() != Core::FluentState::Normal)) {
        painter.drawPixmap(rect, m_cachedBackground);
    }

    if (m_borderCacheValid &&
        (m_buttonStyle == FluentButtonStyle::Outline || hasFocus())) {
        painter.drawPixmap(rect, m_cachedBorder);
    }

    // Paint reveal effect (always dynamic, cannot be cached)
    if (m_revealProgress > 0.0) {
        paintRevealEffect(painter, rect);
    }

    if (m_contentCacheValid) {
        painter.drawPixmap(rect, m_cachedContent);
    }

    // Paint focus ring (dynamic, cannot be cached effectively)
    if (hasFocus() && !m_pressed) {
        paintFocusRing(painter, rect);
    }
}

void FluentButton::paintBackground(QPainter& painter, const QRect& rect) {
    const QColor backgroundColor = getBackgroundColor();

    if (backgroundColor.alpha() == 0) {
        // For Text buttons, we might want to show a subtle background on
        // hover/press
        if (m_buttonStyle == FluentButtonStyle::Text &&
            (state() == Core::FluentState::Hovered ||
             state() == Core::FluentState::Pressed)) {
            const auto& theme = Styling::FluentTheme::instance();
            const auto& palette = theme.currentPalette();

            QColor subtleBackground = palette.neutralLight;
            subtleBackground.setAlphaF(0.1);

            QPainterPath path;
            const int radius = cornerRadius();
            path.addRoundedRect(rect, radius, radius);
            painter.fillPath(path, subtleBackground);
        }
        return;
    }

    // Create path with rounded corners
    QPainterPath path;
    const int radius = cornerRadius();
    path.addRoundedRect(rect, radius, radius);

    // Apply opacity animation
    QColor finalColor = backgroundColor;
    finalColor.setAlphaF(finalColor.alphaF() * m_backgroundOpacity);

    // Add subtle gradient for depth (Primary, Accent, and Split buttons)
    if (m_buttonStyle == FluentButtonStyle::Primary ||
        m_buttonStyle == FluentButtonStyle::Accent ||
        m_buttonStyle == FluentButtonStyle::Split) {
        QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
        gradient.setColorAt(0.0, finalColor.lighter(105));
        gradient.setColorAt(1.0, finalColor.darker(105));
        painter.fillPath(path, gradient);
    } else {
        painter.fillPath(path, finalColor);
    }

    // Add subtle shadow for elevated buttons
    if (state() == Core::FluentState::Hovered &&
        (m_buttonStyle == FluentButtonStyle::Default ||
         m_buttonStyle == FluentButtonStyle::Primary ||
         m_buttonStyle == FluentButtonStyle::Split)) {
        painter.save();
        painter.setPen(Qt::NoPen);

        QColor shadowColor = Qt::black;
        shadowColor.setAlphaF(0.1);

        QPainterPath shadowPath;
        shadowPath.addRoundedRect(rect.adjusted(0, 1, 0, 1), radius, radius);
        painter.fillPath(shadowPath, shadowColor);

        painter.restore();
    }

    // Special handling for Split buttons - add dropdown indicator
    if (m_buttonStyle == FluentButtonStyle::Split && !m_icon.isNull()) {
        painter.save();

        // Draw separator line for split button
        const int separatorX = rect.right() - 24;  // 24px from right edge
        QColor separatorColor = getTextColor();
        separatorColor.setAlphaF(0.3);

        QPen separatorPen(separatorColor, 1);
        painter.setPen(separatorPen);
        painter.drawLine(separatorX, rect.top() + 6, separatorX,
                         rect.bottom() - 6);

        painter.restore();
    }
}

void FluentButton::paintBorder(QPainter& painter, const QRect& rect) {
    if (m_buttonStyle != FluentButtonStyle::Outline && !hasFocus()) {
        return;
    }

    painter.save();

    const QPen borderPen = getBorderPen();
    painter.setPen(borderPen);

    const int radius = cornerRadius();
    const QRectF borderRect = rect.adjusted(0.5, 0.5, -0.5, -0.5);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentButton::paintContent(QPainter& painter, const QRect& rect) {
    if (m_loading) {
        paintLoadingIndicator(painter, rect);
        return;
    }

    const QRect iconRect = calculateIconRect();
    const QRect textRect = calculateTextRect();

    // Paint icon
    if (!m_icon.isNull()) {
        paintIcon(painter, iconRect);
    }

    // Paint text
    if (!m_text.isEmpty()) {
        paintText(painter, textRect);
    }
}

void FluentButton::paintIcon(QPainter& painter, const QRect& iconRect) {
    const QIcon::Mode iconMode =
        isEnabled() ? (state() == Core::FluentState::Pressed ? QIcon::Selected
                                                             : QIcon::Normal)
                    : QIcon::Disabled;

    const QIcon::State iconState = m_checked ? QIcon::On : QIcon::Off;

    m_icon.paint(&painter, iconRect, Qt::AlignCenter, iconMode, iconState);
}

void FluentButton::paintText(QPainter& painter, const QRect& textRect) {
    painter.save();

    painter.setFont(getFont());
    painter.setPen(getTextColor());

    const Qt::Alignment alignment = Qt::AlignCenter;
    const int flags = alignment | Qt::TextShowMnemonic;

    painter.drawText(textRect, flags, m_text);

    painter.restore();
}

void FluentButton::paintLoadingIndicator(QPainter& painter, const QRect& rect) {
    painter.save();

    const QPointF center = rect.center();
    const qreal radius = std::min(rect.width(), rect.height()) * 0.15;

    painter.translate(center);
    painter.rotate(m_loadingRotation);

    const QColor color = getTextColor();
    const int dotCount = 8;
    const qreal dotRadius = radius * 0.3;

    for (int i = 0; i < dotCount; ++i) {
        painter.save();

        const qreal angle = (360.0 / dotCount) * i;
        painter.rotate(angle);

        QColor dotColor = color;
        dotColor.setAlphaF(0.3 + 0.7 * (i / static_cast<qreal>(dotCount - 1)));

        painter.setBrush(dotColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(radius, 0), dotRadius, dotRadius);

        painter.restore();
    }

    painter.restore();
}

void FluentButton::paintFocusRing(QPainter& painter, const QRect& rect) {
    if (!hasFocus()) {
        return;
    }

    painter.save();

    const QColor focusColor = getFocusColor();
    QPen focusPen(focusColor, 2);
    focusPen.setStyle(Qt::SolidLine);

    painter.setPen(focusPen);
    painter.setBrush(Qt::NoBrush);

    const int radius = cornerRadius() + 2;
    const QRect focusRect = rect.adjusted(-2, -2, 2, 2);

    painter.drawRoundedRect(focusRect, radius, radius);

    painter.restore();
}

void FluentButton::paintRevealEffect(QPainter& painter, const QRect& rect) {
    if (m_revealProgress <= 0.0) {
        return;
    }

    painter.save();

    const QColor revealColor = getTextColor();
    QColor effectColor = revealColor;
    effectColor.setAlphaF(0.1 * m_revealProgress);

    const qreal maxRadius =
        std::sqrt(rect.width() * rect.width() + rect.height() * rect.height());
    const qreal currentRadius = maxRadius * m_revealProgress;

    QRadialGradient gradient(m_revealCenter, currentRadius);
    gradient.setColorAt(0.0, effectColor);
    gradient.setColorAt(1.0, Qt::transparent);

    QPainterPath clipPath;
    clipPath.addRoundedRect(rect, cornerRadius(), cornerRadius());
    painter.setClipPath(clipPath);

    painter.fillRect(rect, gradient);

    painter.restore();
}

void FluentButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        m_revealCenter = event->position().toPoint();

        setState(Core::FluentState::Pressed);

        if (isAnimated()) {
            startRevealAnimation(m_revealCenter);
        }

        emit pressed();
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        const bool wasInside = rect().contains(event->position().toPoint());

        if (wasInside) {
            if (m_checkable) {
                setChecked(!m_checked);
            }

            emit clicked(m_checked);
            emit released();

            // Brief click animation (visual only)
            if (isAnimated()) {
                animateClickVisual();
            }
        }

        setState(rect().contains(event->position().toPoint())
                     ? Core::FluentState::Hovered
                     : Core::FluentState::Normal);
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentButton::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        if (!event->isAutoRepeat()) {
            m_spacePressedOnButton = true;
            m_pressed = true;
            setState(Core::FluentState::Pressed);
            emit pressed();

            if (isAnimated()) {
                m_revealCenter = rect().center();
                startRevealAnimation(m_revealCenter);
            }
        }
        event->accept();
        return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentButton::keyReleaseEvent(QKeyEvent* event) {
    if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) &&
        m_spacePressedOnButton && !event->isAutoRepeat()) {
        m_spacePressedOnButton = false;
        m_pressed = false;

        if (m_checkable) {
            setChecked(!m_checked);
        }

        emit clicked(m_checked);
        emit released();

        setState(Core::FluentState::Focused);

        if (isAnimated()) {
            animateClickVisual();
        }

        event->accept();
        return;
    }

    Core::FluentComponent::keyReleaseEvent(event);
}

void FluentButton::focusInEvent(QFocusEvent* event) {
    setState(Core::FluentState::Focused);
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentButton::focusOutEvent(QFocusEvent* event) {
    setState(underMouse() ? Core::FluentState::Hovered
                          : Core::FluentState::Normal);
    update();
    Core::FluentComponent::focusOutEvent(event);
}

void FluentButton::resizeEvent(QResizeEvent* event) {
    // Invalidate cache when size changes
    invalidateCache(FluentButtonDirtyRegion::All);
    Core::FluentComponent::resizeEvent(event);
}

void FluentButton::changeEvent(QEvent* event) {
    if (event->type() == QEvent::EnabledChange) {
        setState(isEnabled() ? Core::FluentState::Normal
                             : Core::FluentState::Disabled);
        invalidateCache(FluentButtonDirtyRegion::All);
        updateStateStyle();
        updateAccessibility();
        update();
    } else if (event->type() == QEvent::FontChange ||
               event->type() == QEvent::StyleChange) {
        invalidateCache(FluentButtonDirtyRegion::All);
        updateStateStyle();
        updateAccessibility();
        update();
    }
    Core::FluentComponent::changeEvent(event);
}

void FluentButton::updateStateStyle() {
    if (!isAnimated()) {
        update();
        return;
    }

    createBackgroundAnimation();
}

void FluentButton::performStateTransition(Core::FluentState from,
                                          Core::FluentState to) {
    if (isAnimated()) {
        startStateTransitionAnimation(from, to);
    } else {
        update();
    }
}

// Color calculation methods
QColor FluentButton::getBackgroundColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Handle checked state for toggle buttons
    const bool effectivePressed = m_pressed || (m_checkable && m_checked);

    switch (m_buttonStyle) {
        case FluentButtonStyle::Primary:
            switch (state()) {
                case Core::FluentState::Normal:
                    return effectivePressed ? palette.accentDark1
                                            : palette.accent;
                case Core::FluentState::Hovered:
                    return effectivePressed ? palette.accentDark2
                                            : palette.accentLight1;
                case Core::FluentState::Pressed:
                    return palette.accentDark1;
                case Core::FluentState::Disabled:
                    return palette.neutralQuaternary;
                default:
                    return palette.accent;
            }

        case FluentButtonStyle::Accent:
            switch (state()) {
                case Core::FluentState::Normal:
                    return effectivePressed ? palette.accentDark1
                                            : palette.accent;
                case Core::FluentState::Hovered:
                    return effectivePressed ? palette.accentDark2
                                            : palette.accentLight1;
                case Core::FluentState::Pressed:
                    return palette.accentDark1;
                case Core::FluentState::Disabled:
                    return palette.neutralQuaternary;
                default:
                    return palette.accent;
            }

        case FluentButtonStyle::Subtle:
            switch (state()) {
                case Core::FluentState::Normal:
                    return effectivePressed ? palette.neutralQuaternaryAlt
                                            : Qt::transparent;
                case Core::FluentState::Hovered:
                    return effectivePressed ? palette.neutralQuaternary
                                            : palette.neutralLight;
                case Core::FluentState::Pressed:
                    return palette.neutralQuaternaryAlt;
                case Core::FluentState::Disabled:
                    return Qt::transparent;
                default:
                    return Qt::transparent;
            }

        case FluentButtonStyle::Outline:
            switch (state()) {
                case Core::FluentState::Normal:
                    return effectivePressed ? palette.neutralLight
                                            : Qt::transparent;
                case Core::FluentState::Hovered:
                    return effectivePressed ? palette.neutralQuaternaryAlt
                                            : palette.neutralLighter;
                case Core::FluentState::Pressed:
                    return palette.neutralLight;
                case Core::FluentState::Disabled:
                    return Qt::transparent;
                default:
                    return Qt::transparent;
            }

        case FluentButtonStyle::Hyperlink:
            return Qt::transparent;

        case FluentButtonStyle::Text:
            return Qt::transparent;  // Text buttons have no background

        case FluentButtonStyle::Split:
            // Split buttons use the same background as Default but with special
            // handling
            switch (state()) {
                case Core::FluentState::Normal:
                    return effectivePressed ? palette.neutralQuaternaryAlt
                                            : palette.neutralLighter;
                case Core::FluentState::Hovered:
                    return effectivePressed ? palette.neutralQuaternary
                                            : palette.neutralLight;
                case Core::FluentState::Pressed:
                    return palette.neutralQuaternaryAlt;
                case Core::FluentState::Disabled:
                    return palette.neutralLighter;
                default:
                    return palette.neutralLighter;
            }

        default:  // FluentButtonStyle::Default
            switch (state()) {
                case Core::FluentState::Normal:
                    return effectivePressed ? palette.neutralQuaternaryAlt
                                            : palette.neutralLighter;
                case Core::FluentState::Hovered:
                    return effectivePressed ? palette.neutralQuaternary
                                            : palette.neutralLight;
                case Core::FluentState::Pressed:
                    return palette.neutralQuaternaryAlt;
                case Core::FluentState::Disabled:
                    return palette.neutralLighter;
                default:
                    return palette.neutralLighter;
            }
    }
}

QColor FluentButton::getTextColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    switch (m_buttonStyle) {
        case FluentButtonStyle::Primary:
        case FluentButtonStyle::Accent:
            switch (state()) {
                case Core::FluentState::Disabled:
                    return palette.neutralTertiary;
                default:
                    return Qt::white;
            }

        case FluentButtonStyle::Hyperlink:
            switch (state()) {
                case Core::FluentState::Normal:
                    return palette.accent;
                case Core::FluentState::Hovered:
                    return palette.accentLight1;
                case Core::FluentState::Pressed:
                    return palette.accentDark1;
                case Core::FluentState::Disabled:
                    return palette.neutralTertiary;
                default:
                    return palette.accent;
            }

        case FluentButtonStyle::Text:
            // Text buttons use accent color for text
            switch (state()) {
                case Core::FluentState::Normal:
                    return palette.accent;
                case Core::FluentState::Hovered:
                    return palette.accentLight1;
                case Core::FluentState::Pressed:
                    return palette.accentDark1;
                case Core::FluentState::Disabled:
                    return palette.neutralTertiary;
                default:
                    return palette.accent;
            }

        case FluentButtonStyle::Split:
            // Split buttons use standard text color
            switch (state()) {
                case Core::FluentState::Disabled:
                    return palette.neutralTertiary;
                default:
                    return palette.neutralPrimary;
            }

        default:
            switch (state()) {
                case Core::FluentState::Disabled:
                    return palette.neutralTertiary;
                default:
                    return palette.neutralPrimary;
            }
    }
}

QColor FluentButton::getBorderColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    switch (state()) {
        case Core::FluentState::Normal:
            return palette.neutralSecondary;
        case Core::FluentState::Hovered:
            return palette.neutralPrimary;
        case Core::FluentState::Pressed:
        case Core::FluentState::Focused:
            return palette.accent;
        case Core::FluentState::Disabled:
            return palette.neutralTertiaryAlt;
        default:
            return palette.neutralSecondary;
    }
}

QColor FluentButton::getFocusColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    return theme.currentPalette().accent;
}

QPen FluentButton::getBorderPen() const {
    return QPen(getBorderColor(), getBorderWidth());
}

QFont FluentButton::getFont() const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_buttonSize) {
        case FluentButtonSize::Small:
            return theme.captionFont();
        case FluentButtonSize::Large:
        case FluentButtonSize::ExtraLarge:
            return theme.subtitleFont();
        default:
            return theme.bodyFont();
    }
}

// Size and spacing calculations
int FluentButton::getHorizontalPadding() const {
    if (m_useDesignTokens) {
        QString sizeStr;
        switch (m_buttonSize) {
            case FluentButtonSize::Small:
                sizeStr = "small";
                break;
            case FluentButtonSize::Medium:
                sizeStr = "medium";
                break;
            case FluentButtonSize::Large:
                sizeStr = "large";
                break;
            case FluentButtonSize::ExtraLarge:
                sizeStr = "extraLarge";
                break;
            default:
                sizeStr = "medium";
        }
        return getTokenSize(
            QString("button.size.%1.padding.horizontal").arg(sizeStr));
    }

    // Fallback to hardcoded values
    switch (m_buttonSize) {
        case FluentButtonSize::Small:
            return 8;
        case FluentButtonSize::Medium:
            return 12;
        case FluentButtonSize::Large:
            return 16;
        case FluentButtonSize::ExtraLarge:
            return 20;
        default:
            return 12;
    }
}

int FluentButton::getVerticalPadding() const {
    if (m_useDesignTokens) {
        QString sizeStr;
        switch (m_buttonSize) {
            case FluentButtonSize::Small:
                sizeStr = "small";
                break;
            case FluentButtonSize::Medium:
                sizeStr = "medium";
                break;
            case FluentButtonSize::Large:
                sizeStr = "large";
                break;
            case FluentButtonSize::ExtraLarge:
                sizeStr = "extraLarge";
                break;
            default:
                sizeStr = "medium";
        }
        return getTokenSize(
            QString("button.size.%1.padding.vertical").arg(sizeStr));
    }

    // Fallback to hardcoded values
    switch (m_buttonSize) {
        case FluentButtonSize::Small:
            return 4;
        case FluentButtonSize::Medium:
            return 6;
        case FluentButtonSize::Large:
            return 8;
        case FluentButtonSize::ExtraLarge:
            return 12;
        default:
            return 6;
    }
}

int FluentButton::getIconTextSpacing() const {
    if (m_useDesignTokens) {
        QString sizeStr;
        switch (m_buttonSize) {
            case FluentButtonSize::Small:
                sizeStr = "small";
                break;
            case FluentButtonSize::Medium:
                sizeStr = "medium";
                break;
            case FluentButtonSize::Large:
                sizeStr = "large";
                break;
            case FluentButtonSize::ExtraLarge:
                sizeStr = "extraLarge";
                break;
            default:
                sizeStr = "medium";
        }
        return getTokenSpacing(
            QString("button.size.%1.iconSpacing").arg(sizeStr));
    }

    // Fallback to hardcoded values
    switch (m_buttonSize) {
        case FluentButtonSize::Small:
            return 4;
        case FluentButtonSize::Medium:
            return 6;
        case FluentButtonSize::Large:
        case FluentButtonSize::ExtraLarge:
            return 8;
        default:
            return 6;
    }
}

int FluentButton::getBorderWidth() const { return 1; }

// Rect calculations
QRect FluentButton::calculateContentRect() const {
    const int hPadding = getHorizontalPadding();
    const int vPadding = getVerticalPadding();
    return rect().adjusted(hPadding, vPadding, -hPadding, -vPadding);
}

QRect FluentButton::calculateIconRect() const {
    if (m_icon.isNull()) {
        return QRect();
    }

    const QRect contentRect = calculateContentRect();
    const QSize iconSize = calculateIconSize();

    QPoint iconPos;

    switch (m_iconPosition) {
        case FluentIconPosition::Left:
            iconPos = QPoint(contentRect.left(),
                             contentRect.center().y() - iconSize.height() / 2);
            break;

        case FluentIconPosition::Right:
            iconPos = QPoint(contentRect.right() - iconSize.width(),
                             contentRect.center().y() - iconSize.height() / 2);
            break;

        case FluentIconPosition::Top:
            iconPos = QPoint(contentRect.center().x() - iconSize.width() / 2,
                             contentRect.top());
            break;

        case FluentIconPosition::Bottom:
            iconPos = QPoint(contentRect.center().x() - iconSize.width() / 2,
                             contentRect.bottom() - iconSize.height());
            break;
    }

    return QRect(iconPos, iconSize);
}

QRect FluentButton::calculateTextRect() const {
    if (m_text.isEmpty()) {
        return QRect();
    }

    const QRect contentRect = calculateContentRect();
    const QSize iconSize = m_icon.isNull() ? QSize() : calculateIconSize();
    const QSize textSize = calculateTextSize();

    QPoint textPos;
    QSize availableSize;

    switch (m_iconPosition) {
        case FluentIconPosition::Left:
            textPos = QPoint(contentRect.left() + iconSize.width() +
                                 (iconSize.isNull() ? 0 : getIconTextSpacing()),
                             contentRect.center().y() - textSize.height() / 2);
            availableSize =
                QSize(contentRect.width() - iconSize.width() -
                          (iconSize.isNull() ? 0 : getIconTextSpacing()),
                      textSize.height());
            break;

        case FluentIconPosition::Right:
            textPos = QPoint(contentRect.left(),
                             contentRect.center().y() - textSize.height() / 2);
            availableSize =
                QSize(contentRect.width() - iconSize.width() -
                          (iconSize.isNull() ? 0 : getIconTextSpacing()),
                      textSize.height());
            break;

        case FluentIconPosition::Top:
            textPos =
                QPoint(contentRect.center().x() - textSize.width() / 2,
                       contentRect.top() + iconSize.height() +
                           (iconSize.isNull() ? 0 : getIconTextSpacing()));
            availableSize = textSize;
            break;

        case FluentIconPosition::Bottom:
            textPos = QPoint(contentRect.center().x() - textSize.width() / 2,
                             contentRect.top());
            availableSize =
                QSize(textSize.width(),
                      contentRect.height() - iconSize.height() -
                          (iconSize.isNull() ? 0 : getIconTextSpacing()));
            break;
    }

    return QRect(textPos, availableSize);
}

QSize FluentButton::calculateIconSize() const {
    if (m_icon.isNull()) {
        return QSize();
    }

    int iconSize;
    if (m_useDesignTokens) {
        QString sizeStr;
        switch (m_buttonSize) {
            case FluentButtonSize::Small:
                sizeStr = "small";
                break;
            case FluentButtonSize::Medium:
                sizeStr = "medium";
                break;
            case FluentButtonSize::Large:
                sizeStr = "large";
                break;
            case FluentButtonSize::ExtraLarge:
                sizeStr = "extraLarge";
                break;
            default:
                sizeStr = "medium";
        }
        iconSize =
            getTokenSize(QString("button.size.%1.iconSize").arg(sizeStr));
        if (iconSize <= 0) {
            // Fallback to hardcoded values if token not found
            iconSize = (m_buttonSize == FluentButtonSize::Small)    ? 12
                       : (m_buttonSize == FluentButtonSize::Medium) ? 16
                       : (m_buttonSize == FluentButtonSize::Large)  ? 20
                                                                    : 24;
        }
    } else {
        // Fallback to hardcoded values
        switch (m_buttonSize) {
            case FluentButtonSize::Small:
                iconSize = 12;
                break;
            case FluentButtonSize::Medium:
                iconSize = 16;
                break;
            case FluentButtonSize::Large:
                iconSize = 20;
                break;
            case FluentButtonSize::ExtraLarge:
                iconSize = 24;
                break;
            default:
                iconSize = 16;
        }
    }

    return QSize(iconSize, iconSize);
}

QSize FluentButton::calculateTextSize() const {
    if (m_text.isEmpty()) {
        return QSize();
    }

    const QFontMetrics fm(getFont());
    return fm.size(Qt::TextShowMnemonic, m_text);
}

// Animation methods
void FluentButton::startStateTransitionAnimation(Core::FluentState from,
                                                 Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)

    createBackgroundAnimation();
}

void FluentButton::createBackgroundAnimation() {
    if (!m_backgroundAnimation) {
        m_backgroundAnimation =
            std::make_unique<QPropertyAnimation>(this, "backgroundOpacity");
        connect(m_backgroundAnimation.get(), &QPropertyAnimation::finished,
                this, &FluentButton::onBackgroundAnimationFinished);
    }

    m_backgroundAnimation->stop();

    // Use FluentUI 2.0 standard duration and easing
    if (m_useDesignTokens) {
        int duration = getTokenSize(
            "animation.duration.normal");  // 200ms for FluentUI 2.0
        m_backgroundAnimation->setDuration(duration > 0 ? duration : 200);
    } else {
        m_backgroundAnimation->setDuration(200);  // FluentUI 2.0 standard
    }

    // Use FluentUI 2.0 standard easing curve (0.1, 0.9, 0.2, 1.0)
    QEasingCurve fluentCurve(QEasingCurve::BezierSpline);
    fluentCurve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0),
                                      QPointF(1.0, 1.0));
    m_backgroundAnimation->setEasingCurve(fluentCurve);

    m_backgroundAnimation->setStartValue(m_backgroundOpacity);
    m_backgroundAnimation->setEndValue(1.0);
    m_backgroundAnimation->start();
}

void FluentButton::startLoadingAnimation() {
    if (!m_loadingAnimation) {
        m_loadingAnimation =
            std::make_unique<QPropertyAnimation>(this, "loadingRotation");
        m_loadingAnimation->setDuration(1000);
        m_loadingAnimation->setLoopCount(-1);
        m_loadingAnimation->setStartValue(0.0);
        m_loadingAnimation->setEndValue(360.0);
        m_loadingAnimation->setEasingCurve(QEasingCurve::Linear);

        connect(m_loadingAnimation.get(), &QPropertyAnimation::valueChanged,
                this, &FluentButton::onLoadingAnimationValueChanged);
    }

    m_loadingAnimation->start();
}

void FluentButton::stopLoadingAnimation() {
    if (m_loadingAnimation) {
        m_loadingAnimation->stop();
        m_loadingRotation = 0.0;
    }
}

void FluentButton::startRevealAnimation(const QPoint& center) {
    m_revealCenter = center;

    if (!m_revealAnimation) {
        m_revealAnimation = std::make_unique<QSequentialAnimationGroup>(this);

        // Use Fluent Design reveal animation timing and curves
        auto expandAnim = new QPropertyAnimation(this, "revealProgress");
        expandAnim->setDuration(250);  // Fluent Design standard duration

        // Create Fluent reveal curve (0.0, 0.0, 0.2, 1.0)
        QEasingCurve revealCurve(QEasingCurve::BezierSpline);
        revealCurve.addCubicBezierSegment(QPointF(0.0, 0.0), QPointF(0.2, 1.0),
                                          QPointF(1.0, 1.0));
        expandAnim->setEasingCurve(revealCurve);

        expandAnim->setStartValue(0.0);
        expandAnim->setEndValue(1.0);

        auto fadeAnim = new QPropertyAnimation(this, "revealProgress");
        fadeAnim->setDuration(150);  // Faster fade for better responsiveness

        // Use Fluent decelerate curve for fade out
        QEasingCurve fadeCurve(QEasingCurve::BezierSpline);
        fadeCurve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
        fadeAnim->setEasingCurve(fadeCurve);

        fadeAnim->setStartValue(1.0);
        fadeAnim->setEndValue(0.0);

        m_revealAnimation->addAnimation(expandAnim);
        m_revealAnimation->addAnimation(fadeAnim);
    }

    m_revealAnimation->start();
}

void FluentButton::animateClick() {
    if (!isEnabled()) {
        return;
    }

    // Trigger the click logic
    if (m_checkable) {
        setChecked(!m_checked);
    }
    emit clicked(m_checked);

    // Perform visual animation
    animateClickVisual();
}

void FluentButton::animateClickVisual() {
    if (!m_clickTimer->isActive()) {
        const qreal originalOpacity = m_backgroundOpacity;

        auto clickAnim =
            std::make_unique<QPropertyAnimation>(this, "backgroundOpacity");

        // Use FluentUI 2.0 micro-interaction timing (80ms for press)
        int pressDuration = 80;
        if (m_useDesignTokens) {
            pressDuration = getTokenSize("animation.duration.fast");
            if (pressDuration <= 0)
                pressDuration = 80;
        }
        clickAnim->setDuration(pressDuration);
        clickAnim->setStartValue(originalOpacity);
        clickAnim->setEndValue(0.7);

        // Use FluentUI 2.0 accelerate curve for press (0.7, 0.0, 1.0, 0.5)
        QEasingCurve pressCurve(QEasingCurve::BezierSpline);
        pressCurve.addCubicBezierSegment(QPointF(0.7, 0.0), QPointF(1.0, 0.5),
                                         QPointF(1.0, 1.0));
        clickAnim->setEasingCurve(pressCurve);

        connect(
            clickAnim.get(), &QPropertyAnimation::finished,
            [this, originalOpacity]() {
                auto restoreAnim = std::make_unique<QPropertyAnimation>(
                    this, "backgroundOpacity");

                // Use FluentUI 2.0 restore timing (120ms)
                int restoreDuration = 120;
                if (m_useDesignTokens) {
                    restoreDuration = getTokenSize("animation.duration.normal");
                    if (restoreDuration <= 0)
                        restoreDuration = 120;
                }
                restoreAnim->setDuration(restoreDuration);
                restoreAnim->setStartValue(0.7);
                restoreAnim->setEndValue(originalOpacity);

                // Use FluentUI 2.0 decelerate curve for restore (0.1, 0.9,
                // 0.2, 1.0)
                QEasingCurve restoreCurve(QEasingCurve::BezierSpline);
                restoreCurve.addCubicBezierSegment(
                    QPointF(0.1, 0.9), QPointF(0.2, 1.0), QPointF(1.0, 1.0));
                restoreAnim->setEasingCurve(restoreCurve);

                restoreAnim->start();
            });

        clickAnim->start();
        m_clickTimer->start();
    }
}

void FluentButton::toggle() {
    if (m_checkable) {
        setChecked(!m_checked);
    }
}

// Slots
void FluentButton::onLoadingAnimationValueChanged(const QVariant& value) {
    m_loadingRotation = value.toReal();
    update();
}

void FluentButton::onBackgroundAnimationFinished() {
    // Animation completed
}

void FluentButton::onThemeChanged() { updateStateStyle(); }

// Utility methods
void FluentButton::updateGeometry() {
    QWidget::updateGeometry();
    if (parentWidget()) {
        parentWidget()->updateGeometry();
    }
}

void FluentButton::updateAccessibility() {
    QString name = m_accessibleName.isEmpty()
                       ? (m_text.isEmpty() ? "Button" : m_text)
                       : m_accessibleName;
    Accessibility::setAccessibleName(this, name);

    QString description =
        m_accessibleDescription.isEmpty() ? m_text : m_accessibleDescription;
    if (m_checkable) {
        description += m_checked ? " (checked)" : " (unchecked)";
    }
    if (m_loading) {
        description += " (loading)";
    }

    Accessibility::setAccessibleDescription(this, description);
    Accessibility::setAccessibleRole(this, m_accessibleRole);
}

// Shortcut key support implementation
void FluentButton::setShortcut(const QKeySequence& shortcut) {
    if (m_shortcut != shortcut) {
        m_shortcut = shortcut;
        setToolTip(m_shortcut.isEmpty() ? QString() : m_shortcut.toString());
    }
}

void FluentButton::setShortcut(QKeySequence::StandardKey key) {
    setShortcut(QKeySequence(key));
}

// Enhanced accessibility methods
QString FluentButton::accessibleName() const {
    return m_accessibleName.isEmpty() ? (m_text.isEmpty() ? "Button" : m_text)
                                      : m_accessibleName;
}

void FluentButton::setAccessibleName(const QString& name) {
    if (m_accessibleName != name) {
        m_accessibleName = name;
        updateAccessibility();
    }
}

QString FluentButton::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentButton::setAccessibleDescription(const QString& description) {
    if (m_accessibleDescription != description) {
        m_accessibleDescription = description;
        updateAccessibility();
    }
}

QAccessible::Role FluentButton::accessibleRole() const {
    return m_accessibleRole;
}

void FluentButton::setAccessibleRole(QAccessible::Role role) {
    if (m_accessibleRole != role) {
        m_accessibleRole = role;
        updateAccessibility();
    }
}

// Static convenience methods
FluentButton* FluentButton::createPrimaryButton(const QString& text,
                                                QWidget* parent) {
    auto* button = new FluentButton(text, parent);
    button->setButtonStyle(FluentButtonStyle::Primary);
    return button;
}

FluentButton* FluentButton::createAccentButton(const QString& text,
                                               QWidget* parent) {
    auto* button = new FluentButton(text, parent);
    button->setButtonStyle(FluentButtonStyle::Accent);
    return button;
}

FluentButton* FluentButton::createSubtleButton(const QString& text,
                                               QWidget* parent) {
    auto* button = new FluentButton(text, parent);
    button->setButtonStyle(FluentButtonStyle::Subtle);
    return button;
}

FluentButton* FluentButton::createTextButton(const QString& text,
                                             QWidget* parent) {
    auto* button = new FluentButton(text, parent);
    button->setButtonStyle(FluentButtonStyle::Text);
    return button;
}

FluentButton* FluentButton::createOutlineButton(const QString& text,
                                                QWidget* parent) {
    auto* button = new FluentButton(text, parent);
    button->setButtonStyle(FluentButtonStyle::Outline);
    return button;
}

FluentButton* FluentButton::createIconButton(const QIcon& icon,
                                             QWidget* parent) {
    auto* button = new FluentButton(parent);
    button->setIcon(icon);
    button->setButtonStyle(FluentButtonStyle::Icon);
    button->setFlat(true);
    return button;
}

FluentButton* FluentButton::createToggleButton(const QString& text,
                                               QWidget* parent) {
    auto* button = new FluentButton(text, parent);
    button->setButtonStyle(FluentButtonStyle::Toggle);
    button->setCheckable(true);
    return button;
}

FluentButton* FluentButton::createSplitButton(const QString& text,
                                              const QIcon& dropdownIcon,
                                              QWidget* parent) {
    auto* button = new FluentButton(text, parent);
    button->setButtonStyle(FluentButtonStyle::Split);
    if (!dropdownIcon.isNull()) {
        button->setIcon(dropdownIcon);
        button->setIconPosition(FluentIconPosition::Right);
    }
    return button;
}

// Cache management methods
void FluentButton::invalidateCache(FluentButtonDirtyRegions regions) const {
    // If no regions specified, invalidate all
    if (regions == FluentButtonDirtyRegions{}) {
        regions = FluentButtonDirtyRegion::All;
    }

    if (regions & FluentButtonDirtyRegion::Background) {
        m_backgroundCacheValid = false;
    }
    if (regions & FluentButtonDirtyRegion::Border) {
        m_borderCacheValid = false;
    }
    if (regions & FluentButtonDirtyRegion::Content) {
        m_contentCacheValid = false;
    }
    m_dirtyRegions |= regions;
}

QString FluentButton::generateStyleKey() const {
    // Generate a unique key based on current style state
    return QString("%1_%2_%3_%4_%5_%6_%7_%8_%9")
        .arg(static_cast<int>(m_buttonStyle))
        .arg(static_cast<int>(m_buttonSize))
        .arg(static_cast<int>(state()))
        .arg(m_flat ? 1 : 0)
        .arg(m_checked ? 1 : 0)
        .arg(isEnabled() ? 1 : 0)
        .arg(m_backgroundOpacity, 0, 'f', 2)
        .arg(rect().width())
        .arg(rect().height());
}

void FluentButton::updateCacheIfNeeded() const {
    const QRect currentRect = rect().adjusted(0, 0, -1, -1);
    const QString currentStyleKey = generateStyleKey();

    // Check if cache needs to be invalidated due to size or style changes
    if (m_cachedRect != currentRect || m_cachedStyleKey != currentStyleKey) {
        invalidateCache(FluentButtonDirtyRegion::All);
        m_cachedRect = currentRect;
        m_cachedStyleKey = currentStyleKey;
    }

    // Update background cache if needed
    if (!m_backgroundCacheValid &&
        m_dirtyRegions & FluentButtonDirtyRegion::Background) {
        if (currentRect.isValid() && !currentRect.isEmpty()) {
            m_cachedBackground = QPixmap(currentRect.size());
            m_cachedBackground.fill(Qt::transparent);

            QPainter painter(&m_cachedBackground);
            painter.setRenderHint(QPainter::Antialiasing);
            const_cast<FluentButton*>(this)->paintBackground(
                painter,
                QRect(0, 0, currentRect.width(), currentRect.height()));

            m_backgroundCacheValid = true;
            m_dirtyRegions &=
                ~FluentButtonDirtyRegions(FluentButtonDirtyRegion::Background);
        }
    }

    // Update border cache if needed
    if (!m_borderCacheValid &&
        m_dirtyRegions & FluentButtonDirtyRegion::Border) {
        if (currentRect.isValid() && !currentRect.isEmpty()) {
            m_cachedBorder = QPixmap(currentRect.size());
            m_cachedBorder.fill(Qt::transparent);

            QPainter painter(&m_cachedBorder);
            painter.setRenderHint(QPainter::Antialiasing);
            const_cast<FluentButton*>(this)->paintBorder(
                painter,
                QRect(0, 0, currentRect.width(), currentRect.height()));

            m_borderCacheValid = true;
            m_dirtyRegions &=
                ~FluentButtonDirtyRegions(FluentButtonDirtyRegion::Border);
        }
    }

    // Update content cache if needed
    if (!m_contentCacheValid &&
        m_dirtyRegions & FluentButtonDirtyRegion::Content) {
        if (currentRect.isValid() && !currentRect.isEmpty()) {
            m_cachedContent = QPixmap(currentRect.size());
            m_cachedContent.fill(Qt::transparent);

            QPainter painter(&m_cachedContent);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::TextAntialiasing);
            const_cast<FluentButton*>(this)->paintContent(
                painter,
                QRect(0, 0, currentRect.width(), currentRect.height()));

            m_contentCacheValid = true;
            m_dirtyRegions &=
                ~FluentButtonDirtyRegions(FluentButtonDirtyRegion::Content);
        }
    }
}

// Design token integration methods
void FluentButton::setUseDesignTokens(bool use) {
    if (m_useDesignTokens != use) {
        m_useDesignTokens = use;
        if (use) {
            refreshFromDesignTokens();
        }
        invalidateCache(FluentButtonDirtyRegion::All);
        updateStateStyle();
    }
}

void FluentButton::refreshFromDesignTokens() {
    if (m_useDesignTokens) {
        // Invalidate size hint cache to recalculate with new tokens
        m_sizeHintValid = false;
        invalidateCache(FluentButtonDirtyRegion::All);
        updateGeometry();
        update();
    }
}

QColor FluentButton::getTokenColor(const QString& tokenName) const {
    return Styling::FluentDesignTokenUtils::instance().getColor(tokenName);
}

QFont FluentButton::getTokenFont(const QString& tokenName) const {
    return Styling::FluentDesignTokenUtils::instance().getFont(tokenName);
}

int FluentButton::getTokenSize(const QString& tokenName) const {
    return Styling::FluentDesignTokenUtils::instance().getSize(tokenName);
}

int FluentButton::getTokenSpacing(const QString& tokenName) const {
    return Styling::FluentDesignTokenUtils::instance().getSpacing(tokenName);
}

}  // namespace FluentQt::Components
