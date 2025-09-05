// src/Components/FluentDatePicker.cpp
#include "FluentQt/Components/FluentDatePicker.h"
#include "FluentQt/Accessibility/FluentAccessibilityCompliance.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
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
#include <QToolTip>

namespace FluentQt::Components {

FluentDatePicker::FluentDatePicker(QWidget* parent)
    : Core::FluentComponent(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setObjectName("FluentDatePicker");

    setupUI();
    setupCalendar();
    setupAnimations();
    setupAccessibilityAttributes();
    updateColors();
    updateFonts();
    updateSizeConstraints();

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

    const auto& theme = Styling::FluentTheme::instance();

    // Create main layout with proper spacing
    m_mainLayout = new QHBoxLayout(this);
    const int borderWidth = theme.spacing("xs");  // 2px
    m_mainLayout->setContentsMargins(borderWidth, borderWidth, borderWidth,
                                     borderWidth);
    m_mainLayout->setSpacing(0);

    // Create line edit for date display
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setObjectName("FluentDatePicker_LineEdit");
    m_lineEdit->setFrame(false);
    m_lineEdit->setPlaceholderText(m_placeholderText);
    m_lineEdit->setReadOnly(true);  // Always read-only, use calendar for input

    // Apply proper padding to line edit
    const int inputPadding = theme.spacing("s");  // 4px
    m_lineEdit->setStyleSheet(QString("QLineEdit { padding: %1px %2px; }")
                                  .arg(inputPadding)
                                  .arg(inputPadding * 2));

    m_mainLayout->addWidget(m_lineEdit);

    // Create calendar button with proper icon
    m_calendarButton = new QPushButton(this);
    m_calendarButton->setObjectName("FluentDatePicker_CalendarButton");
    m_calendarButton->setFlat(true);

    // Use proper Fluent UI calendar icon instead of emoji
    // For now using a more appropriate text representation
    m_calendarButton->setText(
        "📅");  // TODO: Replace with proper FluentIcon when available

    // Set button size based on component size
    updateSizeConstraints();

    m_mainLayout->addWidget(m_calendarButton);

    // Connect signals
    connect(m_lineEdit, &QLineEdit::textChanged, this,
            &FluentDatePicker::onTextChanged);
    connect(m_calendarButton, &QPushButton::clicked, this,
            &FluentDatePicker::toggleCalendar);
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

    const auto& theme = Styling::FluentTheme::instance();
    const QFontMetrics fm(m_lineEdit->font());
    const QString sampleText =
        formatDate(QDate(2023, 12, 31));  // Sample long date

    // Calculate text width with proper padding
    const int inputPadding = theme.spacing("s") * 2;  // Left and right padding
    const int textWidth = fm.horizontalAdvance(sampleText) + inputPadding * 2;

    // Get button width based on component size
    const int componentHeight = getHeightForSize(m_size);
    const int buttonWidth = componentHeight;  // Square button

    // Add spacing between elements
    const int elementSpacing = theme.spacing("xs");
    const int totalWidth = textWidth + buttonWidth + elementSpacing;

    m_cachedSizeHint = QSize(totalWidth, componentHeight);
    m_sizeHintValid = true;

    return m_cachedSizeHint;
}

QSize FluentDatePicker::minimumSizeHint() const {
    const auto& theme = Styling::FluentTheme::instance();
    const int minHeight = getHeightForSize(FluentComponentSize::Small);
    const int minWidth = theme.spacing("xxxl") * 6;  // 24px * 6 = 144px minimum
    return QSize(minWidth, minHeight);
}

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

// New enhanced functionality methods
FluentComponentSize FluentDatePicker::size() const { return m_size; }

void FluentDatePicker::setSize(FluentComponentSize size) {
    if (m_size != size) {
        m_size = size;
        updateSizeConstraints();
        emit sizeChanged(size);
    }
}

FluentValidationState FluentDatePicker::validationState() const {
    return m_validationState;
}

void FluentDatePicker::setValidationState(FluentValidationState state) {
    setValidationState(state, QString());
}

void FluentDatePicker::setValidationState(FluentValidationState state,
                                          const QString& message) {
    if (m_validationState != state || m_errorMessage != message) {
        m_validationState = state;
        m_errorMessage = message;
        updateValidationDisplay();
        emit validationStateChanged(state);
        emit errorMessageChanged(message);

        // Announce validation state change to screen readers
        if (state != FluentValidationState::None) {
            QString announcement =
                message.isEmpty() ? QString("Validation state changed to %1")
                                        .arg(static_cast<int>(state))
                                  : message;
            announceToScreenReader(announcement);
        }
    }
}

QString FluentDatePicker::errorMessage() const { return m_errorMessage; }

void FluentDatePicker::setErrorMessage(const QString& message) {
    if (m_errorMessage != message) {
        m_errorMessage = message;
        updateValidationDisplay();
        emit errorMessageChanged(message);
    }
}

bool FluentDatePicker::showValidationIcon() const {
    return m_showValidationIcon;
}

void FluentDatePicker::setShowValidationIcon(bool show) {
    if (m_showValidationIcon != show) {
        m_showValidationIcon = show;
        updateValidationDisplay();
        emit showValidationIconChanged(show);
    }
}

FluentCalendarPlacement FluentDatePicker::placement() const {
    return m_placement;
}

void FluentDatePicker::setPlacement(FluentCalendarPlacement placement) {
    if (m_placement != placement) {
        m_placement = placement;
        emit placementChanged(placement);
    }
}

bool FluentDatePicker::validateInput() {
    bool isValid = this->isValid();

    if (!isValid && m_validationState == FluentValidationState::None) {
        setValidationState(FluentValidationState::Error, "Invalid date");
    } else if (isValid && m_validationState == FluentValidationState::Error) {
        clearValidation();
    }

    emit inputValidated(isValid);
    return isValid;
}

void FluentDatePicker::clearValidation() {
    if (m_validationState != FluentValidationState::None ||
        !m_errorMessage.isEmpty()) {
        m_validationState = FluentValidationState::None;
        m_errorMessage.clear();
        updateValidationDisplay();
        emit validationCleared();
        emit validationStateChanged(m_validationState);
        emit errorMessageChanged(m_errorMessage);
    }
}

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

