// src/Components/FluentIndicatorCarousel.cpp
// Optimized FluentIndicatorCarousel implementation with enhanced Fluent UI
// compliance
#include "FluentQt/Components/FluentIndicatorCarousel.h"
#include <QAccessible>
#include <QApplication>
#include <QButtonGroup>
#include <QEasingCurve>
#include <QEnterEvent>
#include <QFocusEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentIndicatorCarousel::FluentIndicatorCarousel(QWidget* parent)
    : FluentCarousel(parent),
      m_indicatorAnimation(std::make_unique<QPropertyAnimation>(this)) {
    initializeIndicatorCarousel();
}

FluentIndicatorCarousel::FluentIndicatorCarousel(
    const FluentCarouselConfig& config, QWidget* parent)
    : FluentCarousel(config, parent),
      m_indicatorAnimation(std::make_unique<QPropertyAnimation>(this)) {
    initializeIndicatorCarousel();
}

void FluentIndicatorCarousel::initializeIndicatorCarousel() {
    // Set default configuration for indicator carousel
    FluentCarouselConfig config = this->config();
    config.showIndicators = true;
    config.showNavigation = false;
    config.autoPlay = FluentCarouselAutoPlay::None;
    setConfig(config);

    // Initialize colors from theme
    auto& theme = Styling::FluentTheme::instance();
    m_activeIndicatorColor = theme.color("accent");
    m_inactiveIndicatorColor = theme.color("neutralTertiary");

    createIndicatorContainer();
    setupIndicatorAccessibility();

    // Connect signals
    connect(this, &FluentCarousel::currentIndexChanged, this,
            &FluentIndicatorCarousel::onCurrentIndexChanged);
    connect(this, &FluentCarousel::itemCountChanged, this,
            &FluentIndicatorCarousel::onItemCountChanged);
    connect(&theme, &Styling::FluentTheme::themeChanged, this,
            &FluentIndicatorCarousel::refreshIndicatorStyles);

    updateIndicators();
}

void FluentIndicatorCarousel::createIndicatorContainer() {
    // Create indicator container
    m_indicatorContainer = new QWidget(this);
    m_indicatorContainer->setObjectName("FluentIndicatorCarousel_Indicators");

    // Create indicator layout
    m_indicatorLayout = new QHBoxLayout(m_indicatorContainer);
    m_indicatorLayout->setContentsMargins(8, 8, 8, 8);
    m_indicatorLayout->setSpacing(m_indicatorSpacing);
    m_indicatorLayout->setAlignment(Qt::AlignCenter);

    // Create button group for exclusive selection
    m_indicatorGroup = new QButtonGroup(this);
    m_indicatorGroup->setExclusive(true);

    connect(m_indicatorGroup,
            QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this,
            &FluentIndicatorCarousel::onIndicatorClicked);

    // Position indicator container
    updateIndicatorPositions();

    // Add to main layout
    if (auto* mainLayout = qobject_cast<QVBoxLayout*>(layout())) {
        mainLayout->addWidget(m_indicatorContainer);
    }
}

void FluentIndicatorCarousel::setIndicatorStyle(
    FluentCarouselIndicatorStyle style) {
    if (m_indicatorStyle != style) {
        m_indicatorStyle = style;
        updateIndicators();
        emit indicatorStyleChanged(style);
    }
}

void FluentIndicatorCarousel::setIndicatorPosition(
    FluentCarouselIndicatorPosition position) {
    if (m_indicatorPosition != position) {
        m_indicatorPosition = position;
        updateIndicatorPositions();
        emit indicatorPositionChanged(position);
    }
}

void FluentIndicatorCarousel::setShowIndicators(bool show) {
    if (m_showIndicators != show) {
        m_showIndicators = show;

        if (m_indicatorContainer) {
            m_indicatorContainer->setVisible(show);
        }

        emit indicatorsVisibilityChanged(show);
    }
}

void FluentIndicatorCarousel::setClickableIndicators(bool clickable) {
    if (m_clickableIndicators != clickable) {
        m_clickableIndicators = clickable;

        for (auto* indicator : m_indicators) {
            indicator->setEnabled(clickable);
        }

        emit clickableIndicatorsChanged(clickable);
    }
}

