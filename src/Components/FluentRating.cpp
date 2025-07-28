// src/Components/FluentRating.cpp
#include "FluentQt/Components/FluentRating.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QApplication>
#include <QFontMetrics>
#include <QAccessible>
#include <QToolTip>
#include <cmath>

namespace FluentQt::Components {

FluentRating::FluentRating(QWidget* parent)
    : Core::FluentComponent(parent)
    , m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setupAnimations();
    updateSizeMetrics();
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);
    
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentRating::updateColors);
    
    updateColors();
    updateLayout();
    updateAccessibility();
}

FluentRating::FluentRating(int maxRating, QWidget* parent)
    : FluentRating(parent) {
    setMaxRating(maxRating);
}

FluentRating::FluentRating(double rating, int maxRating, QWidget* parent)
    : FluentRating(maxRating, parent) {
    setRating(rating);
}

FluentRating::~FluentRating() = default;

double FluentRating::rating() const {
    return m_rating;
}

void FluentRating::setRating(double rating) {
    const double boundedRating = qBound(0.0, rating, static_cast<double>(m_maxRating));
    if (qFuzzyCompare(m_rating, boundedRating)) return;
    
    m_rating = boundedRating;
    
    if (m_animated) {
        animateRatingChange();
    } else {
        m_animatedRating = m_rating;
        update();
    }
    
    emit ratingChanged(m_rating);
    updateAccessibility();
}

int FluentRating::maxRating() const {
    return m_maxRating;
}

void FluentRating::setMaxRating(int maxRating) {
    if (m_maxRating == maxRating) return;
    
    m_maxRating = qMax(1, maxRating);
    
    // Adjust current rating if it exceeds new maximum
    if (m_rating > m_maxRating) {
        setRating(m_maxRating);
    }
    
    updateLayout();
    updateGeometry();
    update();
    emit maxRatingChanged(m_maxRating);
}

FluentRatingStyle FluentRating::ratingStyle() const {
    return m_ratingStyle;
}

void FluentRating::setRatingStyle(FluentRatingStyle style) {
    if (m_ratingStyle == style) return;
    
    m_ratingStyle = style;
    updateSizeMetrics();
    updateLayout();
    updateGeometry();
    update();
    emit ratingStyleChanged(m_ratingStyle);
}

FluentRatingPrecision FluentRating::precision() const {
    return m_precision;
}

void FluentRating::setPrecision(FluentRatingPrecision precision) {
    if (m_precision == precision) return;
    
    m_precision = precision;
    
    // Adjust current rating to match new precision
    setRating(snapToGrid(m_rating));
    
    emit precisionChanged(m_precision);
}

FluentRatingSize FluentRating::ratingSize() const {
    return m_ratingSize;
}

void FluentRating::setRatingSize(FluentRatingSize size) {
    if (m_ratingSize == size) return;
    
    m_ratingSize = size;
    updateSizeMetrics();
    updateLayout();
    updateGeometry();
    update();
    emit ratingSizeChanged(m_ratingSize);
}

Qt::Orientation FluentRating::orientation() const {
    return m_orientation;
}

void FluentRating::setOrientation(Qt::Orientation orientation) {
    if (m_orientation == orientation) return;
    
    m_orientation = orientation;
    updateLayout();
    updateGeometry();
    update();
    emit orientationChanged(m_orientation);
}

bool FluentRating::readOnly() const {
    return m_readOnly;
}

void FluentRating::setReadOnly(bool readOnly) {
    if (m_readOnly == readOnly) return;
    
    m_readOnly = readOnly;
    setCursor(m_readOnly ? Qt::ArrowCursor : Qt::PointingHandCursor);
    updateColors();
    emit readOnlyChanged(m_readOnly);
}

bool FluentRating::showValue() const {
    return m_showValue;
}

void FluentRating::setShowValue(bool show) {
    if (m_showValue == show) return;
    
    m_showValue = show;
    updateLayout();
    updateGeometry();
    update();
    emit showValueChanged(m_showValue);
}

bool FluentRating::showTooltip() const {
    return m_showTooltip;
}

void FluentRating::setShowTooltip(bool show) {
    if (m_showTooltip == show) return;
    
    m_showTooltip = show;
    emit showTooltipChanged(m_showTooltip);
}

