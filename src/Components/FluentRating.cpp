// src/Components/FluentRating.cpp
#include "FluentQt/Components/FluentRating.h"
#include <QAccessible>
#include <QApplication>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>
#include <cmath>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"

namespace FluentQt::Components {

FluentRating::FluentRating(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setupAnimations();
    updateSizeMetrics();
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);

    // Initialize with design tokens

    updateColors();
    updateLayout();

    // Initialize accessibility features
#ifndef QT_NO_ACCESSIBILITY
    setAccessibleName(tr("Rating"));
    setAccessibleDescription(tr("Rating control"));
#endif
}

FluentRating::FluentRating(double maximum, QWidget* parent)
    : FluentRating(parent) {
    setMaximum(maximum);
}

FluentRating::FluentRating(double value, double maximum, QWidget* parent)
    : FluentRating(maximum, parent) {
    setValue(value);
}

FluentRating::~FluentRating() = default;

double FluentRating::value() const { return m_value; }

void FluentRating::setValue(double value) {
    const double boundedValue = qBound(m_minimum, value, m_maximum);
    if (qFuzzyCompare(m_value, boundedValue))
        return;

    m_value = boundedValue;

    if (m_animated) {
        startValueAnimation(m_value);
    } else {
        m_animatedValue = m_value;
        update();
    }

    emit valueChanged(m_value);
}

double FluentRating::maximum() const { return m_maximum; }

void FluentRating::setMaximum(double maximum) {
    if (qFuzzyCompare(m_maximum, maximum))
        return;

    m_maximum = qMax(m_minimum, maximum);

    // Adjust current value if it exceeds new maximum
    if (m_value > m_maximum) {
        setValue(m_maximum);
    }

    updateLayout();
    updateGeometry();
    update();
    emit maximumChanged(m_maximum);
}

double FluentRating::minimum() const { return m_minimum; }

void FluentRating::setMinimum(double minimum) {
    if (qFuzzyCompare(m_minimum, minimum))
        return;

    m_minimum = qMin(minimum, m_maximum);

    // Adjust current value if it's below new minimum
    if (m_value < m_minimum) {
        setValue(m_minimum);
    }

    updateLayout();
    updateGeometry();
    update();
    emit minimumChanged(m_minimum);
}

void FluentRating::setRange(double minimum, double maximum) {
    if (qFuzzyCompare(m_minimum, minimum) && qFuzzyCompare(m_maximum, maximum))
        return;

    m_minimum = minimum;
    m_maximum = qMax(minimum, maximum);

    // Adjust current value to fit new range
    setValue(qBound(m_minimum, m_value, m_maximum));
}

FluentRatingStyle FluentRating::ratingStyle() const { return m_ratingStyle; }

void FluentRating::setRatingStyle(FluentRatingStyle style) {
    if (m_ratingStyle == style)
        return;

    m_ratingStyle = style;
    updateSizeMetrics();
    updateLayout();
    updateGeometry();
    update();
    emit ratingStyleChanged(m_ratingStyle);
}

FluentRatingPrecision FluentRating::precision() const { return m_precision; }

void FluentRating::setPrecision(FluentRatingPrecision precision) {
    if (m_precision == precision)
        return;

    m_precision = precision;

    // Adjust current value to match new precision
    setValue(snapToValidValue(m_value));

    emit precisionChanged(m_precision);
}

FluentRatingSize FluentRating::ratingSize() const { return m_ratingSize; }

void FluentRating::setRatingSize(FluentRatingSize size) {
    if (m_ratingSize == size)
        return;

    m_ratingSize = size;
    updateSizeMetrics();
    updateLayout();
    updateGeometry();
    update();
    emit ratingSizeChanged(m_ratingSize);
}

FluentRatingOrientation FluentRating::orientation() const {
    return m_orientation;
}

void FluentRating::setOrientation(FluentRatingOrientation orientation) {
    if (m_orientation == orientation)
        return;

    m_orientation = orientation;
    updateLayout();
    updateGeometry();
    update();
    emit orientationChanged(m_orientation);
}

bool FluentRating::isReadOnly() const { return m_readOnly; }

