// src/Components/FluentCalendar.cpp
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QLocale>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>

namespace FluentQt::Components {

FluentCalendar::FluentCalendar(QWidget* parent)
    : Core::FluentComponent(parent),
      m_selectedDate(QDate::currentDate()),
      m_currentMonth(QDate::currentDate().month()),
      m_currentYear(QDate::currentDate().year()) {
    setupHeader();
    setupCalendarGrid();
    setupNavigation();

    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(300, 250);

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentCalendar::updateCalendarView);

    updateCalendarView();
}

void FluentCalendar::setSelectedDate(const QDate& date) {
    if (date.isValid() && isDateSelectable(date) && m_selectedDate != date) {
        m_selectedDate = date;

        if (m_selectionMode == FluentCalendarSelectionMode::SingleSelection) {
            m_selectedDates.clear();
            m_selectedDates.append(date);
        }

        update();
        emit selectedDateChanged(date);
        emit selectedDatesChanged(m_selectedDates);
    }
}

void FluentCalendar::setSelectedDates(const QList<QDate>& dates) {
    QList<QDate> validDates;
    for (const auto& date : dates) {
        if (date.isValid() && isDateSelectable(date)) {
            validDates.append(date);
        }
    }

    if (validDates != m_selectedDates) {
        m_selectedDates = validDates;

        if (!validDates.isEmpty()) {
            m_selectedDate = validDates.first();
            emit selectedDateChanged(m_selectedDate);
        }

        update();
        emit selectedDatesChanged(m_selectedDates);
    }
}

void FluentCalendar::setSelectedRange(const FluentDateRange& range) {
    if (range.isValid() && m_selectedRange.start != range.start &&
        m_selectedRange.end != range.end) {
        m_selectedRange = range;

        // Update selected dates list
        m_selectedDates.clear();
        QDate current = range.start;
        while (current <= range.end) {
            if (isDateSelectable(current)) {
                m_selectedDates.append(current);
            }
            current = current.addDays(1);
        }

        if (!m_selectedDates.isEmpty()) {
            m_selectedDate = m_selectedDates.first();
            emit selectedDateChanged(m_selectedDate);
        }

        update();
        emit selectedRangeChanged(range);
        emit selectedDatesChanged(m_selectedDates);
    }
}

void FluentCalendar::setSelectionMode(FluentCalendarSelectionMode mode) {
    if (m_selectionMode != mode) {
        m_selectionMode = mode;

        // Clear current selection if mode changed
        clearSelection();
    }
}

void FluentCalendar::setViewMode(FluentCalendarViewMode mode) {
    if (m_viewMode != mode) {
        FluentCalendarViewMode oldMode = m_viewMode;
        m_viewMode = mode;

        if (isAnimated()) {
            animateViewChange(oldMode, mode);
        } else {
            update();
        }

        emit viewModeChanged(mode);
    }
}

void FluentCalendar::setCurrentPage(int year, int month) {
    bool changed = false;

    if (m_currentYear != year) {
        m_currentYear = year;
        changed = true;
    }

    if (m_currentMonth != month) {
        m_currentMonth = month;
        changed = true;
    }

    if (changed) {
        updateCalendarView();
        emit currentPageChanged(year, month);
    }
}

void FluentCalendar::addSpecialDate(const QDate& date, const QString& tooltip) {
    if (date.isValid()) {
        m_specialDates[date] = tooltip;
        update();
    }
}

void FluentCalendar::addHoliday(const QDate& date, const QString& name) {
    if (date.isValid()) {
        m_holidays[date] = name;
        update();
    }
}

void FluentCalendar::showNextMonth() {
    if (m_viewMode == FluentCalendarViewMode::Month) {
        QDate nextMonth = QDate(m_currentYear, m_currentMonth, 1).addMonths(1);
        setCurrentPage(nextMonth.year(), nextMonth.month());
    }
}

void FluentCalendar::showPreviousMonth() {
    if (m_viewMode == FluentCalendarViewMode::Month) {
        QDate prevMonth = QDate(m_currentYear, m_currentMonth, 1).addMonths(-1);
        setCurrentPage(prevMonth.year(), prevMonth.month());
    }
}

void FluentCalendar::showNextYear() {
    setCurrentPage(m_currentYear + 1, m_currentMonth);
}

void FluentCalendar::showPreviousYear() {
    setCurrentPage(m_currentYear - 1, m_currentMonth);
}

