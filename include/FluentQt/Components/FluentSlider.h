// include/FluentQt/Components/FluentSlider.h
#pragma once

#include <QList>
#include <QPair>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

class QLabel;

namespace FluentQt::Components {

enum class FluentSliderOrientation { Horizontal, Vertical };

enum class FluentSliderMode {
    Single,  // Single handle
    Range    // Dual handles for range selection
};

enum class FluentSliderTickPosition {
    NoTicks,
    Above,  // Above horizontal slider, left of vertical
    Below,  // Below horizontal slider, right of vertical
    Both    // Both sides
};

struct FluentSliderTick {
    qreal value;
    QString label;
    bool major{true};  // Major or minor tick

    FluentSliderTick() = default;
    FluentSliderTick(qreal v) : value(v) {}
    FluentSliderTick(qreal v, const QString& l) : value(v), label(l) {}
    FluentSliderTick(qreal v, const QString& l, bool m)
        : value(v), label(l), major(m) {}
};

class FluentSlider : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentSliderOrientation orientation READ orientation WRITE
                   setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(FluentSliderMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(
        qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(
        qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(qreal lowerValue READ lowerValue WRITE setLowerValue NOTIFY
                   lowerValueChanged)
    Q_PROPERTY(qreal upperValue READ upperValue WRITE setUpperValue NOTIFY
                   upperValueChanged)
    Q_PROPERTY(qreal step READ step WRITE setStep NOTIFY stepChanged)
    Q_PROPERTY(
        qreal pageStep READ pageStep WRITE setPageStep NOTIFY pageStepChanged)
    Q_PROPERTY(FluentSliderTickPosition tickPosition READ tickPosition WRITE
                   setTickPosition NOTIFY tickPositionChanged)
    Q_PROPERTY(int tickInterval READ tickInterval WRITE setTickInterval NOTIFY
                   tickIntervalChanged)
    Q_PROPERTY(bool showLabels READ showLabels WRITE setShowLabels NOTIFY
                   showLabelsChanged)
    Q_PROPERTY(bool showTooltip READ showTooltip WRITE setShowTooltip NOTIFY
                   showTooltipChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(bool snapToTicks READ snapToTicks WRITE setSnapToTicks NOTIFY
                   snapToTicksChanged)
    Q_PROPERTY(bool showFocusIndicator READ showFocusIndicator WRITE
                   setShowFocusIndicator NOTIFY showFocusIndicatorChanged)
    Q_PROPERTY(bool highContrastMode READ highContrastMode WRITE
                   setHighContrastMode NOTIFY highContrastModeChanged)

public:
    explicit FluentSlider(QWidget* parent = nullptr);
    explicit FluentSlider(FluentSliderOrientation orientation,
                          QWidget* parent = nullptr);
    explicit FluentSlider(qreal minimum, qreal maximum,
                          QWidget* parent = nullptr);
    explicit FluentSlider(qreal minimum, qreal maximum, qreal value,
                          QWidget* parent = nullptr);
    ~FluentSlider() override;

    // Orientation and mode
    FluentSliderOrientation orientation() const;
    void setOrientation(FluentSliderOrientation orientation);

    FluentSliderMode mode() const;
    void setMode(FluentSliderMode mode);

    // Value range
    qreal minimum() const;
    void setMinimum(qreal minimum);

    qreal maximum() const;
    void setMaximum(qreal maximum);

    void setRange(qreal minimum, qreal maximum);

    // Single value mode
    qreal value() const;
    void setValue(qreal value);

    // Range mode values
    qreal lowerValue() const;
    void setLowerValue(qreal value);

    qreal upperValue() const;
    void setUpperValue(qreal value);

    void setValues(qreal lower, qreal upper);
    QPair<qreal, qreal> values() const;

    // Step configuration
    qreal step() const;
    void setStep(qreal step);

    qreal pageStep() const;
    void setPageStep(qreal pageStep);

    // Tick marks
    FluentSliderTickPosition tickPosition() const;
    void setTickPosition(FluentSliderTickPosition position);

    int tickInterval() const;
    void setTickInterval(int interval);

    void addTick(qreal value);
    void addTick(qreal value, const QString& label);
    void addTick(const FluentSliderTick& tick);
    void removeTick(qreal value);
    void clearTicks();

    QList<FluentSliderTick> ticks() const;
    void setTicks(const QList<FluentSliderTick>& ticks);

    // Labels and tooltips
    bool showLabels() const;
    void setShowLabels(bool show);

    bool showTooltip() const;
    void setShowTooltip(bool show);

    QString formatValue(qreal value) const;
    void setValueFormatter(std::function<QString(qreal)> formatter);

    // Animation and interaction
    bool isAnimated() const;
    void setAnimated(bool animated);

    bool snapToTicks() const;
    void setSnapToTicks(bool snap);

    // Focus and accessibility
    bool showFocusIndicator() const;
    void setShowFocusIndicator(bool show);

    bool highContrastMode() const;
    void setHighContrastMode(bool enabled);

    // Motion preferences
    bool respectMotionPreferences() const;
    void setRespectMotionPreferences(bool respect);

    // ARIA and accessibility
    QString ariaLabel() const;
    void setAriaLabel(const QString& label);

    QString ariaDescription() const;
    void setAriaDescription(const QString& description);

    QString ariaValueText() const;
    void setAriaValueText(const QString& valueText);

    // Utility methods
    qreal valueFromPosition(const QPoint& position) const;
    QPoint positionFromValue(qreal value) const;
    QRect handleRect(bool isUpper = false) const;
    QRect trackRect() const;

    bool isValidValue(qreal value) const;
    qreal constrainValue(qreal value) const;
    qreal snapValue(qreal value) const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void animateToValue(qreal value, int duration = 250);
    void animateToValues(qreal lower, qreal upper, int duration = 250);

signals:
    void orientationChanged(FluentSliderOrientation orientation);
    void modeChanged(FluentSliderMode mode);
    void minimumChanged(qreal minimum);
    void maximumChanged(qreal maximum);
    void valueChanged(qreal value);
    void lowerValueChanged(qreal value);
    void upperValueChanged(qreal value);
    void valuesChanged(qreal lower, qreal upper);
    void stepChanged(qreal step);
    void pageStepChanged(qreal step);
    void tickPositionChanged(FluentSliderTickPosition position);
    void tickIntervalChanged(int interval);
    void showLabelsChanged(bool show);
    void showTooltipChanged(bool show);
    void animatedChanged(bool animated);
    void snapToTicksChanged(bool snap);
    void showFocusIndicatorChanged(bool show);
    void highContrastModeChanged(bool enabled);

    void sliderPressed();
    void sliderMoved(qreal value);
    void sliderReleased();
    void rangeChanged(qreal minimum, qreal maximum);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onAnimationFinished();
    void onTooltipTimer();
    void updateColors();

private:
    void initializeAccessibility();
    void updateAccessibleValue();
    void announceValueChange();
    void detectSystemHighContrast();
    void initializeLiveRegion();
    void announceStateChange(const QString& message);
    void announceSliderPressed();
    void announceSliderReleased();
    void detectSystemMotionPreferences();
    QEasingCurve getOptimalEasingCurve() const;
    void setupAnimations();
    void updateLayout();
    void updateHandlePositions();

    // Painting methods
    void paintTrack(QPainter* painter);
    void paintProgress(QPainter* painter);
    void paintTicks(QPainter* painter);
    void paintLabels(QPainter* painter);
    void paintHandles(QPainter* painter);
    void paintHandle(QPainter* painter, qreal value, int handleIndex);

    // Tick and label drawing
    void drawTick(QPainter* painter, qreal value, int length);
    void drawLabel(QPainter* painter, qreal value, const QString& text);

    // Interaction methods
    int getHandleAtPosition(const QPoint& position) const;
    void startDrag(int handleIndex, const QPoint& position);
    void updateDrag(const QPoint& position);
    void endDrag();

    void showValueTooltip(const QPoint& position, qreal value);
    void hideValueTooltip();

    // Utility methods
    QRect getTrackRect() const;
    QRect getHandleRect(qreal value) const;
    QPoint getHandleCenter(qreal value) const;
    QPoint getPositionFromValue(qreal value) const;
    qreal getValueFromPosition(const QPoint& position) const;
    qreal snapValueToTick(qreal value) const;

    void generateAutoTicks();
    void updateTickPositions();

    void emitValueChanged();
    void constrainValues();

private:
    // Configuration
    FluentSliderOrientation m_orientation{FluentSliderOrientation::Horizontal};
    FluentSliderMode m_mode{FluentSliderMode::Single};
    qreal m_minimum{0.0};
    qreal m_maximum{100.0};
    qreal m_value{0.0};
    qreal m_lowerValue{0.0};
    qreal m_upperValue{100.0};
    qreal m_step{1.0};
    qreal m_pageStep{10.0};

    // Ticks and labels
    FluentSliderTickPosition m_tickPosition{FluentSliderTickPosition::NoTicks};
    qreal m_tickInterval{10.0};
    QList<FluentSliderTick> m_ticks;
    QMap<qreal, QString> m_customTicks;
    bool m_showLabels{false};
    bool m_showTooltip{true};
    std::function<QString(qreal)> m_valueFormatter;

    // Interaction and animation
    bool m_animated{true};
    bool m_snapToTicks{false};

    // Focus and accessibility
    bool m_showFocusIndicator{true};
    bool m_highContrastMode{false};
    bool m_respectMotionPreferences{true};

    // ARIA attributes
    QString m_ariaLabel;
    QString m_ariaDescription;
    QString m_ariaValueText;

    // Live region for announcements
    QLabel* m_liveRegion{nullptr};

    // State
    int m_activeHandle{-1};  // -1: none, 0: lower/single, 1: upper
    int m_hoveredHandle{-1};
    bool m_dragging{false};
    QPoint m_dragStartPosition;
    qreal m_dragStartValue{0.0};
    qreal m_dragStartLowerValue{0.0};
    qreal m_dragStartUpperValue{0.0};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_valueAnimation{nullptr};
    QPropertyAnimation* m_lowerValueAnimation{nullptr};
    QPropertyAnimation* m_upperValueAnimation{nullptr};

    // Tooltip
    QTimer* m_tooltipTimer{nullptr};
    QWidget* m_tooltipWidget{nullptr};

    // Layout cache
    QRect m_trackRect;
    QRect m_lowerHandleRect;
    QRect m_upperHandleRect;
    QList<QPoint> m_tickPositions;
    QList<QRect> m_labelRects;
    bool m_layoutDirty{true};
};

}  // namespace FluentQt::Components