void FluentIndicatorCarousel::setIndicatorSize(int size) {
    if (m_indicatorSize != size) {
        m_indicatorSize = size;

        for (auto* indicator : m_indicators) {
            indicator->setFixedSize(size, size);
        }

        emit indicatorSizeChanged(size);
    }
}

void FluentIndicatorCarousel::setIndicatorSpacing(int spacing) {
    if (m_indicatorSpacing != spacing) {
        m_indicatorSpacing = spacing;

        if (m_indicatorLayout) {
            m_indicatorLayout->setSpacing(spacing);
        }

        emit indicatorSpacingChanged(spacing);
    }
}

void FluentIndicatorCarousel::setActiveIndicatorColor(const QColor& color) {
    if (m_activeIndicatorColor != color) {
        m_activeIndicatorColor = color;
        updateIndicatorStates();
        emit activeIndicatorColorChanged(color);
    }
}

void FluentIndicatorCarousel::setInactiveIndicatorColor(const QColor& color) {
    if (m_inactiveIndicatorColor != color) {
        m_inactiveIndicatorColor = color;
        updateIndicatorStates();
        emit inactiveIndicatorColorChanged(color);
    }
}

QList<QAbstractButton*> FluentIndicatorCarousel::indicatorButtons() const {
    QList<QAbstractButton*> buttons;
    for (auto* indicator : m_indicators) {
        buttons.append(indicator);
    }
    return buttons;
}

QAbstractButton* FluentIndicatorCarousel::indicatorAt(int index) const {
    if (index >= 0 && index < static_cast<int>(m_indicators.size())) {
        return m_indicators[index];
    }
    return nullptr;
}

// Factory methods
FluentIndicatorCarousel* FluentIndicatorCarousel::createWithDots(
    QWidget* parent) {
    auto* carousel = new FluentIndicatorCarousel(parent);
    carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Dots);
    return carousel;
}

FluentIndicatorCarousel* FluentIndicatorCarousel::createWithNumbers(
    QWidget* parent) {
    auto* carousel = new FluentIndicatorCarousel(parent);
    carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Numbers);
    return carousel;
}

FluentIndicatorCarousel* FluentIndicatorCarousel::createWithThumbnails(
    QWidget* parent) {
    auto* carousel = new FluentIndicatorCarousel(parent);
    carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Thumbnails);
    return carousel;
}

FluentIndicatorCarousel* FluentIndicatorCarousel::createOverlay(
    QWidget* parent) {
    auto* carousel = new FluentIndicatorCarousel(parent);
    carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Overlay);
    return carousel;
}

// Public slots
void FluentIndicatorCarousel::updateIndicators() {
    createIndicators();
    updateIndicatorStates();
}

void FluentIndicatorCarousel::setIndicatorActive(int index, bool animated) {
    if (index < 0 || index >= static_cast<int>(m_indicators.size())) {
        return;
    }

    if (animated) {
        animateIndicatorTransition(currentIndex(), index);
    }

    // Update button states
    for (int i = 0; i < static_cast<int>(m_indicators.size()); ++i) {
        auto* button =
            qobject_cast<FluentCarouselIndicatorButton*>(m_indicators[i]);
        if (button) {
            button->setActive(i == index);
        }
    }
}

void FluentIndicatorCarousel::refreshIndicatorStyles() {
    auto& theme = Styling::FluentTheme::instance();
    m_activeIndicatorColor = theme.color("accent");
    m_inactiveIndicatorColor = theme.color("neutralTertiary");

    updateIndicatorStates();
}

// Event handling
void FluentIndicatorCarousel::resizeEvent(QResizeEvent* event) {
    FluentCarousel::resizeEvent(event);
    updateIndicatorPositions();
}