void FluentRating::setReadOnly(bool readOnly) {
    if (m_readOnly == readOnly)
        return;

    m_readOnly = readOnly;
    setCursor(m_readOnly ? Qt::ArrowCursor : Qt::PointingHandCursor);
    updateColors();
    emit readOnlyChanged(m_readOnly);
}

bool FluentRating::showValue() const { return m_showValue; }

void FluentRating::setShowValue(bool show) {
    if (m_showValue == show)
        return;

    m_showValue = show;
    updateLayout();
    updateGeometry();
    update();
    emit showValueChanged(m_showValue);
}

bool FluentRating::showTooltip() const { return m_showTooltip; }

void FluentRating::setShowTooltip(bool show) {
    if (m_showTooltip == show)
        return;

    m_showTooltip = show;
    emit showTooltipChanged(m_showTooltip);
}

bool FluentRating::isAnimated() const { return m_animated; }

void FluentRating::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

bool FluentRating::allowReset() const { return m_allowReset; }

void FluentRating::setAllowReset(bool allow) {
    if (m_allowReset == allow)
        return;

    m_allowReset = allow;
    emit allowResetChanged(m_allowReset);
}

// Icon properties
QIcon FluentRating::filledIcon() const { return m_filledIcon; }

void FluentRating::setFilledIcon(const QIcon& icon) {
    if (m_filledIcon.cacheKey() == icon.cacheKey())
        return;

    m_filledIcon = icon;
    updateIcons();
    update();
    emit filledIconChanged(m_filledIcon);
}

QIcon FluentRating::emptyIcon() const { return m_emptyIcon; }

void FluentRating::setEmptyIcon(const QIcon& icon) {
    if (m_emptyIcon.cacheKey() == icon.cacheKey())
        return;

    m_emptyIcon = icon;
    updateIcons();
    update();
    emit emptyIconChanged(m_emptyIcon);
}

QIcon FluentRating::halfIcon() const { return m_halfIcon; }

void FluentRating::setHalfIcon(const QIcon& icon) {
    if (m_halfIcon.cacheKey() == icon.cacheKey())
        return;

    m_halfIcon = icon;
    updateIcons();
    update();
    emit halfIconChanged(m_halfIcon);
}

// Color properties
QColor FluentRating::filledColor() const { return m_filledColor; }

void FluentRating::setFilledColor(const QColor& color) {
    if (m_filledColor == color)
        return;

    m_filledColor = color;
    update();
    emit filledColorChanged(m_filledColor);
}

QColor FluentRating::emptyColor() const { return m_emptyColor; }

void FluentRating::setEmptyColor(const QColor& color) {
    if (m_emptyColor == color)
        return;

    m_emptyColor = color;
    update();
    emit emptyColorChanged(m_emptyColor);
}

// Utility methods
int FluentRating::itemCount() const {
    return static_cast<int>(m_maximum - m_minimum);
}

QSize FluentRating::itemSize() const { return QSize(m_itemSize, m_itemSize); }

QRect FluentRating::itemRect(int index) const {
    const int maxItems = static_cast<int>(m_maximum - m_minimum);
    if (index < 0 || index >= maxItems) {
        return QRect();
    }

    if (m_layoutDirty) {
        const_cast<FluentRating*>(this)->updateLayout();
    }

    if (index < m_itemRects.size()) {
        return m_itemRects[index];
    }

    return QRect();
}

double FluentRating::valueFromPosition(const QPoint& position) const {
    if (m_layoutDirty) {
        const_cast<FluentRating*>(this)->updateLayout();
    }

    for (int i = 0; i < m_itemRects.size(); ++i) {
        const QRect& itemRect = m_itemRects[i];

        if (itemRect.contains(position)) {
            double value = m_minimum + i + 1.0;

            // Calculate fractional part based on precision and position within
            // item
            if (m_precision != FluentRatingPrecision::Full) {
                double fraction = 0.0;

                if (m_orientation == FluentRatingOrientation::Horizontal) {
                    fraction =
                        static_cast<double>(position.x() - itemRect.left()) /
                        itemRect.width();
                } else {
                    fraction = 1.0 - static_cast<double>(position.y() -
                                                         itemRect.top()) /
                                         itemRect.height();
                }

                fraction = qBound(0.0, fraction, 1.0);
                value = m_minimum + i + fraction;
            }

            return snapToValidValue(value);
        }
    }

    return m_value;
}

