# FluentCalendar Component

The `FluentCalendar` component is an enhanced calendar widget that implements Microsoft's Fluent Design principles. It provides date selection, event display, and comprehensive customization options with smooth animations.

## Overview

FluentCalendar offers:
- **Multiple view modes** (Month, Week, Day, Year)
- **Date range selection** and multiple date selection
- **Event display** with custom styling
- **Smooth animations** for navigation and interactions
- **Localization support** with different calendar systems
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Calendar

```cpp
#include "FluentQt/Components/FluentCalendar.h"

// Create a basic calendar
auto* calendar = new FluentCalendar;

// Handle date selection
connect(calendar, &FluentCalendar::dateSelected, [=](const QDate& date) {
    qDebug() << "Selected date:" << date.toString();
});

// Add to layout
layout->addWidget(calendar);
```

### Calendar with Initial Date

```cpp
auto* calendar = new FluentCalendar;

// Set initial date
calendar->setSelectedDate(QDate::currentDate());

// Set date range limits
calendar->setMinimumDate(QDate(2020, 1, 1));
calendar->setMaximumDate(QDate(2030, 12, 31));

// Navigate to specific month
calendar->setCurrentMonth(2024, 3);  // March 2024
```

## View Modes

### Month View

```cpp
auto* calendar = new FluentCalendar;
calendar->setViewMode(FluentCalendarViewMode::Month);

// Configure month view
calendar->setShowWeekNumbers(true);
calendar->setFirstDayOfWeek(Qt::Monday);
calendar->setWeekendHighlight(true);

// Month navigation
calendar->navigateToNextMonth();
calendar->navigateToPreviousMonth();
calendar->navigateToMonth(2024, 6);  // June 2024
```

### Week View

```cpp
auto* calendar = new FluentCalendar;
calendar->setViewMode(FluentCalendarViewMode::Week);

// Configure week view
calendar->setShowTimeSlots(true);
calendar->setTimeSlotHeight(30);
calendar->setWorkingHours(9, 17);  // 9 AM to 5 PM

// Week navigation
calendar->navigateToNextWeek();
calendar->navigateToPreviousWeek();
calendar->navigateToWeek(QDate(2024, 3, 15));
```

### Day View

```cpp
auto* calendar = new FluentCalendar;
calendar->setViewMode(FluentCalendarViewMode::Day);

// Configure day view
calendar->setHourFormat(FluentCalendarHourFormat::Hour24);
calendar->setTimeSlotDuration(30);  // 30-minute slots
calendar->setShowAllDayEvents(true);

// Day navigation
calendar->navigateToNextDay();
calendar->navigateToPreviousDay();
calendar->navigateToDate(QDate(2024, 3, 15));
```

### Year View

```cpp
auto* calendar = new FluentCalendar;
calendar->setViewMode(FluentCalendarViewMode::Year);

// Configure year view
calendar->setMonthDisplayMode(FluentMonthDisplayMode::Compact);
calendar->setShowMonthNames(true);

// Year navigation
calendar->navigateToNextYear();
calendar->navigateToPreviousYear();
calendar->navigateToYear(2024);
```

## Date Selection

### Single Date Selection

```cpp
auto* calendar = new FluentCalendar;
calendar->setSelectionMode(FluentCalendarSelectionMode::Single);

// Get selected date
QDate selectedDate = calendar->selectedDate();

// Set selected date programmatically
calendar->setSelectedDate(QDate(2024, 3, 15));

// Handle selection changes
connect(calendar, &FluentCalendar::dateSelected, [=](const QDate& date) {
    qDebug() << "Selected:" << date.toString("yyyy-MM-dd");
});
```

### Multiple Date Selection

```cpp
auto* calendar = new FluentCalendar;
calendar->setSelectionMode(FluentCalendarSelectionMode::Multiple);

// Get selected dates
QList<QDate> selectedDates = calendar->selectedDates();

// Set multiple selected dates
QList<QDate> dates = {
    QDate(2024, 3, 15),
    QDate(2024, 3, 20),
    QDate(2024, 3, 25)
};
calendar->setSelectedDates(dates);

// Handle multiple selection
connect(calendar, &FluentCalendar::datesSelected, [=](const QList<QDate>& dates) {
    qDebug() << "Selected" << dates.size() << "dates";
});
```

### Date Range Selection

