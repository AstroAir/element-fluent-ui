// src/Components/FluentSlider.cpp
#include "FluentQt/Components/FluentSlider.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QTimer>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtMath>
#include <memory>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentSlider::FluentSlider(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    m_tooltipTimer = new QTimer();
    setupAnimations();
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);

    // Default value formatter
    m_valueFormatter = [](qreal value) {
        return QString::number(value, 'f', 1);
    };

    connect(m_tooltipTimer, &QTimer::timeout, this,
            &FluentSlider::onTooltipTimer);
    m_tooltipTimer->setSingleShot(true);

    // Initialize accessibility features
    initializeAccessibility();
    initializeLiveRegion();
    detectSystemHighContrast();
    detectSystemMotionPreferences();
    updateColors();
    updateLayout();
}

FluentSlider::FluentSlider(FluentSliderOrientation orientation, QWidget* parent)
    : FluentSlider(parent) {
    setOrientation(orientation);
}

FluentSlider::FluentSlider(qreal minimum, qreal maximum, QWidget* parent)
    : FluentSlider(parent) {
    setRange(minimum, maximum);
}

FluentSlider::FluentSlider(qreal minimum, qreal maximum, qreal value,
                           QWidget* parent)
    : FluentSlider(minimum, maximum, parent) {
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

FluentSliderMode FluentSlider::mode() const { return m_mode; }

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

qreal FluentSlider::minimum() const { return m_minimum; }

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

qreal FluentSlider::maximum() const { return m_maximum; }

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

        if (minChanged)
            emit minimumChanged(minimum);
        if (maxChanged)
            emit maximumChanged(maximum);
        emit rangeChanged(minimum, maximum);
    }
}

qreal FluentSlider::value() const { return m_value; }

void FluentSlider::setValue(qreal value) {
    qreal constrainedValue = constrainValue(value);
    if (m_snapToTicks) {
        constrainedValue = snapValue(constrainedValue);
    }

    if (!qFuzzyCompare(m_value, constrainedValue)) {
        m_value = constrainedValue;
        updateHandlePositions();
        updateAccessibleValue();  // Notify screen readers of value change
        update();
        emit valueChanged(constrainedValue);
    }
}

qreal FluentSlider::lowerValue() const { return m_lowerValue; }

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
        updateAccessibleValue();  // Notify screen readers of value change
        update();
        emit lowerValueChanged(constrainedValue);
        emit valuesChanged(m_lowerValue, m_upperValue);
    }
}

qreal FluentSlider::upperValue() const { return m_upperValue; }

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
        updateAccessibleValue();  // Notify screen readers of value change
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

        if (lowerChanged)
            emit lowerValueChanged(lower);
        if (upperChanged)
            emit upperValueChanged(upper);
        emit valuesChanged(lower, upper);
    }
}

QPair<qreal, qreal> FluentSlider::values() const {
    return qMakePair(m_lowerValue, m_upperValue);
}

qreal FluentSlider::step() const { return m_step; }

void FluentSlider::setStep(qreal step) {
    if (step > 0 && !qFuzzyCompare(m_step, step)) {
        m_step = step;
        emit stepChanged(step);
    }
}

qreal FluentSlider::pageStep() const { return m_pageStep; }

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

int FluentSlider::tickInterval() const { return m_tickInterval; }

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

void FluentSlider::addTick(qreal value) { addTick(FluentSliderTick(value)); }

void FluentSlider::addTick(qreal value, const QString& label) {
    addTick(FluentSliderTick(value, label));
}