void FluentIndicatorCarousel::paintEvent(QPaintEvent* event) {
    FluentCarousel::paintEvent(event);

    // Additional painting for overlay indicators
    if (m_indicatorPosition == FluentCarouselIndicatorPosition::Overlay) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw semi-transparent background for overlay indicators
        QSize containerSize = calculateIndicatorContainerSize();
        QRect overlayRect(0, 0, containerSize.width(), containerSize.height());
        overlayRect.moveCenter(rect().center());
        overlayRect.moveBottom(rect().bottom() - 20);

        painter.fillRect(overlayRect, QColor(0, 0, 0, 100));
        painter.drawRoundedRect(overlayRect, 8, 8);
    }
}

void FluentIndicatorCarousel::changeEvent(QEvent* event) {
    FluentCarousel::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        for (auto* indicator : m_indicators) {
            indicator->setEnabled(isEnabled() && m_clickableIndicators);
        }
    }
}

// Private slots
void FluentIndicatorCarousel::onIndicatorClicked() {
    auto* button = qobject_cast<QAbstractButton*>(sender());
    if (!button)
        return;

    int index = m_indicatorGroup->id(button);
    if (index >= 0) {
        setCurrentIndex(index);
        emit indicatorClicked(index);
    }
}

void FluentIndicatorCarousel::onCurrentIndexChanged(int index) {
    setIndicatorActive(index, true);
}

void FluentIndicatorCarousel::onItemCountChanged(int count) {
    Q_UNUSED(count)
    updateIndicators();
}

void FluentIndicatorCarousel::onIndicatorHovered() {
    auto* button = qobject_cast<QAbstractButton*>(sender());
    if (!button)
        return;

    int index = m_indicatorGroup->id(button);
    if (index >= 0) {
        emit indicatorHovered(index);
    }
}

void FluentIndicatorCarousel::onIndicatorAnimationFinished() {
    m_animatingFromIndex = -1;
    m_animatingToIndex = -1;
}

// Private helper methods
void FluentIndicatorCarousel::createIndicators() {
    // Clear existing indicators
    for (auto* indicator : m_indicators) {
        m_indicatorLayout->removeWidget(indicator);
        m_indicatorGroup->removeButton(indicator);
        indicator->deleteLater();
    }
    m_indicators.clear();

    // Create new indicators
    int itemCount = this->itemCount();
    for (int i = 0; i < itemCount; ++i) {
        auto* indicator = createIndicatorButton(i);
        m_indicators.push_back(indicator);
        m_indicatorLayout->addWidget(indicator);
        m_indicatorGroup->addButton(indicator, i);
    }

    updateIndicatorLayout();
}

void FluentIndicatorCarousel::updateIndicatorLayout() {
    if (!m_indicatorLayout)
        return;

    // Update layout direction based on orientation
    if (config().orientation == FluentCarouselOrientation::Vertical) {
        m_indicatorLayout->setDirection(QBoxLayout::TopToBottom);
    } else {
        m_indicatorLayout->setDirection(QBoxLayout::LeftToRight);
    }

    // Update spacing
    m_indicatorLayout->setSpacing(m_indicatorSpacing);
}

void FluentIndicatorCarousel::updateIndicatorPositions() {
    if (!m_indicatorContainer)
        return;

    // Position container based on indicator position
    QSize containerSize = calculateIndicatorContainerSize();
    QPoint position = calculateIndicatorPosition();

    m_indicatorContainer->setFixedSize(containerSize);
    m_indicatorContainer->move(position);
}

void FluentIndicatorCarousel::updateIndicatorStates() {
    int current = currentIndex();

    for (int i = 0; i < static_cast<int>(m_indicators.size()); ++i) {
        auto* button =
            qobject_cast<FluentCarouselIndicatorButton*>(m_indicators[i]);
        if (button) {
            button->setActive(i == current);
            button->setActiveColor(m_activeIndicatorColor);
            button->setInactiveColor(m_inactiveIndicatorColor);
            button->setIndicatorStyle(m_indicatorStyle);
        }
    }
}

