// examples/DatePickerExample.cpp
#include <QApplication>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class DatePickerExample : public QMainWindow {
    Q_OBJECT

public:
    DatePickerExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupConnections();
        setupTheme();
    }

private slots:
    void onDatePickerDateChanged(const QDate& date);
    void onCalendarDateChanged(const QDate& date);
    void onFormatChanged();
    void onPlaceholderChanged();
    void onCalendarPopupToggled(bool enabled);
    void onCalendarModeChanged();
    void onTodayClicked();
    void onClearClicked();
    void toggleTheme();
    void showDateRange();

private:
    void setupUI();
    void setupConnections();
    void setupTheme();
    void createDatePickerControls();
    void createCalendarControls();
    void createExampleScenarios();
    void updateDateInfo(const QDate& date);

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QVBoxLayout* m_mainLayout{nullptr};

    // Date picker controls
    QGroupBox* m_datePickerGroup{nullptr};
    Components::FluentDatePicker* m_datePicker{nullptr};
    QComboBox* m_formatCombo{nullptr};
    QLineEdit* m_placeholderEdit{nullptr};
    QCheckBox* m_popupCheck{nullptr};
    QLabel* m_selectedDateLabel{nullptr};

    // Calendar controls
    QGroupBox* m_calendarGroup{nullptr};
    Components::FluentCalendar* m_calendar{nullptr};
    QComboBox* m_selectionModeCombo{nullptr};
    QCheckBox* m_weekNumbersCheck{nullptr};
    QCheckBox* m_todayCheck{nullptr};

    // Example scenarios
    QGroupBox* m_examplesGroup{nullptr};
    Components::FluentDatePicker* m_birthdatePicker{nullptr};
    Components::FluentDatePicker* m_startDatePicker{nullptr};
    Components::FluentDatePicker* m_endDatePicker{nullptr};
    QTextEdit* m_infoText{nullptr};

    // Theme controls
    QPushButton* m_themeButton{nullptr};
};

void DatePickerExample::setupUI() {
    setWindowTitle("FluentQt Date Picker Example");
    setMinimumSize(900, 700);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(16);
    m_mainLayout->setContentsMargins(16, 16, 16, 16);

    createDatePickerControls();
    createCalendarControls();
    createExampleScenarios();

    // Theme toggle button
    m_themeButton = new QPushButton("Toggle Dark/Light Theme", this);
    m_mainLayout->addWidget(m_themeButton);

    m_mainLayout->addStretch();
}

void DatePickerExample::createDatePickerControls() {
    m_datePickerGroup = new QGroupBox("Date Picker Controls", this);
    auto* layout = new QGridLayout(m_datePickerGroup);

    // Main date picker
    layout->addWidget(new QLabel("Date Picker:"), 0, 0);
    m_datePicker = new Components::FluentDatePicker(this);
    m_datePicker->setDate(QDate::currentDate());
    layout->addWidget(m_datePicker, 0, 1, 1, 2);

    // Format selection
    layout->addWidget(new QLabel("Date Format:"), 1, 0);
    m_formatCombo = new QComboBox(this);
    m_formatCombo->addItems({"yyyy-MM-dd", "dd/MM/yyyy", "MM/dd/yyyy",
                             "dd.MM.yyyy", "MMM dd, yyyy", "MMMM dd, yyyy",
                             "ddd, MMM dd, yyyy"});
    layout->addWidget(m_formatCombo, 1, 1);

    // Placeholder text
    layout->addWidget(new QLabel("Placeholder:"), 2, 0);
    m_placeholderEdit = new QLineEdit("Select date...", this);
    layout->addWidget(m_placeholderEdit, 2, 1);

    // Calendar popup option
    m_popupCheck = new QCheckBox("Enable Calendar Popup", this);
    m_popupCheck->setChecked(true);
    layout->addWidget(m_popupCheck, 2, 2);

    // Selected date display
    layout->addWidget(new QLabel("Selected Date:"), 3, 0);
    m_selectedDateLabel = new QLabel(m_datePicker->date().toString(), this);
    m_selectedDateLabel->setStyleSheet("font-weight: bold; color: #0078d4;");
    layout->addWidget(m_selectedDateLabel, 3, 1, 1, 2);

    // Control buttons
    auto* buttonLayout = new QHBoxLayout();
    auto* todayBtn = new Components::FluentButton("Today", this);
    auto* clearBtn = new Components::FluentButton("Clear", this);

    todayBtn->setStyle(Components::FluentButtonStyle::Accent);
    clearBtn->setStyle(Components::FluentButtonStyle::Standard);

    buttonLayout->addWidget(todayBtn);
    buttonLayout->addWidget(clearBtn);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout, 4, 0, 1, 3);

    m_mainLayout->addWidget(m_datePickerGroup);

    // Connect buttons
    connect(todayBtn, &QPushButton::clicked, this,
            &DatePickerExample::onTodayClicked);
    connect(clearBtn, &QPushButton::clicked, this,
            &DatePickerExample::onClearClicked);
}

