// src/Components/FluentSlider.cpp
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QApplication>
#include <QToolTip>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QtMath>

namespace FluentQt::Components {

FluentSlider::FluentSlider(QWidget* parent)
    : Core::FluentComponent(parent)
    , m_animator(std::make_unique<Animation::FluentAnimator>(this))
    , m_tooltipTimer(new QTimer(this))
{
    setupAnimations();
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    
    // Default value formatter
    m_valueFormatter = [](qreal value) {
        return QString::number(value, 'f', 1);
    };
    
    connect(m_tooltipTimer, &QTimer::timeout, this, &FluentSlider::onTooltipTimer);
    m_tooltipTimer->setSingleShot(true);
    
    updateLayout();
}

FluentSlider::FluentSlider(FluentSliderOrientation orientation, QWidget* parent)
    : FluentSlider(parent)
{
    setOrientation(orientation);
}

FluentSlider::FluentSlider(qreal minimum, qreal maximum, QWidget* parent)
    : FluentSlider(parent)
{
    setRange(minimum, maximum);
}

FluentSlider::FluentSlider(qreal minimum, qreal maximum, qreal value, QWidget* parent)
    : FluentSlider(minimum, maximum, parent)
{
    setValue(value);
}

FluentSlider::~FluentSlider() = default;

FluentSliderOrientation FluentSlider::orientation() const {
    return m_orientation;
}

void FluentSlider::setOrientation(FluentSliderOrientation orientation) {
    if (m_orientation != orientation) {
        m_orientation = orientation;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit orientationChanged(orientation);
    }
}

FluentSliderMode FluentSlider::mode() const {
    return m_mode;
}

void FluentSlider::setMode(FluentSliderMode mode) {
    if (m_mode != mode) {
        m_mode = mode;
        
        // Initialize range values if switching to range mode
        if (mode == FluentSliderMode::Range) {
            if (m_lowerValue >= m_upperValue) {
                m_lowerValue = m_minimum;
                m_upperValue = m_maximum;
            }
        }
        
        m_layoutDirty = true;
        updateLayout();
        update();
        emit modeChanged(mode);
    }
}

qreal FluentSlider::minimum() const {
    return m_minimum;
}

void FluentSlider::setMinimum(qreal minimum) {
    if (!qFuzzyCompare(m_minimum, minimum)) {
        m_minimum = minimum;
        
        if (m_maximum < m_minimum) {
            m_maximum = m_minimum;
        }
        
        constrainValues();
        m_layoutDirty = true;
        updateLayout();
        update();
        
        emit minimumChanged(minimum);
        emit rangeChanged(m_minimum, m_maximum);
    }
}

qreal FluentSlider::maximum() const {
    return m_maximum;
}

void FluentSlider::setMaximum(qreal maximum) {
    if (!qFuzzyCompare(m_maximum, maximum)) {
        m_maximum = maximum;
        
        if (m_minimum > m_maximum) {
            m_minimum = m_maximum;
        }
        
        constrainValues();
        m_layoutDirty = true;
        updateLayout();
        update();
        
        emit maximumChanged(maximum);
        emit rangeChanged(m_minimum, m_maximum);
    }
}

void FluentSlider::setRange(qreal minimum, qreal maximum) {
    if (minimum > maximum) {
        qSwap(minimum, maximum);
    }
    
    bool minChanged = !qFuzzyCompare(m_minimum, minimum);
    bool maxChanged = !qFuzzyCompare(m_maximum, maximum);
    
    if (minChanged || maxChanged) {
        m_minimum = minimum;
        m_maximum = maximum;
        
        constrainValues();
        m_layoutDirty = true;
        updateLayout();
        update();
        
        if (minChanged) emit minimumChanged(minimum);
        if (maxChanged) emit maximumChanged(maximum);
        emit rangeChanged(minimum, maximum);
    }
}

qreal FluentSlider::value() const {
    return m_value;
}

void FluentSlider::setValue(qreal value) {
    qreal constrainedValue = constrainValue(value);
    if (m_snapToTicks) {
        constrainedValue = snapValue(constrainedValue);
    }
    
    if (!qFuzzyCompare(m_value, constrainedValue)) {
        m_value = constrainedValue;
        updateHandlePositions();
        update();
        emit valueChanged(constrainedValue);
    }
}

qreal FluentSlider::lowerValue() const {
    return m_lowerValue;
}

void FluentSlider::setLowerValue(qreal value) {
    qreal constrainedValue = constrainValue(value);
    if (m_snapToTicks) {
        constrainedValue = snapValue(constrainedValue);
    }
    
    // Ensure lower value doesn't exceed upper value
    if (constrainedValue > m_upperValue) {
        constrainedValue = m_upperValue;
    }
    
    if (!qFuzzyCompare(m_lowerValue, constrainedValue)) {
        m_lowerValue = constrainedValue;
        updateHandlePositions();
        update();
        emit lowerValueChanged(constrainedValue);
        emit valuesChanged(m_lowerValue, m_upperValue);
    }
}

qreal FluentSlider::upperValue() const {
    return m_upperValue;
}

void FluentSlider::setUpperValue(qreal value) {
    qreal constrainedValue = constrainValue(value);
    if (m_snapToTicks) {
        constrainedValue = snapValue(constrainedValue);
    }
    
    // Ensure upper value doesn't go below lower value
    if (constrainedValue < m_lowerValue) {
        constrainedValue = m_lowerValue;
    }
    
    if (!qFuzzyCompare(m_upperValue, constrainedValue)) {
        m_upperValue = constrainedValue;
        updateHandlePositions();
        update();
        emit upperValueChanged(constrainedValue);
        emit valuesChanged(m_lowerValue, m_upperValue);
    }
}

void FluentSlider::setValues(qreal lower, qreal upper) {
    if (lower > upper) {
        qSwap(lower, upper);
    }
    
    lower = constrainValue(lower);
    upper = constrainValue(upper);
    
    if (m_snapToTicks) {
        lower = snapValue(lower);
        upper = snapValue(upper);
    }
    
    bool lowerChanged = !qFuzzyCompare(m_lowerValue, lower);
    bool upperChanged = !qFuzzyCompare(m_upperValue, upper);
    
    if (lowerChanged || upperChanged) {
        m_lowerValue = lower;
        m_upperValue = upper;
        
        updateHandlePositions();
        update();
        
        if (lowerChanged) emit lowerValueChanged(lower);
        if (upperChanged) emit upperValueChanged(upper);
        emit valuesChanged(lower, upper);
    }
}

QPair<qreal, qreal> FluentSlider::values() const {
    return qMakePair(m_lowerValue, m_upperValue);
}

qreal FluentSlider::step() const {
    return m_step;
}

void FluentSlider::setStep(qreal step) {
    if (step > 0 && !qFuzzyCompare(m_step, step)) {
        m_step = step;
        emit stepChanged(step);
    }
}

qreal FluentSlider::pageStep() const {
    return m_pageStep;
}

void FluentSlider::setPageStep(qreal step) {
    if (step > 0 && !qFuzzyCompare(m_pageStep, step)) {
        m_pageStep = step;
        emit pageStepChanged(step);
    }
}

FluentSliderTickPosition FluentSlider::tickPosition() const {
    return m_tickPosition;
}

void FluentSlider::setTickPosition(FluentSliderTickPosition position) {
    if (m_tickPosition != position) {
        m_tickPosition = position;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit tickPositionChanged(position);
    }
}

int FluentSlider::tickInterval() const {
    return m_tickInterval;
}

void FluentSlider::setTickInterval(int interval) {
    if (interval > 0 && m_tickInterval != interval) {
        m_tickInterval = interval;
        generateAutoTicks();
        m_layoutDirty = true;
        updateLayout();
        update();
        emit tickIntervalChanged(interval);
    }
}

void FluentSlider::addTick(qreal value) {
    addTick(FluentSliderTick(value));
}

void FluentSlider::addTick(qreal value, const QString& label) {
    addTick(FluentSliderTick(value, label));
}

void FluentSlider::addTick(const FluentSliderTick& tick) {
    if (isValidValue(tick.value)) {
        // Remove existing tick at same value
        removeTick(tick.value);
        
        // Insert in sorted order
        auto it = std::lower_bound(m_ticks.begin(), m_ticks.end(), tick,
                                   [](const FluentSliderTick& a, const FluentSliderTick& b) {
                                       return a.value < b.value;
                                   });
        m_ticks.insert(it, tick);
        
        m_layoutDirty = true;
        updateLayout();
        update();
    }
}

void FluentSlider::removeTick(qreal value) {
    auto it = std::find_if(m_ticks.begin(), m_ticks.end(),
                           [value](const FluentSliderTick& tick) {
                               return qFuzzyCompare(tick.value, value);
                           });
    
    if (it != m_ticks.end()) {
        m_ticks.erase(it);
        m_layoutDirty = true;
        updateLayout();
        update();
    }
}

void FluentSlider::clearTicks() {
    if (!m_ticks.isEmpty()) {
        m_ticks.clear();
        m_layoutDirty = true;
        updateLayout();
        update();
    }
}

QList<FluentSliderTick> FluentSlider::ticks() const {
    return m_ticks;
}

void FluentSlider::setTicks(const QList<FluentSliderTick>& ticks) {
    m_ticks.clear();
    for (const auto& tick : ticks) {
        if (isValidValue(tick.value)) {
            m_ticks.append(tick);
        }
    }
    
    // Sort by value
    std::sort(m_ticks.begin(), m_ticks.end(),
              [](const FluentSliderTick& a, const FluentSliderTick& b) {
                  return a.value < b.value;
              });
    
    m_layoutDirty = true;
    updateLayout();
    update();
}

bool FluentSlider::showLabels() const {
    return m_showLabels;
}

void FluentSlider::setShowLabels(bool show) {
    if (m_showLabels != show) {
        m_showLabels = show;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit showLabelsChanged(show);
    }
}

bool FluentSlider::showTooltip() const {
    return m_showTooltip;
}

void FluentSlider::setShowTooltip(bool show) {
    if (m_showTooltip != show) {
        m_showTooltip = show;
        emit showTooltipChanged(show);
    }
}

QString FluentSlider::formatValue(qreal value) const {
    return m_valueFormatter ? m_valueFormatter(value) : QString::number(value);
}

void FluentSlider::setValueFormatter(std::function<QString(qreal)> formatter) {
    m_valueFormatter = formatter;
    update();
}

bool FluentSlider::isAnimated() const {
    return m_animated;
}

void FluentSlider::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

bool FluentSlider::snapToTicks() const {
    return m_snapToTicks;
}

void FluentSlider::setSnapToTicks(bool snap) {
    if (m_snapToTicks != snap) {
        m_snapToTicks = snap;
        
        // Re-snap current values if enabling
        if (snap) {
            setValue(m_value);
            if (m_mode == FluentSliderMode::Range) {
                setValues(m_lowerValue, m_upperValue);
            }
        }
        
        emit snapToTicksChanged(snap);
    }
}

qreal FluentSlider::valueFromPosition(const QPoint& position) const {
    return getValueFromPosition(position);
}

QPoint FluentSlider::positionFromValue(qreal value) const {
    return getHandleCenter(value);
}

QRect FluentSlider::handleRect(bool isUpper) const {
    if (m_mode == FluentSliderMode::Single || !isUpper) {
        return getHandleRect(m_mode == FluentSliderMode::Single ? m_value : m_lowerValue);
    } else {
        return getHandleRect(m_upperValue);
    }
}

QRect FluentSlider::trackRect() const {
    return getTrackRect();
}

bool FluentSlider::isValidValue(qreal value) const {
    return value >= m_minimum && value <= m_maximum;
}

qreal FluentSlider::constrainValue(qreal value) const {
    return qBound(m_minimum, value, m_maximum);
}

qreal FluentSlider::snapValue(qreal value) const {
    if (m_ticks.isEmpty()) {
        // Snap to step intervals
        qreal steps = qRound((value - m_minimum) / m_step);
        return m_minimum + steps * m_step;
    } else {
        // Snap to nearest tick
        qreal minDistance = std::numeric_limits<qreal>::max();
        qreal snappedValue = value;

        for (const auto& tick : m_ticks) {
            qreal distance = qAbs(tick.value - value);
            if (distance < minDistance) {
                minDistance = distance;
                snappedValue = tick.value;
            }
        }

        return snappedValue;
    }
}

QSize FluentSlider::sizeHint() const {
    auto& theme = Styling::FluentTheme::instance();

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int width = 200;
        int height = 32;

        if (m_tickPosition != FluentSliderTickPosition::NoTicks) {
            height += 16;
        }
        if (m_showLabels) {
            height += theme.bodyFont().pixelSize() + 4;
        }

        return QSize(width, height);
    } else {
        int width = 32;
        int height = 200;

        if (m_tickPosition != FluentSliderTickPosition::NoTicks) {
            width += 16;
        }
        if (m_showLabels) {
            width += 60; // Approximate label width
        }

        return QSize(width, height);
    }
}

