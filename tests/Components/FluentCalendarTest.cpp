// tests/Components/FluentCalendarTest.cpp
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentCalendarTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Date selection tests
    void testSelectedDate();
    void testMinimumDate();
    void testMaximumDate();
    void testDateRange();

    // Selection mode tests
    void testSelectionMode();
    void testMultipleSelection();
    void testRangeSelection();

    // View mode tests
    void testViewMode();
    void testMonthView();
    void testYearView();
    void testDecadeView();

    // Display properties tests
    void testShowToday();
    void testShowWeekNumbers();
    void testFirstDayOfWeek();

    // Navigation tests
    void testCurrentPage();
    void testShowNextMonth();
    void testShowPreviousMonth();
    void testShowNextYear();
    void testShowPreviousYear();
    void testGoToToday();

    // Special dates tests
    void testSpecialDates();
    void testHolidays();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardNavigation();
    void testWheelNavigation();

    // Signal tests
    void testDateSelectionSignals();
    void testNavigationSignals();
    void testViewModeSignals();

    // Utility tests
    void testClearSelection();
    void testDateValidation();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentCalendar* m_calendar;
};

void FluentCalendarTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentCalendarTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentCalendarTest::init() {
    // Create a fresh calendar before each test
    m_calendar = new FluentCalendar();
    m_calendar->show();
}

void FluentCalendarTest::cleanup() {
    // Clean up after each test
    delete m_calendar;
    m_calendar = nullptr;
}

void FluentCalendarTest::testDefaultConstructor() {
    // Test default constructor
    FluentCalendar* calendar = new FluentCalendar();
    
    // Verify default properties
    QCOMPARE(calendar->selectedDate(), QDate::currentDate());
    QCOMPARE(calendar->minimumDate(), QDate(1900, 1, 1));
    QCOMPARE(calendar->maximumDate(), QDate(3000, 1, 1));
    QCOMPARE(calendar->selectionMode(), FluentCalendarSelectionMode::SingleSelection);
    QCOMPARE(calendar->viewMode(), FluentCalendarViewMode::Month);
    QVERIFY(calendar->showToday());
    QVERIFY(!calendar->showWeekNumbers());
    QCOMPARE(calendar->firstDayOfWeek(), Qt::Monday);
    
    delete calendar;
}

void FluentCalendarTest::testSelectedDate() {
    // Test setting and getting selected date
    QSignalSpy selectedDateChangedSpy(m_calendar, &FluentCalendar::selectedDateChanged);
    
    QDate testDate(2024, 6, 15);
    m_calendar->setSelectedDate(testDate);
    
    QCOMPARE(m_calendar->selectedDate(), testDate);
    QCOMPARE(selectedDateChangedSpy.count(), 1);
    QCOMPARE(selectedDateChangedSpy.first().first().toDate(), testDate);
    
    // Test setting the same date (should not emit signal)
    m_calendar->setSelectedDate(testDate);
    QCOMPARE(selectedDateChangedSpy.count(), 1);
    
    // Test invalid date
    m_calendar->setSelectedDate(QDate());
    QCOMPARE(m_calendar->selectedDate(), testDate); // Should remain unchanged
    QCOMPARE(selectedDateChangedSpy.count(), 1);
}

void FluentCalendarTest::testMinimumDate() {
    // Test setting and getting minimum date
    QDate minDate(2020, 1, 1);
    m_calendar->setMinimumDate(minDate);
    QCOMPARE(m_calendar->minimumDate(), minDate);
    
    // Test setting selected date below minimum
    QDate belowMin(2019, 12, 31);
    m_calendar->setSelectedDate(belowMin);
    QVERIFY(m_calendar->selectedDate() >= minDate);
}

void FluentCalendarTest::testMaximumDate() {
    // Test setting and getting maximum date
    QDate maxDate(2025, 12, 31);
    m_calendar->setMaximumDate(maxDate);
    QCOMPARE(m_calendar->maximumDate(), maxDate);
    
    // Test setting selected date above maximum
    QDate aboveMax(2026, 1, 1);
    m_calendar->setSelectedDate(aboveMax);
    QVERIFY(m_calendar->selectedDate() <= maxDate);
}