QString FluentRating::valueText() const {
    return QString::number(m_value, 'f', 1);
}

QString FluentRating::tooltipText(double value) const {
    return QString::number(value, 'f', 1);
}

bool FluentRating::isValidValue(double value) const {
    return value >= m_minimum && value <= m_maximum;
}

double FluentRating::snapToValidValue(double value) const {
    const double clampedValue = qBound(m_minimum, value, m_maximum);

    switch (m_precision) {
        case FluentRatingPrecision::Full:
            return std::round(clampedValue);

        case FluentRatingPrecision::Half:
            return std::round(clampedValue * 2.0) / 2.0;

        case FluentRatingPrecision::Quarter:
            return std::round(clampedValue * 4.0) / 4.0;

        case FluentRatingPrecision::Exact:
            return clampedValue;
    }

    return clampedValue;
}

QSize FluentRating::sizeHint() const {
    const int totalItems = static_cast<int>(m_maximum - m_minimum);
    const int spacing = (totalItems - 1) * m_spacing;

    if (m_orientation == FluentRatingOrientation::Horizontal) {
        int width = totalItems * m_itemSize + spacing;
        int height = m_itemSize;

        if (m_showValue) {
            const QFontMetrics fm(font());
            const QString sampleText = QString::number(m_maximum, 'f', 1);
            width += m_spacing + fm.horizontalAdvance(sampleText);
            height = qMax(height, fm.height());
        }

        return QSize(width, height);
    } else {
        int width = m_itemSize;
        int height = totalItems * m_itemSize + spacing;

        if (m_showValue) {
            const QFontMetrics fm(font());
            const QString sampleText = QString::number(m_maximum, 'f', 1);
            width = qMax(width, fm.horizontalAdvance(sampleText));
            height += m_spacing + fm.height();
        }

        return QSize(width, height);
    }
}

QSize FluentRating::minimumSizeHint() const {
    const int totalItems = static_cast<int>(m_maximum - m_minimum);
    if (m_orientation == FluentRatingOrientation::Horizontal) {
        return QSize(m_itemSize * totalItems + (totalItems - 1) * m_spacing,
                     m_itemSize);
    } else {
        return QSize(m_itemSize,
                     m_itemSize * totalItems + (totalItems - 1) * m_spacing);
    }
}

FluentRating* FluentRating::createStarRating(int maxStars, QWidget* parent) {
    auto* rating = new FluentRating(maxStars, parent);
    rating->setRatingStyle(FluentRatingStyle::Stars);
    return rating;
}

FluentRating* FluentRating::createHeartRating(int maxHearts, QWidget* parent) {
    auto* rating = new FluentRating(maxHearts, parent);
    rating->setRatingStyle(FluentRatingStyle::Hearts);
    return rating;
}

FluentRating* FluentRating::createThumbsRating(QWidget* parent) {
    auto* rating = new FluentRating(1, parent);
    rating->setRatingStyle(FluentRatingStyle::Thumbs);
    return rating;
}

// Public slots implementation
void FluentRating::reset() { setValue(m_minimum); }

void FluentRating::animateToValue(double value) {
    if (m_animated) {
        startValueAnimation(value);
    } else {
        setValue(value);
    }
}

void FluentRating::highlightItem(int index) {
    if (index >= 0 && index < itemCount()) {
        m_highlightedIndex = index;
        if (m_animated) {
            startHighlightAnimation(index);
        }
        update();
    }
}

void FluentRating::clearHighlight() {
    m_highlightedIndex = -1;
    update();
}

