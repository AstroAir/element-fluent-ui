// tests/Components/FluentDatePickerTest.cpp
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QtTest/QtTest>
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentDatePicker.h"

using namespace FluentQt::Components;

class FluentDatePickerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testConstructor();
    void testDateProperty();
    void testPlaceholderTextProperty();
    void testDateFormatProperty();
    void testCalendarPopupProperty();

    // Calendar integration tests
    void testCalendarAccess();
    void testCalendarPopupShow();
    void testCalendarPopupHide();
    void testDateSelection();

    // Visual tests
    void testPaintEvent();
    void testSizeHints();
    void testFocusStates();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();
    void testFocusHandling();

    // Edge cases
    void testInvalidDates();
    void testDateRanges();
    void testLocalization();

private:
    FluentDatePicker* m_datePicker{nullptr};
};

void FluentDatePickerTest::initTestCase() {
    // Initialize test environment
}

void FluentDatePickerTest::cleanupTestCase() {
    // Clean up test environment
}

void FluentDatePickerTest::init() {
    // Create fresh date picker for each test
    m_datePicker = new FluentDatePicker();
}

void FluentDatePickerTest::cleanup() {
    // Clean up after each test
    if (m_datePicker) {
        m_datePicker->deleteLater();
        m_datePicker = nullptr;
    }
}

// Basic functionality tests
void FluentDatePickerTest::testConstructor() {
    // Test default constructor
    QVERIFY(m_datePicker != nullptr);
    QCOMPARE(m_datePicker->selectedDate(), QDate::currentDate());
    QCOMPARE(m_datePicker->placeholderText(), QString("Select date"));
    QCOMPARE(m_datePicker->dateFormat(), FluentDateFormat::Short);
    QVERIFY(m_datePicker->isCalendarVisible() == false);  // Initially hidden
}

void FluentDatePickerTest::testDateProperty() {
    QSignalSpy spy(m_datePicker, &FluentDatePicker::selectedDateChanged);

    // Test setting valid date
    QDate testDate(2023, 12, 25);
    m_datePicker->setSelectedDate(testDate);
    QCOMPARE(m_datePicker->selectedDate(), testDate);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().first().toDate(), testDate);

    // Test setting same date (should not emit signal)
    m_datePicker->setSelectedDate(testDate);
    QCOMPARE(spy.count(), 1);

    // Test setting different date
    QDate newDate(2024, 1, 1);
    m_datePicker->setSelectedDate(newDate);
    QCOMPARE(m_datePicker->selectedDate(), newDate);
    QCOMPARE(spy.count(), 2);

    // Test setting invalid date (should not change)
    QDate currentDate = m_datePicker->selectedDate();
    m_datePicker->setSelectedDate(QDate());
    QCOMPARE(m_datePicker->selectedDate(), currentDate);
    QCOMPARE(spy.count(), 2);
}

void FluentDatePickerTest::testPlaceholderTextProperty() {
    QString testPlaceholder = "Choose a date...";
    m_datePicker->setPlaceholderText(testPlaceholder);
    QCOMPARE(m_datePicker->placeholderText(), testPlaceholder);

    // Test setting same placeholder (should not cause issues)
    m_datePicker->setPlaceholderText(testPlaceholder);
    QCOMPARE(m_datePicker->placeholderText(), testPlaceholder);
}

void FluentDatePickerTest::testDateFormatProperty() {
    FluentDateFormat testFormat = FluentDateFormat::Medium;
    m_datePicker->setDateFormat(testFormat);
    QCOMPARE(m_datePicker->dateFormat(), testFormat);

    // Test setting same format (should not cause issues)
    m_datePicker->setDateFormat(testFormat);
    QCOMPARE(m_datePicker->dateFormat(), testFormat);
}

void FluentDatePickerTest::testCalendarPopupProperty() {
    // Test disabling calendar popup
    m_datePicker->setCalendarVisible(false);
    QVERIFY(!m_datePicker->isCalendarVisible());

    // Test enabling calendar popup
    m_datePicker->setCalendarVisible(true);
    QVERIFY(m_datePicker->isCalendarVisible());
}

// Calendar integration tests
void FluentDatePickerTest::testCalendarAccess() {
    // Note: FluentDatePicker doesn't expose calendar() method publicly
    // Testing calendar visibility instead
    m_datePicker->setCalendarVisible(true);
    QVERIFY(m_datePicker->isCalendarVisible());

    // Test that date picker maintains its selected date
    QDate testDate(2023, 6, 15);
    m_datePicker->setSelectedDate(testDate);
    QCOMPARE(m_datePicker->selectedDate(), testDate);
}

void FluentDatePickerTest::testCalendarPopupShow() {
    QSignalSpy visibilitySpy(m_datePicker,
                             &FluentDatePicker::calendarVisibilityChanged);

    // Show the date picker first
    m_datePicker->show();

    // Test programmatic calendar show
    m_datePicker->showCalendar();

    // Process events
    QApplication::processEvents();

    // Verify calendar is visible
    QVERIFY(m_datePicker->isCalendarVisible());
    QVERIFY(visibilitySpy.count() >= 1);
}

