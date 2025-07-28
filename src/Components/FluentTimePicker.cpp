// src/Components/FluentTimePicker.cpp
#include "FluentQt/Components/FluentTimePicker.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QTimeEdit>
#include <QApplication>
#include <QAccessible>
#include <QTimer>
#include <QFontMetrics>
#include <cmath>

namespace FluentQt::Components {

FluentTimePicker::FluentTimePicker(QWidget* parent)
    : Core::FluentComponent(parent)
    , m_animator(std::make_unique<Animation::FluentAnimator>(this))
    , m_mainLayout(new QHBoxLayout(this))
    , m_hourSpinBox(new QSpinBox(this))
    , m_minuteSpinBox(new QSpinBox(this))
    , m_secondSpinBox(new QSpinBox(this))
    , m_amPmComboBox(new QComboBox(this))
    , m_timeEdit(new QTimeEdit(this))
    , m_scrollTimer(new QTimer(this)) {
    setupUI();
    setupAnimations();
    setupConnections();
    updateColors();
    updateTimeDisplay();
    updateAccessibility();
    
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentTimePicker::updateColors);
}

FluentTimePicker::FluentTimePicker(const QTime& time, QWidget* parent)
    : FluentTimePicker(parent) {
    setTime(time);
}

FluentTimePicker::FluentTimePicker(FluentTimeFormat format, QWidget* parent)
    : FluentTimePicker(parent) {
    setTimeFormat(format);
}

FluentTimePicker::~FluentTimePicker() = default;

QTime FluentTimePicker::time() const {
    return m_time;
}

void FluentTimePicker::setTime(const QTime& time) {
    if (m_time == time) return;
    
    m_time = time;
    updateTimeDisplay();
    updateSpinBoxes();
    
    if (m_animated) {
        animateTimeChange();
    }
    
    emit timeChanged(m_time);
    updateAccessibility();
}

FluentTimeFormat FluentTimePicker::timeFormat() const {
    return m_timeFormat;
}

void FluentTimePicker::setTimeFormat(FluentTimeFormat format) {
    if (m_timeFormat == format) return;
    
    m_timeFormat = format;
    updateFormatVisibility();
    updateTimeDisplay();
    updateSpinBoxes();
    emit timeFormatChanged(m_timeFormat);
}

FluentTimePickerStyle FluentTimePicker::pickerStyle() const {
    return m_pickerStyle;
}

void FluentTimePicker::setPickerStyle(FluentTimePickerStyle style) {
    if (m_pickerStyle == style) return;
    
    m_pickerStyle = style;
    updateStyleVisibility();
    emit pickerStyleChanged(m_pickerStyle);
}

bool FluentTimePicker::showSeconds() const {
    return m_showSeconds;
}

void FluentTimePicker::setShowSeconds(bool show) {
    if (m_showSeconds == show) return;
    
    m_showSeconds = show;
    m_secondSpinBox->setVisible(m_showSeconds && m_pickerStyle == FluentTimePickerStyle::SpinBox);
    updateTimeDisplay();
    emit showSecondsChanged(m_showSeconds);
}

bool FluentTimePicker::readOnly() const {
    return m_readOnly;
}

void FluentTimePicker::setReadOnly(bool readOnly) {
    if (m_readOnly == readOnly) return;
    
    m_readOnly = readOnly;
    
    m_hourSpinBox->setReadOnly(m_readOnly);
    m_minuteSpinBox->setReadOnly(m_readOnly);
    m_secondSpinBox->setReadOnly(m_readOnly);
    m_amPmComboBox->setEnabled(!m_readOnly);
    m_timeEdit->setReadOnly(m_readOnly);
    
    emit readOnlyChanged(m_readOnly);
}

bool FluentTimePicker::isAnimated() const {
    return m_animated;
}

void FluentTimePicker::setAnimated(bool animated) {
    if (m_animated == animated) return;
    
    m_animated = animated;
    emit animatedChanged(m_animated);
}

QTime FluentTimePicker::minimumTime() const {
    return m_minimumTime;
}

void FluentTimePicker::setMinimumTime(const QTime& time) {
    if (m_minimumTime == time) return;
    
    m_minimumTime = time;
    
    if (m_time < m_minimumTime) {
        setTime(m_minimumTime);
    }
    
    updateSpinBoxRanges();
    emit minimumTimeChanged(m_minimumTime);
}

QTime FluentTimePicker::maximumTime() const {
    return m_maximumTime;
}

void FluentTimePicker::setMaximumTime(const QTime& time) {
    if (m_maximumTime == time) return;
    
    m_maximumTime = time;
    
    if (m_time > m_maximumTime) {
        setTime(m_maximumTime);
    }
    
    updateSpinBoxRanges();
    emit maximumTimeChanged(m_maximumTime);
}