```cpp
auto* calendar = new FluentCalendar;
calendar->setSelectionMode(FluentCalendarSelectionMode::Range);

// Get selected range
QPair<QDate, QDate> range = calendar->selectedDateRange();
QDate startDate = range.first;
QDate endDate = range.second;

// Set date range
calendar->setSelectedDateRange(QDate(2024, 3, 10), QDate(2024, 3, 20));

// Handle range selection
connect(calendar, &FluentCalendar::dateRangeSelected, [=](const QDate& start, const QDate& end) {
    qDebug() << "Range selected:" << start.toString() << "to" << end.toString();
});
```

## Events and Appointments

### Adding Events

```cpp
auto* calendar = new FluentCalendar;

// Create calendar events
FluentCalendarEvent event1;
event1.title = "Team Meeting";
event1.description = "Weekly team sync meeting";
event1.startDate = QDate(2024, 3, 15);
event1.startTime = QTime(10, 0);
event1.endTime = QTime(11, 0);
event1.color = QColor(0, 120, 215);  // Blue
event1.category = "Work";

FluentCalendarEvent event2;
event2.title = "Doctor Appointment";
event2.description = "Annual checkup";
event2.startDate = QDate(2024, 3, 18);
event2.startTime = QTime(14, 30);
event2.endTime = QTime(15, 30);
event2.color = QColor(16, 124, 16);  // Green
event2.category = "Personal";

// Add events to calendar
calendar->addEvent(event1);
calendar->addEvent(event2);

// Add recurring event
FluentCalendarEvent recurringEvent;
recurringEvent.title = "Daily Standup";
recurringEvent.startTime = QTime(9, 0);
recurringEvent.endTime = QTime(9, 15);
recurringEvent.color = QColor(255, 140, 0);  // Orange
recurringEvent.recurrence = FluentCalendarRecurrence::Daily;
recurringEvent.recurrenceEnd = QDate(2024, 12, 31);

calendar->addRecurringEvent(recurringEvent, QDate(2024, 3, 1));
```

### Event Interaction

```cpp
auto* calendar = new FluentCalendar;

// Handle event clicks
connect(calendar, &FluentCalendar::eventClicked, [=](const FluentCalendarEvent& event) {
    qDebug() << "Event clicked:" << event.title;
    // Show event details dialog
    showEventDetails(event);
});

// Handle event double-clicks
connect(calendar, &FluentCalendar::eventDoubleClicked, [=](const FluentCalendarEvent& event) {
    // Edit event
    editEvent(event);
});

// Handle event hover
connect(calendar, &FluentCalendar::eventHovered, [=](const FluentCalendarEvent& event, bool isHovered) {
    if (isHovered) {
        // Show event tooltip
        showEventTooltip(event);
    }
});

// Handle date clicks (for creating new events)
connect(calendar, &FluentCalendar::dateClicked, [=](const QDate& date, const QTime& time) {
    // Create new event at clicked date/time
    createNewEvent(date, time);
});
```

### Event Categories and Filtering

```cpp
auto* calendar = new FluentCalendar;

// Define event categories
QStringList categories = {"Work", "Personal", "Health", "Travel", "Education"};
calendar->setEventCategories(categories);

// Set category colors
calendar->setCategoryColor("Work", QColor(0, 120, 215));      // Blue
calendar->setCategoryColor("Personal", QColor(16, 124, 16));   // Green
calendar->setCategoryColor("Health", QColor(196, 43, 28));     // Red
calendar->setCategoryColor("Travel", QColor(255, 140, 0));     // Orange
calendar->setCategoryColor("Education", QColor(102, 45, 145)); // Purple

// Filter events by category
calendar->setVisibleCategories({"Work", "Personal"});

// Toggle category visibility
calendar->setCategoryVisible("Health", false);
```

## Customization

### Appearance Customization

```cpp
auto* calendar = new FluentCalendar;

// Customize colors
calendar->setTodayColor(QColor(0, 120, 215));
calendar->setSelectedDateColor(QColor(0, 120, 215, 100));
calendar->setWeekendColor(QColor(255, 240, 240));
calendar->setHolidayColor(QColor(255, 200, 200));

// Customize fonts
calendar->setHeaderFont(QFont("Segoe UI", 12, QFont::Bold));
calendar->setDateFont(QFont("Segoe UI", 10));
calendar->setEventFont(QFont("Segoe UI", 8));

// Customize spacing
calendar->setDateSpacing(2);
calendar->setEventSpacing(1);
calendar->setMargins(10, 10, 10, 10);
```