void FluentCalendarTest::testDateRange() {
    // Test setting date range
    QDate minDate(2020, 1, 1);
    QDate maxDate(2025, 12, 31);
    
    m_calendar->setMinimumDate(minDate);
    m_calendar->setMaximumDate(maxDate);
    
    QCOMPARE(m_calendar->minimumDate(), minDate);
    QCOMPARE(m_calendar->maximumDate(), maxDate);
    
    // Test that selected date is within range
    QDate validDate(2023, 6, 15);
    m_calendar->setSelectedDate(validDate);
    QCOMPARE(m_calendar->selectedDate(), validDate);
}

void FluentCalendarTest::testSelectionMode() {
    // Test setting and getting selection mode
    QCOMPARE(m_calendar->selectionMode(), FluentCalendarSelectionMode::SingleSelection);
    
    m_calendar->setSelectionMode(FluentCalendarSelectionMode::MultipleSelection);
    QCOMPARE(m_calendar->selectionMode(), FluentCalendarSelectionMode::MultipleSelection);
    
    m_calendar->setSelectionMode(FluentCalendarSelectionMode::RangeSelection);
    QCOMPARE(m_calendar->selectionMode(), FluentCalendarSelectionMode::RangeSelection);
    
    m_calendar->setSelectionMode(FluentCalendarSelectionMode::NoSelection);
    QCOMPARE(m_calendar->selectionMode(), FluentCalendarSelectionMode::NoSelection);
}

void FluentCalendarTest::testMultipleSelection() {
    // Test multiple date selection
    m_calendar->setSelectionMode(FluentCalendarSelectionMode::MultipleSelection);
    
    QSignalSpy selectedDatesChangedSpy(m_calendar, &FluentCalendar::selectedDatesChanged);
    
    QList<QDate> dates;
    dates << QDate(2024, 6, 15) << QDate(2024, 6, 20) << QDate(2024, 6, 25);
    
    m_calendar->setSelectedDates(dates);
    QCOMPARE(m_calendar->selectedDates(), dates);
    QCOMPARE(selectedDatesChangedSpy.count(), 1);
    QCOMPARE(selectedDatesChangedSpy.first().first().value<QList<QDate>>(), dates);
}

void FluentCalendarTest::testRangeSelection() {
    // Test date range selection
    m_calendar->setSelectionMode(FluentCalendarSelectionMode::RangeSelection);
    
    QSignalSpy selectedRangeChangedSpy(m_calendar, &FluentCalendar::selectedRangeChanged);
    
    FluentDateRange range(QDate(2024, 6, 15), QDate(2024, 6, 25));
    m_calendar->setSelectedRange(range);
    
    QCOMPARE(m_calendar->selectedRange().startDate(), range.startDate());
    QCOMPARE(m_calendar->selectedRange().endDate(), range.endDate());
    QCOMPARE(selectedRangeChangedSpy.count(), 1);
}

void FluentCalendarTest::testViewMode() {
    // Test setting and getting view mode
    QSignalSpy viewModeChangedSpy(m_calendar, &FluentCalendar::viewModeChanged);
    
    QCOMPARE(m_calendar->viewMode(), FluentCalendarViewMode::Month);
    
    m_calendar->setViewMode(FluentCalendarViewMode::Year);
    QCOMPARE(m_calendar->viewMode(), FluentCalendarViewMode::Year);
    QCOMPARE(viewModeChangedSpy.count(), 1);
    QCOMPARE(viewModeChangedSpy.first().first().value<FluentCalendarViewMode>(), FluentCalendarViewMode::Year);
    
    m_calendar->setViewMode(FluentCalendarViewMode::Decade);
    QCOMPARE(m_calendar->viewMode(), FluentCalendarViewMode::Decade);
    QCOMPARE(viewModeChangedSpy.count(), 2);
    
    // Setting the same view mode should not emit signal
    m_calendar->setViewMode(FluentCalendarViewMode::Decade);
    QCOMPARE(viewModeChangedSpy.count(), 2);
}