int FluentTimePicker::hourStep() const {
    return m_hourStep;
}

void FluentTimePicker::setHourStep(int step) {
    if (m_hourStep == step) return;
    
    m_hourStep = qMax(1, step);
    m_hourSpinBox->setSingleStep(m_hourStep);
    emit hourStepChanged(m_hourStep);
}

int FluentTimePicker::minuteStep() const {
    return m_minuteStep;
}

void FluentTimePicker::setMinuteStep(int step) {
    if (m_minuteStep == step) return;
    
    m_minuteStep = qMax(1, step);
    m_minuteSpinBox->setSingleStep(m_minuteStep);
    emit minuteStepChanged(m_minuteStep);
}

int FluentTimePicker::secondStep() const {
    return m_secondStep;
}

void FluentTimePicker::setSecondStep(int step) {
    if (m_secondStep == step) return;
    
    m_secondStep = qMax(1, step);
    m_secondSpinBox->setSingleStep(m_secondStep);
    emit secondStepChanged(m_secondStep);
}

QSize FluentTimePicker::sizeHint() const {
    switch (m_pickerStyle) {
        case FluentTimePickerStyle::SpinBox: {
            const QFontMetrics fm(font());
            const int digitWidth = fm.horizontalAdvance("00");
            const int separatorWidth = fm.horizontalAdvance(":");
            const int amPmWidth = fm.horizontalAdvance("AM");
            
            int width = digitWidth * 2; // Hours
            width += separatorWidth;
            width += digitWidth * 2; // Minutes
            
            if (m_showSeconds) {
                width += separatorWidth;
                width += digitWidth * 2; // Seconds
            }
            
            if (m_timeFormat == FluentTimeFormat::Format12Hour) {
                width += 8; // Spacing
                width += amPmWidth;
            }
            
            return QSize(width + 40, fm.height() + 16);
        }
        
        case FluentTimePickerStyle::Compact:
            return QSize(120, 32);
            
        case FluentTimePickerStyle::Wheel:
            return QSize(200, 150);
    }
    
    return QSize(150, 32);
}

QSize FluentTimePicker::minimumSizeHint() const {
    return QSize(100, 24);
}

FluentTimePicker* FluentTimePicker::create12HourPicker(QWidget* parent) {
    auto* picker = new FluentTimePicker(FluentTimeFormat::Format12Hour, parent);
    return picker;
}

FluentTimePicker* FluentTimePicker::create24HourPicker(QWidget* parent) {
    auto* picker = new FluentTimePicker(FluentTimeFormat::Format24Hour, parent);
    return picker;
}

FluentTimePicker* FluentTimePicker::createCompactPicker(QWidget* parent) {
    auto* picker = new FluentTimePicker(parent);
    picker->setPickerStyle(FluentTimePickerStyle::Compact);
    return picker;
}

FluentTimePicker* FluentTimePicker::createWheelPicker(QWidget* parent) {
    auto* picker = new FluentTimePicker(parent);
    picker->setPickerStyle(FluentTimePickerStyle::Wheel);
    return picker;
}

void FluentTimePicker::setCurrentTime() {
    setTime(QTime::currentTime());
}

void FluentTimePicker::addHours(int hours) {
    setTime(m_time.addSecs(hours * 3600));
}

void FluentTimePicker::addMinutes(int minutes) {
    setTime(m_time.addSecs(minutes * 60));
}

void FluentTimePicker::addSeconds(int seconds) {
    setTime(m_time.addSecs(seconds));
}

void FluentTimePicker::stepHourUp() {
    addHours(m_hourStep);
}

void FluentTimePicker::stepHourDown() {
    addHours(-m_hourStep);
}

void FluentTimePicker::stepMinuteUp() {
    addMinutes(m_minuteStep);
}

void FluentTimePicker::stepMinuteDown() {
    addMinutes(-m_minuteStep);
}

void FluentTimePicker::stepSecondUp() {
    addSeconds(m_secondStep);
}

void FluentTimePicker::stepSecondDown() {
    addSeconds(-m_secondStep);
}

void FluentTimePicker::setupUI() {
    setLayout(m_mainLayout);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(4);
    
    // Setup spin boxes
    setupSpinBoxes();
    
    // Setup compact time edit
    setupTimeEdit();
    
    // Setup wheel picker (placeholder)
    setupWheelPicker();
    
    updateStyleVisibility();
    updateFormatVisibility();
}