void DatePickerExample::createCalendarControls() {
    m_calendarGroup = new QGroupBox("Calendar Widget", this);
    auto* layout = new QHBoxLayout(m_calendarGroup);

    // Calendar widget
    m_calendar = new Components::FluentCalendar(this);
    m_calendar->setSelectedDate(QDate::currentDate());
    layout->addWidget(m_calendar);

    // Calendar settings
    auto* settingsWidget = new QWidget(this);
    auto* settingsLayout = new QVBoxLayout(settingsWidget);

    // Selection mode
    settingsLayout->addWidget(new QLabel("Selection Mode:"));
    m_selectionModeCombo = new QComboBox(this);
    m_selectionModeCombo->addItems({"Single Selection", "Multi Selection",
                                    "Range Selection", "Week Selection",
                                    "Month Selection"});
    settingsLayout->addWidget(m_selectionModeCombo);

    // Options
    m_weekNumbersCheck = new QCheckBox("Show Week Numbers", this);
    m_todayCheck = new QCheckBox("Show Today Button", this);
    m_todayCheck->setChecked(true);

    settingsLayout->addWidget(m_weekNumbersCheck);
    settingsLayout->addWidget(m_todayCheck);

    // Add some holidays for demonstration
    settingsLayout->addWidget(new QLabel("Special Dates:"));
    auto* holidayBtn = new Components::FluentButton("Add Holidays", this);
    settingsLayout->addWidget(holidayBtn);

    settingsLayout->addStretch();
    settingsWidget->setFixedWidth(200);

    layout->addWidget(settingsWidget);
    m_mainLayout->addWidget(m_calendarGroup);

    // Connect holiday button
    connect(holidayBtn, &QPushButton::clicked, [this]() {
        QDate today = QDate::currentDate();
        m_calendar->addHoliday(today.addDays(7), "Team Meeting");
        m_calendar->addHoliday(today.addDays(14), "Project Deadline");
        m_calendar->addHoliday(today.addDays(21), "Company Event");
        m_calendar->addSpecialDate(today.addDays(3), "Important Task");
    });
}

void DatePickerExample::createExampleScenarios() {
    m_examplesGroup = new QGroupBox("Example Scenarios", this);
    auto* layout = new QGridLayout(m_examplesGroup);

    // Birthdate picker
    layout->addWidget(new QLabel("Birthdate:"), 0, 0);
    m_birthdatePicker = new Components::FluentDatePicker(this);
    m_birthdatePicker->setPlaceholderText("Select your birthdate...");
    m_birthdatePicker->setDateFormat("MMMM dd, yyyy");
    layout->addWidget(m_birthdatePicker, 0, 1);

    // Date range picker
    layout->addWidget(new QLabel("Start Date:"), 1, 0);
    m_startDatePicker = new Components::FluentDatePicker(this);
    m_startDatePicker->setPlaceholderText("Project start date...");
    layout->addWidget(m_startDatePicker, 1, 1);

    layout->addWidget(new QLabel("End Date:"), 2, 0);
    m_endDatePicker = new Components::FluentDatePicker(this);
    m_endDatePicker->setPlaceholderText("Project end date...");
    layout->addWidget(m_endDatePicker, 2, 1);

    // Range calculation button
    auto* rangeBtn = new Components::FluentButton("Calculate Range", this);
    rangeBtn->setStyle(Components::FluentButtonStyle::Accent);
    layout->addWidget(rangeBtn, 3, 0, 1, 2);

    // Information display
    m_infoText = new QTextEdit(this);
    m_infoText->setMaximumHeight(100);
    m_infoText->setPlainText("Select dates to see information...");
    layout->addWidget(m_infoText, 4, 0, 1, 2);

    m_mainLayout->addWidget(m_examplesGroup);

    connect(rangeBtn, &QPushButton::clicked, this,
            &DatePickerExample::showDateRange);
}

