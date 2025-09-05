// src/Components/FluentBadge.cpp
#include "FluentQt/Components/FluentBadge.h"
#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentBadge::FluentBadge(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    m_pulseTimer = new QTimer(this);
    setupAnimations();
    updateSizeMetrics();
    setVisible(false);  // Hidden by default

    // Enable mouse tracking for hover effects
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentBadge::onThemeChanged);

    connect(m_pulseTimer, &QTimer::timeout, this, &FluentBadge::onPulseTimer);

    updateColors();
    updateVisibility();
    updateAccessibility();
}

FluentBadge::FluentBadge(FluentBadgeType type, QWidget* parent)
    : FluentBadge(parent) {
    setBadgeType(type);
}

FluentBadge::FluentBadge(const QString& text, QWidget* parent)
    : FluentBadge(parent) {
    setText(text);
    setBadgeType(FluentBadgeType::Text);
}

FluentBadge::FluentBadge(int count, QWidget* parent) : FluentBadge(parent) {
    setCount(count);
    setBadgeType(FluentBadgeType::Count);
}

FluentBadge::FluentBadge(const QIcon& icon, QWidget* parent)
    : FluentBadge(parent) {
    setIcon(icon);
    setBadgeType(FluentBadgeType::Icon);
}

FluentBadge::~FluentBadge() = default;

QString FluentBadge::text() const { return m_text; }

void FluentBadge::setText(const QString& text) {
    if (m_text == text)
        return;

    m_text = text;
    updateContent();
    updateGeometry();
    update();
    emit textChanged(m_text);
    updateAccessibility();
}

int FluentBadge::count() const { return m_count; }

void FluentBadge::setCount(int count) {
    if (m_count == count)
        return;

    m_count = qMax(0, count);
    updateContent();
    updateVisibility();
    updateGeometry();
    update();
    emit countChanged(m_count);
    updateAccessibility();
}

QIcon FluentBadge::icon() const { return m_icon; }

void FluentBadge::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() == icon.cacheKey())
        return;

    m_icon = icon;
    updateContent();
    updateGeometry();
    update();
    emit iconChanged(m_icon);
}

FluentBadgeType FluentBadge::badgeType() const { return m_badgeType; }

void FluentBadge::setBadgeType(FluentBadgeType type) {
    if (m_badgeType == type)
        return;

    m_badgeType = type;
    updateContent();
    updateSizeMetrics();
    updateVisibility();
    updateGeometry();
    update();
    emit badgeTypeChanged(m_badgeType);
}

FluentBadgeSize FluentBadge::badgeSize() const { return m_badgeSize; }

void FluentBadge::setBadgeSize(FluentBadgeSize size) {
    if (m_badgeSize == size)
        return;

    m_badgeSize = size;
    updateSizeMetrics();
    updateGeometry();
    update();
    emit badgeSizeChanged(m_badgeSize);
}

FluentBadgePosition FluentBadge::position() const { return m_position; }

void FluentBadge::setPosition(FluentBadgePosition position) {
    if (m_position == position)
        return;

    m_position = position;
    updatePosition();
    emit positionChanged(m_position);
}

FluentBadgeStatus FluentBadge::status() const { return m_status; }

void FluentBadge::setStatus(FluentBadgeStatus status) {
    if (m_status == status)
        return;

    m_status = status;
    updateColors();
    update();
    emit statusChanged(m_status);
}

QColor FluentBadge::getBackgroundColor() const { return m_backgroundColor; }

QColor FluentBadge::getTextColor() const { return m_textColor; }

QColor FluentBadge::customColor() const { return m_customColor; }

void FluentBadge::setCustomColor(const QColor& color) {
    if (m_customColor != color) {
        m_customColor = color;
        emit customColorChanged(color);
        update();
    }
}

FluentBadgeStyle FluentBadge::badgeStyle() const { return m_badgeStyle; }