void FluentSlider::addTick(const FluentSliderTick& tick) {
    if (isValidValue(tick.value)) {
        // Remove existing tick at same value
        removeTick(tick.value);

        // Insert in sorted order
        auto it = std::lower_bound(
            m_ticks.begin(), m_ticks.end(), tick,
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

QList<FluentSliderTick> FluentSlider::ticks() const { return m_ticks; }

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

bool FluentSlider::showLabels() const { return m_showLabels; }

void FluentSlider::setShowLabels(bool show) {
    if (m_showLabels != show) {
        m_showLabels = show;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit showLabelsChanged(show);
    }
}

bool FluentSlider::showTooltip() const { return m_showTooltip; }

void FluentSlider::setShowTooltip(bool show) {
    if (m_showTooltip != show) {
        m_showTooltip = show;
        emit showTooltipChanged(show);
    }
}

void FluentSlider::setValueFormatter(std::function<QString(qreal)> formatter) {
    m_valueFormatter = formatter;
    update();
}

bool FluentSlider::isAnimated() const { return m_animated; }

void FluentSlider::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

bool FluentSlider::snapToTicks() const { return m_snapToTicks; }

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

bool FluentSlider::showFocusIndicator() const { return m_showFocusIndicator; }

void FluentSlider::setShowFocusIndicator(bool show) {
    if (m_showFocusIndicator != show) {
        m_showFocusIndicator = show;
        update();
        emit showFocusIndicatorChanged(show);
    }
}

bool FluentSlider::highContrastMode() const { return m_highContrastMode; }

void FluentSlider::setHighContrastMode(bool enabled) {
    if (m_highContrastMode != enabled) {
        m_highContrastMode = enabled;
        update();
        emit highContrastModeChanged(enabled);
    }
}

bool FluentSlider::respectMotionPreferences() const {
    return m_respectMotionPreferences;
}

void FluentSlider::setRespectMotionPreferences(bool respect) {
    if (m_respectMotionPreferences != respect) {
        m_respectMotionPreferences = respect;
        detectSystemMotionPreferences();
        update();
    }
}

QString FluentSlider::ariaLabel() const { return m_ariaLabel; }

void FluentSlider::setAriaLabel(const QString& label) {
    if (m_ariaLabel != label) {
        m_ariaLabel = label;
        setAccessibleName(label);
    }
}

QString FluentSlider::ariaDescription() const { return m_ariaDescription; }

void FluentSlider::setAriaDescription(const QString& description) {
    if (m_ariaDescription != description) {
        m_ariaDescription = description;
        setAccessibleDescription(description);
    }
}

QString FluentSlider::ariaValueText() const { return m_ariaValueText; }

void FluentSlider::setAriaValueText(const QString& valueText) {
    if (m_ariaValueText != valueText) {
        m_ariaValueText = valueText;
        // Update accessible value text for screen readers
        if (!valueText.isEmpty()) {
            setAccessibleDescription(m_ariaDescription + " " + valueText);
        }
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
        return getHandleRect(m_mode == FluentSliderMode::Single ? m_value
                                                                : m_lowerValue);
    } else {
        return getHandleRect(m_upperValue);
    }
}

QRect FluentSlider::trackRect() const { return getTrackRect(); }

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

    // Enhanced size calculations for better FluentUI compliance
    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int width = 200;
        int height = 40;  // Increased base height for better touch targets

        if (m_tickPosition != FluentSliderTickPosition::NoTicks) {
            height += 20;  // More space for enhanced tick styling
        }
        if (m_showLabels) {
            height +=
                theme.bodyFont().pixelSize() + 8;  // More padding for labels
        }

        return QSize(width, height);
    } else {
        int width = 40;  // Increased base width for better touch targets
        int height = 200;

        if (m_tickPosition != FluentSliderTickPosition::NoTicks) {
            width += 20;  // More space for enhanced tick styling
        }
        if (m_showLabels) {
            width += 80;  // More space for enhanced label styling
        }

        return QSize(width, height);
    }
}

QSize FluentSlider::minimumSizeHint() const {
    // Enhanced minimum sizes for better FluentUI compliance and touch targets
    if (m_orientation == FluentSliderOrientation::Horizontal) {
        return QSize(100, 40);  // Increased height for better touch targets
    } else {
        return QSize(40, 100);  // Increased width for better touch targets
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

    // Enhanced animation using FluentAnimator for better micro-interactions
    if (m_valueAnimation) {
        m_valueAnimation->stop();
        m_valueAnimation->deleteLater();
    }

    m_valueAnimation = new QPropertyAnimation(this, "value", this);
    m_valueAnimation->setDuration(duration);
    m_valueAnimation->setStartValue(m_value);
    m_valueAnimation->setEndValue(targetValue);

    // Apply FluentUI motion principles with optimal easing curve
    m_valueAnimation->setEasingCurve(getOptimalEasingCurve());

    connect(m_valueAnimation, &QPropertyAnimation::finished, this,
            &FluentSlider::onAnimationFinished);

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

    // Create enhanced lower value animation with FluentAnimator
    m_lowerValueAnimation = new QPropertyAnimation(this, "lowerValue", this);
    m_lowerValueAnimation->setDuration(duration);
    m_lowerValueAnimation->setStartValue(m_lowerValue);
    m_lowerValueAnimation->setEndValue(targetLower);
    m_lowerValueAnimation->setEasingCurve(getOptimalEasingCurve());

    // Create enhanced upper value animation with FluentAnimator
    m_upperValueAnimation = new QPropertyAnimation(this, "upperValue", this);
    m_upperValueAnimation->setDuration(duration);
    m_upperValueAnimation->setStartValue(m_upperValue);
    m_upperValueAnimation->setEndValue(targetUpper);
    m_upperValueAnimation->setEasingCurve(getOptimalEasingCurve());

    connect(m_lowerValueAnimation, &QPropertyAnimation::finished, this,
            &FluentSlider::onAnimationFinished);
    connect(m_upperValueAnimation, &QPropertyAnimation::finished, this,
            &FluentSlider::onAnimationFinished);

    m_lowerValueAnimation->start();
    m_upperValueAnimation->start();
}

void FluentSlider::setupAnimations() {
    // Connect to theme changes
    auto& theme = Styling::FluentTheme::instance();
    connect(&theme, &Styling::FluentTheme::themeChanged, this,
            &FluentSlider::updateColors);
    connect(
        &theme,
        QOverload<const QColor&>::of(&Styling::FluentTheme::accentColorChanged),
        this, &FluentSlider::updateColors);
}

void FluentSlider::updateLayout() {
    if (!m_layoutDirty)
        return;

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

void FluentSlider::onTooltipTimer() { hideValueTooltip(); }

void FluentSlider::updateColors() { update(); }

void FluentSlider::initializeAccessibility() {
    // Set up basic accessibility properties
    setAccessibleName("Slider");
    setAccessibleDescription("Use arrow keys to adjust value");

    // Set accessibility role (slider role is implicit for this widget type)

    // Initialize default ARIA attributes
    if (m_ariaLabel.isEmpty()) {
        m_ariaLabel = "Slider";
        setAccessibleName(m_ariaLabel);
    }

    if (m_ariaDescription.isEmpty()) {
        m_ariaDescription =
            "Use arrow keys or mouse to adjust the slider value";
        setAccessibleDescription(m_ariaDescription);
    }

    // Set up value announcements
    updateAccessibleValue();
}

void FluentSlider::updateAccessibleValue() {
    // Update accessible value information for screen readers
    QString valueText;

    if (m_mode == FluentSliderMode::Single) {
        if (!m_ariaValueText.isEmpty()) {
            valueText = m_ariaValueText;
        } else {
            valueText = QString("Value: %1 of %2 to %3")
                            .arg(formatValue(m_value))
                            .arg(formatValue(m_minimum))
                            .arg(formatValue(m_maximum));
        }
    } else {
        if (!m_ariaValueText.isEmpty()) {
            valueText = m_ariaValueText;
        } else {
            valueText = QString("Range: %1 to %2, from %3 to %4")
                            .arg(formatValue(m_lowerValue))
                            .arg(formatValue(m_upperValue))
                            .arg(formatValue(m_minimum))
                            .arg(formatValue(m_maximum));
        }
    }

    // Update the accessible description with current value
    QString fullDescription = m_ariaDescription;
    if (!fullDescription.isEmpty() && !valueText.isEmpty()) {
        fullDescription += ". " + valueText;
    } else if (!valueText.isEmpty()) {
        fullDescription = valueText;
    }

    setAccessibleDescription(fullDescription);
}

void FluentSlider::announceValueChange() {
    // Announce value changes to screen readers for better accessibility
    QString announcement;

    if (m_mode == FluentSliderMode::Single) {
        if (!m_ariaValueText.isEmpty()) {
            announcement = m_ariaValueText;
        } else {
            announcement = QString("Value: %1").arg(formatValue(m_value));
        }
    } else {
        if (!m_ariaValueText.isEmpty()) {
            announcement = m_ariaValueText;
        } else {
            announcement = QString("Range: %1 to %2")
                               .arg(formatValue(m_lowerValue))
                               .arg(formatValue(m_upperValue));
        }
    }

    // Use live region for better accessibility announcements
    announceStateChange(announcement);

    // Also update accessible description as fallback
    QString currentDescription = m_ariaDescription;
    if (!currentDescription.isEmpty()) {
        setAccessibleDescription(currentDescription + ". " + announcement);
    } else {
        setAccessibleDescription(announcement);
    }

    // Restore original description after a brief delay to avoid cluttering
    QTimer::singleShot(
        100, this, [this]() { setAccessibleDescription(m_ariaDescription); });
}

void FluentSlider::detectSystemHighContrast() {
    // Detect system high contrast mode automatically
    QPalette systemPalette = QApplication::palette();
    QColor windowColor = systemPalette.color(QPalette::Window);
    QColor textColor = systemPalette.color(QPalette::WindowText);

    // Calculate contrast ratio to determine if high contrast mode is active
    qreal windowLuminance = 0.299 * windowColor.redF() +
                            0.587 * windowColor.greenF() +
                            0.114 * windowColor.blueF();
    qreal textLuminance = 0.299 * textColor.redF() +
                          0.587 * textColor.greenF() +
                          0.114 * textColor.blueF();

    qreal contrastRatio = (qMax(windowLuminance, textLuminance) + 0.05) /
                          (qMin(windowLuminance, textLuminance) + 0.05);

    // If contrast ratio is very high, assume high contrast mode
    bool systemHighContrast = contrastRatio > 7.0;

    if (systemHighContrast != m_highContrastMode) {
        setHighContrastMode(systemHighContrast);
    }
}

void FluentSlider::initializeLiveRegion() {
    // Create a hidden live region for screen reader announcements
    if (!m_liveRegion) {
        m_liveRegion = new QLabel(this);
        m_liveRegion->setVisible(false);
        m_liveRegion->setAccessibleName("Slider announcements");
        // Set ARIA live region properties (accessibility role is implicit for
        // QLabel)
        m_liveRegion->setProperty("aria-live", "polite");
        m_liveRegion->setProperty("aria-atomic", "true");
    }
}

void FluentSlider::announceStateChange(const QString& message) {
    // Announce state changes through the live region
    if (m_liveRegion && !message.isEmpty()) {
        m_liveRegion->setText(message);
        m_liveRegion->setAccessibleDescription(message);

        // Clear the message after a brief delay to prepare for next
        // announcement
        QTimer::singleShot(500, this, [this]() {
            if (m_liveRegion) {
                m_liveRegion->clear();
            }
        });
    }
}

void FluentSlider::announceSliderPressed() {
    QString message;
    if (m_mode == FluentSliderMode::Single) {
        message = QString("Slider pressed. Current value: %1")
                      .arg(formatValue(m_value));
    } else {
        message = QString("Range slider pressed. Current range: %1 to %2")
                      .arg(formatValue(m_lowerValue))
                      .arg(formatValue(m_upperValue));
    }
    announceStateChange(message);
}

void FluentSlider::announceSliderReleased() {
    QString message;
    if (m_mode == FluentSliderMode::Single) {
        message = QString("Slider released. Final value: %1")
                      .arg(formatValue(m_value));
    } else {
        message = QString("Range slider released. Final range: %1 to %2")
                      .arg(formatValue(m_lowerValue))
                      .arg(formatValue(m_upperValue));
    }
    announceStateChange(message);
}

void FluentSlider::detectSystemMotionPreferences() {
    // Detect system motion preferences for accessibility
    if (!m_respectMotionPreferences) {
        return;
    }

    // For now, use application-level motion preference detection
    // Platform-specific detection can be added later with proper headers
    // The optimal easing curve method will handle motion preferences
}

QEasingCurve FluentSlider::getOptimalEasingCurve() const {
    // Return optimal easing curve based on motion preferences and context
    if (!m_animated) {
        return QEasingCurve::Linear;
    }

    if (!m_respectMotionPreferences) {
        // Use full FluentUI motion principles when not respecting preferences
        return Animation::FluentAnimator::toQtEasing(
            Animation::FluentEasing::EaseOutCubic);
    }

    // For accessibility, use gentler easing curves
    // Platform-specific motion detection can be added later
    if (m_highContrastMode) {
        // Use simpler easing for high contrast mode
        return Animation::FluentAnimator::toQtEasing(
            Animation::FluentEasing::EaseOut);
    } else {
        // Use standard FluentUI motion principles
        return Animation::FluentAnimator::toQtEasing(
            Animation::FluentEasing::EaseOutCubic);
    }
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
            announceSliderPressed();  // Announce slider press for accessibility

            // Add press micro-interaction using FluentAnimator
            if (m_animated) {
                auto pressAnimation =
                    Animation::FluentAnimator::pressEffect(this);
                if (pressAnimation) {
                    pressAnimation->start();
                }
            }
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
            qreal currentValue =
                m_mode == FluentSliderMode::Single
                    ? m_value
                    : (m_activeHandle == 0 ? m_lowerValue : m_upperValue);
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
        announceSliderReleased();  // Announce slider release for accessibility
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
    qreal step = m_step;

    // Enhanced step size based on modifiers
    if (event->modifiers() & Qt::ShiftModifier) {
        step = m_pageStep;
    } else if (event->modifiers() & Qt::ControlModifier) {
        step = m_step / 10.0;  // Fine adjustment
    }

    // Store old values for change detection
    qreal oldValue = m_value;
    qreal oldLowerValue = m_lowerValue;
    qreal oldUpperValue = m_upperValue;

    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Down:
            // Respect orientation for intuitive navigation
            if (m_orientation == FluentSliderOrientation::Vertical) {
                delta = (event->key() == Qt::Key_Down) ? -step : step;
            } else {
                delta = (event->key() == Qt::Key_Left) ? -step : step;
            }
            break;
        case Qt::Key_Right:
        case Qt::Key_Up:
            // Respect orientation for intuitive navigation
            if (m_orientation == FluentSliderOrientation::Vertical) {
                delta = (event->key() == Qt::Key_Up) ? step : -step;
            } else {
                delta = (event->key() == Qt::Key_Right) ? step : -step;
            }
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
                if (event->modifiers() & Qt::ControlModifier) {
                    setLowerValue(m_minimum);
                } else {
                    // Move both handles to minimum
                    setValues(m_minimum, m_minimum);
                }
            }
            break;
        case Qt::Key_End:
            if (m_mode == FluentSliderMode::Single) {
                setValue(m_maximum);
            } else {
                if (event->modifiers() & Qt::ControlModifier) {
                    setUpperValue(m_maximum);
                } else {
                    // Move both handles to maximum
                    setValues(m_maximum, m_maximum);
                }
            }
            break;
        case Qt::Key_Tab:
            // Enhanced tab navigation for range sliders
            if (m_mode == FluentSliderMode::Range) {
                // Let default tab handling work for handle switching
                handled = false;
            } else {
                handled = false;
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
                // Enhanced range mode navigation
                if (event->modifiers() & Qt::AltModifier) {
                    // Alt+Arrow moves upper handle
                    setUpperValue(m_upperValue + delta);
                } else {
                    // Default: move lower handle, or closest handle to current
                    // focus
                    qreal midpoint = (m_lowerValue + m_upperValue) / 2;
                    if (m_value <= midpoint) {
                        setLowerValue(m_lowerValue + delta);
                    } else {
                        setUpperValue(m_upperValue + delta);
                    }
                }
            }
        }

        // Announce value changes to screen readers
        bool valueChanged = false;
        if (m_mode == FluentSliderMode::Single) {
            valueChanged = !qFuzzyCompare(oldValue, m_value);
        } else {
            valueChanged = !qFuzzyCompare(oldLowerValue, m_lowerValue) ||
                           !qFuzzyCompare(oldUpperValue, m_upperValue);
        }

        if (valueChanged) {
            announceValueChange();
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
    // Add subtle hover micro-interaction using FluentAnimator
    if (m_animated) {
        auto hoverAnimation = Animation::FluentAnimator::hoverEffect(this);
        if (hoverAnimation) {
            hoverAnimation->start();
        }
    }

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

    // Use enhanced track styling with FluentUI colors and state awareness
    auto& theme = Styling::FluentTheme::instance();
    QColor trackColor = theme.color("controlStrokeColorDefault");

    // Enhanced high contrast mode and state-aware color adjustments
    if (m_highContrastMode) {
        // High contrast mode colors for better accessibility
        if (!isEnabled()) {
            trackColor = QColor(96, 96, 96);  // Disabled high contrast
        } else {
            trackColor = QColor(160, 160, 160);  // Standard high contrast track
        }
    } else if (!isEnabled()) {
        trackColor = theme.color("controlStrokeColorDisabled");
    } else if (hasFocus()) {
        // Slightly more prominent track when focused
        trackColor = trackColor.lighter(110);
    }

    painter->setBrush(trackColor);
    painter->setPen(Qt::NoPen);

    // Use design tokens for track dimensions
    int trackThickness = 4;  // Default value, can be made configurable later
    int trackPadding = 10;   // Default value, can be made configurable later
    qreal cornerRadius = trackThickness / 2.0;

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int y = (height() - trackThickness) / 2;
        QRect trackRect(trackPadding, y, width() - 2 * trackPadding,
                        trackThickness);
        painter->drawRoundedRect(trackRect, cornerRadius, cornerRadius);
    } else {
        int x = (width() - trackThickness) / 2;
        QRect trackRect(x, trackPadding, trackThickness,
                        height() - 2 * trackPadding);
        painter->drawRoundedRect(trackRect, cornerRadius, cornerRadius);
    }

    painter->restore();
}

void FluentSlider::paintProgress(QPainter* painter) {
    painter->save();

    // Use enhanced progress styling with FluentUI colors and state awareness
    auto& theme = Styling::FluentTheme::instance();
    QColor progressColor = theme.color("accent");

    // Enhanced high contrast mode and state-aware color adjustments
    if (m_highContrastMode) {
        // High contrast mode colors for better accessibility
        if (!isEnabled()) {
            progressColor = QColor(128, 128, 128);  // Disabled high contrast
        } else {
            progressColor =
                QColor(255, 255, 255);  // High contrast progress (white)
        }
    } else if (!isEnabled()) {
        progressColor = theme.color("accentDisabled");
    } else if (hasFocus()) {
        // More vibrant progress when focused
        progressColor = progressColor.lighter(105);
    }

    painter->setBrush(progressColor);
    painter->setPen(Qt::NoPen);

    // Use consistent dimensions with track
    int trackThickness = 4;  // Default value, can be made configurable later
    int trackPadding = 10;   // Default value, can be made configurable later
    qreal cornerRadius = trackThickness / 2.0;

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int y = (height() - trackThickness) / 2;

        if (m_mode == FluentSliderMode::Single) {
            qreal progress = (m_value - m_minimum) / (m_maximum - m_minimum);
            int progressWidth = qRound((width() - 2 * trackPadding) * progress);
            QRect progressRect(trackPadding, y, progressWidth, trackThickness);
            painter->drawRoundedRect(progressRect, cornerRadius, cornerRadius);
        } else {
            qreal lowerProgress =
                (m_lowerValue - m_minimum) / (m_maximum - m_minimum);
            qreal upperProgress =
                (m_upperValue - m_minimum) / (m_maximum - m_minimum);
            int startX = qRound(trackPadding +
                                (width() - 2 * trackPadding) * lowerProgress);
            int endX = qRound(trackPadding +
                              (width() - 2 * trackPadding) * upperProgress);
            QRect progressRect(startX, y, endX - startX, trackThickness);
            painter->drawRoundedRect(progressRect, cornerRadius, cornerRadius);
        }
    } else {
        int x = (width() - trackThickness) / 2;

        if (m_mode == FluentSliderMode::Single) {
            qreal progress = (m_value - m_minimum) / (m_maximum - m_minimum);
            int progressHeight =
                qRound((height() - 2 * trackPadding) * progress);
            int startY = height() - trackPadding - progressHeight;
            QRect progressRect(x, startY, trackThickness, progressHeight);
            painter->drawRoundedRect(progressRect, cornerRadius, cornerRadius);
        } else {
            qreal lowerProgress =
                (m_lowerValue - m_minimum) / (m_maximum - m_minimum);
            qreal upperProgress =
                (m_upperValue - m_minimum) / (m_maximum - m_minimum);
            int startY = qRound(height() - trackPadding -
                                (height() - 2 * trackPadding) * upperProgress);
            int endY = qRound(height() - trackPadding -
                              (height() - 2 * trackPadding) * lowerProgress);
            QRect progressRect(x, startY, trackThickness, endY - startY);
            painter->drawRoundedRect(progressRect, cornerRadius, cornerRadius);
        }
    }

    painter->restore();
}