bool FluentRating::isAnimated() const {
    return m_animated;
}

void FluentRating::setAnimated(bool animated) {
    if (m_animated == animated) return;
    
    m_animated = animated;
    emit animatedChanged(m_animated);
}

QIcon FluentRating::customIcon() const {
    return m_customIcon;
}

void FluentRating::setCustomIcon(const QIcon& icon) {
    if (m_customIcon.cacheKey() == icon.cacheKey()) return;
    
    m_customIcon = icon;
    
    if (!m_customIcon.isNull()) {
        setRatingStyle(FluentRatingStyle::Custom);
    }
    
    update();
    emit customIconChanged(m_customIcon);
}

QStringList FluentRating::ratingLabels() const {
    return m_ratingLabels;
}

void FluentRating::setRatingLabels(const QStringList& labels) {
    if (m_ratingLabels == labels) return;
    
    m_ratingLabels = labels;
    emit ratingLabelsChanged(m_ratingLabels);
}

double FluentRating::hoverRating() const {
    return m_hoverRating;
}

bool FluentRating::isHovering() const {
    return m_hovering;
}

QSize FluentRating::itemSize() const {
    return QSize(m_itemSize, m_itemSize);
}

QRect FluentRating::itemRect(int index) const {
    if (index < 0 || index >= m_maxRating) {
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

QSize FluentRating::sizeHint() const {
    const int totalItems = m_maxRating;
    const int spacing = (totalItems - 1) * m_spacing;
    
    if (m_orientation == Qt::Horizontal) {
        int width = totalItems * m_itemSize + spacing;
        int height = m_itemSize;
        
        if (m_showValue) {
            const QFontMetrics fm(font());
            const QString sampleText = QString::number(m_maxRating, 'f', 1);
            width += m_spacing + fm.horizontalAdvance(sampleText);
            height = qMax(height, fm.height());
        }
        
        return QSize(width, height);
    } else {
        int width = m_itemSize;
        int height = totalItems * m_itemSize + spacing;
        
        if (m_showValue) {
            const QFontMetrics fm(font());
            const QString sampleText = QString::number(m_maxRating, 'f', 1);
            width = qMax(width, fm.horizontalAdvance(sampleText));
            height += m_spacing + fm.height();
        }
        
        return QSize(width, height);
    }
}

QSize FluentRating::minimumSizeHint() const {
    if (m_orientation == Qt::Horizontal) {
        return QSize(m_itemSize * m_maxRating + (m_maxRating - 1) * m_spacing, m_itemSize);
    } else {
        return QSize(m_itemSize, m_itemSize * m_maxRating + (m_maxRating - 1) * m_spacing);
    }
}

FluentRating* FluentRating::createStarRating(int maxRating, QWidget* parent) {
    auto* rating = new FluentRating(maxRating, parent);
    rating->setRatingStyle(FluentRatingStyle::Stars);
    return rating;
}

FluentRating* FluentRating::createHeartRating(int maxRating, QWidget* parent) {
    auto* rating = new FluentRating(maxRating, parent);
    rating->setRatingStyle(FluentRatingStyle::Hearts);
    return rating;
}

FluentRating* FluentRating::createThumbsRating(QWidget* parent) {
    auto* rating = new FluentRating(1, parent);
    rating->setRatingStyle(FluentRatingStyle::Thumbs);
    return rating;
}

FluentRating* FluentRating::createDotRating(int maxRating, QWidget* parent) {
    auto* rating = new FluentRating(maxRating, parent);
    rating->setRatingStyle(FluentRatingStyle::Dots);
    return rating;
}

void FluentRating::clear() {
    setRating(0.0);
}

void FluentRating::setFullRating() {
    setRating(m_maxRating);
}

void FluentRating::stepUp() {
    const double step = getStepSize();
    setRating(qMin(m_rating + step, static_cast<double>(m_maxRating)));
}

void FluentRating::stepDown() {
    const double step = getStepSize();
    setRating(qMax(m_rating - step, 0.0));
}

void FluentRating::setupAnimations() {
    // Rating change animation
    m_ratingAnimation = new QPropertyAnimation(this, "animatedRating", this);
    m_ratingAnimation->setDuration(300);
    m_ratingAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_ratingAnimation, &QPropertyAnimation::finished,
            this, &FluentRating::onRatingAnimationFinished);
    
    // Hover animation
    m_hoverAnimation = new QPropertyAnimation(this, "hoverOpacity", this);
    m_hoverAnimation->setDuration(150);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Scale animation for press effect
    m_scaleAnimation = new QPropertyAnimation(this, "itemScale", this);
    m_scaleAnimation->setDuration(100);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutBack);
}