QSize FluentSlider::minimumSizeHint() const {
    if (m_orientation == FluentSliderOrientation::Horizontal) {
        return QSize(100, 32);
    } else {
        return QSize(32, 100);
    }
}

void FluentSlider::animateToValue(qreal value, int duration) {
    if (!m_animated) {
        setValue(value);
        return;
    }

    qreal targetValue = constrainValue(value);
    if (m_snapToTicks) {
        targetValue = snapValue(targetValue);
    }

    if (m_valueAnimation) {
        m_valueAnimation->stop();
        m_valueAnimation->deleteLater();
    }

    m_valueAnimation = new QPropertyAnimation(this, "value", this);
    m_valueAnimation->setDuration(duration);
    m_valueAnimation->setStartValue(m_value);
    m_valueAnimation->setEndValue(targetValue);
    m_valueAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_valueAnimation, &QPropertyAnimation::finished,
            this, &FluentSlider::onAnimationFinished);

    m_valueAnimation->start();
}

void FluentSlider::animateToValues(qreal lower, qreal upper, int duration) {
    if (!m_animated || m_mode != FluentSliderMode::Range) {
        setValues(lower, upper);
        return;
    }

    if (lower > upper) {
        qSwap(lower, upper);
    }

    qreal targetLower = constrainValue(lower);
    qreal targetUpper = constrainValue(upper);

    if (m_snapToTicks) {
        targetLower = snapValue(targetLower);
        targetUpper = snapValue(targetUpper);
    }

    // Stop existing animations
    if (m_lowerValueAnimation) {
        m_lowerValueAnimation->stop();
        m_lowerValueAnimation->deleteLater();
    }
    if (m_upperValueAnimation) {
        m_upperValueAnimation->stop();
        m_upperValueAnimation->deleteLater();
    }

    // Create lower value animation
    m_lowerValueAnimation = new QPropertyAnimation(this, "lowerValue", this);
    m_lowerValueAnimation->setDuration(duration);
    m_lowerValueAnimation->setStartValue(m_lowerValue);
    m_lowerValueAnimation->setEndValue(targetLower);
    m_lowerValueAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Create upper value animation
    m_upperValueAnimation = new QPropertyAnimation(this, "upperValue", this);
    m_upperValueAnimation->setDuration(duration);
    m_upperValueAnimation->setStartValue(m_upperValue);
    m_upperValueAnimation->setEndValue(targetUpper);
    m_upperValueAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_lowerValueAnimation, &QPropertyAnimation::finished,
            this, &FluentSlider::onAnimationFinished);
    connect(m_upperValueAnimation, &QPropertyAnimation::finished,
            this, &FluentSlider::onAnimationFinished);

    m_lowerValueAnimation->start();
    m_upperValueAnimation->start();
}