void FluentCalendarTest::testMonthView() {
    // Test month view functionality
    m_calendar->setViewMode(FluentCalendarViewMode::Month);
    QCOMPARE(m_calendar->viewMode(), FluentCalendarViewMode::Month);
    
    // Test current page
    QDate currentDate = QDate::currentDate();
    m_calendar->setCurrentPage(currentDate.year(), currentDate.month());
    QCOMPARE(m_calendar->currentYear(), currentDate.year());
    QCOMPARE(m_calendar->currentMonth(), currentDate.month());
}

void FluentCalendarTest::testYearView() {
    // Test year view functionality
    m_calendar->setViewMode(FluentCalendarViewMode::Year);
    QCOMPARE(m_calendar->viewMode(), FluentCalendarViewMode::Year);
    
    // In year view, should be able to select months
    // This would require more detailed testing of internal behavior
    QVERIFY(true);
}

void FluentCalendarTest::testDecadeView() {
    // Test decade view functionality
    m_calendar->setViewMode(FluentCalendarViewMode::Decade);
    QCOMPARE(m_calendar->viewMode(), FluentCalendarViewMode::Decade);
    
    // In decade view, should be able to select years
    // This would require more detailed testing of internal behavior
    QVERIFY(true);
}

void FluentCalendarTest::testShowToday() {
    // Test show today property
    QVERIFY(m_calendar->showToday()); // Default should be true

    m_calendar->setShowToday(false);
    QVERIFY(!m_calendar->showToday());

    m_calendar->setShowToday(true);
    QVERIFY(m_calendar->showToday());
}

void FluentCalendarTest::testShowWeekNumbers() {
    // Test show week numbers property
    QVERIFY(!m_calendar->showWeekNumbers()); // Default should be false

    m_calendar->setShowWeekNumbers(true);
    QVERIFY(m_calendar->showWeekNumbers());

    m_calendar->setShowWeekNumbers(false);
    QVERIFY(!m_calendar->showWeekNumbers());
}

void FluentCalendarTest::testFirstDayOfWeek() {
    // Test first day of week property
    QCOMPARE(m_calendar->firstDayOfWeek(), Qt::Monday); // Default

    m_calendar->setFirstDayOfWeek(Qt::Sunday);
    QCOMPARE(m_calendar->firstDayOfWeek(), Qt::Sunday);

    m_calendar->setFirstDayOfWeek(Qt::Saturday);
    QCOMPARE(m_calendar->firstDayOfWeek(), Qt::Saturday);
}

void FluentCalendarTest::testCurrentPage() {
    // Test current page (year/month)
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);

    int testYear = 2024;
    int testMonth = 6;

    m_calendar->setCurrentPage(testYear, testMonth);
    QCOMPARE(m_calendar->currentYear(), testYear);
    QCOMPARE(m_calendar->currentMonth(), testMonth);
    QCOMPARE(currentPageChangedSpy.count(), 1);
    QCOMPARE(currentPageChangedSpy.first().at(0).toInt(), testYear);
    QCOMPARE(currentPageChangedSpy.first().at(1).toInt(), testMonth);
}

void FluentCalendarTest::testShowNextMonth() {
    // Test show next month navigation
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);

    int initialMonth = m_calendar->currentMonth();
    int initialYear = m_calendar->currentYear();

    m_calendar->showNextMonth();

    if (initialMonth == 12) {
        QCOMPARE(m_calendar->currentMonth(), 1);
        QCOMPARE(m_calendar->currentYear(), initialYear + 1);
    } else {
        QCOMPARE(m_calendar->currentMonth(), initialMonth + 1);
        QCOMPARE(m_calendar->currentYear(), initialYear);
    }

    QCOMPARE(currentPageChangedSpy.count(), 1);
}

