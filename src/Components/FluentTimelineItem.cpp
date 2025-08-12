// src/Components/FluentTimelineItem.cpp
#include "FluentQt/Components/FluentTimelineItem.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QVBoxLayout>

using namespace FluentQt::Components;
using namespace FluentQt::Core;
using namespace FluentQt::Styling;

FluentTimelineItem::FluentTimelineItem(QWidget* parent)
    : FluentComponent(parent) {
    setupLayout();
    setupAnimations();

    // Connect to theme changes
    connect(&FluentTheme::instance(), &FluentTheme::themeChanged, this,
            &FluentTimelineItem::onThemeChanged);

    updateColors();
    updateAccessibility();

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setCursor(Qt::PointingHandCursor);
}

FluentTimelineItem::FluentTimelineItem(const QString& title, QWidget* parent)
    : FluentTimelineItem(parent) {
    setTitle(title);
}

FluentTimelineItem::FluentTimelineItem(const QString& title,
                                       const QString& description,
                                       QWidget* parent)
    : FluentTimelineItem(parent) {
    setTitle(title);
    setDescription(description);
}

FluentTimelineItem::FluentTimelineItem(const FluentTimelineItemData& data,
                                       QWidget* parent)
    : FluentTimelineItem(parent) {
    setData(data);
}

FluentTimelineItem::~FluentTimelineItem() = default;

void FluentTimelineItem::setTitle(const QString& title) {
    if (m_data.title != title) {
        m_data.title = title;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        updateAccessibility();
        emit titleChanged(title);
    }
}

void FluentTimelineItem::setDescription(const QString& description) {
    if (m_data.description != description) {
        m_data.description = description;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit descriptionChanged(description);
    }
}

void FluentTimelineItem::setSubtitle(const QString& subtitle) {
    if (m_data.subtitle != subtitle) {
        m_data.subtitle = subtitle;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit subtitleChanged(subtitle);
    }
}

void FluentTimelineItem::setDateTime(const QDateTime& dateTime) {
    if (m_data.dateTime != dateTime) {
        m_data.dateTime = dateTime;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit dateTimeChanged(dateTime);
    }
}

void FluentTimelineItem::setIcon(const QIcon& icon) {
    if (m_data.icon.cacheKey() != icon.cacheKey()) {
        m_data.icon = icon;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit iconChanged(icon);
    }
}

void FluentTimelineItem::setAvatar(const QPixmap& avatar) {
    if (m_data.avatar.cacheKey() != avatar.cacheKey()) {
        m_data.avatar = avatar;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit avatarChanged(avatar);
    }
}

void FluentTimelineItem::setUserData(const QVariant& data) {
    m_data.userData = data;
}

void FluentTimelineItem::setItemState(FluentTimelineItemState state) {
    if (m_data.state != state) {
        // Store old state for potential future use (e.g., transition
        // animations)
        [[maybe_unused]] const FluentTimelineItemState oldState = m_data.state;
        m_data.state = state;

        updateColors();

        if (isVisible()) {
            animateStateChange(state);
        }

        update();
        emit itemStateChanged(state);
        emit stateChanged(state);
    }
}

void FluentTimelineItem::setItemType(FluentTimelineItemType type) {
    if (m_data.type != type) {
        m_data.type = type;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit itemTypeChanged(type);
    }
}

void FluentTimelineItem::setItemSize(FluentTimelineItemSize size) {
    if (m_data.size != size) {
        m_data.size = size;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit itemSizeChanged(size);
    }
}

void FluentTimelineItem::setIndicatorStyle(
    FluentTimelineItemIndicatorStyle style) {
    if (m_data.indicatorStyle != style) {
        m_data.indicatorStyle = style;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit indicatorStyleChanged(style);
    }
}

void FluentTimelineItem::setExpandable(bool expandable) {
    if (m_data.expandable != expandable) {
        m_data.expandable = expandable;
        updateAccessibility();
        emit expandableChanged(expandable);
    }
}

void FluentTimelineItem::setExpanded(bool expanded) {
    if (m_data.expanded != expanded) {
        m_data.expanded = expanded;

        if (isVisible()) {
            animateExpansion(expanded);
        } else {
            m_expansionProgress = expanded ? 1.0 : 0.0;
        }

        updateLayout();
        emit expandedChanged(expanded);

        if (expanded) {
            emit this->expanded();
        } else {
            emit collapsed();
        }
    }
}

