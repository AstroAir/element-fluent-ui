# FluentTimePicker Component

The `FluentTimePicker` component is a comprehensive time selection widget that implements Microsoft's Fluent Design principles. It provides multiple time input interfaces with smooth animations and extensive customization options.

## Overview

FluentTimePicker offers:
- **Multiple input modes** (Spinner, Clock, Dropdown, Compact)
- **12/24 hour formats** with automatic locale detection
- **Time range constraints** with validation
- **Smooth animations** for time transitions
- **Custom time intervals** and stepping
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Time Picker

```cpp
#include "FluentQt/Components/FluentTimePicker.h"

// Create a basic time picker
auto* timePicker = new FluentTimePicker;
timePicker->setTime(QTime(14, 30, 0));  // Set to 2:30 PM

// Handle time changes
connect(timePicker, &FluentTimePicker::timeChanged, [=](const QTime& time) {
    qDebug() << "Selected time:" << time.toString("hh:mm:ss");
});

// Add to layout
layout->addWidget(timePicker);
```

### Time Picker with Label

```cpp
auto* timePicker = new FluentTimePicker;
timePicker->setLabelText("Meeting Time");
timePicker->setTime(QTime(9, 0, 0));
timePicker->setTimeFormat("hh:mm AP");  // 12-hour format with AM/PM

// Show current time display
timePicker->setShowTimeDisplay(true);
timePicker->setTimeDisplayFormat("h:mm AP");
```

## Input Modes

### Spinner Mode

```cpp
auto* timePicker = new FluentTimePicker;
timePicker->setInputMode(FluentTimeInputMode::Spinner);

// Configure spinner behavior
timePicker->setSpinnerStyle(FluentSpinnerStyle::Separate);  // Separate hour/minute spinners
timePicker->setSpinnerStyle(FluentSpinnerStyle::Combined);  // Single spinner with time parts

// Set stepping intervals
timePicker->setHourStep(1);
timePicker->setMinuteStep(15);  // 15-minute intervals
timePicker->setSecondStep(30);  // 30-second intervals

// Enable wrapping
timePicker->setWrapping(true);  // 23:59 -> 00:00
```

### Clock Mode

```cpp
auto* timePicker = new FluentTimePicker;
timePicker->setInputMode(FluentTimeInputMode::Clock);

// Configure clock appearance
timePicker->setClockSize(200);
timePicker->setClockStyle(FluentClockStyle::Modern);
timePicker->setClockStyle(FluentClockStyle::Classic);

// Clock interaction
timePicker->setClockSnapToMinutes(true);
timePicker->setClockMinuteInterval(5);  // Snap to 5-minute intervals
timePicker->setShowClockNumbers(true);
timePicker->setShowClockTicks(true);

// Enable smooth hand movement
timePicker->setClockSmoothMovement(true);
```

### Dropdown Mode

```cpp
auto* timePicker = new FluentTimePicker;
timePicker->setInputMode(FluentTimeInputMode::Dropdown);

// Configure dropdown options
timePicker->setDropdownTimeInterval(30);  // 30-minute intervals
timePicker->setDropdownStartTime(QTime(6, 0));   // Start at 6:00 AM
timePicker->setDropdownEndTime(QTime(22, 0));    // End at 10:00 PM

// Custom dropdown items
QList<QTime> customTimes = {
    QTime(9, 0), QTime(9, 30), QTime(10, 0), QTime(10, 30),
    QTime(11, 0), QTime(11, 30), QTime(12, 0), QTime(12, 30),
    QTime(13, 0), QTime(13, 30), QTime(14, 0), QTime(14, 30)
};
timePicker->setDropdownTimes(customTimes);
```

### Compact Mode

```cpp
auto* timePicker = new FluentTimePicker;
timePicker->setInputMode(FluentTimeInputMode::Compact);

// Compact mode shows a button that opens a popup
timePicker->setCompactButtonSize(QSize(120, 32));
timePicker->setCompactShowIcon(true);

// Configure popup content
timePicker->setCompactPopupMode(FluentTimeInputMode::Clock);
timePicker->setCompactPopupSize(QSize(250, 250));
```

## Time Formats

### 12/24 Hour Formats

