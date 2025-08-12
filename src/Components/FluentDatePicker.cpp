// src/Components/FluentDatePicker.cpp
#include "FluentQt/Components/FluentDatePicker.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QLocale>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QStyle>

namespace FluentQt::Components {

FluentDatePicker::FluentDatePicker(QWidget* parent)
    : Core::FluentComponent(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setObjectName("FluentDatePicker");

    setupUI();
    setupCalendar();
    setupAnimations();
    updateColors();
    updateFonts();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentDatePicker::onThemeChanged);
}

FluentDatePicker::FluentDatePicker(const QDate& date, QWidget* parent)
    : FluentDatePicker(parent) {
    setSelectedDate(date);
}

FluentDatePicker::~FluentDatePicker() = default;

void FluentDatePicker::setupUI() {
    FLUENT_PROFILE("FluentDatePicker::setupUI");

    // Create main layout
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(1, 1, 1, 1);
    m_mainLayout->setSpacing(0);

    // Create line edit for date display
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setObjectName("FluentDatePicker_LineEdit");
    m_lineEdit->setFrame(false);
    m_lineEdit->setPlaceholderText(m_placeholderText);
    m_lineEdit->setReadOnly(true);  // Always read-only, use calendar for input
    m_mainLayout->addWidget(m_lineEdit);

    // Create calendar button
    m_calendarButton = new QPushButton(this);
    m_calendarButton->setObjectName("FluentDatePicker_CalendarButton");
    m_calendarButton->setFixedSize(32, 32);
    m_calendarButton->setFlat(true);
    m_calendarButton->setText("📅");
    m_mainLayout->addWidget(m_calendarButton);

    // Connect signals
    connect(m_lineEdit, &QLineEdit::textChanged, this,
            &FluentDatePicker::onTextChanged);
    connect(m_calendarButton, &QPushButton::clicked, this,
            &FluentDatePicker::toggleCalendar);

    // Set minimum height
    setMinimumHeight(32);
    setFixedHeight(32);
}

void FluentDatePicker::setupCalendar() {
    FLUENT_PROFILE("FluentDatePicker::setupCalendar");

    // Create calendar container
    m_calendarContainer = new QWidget(this);
    m_calendarContainer->setObjectName("FluentDatePicker_CalendarContainer");
    m_calendarContainer->setWindowFlags(Qt::Popup);
    m_calendarContainer->hide();

    // Create calendar layout
    auto* calendarLayout = new QVBoxLayout(m_calendarContainer);
    calendarLayout->setContentsMargins(8, 8, 8, 8);

    // Create calendar widget
    m_calendar = new QCalendarWidget(m_calendarContainer);
    m_calendar->setObjectName("FluentDatePicker_Calendar");
    m_calendar->setGridVisible(true);
    m_calendar->setMinimumDate(m_minimumDate);
    m_calendar->setMaximumDate(m_maximumDate);
    calendarLayout->addWidget(m_calendar);

    // Connect calendar signals
    connect(m_calendar, &QCalendarWidget::clicked, this,
            &FluentDatePicker::onCalendarDateSelected);
    connect(m_calendar, &QCalendarWidget::activated, this,
            &FluentDatePicker::onCalendarDateSelected);
}

void FluentDatePicker::setupAnimations() {
    FLUENT_PROFILE("FluentDatePicker::setupAnimations");

    // Create opacity effect for calendar
    m_calendarOpacityEffect = new QGraphicsOpacityEffect(this);
    m_calendarOpacityEffect->setOpacity(0.0);
    m_calendarContainer->setGraphicsEffect(m_calendarOpacityEffect);

    // Create calendar animation
    m_calendarAnimation = std::make_unique<QPropertyAnimation>(
        m_calendarOpacityEffect, "opacity");
    m_calendarAnimation->setDuration(200);
    m_calendarAnimation->setEasingCurve(QEasingCurve::OutQuad);

    connect(m_calendarAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentDatePicker::onCalendarAnimationFinished);
}