void FluentDatePickerTest::testCalendarPopupHide() {
    QSignalSpy visibilitySpy(m_datePicker,
                             &FluentDatePicker::calendarVisibilityChanged);

    m_datePicker->show();

    // First show the calendar
    m_datePicker->showCalendar();
    QVERIFY(m_datePicker->isCalendarVisible());

    QApplication::processEvents();

    // Then hide it
    m_datePicker->hideCalendar();

    QApplication::processEvents();

    // Verify calendar is hidden
    QVERIFY(!m_datePicker->isCalendarVisible());
    QVERIFY(visibilitySpy.count() >= 2);  // Show + Hide
}

void FluentDatePickerTest::testDateSelection() {
    QSignalSpy dateChangedSpy(m_datePicker,
                              &FluentDatePicker::selectedDateChanged);

    // Test date selection through API
    QDate testDate(2023, 6, 15);
    m_datePicker->setSelectedDate(testDate);

    QCOMPARE(m_datePicker->selectedDate(), testDate);
    QVERIFY(dateChangedSpy.count() >= 1);
}

// Visual tests
void FluentDatePickerTest::testPaintEvent() {
    m_datePicker->setSelectedDate(QDate(2023, 12, 25));
    m_datePicker->show();

    // Force a paint event
    m_datePicker->update();
    QApplication::processEvents();

    // If we get here without crashing, painting works
    QVERIFY(true);
}

void FluentDatePickerTest::testSizeHints() {
    QSize sizeHint = m_datePicker->sizeHint();
    QSize minSizeHint = m_datePicker->minimumSizeHint();

    QVERIFY(sizeHint.width() > 0);
    QVERIFY(sizeHint.height() > 0);
    QVERIFY(minSizeHint.width() > 0);
    QVERIFY(minSizeHint.height() > 0);
    QVERIFY(sizeHint.width() >= minSizeHint.width());
    QVERIFY(sizeHint.height() >= minSizeHint.height());
}

void FluentDatePickerTest::testFocusStates() {
    m_datePicker->show();

    // Test focus in
    m_datePicker->setFocus();
    QApplication::processEvents();
    QVERIFY(m_datePicker->hasFocus());

    // Test focus out
    m_datePicker->clearFocus();
    QApplication::processEvents();
    QVERIFY(!m_datePicker->hasFocus());
}

// Interaction tests
void FluentDatePickerTest::testMouseInteraction() {
    m_datePicker->show();

    // Test mouse press/release events
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(10, 10),
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_datePicker, &pressEvent);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(10, 10),
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_datePicker, &releaseEvent);

    // If we get here without crashing, mouse interaction works
    QVERIFY(true);
}

void FluentDatePickerTest::testKeyboardInteraction() {
    m_datePicker->show();
    m_datePicker->setFocus();

    // Test space key
    QKeyEvent spaceEvent(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
    QApplication::sendEvent(m_datePicker, &spaceEvent);

    // Test return key
    QKeyEvent returnEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_datePicker, &returnEvent);

    // Test escape key
    QKeyEvent escapeEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QApplication::sendEvent(m_datePicker, &escapeEvent);

    // If we get here without crashing, keyboard interaction works
    QVERIFY(true);
}

void FluentDatePickerTest::testFocusHandling() {
    m_datePicker->show();

    // Test focus events
    QFocusEvent focusInEvent(QEvent::FocusIn);
    QApplication::sendEvent(m_datePicker, &focusInEvent);

    QFocusEvent focusOutEvent(QEvent::FocusOut);
    QApplication::sendEvent(m_datePicker, &focusOutEvent);

    // If we get here without crashing, focus handling works
    QVERIFY(true);
}

// Edge cases
void FluentDatePickerTest::testInvalidDates() {
    QDate originalDate = m_datePicker->selectedDate();

    // Test setting invalid date
    m_datePicker->setSelectedDate(QDate());
    QCOMPARE(m_datePicker->selectedDate(), originalDate);  // Should not change

    // Test setting date with invalid components
    m_datePicker->setSelectedDate(
        QDate(2023, 13, 32));  // Invalid month and day
    QCOMPARE(m_datePicker->selectedDate(), originalDate);  // Should not change
}

void FluentDatePickerTest::testDateRanges() {
    // Test various date ranges
    QDate minDate(1900, 1, 1);
    QDate maxDate(2100, 12, 31);
    QDate testDate(2023, 6, 15);

    m_datePicker->setSelectedDate(minDate);
    QCOMPARE(m_datePicker->selectedDate(), minDate);

    m_datePicker->setSelectedDate(maxDate);
    QCOMPARE(m_datePicker->selectedDate(), maxDate);

    m_datePicker->setSelectedDate(testDate);
    QCOMPARE(m_datePicker->selectedDate(), testDate);
}

void FluentDatePickerTest::testLocalization() {
    // Test different date formats
    QStringList formats = {"yyyy-MM-dd", "dd/MM/yyyy", "MM/dd/yyyy",
                           "dd.MM.yyyy"};

    QDate testDate(2023, 12, 25);
    m_datePicker->setSelectedDate(testDate);

    // Test with enum formats instead of strings
    QList<FluentDateFormat> enumFormats = {
        FluentDateFormat::ISO, FluentDateFormat::Short,
        FluentDateFormat::Medium, FluentDateFormat::Long};
    for (const FluentDateFormat& format : enumFormats) {
        m_datePicker->setDateFormat(format);
        QCOMPARE(m_datePicker->dateFormat(), format);

        // Force repaint to test format rendering
        m_datePicker->update();
        QApplication::processEvents();
    }
}

QTEST_MAIN(FluentDatePickerTest)
#include "FluentDatePickerTest.moc"