```cpp
auto* timePicker = new FluentTimePicker;

// 24-hour format
timePicker->setTimeFormat("HH:mm:ss");
timePicker->setTimeFormat("HH:mm");

// 12-hour format with AM/PM
timePicker->setTimeFormat("h:mm:ss AP");
timePicker->setTimeFormat("h:mm AP");

// Custom format
timePicker->setTimeFormat("hh 'hours' mm 'minutes'");

// Auto-detect from locale
timePicker->setTimeFormat(QLocale().timeFormat(QLocale::ShortFormat));
```

### Locale Support

```cpp
auto* timePicker = new FluentTimePicker;

// Set specific locale
timePicker->setLocale(QLocale::English);
timePicker->setLocale(QLocale::German);
timePicker->setLocale(QLocale::Japanese);

// Use system locale
timePicker->setLocale(QLocale::system());

// Custom AM/PM text
timePicker->setAmText("AM");
timePicker->setPmText("PM");
timePicker->setAmText("vormittags");  // German
timePicker->setPmText("nachmittags");
```

## Time Constraints

### Time Range Validation

```cpp
auto* timePicker = new FluentTimePicker;

// Set valid time range
timePicker->setMinimumTime(QTime(8, 0));   // 8:00 AM
timePicker->setMaximumTime(QTime(18, 0));  // 6:00 PM

// Business hours constraint
timePicker->setTimeRange(QTime(9, 0), QTime(17, 0));  // 9 AM to 5 PM

// Handle invalid time selection
connect(timePicker, &FluentTimePicker::invalidTimeSelected, [=](const QTime& time) {
    QString message = QString("Time %1 is outside business hours (9:00 AM - 5:00 PM)")
                        .arg(time.toString("h:mm AP"));
    QMessageBox::warning(this, "Invalid Time", message);
});
```

### Custom Validation

```cpp
auto* timePicker = new FluentTimePicker;

// Custom validation function
timePicker->setTimeValidator([](const QTime& time) -> bool {
    // Only allow times on the hour or half-hour
    return time.minute() == 0 || time.minute() == 30;
});

// Multiple constraints
timePicker->setTimeValidator([](const QTime& time) -> bool {
    // Business hours: 9 AM - 5 PM, Monday to Friday
    QTime start(9, 0);
    QTime end(17, 0);
    
    if (time < start || time > end) {
        return false;
    }
    
    // No lunch break: 12:00 - 13:00
    if (time >= QTime(12, 0) && time < QTime(13, 0)) {
        return false;
    }
    
    return true;
});

// Validation feedback
connect(timePicker, &FluentTimePicker::validationFailed, [=](const QTime& time, const QString& reason) {
    timePicker->showValidationMessage(reason);
});
```

## Animations

### Time Transition Animations

```cpp
auto* timePicker = new FluentTimePicker;

// Enable smooth time transitions
timePicker->setAnimated(true);
timePicker->setTimeTransitionDuration(300);
timePicker->setTimeTransitionEasing(FluentEasing::CubicOut);

// Animate to specific time
timePicker->animateToTime(QTime(15, 30));

// Custom transition effects
connect(timePicker, &FluentTimePicker::timeChanging, [=](const QTime& from, const QTime& to) {
    auto* animator = new FluentAnimator(this);
    
    // Animate display update
    QWidget* display = timePicker->timeDisplay();
    if (display) {
        animator->fadeOut(display, 150);
        QTimer::singleShot(150, [=]() {
            animator->fadeIn(display, 150);
        });
    }
});
```

### Clock Hand Animations

```cpp
auto* timePicker = new FluentTimePicker;
timePicker->setInputMode(FluentTimeInputMode::Clock);

// Enable smooth clock hand movement
timePicker->setClockHandAnimationEnabled(true);
timePicker->setClockHandAnimationDuration(400);
timePicker->setClockHandAnimationEasing(FluentEasing::BackOut);

// Custom hand animations
connect(timePicker, &FluentTimePicker::clockHandMoving, [=](FluentClockHand hand, qreal fromAngle, qreal toAngle) {
    auto* animator = new FluentAnimator(this);
    QWidget* handWidget = timePicker->clockHand(hand);
    
    if (handWidget) {
        // Rotate hand with easing
        animator->rotateEffect(handWidget, fromAngle, toAngle, 300, FluentEasing::ElasticOut);
    }
});
```

## Integration Examples

### Meeting Scheduler