void FluentIndicatorCarousel::animateIndicatorTransition(int fromIndex,
                                                         int toIndex) {
    if (fromIndex == toIndex || !m_indicatorAnimation)
        return;

    m_animatingFromIndex = fromIndex;
    m_animatingToIndex = toIndex;

    // Simple opacity animation for now
    if (fromIndex >= 0 && fromIndex < static_cast<int>(m_indicators.size())) {
        auto* fromButton = m_indicators[fromIndex];
        auto* effect = new QGraphicsOpacityEffect(fromButton);
        fromButton->setGraphicsEffect(effect);

        m_indicatorAnimation->setTargetObject(effect);
        m_indicatorAnimation->setPropertyName("opacity");
        m_indicatorAnimation->setStartValue(1.0);
        m_indicatorAnimation->setEndValue(0.5);
        m_indicatorAnimation->setDuration(200);

        connect(m_indicatorAnimation.get(), &QPropertyAnimation::finished, this,
                &FluentIndicatorCarousel::onIndicatorAnimationFinished);

        m_indicatorAnimation->start();
    }
}

void FluentIndicatorCarousel::setupIndicatorAccessibility() {
    if (m_indicatorContainer) {
        m_indicatorContainer->setAccessibleName("Carousel indicators");
        m_indicatorContainer->setAccessibleDescription(
            "Navigate directly to carousel items");
    }
}

QAbstractButton* FluentIndicatorCarousel::createIndicatorButton(int index) {
    auto* button =
        new FluentCarouselIndicatorButton(index, m_indicatorContainer);
    button->setFixedSize(m_indicatorSize, m_indicatorSize);
    button->setIndicatorStyle(m_indicatorStyle);
    button->setActiveColor(m_activeIndicatorColor);
    button->setInactiveColor(m_inactiveIndicatorColor);
    button->setEnabled(m_clickableIndicators);

    // Set accessibility info
    button->setAccessibleName(QString("Item %1").arg(index + 1));
    button->setAccessibleDescription(
        QString("Navigate to carousel item %1").arg(index + 1));

    // Connect hover signal - TODO: implement hover signal
    // connect(button, &FluentCarouselIndicatorButton::entered,
    //         this, &FluentIndicatorCarousel::onIndicatorHovered);

    return button;
}

QSize FluentIndicatorCarousel::calculateIndicatorContainerSize() const {
    int itemCount = this->itemCount();
    if (itemCount == 0)
        return QSize(0, 0);

    if (config().orientation == FluentCarouselOrientation::Vertical) {
        int height =
            itemCount * m_indicatorSize + (itemCount - 1) * m_indicatorSpacing;
        return QSize(m_indicatorSize + 16, height + 16);
    } else {
        int width =
            itemCount * m_indicatorSize + (itemCount - 1) * m_indicatorSpacing;
        return QSize(width + 16, m_indicatorSize + 16);
    }
}

QPoint FluentIndicatorCarousel::calculateIndicatorPosition() const {
    QSize containerSize = calculateIndicatorContainerSize();
    QRect parentRect = rect();

    switch (m_indicatorPosition) {
        case FluentCarouselIndicatorPosition::Bottom:
            return QPoint((parentRect.width() - containerSize.width()) / 2,
                          parentRect.bottom() - containerSize.height());

        case FluentCarouselIndicatorPosition::Top:
            return QPoint((parentRect.width() - containerSize.width()) / 2, 0);

        case FluentCarouselIndicatorPosition::Left:
            return QPoint(0,
                          (parentRect.height() - containerSize.height()) / 2);

        case FluentCarouselIndicatorPosition::Right:
            return QPoint(parentRect.right() - containerSize.width(),
                          (parentRect.height() - containerSize.height()) / 2);

        case FluentCarouselIndicatorPosition::Overlay:
            return QPoint((parentRect.width() - containerSize.width()) / 2,
                          parentRect.bottom() - containerSize.height() - 20);
    }

    return QPoint(0, 0);
}

// FluentCarouselIndicatorButton implementation
FluentCarouselIndicatorButton::FluentCarouselIndicatorButton(int index,
                                                             QWidget* parent)
    : QAbstractButton(parent), m_index(index) {
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);

    // Initialize colors from theme
    auto& theme = Styling::FluentTheme::instance();
    m_activeColor = theme.color("accent");
    m_inactiveColor = theme.color("neutralTertiary");

    updateAccessibilityInfo();
}

