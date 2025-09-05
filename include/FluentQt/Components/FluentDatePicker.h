// include/FluentQt/Components/FluentDatePicker.h
#pragma once

#include <QCalendarWidget>
#include <QDate>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>
#include <memory>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentDateFormat {
    Short,   // MM/dd/yyyy
    Medium,  // MMM dd, yyyy
    Long,    // MMMM dd, yyyy
    ISO,     // yyyy-MM-dd
    Custom
};

enum class FluentComponentSize {
    Small,   // 24px height
    Medium,  // 32px height (default)
    Large    // 40px height
};

enum class FluentValidationState { None, Success, Warning, Error };

enum class FluentCalendarPlacement { Auto, Above, Below, Left, Right };

class FluentDatePicker : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY
                   selectedDateChanged)
    Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate NOTIFY
                   minimumDateChanged)
    Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate NOTIFY
                   maximumDateChanged)
    Q_PROPERTY(FluentDateFormat dateFormat READ dateFormat WRITE setDateFormat
                   NOTIFY dateFormatChanged)
    Q_PROPERTY(QString customFormat READ customFormat WRITE setCustomFormat
                   NOTIFY customFormatChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE
                   setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(
        bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool calendarVisible READ isCalendarVisible WRITE
                   setCalendarVisible NOTIFY calendarVisibilityChanged)
    Q_PROPERTY(
        FluentComponentSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(FluentValidationState validationState READ validationState WRITE
                   setValidationState NOTIFY validationStateChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage
                   NOTIFY errorMessageChanged)
    Q_PROPERTY(bool showValidationIcon READ showValidationIcon WRITE
                   setShowValidationIcon NOTIFY showValidationIconChanged)
    Q_PROPERTY(FluentCalendarPlacement placement READ placement WRITE
                   setPlacement NOTIFY placementChanged)

public:
    explicit FluentDatePicker(QWidget* parent = nullptr);
    explicit FluentDatePicker(const QDate& date, QWidget* parent = nullptr);
    ~FluentDatePicker() override;

    // Date properties
    QDate selectedDate() const;
    void setSelectedDate(const QDate& date);

    QDate minimumDate() const;
    void setMinimumDate(const QDate& date);

    QDate maximumDate() const;
    void setMaximumDate(const QDate& date);

    // Format properties
    FluentDateFormat dateFormat() const;
    void setDateFormat(FluentDateFormat format);

    QString customFormat() const;
    void setCustomFormat(const QString& format);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    // State properties
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    bool isCalendarVisible() const;
    void setCalendarVisible(bool visible);

    // Size properties
    FluentComponentSize size() const;
    void setSize(FluentComponentSize size);

    // Validation properties
    FluentValidationState validationState() const;
    void setValidationState(FluentValidationState state);
    void setValidationState(FluentValidationState state,
                            const QString& message);

    QString errorMessage() const;
    void setErrorMessage(const QString& message);

    bool showValidationIcon() const;
    void setShowValidationIcon(bool show);

    // Calendar placement
    FluentCalendarPlacement placement() const;
    void setPlacement(FluentCalendarPlacement placement);

    // Validation
    bool isValid() const;
    bool isDateInRange(const QDate& date) const;
    bool validateInput();
    void clearValidation();

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void showCalendar();
    void hideCalendar();
    void toggleCalendar();
    void clearDate();
    void setToday();

signals:
    void selectedDateChanged(const QDate& date);
    void minimumDateChanged(const QDate& date);
    void maximumDateChanged(const QDate& date);
    void dateFormatChanged(FluentDateFormat format);
    void customFormatChanged(const QString& format);
    void placeholderTextChanged(const QString& text);
    void readOnlyChanged(bool readOnly);
    void calendarVisibilityChanged(bool visible);
    void dateSelected(const QDate& date);
    void dateCleared();

    // New signals for enhanced functionality
    void sizeChanged(FluentComponentSize size);
    void validationStateChanged(FluentValidationState state);
    void errorMessageChanged(const QString& message);
    void showValidationIconChanged(bool show);
    void placementChanged(FluentCalendarPlacement placement);
    void validationCleared();
    void inputValidated(bool isValid);

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

private slots:
    void onCalendarDateSelected(const QDate& date);
    void onTextChanged(const QString& text);
    void onThemeChanged();
    void onCalendarAnimationFinished();

private:
    // Setup methods
    void setupUI();
    void setupCalendar();
    void setupAnimations();
    void updateColors();
    void updateFonts();

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintIcon(QPainter& painter, const QRect& rect);
    void paintFocusRing(QPainter& painter, const QRect& rect);

    // Animation methods
    void startShowCalendarAnimation();
    void startHideCalendarAnimation();

    // Utility methods
    QString formatDate(const QDate& date) const;
    QDate parseDate(const QString& text) const;
    void updateDisplayText();
    void updateCalendarPosition();
    QRect iconRect() const;
    QRect textRect() const;
    QRect validationIconRect() const;

    // Enhanced functionality
    void updateSizeConstraints();
    void updateValidationDisplay();
    void setupAccessibilityAttributes();
    void announceToScreenReader(const QString& message);
    bool isValidDateString(const QString& text) const;
    int getHeightForSize(FluentComponentSize size) const;
    QColor getValidationColor(FluentValidationState state) const;

private:
    // Date properties
    QDate m_selectedDate;
    QDate m_minimumDate{QDate(1900, 1, 1)};
    QDate m_maximumDate{QDate(2100, 12, 31)};

    // Format properties
    FluentDateFormat m_dateFormat{FluentDateFormat::Short};
    QString m_customFormat;
    QString m_placeholderText{"Select date"};

    // State
    bool m_readOnly{false};
    bool m_calendarVisible{false};
    bool m_pressed{false};

    // Enhanced properties
    FluentComponentSize m_size{FluentComponentSize::Medium};
    FluentValidationState m_validationState{FluentValidationState::None};
    QString m_errorMessage;
    bool m_showValidationIcon{true};
    FluentCalendarPlacement m_placement{FluentCalendarPlacement::Auto};

    // UI components
    QHBoxLayout* m_mainLayout{nullptr};
    QLineEdit* m_lineEdit{nullptr};
    QPushButton* m_calendarButton{nullptr};
    QCalendarWidget* m_calendar{nullptr};
    QWidget* m_calendarContainer{nullptr};

    // Animation
    std::unique_ptr<QPropertyAnimation> m_calendarAnimation;
    QGraphicsOpacityEffect* m_calendarOpacityEffect{nullptr};

    // Cached values
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

}  // namespace FluentQt::Components