void FluentTimelineItem::setInteractive(bool interactive) {
    if (m_data.interactive != interactive) {
        m_data.interactive = interactive;

        setCursor(interactive ? Qt::PointingHandCursor : Qt::ArrowCursor);
        setFocusPolicy(interactive ? Qt::StrongFocus : Qt::NoFocus);

        emit interactiveChanged(interactive);
    }
}

void FluentTimelineItem::setShowDateTime(bool show) {
    if (m_data.showDateTime != show) {
        m_data.showDateTime = show;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit showDateTimeChanged(show);
    }
}

void FluentTimelineItem::setShowIndicator(bool show) {
    if (m_data.showIndicator != show) {
        m_data.showIndicator = show;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit showIndicatorChanged(show);
    }
}

void FluentTimelineItem::setCustomColor(const QColor& color) {
    if (m_data.customColor != color) {
        m_data.customColor = color;
        updateColors();
        update();
    }
}

void FluentTimelineItem::setCustomText(const QString& text) {
    if (m_data.customText != text) {
        m_data.customText = text;
        m_sizeHintValid = false;
        updateGeometry();
        update();
    }
}

void FluentTimelineItem::setCustomIcon(const QIcon& icon) {
    if (m_data.customIcon.cacheKey() != icon.cacheKey()) {
        m_data.customIcon = icon;
        m_sizeHintValid = false;
        updateGeometry();
        update();
    }
}

void FluentTimelineItem::setData(const FluentTimelineItemData& data) {
    const bool hasTitleChanged = (m_data.title != data.title);
    const bool hasDescriptionChanged = (m_data.description != data.description);
    const bool hasSubtitleChanged = (m_data.subtitle != data.subtitle);
    const bool hasDateTimeChanged = (m_data.dateTime != data.dateTime);
    const bool hasIconChanged =
        (m_data.icon.cacheKey() != data.icon.cacheKey());
    const bool hasAvatarChanged =
        (m_data.avatar.cacheKey() != data.avatar.cacheKey());
    const bool hasStateChanged = (m_data.state != data.state);
    const bool hasTypeChanged = (m_data.type != data.type);
    const bool hasSizeChanged = (m_data.size != data.size);
    const bool hasIndicatorStyleChanged =
        (m_data.indicatorStyle != data.indicatorStyle);
    const bool hasExpandableChanged = (m_data.expandable != data.expandable);
    const bool hasExpandedChanged = (m_data.expanded != data.expanded);
    const bool hasInteractiveChanged = (m_data.interactive != data.interactive);
    const bool hasShowDateTimeChanged =
        (m_data.showDateTime != data.showDateTime);
    const bool hasShowIndicatorChanged =
        (m_data.showIndicator != data.showIndicator);

    m_data = data;

    m_sizeHintValid = false;
    updateGeometry();
    updateColors();
    updateAccessibility();
    update();

    // Emit change signals
    if (hasTitleChanged)
        emit titleChanged(data.title);
    if (hasDescriptionChanged)
        emit descriptionChanged(data.description);
    if (hasSubtitleChanged)
        emit subtitleChanged(data.subtitle);
    if (hasDateTimeChanged)
        emit dateTimeChanged(data.dateTime);
    if (hasIconChanged)
        emit iconChanged(data.icon);
    if (hasAvatarChanged)
        emit avatarChanged(data.avatar);
    if (hasStateChanged)
        emit itemStateChanged(data.state);
    if (hasTypeChanged)
        emit itemTypeChanged(data.type);
    if (hasSizeChanged)
        emit itemSizeChanged(data.size);
    if (hasIndicatorStyleChanged)
        emit indicatorStyleChanged(data.indicatorStyle);
    if (hasExpandableChanged)
        emit expandableChanged(data.expandable);
    if (hasExpandedChanged)
        emit expandedChanged(data.expanded);
    if (hasInteractiveChanged)
        emit interactiveChanged(data.interactive);
    if (hasShowDateTimeChanged)
        emit showDateTimeChanged(data.showDateTime);
    if (hasShowIndicatorChanged)
        emit showIndicatorChanged(data.showIndicator);
}