```cpp
class MeetingScheduler : public QWidget
{
public:
    MeetingScheduler(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Title
        auto* titleLabel = new QLabel("Schedule Meeting");
        titleLabel->setFont(QFont("Segoe UI", 14, QFont::Bold));
        
        // Date picker (assuming FluentDatePicker exists)
        m_datePicker = new FluentDatePicker;
        m_datePicker->setLabelText("Meeting Date");
        m_datePicker->setDate(QDate::currentDate());
        
        // Start time
        m_startTimePicker = new FluentTimePicker;
        m_startTimePicker->setLabelText("Start Time");
        m_startTimePicker->setInputMode(FluentTimeInputMode::Dropdown);
        m_startTimePicker->setDropdownTimeInterval(15);  // 15-minute intervals
        m_startTimePicker->setTimeRange(QTime(8, 0), QTime(18, 0));  // Business hours
        m_startTimePicker->setTime(QTime(9, 0));
        
        // End time
        m_endTimePicker = new FluentTimePicker;
        m_endTimePicker->setLabelText("End Time");
        m_endTimePicker->setInputMode(FluentTimeInputMode::Dropdown);
        m_endTimePicker->setDropdownTimeInterval(15);
        m_endTimePicker->setTimeRange(QTime(8, 0), QTime(18, 0));
        m_endTimePicker->setTime(QTime(10, 0));
        
        // Duration display
        m_durationLabel = new QLabel("Duration: 1 hour");
        m_durationLabel->setStyleSheet("color: #666; font-style: italic;");
        
        // Time zone
        m_timeZoneCombo = new FluentComboBox;
        m_timeZoneCombo->setLabelText("Time Zone");
        m_timeZoneCombo->addItems({"UTC", "EST", "PST", "GMT", "CET"});
        m_timeZoneCombo->setCurrentText("EST");
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout;
        auto* scheduleButton = FluentButton::createPrimaryButton("Schedule Meeting");
        auto* cancelButton = new FluentButton("Cancel");
        
        buttonLayout->addStretch();
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(scheduleButton);
        
        layout->addWidget(titleLabel);
        layout->addWidget(m_datePicker);
        layout->addWidget(m_startTimePicker);
        layout->addWidget(m_endTimePicker);
        layout->addWidget(m_durationLabel);
        layout->addWidget(m_timeZoneCombo);
        layout->addLayout(buttonLayout);
        
        connect(scheduleButton, &FluentButton::clicked, this, &MeetingScheduler::scheduleMeeting);
        connect(cancelButton, &FluentButton::clicked, this, &QWidget::close);
    }
    
    void setupConnections() {
        // Update end time when start time changes
        connect(m_startTimePicker, &FluentTimePicker::timeChanged, [=](const QTime& startTime) {
            QTime currentEndTime = m_endTimePicker->time();
            if (currentEndTime <= startTime) {
                // Automatically set end time to 1 hour after start
                QTime newEndTime = startTime.addSecs(3600);
                m_endTimePicker->setTime(newEndTime);
            }
            updateDuration();
        });
        
        // Update duration when end time changes
        connect(m_endTimePicker, &FluentTimePicker::timeChanged, [=](const QTime& endTime) {
            QTime startTime = m_startTimePicker->time();
            if (endTime <= startTime) {
                // Show warning
                m_durationLabel->setText("End time must be after start time");
                m_durationLabel->setStyleSheet("color: red; font-style: italic;");
            } else {
                updateDuration();
            }
        });
    }
    
    void updateDuration() {
        QTime startTime = m_startTimePicker->time();
        QTime endTime = m_endTimePicker->time();
        
        if (endTime > startTime) {
            int seconds = startTime.secsTo(endTime);
            int hours = seconds / 3600;
            int minutes = (seconds % 3600) / 60;
            
            QString durationText;
            if (hours > 0) {
                durationText = QString("Duration: %1 hour%2").arg(hours).arg(hours > 1 ? "s" : "");
                if (minutes > 0) {
                    durationText += QString(" %1 minute%2").arg(minutes).arg(minutes > 1 ? "s" : "");
                }
            } else {
                durationText = QString("Duration: %1 minute%2").arg(minutes).arg(minutes > 1 ? "s" : "");
            }
            
            m_durationLabel->setText(durationText);
            m_durationLabel->setStyleSheet("color: #666; font-style: italic;");
        }
    }
    
    void scheduleMeeting() {
        QDate date = m_datePicker->date();
        QTime startTime = m_startTimePicker->time();
        QTime endTime = m_endTimePicker->time();
        QString timeZone = m_timeZoneCombo->currentText();
        
        if (endTime <= startTime) {
            QMessageBox::warning(this, "Invalid Time", "End time must be after start time.");
            return;
        }
        
        // Create meeting
        Meeting meeting;
        meeting.date = date;
        meeting.startTime = startTime;
        meeting.endTime = endTime;
        meeting.timeZone = timeZone;
        
        emit meetingScheduled(meeting);
        close();
    }

signals:
    void meetingScheduled(const Meeting& meeting);

private:
    FluentDatePicker* m_datePicker{nullptr};
    FluentTimePicker* m_startTimePicker{nullptr};
    FluentTimePicker* m_endTimePicker{nullptr};
    QLabel* m_durationLabel{nullptr};
    FluentComboBox* m_timeZoneCombo{nullptr};
};
```