void FluentSlider::setupAnimations() {
    // Connect to theme changes
    auto& theme = Styling::FluentTheme::instance();
    connect(&theme, &Styling::FluentTheme::themeChanged,
            this, &FluentSlider::updateColors);
    connect(&theme, &Styling::FluentTheme::accentColorChanged,
            this, &FluentSlider::updateColors);
}

void FluentSlider::updateLayout() {
    if (!m_layoutDirty) return;

    m_trackRect = getTrackRect();
    updateHandlePositions();
    updateTickPositions();

    m_layoutDirty = false;
}

void FluentSlider::updateHandlePositions() {
    if (m_mode == FluentSliderMode::Single) {
        m_lowerHandleRect = getHandleRect(m_value);
    } else {
        m_lowerHandleRect = getHandleRect(m_lowerValue);
        m_upperHandleRect = getHandleRect(m_upperValue);
    }
}

void FluentSlider::onAnimationFinished() {
    // Clean up finished animations
    auto* animation = qobject_cast<QPropertyAnimation*>(sender());
    if (animation) {
        animation->deleteLater();

        if (animation == m_valueAnimation) {
            m_valueAnimation = nullptr;
        } else if (animation == m_lowerValueAnimation) {
            m_lowerValueAnimation = nullptr;
        } else if (animation == m_upperValueAnimation) {
            m_upperValueAnimation = nullptr;
        }
    }
}