void FluentSlider::paintTicks(QPainter* painter) {
    if (m_tickPosition == FluentSliderTickPosition::NoTicks) {
        return;
    }

    painter->save();

    // Use enhanced tick styling with FluentUI colors and high contrast support
    auto& theme = Styling::FluentTheme::instance();
    QColor tickColor = theme.color("textFillColorSecondary");

    // Enhanced high contrast mode support for ticks
    if (m_highContrastMode) {
        if (!isEnabled()) {
            tickColor = QColor(96, 96, 96);  // Disabled high contrast
        } else {
            tickColor = QColor(255, 255, 255);  // High contrast tick color
        }
    } else if (!isEnabled()) {
        tickColor = theme.color("textFillColorDisabled");
    }

    int tickStroke = 1;
    int majorTickLength = 8;
    int minorTickLength = 6;

    painter->setPen(QPen(tickColor, tickStroke));

    // Draw interval ticks
    if (m_tickInterval > 0) {
        for (qreal value = m_minimum; value <= m_maximum;
             value += m_tickInterval) {
            drawTick(painter, value, minorTickLength);
        }
    }

    // Draw custom ticks
    for (auto it = m_customTicks.begin(); it != m_customTicks.end(); ++it) {
        drawTick(painter, it.key(), majorTickLength);
    }

    painter->restore();
}

