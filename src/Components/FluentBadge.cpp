// src/Components/FluentBadge.cpp
#include "FluentQt/Components/FluentBadge.h"
#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentBadge::FluentBadge(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    m_pulseTimer = new QTimer(this);
    setupAnimations();
    updateSizeMetrics();
    setVisible(false);  // Hidden by default
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentBadge::updateColors);

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
}

void FluentBadge::updateSizeMetrics() {
    switch (m_badgeSize) {
        case FluentBadgeSize::Small:
            m_dotSize = 8;
            m_iconSize = 12;
            m_minWidth = 16;
            m_minHeight = 16;
            m_padding = 4;
            break;

        case FluentBadgeSize::Medium:
            m_dotSize = 10;
            m_iconSize = 16;
            m_minWidth = 20;
            m_minHeight = 20;
            m_padding = 6;
            break;

        case FluentBadgeSize::Large:
            m_dotSize = 12;
            m_iconSize = 20;
            m_minWidth = 24;
            m_minHeight = 24;
            m_padding = 8;
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

    // Paint badge based on type
    switch (m_badgeType) {
        case FluentBadgeType::Dot:
        case FluentBadgeType::Status:
            paintDot(&painter);
            break;

        case FluentBadgeType::Count:
        case FluentBadgeType::Text:
            paintTextBadge(&painter);
            break;

        case FluentBadgeType::Icon:
            paintIconBadge(&painter);
            break;
    }
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

void FluentBadge::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    if (!m_hasCustomBackgroundColor) {
        switch (m_status) {
            case FluentBadgeStatus::None:
                m_backgroundColor = theme.color("accent");
                break;
            case FluentBadgeStatus::Success:
                m_backgroundColor = QColor(16, 124, 16);  // Green
                break;
            case FluentBadgeStatus::Warning:
                m_backgroundColor = QColor(255, 140, 0);  // Orange
                break;
            case FluentBadgeStatus::Error:
                m_backgroundColor = QColor(196, 43, 28);  // Red
                break;
            case FluentBadgeStatus::Info:
                m_backgroundColor = theme.color("accent");
                break;
            case FluentBadgeStatus::Neutral:
                m_backgroundColor = theme.color("neutralSecondary");
                break;
        }
    }

    if (!m_hasCustomTextColor) {
        m_textColor = theme.color("textPrimary");
    }

    update();
}

void FluentBadge::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        QString accessibleText;

        switch (m_badgeType) {
            case FluentBadgeType::Count:
                if (m_count > 0) {
                    accessibleText = tr("%n notification(s)", "", m_count);
                }
                break;

            case FluentBadgeType::Text:
                accessibleText = m_text;
                break;

            case FluentBadgeType::Dot:
            case FluentBadgeType::Status:
                accessibleText = tr("Status indicator");
                break;

            case FluentBadgeType::Icon:
                accessibleText = tr("Icon badge");
                break;
        }

        setAccessibleName(accessibleText);
        setAccessibleDescription(accessibleText);

        QAccessibleEvent event(this, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void FluentBadge::paintDot(QPainter* painter) {
    const QRect rect = this->rect();
    const QPointF center = rect.center();
    const qreal radius = m_dotSize / 2.0;

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_backgroundColor);
    painter->drawEllipse(center, radius, radius);
}

void FluentBadge::paintTextBadge(QPainter* painter) {
    const QRect rect = this->rect();
    const QString text = displayText();

    if (text.isEmpty())
        return;

    // Draw background
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_backgroundColor);
    painter->drawRoundedRect(rect, rect.height() / 2.0, rect.height() / 2.0);

    // Draw text
    painter->setPen(m_textColor);
    painter->setFont(font());
    painter->drawText(rect, Qt::AlignCenter, text);
}

void FluentBadge::paintIconBadge(QPainter* painter) {
    if (m_icon.isNull())
        return;

    const QRect rect = this->rect();

    // Draw background circle
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_backgroundColor);
    painter->drawEllipse(rect);

    // Draw icon
    const QSize iconSize(m_iconSize, m_iconSize);
    const QRect iconRect(rect.center().x() - iconSize.width() / 2,
                         rect.center().y() - iconSize.height() / 2,
                         iconSize.width(), iconSize.height());

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    QPixmap pixmap = m_icon.pixmap(iconSize, mode);
    painter->drawPixmap(iconRect, pixmap);
}

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
    if (!m_pulseAnimation)
        return;

    m_pulseAnimation->stop();
    m_pulseAnimation->setStartValue(1.0);
    m_pulseAnimation->setEndValue(1.2);
    m_pulseAnimation->start();

    if (m_pulsing) {
        m_pulseTimer->start(2000);  // Pulse every 2 seconds
    }
}

void FluentBadge::startSinglePulse() {
    if (!m_pulseAnimation)
        return;

    m_pulseAnimation->stop();
    m_pulseAnimation->setLoopCount(1);
    m_pulseAnimation->setStartValue(1.0);
    m_pulseAnimation->setEndValue(1.3);
    m_pulseAnimation->start();

    // Restore infinite loop after single pulse
    QTimer::singleShot(m_pulseAnimation->duration(), this, [this]() {
        if (m_pulseAnimation) {
            m_pulseAnimation->setLoopCount(-1);
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