QDate FluentDatePicker::selectedDate() const { return m_selectedDate; }

void FluentDatePicker::setSelectedDate(const QDate& date) {
    if (m_selectedDate != date) {
        if (date.isValid() && isDateInRange(date)) {
            m_selectedDate = date;
            m_calendar->setSelectedDate(date);
            updateDisplayText();
            m_sizeHintValid = false;
            updateGeometry();
            emit selectedDateChanged(date);
            emit dateSelected(date);
        }
    }
}

QDate FluentDatePicker::minimumDate() const { return m_minimumDate; }

void FluentDatePicker::setMinimumDate(const QDate& date) {
    if (m_minimumDate != date && date.isValid()) {
        m_minimumDate = date;
        m_calendar->setMinimumDate(date);

        // Adjust selected date if necessary
        if (m_selectedDate.isValid() && m_selectedDate < date) {
            setSelectedDate(date);
        }

        emit minimumDateChanged(date);
    }
}

QDate FluentDatePicker::maximumDate() const { return m_maximumDate; }

void FluentDatePicker::setMaximumDate(const QDate& date) {
    if (m_maximumDate != date && date.isValid()) {
        m_maximumDate = date;
        m_calendar->setMaximumDate(date);

        // Adjust selected date if necessary
        if (m_selectedDate.isValid() && m_selectedDate > date) {
            setSelectedDate(date);
        }

        emit maximumDateChanged(date);
    }
}

FluentDateFormat FluentDatePicker::dateFormat() const { return m_dateFormat; }

void FluentDatePicker::setDateFormat(FluentDateFormat format) {
    if (m_dateFormat != format) {
        m_dateFormat = format;
        updateDisplayText();
        emit dateFormatChanged(format);
    }
}

QString FluentDatePicker::customFormat() const { return m_customFormat; }

void FluentDatePicker::setCustomFormat(const QString& format) {
    if (m_customFormat != format) {
        m_customFormat = format;
        if (m_dateFormat == FluentDateFormat::Custom) {
            updateDisplayText();
        }
        emit customFormatChanged(format);
    }
}

QString FluentDatePicker::placeholderText() const { return m_placeholderText; }

void FluentDatePicker::setPlaceholderText(const QString& text) {
    if (m_placeholderText != text) {
        m_placeholderText = text;
        m_lineEdit->setPlaceholderText(text);
        emit placeholderTextChanged(text);
    }
}

bool FluentDatePicker::isReadOnly() const { return m_readOnly; }

void FluentDatePicker::setReadOnly(bool readOnly) {
    if (m_readOnly != readOnly) {
        m_readOnly = readOnly;
        m_calendarButton->setEnabled(!readOnly);
        emit readOnlyChanged(readOnly);
    }
}

bool FluentDatePicker::isCalendarVisible() const { return m_calendarVisible; }

void FluentDatePicker::setCalendarVisible(bool visible) {
    if (m_calendarVisible != visible) {
        m_calendarVisible = visible;

        if (visible) {
            startShowCalendarAnimation();
        } else {
            startHideCalendarAnimation();
        }

        emit calendarVisibilityChanged(visible);
    }
}

bool FluentDatePicker::isValid() const {
    return m_selectedDate.isValid() && isDateInRange(m_selectedDate);
}

bool FluentDatePicker::isDateInRange(const QDate& date) const {
    return date >= m_minimumDate && date <= m_maximumDate;
}

QSize FluentDatePicker::sizeHint() const {
    FLUENT_PROFILE("FluentDatePicker::sizeHint");

    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }

    const QFontMetrics fm(m_lineEdit->font());
    const QString sampleText =
        formatDate(QDate(2023, 12, 31));  // Sample long date
    const int textWidth = fm.horizontalAdvance(sampleText) + 20;  // Add padding
    const int buttonWidth = 32;
    const int totalWidth = textWidth + buttonWidth + 10;  // Add spacing

    m_cachedSizeHint = QSize(totalWidth, 32);
    m_sizeHintValid = true;

    return m_cachedSizeHint;
}