void FluentSlider::paintLabels(QPainter* painter) {
    if (!m_showLabels) {
        return;
    }

    painter->save();

    // Use enhanced label styling with FluentUI colors, fonts, and high contrast
    // support
    auto& theme = Styling::FluentTheme::instance();
    QColor labelColor = theme.color("textFillColorPrimary");
    QFont labelFont = theme.captionFont();

    // Enhanced high contrast mode support for labels
    if (m_highContrastMode) {
        if (!isEnabled()) {
            labelColor = QColor(96, 96, 96);  // Disabled high contrast
        } else {
            labelColor = QColor(255, 255, 255);  // High contrast label color
        }
        // Use a slightly bolder font in high contrast mode for better
        // readability
        labelFont.setBold(true);
    } else if (!isEnabled()) {
        labelColor = theme.color("textFillColorDisabled");
    }

    painter->setFont(labelFont);
    painter->setPen(labelColor);

    // Draw interval labels
    if (m_tickInterval > 0) {
        for (qreal value = m_minimum; value <= m_maximum;
             value += m_tickInterval) {
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
        if (m_tickPosition == FluentSliderTickPosition::Above ||
            m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x(), pos.y() - 10, pos.x(),
                              pos.y() - 10 - length);
        }
        if (m_tickPosition == FluentSliderTickPosition::Below ||
            m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x(), pos.y() + 10, pos.x(),
                              pos.y() + 10 + length);
        }
    } else {
        if (m_tickPosition == FluentSliderTickPosition::Above ||
            m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x() - 10, pos.y(), pos.x() - 10 - length,
                              pos.y());
        }
        if (m_tickPosition == FluentSliderTickPosition::Below ||
            m_tickPosition == FluentSliderTickPosition::Both) {
            painter->drawLine(pos.x() + 10, pos.y(), pos.x() + 10 + length,
                              pos.y());
        }
    }
}

