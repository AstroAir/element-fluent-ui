#include <QAccessible>
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStandardPaths>
#include <QStyleOption>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <chrono>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentBadge.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentComboBox.h"

#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentSplitter.h"
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;
using namespace std::chrono_literals;

struct ValidationResult {
    QString componentName;
    QString testName;
    bool passed;
    QString details;
    std::chrono::milliseconds duration;
};

class SimpleFluentValidator : public QMainWindow {
    Q_OBJECT

public:
    explicit SimpleFluentValidator(QWidget* parent = nullptr)
        : QMainWindow(parent), m_totalTests(0), m_passedTests(0) {
        setupUI();
        setupTests();
    }

private slots:
    void runValidation() {
        m_logOutput->clear();
        m_results.clear();
        m_totalTests = 0;
        m_passedTests = 0;

        m_logOutput->append("🚀 Starting FluentQt Component Validation...\n");

        // Test core components that we know exist
        testFluentButton();
        testFluentCard();
        testFluentTextInput();
        testFluentCheckBox();
        testFluentProgressBar();
        testFluentBadge();

        // Test additional form components
        testFluentComboBox();
        testFluentSlider();

        // Test feedback components
        testFluentTooltip();
        testFluentToast();

        // Test layout components
        testFluentNavigationView();
        testFluentTabView();
        testFluentSplitter();

        // Test specialized components
        testFluentCalendar();
        testFluentTreeView();

        // Test systems
        testAnimationSystem();
        testThemeSystem();

        showSummary();
    }

    void exportResults() {
        QString fileName = QFileDialog::getSaveFileName(
            this, "Export Validation Results",
            QStandardPaths::writableLocation(
                QStandardPaths::DocumentsLocation) +
                QString("/FluentQt_Simple_Validation_%1.json")
                    .arg(QDateTime::currentDateTime().toString(
                        "yyyyMMdd_hhmmss")),
            "JSON Files (*.json)");

        if (!fileName.isEmpty()) {
            QJsonObject report;
            report["timestamp"] =
                QDateTime::currentDateTime().toString(Qt::ISODate);
            report["totalTests"] = m_totalTests;
            report["passedTests"] = m_passedTests;
            report["failedTests"] = m_totalTests - m_passedTests;
            report["successRate"] =
                m_totalTests > 0
                    ? (double(m_passedTests) / m_totalTests * 100.0)
                    : 0.0;

            QJsonArray results;
            for (const auto& result : m_results) {
                QJsonObject resultObj;
                resultObj["componentName"] = result.componentName;
                resultObj["testName"] = result.testName;
                resultObj["passed"] = result.passed;
                resultObj["details"] = result.details;
                resultObj["duration"] =
                    static_cast<qint64>(result.duration.count());
                results.append(resultObj);
            }
            report["results"] = results;

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                QJsonDocument doc(report);
                file.write(doc.toJson());
                QMessageBox::information(
                    this, "Export Complete",
                    QString("Results exported to:\n%1").arg(fileName));
            }
        }
    }