void FluentBadge::setBadgeStyle(FluentBadgeStyle style) {
    if (m_badgeStyle != style) {
        m_badgeStyle = style;
        emit badgeStyleChanged(style);
        update();
    }
}

bool FluentBadge::isVisible() const { return QWidget::isVisible(); }

void FluentBadge::setVisible(bool visible) {
    QWidget::setVisible(visible);
    emit visibilityChanged(visible);
}

bool FluentBadge::showZero() const { return m_showZero; }

void FluentBadge::setShowZero(bool show) {
    if (m_showZero == show)
        return;

    m_showZero = show;
    updateVisibility();
    emit showZeroChanged(m_showZero);
}

int FluentBadge::maxCount() const { return m_maxCount; }

void FluentBadge::setMaxCount(int maxCount) {
    if (m_maxCount == maxCount)
        return;

    m_maxCount = qMax(0, maxCount);
    updateContent();
    update();
    emit maxCountChanged(m_maxCount);
}

bool FluentBadge::isPulsing() const { return m_pulsing; }

void FluentBadge::setPulsing(bool pulsing) {
    if (m_pulsing == pulsing)
        return;

    m_pulsing = pulsing;

    if (m_pulsing) {
        startPulseAnimation();
    } else {
        if (m_pulseAnimation) {
            m_pulseAnimation->stop();
        }
        if (m_pulseTimer) {
            m_pulseTimer->stop();
        }
        m_pulseScale = 1.0;
        update();
    }

    emit pulsingChanged(m_pulsing);
}

bool FluentBadge::isAnimated() const { return m_animated; }

void FluentBadge::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

QString FluentBadge::displayText() const {
    switch (m_badgeType) {
        case FluentBadgeType::Dot:
            return QString();

        case FluentBadgeType::Count:
            if (m_count == 0 && !m_showZero) {
                return QString();
            }
            if (m_maxCount > 0 && m_count > m_maxCount) {
                return QString("%1+").arg(m_maxCount);
            }
            return QString::number(m_count);

        case FluentBadgeType::Text:
            return m_text;

        case FluentBadgeType::Icon:
            return QString();

        case FluentBadgeType::Status:
            return QString();
    }

    return QString();
}

bool FluentBadge::isEmpty() const {
    switch (m_badgeType) {
        case FluentBadgeType::Dot:
        case FluentBadgeType::Status:
            return false;

        case FluentBadgeType::Count:
            return m_count == 0 && !m_showZero;

        case FluentBadgeType::Text:
            return m_text.isEmpty();

        case FluentBadgeType::Icon:
            return m_icon.isNull();
    }

    return true;
}

QSize FluentBadge::contentSize() const {
    const QString text = displayText();

    if (text.isEmpty() && m_badgeType != FluentBadgeType::Dot &&
        m_badgeType != FluentBadgeType::Icon &&
        m_badgeType != FluentBadgeType::Status) {
        return QSize(0, 0);
    }

    switch (m_badgeType) {
        case FluentBadgeType::Dot:
        case FluentBadgeType::Status:
            return QSize(m_dotSize, m_dotSize);

        case FluentBadgeType::Icon:
            return QSize(m_iconSize, m_iconSize);

        case FluentBadgeType::Count:
        case FluentBadgeType::Text: {
            if (text.isEmpty()) {
                return QSize(0, 0);
            }

            const QFontMetrics fm(font());
            const QSize textSize = fm.size(Qt::TextSingleLine, text);
            const int width =
                qMax(textSize.width() + m_padding * 2, m_minWidth);
            const int height = textSize.height() + m_padding * 2;
            return QSize(width, height);
        }
    }

    return QSize(0, 0);
}

QSize FluentBadge::sizeHint() const { return contentSize(); }

QRect FluentBadge::badgeRect() const {
    // For now, return current geometry as the badge rect
    return QRect(QPoint(0, 0), sizeHint());
}