void FluentSlider::drawLabel(QPainter* painter, qreal value,
                             const QString& text) {
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

void FluentSlider::paintHandle(QPainter* painter, qreal value,
                               int handleIndex) {
    QPoint center = getPositionFromValue(value);

    // Use enhanced handle styling with FluentUI design
    int radius = 8;
    int strokeWidth = 1;
    int focusRingWidth = 2;
    int focusRingOffset = 3;

    // Determine handle state
    bool isHovered = (m_hoveredHandle == handleIndex);
    bool isFocused = hasFocus();
    bool isPressed = m_dragging && (m_activeHandle == handleIndex);

    // Draw enhanced handle shadow with proper FluentUI styling
    if (isHovered || isPressed) {
        // Multiple shadow layers for depth
        QColor shadowColor = QColor(0, 0, 0, 20);

        // Outer shadow for depth
        painter->setBrush(QColor(shadowColor.red(), shadowColor.green(),
                                 shadowColor.blue(), 15));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(center + QPoint(2, 2), radius + 3, radius + 3);

        // Inner shadow for definition
        painter->setBrush(QColor(shadowColor.red(), shadowColor.green(),
                                 shadowColor.blue(), 25));
        painter->drawEllipse(center + QPoint(1, 1), radius + 1, radius + 1);
    }

    // Base shadow for all states (subtle)
    QColor baseShadowColor = QColor(0, 0, 0, 8);
    painter->setBrush(baseShadowColor);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(center + QPoint(0, 1), radius, radius);

    // Get handle colors from FluentUI theme
    auto& theme = Styling::FluentTheme::instance();
    QColor handleColor = theme.color("controlFillColorDefault");
    QColor strokeColor = theme.color("controlStrokeColorDefault");
    QColor focusColor = theme.color("accent");

    // Apply state-specific colors with enhanced visual feedback
    if (isPressed) {
        handleColor = theme.color("controlFillColorTertiary");
        // Slightly smaller radius when pressed for tactile feedback
        radius = qMax(radius - 1, 6);
    } else if (isHovered) {
        handleColor = theme.color("controlFillColorSecondary");
        // Slightly larger radius when hovered for better visibility
        radius = radius + 1;
    }

    // Draw handle background with enhanced styling
    painter->setBrush(handleColor);
    painter->setPen(QPen(strokeColor, strokeWidth));
    painter->drawEllipse(center, radius, radius);

    // Add inner highlight for depth (FluentUI style)
    if (!isPressed) {
        QColor highlightColor = QColor(255, 255, 255, isHovered ? 40 : 20);
        painter->setBrush(highlightColor);
        painter->setPen(Qt::NoPen);
        // Draw highlight in upper portion of handle
        QRect highlightRect(center.x() - radius / 2, center.y() - radius / 2,
                            radius, radius / 2);
        painter->drawEllipse(highlightRect);
    }

    // Draw enhanced focus indicator with FluentUI compliance
    if (isFocused && m_showFocusIndicator) {
        painter->setBrush(Qt::NoBrush);

        // Enhanced focus ring for better accessibility
        if (m_highContrastMode) {
            // High contrast focus indicator
            painter->setPen(QPen(QColor(255, 255, 255), focusRingWidth + 1));
            painter->drawEllipse(center, radius + focusRingOffset + 1,
                                 radius + focusRingOffset + 1);
            painter->setPen(QPen(QColor(0, 0, 0), focusRingWidth));
            painter->drawEllipse(center, radius + focusRingOffset,
                                 radius + focusRingOffset);
        } else {
            // Standard FluentUI focus indicator
            painter->setPen(QPen(focusColor, focusRingWidth));
            painter->drawEllipse(center, radius + focusRingOffset,
                                 radius + focusRingOffset);

            // Add subtle outer glow for better visibility
            QColor glowColor = focusColor;
            glowColor.setAlpha(60);
            painter->setPen(QPen(glowColor, 1));
            painter->drawEllipse(center, radius + focusRingOffset + 2,
                                 radius + focusRingOffset + 2);
        }
    }
}

QPoint FluentSlider::getPositionFromValue(qreal value) const {
    // Use consistent padding values that match our track styling
    int trackPadding = 10;

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        qreal progress = (value - m_minimum) / (m_maximum - m_minimum);
        int x = qRound(trackPadding + (width() - 2 * trackPadding) * progress);
        return QPoint(x, height() / 2);
    } else {
        qreal progress = (value - m_minimum) / (m_maximum - m_minimum);
        int y = qRound(height() - trackPadding -
                       (height() - 2 * trackPadding) * progress);
        return QPoint(width() / 2, y);
    }
}