void FluentTimePicker::setupSpinBoxes() {
    // Hour spin box
    m_hourSpinBox->setMinimum(0);
    m_hourSpinBox->setMaximum(23);
    m_hourSpinBox->setSingleStep(m_hourStep);
    m_hourSpinBox->setWrapping(true);
    m_hourSpinBox->setAlignment(Qt::AlignCenter);
    m_hourSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_mainLayout->addWidget(m_hourSpinBox);
    
    // Separator
    auto* separator1 = new QLabel(":", this);
    separator1->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(separator1);
    m_separatorLabels.append(separator1);
    
    // Minute spin box
    m_minuteSpinBox->setMinimum(0);
    m_minuteSpinBox->setMaximum(59);
    m_minuteSpinBox->setSingleStep(m_minuteStep);
    m_minuteSpinBox->setWrapping(true);
    m_minuteSpinBox->setAlignment(Qt::AlignCenter);
    m_minuteSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_mainLayout->addWidget(m_minuteSpinBox);
    
    // Second separator and spin box
    auto* separator2 = new QLabel(":", this);
    separator2->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(separator2);
    m_separatorLabels.append(separator2);
    
    m_secondSpinBox->setMinimum(0);
    m_secondSpinBox->setMaximum(59);
    m_secondSpinBox->setSingleStep(m_secondStep);
    m_secondSpinBox->setWrapping(true);
    m_secondSpinBox->setAlignment(Qt::AlignCenter);
    m_secondSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_mainLayout->addWidget(m_secondSpinBox);
    
    // AM/PM combo box
    m_amPmComboBox->addItems({"AM", "PM"});
    m_amPmComboBox->setCurrentIndex(0);
    m_mainLayout->addWidget(m_amPmComboBox);
}

void FluentTimePicker::setupTimeEdit() {
    m_timeEdit->setDisplayFormat("hh:mm:ss");
    m_timeEdit->setTime(m_time);
    m_mainLayout->addWidget(m_timeEdit);
}

void FluentTimePicker::setupWheelPicker() {
    // Wheel picker implementation would go here
    // For now, we'll use a placeholder widget
    m_wheelWidget = new QWidget(this);
    m_wheelWidget->setMinimumSize(200, 150);
    m_mainLayout->addWidget(m_wheelWidget);
}

void FluentTimePicker::setupAnimations() {
    // Time change animation
    m_timeAnimation = new QPropertyAnimation(this, "animatedTime", this);
    m_timeAnimation->setDuration(300);
    m_timeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_timeAnimation, &QPropertyAnimation::finished,
            this, &FluentTimePicker::onTimeAnimationFinished);
    
    // Scroll animation for wheel picker
    m_scrollAnimation = new QPropertyAnimation(this, "wheelOffset", this);
    m_scrollAnimation->setDuration(200);
    m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void FluentTimePicker::setupConnections() {
    // Spin box connections
    connect(m_hourSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FluentTimePicker::onHourChanged);
    connect(m_minuteSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FluentTimePicker::onMinuteChanged);
    connect(m_secondSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FluentTimePicker::onSecondChanged);

    // AM/PM combo box
    connect(m_amPmComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FluentTimePicker::onAmPmChanged);

    // Time edit
    connect(m_timeEdit, &QTimeEdit::timeChanged,
            this, &FluentTimePicker::onTimeEditChanged);

    // Scroll timer for wheel picker
    connect(m_scrollTimer, &QTimer::timeout,
            this, &FluentTimePicker::onScrollTimer);
}

void FluentTimePicker::updateStyleVisibility() {
    const bool showSpinBoxes = (m_pickerStyle == FluentTimePickerStyle::SpinBox);
    const bool showTimeEdit = (m_pickerStyle == FluentTimePickerStyle::Compact);
    const bool showWheel = (m_pickerStyle == FluentTimePickerStyle::Wheel);

    m_hourSpinBox->setVisible(showSpinBoxes);
    m_minuteSpinBox->setVisible(showSpinBoxes);
    m_secondSpinBox->setVisible(showSpinBoxes && m_showSeconds);
    m_amPmComboBox->setVisible(showSpinBoxes && m_timeFormat == FluentTimeFormat::Format12Hour);

    for (auto* label : m_separatorLabels) {
        label->setVisible(showSpinBoxes);
    }

    m_timeEdit->setVisible(showTimeEdit);
    m_wheelWidget->setVisible(showWheel);
}