void FluentCalendar::goToToday() {
    QDate today = QDate::currentDate();
    setCurrentPage(today.year(), today.month());
    setSelectedDate(today);
}

void FluentCalendar::clearSelection() {
    if (!m_selectedDates.isEmpty() || m_selectedRange.isValid()) {
        m_selectedDate = QDate();
        m_selectedDates.clear();
        m_selectedRange = FluentDateRange();

        update();
        emit selectedDatesChanged(m_selectedDates);
        emit selectedRangeChanged(m_selectedRange);
    }
}

void FluentCalendar::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect calendarRect = m_calendarWidget->geometry();

    switch (m_viewMode) {
        case FluentCalendarViewMode::Month:
            paintMonthView(painter, calendarRect);
            break;
        case FluentCalendarViewMode::Year:
            paintYearView(painter, calendarRect);
            break;
        case FluentCalendarViewMode::Decade:
            paintDecadeView(painter, calendarRect);
            break;
    }

    Core::FluentComponent::paintEvent(event);
}

void FluentCalendar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QDate clickedDate = getDateFromPos(event->pos());
        if (clickedDate.isValid() && isDateSelectable(clickedDate)) {
            m_selecting = true;
            m_selectionStart = clickedDate;

            updateSelection(clickedDate);
            emit dateClicked(clickedDate);
        }
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentCalendar::mouseMoveEvent(QMouseEvent* event) {
    if (m_selecting &&
        m_selectionMode == FluentCalendarSelectionMode::RangeSelection) {
        QDate hoverDate = getDateFromPos(event->pos());
        if (hoverDate.isValid() && isDateSelectable(hoverDate)) {
            updateRangeSelection(hoverDate);
        }
    }

    Core::FluentComponent::mouseMoveEvent(event);
}

void FluentCalendar::mouseReleaseEvent(QMouseEvent* event) {
    if (m_selecting) {
        m_selecting = false;

        if (m_selectionMode == FluentCalendarSelectionMode::RangeSelection &&
            m_selectionStart.isValid() && m_selectionEnd.isValid()) {
            FluentDateRange range;
            range.start = qMin(m_selectionStart, m_selectionEnd);
            range.end = qMax(m_selectionStart, m_selectionEnd);
            setSelectedRange(range);
        }
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentCalendar::mouseDoubleClickEvent(QMouseEvent* event) {
    QDate clickedDate = getDateFromPos(event->pos());
    if (clickedDate.isValid()) {
        emit dateDoubleClicked(clickedDate);

        // Switch view mode on double-click
        if (m_viewMode == FluentCalendarViewMode::Year) {
            setViewMode(FluentCalendarViewMode::Month);
        } else if (m_viewMode == FluentCalendarViewMode::Decade) {
            setViewMode(FluentCalendarViewMode::Year);
        }
    }

    Core::FluentComponent::mouseDoubleClickEvent(event);
}

void FluentCalendar::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Left:
            if (m_selectedDate.isValid()) {
                setSelectedDate(m_selectedDate.addDays(-1));
            }
            break;
        case Qt::Key_Right:
            if (m_selectedDate.isValid()) {
                setSelectedDate(m_selectedDate.addDays(1));
            }
            break;
        case Qt::Key_Up:
            if (m_selectedDate.isValid()) {
                setSelectedDate(m_selectedDate.addDays(-7));
            }
            break;
        case Qt::Key_Down:
            if (m_selectedDate.isValid()) {
                setSelectedDate(m_selectedDate.addDays(7));
            }
            break;
        case Qt::Key_PageUp:
            showPreviousMonth();
            break;
        case Qt::Key_PageDown:
            showNextMonth();
            break;
        case Qt::Key_Home:
            goToToday();
            break;
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (m_selectedDate.isValid()) {
                emit dateClicked(m_selectedDate);
            }
            break;
        default:
            Core::FluentComponent::keyPressEvent(event);
            break;
    }
}

void FluentCalendar::wheelEvent(QWheelEvent* event) {
    int delta = event->angleDelta().y();
    if (delta > 0) {
        showPreviousMonth();
    } else if (delta < 0) {
        showNextMonth();
    }

    event->accept();
}

void FluentCalendar::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);

    // Update calendar layout when size changes
    if (m_calendarWidget) {
        updateCalendarView();
    }
}