### Custom Date Rendering

```cpp
class CustomCalendarDelegate : public FluentCalendarDelegate
{
public:
    void paintDate(QPainter* painter, const QRect& rect, const QDate& date, 
                   const FluentCalendarDateInfo& info) override {
        // Custom date painting
        painter->save();
        
        // Draw background
        if (info.isSelected) {
            painter->fillRect(rect, QColor(0, 120, 215, 100));
        } else if (info.isToday) {
            painter->fillRect(rect, QColor(0, 120, 215, 50));
        }
        
        // Draw date number
        painter->setPen(info.isWeekend ? QColor(150, 150, 150) : QColor(50, 50, 50));
        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        
        // Draw event indicators
        if (info.hasEvents) {
            QRect indicatorRect(rect.right() - 8, rect.top() + 2, 6, 6);
            painter->fillRect(indicatorRect, QColor(255, 0, 0));
        }
        
        painter->restore();
    }
};

auto* calendar = new FluentCalendar;
calendar->setDelegate(new CustomCalendarDelegate);
```

### Holidays and Special Dates

```cpp
auto* calendar = new FluentCalendar;

// Add holidays
calendar->addHoliday(QDate(2024, 1, 1), "New Year's Day");
calendar->addHoliday(QDate(2024, 7, 4), "Independence Day");
calendar->addHoliday(QDate(2024, 12, 25), "Christmas Day");

// Add special dates
calendar->addSpecialDate(QDate(2024, 3, 15), "Project Deadline", QColor(255, 0, 0));
calendar->addSpecialDate(QDate(2024, 6, 1), "Vacation Start", QColor(0, 255, 0));

// Handle holiday clicks
connect(calendar, &FluentCalendar::holidayClicked, [=](const QDate& date, const QString& name) {
    qDebug() << "Holiday clicked:" << name << "on" << date.toString();
});
```

## Animations

FluentCalendar includes smooth animations for navigation and interactions:

```cpp
auto* calendar = new FluentCalendar;

// Enable/disable animations
calendar->setAnimated(true);   // Default: enabled

// Configure animation settings
calendar->setNavigationAnimationDuration(300);
calendar->setSelectionAnimationDuration(200);
calendar->setEventAnimationDuration(150);

// Custom animations
connect(calendar, &FluentCalendar::monthChanging, [=](int oldMonth, int newMonth) {
    auto* animator = new FluentAnimator(this);
    if (newMonth > oldMonth) {
        animator->slideInFromRight(calendar->monthWidget(), 250);
    } else {
        animator->slideInFromLeft(calendar->monthWidget(), 250);
    }
});
```

## Localization

### Different Locales

```cpp
auto* calendar = new FluentCalendar;

// Set locale
calendar->setLocale(QLocale(QLocale::French, QLocale::France));
calendar->setLocale(QLocale(QLocale::German, QLocale::Germany));
calendar->setLocale(QLocale(QLocale::Japanese, QLocale::Japan));

// Custom date format
calendar->setDateFormat("dd/MM/yyyy");  // European format
calendar->setDateFormat("yyyy-MM-dd");  // ISO format
calendar->setDateFormat("MMM d, yyyy"); // US format

// Custom month/day names
QStringList customMonthNames = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
calendar->setMonthNames(customMonthNames);

QStringList customDayNames = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
calendar->setDayNames(customDayNames);
```

### Different Calendar Systems

```cpp
auto* calendar = new FluentCalendar;

// Islamic calendar
calendar->setCalendarSystem(FluentCalendarSystem::Islamic);

// Hebrew calendar
calendar->setCalendarSystem(FluentCalendarSystem::Hebrew);

// Chinese calendar
calendar->setCalendarSystem(FluentCalendarSystem::Chinese);

// Custom calendar conversion
connect(calendar, &FluentCalendar::dateConversionRequested, [=](const QDate& gregorianDate) {
    // Convert to custom calendar system
    QString customDate = convertToCustomCalendar(gregorianDate);
    calendar->setCustomDateText(gregorianDate, customDate);
});
```

## Integration Examples

### Event Management Application