void FluentRating::updateSizeMetrics() {
    switch (m_ratingSize) {
        case FluentRatingSize::Small:
            m_itemSize = 16;
            m_spacing = 4;
            break;
        case FluentRatingSize::Medium:
            m_itemSize = 24;
            m_spacing = 6;
            break;
        case FluentRatingSize::Large:
            m_itemSize = 32;
            m_spacing = 8;
            break;
    }
}

void FluentRating::updateLayout() {
    if (!m_layoutDirty) return;

    m_itemRects.clear();
    m_itemRects.reserve(m_maxRating);

    const QRect rect = this->rect();
    QPoint startPos = rect.topLeft();

    // Calculate value text area if needed
    QRect valueRect;
    if (m_showValue) {
        const QFontMetrics fm(font());
        const QString valueText = QString::number(m_rating, 'f', 1);
        const QSize textSize = fm.size(Qt::TextSingleLine, valueText);

        if (m_orientation == Qt::Horizontal) {
            valueRect = QRect(
                rect.right() - textSize.width(),
                rect.center().y() - textSize.height() / 2,
                textSize.width(),
                textSize.height()
            );
        } else {
            valueRect = QRect(
                rect.center().x() - textSize.width() / 2,
                rect.bottom() - textSize.height(),
                textSize.width(),
                textSize.height()
            );
        }
    }

    m_valueRect = valueRect;

    // Calculate item positions
    for (int i = 0; i < m_maxRating; ++i) {
        QRect itemRect;

        if (m_orientation == Qt::Horizontal) {
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

double FluentRating::snapToGrid(double value) const {
    switch (m_precision) {
        case FluentRatingPrecision::Full:
            return std::round(value);

        case FluentRatingPrecision::Half:
            return std::round(value * 2.0) / 2.0;

        case FluentRatingPrecision::Quarter:
            return std::round(value * 4.0) / 4.0;

        case FluentRatingPrecision::Exact:
            return value;
    }

    return value;
}

double FluentRating::getStepSize() const {
    switch (m_precision) {
        case FluentRatingPrecision::Full:
            return 1.0;
        case FluentRatingPrecision::Half:
            return 0.5;
        case FluentRatingPrecision::Quarter:
            return 0.25;
        case FluentRatingPrecision::Exact:
            return 0.1;
    }
    return 1.0;
}

double FluentRating::ratingFromPosition(const QPoint& pos) const {
    if (m_layoutDirty) {
        const_cast<FluentRating*>(this)->updateLayout();
    }

    for (int i = 0; i < m_itemRects.size(); ++i) {
        const QRect& itemRect = m_itemRects[i];

        if (itemRect.contains(pos)) {
            double rating = i + 1.0;

            // Calculate fractional part based on precision and position within item
            if (m_precision != FluentRatingPrecision::Full) {
                double fraction = 0.0;

                if (m_orientation == Qt::Horizontal) {
                    fraction = static_cast<double>(pos.x() - itemRect.left()) / itemRect.width();
                } else {
                    fraction = 1.0 - static_cast<double>(pos.y() - itemRect.top()) / itemRect.height();
                }

                fraction = qBound(0.0, fraction, 1.0);
                rating = i + fraction;
            }

            return snapToGrid(rating);
        }
    }

    return m_rating;
}

void FluentRating::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_layoutDirty) {
        updateLayout();
    }

    // Paint rating items
    paintRatingItems(&painter);

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
        const double newRating = ratingFromPosition(event->pos());
        setRating(newRating);

        if (m_animated) {
            startPressAnimation();
        }

        emit clicked(newRating);
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentRating::mouseMoveEvent(QMouseEvent* event) {
    if (!m_readOnly && isEnabled()) {
        const double newHoverRating = ratingFromPosition(event->pos());

        if (!qFuzzyCompare(m_hoverRating, newHoverRating)) {
            m_hoverRating = newHoverRating;
            update();
            emit hoverRatingChanged(m_hoverRating);

            // Show tooltip if enabled
            if (m_showTooltip) {
                showRatingTooltip(event->globalPosition().toPoint());
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

    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Down:
            stepDown();
            event->accept();
            return;

        case Qt::Key_Right:
        case Qt::Key_Up:
            stepUp();
            event->accept();
            return;

        case Qt::Key_Home:
            setRating(0.0);
            event->accept();
            return;

        case Qt::Key_End:
            setRating(m_maxRating);
            event->accept();
            return;

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
            if (digit <= m_maxRating) {
                setRating(digit);
                event->accept();
                return;
            }
            break;
        }
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentRating::enterEvent(QEnterEvent* event) {
    if (!m_readOnly && isEnabled()) {
        m_hovering = true;
        setState(Core::FluentState::Hovered);

        if (m_animated) {
            startHoverAnimation(true);
        }

        update();
    }

    Core::FluentComponent::enterEvent(event);
}

void FluentRating::leaveEvent(QEvent* event) {
    m_hovering = false;
    m_hoverRating = -1.0;
    setState(Core::FluentState::Normal);

    if (m_animated) {
        startHoverAnimation(false);
    }

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

void FluentRating::changeEvent(QEvent* event) {
    if (event->type() == QEvent::EnabledChange ||
        event->type() == QEvent::FontChange ||
        event->type() == QEvent::StyleChange) {
        updateColors();
        update();
    }
    Core::FluentComponent::changeEvent(event);
}

void FluentRating::onRatingAnimationFinished() {
    m_animatedRating = m_rating;
    update();
}

void FluentRating::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    if (isEnabled() && !m_readOnly) {
        m_filledColor = theme.color(Styling::FluentThemeColor::AccentFillDefault);
        m_unfilledColor = theme.color(Styling::FluentThemeColor::ControlStrokeDefault);
        m_hoverColor = theme.color(Styling::FluentThemeColor::AccentFillSecondary);
        m_textColor = theme.color(Styling::FluentThemeColor::TextFillPrimary);
    } else {
        m_filledColor = theme.color(Styling::FluentThemeColor::ControlFillDisabled);
        m_unfilledColor = theme.color(Styling::FluentThemeColor::ControlStrokeDisabled);
        m_hoverColor = m_filledColor;
        m_textColor = theme.color(Styling::FluentThemeColor::TextFillDisabled);
    }

    update();
}

void FluentRating::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        const QString accessibleText = tr("Rating: %1 out of %2").arg(m_rating).arg(m_maxRating);
        setAccessibleName(accessibleText);
        setAccessibleDescription(accessibleText);

        QAccessibleEvent event(this, QAccessible::ValueChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void FluentRating::paintRatingItems(QPainter* painter) {
    const double displayRating = m_hovering && m_hoverRating >= 0 ? m_hoverRating : m_animatedRating;

    for (int i = 0; i < m_maxRating; ++i) {
        const QRect& itemRect = m_itemRects[i];
        const double itemRating = displayRating - i;
        const bool isHovered = m_hovering && i < m_hoverRating;

        paintRatingItem(painter, itemRect, itemRating, isHovered, i);
    }
}

void FluentRating::paintRatingItem(QPainter* painter, const QRect& rect, double fillAmount, bool hovered, int index) {
    painter->save();

    // Apply scale transformation if needed
    if (m_itemScale != 1.0) {
        const QPointF center = rect.center();
        painter->translate(center);
        painter->scale(m_itemScale, m_itemScale);
        painter->translate(-center);
    }

    // Determine colors
    QColor fillColor = hovered ? m_hoverColor : m_filledColor;
    QColor emptyColor = m_unfilledColor;

    // Apply hover opacity
    if (hovered) {
        fillColor.setAlphaF(fillColor.alphaF() * m_hoverOpacity);
    }

    // Paint based on style
    switch (m_ratingStyle) {
        case FluentRatingStyle::Stars:
            paintStar(painter, rect, fillAmount, fillColor, emptyColor);
            break;

        case FluentRatingStyle::Hearts:
            paintHeart(painter, rect, fillAmount, fillColor, emptyColor);
            break;

        case FluentRatingStyle::Thumbs:
            paintThumb(painter, rect, fillAmount, fillColor, emptyColor);
            break;

        case FluentRatingStyle::Dots:
            paintDot(painter, rect, fillAmount, fillColor, emptyColor);
            break;

        case FluentRatingStyle::Custom:
            paintCustomIcon(painter, rect, fillAmount, fillColor, emptyColor);
            break;
    }

    painter->restore();
}

void FluentRating::paintStar(QPainter* painter, const QRect& rect, double fillAmount, const QColor& fillColor, const QColor& emptyColor) {
    const QPointF center = rect.center();
    const qreal radius = qMin(rect.width(), rect.height()) / 2.0 * 0.8;

    // Create star path
    QPainterPath starPath;
    const int points = 5;
    const qreal outerRadius = radius;
    const qreal innerRadius = radius * 0.4;

    for (int i = 0; i < points * 2; ++i) {
        const qreal angle = i * M_PI / points - M_PI / 2;
        const qreal r = (i % 2 == 0) ? outerRadius : innerRadius;
        const QPointF point(center.x() + r * cos(angle), center.y() + r * sin(angle));

        if (i == 0) {
            starPath.moveTo(point);
        } else {
            starPath.lineTo(point);
        }
    }
    starPath.closeSubpath();

    // Paint unfilled star
    painter->setPen(QPen(emptyColor, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(starPath);

    // Paint filled portion
    if (fillAmount > 0.0) {
        painter->setClipRect(rect.left(), rect.top(), rect.width() * qBound(0.0, fillAmount, 1.0), rect.height());
        painter->setBrush(fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawPath(starPath);
    }
}

void FluentRating::paintHeart(QPainter* painter, const QRect& rect, double fillAmount, const QColor& fillColor, const QColor& emptyColor) {
    const QPointF center = rect.center();
    const qreal size = qMin(rect.width(), rect.height()) * 0.8;

    // Create heart path (simplified)
    QPainterPath heartPath;
    const qreal w = size / 2;
    const qreal h = size / 2;

    heartPath.moveTo(center.x(), center.y() + h * 0.3);
    heartPath.cubicTo(center.x() - w, center.y() - h * 0.5, center.x() - w * 0.5, center.y() - h, center.x(), center.y() - h * 0.3);
    heartPath.cubicTo(center.x() + w * 0.5, center.y() - h, center.x() + w, center.y() - h * 0.5, center.x(), center.y() + h * 0.3);

    // Paint unfilled heart
    painter->setPen(QPen(emptyColor, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(heartPath);

    // Paint filled portion
    if (fillAmount > 0.0) {
        painter->setClipRect(rect.left(), rect.top(), rect.width() * qBound(0.0, fillAmount, 1.0), rect.height());
        painter->setBrush(fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawPath(heartPath);
    }
}

void FluentRating::paintThumb(QPainter* painter, const QRect& rect, double fillAmount, const QColor& fillColor, const QColor& emptyColor) {
    const QPointF center = rect.center();
    const qreal size = qMin(rect.width(), rect.height()) * 0.8;

    // Simple thumb up shape
    QPainterPath thumbPath;
    const qreal w = size * 0.3;
    const qreal h = size * 0.5;

    // Thumb
    thumbPath.addRect(center.x() - w/2, center.y() - h/4, w, h);
    // Thumb tip
    thumbPath.addRect(center.x() - w/4, center.y() - h/2, w/2, h/4);

    QColor color = fillAmount > 0.5 ? fillColor : emptyColor;
    painter->setPen(QPen(color, 1));
    painter->setBrush(fillAmount > 0.5 ? fillColor : Qt::NoBrush);
    painter->drawPath(thumbPath);
}

void FluentRating::paintDot(QPainter* painter, const QRect& rect, double fillAmount, const QColor& fillColor, const QColor& emptyColor) {
    const QPointF center = rect.center();
    const qreal radius = qMin(rect.width(), rect.height()) / 2.0 * 0.6;

    QColor color = fillAmount > 0.5 ? fillColor : emptyColor;
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawEllipse(center, radius, radius);
}

void FluentRating::paintCustomIcon(QPainter* painter, const QRect& rect, double fillAmount, const QColor& fillColor, const QColor& emptyColor) {
    if (m_customIcon.isNull()) return;

    QIcon::Mode mode = fillAmount > 0.5 ? QIcon::Normal : QIcon::Disabled;
    QIcon::State state = fillAmount > 0.5 ? QIcon::On : QIcon::Off;

    QPixmap pixmap = m_customIcon.pixmap(rect.size(), mode, state);
    painter->drawPixmap(rect, pixmap);
}

void FluentRating::paintValueText(QPainter* painter) {
    if (m_valueRect.isEmpty()) return;

    painter->save();

    painter->setPen(m_textColor);
    painter->setFont(font());

    const QString valueText = QString::number(m_rating, 'f', 1);
    painter->drawText(m_valueRect, Qt::AlignCenter, valueText);

    painter->restore();
}

void FluentRating::paintFocusIndicator(QPainter* painter) {
    if (!hasFocus()) return;

    painter->save();

    const auto& theme = Styling::FluentTheme::instance();
    const QColor focusColor = theme.color(Styling::FluentThemeColor::AccentFillDefault);

    QPen focusPen(focusColor, 2);
    focusPen.setStyle(Qt::DashLine);
    painter->setPen(focusPen);
    painter->setBrush(Qt::NoBrush);

    const QRect focusRect = rect().adjusted(1, 1, -1, -1);
    painter->drawRect(focusRect);

    painter->restore();
}

void FluentRating::animateRatingChange() {
    if (!m_ratingAnimation) return;

    m_ratingAnimation->stop();
    m_ratingAnimation->setStartValue(m_animatedRating);
    m_ratingAnimation->setEndValue(m_rating);
    m_ratingAnimation->start();
}

void FluentRating::startHoverAnimation(bool hover) {
    if (!m_hoverAnimation) return;

    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(m_hoverOpacity);
    m_hoverAnimation->setEndValue(hover ? 1.0 : 0.7);
    m_hoverAnimation->start();
}

void FluentRating::startPressAnimation() {
    if (!m_scaleAnimation) return;

    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(m_itemScale);
    m_scaleAnimation->setEndValue(0.95);
    m_scaleAnimation->start();

    // Return to normal scale
    QTimer::singleShot(m_scaleAnimation->duration(), this, [this]() {
        if (m_scaleAnimation) {
            m_scaleAnimation->setStartValue(m_itemScale);
            m_scaleAnimation->setEndValue(1.0);
            m_scaleAnimation->start();
        }
    });
}

void FluentRating::showRatingTooltip(const QPoint& globalPos) {
    QString tooltipText;

    if (m_hoverRating >= 0 && m_hoverRating <= m_maxRating) {
        if (!m_ratingLabels.isEmpty() && static_cast<int>(m_hoverRating) - 1 < m_ratingLabels.size()) {
            tooltipText = m_ratingLabels[static_cast<int>(m_hoverRating) - 1];
        } else {
            tooltipText = QString::number(m_hoverRating, 'f', 1);
        }
    }

    if (!tooltipText.isEmpty()) {
        QToolTip::showText(globalPos, tooltipText, this);
    }
}

// Property accessors for animations
double FluentRating::animatedRating() const {
    return m_animatedRating;
}

void FluentRating::setAnimatedRating(double rating) {
    if (qFuzzyCompare(m_animatedRating, rating)) return;
    m_animatedRating = rating;
    update();
}

qreal FluentRating::hoverOpacity() const {
    return m_hoverOpacity;
}

void FluentRating::setHoverOpacity(qreal opacity) {
    if (qFuzzyCompare(m_hoverOpacity, opacity)) return;
    m_hoverOpacity = opacity;
    update();
}

qreal FluentRating::itemScale() const {
    return m_itemScale;
}

void FluentRating::setItemScale(qreal scale) {
    if (qFuzzyCompare(m_itemScale, scale)) return;
    m_itemScale = scale;
    update();
}

} // namespace FluentQt::Components
