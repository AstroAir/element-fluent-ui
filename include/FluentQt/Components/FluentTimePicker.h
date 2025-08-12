// include/FluentQt/Components/FluentTimePicker.h
#pragma once

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QTime>
#include <QVBoxLayout>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentSpinBox.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentTimePickerFormat {
    Hour12,  // 12-hour format with AM/PM
    Hour24   // 24-hour format
};

enum class FluentTimePickerStyle {
    SpinBox,  // Traditional spin boxes
    Clock,    // Analog clock interface
    Wheel,    // Scrollable wheels
    Compact   // Compact inline picker
};

enum class FluentTimePickerPrecision {
    Hour,        // Hour only
    Minute,      // Hour and minute
    Second,      // Hour, minute, and second
    Millisecond  // Full precision
};

class FluentClockWidget;
class FluentTimeWheel;

class FluentTimePicker : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(QTime minimumTime READ minimumTime WRITE setMinimumTime NOTIFY
                   minimumTimeChanged)
    Q_PROPERTY(QTime maximumTime READ maximumTime WRITE setMaximumTime NOTIFY
                   maximumTimeChanged)
    Q_PROPERTY(FluentTimePickerFormat format READ format WRITE setFormat NOTIFY
                   formatChanged)
    Q_PROPERTY(FluentTimePickerStyle style READ style WRITE setStyle NOTIFY
                   styleChanged)
    Q_PROPERTY(FluentTimePickerPrecision precision READ precision WRITE
                   setPrecision NOTIFY precisionChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE
                   setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(
        bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool showSeconds READ showSeconds WRITE setShowSeconds NOTIFY
                   showSecondsChanged)
    Q_PROPERTY(bool showMilliseconds READ showMilliseconds WRITE
                   setShowMilliseconds NOTIFY showMillisecondsChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(bool popupMode READ isPopupMode WRITE setPopupMode NOTIFY
                   popupModeChanged)
    Q_PROPERTY(int minuteStep READ minuteStep WRITE setMinuteStep NOTIFY
                   minuteStepChanged)
    Q_PROPERTY(int secondStep READ secondStep WRITE setSecondStep NOTIFY
                   secondStepChanged)

public:
    explicit FluentTimePicker(QWidget* parent = nullptr);
    explicit FluentTimePicker(const QTime& time, QWidget* parent = nullptr);
    explicit FluentTimePicker(FluentTimePickerStyle style,
                              QWidget* parent = nullptr);
    ~FluentTimePicker() override;

    // Time properties
    QTime time() const;
    void setTime(const QTime& time);

    QTime minimumTime() const;
    void setMinimumTime(const QTime& time);

    QTime maximumTime() const;
    void setMaximumTime(const QTime& time);

    void setTimeRange(const QTime& minimum, const QTime& maximum);

    // Format and style
    FluentTimePickerFormat format() const;
    void setFormat(FluentTimePickerFormat format);

    FluentTimePickerStyle style() const;
    void setStyle(FluentTimePickerStyle style);

    FluentTimePickerPrecision precision() const;
    void setPrecision(FluentTimePickerPrecision precision);

    // Text properties
    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    QString displayText() const;
    QString timeString() const;
    QString timeString(const QString& format) const;

    // Behavior properties
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    bool showSeconds() const;
    void setShowSeconds(bool show);

    bool showMilliseconds() const;
    void setShowMilliseconds(bool show);

    bool isAnimated() const;
    void setAnimated(bool animated);

    bool isPopupMode() const;
    void setPopupMode(bool popup);

    // Step properties
    int minuteStep() const;
    void setMinuteStep(int step);

    int secondStep() const;
    void setSecondStep(int step);

    // Validation
    bool isValidTime(const QTime& time) const;
    QTime boundTime(const QTime& time) const;

    // Utility methods
    int hour() const;
    void setHour(int hour);

    int minute() const;
    void setMinute(int minute);

    int second() const;
    void setSecond(int second);

    int msec() const;
    void setMsec(int msec);

    bool isPM() const;
    void setAMPM(bool pm);

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Access to sub-components
    FluentClockWidget* clockWidget() const { return m_clockWidget; }
    FluentSpinBox* hourSpinBox() const { return m_hourSpinBox; }
    FluentSpinBox* minuteSpinBox() const { return m_minuteSpinBox; }
    FluentSpinBox* secondSpinBox() const { return m_secondSpinBox; }

    // Static convenience methods
    static QTime getTime(const QTime& initial = QTime::currentTime(),
                         QWidget* parent = nullptr,
                         const QString& title = QString());
    static FluentTimePicker* createClockPicker(QWidget* parent = nullptr);
    static FluentTimePicker* createCompactPicker(QWidget* parent = nullptr);

public slots:
    void setCurrentTime();
    void showTimePopup();
    void hideTimePopup();
    void stepHourUp();
    void stepHourDown();
    void stepMinuteUp();
    void stepMinuteDown();
    void stepSecondUp();
    void stepSecondDown();

signals:
    void timeChanged(const QTime& time);
    void minimumTimeChanged(const QTime& time);
    void maximumTimeChanged(const QTime& time);
    void formatChanged(FluentTimePickerFormat format);
    void styleChanged(FluentTimePickerStyle style);
    void precisionChanged(FluentTimePickerPrecision precision);
    void placeholderTextChanged(const QString& text);
    void readOnlyChanged(bool readOnly);
    void showSecondsChanged(bool show);
    void showMillisecondsChanged(bool show);
    void animatedChanged(bool animated);
    void popupModeChanged(bool popup);
    void minuteStepChanged(int step);
    void secondStepChanged(int step);

    void timeSelected(const QTime& time);
    void editingFinished();
    void popupShown();
    void popupHidden();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onHourChanged(double value);
    void onMinuteChanged(double value);
    void onSecondChanged(double value);
    void onAMPMChanged();
    void onClockTimeChanged(const QTime& time);
    void onWheelTimeChanged(const QTime& time);
    void onTimeAnimationFinished();
    void updateDisplayText();
    void updateSpinBoxRanges();

private:
    void setupLayout();
    void setupSpinBoxes();
    void setupClock();
    void setupWheels();
    void setupAMPMSelector();
    void setupPopup();
    void setupAnimations();

    void updateLayout();
    void updateSizeMetrics();
    void updateVisibility();
    void updateSpinBoxValues();
    void updateClockTime();
    void updateWheelTime();

    // Time conversion methods
    void updateFromTime();
    void updateTimeFromSpinBoxes();
    QTime getTimeFromComponents() const;
    void setTimeToComponents(const QTime& time);

    // Format methods
    QString formatTimeString(const QTime& time) const;
    int convertTo12Hour(int hour) const;
    int convertTo24Hour(int hour, bool pm) const;

    // Animation methods
    void animateTimeChange(const QTime& newTime);
    void animatePopupShow();
    void animatePopupHide();

    // Popup methods
    void positionPopup();
    void createPopupWidget();
    void destroyPopupWidget();

private:
    // Core properties
    QTime m_time{QTime::currentTime()};
    QTime m_minimumTime{QTime(0, 0, 0)};
    QTime m_maximumTime{QTime(23, 59, 59, 999)};
    FluentTimePickerFormat m_format{FluentTimePickerFormat::Hour24};
    FluentTimePickerStyle m_style{FluentTimePickerStyle::SpinBox};
    FluentTimePickerPrecision m_precision{FluentTimePickerPrecision::Minute};
    QString m_placeholderText{"Select time..."};

    // Behavior properties
    bool m_readOnly{false};
    bool m_showSeconds{false};
    bool m_showMilliseconds{false};
    bool m_animated{true};
    bool m_popupMode{false};
    int m_minuteStep{1};
    int m_secondStep{1};

    // State
    bool m_showingPopup{false};
    bool m_updatingFromTime{false};
    bool m_updatingFromComponents{false};

    // Layout components
    QHBoxLayout* m_mainLayout{nullptr};
    QVBoxLayout* m_popupLayout{nullptr};
    QHBoxLayout* m_spinBoxLayout{nullptr};

    // SpinBox components
    FluentSpinBox* m_hourSpinBox{nullptr};
    FluentSpinBox* m_minuteSpinBox{nullptr};
    FluentSpinBox* m_secondSpinBox{nullptr};
    FluentSpinBox* m_millisecondSpinBox{nullptr};
    QComboBox* m_ampmComboBox{nullptr};

    // Labels
    QLabel* m_hourLabel{nullptr};
    QLabel* m_minuteLabel{nullptr};
    QLabel* m_secondLabel{nullptr};
    QLabel* m_colonLabel1{nullptr};
    QLabel* m_colonLabel2{nullptr};
    QLabel* m_dotLabel{nullptr};

    // Alternative interfaces
    FluentClockWidget* m_clockWidget{nullptr};
    FluentTimeWheel* m_hourWheel{nullptr};
    FluentTimeWheel* m_minuteWheel{nullptr};
    FluentTimeWheel* m_secondWheel{nullptr};

    // Popup
    QWidget* m_popupWidget{nullptr};
    FluentButton* m_popupButton{nullptr};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_timeAnimation{nullptr};
    QPropertyAnimation* m_popupAnimation{nullptr};

    // Animation properties
    Q_PROPERTY(QTime animatedTime READ animatedTime WRITE setAnimatedTime)
    Q_PROPERTY(qreal popupOpacity READ popupOpacity WRITE setPopupOpacity)

    QTime m_animatedTime;
    qreal m_popupOpacity{1.0};

    // Property accessors for animations
    QTime animatedTime() const { return m_animatedTime; }
    void setAnimatedTime(const QTime& time);

    qreal popupOpacity() const { return m_popupOpacity; }
    void setPopupOpacity(qreal opacity);
};

// Helper components
class FluentClockWidget : public QWidget {
    Q_OBJECT
public:
    explicit FluentClockWidget(QWidget* parent = nullptr);
    QTime time() const { return m_time; }
    void setTime(const QTime& time);
    void setFormat(FluentTimePickerFormat format);

signals:
    void timeChanged(const QTime& time);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QTime m_time{QTime::currentTime()};
    FluentTimePickerFormat m_format{FluentTimePickerFormat::Hour24};
    QPoint m_center;
    int m_radius{100};
    bool m_draggingHour{false};
    bool m_draggingMinute{false};
};

class FluentTimeWheel : public QScrollArea {
    Q_OBJECT
public:
    explicit FluentTimeWheel(QWidget* parent = nullptr);
    void setRange(int minimum, int maximum, int step = 1);
    void setValue(int value);
    int value() const { return m_value; }

signals:
    void valueChanged(int value);

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    int m_value{0};
    int m_minimum{0};
    int m_maximum{59};
    int m_step{1};
};

}  // namespace FluentQt::Components