    // Paint validation icon
    paintIcon(painter, rect);

    // Paint focus ring
    if (hasFocus()) {
        paintFocusRing(painter, rect);
    }
}

void FluentDatePicker::paintBackground(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    QColor backgroundColor;

    // Check validation state first for error styling
    if (m_validationState == FluentValidationState::Error) {
        backgroundColor = theme.color("input.background.error");
    } else if (m_validationState == FluentValidationState::Warning) {
        backgroundColor = theme.color("input.background.warning");
    } else if (m_validationState == FluentValidationState::Success) {
        backgroundColor = theme.color("input.background.success");
    } else {
        // Use standard state-based colors
        switch (state()) {
            case Core::FluentState::Normal:
                backgroundColor = theme.color("input.background.rest");
                break;
            case Core::FluentState::Hovered:
                backgroundColor = theme.color("input.background.hover");
                break;
            case Core::FluentState::Pressed:
            case Core::FluentState::Focused:
                backgroundColor = theme.color("input.background.focus");
                break;
            case Core::FluentState::Disabled:
                backgroundColor = theme.color("input.background.disabled");
                break;
            default:
                backgroundColor = theme.color("input.background.rest");
        }
    }

    // Create path with rounded corners using design tokens
    QPainterPath path;
    const int radius = theme.spacing("xs");  // 2px corner radius for inputs
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, backgroundColor);
}