QSize FluentBadge::minimumSizeHint() const {
    switch (m_badgeType) {
        case FluentBadgeType::Dot:
        case FluentBadgeType::Status:
            return QSize(m_dotSize, m_dotSize);

        case FluentBadgeType::Icon:
            return QSize(m_iconSize, m_iconSize);

        case FluentBadgeType::Count:
        case FluentBadgeType::Text:
            return QSize(m_minWidth, m_minHeight);
    }

    return QSize(0, 0);
}

void FluentBadge::attachTo(QWidget* widget, FluentBadgePosition position) {
    if (m_attachedWidget == widget)
        return;

    if (m_attachedWidget) {
        m_attachedWidget->removeEventFilter(this);
    }

    m_attachedWidget = widget;
    setPosition(position);

    if (m_attachedWidget) {
        setParent(m_attachedWidget);
        m_attachedWidget->installEventFilter(this);
        updatePosition();
        raise();
    }

    emit attachedWidgetChanged(m_attachedWidget);
}

void FluentBadge::detach() { attachTo(nullptr); }

FluentBadge* FluentBadge::createDotBadge(QWidget* parent) {
    auto* badge = new FluentBadge(FluentBadgeType::Dot, parent);
    return badge;
}

FluentBadge* FluentBadge::createCountBadge(int count, QWidget* parent) {
    auto* badge = new FluentBadge(count, parent);
    return badge;
}

FluentBadge* FluentBadge::createTextBadge(const QString& text,
                                          QWidget* parent) {
    auto* badge = new FluentBadge(text, parent);
    return badge;
}

FluentBadge* FluentBadge::createStatusBadge(FluentBadgeStatus status,
                                            QWidget* parent) {
    auto* badge = new FluentBadge(FluentBadgeType::Status, parent);
    badge->setStatus(status);
    return badge;
}

void FluentBadge::show() {
    if (m_animated) {
        animateIn();
    } else {
        setVisible(true);
    }
}

void FluentBadge::hide() {
    if (m_animated) {
        animateOut();
    } else {
        setVisible(false);
    }
}

void FluentBadge::increment() { setCount(m_count + 1); }

void FluentBadge::decrement() { setCount(qMax(0, m_count - 1)); }

void FluentBadge::reset() { setCount(0); }

void FluentBadge::pulse() {
    if (m_animated) {
        startSinglePulse();
    }
}

void FluentBadge::setupAnimations() {
    // Show/hide animation
    m_showAnimation = new QPropertyAnimation(this, "badgeOpacity", this);
    m_showAnimation->setDuration(200);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_showAnimation, &QPropertyAnimation::finished, this,
            &FluentBadge::onShowAnimationFinished);

    // Scale animation for show/hide
    m_scaleAnimation = new QPropertyAnimation(this, "badgeScale", this);
    m_scaleAnimation->setDuration(200);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutBack);

    // Pulse animation
    m_pulseAnimation = new QPropertyAnimation(this, "pulseScale", this);
    m_pulseAnimation->setDuration(1000);
    m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
    m_pulseAnimation->setLoopCount(-1);  // Infinite loop
    connect(m_pulseAnimation, &QPropertyAnimation::finished, this,
            &FluentBadge::onPulseAnimationFinished);

    // Color transition animation for smooth style changes
    m_colorAnimation = new QPropertyAnimation(this, "geometry", this);
    m_colorAnimation->setDuration(150);
    m_colorAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_colorAnimation, &QPropertyAnimation::finished, this, [this]() {
        update();  // Ensure final color state is painted
    });
}