void FluentTimelineItem::setContentWidget(QWidget* widget) {
    if (m_contentWidget != widget) {
        if (m_contentWidget) {
            m_mainLayout->removeWidget(m_contentWidget);
            m_contentWidget->setParent(nullptr);
        }

        m_contentWidget = widget;

        if (m_contentWidget) {
            m_contentWidget->setParent(this);
            m_mainLayout->addWidget(m_contentWidget);
        }

        updateLayout();
    }
}

void FluentTimelineItem::setExpandedWidget(QWidget* widget) {
    if (m_expandedWidget != widget) {
        if (m_expandedWidget) {
            m_mainLayout->removeWidget(m_expandedWidget);
            m_expandedWidget->setParent(nullptr);
        }

        m_expandedWidget = widget;

        if (m_expandedWidget) {
            m_expandedWidget->setParent(this);
            m_mainLayout->addWidget(m_expandedWidget);
            m_expandedWidget->setVisible(m_data.expanded);
        }

        updateLayout();
    }
}

QSize FluentTimelineItem::sizeHint() const {
    if (!m_sizeHintValid) {
        const_cast<FluentTimelineItem*>(this)->m_cachedSizeHint =
            calculateSizeHint();
        const_cast<FluentTimelineItem*>(this)->m_sizeHintValid = true;
    }
    return m_cachedSizeHint;
}

QSize FluentTimelineItem::minimumSizeHint() const {
    return QSize(100, 40);  // Minimum reasonable size
}

QSize FluentTimelineItem::calculateSizeHint() const {
    // Theme instance available for future styling calculations
    [[maybe_unused]] const auto& theme = FluentTheme::instance();
    const QFontMetrics titleMetrics(getTitleFont());
    const QFontMetrics descMetrics(getDescriptionFont());
    const QFontMetrics dateMetrics(getDateTimeFont());

    int width = 200;  // Base width
    int height = 0;

    // Calculate height based on content
    if (!m_data.title.isEmpty()) {
        height += titleMetrics.height() + 4;
    }

    if (!m_data.subtitle.isEmpty()) {
        height += titleMetrics.height() + 2;
    }

    if (!m_data.description.isEmpty()) {
        const QRect descRect = descMetrics.boundingRect(
            QRect(0, 0, width - 60, 0), Qt::TextWordWrap, m_data.description);
        height += descRect.height() + 4;
    }

    if (m_data.showDateTime && m_data.dateTime.isValid()) {
        height += dateMetrics.height() + 4;
    }

    // Add padding based on size
    int padding = 8;
    switch (m_data.size) {
        case FluentTimelineItemSize::Small:
            padding = 4;
            break;
        case FluentTimelineItemSize::Large:
            padding = 16;
            break;
        default:
            padding = 8;
            break;
    }

    height += padding * 2;

    // Minimum height
    height = qMax(height, 40);

    // Add space for indicator
    if (m_data.showIndicator) {
        width += getIndicatorSize() + 8;
    }

    // Add space for expanded content
    if (m_data.expanded && m_expandedWidget) {
        height += m_expandedWidget->sizeHint().height();
    }

    return QSize(width, height);
}

void FluentTimelineItem::animateExpansion(bool expanded) {
    if (!m_expansionAnimation)
        return;

    m_expansionAnimation->stop();
    m_expansionAnimation->setStartValue(m_expansionProgress);
    m_expansionAnimation->setEndValue(expanded ? 1.0 : 0.0);
    m_expansionAnimation->setDuration(250);
    m_expansionAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_expansionAnimation->start();
}

void FluentTimelineItem::animateStateChange(FluentTimelineItemState newState) {
    // newState parameter available for future state-specific animations
    [[maybe_unused]] FluentTimelineItemState targetState = newState;

    if (!m_stateAnimation)
        return;

    m_stateAnimation->stop();
    m_stateAnimation->setStartValue(m_stateProgress);
    m_stateAnimation->setEndValue(1.0);
    m_stateAnimation->setDuration(200);
    m_stateAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_stateAnimation->start();
}

// Static factory methods
FluentTimelineItem* FluentTimelineItem::createMilestone(
    const QString& title, const QDateTime& dateTime, QWidget* parent) {
    FluentTimelineItemData data;
    data.title = title;
    data.dateTime = dateTime;
    data.type = FluentTimelineItemType::Milestone;
    data.indicatorStyle = FluentTimelineItemIndicatorStyle::Diamond;
    data.state = FluentTimelineItemState::Completed;

    return new FluentTimelineItem(data, parent);
}