void FluentCalendar::setupHeader() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(8);

    m_headerLayout = new QHBoxLayout();

    m_prevButton = new FluentButton("‹", this);
    m_prevButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_prevButton->setFixedSize(32, 32);
    connect(m_prevButton, &FluentButton::clicked, this,
            &FluentCalendar::showPreviousMonth);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Make title clickable to switch view modes
    m_titleLabel->setCursor(Qt::PointingHandCursor);
    m_titleLabel->installEventFilter(this);

    m_nextButton = new FluentButton("›", this);
    m_nextButton->setButtonStyle(FluentButtonStyle::Subtle);
    m_nextButton->setFixedSize(32, 32);
    connect(m_nextButton, &FluentButton::clicked, this,
            &FluentCalendar::showNextMonth);

    if (m_showToday) {
        m_todayButton = new FluentButton("Today", this);
        m_todayButton->setButtonStyle(FluentButtonStyle::Primary);
        connect(m_todayButton, &FluentButton::clicked, this,
                &FluentCalendar::goToToday);
    }

    m_headerLayout->addWidget(m_prevButton);
    m_headerLayout->addWidget(m_titleLabel);
    m_headerLayout->addWidget(m_nextButton);

    if (m_todayButton) {
        m_headerLayout->addWidget(m_todayButton);
    }

    m_layout->addLayout(m_headerLayout);
}

void FluentCalendar::setupCalendarGrid() {
    m_calendarWidget = new QWidget(this);
    m_calendarWidget->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    m_layout->addWidget(m_calendarWidget);
}

void FluentCalendar::paintMonthView(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Calculate cell dimensions
    int cellWidth = rect.width() / 7;
    int cellHeight = (rect.height() - 30) / 7;  // Reserve space for day headers

    // Paint day headers
    QStringList dayNames;
    QLocale locale;
    for (int i = 0; i < 7; ++i) {
        int dayOfWeek = (static_cast<int>(m_firstDayOfWeek) + i - 1) % 7 + 1;
        dayNames << locale.dayName(dayOfWeek, QLocale::ShortFormat);
    }

    painter.setFont(
        QFont(font().family(), font().pointSize() - 1, QFont::Bold));
    painter.setPen(palette.neutralSecondary);

    for (int i = 0; i < 7; ++i) {
        QRect headerRect(rect.x() + i * cellWidth, rect.y(), cellWidth, 30);
        painter.drawText(headerRect, Qt::AlignCenter, dayNames[i]);
    }

    // Calculate first day of month
    QDate firstDay(m_currentYear, m_currentMonth, 1);
    int startDayOfWeek =
        (firstDay.dayOfWeek() - static_cast<int>(m_firstDayOfWeek) + 7) % 7;

    // Paint calendar cells
    QDate currentDate = firstDay.addDays(-startDayOfWeek);

    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            QRect cellRect(rect.x() + col * cellWidth,
                           rect.y() + 30 + row * cellHeight, cellWidth,
                           cellHeight);

            paintDateCell(painter, cellRect, currentDate);
            currentDate = currentDate.addDays(1);
        }
    }
}

void FluentCalendar::paintDateCell(QPainter& painter, const QRect& rect,
                                   const QDate& date) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    bool isCurrentMonth = (date.month() == m_currentMonth);
    bool isToday = (date == QDate::currentDate());
    bool isSelected = m_selectedDates.contains(date);
    bool isInRange = m_selectedRange.contains(date);
    bool isHoliday = m_holidays.contains(date);
    bool isSpecial = m_specialDates.contains(date);

    // Background
    QColor backgroundColor = Qt::transparent;
    if (isSelected || isInRange) {
        backgroundColor = palette.accent;
    } else if (isToday) {
        backgroundColor = palette.accentLight2;
    } else if (isSpecial) {
        backgroundColor = palette.infoLight;
    }

    if (backgroundColor != Qt::transparent) {
        QPainterPath path;
        path.addRoundedRect(rect.adjusted(2, 2, -2, -2), 4, 4);
        painter.fillPath(path, backgroundColor);
    }

    // Text
    QColor textColor;
    if (isSelected || isInRange) {
        textColor = palette.neutralLightest;
    } else if (isToday) {
        textColor = palette.accent;
    } else if (isHoliday) {
        textColor = palette.error;
    } else if (!isCurrentMonth) {
        textColor = palette.neutralTertiary;
    } else {
        textColor = palette.neutralPrimary;
    }

    painter.setPen(textColor);
    painter.setFont(
        isToday ? QFont(font().family(), font().pointSize(), QFont::Bold)
                : font());
    painter.drawText(rect, Qt::AlignCenter, QString::number(date.day()));

    // Special indicators
    if (isHoliday || isSpecial) {
        QRect indicatorRect(rect.right() - 8, rect.top() + 2, 4, 4);
        painter.fillRect(indicatorRect,
                         isHoliday ? palette.error : palette.info);
    }

    // Week numbers
    if (m_showWeekNumbers && rect.x() == m_calendarWidget->geometry().x()) {
        painter.setPen(palette.neutralTertiary);
        painter.setFont(QFont(font().family(), font().pointSize() - 2));
        QRect weekRect(rect.x() - 20, rect.y(), 18, rect.height());
        painter.drawText(weekRect, Qt::AlignCenter,
                         QString::number(date.weekNumber()));
    }
}