void FluentSlider::onTooltipTimer() {
    hideValueTooltip();
}

void FluentSlider::updateColors() {
    update();
}

void FluentSlider::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    updateLayout();

    paintTrack(&painter);
    paintProgress(&painter);

    if (m_tickPosition != FluentSliderTickPosition::NoTicks) {
        paintTicks(&painter);
    }

    if (m_showLabels) {
        paintLabels(&painter);
    }

    paintHandles(&painter);
}

void FluentSlider::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int handleIndex = getHandleAtPosition(event->pos());

        if (handleIndex >= 0) {
            // Clicked on a handle
            startDrag(handleIndex, event->pos());
        } else {
            // Clicked on track - move nearest handle
            qreal clickValue = getValueFromPosition(event->pos());

            if (m_mode == FluentSliderMode::Single) {
                if (m_animated) {
                    animateToValue(clickValue);
                } else {
                    setValue(clickValue);
                }
            } else {
                // For range mode, move the nearest handle
                qreal lowerDist = qAbs(clickValue - m_lowerValue);
                qreal upperDist = qAbs(clickValue - m_upperValue);

                if (lowerDist < upperDist) {
                    if (m_animated) {
                        animateToValues(clickValue, m_upperValue);
                    } else {
                        setLowerValue(clickValue);
                    }
                } else {
                    if (m_animated) {
                        animateToValues(m_lowerValue, clickValue);
                    } else {
                        setUpperValue(clickValue);
                    }
                }
            }
        }

        emit sliderPressed();
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentSlider::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        updateDrag(event->pos());

        if (m_showTooltip) {
            qreal currentValue = m_mode == FluentSliderMode::Single ? m_value :
                               (m_activeHandle == 0 ? m_lowerValue : m_upperValue);
            showValueTooltip(event->pos(), currentValue);
        }
    } else {
        // Update hover state
        int hoveredHandle = getHandleAtPosition(event->pos());
        if (m_hoveredHandle != hoveredHandle) {
            m_hoveredHandle = hoveredHandle;
            update();
        }
    }

    Core::FluentComponent::mouseMoveEvent(event);
}