void FluentTimePicker::updateFormatVisibility() {
    if (m_timeFormat == FluentTimeFormat::Format12Hour) {
        m_hourSpinBox->setMaximum(12);
        m_hourSpinBox->setMinimum(1);
        m_amPmComboBox->setVisible(m_pickerStyle == FluentTimePickerStyle::SpinBox);
        m_timeEdit->setDisplayFormat(m_showSeconds ? "h:mm:ss AP" : "h:mm AP");
    } else {
        m_hourSpinBox->setMaximum(23);
        m_hourSpinBox->setMinimum(0);
        m_amPmComboBox->setVisible(false);
        m_timeEdit->setDisplayFormat(m_showSeconds ? "hh:mm:ss" : "hh:mm");
    }
}

void FluentTimePicker::updateTimeDisplay() {
    if (m_updatingFromControls) return;

    m_updatingFromTime = true;

    if (m_pickerStyle == FluentTimePickerStyle::Compact) {
        m_timeEdit->setTime(m_time);
    }

    m_updatingFromTime = false;
}

void FluentTimePicker::updateSpinBoxes() {
    if (m_updatingFromSpinBoxes || m_pickerStyle != FluentTimePickerStyle::SpinBox) return;

    m_updatingFromControls = true;

    if (m_timeFormat == FluentTimeFormat::Format12Hour) {
        int hour = m_time.hour();
        bool isPM = hour >= 12;

        if (hour == 0) hour = 12;
        else if (hour > 12) hour -= 12;

        m_hourSpinBox->setValue(hour);
        m_amPmComboBox->setCurrentIndex(isPM ? 1 : 0);
    } else {
        m_hourSpinBox->setValue(m_time.hour());
    }

    m_minuteSpinBox->setValue(m_time.minute());
    m_secondSpinBox->setValue(m_time.second());

    m_updatingFromControls = false;
}

void FluentTimePicker::updateSpinBoxRanges() {
    // Update ranges based on minimum and maximum times
    // This is a simplified implementation
    if (m_timeFormat == FluentTimeFormat::Format12Hour) {
        m_hourSpinBox->setRange(1, 12);
    } else {
        m_hourSpinBox->setRange(0, 23);
    }

    m_minuteSpinBox->setRange(0, 59);
    m_secondSpinBox->setRange(0, 59);
}

void FluentTimePicker::onHourChanged(int hour) {
    if (m_updatingFromControls) return;

    m_updatingFromSpinBoxes = true;

    int actualHour = hour;
    if (m_timeFormat == FluentTimeFormat::Format12Hour) {
        bool isPM = m_amPmComboBox->currentIndex() == 1;
        if (hour == 12) {
            actualHour = isPM ? 12 : 0;
        } else {
            actualHour = isPM ? hour + 12 : hour;
        }
    }

    QTime newTime(actualHour, m_time.minute(), m_time.second());
    if (newTime.isValid() && newTime >= m_minimumTime && newTime <= m_maximumTime) {
        setTime(newTime);
    }

    m_updatingFromSpinBoxes = false;
}

void FluentTimePicker::onMinuteChanged(int minute) {
    if (m_updatingFromControls) return;

    m_updatingFromSpinBoxes = true;

    QTime newTime(m_time.hour(), minute, m_time.second());
    if (newTime.isValid() && newTime >= m_minimumTime && newTime <= m_maximumTime) {
        setTime(newTime);
    }

    m_updatingFromSpinBoxes = false;
}

void FluentTimePicker::onSecondChanged(int second) {
    if (m_updatingFromControls) return;

    m_updatingFromSpinBoxes = true;

    QTime newTime(m_time.hour(), m_time.minute(), second);
    if (newTime.isValid() && newTime >= m_minimumTime && newTime <= m_maximumTime) {
        setTime(newTime);
    }

    m_updatingFromSpinBoxes = false;
}

void FluentTimePicker::onAmPmChanged(int index) {
    if (m_updatingFromControls) return;

    m_updatingFromSpinBoxes = true;

    int hour = m_hourSpinBox->value();
    bool isPM = (index == 1);

    int actualHour;
    if (hour == 12) {
        actualHour = isPM ? 12 : 0;
    } else {
        actualHour = isPM ? hour + 12 : hour;
    }

    QTime newTime(actualHour, m_time.minute(), m_time.second());
    if (newTime.isValid() && newTime >= m_minimumTime && newTime <= m_maximumTime) {
        setTime(newTime);
    }

    m_updatingFromSpinBoxes = false;
}

void FluentTimePicker::onTimeEditChanged(const QTime& time) {
    if (m_updatingFromTime) return;

    if (time.isValid() && time >= m_minimumTime && time <= m_maximumTime) {
        setTime(time);
    }
}

void FluentTimePicker::onTimeAnimationFinished() {
    m_animatedTime = m_time;
}

void FluentTimePicker::onScrollTimer() {
    // Wheel picker scroll handling would go here
}