QSize FluentDatePicker::minimumSizeHint() const { return QSize(150, 32); }

void FluentDatePicker::showCalendar() { setCalendarVisible(true); }

void FluentDatePicker::hideCalendar() { setCalendarVisible(false); }

void FluentDatePicker::toggleCalendar() {
    setCalendarVisible(!m_calendarVisible);
}

void FluentDatePicker::clearDate() {
    if (m_selectedDate.isValid()) {
        m_selectedDate = QDate();
        updateDisplayText();
        emit selectedDateChanged(m_selectedDate);
        emit dateCleared();
    }
}

void FluentDatePicker::setToday() { setSelectedDate(QDate::currentDate()); }

void FluentDatePicker::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentDatePicker::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);

    // Paint focus ring
    if (hasFocus()) {
        paintFocusRing(painter, rect);
    }
}

void FluentDatePicker::paintBackground(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    QColor backgroundColor;
    switch (state()) {
        case Core::FluentState::Normal:
            backgroundColor = palette.neutralLighter;
            break;
        case Core::FluentState::Hovered:
            backgroundColor = palette.neutralLight;
            break;
        case Core::FluentState::Pressed:
            backgroundColor = palette.neutralQuaternaryAlt;
            break;
        case Core::FluentState::Disabled:
            backgroundColor = palette.neutralLighter;
            break;
        default:
            backgroundColor = palette.neutralLighter;
    }

    // Create path with rounded corners
    QPainterPath path;
    const int radius = cornerRadius();
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, backgroundColor);
}

void FluentDatePicker::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QColor borderColor;
    switch (state()) {
        case Core::FluentState::Normal:
            borderColor = palette.neutralSecondary;
            break;
        case Core::FluentState::Hovered:
            borderColor = palette.neutralPrimary;
            break;
        case Core::FluentState::Pressed:
        case Core::FluentState::Focused:
            borderColor = palette.accent;
            break;
        case Core::FluentState::Disabled:
            borderColor = palette.neutralTertiaryAlt;
            break;
        default:
            borderColor = palette.neutralSecondary;
    }

    QPen borderPen(borderColor, 1);
    painter.setPen(borderPen);

    const int radius = cornerRadius();
    const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentDatePicker::paintFocusRing(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QPen focusPen(palette.accent, 2);
    painter.setPen(focusPen);
    painter.setBrush(Qt::NoBrush);

    const int radius = cornerRadius() + 2;
    const QRect focusRect = rect.adjusted(-2, -2, 2, 2);

    painter.drawRoundedRect(focusRect, radius, radius);

    painter.restore();
}