void FluentSlider::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_dragging) {
        endDrag();
        emit sliderReleased();

        if (m_showTooltip) {
            hideValueTooltip();
        }
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentSlider::wheelEvent(QWheelEvent* event) {
    if (!hasFocus()) {
        event->ignore();
        return;
    }

    qreal delta = event->angleDelta().y() > 0 ? m_step : -m_step;

    if (m_mode == FluentSliderMode::Single) {
        setValue(m_value + delta);
    } else {
        // For range mode, adjust the handle closest to mouse
        QPoint pos = event->position().toPoint();
        qreal clickValue = getValueFromPosition(pos);
        qreal lowerDist = qAbs(clickValue - m_lowerValue);
        qreal upperDist = qAbs(clickValue - m_upperValue);

        if (lowerDist < upperDist) {
            setLowerValue(m_lowerValue + delta);
        } else {
            setUpperValue(m_upperValue + delta);
        }
    }

    event->accept();
}

void FluentSlider::keyPressEvent(QKeyEvent* event) {
    qreal delta = 0;
    bool handled = true;

    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Down:
            delta = -m_step;
            break;
        case Qt::Key_Right:
        case Qt::Key_Up:
            delta = m_step;
            break;
        case Qt::Key_PageDown:
            delta = -m_pageStep;
            break;
        case Qt::Key_PageUp:
            delta = m_pageStep;
            break;
        case Qt::Key_Home:
            if (m_mode == FluentSliderMode::Single) {
                setValue(m_minimum);
            } else {
                setLowerValue(m_minimum);
            }
            break;
        case Qt::Key_End:
            if (m_mode == FluentSliderMode::Single) {
                setValue(m_maximum);
            } else {
                setUpperValue(m_maximum);
            }
            break;
        default:
            handled = false;
            break;
    }

    if (handled) {
        if (delta != 0) {
            if (m_mode == FluentSliderMode::Single) {
                setValue(m_value + delta);
            } else {
                // For range mode, adjust based on which handle has focus
                // For simplicity, adjust lower handle by default
                setLowerValue(m_lowerValue + delta);
            }
        }
        event->accept();
    } else {
        Core::FluentComponent::keyPressEvent(event);
    }
}