void FluentBadge::updateSizeMetrics() {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    switch (m_badgeSize) {
        case FluentBadgeSize::Small:
            m_dotSize = tokenUtils.getSpacing("xs");    // 4px * 2 = 8px
            m_iconSize = tokenUtils.getSpacing("m");    // 12px
            m_minWidth = tokenUtils.getSpacing("xl");   // 20px -> 16px
            m_minHeight = tokenUtils.getSpacing("xl");  // 20px -> 16px
            m_padding = tokenUtils.getSpacing("xs");    // 4px
            break;

        case FluentBadgeSize::Medium:
            m_dotSize = tokenUtils.getSpacing("s") + 2;  // 8px + 2 = 10px
            m_iconSize = tokenUtils.getSpacing("xl");    // 20px -> 16px
            m_minWidth = tokenUtils.getSpacing("xl");    // 20px
            m_minHeight = tokenUtils.getSpacing("xl");   // 20px
            m_padding = tokenUtils.getSpacing("s") - 2;  // 8px - 2 = 6px
            break;

        case FluentBadgeSize::Large:
            m_dotSize = tokenUtils.getSpacing("m");      // 12px
            m_iconSize = tokenUtils.getSpacing("xl");    // 20px
            m_minWidth = tokenUtils.getSpacing("xxl");   // 24px
            m_minHeight = tokenUtils.getSpacing("xxl");  // 24px
            m_padding = tokenUtils.getSpacing("s");      // 8px
            break;
    }
}

void FluentBadge::updateContent() {
    // Content is updated in displayText() method
    updateAccessibility();
}

void FluentBadge::updateVisibility() {
    if (m_autoHide && isEmpty()) {
        if (isVisible()) {
            hide();
        }
    } else {
        if (!isVisible() && !isEmpty()) {
            show();
        }
    }
}

void FluentBadge::updatePosition() {
    if (!m_attachedWidget)
        return;

    const QSize badgeSize = sizeHint();
    const QRect parentRect = m_attachedWidget->rect();
    QPoint pos;

    switch (m_position) {
        case FluentBadgePosition::TopLeft:
            pos = QPoint(-badgeSize.width() / 2, -badgeSize.height() / 2);
            break;

        case FluentBadgePosition::TopRight:
            pos = QPoint(parentRect.width() - badgeSize.width() / 2,
                         -badgeSize.height() / 2);
            break;

        case FluentBadgePosition::BottomLeft:
            pos = QPoint(-badgeSize.width() / 2,
                         parentRect.height() - badgeSize.height() / 2);
            break;

        case FluentBadgePosition::BottomRight:
            pos = QPoint(parentRect.width() - badgeSize.width() / 2,
                         parentRect.height() - badgeSize.height() / 2);
            break;

        case FluentBadgePosition::Center:
            pos = QPoint(parentRect.width() / 2 - badgeSize.width() / 2,
                         parentRect.height() / 2 - badgeSize.height() / 2);
            break;

        case FluentBadgePosition::Inline:
            // For inline badges, position at origin (will be handled by layout)
            pos = QPoint(0, 0);
            break;
    }

    pos += m_offset;
    setGeometry(pos.x(), pos.y(), badgeSize.width(), badgeSize.height());
}

void FluentBadge::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    if (isEmpty())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Apply transformations
    const QPointF center = rect().center();
    painter.translate(center);
    painter.scale(m_badgeScale * m_pulseScale, m_badgeScale * m_pulseScale);
    painter.translate(-center);

    // Set opacity
    painter.setOpacity(m_badgeOpacity);

    const QRect paintRect = rect();

    // Paint background based on style
    paintBackground(&painter, paintRect);

    // Paint border for outline and ghost styles
    paintBorder(&painter, paintRect);

    // Paint content (text, icon, etc.)
    paintContent(&painter, paintRect);
}

bool FluentBadge::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_attachedWidget) {
        switch (event->type()) {
            case QEvent::Resize:
            case QEvent::Move:
                updatePosition();
                break;

            case QEvent::Show:
                if (!isEmpty()) {
                    show();
                }
                break;

            case QEvent::Hide:
                hide();
                break;

            default:
                break;
        }
    }

    return Core::FluentComponent::eventFilter(watched, event);
}

void FluentBadge::onShowAnimationFinished() {
    if (m_badgeOpacity == 0.0) {
        setVisible(false);
    }
}