void FluentRating::setupAnimations() {
    using namespace FluentQt::Animation;

    // Value change animation using FluentAnimator with authentic Fluent Design
    // motion
    auto valueAnimConfig = FluentAnimationConfig::uiTransition();
    valueAnimConfig.duration = std::chrono::milliseconds(300);
    valueAnimConfig.easing = FluentEasing::FluentStandard;

    m_valueAnimation =
        FluentAnimator::primaryMotion(this, valueAnimConfig).release();
    m_valueAnimation->setPropertyName("animatedValue");
    connect(m_valueAnimation, &QPropertyAnimation::finished, this,
            &FluentRating::onValueAnimationFinished);

    // Highlight animation using FluentAnimator micro-interaction
    auto highlightAnimConfig = FluentAnimationConfig::microInteraction();
    highlightAnimConfig.duration = std::chrono::milliseconds(150);
    highlightAnimConfig.easing = FluentEasing::FluentButton;

    m_highlightAnimation =
        FluentAnimator::utilityMotion(this, highlightAnimConfig).release();
    m_highlightAnimation->setPropertyName("highlightOpacity");
    connect(m_highlightAnimation, &QPropertyAnimation::finished, this,
            &FluentRating::onHighlightAnimationFinished);

    // Scale animation for press effect using FluentAnimator
    m_scaleAnimation = new QSequentialAnimationGroup(this);

    // Create press effect animations with authentic Fluent Design timing
    auto pressConfig = FluentAnimationConfig::buttonInteraction();
    auto scaleDownAnim = FluentAnimator::pressEffect(this, pressConfig);
    auto scaleUpAnim = FluentAnimator::hoverEffect(this, pressConfig);

    if (scaleDownAnim && scaleUpAnim) {
        m_scaleAnimation->addAnimation(scaleDownAnim.release());
        m_scaleAnimation->addAnimation(scaleUpAnim.release());
    }
}

void FluentRating::updateSizeMetrics() {
    // Use FluentUI design specifications for sizing and spacing
    switch (m_ratingSize) {
        case FluentRatingSize::Small:
            // FluentUI Small: 16px items with 4px spacing
            m_itemSize = 16;
            m_spacing = FLUENT_SPACING("xs");  // 4px
            break;
        case FluentRatingSize::Medium:
            // FluentUI Medium: 20px items with 4px spacing (default)
            m_itemSize = 20;
            m_spacing = FLUENT_SPACING("xs");  // 4px
            break;
        case FluentRatingSize::Large:
            // FluentUI Large: 24px items with 8px spacing
            m_itemSize = 24;
            m_spacing = FLUENT_SPACING("s");  // 8px
            break;
        case FluentRatingSize::ExtraLarge:
            // FluentUI Extra Large: 32px items with 12px spacing
            m_itemSize = 32;
            m_spacing = FLUENT_SPACING("m");  // 12px
            break;
    }
}

void FluentRating::updateLayout() {
    if (!m_layoutDirty)
        return;

    const int totalItems = static_cast<int>(m_maximum - m_minimum);
    m_itemRects.clear();
    m_itemRects.reserve(totalItems);

    const QRect rect = this->rect();
    QPoint startPos = rect.topLeft();

    // Calculate value text area if needed
    if (m_showValue) {
        const QFontMetrics fm(font());
        const QString valueText = QString::number(m_value, 'f', 1);
        const QSize textSize = fm.size(Qt::TextSingleLine, valueText);

        if (m_orientation == FluentRatingOrientation::Horizontal) {
            m_valueTextRect = QRect(rect.right() - textSize.width(),
                                    rect.center().y() - textSize.height() / 2,
                                    textSize.width(), textSize.height());
        } else {
            m_valueTextRect = QRect(rect.center().x() - textSize.width() / 2,
                                    rect.bottom() - textSize.height(),
                                    textSize.width(), textSize.height());
        }
    } else {
        m_valueTextRect = QRect();
    }

    // Calculate item positions
    for (int i = 0; i < totalItems; ++i) {
        QRect itemRect;

        if (m_orientation == FluentRatingOrientation::Horizontal) {
            const int x = startPos.x() + i * (m_itemSize + m_spacing);
            const int y = rect.center().y() - m_itemSize / 2;
            itemRect = QRect(x, y, m_itemSize, m_itemSize);
        } else {
            const int x = rect.center().x() - m_itemSize / 2;
            const int y = startPos.y() + i * (m_itemSize + m_spacing);
            itemRect = QRect(x, y, m_itemSize, m_itemSize);
        }

        m_itemRects.append(itemRect);
    }

    m_layoutDirty = false;
}