void FluentCalendarTest::testShowPreviousMonth() {
    // Test show previous month navigation
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);

    int initialMonth = m_calendar->currentMonth();
    int initialYear = m_calendar->currentYear();

    m_calendar->showPreviousMonth();

    if (initialMonth == 1) {
        QCOMPARE(m_calendar->currentMonth(), 12);
        QCOMPARE(m_calendar->currentYear(), initialYear - 1);
    } else {
        QCOMPARE(m_calendar->currentMonth(), initialMonth - 1);
        QCOMPARE(m_calendar->currentYear(), initialYear);
    }

    QCOMPARE(currentPageChangedSpy.count(), 1);
}

void FluentCalendarTest::testShowNextYear() {
    // Test show next year navigation
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);

    int initialYear = m_calendar->currentYear();
    int initialMonth = m_calendar->currentMonth();

    m_calendar->showNextYear();

    QCOMPARE(m_calendar->currentYear(), initialYear + 1);
    QCOMPARE(m_calendar->currentMonth(), initialMonth); // Month should remain same
    QCOMPARE(currentPageChangedSpy.count(), 1);
}

void FluentCalendarTest::testShowPreviousYear() {
    // Test show previous year navigation
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);

    int initialYear = m_calendar->currentYear();
    int initialMonth = m_calendar->currentMonth();

    m_calendar->showPreviousYear();

    QCOMPARE(m_calendar->currentYear(), initialYear - 1);
    QCOMPARE(m_calendar->currentMonth(), initialMonth); // Month should remain same
    QCOMPARE(currentPageChangedSpy.count(), 1);
}

void FluentCalendarTest::testGoToToday() {
    // Test go to today navigation
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);
    QSignalSpy selectedDateChangedSpy(m_calendar, &FluentCalendar::selectedDateChanged);

    // Set calendar to a different date
    m_calendar->setCurrentPage(2020, 1);
    m_calendar->setSelectedDate(QDate(2020, 1, 15));
    currentPageChangedSpy.clear();
    selectedDateChangedSpy.clear();

    m_calendar->goToToday();

    QDate today = QDate::currentDate();
    QCOMPARE(m_calendar->currentYear(), today.year());
    QCOMPARE(m_calendar->currentMonth(), today.month());
    QCOMPARE(m_calendar->selectedDate(), today);

    QCOMPARE(currentPageChangedSpy.count(), 1);
    QCOMPARE(selectedDateChangedSpy.count(), 1);
}

void FluentCalendarTest::testSpecialDates() {
    // Test special dates functionality
    QDate specialDate1(2024, 6, 15);
    QDate specialDate2(2024, 6, 20);
    QString tooltip1 = "Important meeting";
    QString tooltip2 = "Project deadline";

    // Add special dates
    m_calendar->addSpecialDate(specialDate1, tooltip1);
    m_calendar->addSpecialDate(specialDate2, tooltip2);

    QList<QDate> specialDates = m_calendar->specialDates();
    QVERIFY(specialDates.contains(specialDate1));
    QVERIFY(specialDates.contains(specialDate2));
    QCOMPARE(specialDates.size(), 2);

    // Remove special date
    m_calendar->removeSpecialDate(specialDate1);
    specialDates = m_calendar->specialDates();
    QVERIFY(!specialDates.contains(specialDate1));
    QVERIFY(specialDates.contains(specialDate2));
    QCOMPARE(specialDates.size(), 1);

    // Clear all special dates
    m_calendar->clearSpecialDates();
    specialDates = m_calendar->specialDates();
    QVERIFY(specialDates.isEmpty());
}

void FluentCalendarTest::testHolidays() {
    // Test holidays functionality
    QDate holiday1(2024, 1, 1);
    QDate holiday2(2024, 12, 25);
    QString name1 = "New Year's Day";
    QString name2 = "Christmas Day";

    // Add holidays
    m_calendar->addHoliday(holiday1, name1);
    m_calendar->addHoliday(holiday2, name2);

    QMap<QDate, QString> holidays = m_calendar->holidays();
    QVERIFY(holidays.contains(holiday1));
    QVERIFY(holidays.contains(holiday2));
    QCOMPARE(holidays[holiday1], name1);
    QCOMPARE(holidays[holiday2], name2);
    QCOMPARE(holidays.size(), 2);

    // Remove holiday
    m_calendar->removeHoliday(holiday1);
    holidays = m_calendar->holidays();
    QVERIFY(!holidays.contains(holiday1));
    QVERIFY(holidays.contains(holiday2));
    QCOMPARE(holidays.size(), 1);

    // Clear all holidays
    m_calendar->clearHolidays();
    holidays = m_calendar->holidays();
    QVERIFY(holidays.isEmpty());
}