void FluentBadge::onPulseAnimationFinished() {
    // Pulse animation loops infinitely, so this shouldn't be called
    // unless we stop the animation
}

void FluentBadge::onPulseTimer() {
    if (m_pulsing && m_pulseAnimation &&
        m_pulseAnimation->state() != QAbstractAnimation::Running) {
        startPulseAnimation();
    }
}

void FluentBadge::onThemeChanged() {
    // Update colors when theme changes
    updateColors();

    // Update size metrics in case spacing tokens changed
    updateSizeMetrics();

    // Update geometry to reflect new size metrics
    updateGeometry();

    // Force repaint with new theme
    update();
}

void FluentBadge::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (!m_hasCustomBackgroundColor) {
        // Check for high contrast mode
        bool isHighContrast = theme.isHighContrastMode();

        if (isHighContrast) {
            // Use high contrast colors for accessibility
            m_backgroundColor = palette.highContrastBackground;
        } else {
            switch (m_status) {
                case FluentBadgeStatus::None:
                    m_backgroundColor = palette.accent;
                    break;
                case FluentBadgeStatus::Success:
                    m_backgroundColor = palette.success;
                    break;
                case FluentBadgeStatus::Warning:
                    m_backgroundColor = palette.warning;
                    break;
                case FluentBadgeStatus::Error:
                    m_backgroundColor = palette.error;
                    break;
                case FluentBadgeStatus::Info:
                    m_backgroundColor = palette.info;
                    break;
                case FluentBadgeStatus::Neutral:
                    m_backgroundColor = palette.neutralSecondary;
                    break;
            }
        }
    }

    if (!m_hasCustomTextColor) {
        bool isHighContrast = theme.isHighContrastMode();

        if (isHighContrast) {
            m_textColor = palette.highContrastText;
        } else {
            // Use white text for filled badges, theme text color for others
            if (m_badgeStyle == FluentBadgeStyle::Filled) {
                m_textColor = Qt::white;
            } else {
                m_textColor = palette.neutralPrimary;
            }
        }
    }

    update();
}