void FluentCarouselIndicatorButton::setActive(bool active) {
    if (m_active != active) {
        m_active = active;
        setChecked(active);
        update();
        emit activeChanged(active);
    }
}

void FluentCarouselIndicatorButton::setIndicatorStyle(
    FluentCarouselIndicatorStyle style) {
    if (m_indicatorStyle != style) {
        m_indicatorStyle = style;
        updateGeometry();
        update();
    }
}

void FluentCarouselIndicatorButton::setActiveColor(const QColor& color) {
    if (m_activeColor != color) {
        m_activeColor = color;
        update();
    }
}

void FluentCarouselIndicatorButton::setInactiveColor(const QColor& color) {
    if (m_inactiveColor != color) {
        m_inactiveColor = color;
        update();
    }
}

void FluentCarouselIndicatorButton::setThumbnail(const QPixmap& thumbnail) {
    m_thumbnail = thumbnail;
    update();
}

QSize FluentCarouselIndicatorButton::sizeHint() const {
    switch (m_indicatorStyle) {
        case FluentCarouselIndicatorStyle::Dots:
            return QSize(12, 12);
        case FluentCarouselIndicatorStyle::Lines:
            return QSize(24, 4);
        case FluentCarouselIndicatorStyle::Numbers:
            return QSize(24, 24);
        case FluentCarouselIndicatorStyle::Thumbnails:
            return QSize(48, 32);
        case FluentCarouselIndicatorStyle::Progress:
            return QSize(32, 4);
        default:
            return QSize(12, 12);
    }
}

QSize FluentCarouselIndicatorButton::minimumSizeHint() const {
    return sizeHint();
}

void FluentCarouselIndicatorButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect paintRect = rect().adjusted(2, 2, -2, -2);

    switch (m_indicatorStyle) {
        case FluentCarouselIndicatorStyle::Dots:
            paintDotIndicator(painter, paintRect);
            break;
        case FluentCarouselIndicatorStyle::Lines:
            paintLineIndicator(painter, paintRect);
            break;
        case FluentCarouselIndicatorStyle::Numbers:
            paintNumberIndicator(painter, paintRect);
            break;
        case FluentCarouselIndicatorStyle::Thumbnails:
            paintThumbnailIndicator(painter, paintRect);
            break;
        case FluentCarouselIndicatorStyle::Progress:
            paintProgressIndicator(painter, paintRect);
            break;
        default:
            paintDotIndicator(painter, paintRect);
            break;
    }
}

void FluentCarouselIndicatorButton::enterEvent(QEnterEvent* event) {
    QAbstractButton::enterEvent(event);
    m_hovered = true;
    update();
}

void FluentCarouselIndicatorButton::leaveEvent(QEvent* event) {
    QAbstractButton::leaveEvent(event);
    m_hovered = false;
    update();
}

void FluentCarouselIndicatorButton::mousePressEvent(QMouseEvent* event) {
    QAbstractButton::mousePressEvent(event);
    m_pressed = true;
    update();
}

void FluentCarouselIndicatorButton::mouseReleaseEvent(QMouseEvent* event) {
    QAbstractButton::mouseReleaseEvent(event);
    m_pressed = false;
    update();
}

void FluentCarouselIndicatorButton::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_Space) {
        animateClick();
        event->accept();
    } else {
        QAbstractButton::keyPressEvent(event);
    }
}

void FluentCarouselIndicatorButton::focusInEvent(QFocusEvent* event) {
    QAbstractButton::focusInEvent(event);
    update();
}

void FluentCarouselIndicatorButton::focusOutEvent(QFocusEvent* event) {
    QAbstractButton::focusOutEvent(event);
    update();
}