FluentTimelineItem* FluentTimelineItem::createEvent(const QString& title,
                                                    const QString& description,
                                                    const QDateTime& dateTime,
                                                    QWidget* parent) {
    FluentTimelineItemData data;
    data.title = title;
    data.description = description;
    data.dateTime = dateTime;
    data.type = FluentTimelineItemType::Event;
    data.indicatorStyle = FluentTimelineItemIndicatorStyle::Circle;

    return new FluentTimelineItem(data, parent);
}

FluentTimelineItem* FluentTimelineItem::createTask(
    const QString& title, FluentTimelineItemState state, QWidget* parent) {
    FluentTimelineItemData data;
    data.title = title;
    data.type = FluentTimelineItemType::Task;
    data.state = state;
    data.indicatorStyle = FluentTimelineItemIndicatorStyle::Square;

    return new FluentTimelineItem(data, parent);
}

FluentTimelineItem* FluentTimelineItem::createNote(const QString& title,
                                                   const QString& description,
                                                   QWidget* parent) {
    FluentTimelineItemData data;
    data.title = title;
    data.description = description;
    data.type = FluentTimelineItemType::Note;
    data.indicatorStyle = FluentTimelineItemIndicatorStyle::Circle;
    data.state = FluentTimelineItemState::Current;

    return new FluentTimelineItem(data, parent);
}

FluentTimelineItem* FluentTimelineItem::createIconItem(
    const QIcon& icon, const QString& title, const QString& description,
    QWidget* parent) {
    FluentTimelineItemData data;
    data.icon = icon;
    data.title = title;
    data.description = description;
    data.indicatorStyle = FluentTimelineItemIndicatorStyle::Icon;

    return new FluentTimelineItem(data, parent);
}

FluentTimelineItem* FluentTimelineItem::createAvatarItem(
    const QPixmap& avatar, const QString& title, const QString& description,
    QWidget* parent) {
    FluentTimelineItemData data;
    data.avatar = avatar;
    data.title = title;
    data.description = description;
    data.indicatorStyle = FluentTimelineItemIndicatorStyle::Avatar;

    return new FluentTimelineItem(data, parent);
}

// Public slots
void FluentTimelineItem::toggle() {
    if (m_data.expandable) {
        setExpanded(!m_data.expanded);
    }
}

void FluentTimelineItem::expand() {
    if (m_data.expandable) {
        setExpanded(true);
    }
}

void FluentTimelineItem::collapse() {
    if (m_data.expandable) {
        setExpanded(false);
    }
}

void FluentTimelineItem::markCompleted() {
    setItemState(FluentTimelineItemState::Completed);
}

void FluentTimelineItem::markFailed() {
    setItemState(FluentTimelineItemState::Failed);
}

void FluentTimelineItem::markCancelled() {
    setItemState(FluentTimelineItemState::Cancelled);
}

void FluentTimelineItem::markPending() {
    setItemState(FluentTimelineItemState::Pending);
}

// Protected event handlers
void FluentTimelineItem::paintEvent(QPaintEvent* event) {
    // Event parameter available for future paint optimization
    [[maybe_unused]] QPaintEvent* paintEvent = event;

    FLUENT_PROFILE("FluentTimelineItem::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border if focused or hovered
    if (hasFocus() || m_hovered) {
        paintBorder(painter, rect);
    }

    // Paint indicator
    if (m_data.showIndicator) {
        const QRect indicatorRect = getIndicatorRect();
        paintIndicator(painter, indicatorRect);
    }

    // Paint content
    const QRect contentRect = getContentRect();
    paintContent(painter, contentRect);
}

void FluentTimelineItem::mousePressEvent(QMouseEvent* event) {
    if (!m_data.interactive) {
        FluentComponent::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
        event->accept();
    } else {
        FluentComponent::mousePressEvent(event);
    }
}

void FluentTimelineItem::mouseReleaseEvent(QMouseEvent* event) {
    if (!m_data.interactive) {
        FluentComponent::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        if (rect().contains(event->pos())) {
            emit clicked();
        }

        update();
        event->accept();
    } else {
        FluentComponent::mouseReleaseEvent(event);
    }
}

void FluentTimelineItem::mouseDoubleClickEvent(QMouseEvent* event) {
    if (!m_data.interactive) {
        FluentComponent::mouseDoubleClickEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();

        // Auto-toggle expansion on double-click
        if (m_data.expandable) {
            toggle();
        }

        event->accept();
    } else {
        FluentComponent::mouseDoubleClickEvent(event);
    }
}

void FluentTimelineItem::enterEvent(QEnterEvent* event) {
    FluentComponent::enterEvent(event);

    if (m_data.interactive) {
        m_hovered = true;
        update();
    }
}

void FluentTimelineItem::leaveEvent(QEvent* event) {
    FluentComponent::leaveEvent(event);

    m_hovered = false;
    m_pressed = false;
    update();
}

void FluentTimelineItem::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);

    if (event->size() != m_lastRect.size()) {
        m_lastRect = rect();
        m_sizeHintValid = false;
    }
}