void FluentDatePicker::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();

    painter.save();

    QColor borderColor;
    int borderWidth = 1;

    // Check validation state first for error styling
    if (m_validationState == FluentValidationState::Error) {
        borderColor = theme.color("input.border.error");
        borderWidth = 2;  // Thicker border for errors
    } else if (m_validationState == FluentValidationState::Warning) {
        borderColor = theme.color("input.border.warning");
        borderWidth = 2;
    } else if (m_validationState == FluentValidationState::Success) {
        borderColor = theme.color("input.border.success");
    } else {
        // Use standard state-based colors
        switch (state()) {
            case Core::FluentState::Normal:
                borderColor = theme.color("input.border.rest");
                break;
            case Core::FluentState::Hovered:
                borderColor = theme.color("input.border.hover");
                break;
            case Core::FluentState::Pressed:
            case Core::FluentState::Focused:
                borderColor = theme.color("input.border.focus");
                borderWidth = 2;  // Thicker border for focus
                break;
            case Core::FluentState::Disabled:
                borderColor = theme.color("input.border.disabled");
                break;
            default:
                borderColor = theme.color("input.border.rest");
        }
    }

    QPen borderPen(borderColor, borderWidth);
    painter.setPen(borderPen);

    const int radius = theme.spacing("xs");  // 2px corner radius
    const qreal adjustment = borderWidth / 2.0;
    const QRectF borderRect =
        QRectF(rect).adjusted(adjustment, adjustment, -adjustment, -adjustment);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentDatePicker::paintIcon(QPainter& painter, const QRect& rect) {
    Q_UNUSED(rect)

    // Paint validation icon if enabled and validation state is not None
    if (m_showValidationIcon &&
        m_validationState != FluentValidationState::None) {
        const auto& theme = Styling::FluentTheme::instance();
        const QRect iconRect = validationIconRect();

        if (!iconRect.isEmpty()) {
            painter.save();

            QColor iconColor = getValidationColor(m_validationState);
            painter.setPen(QPen(iconColor, 2));
            painter.setBrush(Qt::NoBrush);

            // Draw simple validation icons
            const int centerX = iconRect.center().x();
            const int centerY = iconRect.center().y();
            const int radius = qMin(iconRect.width(), iconRect.height()) / 3;

            switch (m_validationState) {
                case FluentValidationState::Error:
                    // Draw X
                    painter.drawLine(centerX - radius, centerY - radius,
                                     centerX + radius, centerY + radius);
                    painter.drawLine(centerX + radius, centerY - radius,
                                     centerX - radius, centerY + radius);
                    break;
                case FluentValidationState::Warning:
                    // Draw exclamation mark
                    painter.drawLine(centerX, centerY - radius, centerX,
                                     centerY);
                    painter.drawPoint(centerX, centerY + radius / 2);
                    break;
                case FluentValidationState::Success:
                    // Draw checkmark
                    painter.drawLine(centerX - radius, centerY,
                                     centerX - radius / 2,
                                     centerY + radius / 2);
                    painter.drawLine(centerX - radius / 2, centerY + radius / 2,
                                     centerX + radius, centerY - radius / 2);
                    break;
                default:
                    break;
            }

            painter.restore();
        }
    }
}

void FluentDatePicker::paintFocusRing(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();

    painter.save();

    QColor focusColor = theme.color("input.border.focus");
    QPen focusPen(focusColor, 2);
    painter.setPen(focusPen);
    painter.setBrush(Qt::NoBrush);

    const int radius =
        theme.spacing("xs") + 2;  // 2px corner radius + 2px offset
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
                clearValidation();  // Clear validation when date is cleared
                event->accept();
                return;

            case Qt::Key_Up:
                // Navigate to previous day when calendar is not visible
                if (!m_calendarVisible && m_selectedDate.isValid()) {
                    QDate newDate = m_selectedDate.addDays(-1);
                    if (isDateInRange(newDate)) {
                        setSelectedDate(newDate);
                        validateInput();
                    }
                    event->accept();
                    return;
                }
                break;

            case Qt::Key_Down:
                // Navigate to next day when calendar is not visible
                if (!m_calendarVisible && m_selectedDate.isValid()) {
                    QDate newDate = m_selectedDate.addDays(1);
                    if (isDateInRange(newDate)) {
                        setSelectedDate(newDate);
                        validateInput();
                    }
                    event->accept();
                    return;
                }
                break;

            case Qt::Key_Home:
                // Go to today
                if (event->modifiers() & Qt::ControlModifier) {
                    setToday();
                    validateInput();
                    event->accept();
                    return;
                }
                break;

            case Qt::Key_F4:
                // Alt+F4 or F4 to toggle calendar (common shortcut)
                if (!event->isAutoRepeat()) {
                    toggleCalendar();
                    event->accept();
                    return;
                }
                break;
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

    QPoint position;
    const QSize calendarSize = m_calendar->sizeHint() + QSize(16, 16);
    const QRect screenGeometry =
        QGuiApplication::primaryScreen()->availableGeometry();
    const QPoint globalPos = mapToGlobal(QPoint(0, 0));

    // Calculate position based on placement preference
    switch (m_placement) {
        case FluentCalendarPlacement::Above:
            position =
                QPoint(globalPos.x(), globalPos.y() - calendarSize.height());
            break;
        case FluentCalendarPlacement::Below:
            position = QPoint(globalPos.x(), globalPos.y() + height());
            break;
        case FluentCalendarPlacement::Left:
            position =
                QPoint(globalPos.x() - calendarSize.width(), globalPos.y());
            break;
        case FluentCalendarPlacement::Right:
            position = QPoint(globalPos.x() + width(), globalPos.y());
            break;
        case FluentCalendarPlacement::Auto:
        default:
            // Auto placement - choose best position
            position = QPoint(globalPos.x(), globalPos.y() + height());

            // Check if calendar would go off screen and adjust
            if (position.y() + calendarSize.height() >
                screenGeometry.bottom()) {
                position.setY(globalPos.y() - calendarSize.height());
            }
            if (position.x() + calendarSize.width() > screenGeometry.right()) {
                position.setX(screenGeometry.right() - calendarSize.width());
            }
            if (position.x() < screenGeometry.left()) {
                position.setX(screenGeometry.left());
            }
            break;
    }

    m_calendarContainer->move(position);
    m_calendarContainer->resize(calendarSize);
}