qreal FluentSlider::getValueFromPosition(const QPoint& position) const {
    // Use consistent padding values that match our track styling
    int trackPadding = 10;

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        qreal progress =
            qreal(position.x() - trackPadding) / (width() - 2 * trackPadding);
        progress = qBound(0.0, progress, 1.0);
        return m_minimum + progress * (m_maximum - m_minimum);
    } else {
        qreal progress = qreal(height() - trackPadding - position.y()) /
                         (height() - 2 * trackPadding);
        progress = qBound(0.0, progress, 1.0);
        return m_minimum + progress * (m_maximum - m_minimum);
    }
}

int FluentSlider::getHandleAtPosition(const QPoint& position) const {
    const int handleRadius = 12;  // Slightly larger for easier clicking

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
            return 0;  // Lower handle
        } else if (upperDist <= handleRadius) {
            return 1;  // Upper handle
        }
    }

    return -1;  // No handle
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
    if (!m_dragging)
        return;

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
    if (!m_showTooltip)
        return;

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
    if (!m_snapToTicks)
        return value;

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

// Enhanced utility methods with FluentUI styling
QRect FluentSlider::getTrackRect() const {
    // Use consistent values that match our enhanced track styling
    int trackThickness = 4;
    int trackPadding = 10;

    if (m_orientation == FluentSliderOrientation::Horizontal) {
        int y = (height() - trackThickness) / 2;
        return QRect(trackPadding, y, width() - 2 * trackPadding,
                     trackThickness);
    } else {
        int x = (width() - trackThickness) / 2;
        return QRect(x, trackPadding, trackThickness,
                     height() - 2 * trackPadding);
    }
}

QRect FluentSlider::getHandleRect(qreal value) const {
    QPoint center = getPositionFromValue(value);
    int radius = 8;
    return QRect(center.x() - radius, center.y() - radius, radius * 2,
                 radius * 2);
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

}  // namespace FluentQt::Components