void FluentSlider::focusInEvent(QFocusEvent* event) {
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentSlider::focusOutEvent(QFocusEvent* event) {
    update();
    Core::FluentComponent::focusOutEvent(event);
}

void FluentSlider::enterEvent(QEnterEvent* event) {
    update();
    Core::FluentComponent::enterEvent(event);
}

void FluentSlider::leaveEvent(QEvent* event) {
    m_hoveredHandle = -1;
    update();
    Core::FluentComponent::leaveEvent(event);
}

void FluentSlider::resizeEvent(QResizeEvent* event) {
    m_layoutDirty = true;
    updateLayout();
    Core::FluentComponent::resizeEvent(event);
}

// Paint methods
void FluentSlider::paintTrack(QPainter* painter) {
    painter->save();

    auto& theme = Styling::FluentTheme::instance();
    painter->setBrush(theme.color("controlStrokeColorDefault"));
    painter->setPen(Qt::NoPen);

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int trackHeight = 4;
        int y = (height() - trackHeight) / 2;
        QRect trackRect(10, y, width() - 20, trackHeight);
        painter->drawRoundedRect(trackRect, trackHeight / 2, trackHeight / 2);
    } else {
        int trackWidth = 4;
        int x = (width() - trackWidth) / 2;
        QRect trackRect(x, 10, trackWidth, height() - 20);
        painter->drawRoundedRect(trackRect, trackWidth / 2, trackWidth / 2);
    }

    painter->restore();
}

void FluentSlider::paintProgress(QPainter* painter) {
    painter->save();

    auto& theme = Styling::FluentTheme::instance();
    painter->setBrush(theme.color("accent"));
    painter->setPen(Qt::NoPen);

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int trackHeight = 4;
        int y = (height() - trackHeight) / 2;

        if (m_mode == FluentSliderMode::Single) {
            qreal progress = (m_value - m_minimum) / (m_maximum - m_minimum);
            int progressWidth = qRound((width() - 20) * progress);
            QRect progressRect(10, y, progressWidth, trackHeight);
            painter->drawRoundedRect(progressRect, trackHeight / 2, trackHeight / 2);
        } else {
            qreal lowerProgress = (m_lowerValue - m_minimum) / (m_maximum - m_minimum);
            qreal upperProgress = (m_upperValue - m_minimum) / (m_maximum - m_minimum);
            int startX = qRound(10 + (width() - 20) * lowerProgress);
            int endX = qRound(10 + (width() - 20) * upperProgress);
            QRect progressRect(startX, y, endX - startX, trackHeight);
            painter->drawRoundedRect(progressRect, trackHeight / 2, trackHeight / 2);
        }
    } else {
        int trackWidth = 4;
        int x = (width() - trackWidth) / 2;

        if (m_mode == FluentSliderMode::Single) {
            qreal progress = (m_value - m_minimum) / (m_maximum - m_minimum);
            int progressHeight = qRound((height() - 20) * progress);
            int startY = height() - 10 - progressHeight;
            QRect progressRect(x, startY, trackWidth, progressHeight);
            painter->drawRoundedRect(progressRect, trackWidth / 2, trackWidth / 2);
        } else {
            qreal lowerProgress = (m_lowerValue - m_minimum) / (m_maximum - m_minimum);
            qreal upperProgress = (m_upperValue - m_minimum) / (m_maximum - m_minimum);
            int startY = qRound(height() - 10 - (height() - 20) * upperProgress);
            int endY = qRound(height() - 10 - (height() - 20) * lowerProgress);
            QRect progressRect(x, startY, trackWidth, endY - startY);
            painter->drawRoundedRect(progressRect, trackWidth / 2, trackWidth / 2);
        }
    }

    painter->restore();
}

void FluentSlider::paintTicks(QPainter* painter) {
    if (m_tickPosition == FluentSliderTickPosition::NoTicks) {
        return;
    }

    painter->save();

    auto& theme = Styling::FluentTheme::instance();
    painter->setPen(QPen(theme.color("textFillColorSecondary"), 1));

    // Draw interval ticks
    if (m_tickInterval > 0) {
        for (qreal value = m_minimum; value <= m_maximum; value += m_tickInterval) {
            drawTick(painter, value, 6);
        }
    }

    // Draw custom ticks
    for (auto it = m_customTicks.begin(); it != m_customTicks.end(); ++it) {
        drawTick(painter, it.key(), 8);
    }

    painter->restore();
}

void FluentSlider::paintLabels(QPainter* painter) {
    if (!m_showLabels) {
        return;
    }

    painter->save();

    auto& theme = Styling::FluentTheme::instance();
    painter->setFont(theme.captionFont());
    painter->setPen(theme.color("textFillColorPrimary"));

    // Draw interval labels
    if (m_tickInterval > 0) {
        for (qreal value = m_minimum; value <= m_maximum; value += m_tickInterval) {
            drawLabel(painter, value, formatValue(value));
        }
    }

    // Draw custom labels
    for (auto it = m_customTicks.begin(); it != m_customTicks.end(); ++it) {
        if (!it.value().isEmpty()) {
            drawLabel(painter, it.key(), it.value());
        }
    }

    painter->restore();
}