void FluentDatePicker::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !m_readOnly) {
        m_pressed = true;
        setState(Core::FluentState::Pressed);

        if (!m_calendarVisible) {
            showCalendar();
        }
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentDatePicker::keyPressEvent(QKeyEvent* event) {
    if (!m_readOnly) {
        switch (event->key()) {
            case Qt::Key_Space:
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if (!event->isAutoRepeat()) {
                    toggleCalendar();
                }
                event->accept();
                return;

            case Qt::Key_Escape:
                if (m_calendarVisible) {
                    hideCalendar();
                    event->accept();
                    return;
                }
                break;

            case Qt::Key_Delete:
            case Qt::Key_Backspace:
                clearDate();
                event->accept();
                return;
        }
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentDatePicker::focusInEvent(QFocusEvent* event) {
    setState(Core::FluentState::Focused);
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentDatePicker::focusOutEvent(QFocusEvent* event) {
    setState(underMouse() ? Core::FluentState::Hovered
                          : Core::FluentState::Normal);

    // Hide calendar when losing focus (unless clicking on calendar)
    if (m_calendarVisible && event->reason() != Qt::PopupFocusReason) {
        hideCalendar();
    }

    update();
    Core::FluentComponent::focusOutEvent(event);
}

void FluentDatePicker::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
    m_sizeHintValid = false;
    updateCalendarPosition();
}

void FluentDatePicker::updateStateStyle() { update(); }

void FluentDatePicker::performStateTransition(Core::FluentState from,
                                              Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    update();
}

void FluentDatePicker::onCalendarDateSelected(const QDate& date) {
    setSelectedDate(date);
    hideCalendar();
}

void FluentDatePicker::onTextChanged(const QString& text) {
    Q_UNUSED(text)
    // Text changes are handled internally
}

void FluentDatePicker::onThemeChanged() {
    updateColors();
    updateFonts();
    update();
}

void FluentDatePicker::onCalendarAnimationFinished() {
    if (!m_calendarVisible) {
        m_calendarContainer->hide();
    }
}

void FluentDatePicker::updateColors() {
    // Colors are handled in paintEvent
}

void FluentDatePicker::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();
    m_lineEdit->setFont(theme.bodyFont());
}

void FluentDatePicker::startShowCalendarAnimation() {
    if (!isAnimated()) {
        m_calendarOpacityEffect->setOpacity(1.0);
        m_calendarContainer->show();
        updateCalendarPosition();
        return;
    }

    updateCalendarPosition();
    m_calendarContainer->show();

    m_calendarAnimation->setStartValue(0.0);
    m_calendarAnimation->setEndValue(1.0);
    m_calendarAnimation->start();
}

void FluentDatePicker::startHideCalendarAnimation() {
    if (!isAnimated()) {
        m_calendarOpacityEffect->setOpacity(0.0);
        m_calendarContainer->hide();
        return;
    }

    m_calendarAnimation->setStartValue(1.0);
    m_calendarAnimation->setEndValue(0.0);
    m_calendarAnimation->start();
}

QString FluentDatePicker::formatDate(const QDate& date) const {
    if (!date.isValid()) {
        return QString();
    }

    switch (m_dateFormat) {
        case FluentDateFormat::Short:
            return date.toString("MM/dd/yyyy");
        case FluentDateFormat::Medium:
            return date.toString("MMM dd, yyyy");
        case FluentDateFormat::Long:
            return date.toString("MMMM dd, yyyy");
        case FluentDateFormat::ISO:
            return date.toString("yyyy-MM-dd");
        case FluentDateFormat::Custom:
            return m_customFormat.isEmpty() ? date.toString()
                                            : date.toString(m_customFormat);
        default:
            return date.toString("MM/dd/yyyy");
    }
}

QDate FluentDatePicker::parseDate(const QString& text) const {
    if (text.isEmpty()) {
        return QDate();
    }

    // Try different formats
    QDate date = QDate::fromString(text, "MM/dd/yyyy");
    if (!date.isValid()) {
        date = QDate::fromString(text, "MMM dd, yyyy");
    }
    if (!date.isValid()) {
        date = QDate::fromString(text, "MMMM dd, yyyy");
    }
    if (!date.isValid()) {
        date = QDate::fromString(text, "yyyy-MM-dd");
    }
    if (!date.isValid() && !m_customFormat.isEmpty()) {
        date = QDate::fromString(text, m_customFormat);
    }

    return date;
}

void FluentDatePicker::updateDisplayText() {
    const QString text = formatDate(m_selectedDate);
    m_lineEdit->setText(text);
}

void FluentDatePicker::updateCalendarPosition() {
    if (!m_calendarContainer)
        return;

    const QPoint globalPos = mapToGlobal(QPoint(0, height()));
    m_calendarContainer->move(globalPos);
    m_calendarContainer->resize(m_calendar->sizeHint() + QSize(16, 16));
}

}  // namespace FluentQt::Components