void FluentBadge::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        QString accessibleName;
        QString accessibleDescription;
        QString statusText;

        // Add status information to accessibility text
        switch (m_status) {
            case FluentBadgeStatus::Success:
                statusText = tr("Success");
                break;
            case FluentBadgeStatus::Warning:
                statusText = tr("Warning");
                break;
            case FluentBadgeStatus::Error:
                statusText = tr("Error");
                break;
            case FluentBadgeStatus::Info:
                statusText = tr("Information");
                break;
            case FluentBadgeStatus::Neutral:
                statusText = tr("Neutral");
                break;
            case FluentBadgeStatus::None:
            default:
                statusText = QString();
                break;
        }

        switch (m_badgeType) {
            case FluentBadgeType::Count:
                if (m_count > 0) {
                    accessibleName = tr("%n notification(s)", "", m_count);
                    accessibleDescription =
                        statusText.isEmpty()
                            ? accessibleName
                            : tr("%1 %2").arg(statusText, accessibleName);
                } else if (m_showZero) {
                    accessibleName = tr("No notifications");
                    accessibleDescription =
                        statusText.isEmpty()
                            ? accessibleName
                            : tr("%1 %2").arg(statusText, accessibleName);
                }
                break;

            case FluentBadgeType::Text:
                accessibleName = m_text;
                accessibleDescription =
                    statusText.isEmpty()
                        ? tr("Text badge: %1").arg(m_text)
                        : tr("%1 text badge: %2").arg(statusText, m_text);
                break;

            case FluentBadgeType::Dot:
                accessibleName = tr("Indicator");
                accessibleDescription =
                    statusText.isEmpty()
                        ? tr("Status indicator")
                        : tr("%1 status indicator").arg(statusText);
                break;

            case FluentBadgeType::Status:
                accessibleName =
                    statusText.isEmpty() ? tr("Status") : statusText;
                accessibleDescription =
                    tr("Status indicator: %1").arg(accessibleName);
                break;

            case FluentBadgeType::Icon:
                accessibleName = tr("Icon badge");
                accessibleDescription =
                    statusText.isEmpty() ? tr("Icon badge")
                                         : tr("%1 icon badge").arg(statusText);
                break;
        }

        setAccessibleName(accessibleName);
        setAccessibleDescription(accessibleDescription);

        // Set focus policy for keyboard navigation
        if (m_badgeType == FluentBadgeType::Count || !m_text.isEmpty()) {
            setFocusPolicy(Qt::TabFocus);
        } else {
            setFocusPolicy(Qt::NoFocus);
        }

        QAccessibleEvent event(this, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void FluentBadge::paintDot(QPainter* painter) {
    const QRect rect = this->rect();

    // Use the new unified paint methods for consistency
    paintBackground(painter, rect);
    paintBorder(painter, rect);
}

void FluentBadge::paintTextBadge(QPainter* painter) {
    const QRect rect = this->rect();

    // Use the new unified paint methods for consistency
    paintBackground(painter, rect);
    paintBorder(painter, rect);
    paintContent(painter, rect);
}

void FluentBadge::paintIconBadge(QPainter* painter) {
    const QRect rect = this->rect();

    // Use the new unified paint methods for consistency
    paintBackground(painter, rect);
    paintBorder(painter, rect);
    paintContent(painter, rect);
}

// Missing paint methods implementation
void FluentBadge::paintBackground(QPainter* painter, const QRect& rect) {
    painter->save();
    painter->setPen(Qt::NoPen);

    QColor bgColor = getBackgroundColor();

    // Apply state-based color modifications
    if (!isEnabled()) {
        const auto& theme = Styling::FluentTheme::instance();
        const auto& palette = theme.currentPalette();
        bgColor = palette.disabled;
    } else if (underMouse() && m_badgeStyle != FluentBadgeStyle::Ghost) {
        // Slightly lighten on hover for filled badges
        if (m_badgeStyle == FluentBadgeStyle::Filled) {
            bgColor = bgColor.lighter(110);
        }
    }

    switch (m_badgeStyle) {
        case FluentBadgeStyle::Filled:
            painter->setBrush(bgColor);
            break;
        case FluentBadgeStyle::Outline:
            painter->setBrush(Qt::NoBrush);
            break;
        case FluentBadgeStyle::Subtle:
            bgColor.setAlphaF(0.1);
            painter->setBrush(bgColor);
            break;
        case FluentBadgeStyle::Ghost:
            if (underMouse()) {
                bgColor.setAlphaF(0.05);
                painter->setBrush(bgColor);
            } else {
                painter->setBrush(Qt::NoBrush);
            }
            break;
    }

    // Draw background based on badge type
    switch (m_badgeType) {
        case FluentBadgeType::Dot:
        case FluentBadgeType::Status:
        case FluentBadgeType::Icon:
            painter->drawEllipse(rect);
            break;
        case FluentBadgeType::Count:
        case FluentBadgeType::Text:
            painter->drawRoundedRect(rect, rect.height() / 2.0,
                                     rect.height() / 2.0);
            break;
    }

    painter->restore();
}

void FluentBadge::paintBorder(QPainter* painter, const QRect& rect) {
    if (m_badgeStyle != FluentBadgeStyle::Outline &&
        m_badgeStyle != FluentBadgeStyle::Ghost) {
        return;
    }

    painter->save();

    QColor borderColor = getBorderColor();
    int borderWidth = getBorderWidth();

    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(Qt::NoBrush);

    // Draw border based on badge type
    switch (m_badgeType) {
        case FluentBadgeType::Dot:
        case FluentBadgeType::Status:
        case FluentBadgeType::Icon:
            painter->drawEllipse(rect.adjusted(borderWidth / 2, borderWidth / 2,
                                               -borderWidth / 2,
                                               -borderWidth / 2));
            break;
        case FluentBadgeType::Count:
        case FluentBadgeType::Text: {
            QRect borderRect =
                rect.adjusted(borderWidth / 2, borderWidth / 2,
                              -borderWidth / 2, -borderWidth / 2);
            painter->drawRoundedRect(borderRect, borderRect.height() / 2.0,
                                     borderRect.height() / 2.0);
            break;
        }
    }

    painter->restore();
}

void FluentBadge::paintContent(QPainter* painter, const QRect& rect) {
    switch (m_badgeType) {
        case FluentBadgeType::Dot:
        case FluentBadgeType::Status:
            // No content for dot badges
            break;
        case FluentBadgeType::Count:
        case FluentBadgeType::Text:
            paintText(painter, rect);
            break;
        case FluentBadgeType::Icon:
            paintIcon(painter, rect);
            break;
    }
}

void FluentBadge::paintText(QPainter* painter, const QRect& rect) {
    const QString text = displayText();
    if (text.isEmpty()) {
        return;
    }

    painter->save();

    QColor textColor = getTextColor();

    // Apply state-based text color modifications
    if (!isEnabled()) {
        const auto& theme = Styling::FluentTheme::instance();
        const auto& palette = theme.currentPalette();
        textColor = palette.disabled;
    }

    painter->setPen(textColor);
    painter->setFont(getFont());
    painter->drawText(rect, Qt::AlignCenter, text);
    painter->restore();
}

void FluentBadge::paintIcon(QPainter* painter, const QRect& rect) {
    if (m_icon.isNull()) {
        return;
    }

    painter->save();

    const QSize iconSize(m_iconSize, m_iconSize);
    const QRect iconRect(rect.center().x() - iconSize.width() / 2,
                         rect.center().y() - iconSize.height() / 2,
                         iconSize.width(), iconSize.height());

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    QPixmap pixmap = m_icon.pixmap(iconSize, mode);
    painter->drawPixmap(iconRect, pixmap);

    painter->restore();
}

// Missing utility methods implementation
QColor FluentBadge::getBorderColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    switch (m_status) {
        case FluentBadgeStatus::Success:
            return palette.success;
        case FluentBadgeStatus::Warning:
            return palette.warning;
        case FluentBadgeStatus::Error:
            return palette.error;
        case FluentBadgeStatus::Info:
            return palette.info;
        case FluentBadgeStatus::Neutral:
            return palette.neutralSecondary;
        case FluentBadgeStatus::None:
        default:
            return palette.accent;
    }
}