```cpp
class EventManagerWidget : public QWidget
{
public:
    EventManagerWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
        loadEvents();
    }

private:
    void setupUI() {
        auto* layout = new QHBoxLayout(this);
        
        // Calendar
        m_calendar = new FluentCalendar;
        m_calendar->setViewMode(FluentCalendarViewMode::Month);
        m_calendar->setSelectionMode(FluentCalendarSelectionMode::Single);
        
        // Event list
        m_eventList = new QListWidget;
        m_eventList->setMaximumWidth(300);
        
        // Event details
        m_eventDetails = new FluentPanel("Event Details");
        m_eventDetails->setMaximumWidth(250);
        
        layout->addWidget(m_calendar, 2);
        layout->addWidget(m_eventList, 1);
        layout->addWidget(m_eventDetails, 1);
    }
    
    void setupConnections() {
        // Update event list when date changes
        connect(m_calendar, &FluentCalendar::dateSelected, [=](const QDate& date) {
            updateEventList(date);
        });
        
        // Show event details when event is clicked
        connect(m_calendar, &FluentCalendar::eventClicked, [=](const FluentCalendarEvent& event) {
            showEventDetails(event);
        });
        
        // Create new event on double-click
        connect(m_calendar, &FluentCalendar::dateDoubleClicked, [=](const QDate& date) {
            createNewEvent(date);
        });
    }
    
    void loadEvents() {
        // Load events from database or file
        QList<FluentCalendarEvent> events = m_eventManager->loadEvents();
        for (const auto& event : events) {
            m_calendar->addEvent(event);
        }
    }
    
    void updateEventList(const QDate& date) {
        m_eventList->clear();
        QList<FluentCalendarEvent> dayEvents = m_calendar->eventsForDate(date);
        
        for (const auto& event : dayEvents) {
            auto* item = new QListWidgetItem(event.title);
            item->setData(Qt::UserRole, QVariant::fromValue(event));
            m_eventList->addItem(item);
        }
    }
    
    void showEventDetails(const FluentCalendarEvent& event) {
        // Update event details panel
        auto* layout = new QVBoxLayout;
        
        auto* titleLabel = new QLabel(event.title);
        titleLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
        
        auto* timeLabel = new QLabel(QString("%1 - %2")
            .arg(event.startTime.toString("hh:mm"))
            .arg(event.endTime.toString("hh:mm")));
        
        auto* descLabel = new QLabel(event.description);
        descLabel->setWordWrap(true);
        
        layout->addWidget(titleLabel);
        layout->addWidget(timeLabel);
        layout->addWidget(descLabel);
        
        auto* contentWidget = new QWidget;
        contentWidget->setLayout(layout);
        m_eventDetails->setContentWidget(contentWidget);
    }

private:
    FluentCalendar* m_calendar{nullptr};
    QListWidget* m_eventList{nullptr};
    FluentPanel* m_eventDetails{nullptr};
    EventManager* m_eventManager{new EventManager(this)};
};
```

## Accessibility

FluentCalendar provides comprehensive accessibility support:

```cpp
auto* calendar = new FluentCalendar;

// Accessible properties (automatically set)
calendar->setAccessibleName("Date picker calendar");
calendar->setAccessibleDescription("Select a date from the calendar");

// Keyboard navigation (automatic)
// - Arrow keys to navigate dates
// - Page Up/Down for month navigation
// - Home/End for week navigation
// - Enter/Space to select date
// - Tab to navigate between calendar elements

// Screen reader support
calendar->setAccessibleRole(QAccessible::Table);

// Custom accessibility for events
connect(calendar, &FluentCalendar::eventFocused, [=](const FluentCalendarEvent& event) {
    QString announcement = QString("Event: %1 at %2")
        .arg(event.title)
        .arg(event.startTime.toString("hh:mm"));
    calendar->announceToScreenReader(announcement);
});
```

## Best Practices

### Calendar Design Guidelines
1. **Provide clear navigation** - Make it easy to move between months/years
2. **Use consistent event colors** - Establish a color coding system
3. **Show today clearly** - Highlight the current date
4. **Support keyboard navigation** - Ensure accessibility
5. **Provide event details** - Show tooltips or details panels

### Performance Considerations
- Lazy load events for large date ranges
- Use virtual scrolling for year view
- Cache rendered calendar cells
- Optimize event rendering for dense calendars

### Accessibility Guidelines
- Support keyboard navigation
- Provide screen reader announcements
- Use sufficient color contrast
- Include text alternatives for visual elements
- Test with assistive technologies