void FluentTimelineItem::updateStateStyle() {
    FluentComponent::updateStateStyle();
    updateColors();
}

void FluentTimelineItem::performStateTransition(FluentState from,
                                                FluentState to) {
    FluentComponent::performStateTransition(from, to);

    // Add any state transition animations here
}

// Private slots
void FluentTimelineItem::onThemeChanged() {
    updateColors();
    m_sizeHintValid = false;
    updateGeometry();
    update();
}

void FluentTimelineItem::onExpansionAnimationFinished() {
    if (m_expandedWidget) {
        m_expandedWidget->setVisible(m_data.expanded);
    }
    updateLayout();
}

void FluentTimelineItem::onStateAnimationFinished() {
    // State animation completed
}

// Private implementation methods
void FluentTimelineItem::setupLayout() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(4);
}

void FluentTimelineItem::setupAnimations() {
    // Expansion animation
    m_expansionAnimation =
        std::make_unique<QPropertyAnimation>(this, "expansionProgress");
    connect(m_expansionAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentTimelineItem::onExpansionAnimationFinished);

    // State animation
    m_stateAnimation =
        std::make_unique<QPropertyAnimation>(this, "stateProgress");
    connect(m_stateAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentTimelineItem::onStateAnimationFinished);

    // Opacity effect for animations
    m_opacityEffect = std::make_unique<QGraphicsOpacityEffect>(this);
}

void FluentTimelineItem::updateLayout() {
    if (m_expandedWidget) {
        m_expandedWidget->setVisible(m_data.expanded &&
                                     m_expansionProgress > 0.5);
    }

    m_sizeHintValid = false;
    updateGeometry();
}

void FluentTimelineItem::updateColors() {
    // Colors will be calculated in paint methods based on current theme
}

void FluentTimelineItem::updateAccessibility() {
    QString accessibleName = m_data.title;
    if (!m_data.description.isEmpty()) {
        accessibleName += " - " + m_data.description;
    }

    setAccessibleName(accessibleName);
    setAccessibleDescription(QString("Timeline item: %1").arg(m_data.title));

    if (m_data.expandable) {
        FluentQt::Accessibility::setAccessibleRole(this, QAccessible::Button);
    } else {
        FluentQt::Accessibility::setAccessibleRole(this,
                                                   QAccessible::StaticText);
    }
}

void FluentTimelineItem::paintIndicator(QPainter& painter, const QRect& rect) {
    const QColor indicatorColor = getStateColor();
    const int size = getIndicatorSize();
    const QRect indicatorRect =
        QRect(rect.center() - QPoint(size / 2, size / 2), QSize(size, size));

    painter.setRenderHint(QPainter::Antialiasing);

    switch (m_data.indicatorStyle) {
        case FluentTimelineItemIndicatorStyle::Circle:
            painter.setBrush(indicatorColor);
            painter.setPen(QPen(indicatorColor.darker(120), 1));
            painter.drawEllipse(indicatorRect);
            break;

        case FluentTimelineItemIndicatorStyle::Square:
            painter.setBrush(indicatorColor);
            painter.setPen(QPen(indicatorColor.darker(120), 1));
            painter.drawRect(indicatorRect);
            break;

        case FluentTimelineItemIndicatorStyle::Diamond: {
            QPainterPath path;
            const QPoint center = indicatorRect.center();
            const int halfSize = size / 2;
            path.moveTo(center.x(), center.y() - halfSize);
            path.lineTo(center.x() + halfSize, center.y());
            path.lineTo(center.x(), center.y() + halfSize);
            path.lineTo(center.x() - halfSize, center.y());
            path.closeSubpath();

            painter.setBrush(indicatorColor);
            painter.setPen(QPen(indicatorColor.darker(120), 1));
            painter.drawPath(path);
            break;
        }

        case FluentTimelineItemIndicatorStyle::Icon:
            if (!m_data.icon.isNull()) {
                m_data.icon.paint(&painter, indicatorRect);
            } else if (!m_data.customIcon.isNull()) {
                m_data.customIcon.paint(&painter, indicatorRect);
            }
            break;

        case FluentTimelineItemIndicatorStyle::Avatar:
            if (!m_data.avatar.isNull()) {
                painter.setBrush(QBrush(m_data.avatar.scaled(
                    indicatorRect.size(), Qt::KeepAspectRatioByExpanding,
                    Qt::SmoothTransformation)));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(indicatorRect);
            }
            break;

        case FluentTimelineItemIndicatorStyle::Custom:
            // Custom indicator rendering can be implemented by subclasses
            painter.setBrush(indicatorColor);
            painter.setPen(QPen(indicatorColor.darker(120), 1));
            painter.drawEllipse(indicatorRect);
            break;
    }
}

