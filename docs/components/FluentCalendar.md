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

## See Also

- [FluentTimePicker](FluentTimePicker.md) - For time selection
- [FluentComboBox](FluentComboBox.md) - For dropdown date selection
- [FluentPanel](FluentPanel.md) - For organizing calendar layouts
- [API Reference - FluentCalendar](../API_REFERENCE.md#fluentcalendar) - Complete API documentation