## Signals Reference

```cpp
// Date selection signals
void dateSelected(const QDate& date);
void datesSelected(const QList<QDate>& dates);
void dateRangeSelected(const QDate& start, const QDate& end);

// Event signals
void eventClicked(const FluentCalendarEvent& event);
void eventDoubleClicked(const FluentCalendarEvent& event);
void eventHovered(const FluentCalendarEvent& event, bool isHovered);

// Navigation signals
void monthChanged(int year, int month);
void yearChanged(int year);
void viewModeChanged(FluentCalendarViewMode mode);
```

## Standalone Examples Collection

### Example 1: Event Management Calendar

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTimeEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QListWidget>
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCard.h"

struct CalendarEvent {
    QString title;
    QString description;
    QDate date;
    QTime startTime;
    QTime endTime;
    QString category;
    QColor color;
    int id;
};

class EventManagementCalendar : public QWidget {
    Q_OBJECT
public:
    EventManagementCalendar(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        loadSampleEvents();
        connectSignals();
    }

private slots:
    void onDateSelected(const QDate& date) {
        m_selectedDate = date;
        updateEventsList();
        updateDateInfo();
    }

    void showAddEventDialog() {
        auto* dialog = new QDialog(this);
        dialog->setWindowTitle("Add New Event");
        dialog->setModal(true);
        dialog->resize(400, 300);

        auto* layout = new QVBoxLayout(dialog);

        // Event title
        auto* titleInput = new FluentTextInput();
        titleInput->setLabelText("Event Title");
        titleInput->setPlaceholderText("Enter event title");

        // Event description
        auto* descInput = new QTextEdit;
        descInput->setPlaceholderText("Event description (optional)");
        descInput->setMaximumHeight(80);

        // Date and time
        auto* dateTimeLayout = new QHBoxLayout;

        auto* startTimeEdit = new QTimeEdit(QTime(9, 0));
        auto* endTimeEdit = new QTimeEdit(QTime(10, 0));

        dateTimeLayout->addWidget(new QLabel("From:"));
        dateTimeLayout->addWidget(startTimeEdit);
        dateTimeLayout->addWidget(new QLabel("To:"));
        dateTimeLayout->addWidget(endTimeEdit);

        // Category
        auto* categoryCombo = new QComboBox;
        categoryCombo->addItems({"Work", "Personal", "Meeting", "Appointment", "Other"});

        // Buttons
        auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        layout->addWidget(titleInput);
        layout->addWidget(new QLabel("Description:"));
        layout->addWidget(descInput);
        layout->addWidget(new QLabel("Time:"));
        layout->addLayout(dateTimeLayout);
        layout->addWidget(new QLabel("Category:"));
        layout->addWidget(categoryCombo);
        layout->addWidget(buttonBox);

        connect(buttonBox, &QDialogButtonBox::accepted, [=]() {
            if (!titleInput->text().isEmpty()) {
                CalendarEvent event;
                event.id = m_nextEventId++;
                event.title = titleInput->text();
                event.description = descInput->toPlainText();
                event.date = m_selectedDate;
                event.startTime = startTimeEdit->time();
                event.endTime = endTimeEdit->time();
                event.category = categoryCombo->currentText();
                event.color = getCategoryColor(event.category);

                m_events.append(event);
                updateCalendarEvents();
                updateEventsList();

                dialog->accept();
            }
        });

        connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

        dialog->exec();
        dialog->deleteLater();
    }

    void editEvent(int eventId) {
        auto it = std::find_if(m_events.begin(), m_events.end(),
                              [eventId](const CalendarEvent& e) { return e.id == eventId; });

        if (it != m_events.end()) {
            // Similar dialog to add event, but pre-filled with existing data
            showEditEventDialog(*it);
        }
    }

    void deleteEvent(int eventId) {
        m_events.removeIf([eventId](const CalendarEvent& e) { return e.id == eventId; });
        updateCalendarEvents();
        updateEventsList();
    }

