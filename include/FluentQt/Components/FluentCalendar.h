// include/FluentQt/Components/FluentCalendar.h
#pragma once

#include <QButtonGroup>
#include <QCalendarWidget>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QGridLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

// Forward declarations
class FluentButton;

enum class FluentCalendarSelectionMode {
    SingleSelection,
    MultiSelection,
    RangeSelection,
    WeekSelection,
    MonthSelection,
    // Compatibility aliases expected by tests
    MultipleSelection = MultiSelection,
    NoSelection = WeekSelection
};

enum class FluentCalendarViewMode { Month, Year, Decade };

struct FluentDateRange {
    QDate start;
    QDate end;

    FluentDateRange() = default;
    FluentDateRange(const QDate& s, const QDate& e) : start(s), end(e) {}

    QDate startDate() const { return start; }
    QDate endDate() const { return end; }

    bool contains(const QDate& date) const {
        return date >= start && date <= end;
    }

    bool isValid() const {
        return start.isValid() && end.isValid() && start <= end;
    }
};

class FluentCalendar : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY
                   selectedDateChanged)
    Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate)
    Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate)
    Q_PROPERTY(FluentCalendarSelectionMode selectionMode READ selectionMode
                   WRITE setSelectionMode)
    Q_PROPERTY(FluentCalendarViewMode viewMode READ viewMode WRITE setViewMode
                   NOTIFY viewModeChanged)
    Q_PROPERTY(bool showToday READ showToday WRITE setShowToday)
    Q_PROPERTY(
        bool showWeekNumbers READ showWeekNumbers WRITE setShowWeekNumbers)
    Q_PROPERTY(Qt::DayOfWeek firstDayOfWeek READ firstDayOfWeek WRITE
                   setFirstDayOfWeek)

public:
    explicit FluentCalendar(QWidget* parent = nullptr);

    // Selection
    QDate selectedDate() const noexcept { return m_selectedDate; }
    void setSelectedDate(const QDate& date);

    QList<QDate> selectedDates() const { return m_selectedDates; }
    void setSelectedDates(const QList<QDate>& dates);

    FluentDateRange selectedRange() const { return m_selectedRange; }
    void setSelectedRange(const FluentDateRange& range);

    // Date bounds
    QDate minimumDate() const noexcept { return m_minimumDate; }
    void setMinimumDate(const QDate& date);

    QDate maximumDate() const noexcept { return m_maximumDate; }
    void setMaximumDate(const QDate& date);

    void setDateRange(const QDate& min, const QDate& max);

    // Selection mode
    FluentCalendarSelectionMode selectionMode() const noexcept {
        return m_selectionMode;
    }
    void setSelectionMode(FluentCalendarSelectionMode mode);

    // View mode
    FluentCalendarViewMode viewMode() const noexcept { return m_viewMode; }
    void setViewMode(FluentCalendarViewMode mode);

    // Display options
    bool showToday() const noexcept { return m_showToday; }
    void setShowToday(bool show);

    bool showWeekNumbers() const noexcept { return m_showWeekNumbers; }
    void setShowWeekNumbers(bool show);

    Qt::DayOfWeek firstDayOfWeek() const noexcept { return m_firstDayOfWeek; }
    void setFirstDayOfWeek(Qt::DayOfWeek day);

    // Current view
    int currentMonth() const;
    void setCurrentMonth(int month);

    int currentYear() const;
    void setCurrentYear(int year);

    void setCurrentPage(int year, int month);

    // Special dates
    void addSpecialDate(const QDate& date, const QString& tooltip = QString());
    void removeSpecialDate(const QDate& date);
    void clearSpecialDates();
    QList<QDate> specialDates() const;

    // Holidays support
    void addHoliday(const QDate& date, const QString& name);
    void removeHoliday(const QDate& date);
    void clearHolidays();
    QMap<QDate, QString> holidays() const;

    // Navigation
    void showNextMonth();
    void showPreviousMonth();
    void showNextYear();
    void showPreviousYear();
    void goToToday();

public slots:
    void clearSelection();

signals:
    void selectedDateChanged(const QDate& date);
    void selectedDatesChanged(const QList<QDate>& dates);
    void selectedRangeChanged(const FluentDateRange& range);
    void viewModeChanged(FluentCalendarViewMode mode);
    void currentPageChanged(int year, int month);
    void dateClicked(const QDate& date);
    void dateDoubleClicked(const QDate& date);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onNavigationClicked();
    void onTodayClicked();
    void updateCalendarView();

private:
    void setupHeader();
    void setupCalendarGrid();
    void setupNavigation();
    void paintCalendarGrid(QPainter& painter, const QRect& rect);
    void paintMonthView(QPainter& painter, const QRect& rect);
    void paintYearView(QPainter& painter, const QRect& rect);
    void paintDecadeView(QPainter& painter, const QRect& rect);
    void paintDateCell(QPainter& painter, const QRect& rect, const QDate& date);
    void paintMonthCell(QPainter& painter, const QRect& rect, int month);
    void paintYearCell(QPainter& painter, const QRect& rect, int year);

    QRect getCellRect(int row, int col) const;
    QDate getDateFromPos(const QPoint& pos) const;
    QRect getDateRect(const QDate& date) const;

    bool isDateSelectable(const QDate& date) const;
    void updateSelection(const QDate& date);
    void updateRangeSelection(const QDate& date);
    void updateMultiSelection(const QDate& date);

    void animateViewChange(FluentCalendarViewMode from,
                           FluentCalendarViewMode to);

private:
    QVBoxLayout* m_layout;
    QHBoxLayout* m_headerLayout;
    QLabel* m_titleLabel;
    FluentButton* m_prevButton;
    FluentButton* m_nextButton;
    FluentButton* m_todayButton;
    QWidget* m_calendarWidget;

    QDate m_selectedDate;
    QList<QDate> m_selectedDates;
    FluentDateRange m_selectedRange;

    QDate m_minimumDate{QDate(1900, 1, 1)};
    QDate m_maximumDate{QDate(2100, 12, 31)};

    FluentCalendarSelectionMode m_selectionMode{
        FluentCalendarSelectionMode::SingleSelection};
    FluentCalendarViewMode m_viewMode{FluentCalendarViewMode::Month};

    bool m_showToday{true};
    bool m_showWeekNumbers{false};
    Qt::DayOfWeek m_firstDayOfWeek{Qt::Monday};

    int m_currentMonth;
    int m_currentYear;

    QMap<QDate, QString> m_specialDates;
    QMap<QDate, QString> m_holidays;

    // Selection state
    bool m_selecting{false};
    QDate m_selectionStart;
    QDate m_selectionEnd;

    std::unique_ptr<QPropertyAnimation> m_viewAnimation;
};

}  // namespace FluentQt::Components