QDate FluentCalendar::getDateFromPos(const QPoint& pos) const {
    QRect calendarRect = m_calendarWidget->geometry();
    if (!calendarRect.contains(pos)) {
        return QDate();
    }

    if (m_viewMode != FluentCalendarViewMode::Month) {
        return QDate();  // Only handle month view for now
    }

    int cellWidth = calendarRect.width() / 7;
    int cellHeight = (calendarRect.height() - 30) / 7;

    int col = (pos.x() - calendarRect.x()) / cellWidth;
    int row = (pos.y() - calendarRect.y() - 30) / cellHeight;

    if (col >= 0 && col < 7 && row >= 0 && row < 6) {
        QDate firstDay(m_currentYear, m_currentMonth, 1);
        int startDayOfWeek =
            (firstDay.dayOfWeek() - static_cast<int>(m_firstDayOfWeek) + 7) % 7;
        return firstDay.addDays(-startDayOfWeek + row * 7 + col);
    }

    return QDate();
}

bool FluentCalendar::isDateSelectable(const QDate& date) const {
    return date.isValid() && date >= m_minimumDate && date <= m_maximumDate;
}

void FluentCalendar::updateSelection(const QDate& date) {
    switch (m_selectionMode) {
        case FluentCalendarSelectionMode::SingleSelection:
            setSelectedDate(date);
            break;
        case FluentCalendarSelectionMode::MultiSelection:
            updateMultiSelection(date);
            break;
        case FluentCalendarSelectionMode::RangeSelection:
            // Range selection handled in mouse events
            break;
        case FluentCalendarSelectionMode::WeekSelection: {
            // Select entire week
            QDate weekStart = date.addDays(
                -(date.dayOfWeek() - static_cast<int>(m_firstDayOfWeek) + 7) %
                7);
            QList<QDate> weekDates;
            for (int i = 0; i < 7; ++i) {
                weekDates.append(weekStart.addDays(i));
            }
            setSelectedDates(weekDates);
            break;
        }
        case FluentCalendarSelectionMode::MonthSelection: {
            // Select entire month
            QDate monthStart(date.year(), date.month(), 1);
            QDate monthEnd = monthStart.addMonths(1).addDays(-1);
            QList<QDate> monthDates;
            QDate current = monthStart;
            while (current <= monthEnd) {
                monthDates.append(current);
                current = current.addDays(1);
            }
            setSelectedDates(monthDates);
            break;
        }
    }
}

void FluentCalendar::updateCalendarView() {
    // Update title
    QString title;
    switch (m_viewMode) {
        case FluentCalendarViewMode::Month:
            title = QLocale().monthName(m_currentMonth, QLocale::LongFormat) +
                    " " + QString::number(m_currentYear);
            break;
        case FluentCalendarViewMode::Year:
            title = QString::number(m_currentYear);
            break;
        case FluentCalendarViewMode::Decade: {
            int decadeStart = (m_currentYear / 10) * 10;
            title = QString("%1 - %2").arg(decadeStart).arg(decadeStart + 9);
            break;
        }
    }
    m_titleLabel->setText(title);

    update();
}

void FluentCalendar::animateViewChange(FluentCalendarViewMode from,
                                       FluentCalendarViewMode to) {
    Q_UNUSED(from)
    Q_UNUSED(to)

    // Create smooth transition animation
    m_viewAnimation = std::make_unique<QPropertyAnimation>(this, "geometry");
    m_viewAnimation->setDuration(300);
    m_viewAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_viewAnimation.get(), &QPropertyAnimation::finished,
            [this]() { update(); });

    m_viewAnimation->start();
}