private:
    void setupUI() {
        setWindowTitle("Simple FluentQt Component Validator");
        setMinimumSize(800, 600);

        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* layout = new QVBoxLayout(centralWidget);

        // Header
        auto* titleLabel = new QLabel("Simple FluentQt Component Validator");
        titleLabel->setStyleSheet(
            "font-size: 20px; font-weight: bold; color: #0078d4; margin: "
            "10px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);

        // Controls
        auto* controlLayout = new QHBoxLayout();
        auto* runButton = new QPushButton("🚀 Run Validation");
        auto* exportButton = new QPushButton("📊 Export Results");
        auto* clearButton = new QPushButton("🗑️ Clear");

        connect(runButton, &QPushButton::clicked, this,
                &SimpleFluentValidator::runValidation);
        connect(exportButton, &QPushButton::clicked, this,
                &SimpleFluentValidator::exportResults);
        connect(clearButton, &QPushButton::clicked, [this]() {
            m_logOutput->clear();
            m_progressBar->setValue(0);
            m_statusLabel->setText("Ready");
        });

        controlLayout->addWidget(runButton);
        controlLayout->addWidget(exportButton);
        controlLayout->addWidget(clearButton);
        controlLayout->addStretch();
        layout->addLayout(controlLayout);

        // Status
        auto* statusLayout = new QHBoxLayout();
        m_statusLabel = new QLabel("Ready");
        m_progressBar = new QProgressBar();
        statusLayout->addWidget(new QLabel("Status:"));
        statusLayout->addWidget(m_statusLabel);
        statusLayout->addStretch();
        statusLayout->addWidget(m_progressBar);
        layout->addLayout(statusLayout);

        // Log output
        m_logOutput = new QTextEdit();
        m_logOutput->setReadOnly(true);
        m_logOutput->setFont(QFont("Consolas", 9));
        layout->addWidget(m_logOutput);

        // Summary
        auto* summaryLayout = new QHBoxLayout();
        m_totalLabel = new QLabel("Total: 0");
        m_passedLabel = new QLabel("Passed: 0");
        m_failedLabel = new QLabel("Failed: 0");
        m_rateLabel = new QLabel("Rate: 0%");

        summaryLayout->addWidget(m_totalLabel);
        summaryLayout->addWidget(m_passedLabel);
        summaryLayout->addWidget(m_failedLabel);
        summaryLayout->addWidget(m_rateLabel);
        summaryLayout->addStretch();
        layout->addLayout(summaryLayout);
    }

    void setupTests() {
        // Initialize theme
        auto& theme = Styling::FluentTheme::instance();
        theme.setAccentColor(Styling::FluentAccentColor::Blue);
        theme.setMode(Styling::FluentThemeMode::Light);
    }

    void addResult(const QString& componentName, const QString& testName,
                   bool passed, const QString& details) {
        ValidationResult result;
        result.componentName = componentName;
        result.testName = testName;
        result.passed = passed;
        result.details = details;
        result.duration = 0ms;  // Simplified for this version

        m_results.append(result);
        m_totalTests++;
        if (passed)
            m_passedTests++;

        QString status = passed ? "✅ PASS" : "❌ FAIL";
        m_logOutput->append(QString("[%1] %2::%3 - %4")
                                .arg(status, componentName, testName, details));

        // Update progress
        int progress =
            (m_totalTests * 100) / 40;  // Estimate 40 total tests now
        m_progressBar->setValue(qMin(progress, 100));

        QCoreApplication::processEvents();
    }

    void testFluentButton() {
        m_statusLabel->setText("Testing FluentButton...");

        try {
            auto* button = new Components::FluentButton("Test Button", this);

            // Test basic functionality
            addResult("FluentButton", "Creation", true,
                      "Button created successfully");

            // Test text setting
            QString testText = "New Text";
            button->setText(testText);
            bool textSet = (button->text() == testText);
            addResult("FluentButton", "Text Setting", textSet,
                      textSet ? "Text set correctly" : "Text not set");

            // Test button style
            button->setButtonStyle(Components::FluentButtonStyle::Accent);
            bool styleSet = (button->buttonStyle() ==
                             Components::FluentButtonStyle::Accent);
            addResult("FluentButton", "Style Setting", styleSet,
                      styleSet ? "Style set correctly" : "Style not set");

            // Test enabled state
            button->setEnabled(false);
            bool disabledCorrectly = !button->isEnabled();
            addResult("FluentButton", "Disable State", disabledCorrectly,
                      disabledCorrectly ? "Button disabled correctly"
                                        : "Button not disabled");

            button->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentButton", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentButton", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentCard() {
        m_statusLabel->setText("Testing FluentCard...");

        try {
            auto* card = new Components::FluentCard(this);

            addResult("FluentCard", "Creation", true,
                      "Card created successfully");

            // Test title setting
            QString testTitle = "Test Card Title";
            card->setTitle(testTitle);
            bool titleSet = (card->title() == testTitle);
            addResult("FluentCard", "Title Setting", titleSet,
                      titleSet ? "Title set correctly" : "Title not set");

            // Test subtitle setting
            QString testSubtitle = "Test Subtitle";
            card->setSubtitle(testSubtitle);
            bool subtitleSet = (card->subtitle() == testSubtitle);
            addResult(
                "FluentCard", "Subtitle Setting", subtitleSet,
                subtitleSet ? "Subtitle set correctly" : "Subtitle not set");

            // Test elevation setting
            card->setElevation(Components::FluentCardElevation::Medium);
            bool elevationSet =
                (card->elevation() == Components::FluentCardElevation::Medium);
            addResult(
                "FluentCard", "Elevation Setting", elevationSet,
                elevationSet ? "Elevation set correctly" : "Elevation not set");

            card->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentCard", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentCard", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentTextInput() {
        m_statusLabel->setText("Testing FluentTextInput...");

        try {
            auto* input = new Components::FluentTextInput(this);

            addResult("FluentTextInput", "Creation", true,
                      "TextInput created successfully");

            // Test text setting
            QString testText = "Test Input Text";
            input->setText(testText);
            bool textSet = (input->text() == testText);
            addResult("FluentTextInput", "Text Setting", textSet,
                      textSet ? "Text set correctly" : "Text not set");

            // Test placeholder
            QString placeholder = "Enter text...";
            input->setPlaceholderText(placeholder);
            bool placeholderSet = (input->placeholderText() == placeholder);
            addResult("FluentTextInput", "Placeholder Setting", placeholderSet,
                      placeholderSet ? "Placeholder set correctly"
                                     : "Placeholder not set");

            input->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentTextInput", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentTextInput", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentCheckBox() {
        m_statusLabel->setText("Testing FluentCheckBox...");

        try {
            auto* checkbox =
                new Components::FluentCheckBox("Test Checkbox", this);

            addResult("FluentCheckBox", "Creation", true,
                      "CheckBox created successfully");

            // Test checked state
            checkbox->setChecked(true);
            bool checkedCorrectly = checkbox->isChecked();
            addResult("FluentCheckBox", "Checked State", checkedCorrectly,
                      checkedCorrectly ? "CheckBox checked correctly"
                                       : "CheckBox not checked");

            checkbox->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentCheckBox", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentCheckBox", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentProgressBar() {
        m_statusLabel->setText("Testing FluentProgressBar...");

        try {
            auto* progress = new Components::FluentProgressBar(this);

            addResult("FluentProgressBar", "Creation", true,
                      "ProgressBar created successfully");

            // Test range setting
            progress->setRange(0, 100);
            bool rangeSet =
                (progress->minimum() == 0 && progress->maximum() == 100);
            addResult("FluentProgressBar", "Range Setting", rangeSet,
                      rangeSet ? "Range set correctly" : "Range not set");

            // Test value setting
            progress->setValue(50);
            bool valueSet = (progress->value() == 50);
            addResult("FluentProgressBar", "Value Setting", valueSet,
                      valueSet ? "Value set correctly" : "Value not set");

            progress->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentProgressBar", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentProgressBar", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentBadge() {
        m_statusLabel->setText("Testing FluentBadge...");

        try {
            auto* badge = new Components::FluentBadge("99+", this);

            addResult("FluentBadge", "Creation", true,
                      "Badge created successfully");

            // Test text setting
            QString testText = "New Badge";
            badge->setText(testText);
            bool textSet = (badge->text() == testText);
            addResult("FluentBadge", "Text Setting", textSet,
                      textSet ? "Text set correctly" : "Text not set");

            badge->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentBadge", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentBadge", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentComboBox() {
        m_statusLabel->setText("Testing FluentComboBox...");

        try {
            auto* combo = new Components::FluentComboBox(this);

            addResult("FluentComboBox", "Creation", true,
                      "ComboBox created successfully");

            // Test adding items
            combo->addItem("Item 1");
            combo->addItem("Item 2");
            combo->addItem("Item 3");
            bool itemsAdded = (combo->count() == 3);
            addResult(
                "FluentComboBox", "Add Items", itemsAdded,
                itemsAdded ? "Items added successfully" : "Items not added");

            // Test current selection
            combo->setCurrentIndex(1);
            bool selectionSet = (combo->currentIndex() == 1);
            addResult(
                "FluentComboBox", "Selection", selectionSet,
                selectionSet ? "Selection set correctly" : "Selection not set");

            combo->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentComboBox", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentComboBox", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentSlider() {
        m_statusLabel->setText("Testing FluentSlider...");

        try {
            auto* slider = new Components::FluentSlider(this);

            addResult("FluentSlider", "Creation", true,
                      "Slider created successfully");

            // Test range setting
            slider->setRange(0, 100);
            bool rangeSet =
                (slider->minimum() == 0 && slider->maximum() == 100);
            addResult("FluentSlider", "Range Setting", rangeSet,
                      rangeSet ? "Range set correctly" : "Range not set");

            // Test value setting
            slider->setValue(50);
            bool valueSet = (slider->value() == 50);
            addResult("FluentSlider", "Value Setting", valueSet,
                      valueSet ? "Value set correctly" : "Value not set");

            slider->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentSlider", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentSlider", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    // FluentSpinBox not implemented yet - skipping test

    // FluentToggleSwitch not implemented yet - skipping test

    void testFluentTooltip() {
        m_statusLabel->setText("Testing FluentTooltip...");

        try {
            auto* tooltip = new Components::FluentTooltip(this);

            addResult("FluentTooltip", "Creation", true,
                      "Tooltip created successfully");

            // Test text setting
            QString testText = "Test Tooltip Text";
            tooltip->setText(testText);
            bool textSet = (tooltip->text() == testText);
            addResult("FluentTooltip", "Text Setting", textSet,
                      textSet ? "Text set correctly" : "Text not set");

            tooltip->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentTooltip", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentTooltip", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentToast() {
        m_statusLabel->setText("Testing FluentToast...");

        try {
            auto* toast =
                new Components::FluentToast(Components::FluentToastType::Info,
                                            "Test Title", "Test Message", this);

            addResult("FluentToast", "Creation", true,
                      "Toast created successfully");

            // Test title setting
            QString testTitle = "New Title";
            toast->setTitle(testTitle);
            bool titleSet = (toast->title() == testTitle);
            addResult("FluentToast", "Title Setting", titleSet,
                      titleSet ? "Title set correctly" : "Title not set");

            // Test message setting
            QString testMessage = "New Message";
            toast->setMessage(testMessage);
            bool messageSet = (toast->message() == testMessage);
            addResult("FluentToast", "Message Setting", messageSet,
                      messageSet ? "Message set correctly" : "Message not set");

            toast->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentToast", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentToast", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentNavigationView() {
        m_statusLabel->setText("Testing FluentNavigationView...");

        try {
            auto* navView = new Components::FluentNavigationView(this);

            addResult("FluentNavigationView", "Creation", true,
                      "NavigationView created successfully");

            // Test basic functionality (simplified to avoid linking issues)
            bool basicFunctionality = navView->isVisible();
            addResult("FluentNavigationView", "Basic Functionality",
                      basicFunctionality,
                      basicFunctionality ? "NavigationView functional"
                                         : "NavigationView not functional");

            // Test size hint
            QSize hint = navView->sizeHint();
            bool hasSizeHint = (hint.width() > 0 && hint.height() > 0);
            addResult("FluentNavigationView", "Size Hint", hasSizeHint,
                      hasSizeHint ? "Size hint available"
                                  : "Size hint not available");

            navView->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentNavigationView", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentNavigationView", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentTabView() {
        m_statusLabel->setText("Testing FluentTabView...");

        try {
            auto* tabView = new Components::FluentTabView(this);

            addResult("FluentTabView", "Creation", true,
                      "TabView created successfully");

            // Test adding tabs
            auto* widget1 = new QLabel("Tab 1 Content", this);
            auto* widget2 = new QLabel("Tab 2 Content", this);

            tabView->addTab("Tab 1", widget1);
            tabView->addTab("Tab 2", widget2);

            bool tabsAdded = (tabView->count() == 2);
            addResult("FluentTabView", "Add Tabs", tabsAdded,
                      tabsAdded ? "Tabs added successfully" : "Tabs not added");

            // Test current tab
            tabView->setCurrentIndex(1);
            bool currentTabSet = (tabView->currentIndex() == 1);
            addResult("FluentTabView", "Current Tab", currentTabSet,
                      currentTabSet ? "Current tab set correctly"
                                    : "Current tab not set");

            tabView->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentTabView", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentTabView", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentSplitter() {
        m_statusLabel->setText("Testing FluentSplitter...");

        try {
            auto* splitter = new Components::FluentSplitter(this);

            addResult("FluentSplitter", "Creation", true,
                      "Splitter created successfully");

            // Test adding widgets
            auto* widget1 = new QLabel("Left Panel", this);
            auto* widget2 = new QLabel("Right Panel", this);

            splitter->addWidget(widget1);
            splitter->addWidget(widget2);

            bool widgetsAdded = (splitter->count() == 2);
            addResult("FluentSplitter", "Add Widgets", widgetsAdded,
                      widgetsAdded ? "Widgets added successfully"
                                   : "Widgets not added");

            // Test orientation
            splitter->setOrientation(Qt::Vertical);
            bool orientationSet = (splitter->orientation() == Qt::Vertical);
            addResult("FluentSplitter", "Orientation", orientationSet,
                      orientationSet ? "Orientation set correctly"
                                     : "Orientation not set");

            splitter->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentSplitter", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentSplitter", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentCalendar() {
        m_statusLabel->setText("Testing FluentCalendar...");

        try {
            auto* calendar = new Components::FluentCalendar(this);

            addResult("FluentCalendar", "Creation", true,
                      "Calendar created successfully");

            // Test date selection
            QDate testDate = QDate::currentDate().addDays(5);
            calendar->setSelectedDate(testDate);
            bool dateSet = (calendar->selectedDate() == testDate);
            addResult("FluentCalendar", "Date Selection", dateSet,
                      dateSet ? "Date set correctly" : "Date not set");

            // Test minimum date
            QDate minDate = QDate::currentDate().addDays(-30);
            calendar->setMinimumDate(minDate);
            bool minDateSet = (calendar->minimumDate() == minDate);
            addResult("FluentCalendar", "Minimum Date", minDateSet,
                      minDateSet ? "Minimum date set correctly"
                                 : "Minimum date not set");

            calendar->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentCalendar", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentCalendar", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testFluentTreeView() {
        m_statusLabel->setText("Testing FluentTreeView...");

        try {
            auto* treeView = new Components::FluentTreeView(this);

            addResult("FluentTreeView", "Creation", true,
                      "TreeView created successfully");

            // Test basic functionality
            bool basicFunctionality = treeView->isVisible();
            addResult("FluentTreeView", "Basic Functionality",
                      basicFunctionality,
                      basicFunctionality ? "TreeView functional"
                                         : "TreeView not functional");

            // Test size hint
            QSize hint = treeView->sizeHint();
            bool hasSizeHint = (hint.width() > 0 && hint.height() > 0);
            addResult("FluentTreeView", "Size Hint", hasSizeHint,
                      hasSizeHint ? "Size hint available"
                                  : "Size hint not available");

            treeView->deleteLater();

        } catch (const std::exception& e) {
            addResult("FluentTreeView", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("FluentTreeView", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testAnimationSystem() {
        m_statusLabel->setText("Testing Animation System...");

        try {
            auto* testWidget = new QLabel("Animation Test", this);

            // Test fade animation
            auto fadeAnim = Animation::FluentAnimator::fadeIn(testWidget);
            bool fadeAnimCreated = (fadeAnim != nullptr);
            addResult("Animation", "Fade Animation", fadeAnimCreated,
                      fadeAnimCreated ? "Fade animation created"
                                      : "Fade animation failed");

            // Test slide animation
            auto slideAnim =
                Animation::FluentAnimator::slideIn(testWidget, QPoint(0, 20));
            bool slideAnimCreated = (slideAnim != nullptr);
            addResult("Animation", "Slide Animation", slideAnimCreated,
                      slideAnimCreated ? "Slide animation created"
                                       : "Slide animation failed");

            testWidget->deleteLater();

        } catch (const std::exception& e) {
            addResult("Animation", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("Animation", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void testThemeSystem() {
        m_statusLabel->setText("Testing Theme System...");

        try {
            auto& theme = Styling::FluentTheme::instance();

            // Test theme mode switching
            auto originalMode = theme.mode();
            auto newMode = (originalMode == Styling::FluentThemeMode::Light)
                               ? Styling::FluentThemeMode::Dark
                               : Styling::FluentThemeMode::Light;

            theme.setMode(newMode);
            bool modeChanged = (theme.mode() == newMode);
            addResult("Theme", "Mode Switching", modeChanged,
                      modeChanged ? "Theme mode changed successfully"
                                  : "Theme mode change failed");

            // Restore original mode
            theme.setMode(originalMode);

            // Test accent color
            theme.setAccentColor(Styling::FluentAccentColor::Green);
            bool accentChanged =
                (theme.accentColorEnum() == Styling::FluentAccentColor::Green);
            addResult("Theme", "Accent Color", accentChanged,
                      accentChanged ? "Accent color changed successfully"
                                    : "Accent color change failed");

            // Restore original accent
            theme.setAccentColor(Styling::FluentAccentColor::Blue);

        } catch (const std::exception& e) {
            addResult("Theme", "Exception Test", false,
                      QString("Exception: %1").arg(e.what()));
        } catch (...) {
            addResult("Theme", "Exception Test", false,
                      "Unknown exception occurred");
        }
    }

    void showSummary() {
        m_statusLabel->setText("Validation Complete");
        m_progressBar->setValue(100);

        double successRate =
            m_totalTests > 0 ? (double(m_passedTests) / m_totalTests * 100.0)
                             : 0.0;

        m_totalLabel->setText(QString("Total: %1").arg(m_totalTests));
        m_passedLabel->setText(QString("Passed: %1").arg(m_passedTests));
        m_failedLabel->setText(
            QString("Failed: %1").arg(m_totalTests - m_passedTests));
        m_rateLabel->setText(
            QString("Rate: %1%").arg(QString::number(successRate, 'f', 1)));

        QString summary = QString(
                              "\n🎯 Validation Summary:\n"
                              "Total Tests: %1\n"
                              "Passed: %2\n"
                              "Failed: %3\n"
                              "Success Rate: %4%\n")
                              .arg(m_totalTests)
                              .arg(m_passedTests)
                              .arg(m_totalTests - m_passedTests)
                              .arg(QString::number(successRate, 'f', 1));

        if (successRate >= 90.0) {
            summary += "🎉 Excellent! FluentQt components are working well.";
        } else if (successRate >= 75.0) {
            summary += "✅ Good! Most FluentQt components are functional.";
        } else if (successRate >= 50.0) {
            summary += "⚠️ Fair. Some components need attention.";
        } else {
            summary += "❌ Poor. Significant issues detected.";
        }

        m_logOutput->append(summary);
    }

private:
    QTextEdit* m_logOutput;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    QLabel* m_totalLabel;
    QLabel* m_passedLabel;
    QLabel* m_failedLabel;
    QLabel* m_rateLabel;

    QList<ValidationResult> m_results;
    int m_totalTests;
    int m_passedTests;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    SimpleFluentValidator validator;
    validator.show();

    return app.exec();
}

#include "SimpleFluentValidator.moc"