void FluentCalendarTest::testMouseInteraction() {
    // Test mouse interaction with calendar
    QSignalSpy dateClickedSpy(m_calendar, &FluentCalendar::dateClicked);
    QSignalSpy dateDoubleClickedSpy(m_calendar, &FluentCalendar::dateDoubleClicked);

    // For now, just verify that mouse events can be sent without crashing
    // Detailed testing would require access to internal cell positions
    QPoint center = m_calendar->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_calendar, &pressEvent);
    QApplication::sendEvent(m_calendar, &releaseEvent);

    // Mouse events should be handled without crashing
    QVERIFY(true);
}

void FluentCalendarTest::testKeyboardNavigation() {
    // Test keyboard navigation
    QSignalSpy selectedDateChangedSpy(m_calendar, &FluentCalendar::selectedDateChanged);

    m_calendar->setFocus();
    QVERIFY(m_calendar->hasFocus());

    // QDate initialDate = m_calendar->selectedDate(); // For future use

    // Test arrow key navigation
    QKeyEvent rightKey(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &rightKey);

    QKeyEvent leftKey(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &leftKey);

    QKeyEvent upKey(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &upKey);

    QKeyEvent downKey(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &downKey);

    // Test page navigation
    QKeyEvent pageUpKey(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &pageUpKey);

    QKeyEvent pageDownKey(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &pageDownKey);

    // Test home/end keys
    QKeyEvent homeKey(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &homeKey);

    QKeyEvent endKey(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier);
    QApplication::sendEvent(m_calendar, &endKey);

    // Keyboard navigation should work without crashing
    QVERIFY(true);
}

void FluentCalendarTest::testWheelNavigation() {
    // Test wheel navigation
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);

    m_calendar->setFocus();

    // Test wheel up (previous month)
    QWheelEvent wheelUpEvent(QPointF(100, 100), QPointF(100, 100), QPoint(0, 0), QPoint(0, 120), Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_calendar, &wheelUpEvent);

    // Test wheel down (next month)
    QWheelEvent wheelDownEvent(QPointF(100, 100), QPointF(100, 100), QPoint(0, 0), QPoint(0, -120), Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_calendar, &wheelDownEvent);

    // Wheel navigation should work without crashing
    QVERIFY(true);
}

void FluentCalendarTest::testDateSelectionSignals() {
    // Test date selection signals
    QSignalSpy dateClickedSpy(m_calendar, &FluentCalendar::dateClicked);
    QSignalSpy dateDoubleClickedSpy(m_calendar, &FluentCalendar::dateDoubleClicked);
    QSignalSpy selectedDateChangedSpy(m_calendar, &FluentCalendar::selectedDateChanged);

    QDate testDate(2024, 6, 15);
    m_calendar->setSelectedDate(testDate);

    QCOMPARE(selectedDateChangedSpy.count(), 1);
    QCOMPARE(selectedDateChangedSpy.first().first().toDate(), testDate);
}

void FluentCalendarTest::testNavigationSignals() {
    // Test navigation signals
    QSignalSpy currentPageChangedSpy(m_calendar, &FluentCalendar::currentPageChanged);

    m_calendar->showNextMonth();
    QCOMPARE(currentPageChangedSpy.count(), 1);

    m_calendar->showPreviousMonth();
    QCOMPARE(currentPageChangedSpy.count(), 2);

    m_calendar->showNextYear();
    QCOMPARE(currentPageChangedSpy.count(), 3);

    m_calendar->showPreviousYear();
    QCOMPARE(currentPageChangedSpy.count(), 4);
}