void FluentCalendar::setupNavigation() {
    // Create navigation buttons if they don't exist
    if (!m_prevButton) {
        m_prevButton = new FluentButton("<", this);
        m_prevButton->setFixedSize(32, 32);
        connect(m_prevButton, &FluentButton::clicked, this,
                &FluentCalendar::showPreviousMonth);
    }

    if (!m_nextButton) {
        m_nextButton = new FluentButton(">", this);
        m_nextButton->setFixedSize(32, 32);
        connect(m_nextButton, &FluentButton::clicked, this,
                &FluentCalendar::showNextMonth);
    }

    if (!m_todayButton) {
        m_todayButton = new FluentButton("Today", this);
        connect(m_todayButton, &FluentButton::clicked, this,
                &FluentCalendar::goToToday);
    }
}

void FluentCalendar::paintYearView(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.setPen(palette.neutralPrimary);
    painter.setFont(font());

    // Draw months in a 3x4 grid
    int monthWidth = rect.width() / 4;
    int monthHeight = rect.height() / 3;

    for (int month = 1; month <= 12; ++month) {
        int row = (month - 1) / 4;
        int col = (month - 1) % 4;

        QRect monthRect(rect.x() + col * monthWidth,
                        rect.y() + row * monthHeight, monthWidth, monthHeight);

        QString monthName = QLocale().monthName(month, QLocale::LongFormat);
        painter.drawText(monthRect, Qt::AlignCenter, monthName);
    }
}

void FluentCalendar::paintDecadeView(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.setPen(palette.neutralPrimary);
    painter.setFont(font());

    // Draw years in a 3x4 grid
    int startYear = (m_currentYear / 10) * 10;
    int yearWidth = rect.width() / 4;
    int yearHeight = rect.height() / 3;

    for (int i = 0; i < 12; ++i) {
        int year = startYear + i;
        int row = i / 4;
        int col = i % 4;

        QRect yearRect(rect.x() + col * yearWidth, rect.y() + row * yearHeight,
                       yearWidth, yearHeight);

        painter.drawText(yearRect, Qt::AlignCenter, QString::number(year));
    }
}

void FluentCalendar::updateMultiSelection(const QDate& date) {
    if (!date.isValid() || !isDateSelectable(date))
        return;

    if (m_selectedDates.contains(date)) {
        m_selectedDates.removeOne(date);
    } else {
        m_selectedDates.append(date);
    }

    update();
    emit selectedDatesChanged(m_selectedDates);
}

void FluentCalendar::updateRangeSelection(const QDate& date) {
    if (!date.isValid() || !isDateSelectable(date))
        return;

    if (m_selectedRange.start.isValid()) {
        if (date < m_selectedRange.start) {
            m_selectedRange.end = m_selectedRange.start;
            m_selectedRange.start = date;
        } else {
            m_selectedRange.end = date;
        }

        // Update selected dates list for the range
        m_selectedDates.clear();
        QDate current = m_selectedRange.start;
        while (current <= m_selectedRange.end) {
            if (isDateSelectable(current)) {
                m_selectedDates.append(current);
            }
            current = current.addDays(1);
        }

        update();
        emit selectedRangeChanged(m_selectedRange);
        emit selectedDatesChanged(m_selectedDates);
    }
}

void FluentCalendar::onTodayClicked() { goToToday(); }

void FluentCalendar::onNavigationClicked() {
    // Handle navigation button clicks - this is a generic handler
    // Specific navigation is handled by individual button connections
}

void FluentCalendar::setShowWeekNumbers(bool show) {
    if (m_showWeekNumbers != show) {
        m_showWeekNumbers = show;
        update();
    }
}

void FluentCalendar::setShowToday(bool show) {
    if (m_showToday != show) {
        m_showToday = show;
        if (m_todayButton) {
            m_todayButton->setVisible(show);
        }
        update();
    }
}

void FluentCalendar::setFirstDayOfWeek(Qt::DayOfWeek dayOfWeek) {
    if (m_firstDayOfWeek != dayOfWeek) {
        m_firstDayOfWeek = dayOfWeek;
        updateCalendarView();
    }
}

void FluentCalendar::setMaximumDate(const QDate& date) {
    if (m_maximumDate != date) {
        m_maximumDate = date;

        // Clear invalid selections
        if (m_selectedDate.isValid() && m_selectedDate > date) {
            clearSelection();
        }

        update();
    }
}

void FluentCalendar::setMinimumDate(const QDate& date) {
    if (m_minimumDate != date) {
        m_minimumDate = date;

        // Clear invalid selections
        if (m_selectedDate.isValid() && m_selectedDate < date) {
            clearSelection();
        }

        update();
    }
}

}  // namespace FluentQt::Components