// Private helper methods
void FluentRating::updateIcons() {
    // Update default icons based on rating style
    // This will be implemented when we add icon support
}

void FluentRating::updateItemLayout() { updateLayout(); }

void FluentRating::updateTooltip(double value) {
    if (m_showTooltip) {
        setToolTip(tooltipText(value));
    }
}

void FluentRating::showTooltipAt(const QPoint& position, double value) {
    if (m_showTooltip) {
        QToolTip::showText(position, tooltipText(value), this);
    }
}

void FluentRating::hideTooltip() { QToolTip::hideText(); }

void FluentRating::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_layoutDirty) {
        updateLayout();
    }

    // Paint items
    paintItems(&painter);

    // Paint value text
    if (m_showValue) {
        paintValueText(&painter);
    }

    // Paint focus indicator
    if (hasFocus()) {
        paintFocusIndicator(&painter);
    }
}

void FluentRating::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !m_readOnly && isEnabled()) {
        const double newValue = valueFromPosition(event->pos());
        setValue(newValue);

        if (m_animated) {
            animateItemScale(static_cast<int>(newValue - m_minimum));
        }

        emit clicked(newValue);
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentRating::mouseMoveEvent(QMouseEvent* event) {
    if (!m_readOnly && isEnabled()) {
        const double newHoverValue = valueFromPosition(event->pos());

        if (!qFuzzyCompare(m_hoverValue, newHoverValue)) {
            m_hoverValue = newHoverValue;

            // Enhanced hover feedback with smooth transitions
            if (m_animated) {
                startHighlightAnimation(
                    static_cast<int>(newHoverValue - m_minimum));
            }

            update();
            emit hovered(m_hoverValue);

            // Show tooltip if enabled with enhanced positioning
            if (m_showTooltip) {
                showTooltipAt(event->globalPosition().toPoint(), m_hoverValue);
            }
        }
    }

    Core::FluentComponent::mouseMoveEvent(event);
}

void FluentRating::keyPressEvent(QKeyEvent* event) {
    if (m_readOnly || !isEnabled()) {
        Core::FluentComponent::keyPressEvent(event);
        return;
    }

    // Enhanced keyboard navigation with comprehensive FluentUI support
    const double step = (m_precision == FluentRatingPrecision::Full)      ? 1.0
                        : (m_precision == FluentRatingPrecision::Half)    ? 0.5
                        : (m_precision == FluentRatingPrecision::Quarter) ? 0.25
                                                                          : 0.1;

    bool handled = true;
    double newValue = m_value;

    switch (event->key()) {
        // Primary navigation keys
        case Qt::Key_Left:
        case Qt::Key_Down:
            newValue = qMax(m_value - step, m_minimum);
            break;

        case Qt::Key_Right:
        case Qt::Key_Up:
            newValue = qMin(m_value + step, m_maximum);
            break;

        // Jump to extremes
        case Qt::Key_Home:
            newValue = m_minimum;
            break;

        case Qt::Key_End:
            newValue = m_maximum;
            break;

        // Page navigation for larger steps
        case Qt::Key_PageUp:
            newValue = qMin(m_value + (step * 5), m_maximum);
            break;

        case Qt::Key_PageDown:
            newValue = qMax(m_value - (step * 5), m_minimum);
            break;

        // Reset functionality
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            if (m_allowReset) {
                newValue = m_minimum;
            } else {
                handled = false;
            }
            break;

        // Space and Enter for activation/confirmation
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Trigger click signal for current value
            emit clicked(m_value);
            break;

        // Escape to cancel/reset
        case Qt::Key_Escape:
            if (m_allowReset) {
                newValue = m_minimum;
            } else {
                handled = false;
            }
            break;

        // Direct numeric input
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9: {
            const int digit = event->key() - Qt::Key_0;
            const double targetValue = m_minimum + digit;
            if (targetValue <= m_maximum) {
                newValue = targetValue;
            } else {
                handled = false;
            }
            break;
        }

        // Plus/Minus for fine adjustment
        case Qt::Key_Plus:
        case Qt::Key_Equal:  // For + without shift
            newValue = qMin(m_value + step, m_maximum);
            break;

        case Qt::Key_Minus:
            newValue = qMax(m_value - step, m_minimum);
            break;

        default:
            handled = false;
            break;
    }

    if (handled) {
        if (!qFuzzyCompare(newValue, m_value)) {
            setValue(newValue);

            // Provide audio feedback for accessibility
            if (m_animated) {
                animateItemScale(static_cast<int>(newValue - m_minimum));
            }
        }
        event->accept();
    } else {
        Core::FluentComponent::keyPressEvent(event);
    }
}

void FluentRating::leaveEvent(QEvent* event) {
    m_hoverValue = -1.0;
    m_highlightedIndex = -1;
    setState(Core::FluentState::Normal);

    // Smooth transition out of hover state
    if (m_animated) {
        m_highlightOpacity = 1.0;
    }

    // Hide tooltip when leaving
    hideTooltip();

    update();
    Core::FluentComponent::leaveEvent(event);
}

void FluentRating::focusInEvent(QFocusEvent* event) {
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentRating::focusOutEvent(QFocusEvent* event) {
    update();
    Core::FluentComponent::focusOutEvent(event);
}

void FluentRating::resizeEvent(QResizeEvent* event) {
    m_layoutDirty = true;
    Core::FluentComponent::resizeEvent(event);
}

// Private slots implementation
void FluentRating::onValueAnimationFinished() {
    m_animatedValue = m_value;
    update();
}

void FluentRating::onHighlightAnimationFinished() {
    // Animation finished, nothing special to do
    update();
}

void FluentRating::updateColors() {
    using namespace FluentQt::Styling;

    // Use design tokens for colors
    if (isEnabled() && !m_readOnly) {
        if (!m_filledColor.isValid()) {
            // Use brand color for filled items
            m_filledColor = FLUENT_BRAND_COLOR(100);
        }
        if (!m_emptyColor.isValid()) {
            // Use neutral color for empty items
            m_emptyColor = FLUENT_NEUTRAL_COLOR(30);
        }
    } else {
        // Use disabled colors from design tokens
        m_filledColor = FLUENT_NEUTRAL_COLOR(40);
        m_emptyColor = FLUENT_NEUTRAL_COLOR(20);
    }

    update();
}

// Painting methods implementation
void FluentRating::paintItems(QPainter* painter) {
    const double displayValue =
        (m_hoverValue >= 0) ? m_hoverValue : m_animatedValue;
    const int totalItems = static_cast<int>(m_maximum - m_minimum);

    for (int i = 0; i < totalItems && i < m_itemRects.size(); ++i) {
        const QRect& itemRect = m_itemRects[i];
        const double itemValue = displayValue - (m_minimum + i);
        const double fillRatio = qBound(0.0, itemValue, 1.0);

        paintItem(painter, i, itemRect, fillRatio);
    }
}

void FluentRating::paintItem(QPainter* painter, int index, const QRect& rect,
                             double fillRatio) {
    painter->save();

    // Apply scale transformation if needed
    if (m_itemScale != 1.0) {
        const QPointF center = rect.center();
        painter->translate(center);
        painter->scale(m_itemScale, m_itemScale);
        painter->translate(-center);
    }

    // Determine colors
    QColor fillColor = m_filledColor;
    QColor emptyColor = m_emptyColor;

    // Apply highlight if this item is highlighted
    if (index == m_highlightedIndex) {
        fillColor.setAlphaF(fillColor.alphaF() * m_highlightOpacity);
    }

    // Paint based on style - simplified implementation for now
    QColor color = fillRatio > 0.5 ? fillColor : emptyColor;
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);

    switch (m_ratingStyle) {
        case FluentRatingStyle::Stars:
            // Draw a simple star shape
            painter->drawEllipse(rect.adjusted(2, 2, -2, -2));
            break;

        case FluentRatingStyle::Hearts:
            // Draw a simple heart shape (circle for now)
            painter->drawEllipse(rect.adjusted(2, 2, -2, -2));
            break;

        case FluentRatingStyle::Thumbs:
            // Draw a simple thumb shape (rectangle for now)
            painter->drawRect(rect.adjusted(4, 4, -4, -4));
            break;

        case FluentRatingStyle::Dots:
            // Draw a simple dot
            painter->drawEllipse(rect.center(), rect.width() / 4,
                                 rect.height() / 4);
            break;

        case FluentRatingStyle::Custom:
            // Use custom icon if available
            if (!m_filledIcon.isNull() && fillRatio > 0.5) {
                paintIcon(painter, rect, m_filledIcon, 1.0);
            } else if (!m_emptyIcon.isNull()) {
                paintIcon(painter, rect, m_emptyIcon, 1.0);
            } else {
                painter->drawRect(rect.adjusted(2, 2, -2, -2));
            }
            break;
    }

    painter->restore();
}

void FluentRating::paintIcon(QPainter* painter, const QRect& rect,
                             const QIcon& icon, double opacity) {
    if (icon.isNull())
        return;

    painter->save();
    painter->setOpacity(opacity);

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    QIcon::State state = QIcon::Off;

    QPixmap pixmap = icon.pixmap(rect.size(), mode, state);
    painter->drawPixmap(rect, pixmap);

    painter->restore();
}

// Animation methods implementation using FluentAnimator
void FluentRating::startValueAnimation(double targetValue) {
    if (!m_valueAnimation)
        return;

    // Use FluentAnimator's smooth value transition with authentic Fluent Design
    // motion
    m_valueAnimation->stop();
    m_valueAnimation->setStartValue(m_animatedValue);
    m_valueAnimation->setEndValue(targetValue);

    // Apply Fluent Design easing for smooth, natural motion
    m_valueAnimation->setEasingCurve(QEasingCurve::Type(
        static_cast<int>(FluentQt::Animation::FluentEasing::FluentStandard)));
    m_valueAnimation->start();
}

void FluentRating::startHighlightAnimation(int index) {
    Q_UNUSED(index)
    if (!m_highlightAnimation)
        return;

    // Use FluentAnimator's micro-interaction timing for responsive feedback
    m_highlightAnimation->stop();
    m_highlightAnimation->setStartValue(m_highlightOpacity);
    m_highlightAnimation->setEndValue(1.0);

    // Apply Fluent Design button easing for micro-interactions
    m_highlightAnimation->setEasingCurve(QEasingCurve::Type(
        static_cast<int>(FluentQt::Animation::FluentEasing::FluentButton)));
    m_highlightAnimation->start();
}

void FluentRating::animateItemScale(int index) {
    Q_UNUSED(index)
    if (!m_scaleAnimation)
        return;

    // Use FluentAnimator's authentic press effect with Fluent Design timing
    using namespace FluentQt::Animation;

    auto pressConfig = FluentAnimationConfig::buttonInteraction();
    pressConfig.duration = std::chrono::milliseconds(100);
    pressConfig.easing = FluentEasing::FluentButton;

    // Create scale down animation with Fluent Design easing
    auto* scaleDown = new QPropertyAnimation(this, "itemScale");
    scaleDown->setDuration(100);
    scaleDown->setStartValue(1.0);
    scaleDown->setEndValue(0.95);
    scaleDown->setEasingCurve(
        QEasingCurve::Type(static_cast<int>(FluentEasing::FluentButton)));

    // Create scale up animation with Fluent Design easing
    auto* scaleUp = new QPropertyAnimation(this, "itemScale");
    scaleUp->setDuration(100);
    scaleUp->setStartValue(0.95);
    scaleUp->setEndValue(1.0);
    scaleUp->setEasingCurve(
        QEasingCurve::Type(static_cast<int>(FluentEasing::FluentButton)));

    m_scaleAnimation->clear();
    m_scaleAnimation->addAnimation(scaleDown);
    m_scaleAnimation->addAnimation(scaleUp);
    m_scaleAnimation->start();
}

// Utility methods implementation
QRect FluentRating::calculateItemRect(int index) const {
    return itemRect(index);
}

QRect FluentRating::calculateValueTextRect() const { return m_valueTextRect; }

double FluentRating::calculateFillRatio(int index, double value) const {
    const double itemValue = value - (m_minimum + index);
    return qBound(0.0, itemValue, 1.0);
}

QIcon FluentRating::getItemIcon(double fillRatio) const {
    if (fillRatio >= 1.0) {
        return m_filledIcon.isNull() ? QIcon() : m_filledIcon;
    } else if (fillRatio >= 0.5 && !m_halfIcon.isNull()) {
        return m_halfIcon;
    } else {
        return m_emptyIcon.isNull() ? QIcon() : m_emptyIcon;
    }
}

QColor FluentRating::getItemColor(double fillRatio) const {
    return fillRatio > 0.5 ? m_filledColor : m_emptyColor;
}

// Additional missing methods implementation
void FluentRating::mouseReleaseEvent(QMouseEvent* event) {
    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentRating::wheelEvent(QWheelEvent* event) {
    if (m_readOnly || !isEnabled()) {
        Core::FluentComponent::wheelEvent(event);
        return;
    }

    const double step = (m_precision == FluentRatingPrecision::Full)      ? 1.0
                        : (m_precision == FluentRatingPrecision::Half)    ? 0.5
                        : (m_precision == FluentRatingPrecision::Quarter) ? 0.25
                                                                          : 0.1;

    const double delta =
        event->angleDelta().y() / 120.0;  // Standard wheel step
    const double newValue = m_value + (delta * step);
    setValue(qBound(m_minimum, newValue, m_maximum));

    event->accept();
}

void FluentRating::paintValueText(QPainter* painter) {
    if (m_valueTextRect.isEmpty())
        return;

    painter->save();

    // Use design tokens for text color
    QColor textColor = isEnabled() ? FLUENT_COLOR("color.text.primary")
                                   : FLUENT_NEUTRAL_COLOR(40);
    painter->setPen(textColor);
    painter->setFont(font());

    const QString valueText = QString::number(m_value, 'f', 1);
    painter->drawText(m_valueTextRect, Qt::AlignCenter, valueText);

    painter->restore();
}

void FluentRating::paintFocusIndicator(QPainter* painter) {
    if (!hasFocus())
        return;

    painter->save();

    // Enhanced focus indicator following FluentUI design principles
    const QColor focusColor = FLUENT_BRAND_COLOR(100);
    const QColor focusBackgroundColor =
        QColor(focusColor.red(), focusColor.green(), focusColor.blue(), 20);

    // Draw focus background with subtle transparency
    painter->setPen(Qt::NoPen);
    painter->setBrush(focusBackgroundColor);
    const QRect focusBackgroundRect = rect().adjusted(-2, -2, 2, 2);
    painter->drawRoundedRect(focusBackgroundRect, 4, 4);

    // Draw focus border with FluentUI style
    QPen focusPen(focusColor, 2);
    focusPen.setStyle(Qt::SolidLine);
    painter->setPen(focusPen);
    painter->setBrush(Qt::NoBrush);

    const QRect focusRect = rect().adjusted(1, 1, -1, -1);
    painter->drawRoundedRect(focusRect, 2, 2);

    // Add inner glow effect for better visibility
    QPen innerGlowPen(QColor(255, 255, 255, 100), 1);
    painter->setPen(innerGlowPen);
    const QRect innerRect = focusRect.adjusted(1, 1, -1, -1);
    painter->drawRoundedRect(innerRect, 1, 1);

    painter->restore();
}

// Property accessors for animations (already implemented in header)
void FluentRating::setAnimatedValue(double value) {
    if (qFuzzyCompare(m_animatedValue, value))
        return;
    m_animatedValue = value;
    update();
}

void FluentRating::setHighlightOpacity(qreal opacity) {
    if (qFuzzyCompare(m_highlightOpacity, opacity))
        return;
    m_highlightOpacity = opacity;
    update();
}

void FluentRating::setItemScale(qreal scale) {
    if (qFuzzyCompare(m_itemScale, scale))
        return;
    m_itemScale = scale;
    update();
}

}  // namespace FluentQt::Components