### Alarm Clock Widget

```cpp
class AlarmClockWidget : public QWidget
{
public:
    AlarmClockWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupTimer();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Current time display
        m_currentTimeLabel = new QLabel;
        m_currentTimeLabel->setAlignment(Qt::AlignCenter);
        m_currentTimeLabel->setFont(QFont("Segoe UI", 24, QFont::Bold));
        m_currentTimeLabel->setStyleSheet("color: #0078D4; padding: 20px;");
        
        // Alarm time picker
        m_alarmTimePicker = new FluentTimePicker;
        m_alarmTimePicker->setLabelText("Alarm Time");
        m_alarmTimePicker->setInputMode(FluentTimeInputMode::Clock);
        m_alarmTimePicker->setClockSize(180);
        m_alarmTimePicker->setTime(QTime(7, 0));
        
        // Alarm controls
        auto* controlsLayout = new QHBoxLayout;
        
        m_alarmEnabledCheck = new FluentCheckBox("Enable Alarm");
        m_snoozeButton = new FluentButton("Snooze (5 min)");
        m_stopButton = FluentButton::createPrimaryButton("Stop Alarm");
        
        m_snoozeButton->setEnabled(false);
        m_stopButton->setEnabled(false);
        
        controlsLayout->addWidget(m_alarmEnabledCheck);
        controlsLayout->addStretch();
        controlsLayout->addWidget(m_snoozeButton);
        controlsLayout->addWidget(m_stopButton);
        
        // Status label
        m_statusLabel = new QLabel("Alarm disabled");
        m_statusLabel->setAlignment(Qt::AlignCenter);
        m_statusLabel->setStyleSheet("color: #666; font-style: italic;");
        
        layout->addWidget(m_currentTimeLabel);
        layout->addWidget(m_alarmTimePicker);
        layout->addLayout(controlsLayout);
        layout->addWidget(m_statusLabel);
    }
    
    void setupTimer() {
        m_clockTimer = new QTimer(this);
        connect(m_clockTimer, &QTimer::timeout, this, &AlarmClockWidget::updateCurrentTime);
        m_clockTimer->start(1000);  // Update every second
        
        updateCurrentTime();
    }
    
    void setupConnections() {
        connect(m_alarmEnabledCheck, &FluentCheckBox::toggled, this, &AlarmClockWidget::toggleAlarm);
        connect(m_alarmTimePicker, &FluentTimePicker::timeChanged, this, &AlarmClockWidget::updateAlarmStatus);
        connect(m_snoozeButton, &FluentButton::clicked, this, &AlarmClockWidget::snoozeAlarm);
        connect(m_stopButton, &FluentButton::clicked, this, &AlarmClockWidget::stopAlarm);
    }
    
    void updateCurrentTime() {
        QTime currentTime = QTime::currentTime();
        m_currentTimeLabel->setText(currentTime.toString("hh:mm:ss"));
        
        // Check if alarm should trigger
        if (m_alarmEnabled && currentTime.hour() == m_alarmTime.hour() && 
            currentTime.minute() == m_alarmTime.minute() && currentTime.second() == 0) {
            triggerAlarm();
        }
    }
    
    void toggleAlarm(bool enabled) {
        m_alarmEnabled = enabled;
        updateAlarmStatus();
    }
    
    void updateAlarmStatus() {
        m_alarmTime = m_alarmTimePicker->time();
        
        if (m_alarmEnabled) {
            QString timeStr = m_alarmTime.toString("h:mm AP");
            m_statusLabel->setText(QString("Alarm set for %1").arg(timeStr));
            m_statusLabel->setStyleSheet("color: #0078D4; font-weight: bold;");
        } else {
            m_statusLabel->setText("Alarm disabled");
            m_statusLabel->setStyleSheet("color: #666; font-style: italic;");
        }
    }
    
    void triggerAlarm() {
        m_snoozeButton->setEnabled(true);
        m_stopButton->setEnabled(true);
        m_statusLabel->setText("ALARM RINGING!");
        m_statusLabel->setStyleSheet("color: red; font-weight: bold; font-size: 14px;");
        
        // Flash the display
        auto* animator = new FluentAnimator(this);
        animator->blinkEffect(m_currentTimeLabel, 500, 10);  // Blink 10 times
        
        emit alarmTriggered(m_alarmTime);
    }
    
    void snoozeAlarm() {
        // Snooze for 5 minutes
        m_alarmTime = m_alarmTime.addSecs(300);
        m_alarmTimePicker->setTime(m_alarmTime);
        
        m_snoozeButton->setEnabled(false);
        m_stopButton->setEnabled(false);
        
        updateAlarmStatus();
    }
    
    void stopAlarm() {
        m_alarmEnabledCheck->setChecked(false);
        m_snoozeButton->setEnabled(false);
        m_stopButton->setEnabled(false);
        
        updateAlarmStatus();
    }

signals:
    void alarmTriggered(const QTime& time);

private:
    QLabel* m_currentTimeLabel{nullptr};
    FluentTimePicker* m_alarmTimePicker{nullptr};
    FluentCheckBox* m_alarmEnabledCheck{nullptr};
    FluentButton* m_snoozeButton{nullptr};
    FluentButton* m_stopButton{nullptr};
    QLabel* m_statusLabel{nullptr};
    QTimer* m_clockTimer{nullptr};
    
    bool m_alarmEnabled{false};
    QTime m_alarmTime;
};
```