void FluentSlider::paintHandles(QPainter* painter) {
    painter->save();

    if (m_mode == FluentSliderMode::Single) {
        paintHandle(painter, m_value, 0);
    } else {
        paintHandle(painter, m_lowerValue, 0);
        paintHandle(painter, m_upperValue, 1);
    }

    painter->restore();
}

// Utility methods
void FluentSlider::drawTick(QPainter* painter, qreal value, int length) {
    QPoint pos = getPositionFromValue(value);

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        if (m_tickPosition == FluentSliderTickPosition::Above || m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x(), pos.y() - 10, pos.x(), pos.y() - 10 - length);
        }
        if (m_tickPosition == FluentSliderTickPosition::Below || m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x(), pos.y() + 10, pos.x(), pos.y() + 10 + length);
        }
    } else {
        if (m_tickPosition == FluentSliderTickPosition::Above || m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x() - 10, pos.y(), pos.x() - 10 - length, pos.y());
        }
        if (m_tickPosition == FluentSliderTickPosition::Below || m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x() + 10, pos.y(), pos.x() + 10 + length, pos.y());
        }
    }
}

void FluentSlider::drawLabel(QPainter* painter, qreal value, const QString& text) {
    QPoint pos = getPositionFromValue(value);
    QFontMetrics fm(painter->font());
    QRect textRect = fm.boundingRect(text);

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        textRect.moveCenter(QPoint(pos.x(), pos.y() + 25));
    } else {
        textRect.moveCenter(QPoint(pos.x() - 25, pos.y()));
    }

    painter->drawText(textRect, Qt::AlignCenter, text);
}

void FluentSlider::paintHandle(QPainter* painter, qreal value, int handleIndex) {
    QPoint center = getPositionFromValue(value);
    int radius = 8;

    auto& theme = Styling::FluentTheme::instance();

    // Determine handle state
    bool isHovered = (m_hoveredHandle == handleIndex);
    bool isFocused = hasFocus();
    bool isPressed = m_dragging && (m_activeHandle == handleIndex);

    // Draw handle shadow
    if (isHovered || isPressed) {
        painter->setBrush(QColor(0, 0, 0, 30));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(center + QPoint(1, 1), radius + 1, radius + 1);
    }

    // Draw handle background
    QColor handleColor = theme.color("controlFillColorDefault");
    if (isPressed) {
        handleColor = theme.color("controlFillColorTertiary");
    } else if (isHovered) {
        handleColor = theme.color("controlFillColorSecondary");
    }

    painter->setBrush(handleColor);
    painter->setPen(QPen(theme.color("controlStrokeColorDefault"), 1));
    painter->drawEllipse(center, radius, radius);

    // Draw focus indicator
    if (isFocused) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(theme.color("accent"), 2));
        painter->drawEllipse(center, radius + 3, radius + 3);
    }
}

QPoint FluentSlider::getPositionFromValue(qreal value) const {
    if (m_orientation == FluentSliderOrientation::Horizontal) {
        qreal progress = (value - m_minimum) / (m_maximum - m_minimum);
        int x = qRound(10 + (width() - 20) * progress);
        return QPoint(x, height() / 2);
    } else {
        qreal progress = (value - m_minimum) / (m_maximum - m_minimum);
        int y = qRound(height() - 10 - (height() - 20) * progress);
        return QPoint(width() / 2, y);
    }
}

qreal FluentSlider::getValueFromPosition(const QPoint& position) const {
    if (m_orientation == FluentSliderOrientation::Horizontal) {
        qreal progress = qreal(position.x() - 10) / (width() - 20);
        progress = qBound(0.0, progress, 1.0);
        return m_minimum + progress * (m_maximum - m_minimum);
    } else {
        qreal progress = qreal(height() - 10 - position.y()) / (height() - 20);
        progress = qBound(0.0, progress, 1.0);
        return m_minimum + progress * (m_maximum - m_minimum);
    }
}

int FluentSlider::getHandleAtPosition(const QPoint& position) const {
    const int handleRadius = 12; // Slightly larger for easier clicking

    if (m_mode == FluentSliderMode::Single) {
        QPoint handlePos = getPositionFromValue(m_value);
        if ((position - handlePos).manhattanLength() <= handleRadius) {
            return 0;
        }
    } else {
        QPoint lowerPos = getPositionFromValue(m_lowerValue);
        QPoint upperPos = getPositionFromValue(m_upperValue);

        int lowerDist = (position - lowerPos).manhattanLength();
        int upperDist = (position - upperPos).manhattanLength();

        if (lowerDist <= handleRadius && lowerDist <= upperDist) {
            return 0; // Lower handle
        } else if (upperDist <= handleRadius) {
            return 1; // Upper handle
        }
    }

    return -1; // No handle
}