void FluentCarouselIndicatorButton::paintDotIndicator(QPainter& painter,
                                                      const QRect& rect) {
    QColor color = getCurrentColor();

    if (m_hovered) {
        color = color.lighter(120);
    }
    if (m_pressed) {
        color = color.darker(120);
    }

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);

    int radius = std::min(rect.width(), rect.height()) / 2;
    painter.drawEllipse(rect.center(), radius, radius);

    // Draw focus ring
    if (hasFocus()) {
        QPen focusPen(color.lighter(150), 2);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(rect.center(), radius + 2, radius + 2);
    }
}

void FluentCarouselIndicatorButton::paintLineIndicator(QPainter& painter,
                                                       const QRect& rect) {
    QColor color = getCurrentColor();

    if (m_hovered) {
        color = color.lighter(120);
    }
    if (m_pressed) {
        color = color.darker(120);
    }

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 2, 2);

    // Draw focus ring
    if (hasFocus()) {
        QPen focusPen(color.lighter(150), 1);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-1, -1, 1, 1), 2, 2);
    }
}

void FluentCarouselIndicatorButton::paintNumberIndicator(QPainter& painter,
                                                         const QRect& rect) {
    QColor color = getCurrentColor();
    QColor textColor = m_active ? Qt::white : color;

    if (m_hovered) {
        color = color.lighter(120);
    }
    if (m_pressed) {
        color = color.darker(120);
    }

    // Draw background circle
    painter.setBrush(m_active ? color : Qt::transparent);
    painter.setPen(QPen(color, 2));

    int radius = std::min(rect.width(), rect.height()) / 2;
    painter.drawEllipse(rect.center(), radius, radius);

    // Draw number
    painter.setPen(textColor);
    QFont font = painter.font();
    font.setPointSize(10);
    font.setBold(m_active);
    painter.setFont(font);

    QString text = QString::number(m_index + 1);
    painter.drawText(rect, Qt::AlignCenter, text);

    // Draw focus ring
    if (hasFocus()) {
        QPen focusPen(color.lighter(150), 2);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(rect.center(), radius + 2, radius + 2);
    }
}

void FluentCarouselIndicatorButton::paintThumbnailIndicator(QPainter& painter,
                                                            const QRect& rect) {
    QColor borderColor = getCurrentColor();

    if (m_hovered) {
        borderColor = borderColor.lighter(120);
    }
    if (m_pressed) {
        borderColor = borderColor.darker(120);
    }

    // Draw thumbnail if available
    if (!m_thumbnail.isNull()) {
        QPixmap scaled =
            m_thumbnail.scaled(rect.size(), Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
        painter.drawPixmap(rect, scaled);
    } else {
        // Draw placeholder
        painter.fillRect(rect, QColor(200, 200, 200));
        painter.setPen(Qt::gray);
        painter.drawText(rect, Qt::AlignCenter, QString::number(m_index + 1));
    }

    // Draw border
    QPen borderPen(borderColor, m_active ? 3 : 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, 4, 4);

    // Draw focus ring
    if (hasFocus()) {
        QPen focusPen(borderColor.lighter(150), 2);
        painter.setPen(focusPen);
        painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), 4, 4);
    }
}

void FluentCarouselIndicatorButton::paintProgressIndicator(QPainter& painter,
                                                           const QRect& rect) {
    QColor color = getCurrentColor();

    if (m_hovered) {
        color = color.lighter(120);
    }
    if (m_pressed) {
        color = color.darker(120);
    }

    // Draw background
    painter.setBrush(m_inactiveColor.lighter(150));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 2, 2);

    // Draw progress (active state shows as filled)
    if (m_active) {
        painter.setBrush(color);
        painter.drawRoundedRect(rect, 2, 2);
    }

    // Draw focus ring
    if (hasFocus()) {
        QPen focusPen(color.lighter(150), 1);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-1, -1, 1, 1), 2, 2);
    }
}

QColor FluentCarouselIndicatorButton::getCurrentColor() const {
    return m_active ? m_activeColor : m_inactiveColor;
}

void FluentCarouselIndicatorButton::updateAccessibilityInfo() {
    setAccessibleName(QString("Carousel indicator %1").arg(m_index + 1));
    setAccessibleDescription(QString("Navigate to item %1").arg(m_index + 1));
}

}  // namespace FluentQt::Components