QColor FluentBadge::getStatusColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    switch (m_status) {
        case FluentBadgeStatus::Success:
            return palette.success;
        case FluentBadgeStatus::Warning:
            return palette.warning;
        case FluentBadgeStatus::Error:
            return palette.error;
        case FluentBadgeStatus::Info:
            return palette.info;
        case FluentBadgeStatus::Neutral:
            return palette.neutralSecondary;
        case FluentBadgeStatus::None:
        default:
            return palette.accent;
    }
}

QFont FluentBadge::getFont() const {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Use FluentUI typography tokens based on badge size
    switch (m_badgeSize) {
        case FluentBadgeSize::Small:
            return tokenUtils.getCaptionFont(1);  // caption1: 12px
        case FluentBadgeSize::Medium:
            return tokenUtils.getCaptionFont(1);  // caption1: 12px
        case FluentBadgeSize::Large:
            return tokenUtils.getBodyFont(false);  // body1: 14px
    }

    return tokenUtils.getCaptionFont(1);
}

int FluentBadge::getBorderWidth() const {
    switch (m_badgeSize) {
        case FluentBadgeSize::Small:
            return 1;
        case FluentBadgeSize::Medium:
            return 1;
        case FluentBadgeSize::Large:
            return 2;
    }
    return 1;
}

int FluentBadge::getPadding() const { return m_padding; }