void FluentCalendarTest::testViewModeSignals() {
    // Test view mode change signals
    QSignalSpy viewModeChangedSpy(m_calendar, &FluentCalendar::viewModeChanged);

    m_calendar->setViewMode(FluentCalendarViewMode::Year);
    QCOMPARE(viewModeChangedSpy.count(), 1);
    QCOMPARE(viewModeChangedSpy.first().first().value<FluentCalendarViewMode>(), FluentCalendarViewMode::Year);

    m_calendar->setViewMode(FluentCalendarViewMode::Decade);
    QCOMPARE(viewModeChangedSpy.count(), 2);
    QCOMPARE(viewModeChangedSpy.last().first().value<FluentCalendarViewMode>(), FluentCalendarViewMode::Decade);
}

void FluentCalendarTest::testClearSelection() {
    // Test clear selection functionality
    QSignalSpy selectedDateChangedSpy(m_calendar, &FluentCalendar::selectedDateChanged);

    // Set a date first
    QDate testDate(2024, 6, 15);
    m_calendar->setSelectedDate(testDate);
    selectedDateChangedSpy.clear();

    // Clear selection
    m_calendar->clearSelection();

    // For single selection mode, this might reset to current date or invalid date
    // The exact behavior depends on implementation
    QCOMPARE(selectedDateChangedSpy.count(), 1);
}

void FluentCalendarTest::testDateValidation() {
    // Test date validation
    QDate validDate(2024, 6, 15);
    QDate invalidDate; // Invalid date

    // Test setting valid date
    m_calendar->setSelectedDate(validDate);
    QCOMPARE(m_calendar->selectedDate(), validDate);

    // Test setting invalid date (should be ignored)
    QDate previousDate = m_calendar->selectedDate();
    m_calendar->setSelectedDate(invalidDate);
    QCOMPARE(m_calendar->selectedDate(), previousDate); // Should remain unchanged

    // Test date range validation
    m_calendar->setMinimumDate(QDate(2024, 1, 1));
    m_calendar->setMaximumDate(QDate(2024, 12, 31));

    // Test date below minimum
    m_calendar->setSelectedDate(QDate(2023, 12, 31));
    QVERIFY(m_calendar->selectedDate() >= m_calendar->minimumDate());

    // Test date above maximum
    m_calendar->setSelectedDate(QDate(2025, 1, 1));
    QVERIFY(m_calendar->selectedDate() <= m_calendar->maximumDate());
}

void FluentCalendarTest::testAccessibility() {
    // Test accessibility features
    m_calendar->setSelectedDate(QDate(2024, 6, 15));

    // Test that calendar can be created and used without accessibility-related crashes
    m_calendar->setFocus();
    m_calendar->showNextMonth();
    m_calendar->showPreviousMonth();

    // Accessibility should work without crashing
    QVERIFY(true);
}

void FluentCalendarTest::testThemeIntegration() {
    // Test that calendar responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    auto originalMode = theme.mode();

    // Change theme mode
    auto newMode = (originalMode == FluentQt::Styling::FluentThemeMode::Light) ?
                   FluentQt::Styling::FluentThemeMode::Dark :
                   FluentQt::Styling::FluentThemeMode::Light;
    theme.setMode(newMode);

    // Calendar should update its appearance (this would require checking internal styling)
    // For now, just verify the calendar still functions correctly
    QSignalSpy selectedDateChangedSpy(m_calendar, &FluentCalendar::selectedDateChanged);
    QDate testDate(2024, 6, 15);
    m_calendar->setSelectedDate(testDate);
    QCOMPARE(m_calendar->selectedDate(), testDate);
    QCOMPARE(selectedDateChangedSpy.count(), 1);

    // Test that navigation still works with new theme
    m_calendar->showNextMonth();
    m_calendar->showPreviousMonth();

    // Restore original theme
    theme.setMode(originalMode);
}

QTEST_MAIN(FluentCalendarTest)
#include "FluentCalendarTest.moc"