    void updateEventsList() {
        m_eventsList->clear();

        QList<CalendarEvent> dayEvents;
        for (const auto& event : m_events) {
            if (event.date == m_selectedDate) {
                dayEvents.append(event);
            }
        }

        // Sort by start time
        std::sort(dayEvents.begin(), dayEvents.end(),
                 [](const CalendarEvent& a, const CalendarEvent& b) {
                     return a.startTime < b.startTime;
                 });

        if (dayEvents.isEmpty()) {
            auto* item = new QListWidgetItem("No events for this date");
            item->setFlags(Qt::NoItemFlags);
            m_eventsList->addItem(item);
        } else {
            for (const auto& event : dayEvents) {
                auto* widget = createEventListItem(event);
                auto* item = new QListWidgetItem;
                item->setSizeHint(widget->sizeHint());
                m_eventsList->addItem(item);
                m_eventsList->setItemWidget(item, widget);
            }
        }
    }

    void updateDateInfo() {
        QString dateText = m_selectedDate.toString("dddd, MMMM d, yyyy");
        m_dateLabel->setText(dateText);

        int eventCount = 0;
        for (const auto& event : m_events) {
            if (event.date == m_selectedDate) {
                eventCount++;
            }
        }

        m_eventCountLabel->setText(QString("%1 event(s)").arg(eventCount));
    }

private:
    void setupUI() {
        auto* layout = new QHBoxLayout(this);

        // Left side - Calendar
        auto* leftPanel = new FluentCard("Calendar");
        leftPanel->setMinimumWidth(400);

        auto* leftLayout = new QVBoxLayout;

        m_calendar = new FluentCalendar;
        m_calendar->setSelectedDate(QDate::currentDate());

        auto* calendarControls = new QHBoxLayout;

        auto* todayButton = new FluentButton("Today");
        todayButton->setButtonStyle(FluentButtonStyle::Subtle);
        connect(todayButton, &FluentButton::clicked, [this]() {
            m_calendar->setSelectedDate(QDate::currentDate());
        });

        auto* prevButton = new FluentButton("◀ Previous");
        prevButton->setButtonStyle(FluentButtonStyle::Subtle);
        connect(prevButton, &FluentButton::clicked, [this]() {
            m_calendar->navigateToPreviousMonth();
        });

        auto* nextButton = new FluentButton("Next ▶");
        nextButton->setButtonStyle(FluentButtonStyle::Subtle);
        connect(nextButton, &FluentButton::clicked, [this]() {
            m_calendar->navigateToNextMonth();
        });

        calendarControls->addWidget(todayButton);
        calendarControls->addStretch();
        calendarControls->addWidget(prevButton);
        calendarControls->addWidget(nextButton);

        leftLayout->addWidget(m_calendar);
        leftLayout->addLayout(calendarControls);

        leftPanel->setContentWidget(new QWidget);
        leftPanel->contentWidget()->setLayout(leftLayout);

        // Right side - Events
        auto* rightPanel = new FluentCard("Events");
        rightPanel->setMinimumWidth(300);

        auto* rightLayout = new QVBoxLayout;

        // Date info
        m_dateLabel = new QLabel;
        m_dateLabel->setStyleSheet("font-size: 18px; font-weight: bold;");

        m_eventCountLabel = new QLabel;
        m_eventCountLabel->setStyleSheet("color: gray;");

        // Add event button
        auto* addEventButton = new FluentButton("+ Add Event");
        addEventButton->setButtonStyle(FluentButtonStyle::Primary);

        // Events list
        m_eventsList = new QListWidget;
        m_eventsList->setAlternatingRowColors(true);

        rightLayout->addWidget(m_dateLabel);
        rightLayout->addWidget(m_eventCountLabel);
        rightLayout->addWidget(addEventButton);
        rightLayout->addWidget(m_eventsList);

        rightPanel->setContentWidget(new QWidget);
        rightPanel->contentWidget()->setLayout(rightLayout);

        layout->addWidget(leftPanel);
        layout->addWidget(rightPanel);

        // Initialize with current date
        m_selectedDate = QDate::currentDate();
        updateDateInfo();

        connect(addEventButton, &FluentButton::clicked, this, &EventManagementCalendar::showAddEventDialog);
    }

    void loadSampleEvents() {
        m_nextEventId = 1;

        // Add some sample events
        QDate today = QDate::currentDate();

        m_events = {
            {1, "Team Meeting", "Weekly team sync", today, QTime(9, 0), QTime(10, 0), "Work", QColor("#0078d4")},
            {2, "Lunch with Client", "Discuss project requirements", today.addDays(1), QTime(12, 0), QTime(13, 30), "Meeting", QColor("#107c10")},
            {3, "Doctor Appointment", "Annual checkup", today.addDays(2), QTime(14, 0), QTime(15, 0), "Personal", QColor("#d83b01")},
            {4, "Project Deadline", "Submit final deliverables", today.addDays(5), QTime(17, 0), QTime(17, 0), "Work", QColor("#0078d4")}
        };

        m_nextEventId = 5;
        updateCalendarEvents();
    }