QString FluentSlider::formatValue(qreal value) const {
    if (m_valueFormatter) {
        return m_valueFormatter(value);
    }

    // Default formatting
    if (qFuzzyCompare(value, qRound(value))) {
        return QString::number(qRound(value));
    } else {
        return QString::number(value, 'f', 1);
    }
}

// Missing interaction methods
void FluentSlider::startDrag(int handleIndex, const QPoint& position) {
    m_dragging = true;
    m_activeHandle = handleIndex;
    m_dragStartPosition = position;

    if (m_mode == FluentSliderMode::Single) {
        m_dragStartValue = m_value;
    } else {
        m_dragStartLowerValue = m_lowerValue;
        m_dragStartUpperValue = m_upperValue;
    }

    update();
}

void FluentSlider::updateDrag(const QPoint& position) {
    if (!m_dragging) return;

    qreal newValue = getValueFromPosition(position);

    if (m_snapToTicks) {
        newValue = snapValueToTick(newValue);
    }

    if (m_mode == FluentSliderMode::Single) {
        setValue(newValue);
    } else {
        if (m_activeHandle == 0) {
            setLowerValue(newValue);
        } else {
            setUpperValue(newValue);
        }
    }
}

void FluentSlider::endDrag() {
    m_dragging = false;
    m_activeHandle = -1;
    update();
}

void FluentSlider::showValueTooltip(const QPoint& position, qreal value) {
    if (!m_showTooltip) return;

    // Implementation would create and show tooltip widget
    // For now, just a placeholder
    Q_UNUSED(position)
    Q_UNUSED(value)
}

void FluentSlider::hideValueTooltip() {
    if (m_tooltipWidget) {
        m_tooltipWidget->hide();
    }
}

qreal FluentSlider::snapValueToTick(qreal value) const {
    if (!m_snapToTicks) return value;

    qreal closestTick = value;
    qreal minDistance = std::numeric_limits<qreal>::max();

    // Check interval ticks
    if (m_tickInterval > 0) {
        qreal tickValue = m_minimum;
        while (tickValue <= m_maximum) {
            qreal distance = qAbs(value - tickValue);
            if (distance < minDistance) {
                minDistance = distance;
                closestTick = tickValue;
            }
            tickValue += m_tickInterval;
        }
    }

    // Check custom ticks
    for (auto it = m_customTicks.begin(); it != m_customTicks.end(); ++it) {
        qreal distance = qAbs(value - it.key());
        if (distance < minDistance) {
            minDistance = distance;
            closestTick = it.key();
        }
    }

    return closestTick;
}

// Missing utility methods
QRect FluentSlider::getTrackRect() const {
    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int trackHeight = 4;
        int y = (height() - trackHeight) / 2;
        return QRect(10, y, width() - 20, trackHeight);
    } else {
        int trackWidth = 4;
        int x = (width() - trackWidth) / 2;
        return QRect(x, 10, trackWidth, height() - 20);
    }
}

QRect FluentSlider::getHandleRect(qreal value) const {
    QPoint center = getPositionFromValue(value);
    int radius = 8;
    return QRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
}

QPoint FluentSlider::getHandleCenter(qreal value) const {
    return getPositionFromValue(value);
}

void FluentSlider::generateAutoTicks() {
    // Implementation for automatic tick generation
    // This would be called when tick interval changes
}

void FluentSlider::updateTickPositions() {
    // Update cached tick positions for performance
    m_layoutDirty = true;
}

void FluentSlider::emitValueChanged() {
    if (m_mode == FluentSliderMode::Single) {
        emit valueChanged(m_value);
    } else {
        emit valuesChanged(m_lowerValue, m_upperValue);
    }
}

void FluentSlider::constrainValues() {
    // Ensure values are within bounds and properly ordered
    m_value = qBound(m_minimum, m_value, m_maximum);
    m_lowerValue = qBound(m_minimum, m_lowerValue, m_maximum);
    m_upperValue = qBound(m_minimum, m_upperValue, m_maximum);

    if (m_lowerValue > m_upperValue) {
        qSwap(m_lowerValue, m_upperValue);
    }
}

} // namespace FluentQt::Components