void FluentTimePicker::wheelEvent(QWheelEvent* event) {
    if (m_readOnly || !isEnabled()) {
        event->ignore();
        return;
    }

    if (m_pickerStyle == FluentTimePickerStyle::SpinBox) {
        // Determine which component to scroll based on mouse position
        QWidget* targetWidget = childAt(event->position().toPoint());

        const int delta = event->angleDelta().y();
        const bool scrollUp = delta > 0;

        if (targetWidget == m_hourSpinBox) {
            if (scrollUp) stepHourUp(); else stepHourDown();
        } else if (targetWidget == m_minuteSpinBox) {
            if (scrollUp) stepMinuteUp(); else stepMinuteDown();
        } else if (targetWidget == m_secondSpinBox && m_showSeconds) {
            if (scrollUp) stepSecondUp(); else stepSecondDown();
        }

        event->accept();
    } else {
        Core::FluentComponent::wheelEvent(event);
    }
}

void FluentTimePicker::keyPressEvent(QKeyEvent* event) {
    if (m_readOnly || !isEnabled()) {
        Core::FluentComponent::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
        case Qt::Key_Up:
            if (event->modifiers() & Qt::ShiftModifier) {
                stepHourUp();
            } else {
                stepMinuteUp();
            }
            event->accept();
            return;

        case Qt::Key_Down:
            if (event->modifiers() & Qt::ShiftModifier) {
                stepHourDown();
            } else {
                stepMinuteDown();
            }
            event->accept();
            return;

        case Qt::Key_Left:
            stepMinuteDown();
            event->accept();
            return;

        case Qt::Key_Right:
            stepMinuteUp();
            event->accept();
            return;

        case Qt::Key_PageUp:
            stepHourUp();
            event->accept();
            return;

        case Qt::Key_PageDown:
            stepHourDown();
            event->accept();
            return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentTimePicker::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    const QColor backgroundColor = theme.color(Styling::FluentThemeColor::ControlFillDefault);
    const QColor textColor = theme.color(Styling::FluentThemeColor::TextFillPrimary);
    const QColor borderColor = theme.color(Styling::FluentThemeColor::ControlStrokeDefault);

    const QString styleSheet = QString(
        "QSpinBox {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 1px solid %3;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "    min-width: 40px;"
        "}"
        "QSpinBox:focus {"
        "    border-color: %4;"
        "}"
        "QSpinBox:disabled {"
        "    background-color: %5;"
        "    color: %6;"
        "}"
        "QComboBox {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 1px solid %3;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "    min-width: 50px;"
        "}"
        "QTimeEdit {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 1px solid %3;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "}"
        "QLabel {"
        "    color: %2;"
        "    font-weight: bold;"
        "}"
    ).arg(backgroundColor.name())
     .arg(textColor.name())
     .arg(borderColor.name())
     .arg(theme.color(Styling::FluentThemeColor::AccentFillDefault).name())
     .arg(theme.color(Styling::FluentThemeColor::ControlFillDisabled).name())
     .arg(theme.color(Styling::FluentThemeColor::TextFillDisabled).name());

    setStyleSheet(styleSheet);
}

void FluentTimePicker::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        const QString timeString = m_time.toString(
            m_timeFormat == FluentTimeFormat::Format12Hour ? "h:mm:ss AP" : "hh:mm:ss"
        );
        const QString accessibleText = tr("Time picker. Current time: %1").arg(timeString);
        setAccessibleName(accessibleText);
        setAccessibleDescription(accessibleText);

        QAccessibleEvent event(this, QAccessible::ValueChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void FluentTimePicker::animateTimeChange() {
    if (!m_timeAnimation) return;

    // For time animation, we could animate individual components
    // For simplicity, we'll just trigger the finished signal
    m_timeAnimation->stop();
    QTimer::singleShot(0, this, &FluentTimePicker::onTimeAnimationFinished);
}

// Property accessors for animations
QTime FluentTimePicker::animatedTime() const {
    return m_animatedTime;
}

void FluentTimePicker::setAnimatedTime(const QTime& time) {
    if (m_animatedTime == time) return;
    m_animatedTime = time;
    // Update display based on animated time if needed
}

qreal FluentTimePicker::wheelOffset() const {
    return m_wheelOffset;
}

void FluentTimePicker::setWheelOffset(qreal offset) {
    if (qFuzzyCompare(m_wheelOffset, offset)) return;
    m_wheelOffset = offset;
    // Update wheel display if needed
    if (m_wheelWidget) {
        m_wheelWidget->update();
    }
}

} // namespace FluentQt::Components