QRect FluentDatePicker::validationIconRect() const {
    if (!m_showValidationIcon ||
        m_validationState == FluentValidationState::None) {
        return QRect();
    }

    const auto& theme = Styling::FluentTheme::instance();
    const int iconSize = theme.iconSize("small").width();  // 12px
    const int margin = theme.spacing("xs");                // 2px

    // Position icon on the right side of the input, before the calendar button
    const int buttonWidth = getHeightForSize(m_size);
    const int x = width() - buttonWidth - iconSize - margin * 2;
    const int y = (height() - iconSize) / 2;

    return QRect(x, y, iconSize, iconSize);
}

void FluentDatePicker::updateSizeConstraints() {
    const int componentHeight = getHeightForSize(m_size);
    const int buttonSize = componentHeight;

    setMinimumHeight(componentHeight);
    setMaximumHeight(componentHeight);

    if (m_calendarButton) {
        m_calendarButton->setFixedSize(buttonSize, buttonSize);
    }

    m_sizeHintValid = false;
    updateGeometry();
}

void FluentDatePicker::updateValidationDisplay() {
    update();  // Trigger repaint to show validation state

    // Update tooltip with error message
    if (m_validationState == FluentValidationState::Error &&
        !m_errorMessage.isEmpty()) {
        setToolTip(m_errorMessage);
    } else {
        setToolTip(QString());
    }
}

void FluentDatePicker::setupAccessibilityAttributes() {
    // Set ARIA role and properties for better screen reader support
    setProperty("role", "combobox");
    setProperty("aria-haspopup", "dialog");
    setProperty("aria-expanded", m_calendarVisible);
    setProperty("aria-readonly", m_readOnly);

    // Set accessible name and description
    if (accessibleName().isEmpty()) {
        setAccessibleName("Date picker");
    }

    if (accessibleDescription().isEmpty()) {
        setAccessibleDescription("Select a date from the calendar");
    }

    // Enable keyboard navigation
    setFocusPolicy(Qt::StrongFocus);
    setProperty("accessibleTabOrder", true);
}

void FluentDatePicker::announceToScreenReader(const QString& message) {
    // Use Qt's accessibility system to announce to screen readers
    if (QAccessibleInterface* iface =
            QAccessible::queryAccessibleInterface(this)) {
        QAccessibleEvent event(this, QAccessible::Alert);
        // Note: setValue is not available in Qt6, using alternative approach
        QAccessible::updateAccessibility(&event);
        QAccessible::updateAccessibility(&event);
    }
}

bool FluentDatePicker::isValidDateString(const QString& text) const {
    if (text.isEmpty()) {
        return true;  // Empty is valid (no date selected)
    }

    QDate parsed = parseDate(text);
    return parsed.isValid() && isDateInRange(parsed);
}

int FluentDatePicker::getHeightForSize(FluentComponentSize size) const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (size) {
        case FluentComponentSize::Small:
            return theme.spacing("xxxl");  // 24px
        case FluentComponentSize::Medium:
            return theme.spacing("xxxl") + theme.spacing("s");  // 32px
        case FluentComponentSize::Large:
            return theme.spacing("huge");  // 40px
        default:
            return theme.spacing("xxxl") + theme.spacing("s");  // 32px default
    }
}

QColor FluentDatePicker::getValidationColor(FluentValidationState state) const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (state) {
        case FluentValidationState::Error:
            return theme.color("semantic.error");
        case FluentValidationState::Warning:
            return theme.color("semantic.warning");
        case FluentValidationState::Success:
            return theme.color("semantic.success");
        default:
            return theme.color("text.primary");
    }
}

}  // namespace FluentQt::Components