    void updateCalendarEvents() {
        // Clear existing events
        m_calendar->clearEvents();

        // Add events to calendar
        for (const auto& event : m_events) {
            FluentCalendarEvent calEvent;
            calEvent.title = event.title;
            calEvent.date = event.date;
            calEvent.color = event.color;
            calEvent.isAllDay = (event.startTime == event.endTime);

            m_calendar->addEvent(calEvent);
        }
    }

    QWidget* createEventListItem(const CalendarEvent& event) {
        auto* widget = new QWidget;
        auto* layout = new QHBoxLayout(widget);

        // Color indicator
        auto* colorLabel = new QLabel;
        colorLabel->setFixedSize(4, 40);
        colorLabel->setStyleSheet(QString("background-color: %1; border-radius: 2px;").arg(event.color.name()));

        // Event info
        auto* infoLayout = new QVBoxLayout;

        auto* titleLabel = new QLabel(event.title);
        titleLabel->setStyleSheet("font-weight: bold;");

        auto* timeLabel = new QLabel(QString("%1 - %2")
                                    .arg(event.startTime.toString("h:mm AP"))
                                    .arg(event.endTime.toString("h:mm AP")));
        timeLabel->setStyleSheet("color: gray; font-size: 12px;");

        auto* categoryLabel = new QLabel(event.category);
        categoryLabel->setStyleSheet("color: gray; font-size: 11px;");

        infoLayout->addWidget(titleLabel);
        infoLayout->addWidget(timeLabel);
        infoLayout->addWidget(categoryLabel);
        infoLayout->setSpacing(2);

        // Actions
        auto* actionsLayout = new QVBoxLayout;

        auto* editButton = new FluentButton("Edit");
        editButton->setButtonStyle(FluentButtonStyle::Subtle);
        editButton->setButtonSize(FluentButtonSize::Small);
        connect(editButton, &FluentButton::clicked, [this, event]() {
            editEvent(event.id);
        });

        auto* deleteButton = new FluentButton("Delete");
        deleteButton->setButtonStyle(FluentButtonStyle::Subtle);
        deleteButton->setButtonSize(FluentButtonSize::Small);
        connect(deleteButton, &FluentButton::clicked, [this, event]() {
            deleteEvent(event.id);
        });

        actionsLayout->addWidget(editButton);
        actionsLayout->addWidget(deleteButton);

        layout->addWidget(colorLabel);
        layout->addLayout(infoLayout);
        layout->addStretch();
        layout->addLayout(actionsLayout);

        return widget;
    }

    void showEditEventDialog(const CalendarEvent& event) {
        // Implementation similar to showAddEventDialog but pre-filled
        // For brevity, showing simplified version
        qDebug() << "Edit event:" << event.title;
    }

    QColor getCategoryColor(const QString& category) {
        static QMap<QString, QColor> categoryColors = {
            {"Work", QColor("#0078d4")},
            {"Personal", QColor("#d83b01")},
            {"Meeting", QColor("#107c10")},
            {"Appointment", QColor("#5c2d91")},
            {"Other", QColor("#767676")}
        };

        return categoryColors.value(category, QColor("#767676"));
    }

    void connectSignals() {
        connect(m_calendar, &FluentCalendar::dateSelected, this, &EventManagementCalendar::onDateSelected);
    }

    FluentCalendar* m_calendar;
    QListWidget* m_eventsList;
    QLabel* m_dateLabel;
    QLabel* m_eventCountLabel;
    QDate m_selectedDate;
    QList<CalendarEvent> m_events;
    int m_nextEventId;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    EventManagementCalendar calendar;
    calendar.resize(900, 600);
    calendar.show();

    return app.exec();
}

#include "event_calendar.moc"
```

## See Also

- [FluentTimePicker](FluentTimePicker.md) - For time selection
- [FluentComboBox](FluentComboBox.md) - For dropdown date selection
- [FluentPanel](FluentPanel.md) - For organizing calendar layouts
- [API Reference - FluentCalendar](../API_REFERENCE.md#fluentcalendar) - Complete API documentation