void DatePickerExample::setupConnections() {
    // Date picker connections
    connect(m_datePicker, &Components::FluentDatePicker::dateChanged, this,
            &DatePickerExample::onDatePickerDateChanged);
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DatePickerExample::onFormatChanged);
    connect(m_placeholderEdit, &QLineEdit::textChanged, this,
            &DatePickerExample::onPlaceholderChanged);
    connect(m_popupCheck, &QCheckBox::toggled, this,
            &DatePickerExample::onCalendarPopupToggled);

    // Calendar connections
    connect(m_calendar, &Components::FluentCalendar::selectedDateChanged, this,
            &DatePickerExample::onCalendarDateChanged);
    connect(m_selectionModeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &DatePickerExample::onCalendarModeChanged);
    connect(m_weekNumbersCheck, &QCheckBox::toggled, m_calendar,
            &Components::FluentCalendar::setShowWeekNumbers);
    connect(m_todayCheck, &QCheckBox::toggled, m_calendar,
            &Components::FluentCalendar::setShowToday);

    // Example scenario connections
    connect(m_birthdatePicker, &Components::FluentDatePicker::dateChanged,
            [this](const QDate& date) {
                if (date.isValid()) {
                    int age = QDate::currentDate().year() - date.year();
                    m_infoText->append(QString("Age: %1 years").arg(age));
                }
            });

    // Theme connection
    connect(m_themeButton, &QPushButton::clicked, this,
            &DatePickerExample::toggleTheme);
}

void DatePickerExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
}

// Slot implementations
void DatePickerExample::onDatePickerDateChanged(const QDate& date) {
    updateDateInfo(date);
    m_calendar->setSelectedDate(date);
}

void DatePickerExample::onCalendarDateChanged(const QDate& date) {
    m_datePicker->setDate(date);
    updateDateInfo(date);
}

void DatePickerExample::onFormatChanged() {
    QString format = m_formatCombo->currentText();
    m_datePicker->setDateFormat(format);
    updateDateInfo(m_datePicker->date());
}

void DatePickerExample::onPlaceholderChanged() {
    m_datePicker->setPlaceholderText(m_placeholderEdit->text());
}

void DatePickerExample::onCalendarPopupToggled(bool enabled) {
    m_datePicker->setCalendarPopup(enabled);
}

void DatePickerExample::onCalendarModeChanged() {
    int index = m_selectionModeCombo->currentIndex();
    auto mode = static_cast<Components::FluentCalendarSelectionMode>(index);
    m_calendar->setSelectionMode(mode);
}

void DatePickerExample::onTodayClicked() {
    m_datePicker->setDate(QDate::currentDate());
}

void DatePickerExample::onClearClicked() {
    // For demonstration, we'll set to an invalid date to show placeholder
    m_selectedDateLabel->setText("No date selected");
}

void DatePickerExample::toggleTheme() {
    auto& theme = Styling::FluentTheme::instance();
    auto newMode = (theme.mode() == Styling::FluentThemeMode::Light)
                       ? Styling::FluentThemeMode::Dark
                       : Styling::FluentThemeMode::Light;
    theme.setMode(newMode);
}

void DatePickerExample::showDateRange() {
    QDate startDate = m_startDatePicker->date();
    QDate endDate = m_endDatePicker->date();

    if (startDate.isValid() && endDate.isValid()) {
        int days = startDate.daysTo(endDate);
        QString info = QString("Project duration: %1 days\n").arg(days);
        info += QString("Start: %1\n").arg(startDate.toString("MMMM dd, yyyy"));
        info += QString("End: %1").arg(endDate.toString("MMMM dd, yyyy"));
        m_infoText->setPlainText(info);
    } else {
        m_infoText->setPlainText("Please select both start and end dates.");
    }
}

void DatePickerExample::updateDateInfo(const QDate& date) {
    if (date.isValid()) {
        m_selectedDateLabel->setText(date.toString(m_datePicker->dateFormat()));
    } else {
        m_selectedDateLabel->setText("No date selected");
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    DatePickerExample example;
    example.show();

    return app.exec();
}

#include "DatePickerExample.moc"