void FluentTimelineItem::paintContent(QPainter& painter, const QRect& rect) {
    // Theme instance available for future styling enhancements
    [[maybe_unused]] const auto& theme = FluentTheme::instance();
    const QColor textColor = getTextColor();
    const QColor secondaryTextColor = getSecondaryTextColor();

    int y = rect.top();
    const int leftMargin = rect.left();
    const int rightMargin = rect.right();

    // Paint title
    if (!m_data.title.isEmpty()) {
        painter.setFont(getTitleFont());
        painter.setPen(textColor);

        const QRect titleRect(leftMargin, y, rightMargin - leftMargin,
                              getTitleFont().pointSize() + 4);
        painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop, m_data.title);
        y += titleRect.height() + 2;
    }

    // Paint subtitle
    if (!m_data.subtitle.isEmpty()) {
        painter.setFont(getTitleFont());
        painter.setPen(secondaryTextColor);

        const QRect subtitleRect(leftMargin, y, rightMargin - leftMargin,
                                 getTitleFont().pointSize() + 2);
        painter.drawText(subtitleRect, Qt::AlignLeft | Qt::AlignTop,
                         m_data.subtitle);
        y += subtitleRect.height() + 2;
    }

    // Paint description
    if (!m_data.description.isEmpty()) {
        painter.setFont(getDescriptionFont());
        painter.setPen(secondaryTextColor);

        const QRect descRect(leftMargin, y, rightMargin - leftMargin,
                             rect.bottom() - y);
        painter.drawText(descRect,
                         Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                         m_data.description);

        const QFontMetrics metrics(getDescriptionFont());
        const QRect boundingRect = metrics.boundingRect(
            descRect, Qt::TextWordWrap, m_data.description);
        y += boundingRect.height() + 4;
    }

    // Paint date/time
    if (m_data.showDateTime && m_data.dateTime.isValid()) {
        painter.setFont(getDateTimeFont());
        painter.setPen(secondaryTextColor);

        const QString dateTimeText =
            m_data.dateTime.toString("MMM dd, yyyy hh:mm");
        const QRect dateTimeRect(leftMargin, y, rightMargin - leftMargin,
                                 getDateTimeFont().pointSize() + 2);
        painter.drawText(dateTimeRect, Qt::AlignLeft | Qt::AlignTop,
                         dateTimeText);
    }

    // Paint custom text if provided
    if (!m_data.customText.isEmpty()) {
        painter.setFont(getDescriptionFont());
        painter.setPen(getStateColor());

        const QRect customRect(leftMargin, rect.bottom() - 20,
                               rightMargin - leftMargin, 20);
        painter.drawText(customRect, Qt::AlignLeft | Qt::AlignBottom,
                         m_data.customText);
    }
}

void FluentTimelineItem::paintBackground(QPainter& painter, const QRect& rect) {
    const QColor backgroundColor = getBackgroundColor();

    if (backgroundColor.alpha() > 0) {
        const int radius = cornerRadius();

        QPainterPath path;
        path.addRoundedRect(rect, radius, radius);

        painter.fillPath(path, backgroundColor);
    }
}