## Accessibility

FluentTimePicker provides comprehensive accessibility support:

```cpp
auto* timePicker = new FluentTimePicker;

// Accessible properties (automatically set)
timePicker->setAccessibleName("Time picker");
timePicker->setAccessibleDescription("Select a time using various input methods");

// Keyboard navigation (automatic)
// - Tab to navigate between time components
// - Arrow keys to adjust time values
// - Enter to confirm selection
// - Escape to cancel changes

// Screen reader support
timePicker->setAccessibleRole(QAccessible::SpinBox);

// Time announcements
connect(timePicker, &FluentTimePicker::timeChanged, [=](const QTime& time) {
    QString announcement = QString("Time set to %1").arg(time.toString("h:mm AP"));
    timePicker->announceToScreenReader(announcement);
});
```

## Best Practices

### Time Picker Design Guidelines
1. **Use appropriate input modes** - Match the interface to user needs
2. **Provide clear time formats** - Show AM/PM or 24-hour as appropriate
3. **Include validation feedback** - Help users understand constraints
4. **Support keyboard navigation** - Ensure accessibility
5. **Show current selection** - Make the selected time obvious

### Performance Considerations
- Cache time calculations for smooth interactions
- Use efficient time validation
- Optimize clock rendering for smooth animations
- Implement lazy loading for dropdown options

### Accessibility Guidelines
- Support keyboard navigation
- Provide time announcements
- Use sufficient color contrast
- Include meaningful labels and descriptions
- Test with assistive technologies

## Signals Reference

```cpp
// Time change signals
void timeChanged(const QTime& time);
void timeSelected(const QTime& time);
void timeEdited(const QTime& time);

// Validation signals
void invalidTimeSelected(const QTime& time);
void validationFailed(const QTime& time, const QString& reason);

// Interaction signals
void timeChanging(const QTime& from, const QTime& to);
void clockHandMoving(FluentClockHand hand, qreal fromAngle, qreal toAngle);

// Mode change signals
void inputModeChanged(FluentTimeInputMode mode);
```

## See Also

- [FluentCalendar](FluentCalendar.md) - For date selection
- [FluentComboBox](FluentComboBox.md) - For dropdown time selection
- [FluentSlider](FluentSlider.md) - For time range selection
- [API Reference - FluentTimePicker](../API_REFERENCE.md#fluenttimepicker) - Complete API documentation