void FluentBadge::animateIn() {
    if (!m_showAnimation || !m_scaleAnimation)
        return;

    setVisible(true);

    // Animate opacity
    m_showAnimation->stop();
    m_showAnimation->setStartValue(m_badgeOpacity);
    m_showAnimation->setEndValue(1.0);
    m_showAnimation->start();

    // Animate scale
    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(0.0);
    m_scaleAnimation->setEndValue(1.0);
    m_scaleAnimation->start();
}

void FluentBadge::animateOut() {
    if (!m_showAnimation || !m_scaleAnimation)
        return;

    // Animate opacity
    m_showAnimation->stop();
    m_showAnimation->setStartValue(m_badgeOpacity);
    m_showAnimation->setEndValue(0.0);
    m_showAnimation->start();

    // Animate scale
    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(m_badgeScale);
    m_scaleAnimation->setEndValue(0.0);
    m_scaleAnimation->start();
}

void FluentBadge::startPulseAnimation() {
    if (!m_pulseAnimation || !m_animated)
        return;

    // Stop any existing pulse animation to prevent conflicts
    m_pulseAnimation->stop();

    // Optimize animation by using smaller scale change for better performance
    m_pulseAnimation->setStartValue(1.0);
    m_pulseAnimation->setEndValue(
        1.15);  // Reduced from 1.2 for smoother animation
    m_pulseAnimation->setDuration(
        800);  // Slightly faster for better responsiveness
    m_pulseAnimation->start();

    if (m_pulsing) {
        m_pulseTimer->start(1800);  // Slightly faster pulse interval
    }
}

void FluentBadge::startSinglePulse() {
    if (!m_pulseAnimation || !m_animated)
        return;

    // Stop any existing animation
    m_pulseAnimation->stop();

    // Configure for single pulse with optimized values
    m_pulseAnimation->setLoopCount(1);
    m_pulseAnimation->setStartValue(1.0);
    m_pulseAnimation->setEndValue(
        1.2);  // Reduced from 1.3 for better performance
    m_pulseAnimation->setDuration(600);  // Faster single pulse
    m_pulseAnimation->start();

    // Restore infinite loop after single pulse
    QTimer::singleShot(m_pulseAnimation->duration(), this, [this]() {
        if (m_pulseAnimation) {
            m_pulseAnimation->setLoopCount(-1);
            m_pulseAnimation->setDuration(800);  // Restore optimized duration
        }
    });
}

// Property accessors for animations
void FluentBadge::setBadgeOpacity(qreal opacity) {
    if (qFuzzyCompare(m_badgeOpacity, opacity))
        return;
    m_badgeOpacity = opacity;
    update();
}

void FluentBadge::setBadgeScale(qreal scale) {
    if (qFuzzyCompare(m_badgeScale, scale))
        return;
    m_badgeScale = scale;
    update();
}

void FluentBadge::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    emit clicked();
}

void FluentBadge::mouseDoubleClickEvent(QMouseEvent* event) {
    QWidget::mouseDoubleClickEvent(event);
    emit doubleClicked();
}

void FluentBadge::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updatePosition();
}

void FluentBadge::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    updatePosition();
}

void FluentBadge::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    if (isEnabled()) {
        update();  // Trigger repaint for hover state
    }
}

void FluentBadge::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    if (isEnabled()) {
        update();  // Trigger repaint to remove hover state
    }
}

void FluentBadge::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
    update();  // Trigger repaint for focus state
}

void FluentBadge::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
    update();  // Trigger repaint to remove focus state
}

void FluentBadge::onHideAnimationFinished() { QWidget::hide(); }

void FluentBadge::stopPulsing() { setPulsing(false); }

qreal FluentBadge::pulseScale() const { return m_pulseScale; }

void FluentBadge::setPulseScale(qreal scale) {
    if (qFuzzyCompare(m_pulseScale, scale))
        return;
    m_pulseScale = scale;
    update();
}

}  // namespace FluentQt::Components