void FluentTimelineItem::paintBorder(QPainter& painter, const QRect& rect) {
    const QColor borderColor = getBorderColor();
    const int radius = cornerRadius();

    QPen pen(borderColor, 1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath path;
    path.addRoundedRect(rect.adjusted(0, 0, -1, -1), radius, radius);
    painter.drawPath(path);
}

QRect FluentTimelineItem::getIndicatorRect() const {
    const int size = getIndicatorSize();
    return QRect(8, 8, size, size);
}

QRect FluentTimelineItem::getContentRect() const {
    const QRect indicatorRect = getIndicatorRect();
    const int leftOffset = m_data.showIndicator ? indicatorRect.right() + 8 : 8;

    return QRect(leftOffset, 8, width() - leftOffset - 8, height() - 16);
}

QRect FluentTimelineItem::getTitleRect() const {
    const QRect contentRect = getContentRect();
    const QFontMetrics metrics(getTitleFont());

    return QRect(contentRect.left(), contentRect.top(), contentRect.width(),
                 metrics.height() + 4);
}

QRect FluentTimelineItem::getDescriptionRect() const {
    const QRect titleRect = getTitleRect();
    const QRect contentRect = getContentRect();

    return QRect(contentRect.left(), titleRect.bottom() + 2,
                 contentRect.width(),
                 contentRect.bottom() - titleRect.bottom() - 2);
}

QRect FluentTimelineItem::getDateTimeRect() const {
    const QRect contentRect = getContentRect();
    const QFontMetrics metrics(getDateTimeFont());

    return QRect(contentRect.left(),
                 contentRect.bottom() - metrics.height() - 4,
                 contentRect.width(), metrics.height() + 4);
}

QColor FluentTimelineItem::getStateColor() const {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Use custom color if provided
    if (m_data.customColor.isValid()) {
        return m_data.customColor;
    }

    // Return color based on state
    switch (m_data.state) {
        case FluentTimelineItemState::Completed:
            return palette.success;
        case FluentTimelineItemState::Current:
            return palette.accent;
        case FluentTimelineItemState::Failed:
            return palette.error;
        case FluentTimelineItemState::Cancelled:
            return palette.neutralSecondary;
        case FluentTimelineItemState::Warning:
            return palette.warning;
        case FluentTimelineItemState::Pending:
        default:
            return palette.neutralTertiary;
    }
}

QColor FluentTimelineItem::getBackgroundColor() const {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (m_hovered) {
        return theme.adjustAlpha(palette.neutralLighter, 0.5);
    } else if (hasFocus()) {
        return theme.adjustAlpha(palette.accent, 0.1);
    } else if (m_pressed) {
        return theme.adjustAlpha(palette.neutralLight, 0.7);
    }

    return Qt::transparent;
}

QColor FluentTimelineItem::getBorderColor() const {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (hasFocus()) {
        return palette.accent;
    } else if (m_hovered) {
        return palette.neutralSecondary;
    }

    return palette.neutralQuaternaryAlt;
}

QColor FluentTimelineItem::getTextColor() const {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    return palette.neutralPrimary;
}

QColor FluentTimelineItem::getSecondaryTextColor() const {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    return palette.neutralSecondary;
}

int FluentTimelineItem::getIndicatorSize() const {
    switch (m_data.size) {
        case FluentTimelineItemSize::Small:
            return 8;
        case FluentTimelineItemSize::Large:
            return 16;
        case FluentTimelineItemSize::Medium:
        default:
            return 12;
    }
}

QFont FluentTimelineItem::getTitleFont() const {
    const auto& theme = FluentTheme::instance();

    switch (m_data.size) {
        case FluentTimelineItemSize::Small:
            return theme.bodySmallFont();
        case FluentTimelineItemSize::Large:
            return theme.titleSmallFont();
        case FluentTimelineItemSize::Medium:
        default:
            return theme.bodyFont();
    }
}

QFont FluentTimelineItem::getDescriptionFont() const {
    const auto& theme = FluentTheme::instance();

    switch (m_data.size) {
        case FluentTimelineItemSize::Small:
            return theme.captionFont();
        case FluentTimelineItemSize::Large:
            return theme.bodyFont();
        case FluentTimelineItemSize::Medium:
        default:
            return theme.bodySmallFont();
    }
}

QFont FluentTimelineItem::getDateTimeFont() const {
    const auto& theme = FluentTheme::instance();
    return theme.captionFont();
}
