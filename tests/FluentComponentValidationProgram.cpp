#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "FluentComponentValidator.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class FluentValidationDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentValidationDashboard(QWidget* parent = nullptr)
        : QMainWindow(parent), m_validator(new FluentComponentValidator(this)) {
        setupUI();
        connectSignals();
        setupValidator();
    }

private slots:
    void startValidation() {
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_progressBar->setValue(0);
        m_resultsTable->setRowCount(0);
        m_logOutput->clear();

        m_logOutput->append(
            "🚀 Starting comprehensive FluentQt component validation...\n");

        // Configure validator based on UI settings
        m_validator->setStrictMode(m_strictModeCheck->isChecked());
        m_validator->setVerboseOutput(m_verboseCheck->isChecked());
        m_validator->setPlatformSpecific(m_platformCheck->isChecked());

        // Start validation
        m_validator->validateAllComponents();
    }

    void stopValidation() {
        m_startButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_logOutput->append("⏹️ Validation stopped by user.\n");
    }

    void onValidationStarted(const QString& componentName) {
        m_currentComponentLabel->setText(
            QString("Validating: %1").arg(componentName));
        m_logOutput->append(
            QString("📋 Starting validation of %1...").arg(componentName));
    }

    void onValidationCompleted(const QString& componentName, bool passed) {
        QString status = passed ? "✅ PASSED" : "❌ FAILED";
        m_logOutput->append(
            QString("%1 %2 validation completed").arg(status, componentName));

        // Update component status in the overview
        updateComponentStatus(componentName, passed);
    }

    void onTestCompleted(const ValidationResult& result) {
        // Add result to table
        int row = m_resultsTable->rowCount();
        m_resultsTable->insertRow(row);

        m_resultsTable->setItem(row, 0,
                                new QTableWidgetItem(result.componentName));
        m_resultsTable->setItem(row, 1, new QTableWidgetItem(result.testName));
        m_resultsTable->setItem(row, 2, new QTableWidgetItem(result.category));

        QTableWidgetItem* statusItem =
            new QTableWidgetItem(result.passed ? "✅ PASS" : "❌ FAIL");
        statusItem->setBackground(result.passed ? QColor(200, 255, 200)
                                                : QColor(255, 200, 200));
        m_resultsTable->setItem(row, 3, statusItem);

        m_resultsTable->setItem(row, 4, new QTableWidgetItem(result.details));
        m_resultsTable->setItem(
            row, 5,
            new QTableWidgetItem(QString("%1ms").arg(result.duration.count())));

        // Auto-scroll to latest result
        m_resultsTable->scrollToBottom();
    }

    void onAllValidationsCompleted(int totalTests, int passedTests) {
        m_startButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_currentComponentLabel->setText("Validation Complete");
        m_progressBar->setValue(100);

        // Update summary
        double successRate =
            totalTests > 0 ? (double(passedTests) / totalTests * 100.0) : 0.0;

        QString summary = QString(
                              "🎯 Validation Summary:\n"
                              "Total Tests: %1\n"
                              "Passed: %2\n"
                              "Failed: %3\n"
                              "Success Rate: %4%\n\n")
                              .arg(totalTests)
                              .arg(passedTests)
                              .arg(totalTests - passedTests)
                              .arg(QString::number(successRate, 'f', 1));

        if (successRate >= 90.0) {
            summary +=
                "🎉 Excellent! FluentQt components are highly compliant with "
                "Fluent Design.";
        } else if (successRate >= 75.0) {
            summary +=
                "✅ Good! Most FluentQt components meet Fluent Design "
                "standards.";
        } else if (successRate >= 50.0) {
            summary +=
                "⚠️ Fair. Some components need improvement to meet Fluent "
                "Design standards.";
        } else {
            summary +=
                "❌ Poor. Significant work needed to achieve Fluent Design "
                "compliance.";
        }

        m_logOutput->append(summary);

        // Update summary labels
        m_totalTestsLabel->setText(QString("Total Tests: %1").arg(totalTests));
        m_passedTestsLabel->setText(QString("Passed: %1").arg(passedTests));
        m_failedTestsLabel->setText(
            QString("Failed: %1").arg(totalTests - passedTests));
        m_successRateLabel->setText(
            QString("Success Rate: %1%")
                .arg(QString::number(successRate, 'f', 1)));

        // Show completion message
        QMessageBox::information(
            this, "Validation Complete",
            QString("Validation completed!\n\n%1").arg(summary));
    }

    void exportResults() {
        QString fileName = QFileDialog::getSaveFileName(
            this, "Export Validation Results",
            QStandardPaths::writableLocation(
                QStandardPaths::DocumentsLocation) +
                QString("/FluentQt_Validation_%1.json")
                    .arg(QDateTime::currentDateTime().toString(
                        "yyyyMMdd_hhmmss")),
            "JSON Files (*.json)");

        if (!fileName.isEmpty()) {
            m_validator->generateReport(fileName);
            QMessageBox::information(
                this, "Export Complete",
                QString("Validation results exported to:\n%1").arg(fileName));
        }
    }

    void clearResults() {
        m_resultsTable->setRowCount(0);
        m_logOutput->clear();
        m_progressBar->setValue(0);
        m_currentComponentLabel->setText("Ready");

        // Reset summary
        m_totalTestsLabel->setText("Total Tests: 0");
        m_passedTestsLabel->setText("Passed: 0");
        m_failedTestsLabel->setText("Failed: 0");
        m_successRateLabel->setText("Success Rate: 0%");

        // Reset component overview
        for (int i = 0; i < m_componentOverview->topLevelItemCount(); ++i) {
            QTreeWidgetItem* item = m_componentOverview->topLevelItem(i);
            item->setText(1, "Pending");
            item->setIcon(1, QIcon());
        }
    }

    void toggleTheme() {
        auto& theme = Styling::FluentTheme::instance();
        auto newMode = (theme.mode() == Styling::FluentThemeMode::Light)
                           ? Styling::FluentThemeMode::Dark
                           : Styling::FluentThemeMode::Light;
        theme.setMode(newMode);

        m_themeButton->setText(newMode == Styling::FluentThemeMode::Light
                                   ? "🌙 Dark Mode"
                                   : "☀️ Light Mode");
    }

private:
    void setupUI() {
        setWindowTitle("FluentQt Component Validation Dashboard");
        setMinimumSize(1200, 800);

        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);

        // Header
        setupHeader(mainLayout);

        // Main content area
        auto* splitter = new QSplitter(Qt::Horizontal, this);
        mainLayout->addWidget(splitter);

        // Left panel - Controls and overview
        setupLeftPanel(splitter);

        // Right panel - Results and logs
        setupRightPanel(splitter);

        // Status bar
        setupStatusBar();

        // Set splitter proportions
        splitter->setSizes({300, 900});
    }

    void setupHeader(QVBoxLayout* mainLayout) {
        auto* headerWidget = new QWidget();
        auto* headerLayout = new QHBoxLayout(headerWidget);

        auto* titleLabel =
            new QLabel("FluentQt Component Validation Dashboard");
        titleLabel->setStyleSheet(
            "font-size: 24px; font-weight: bold; color: #0078d4;");
        headerLayout->addWidget(titleLabel);

        headerLayout->addStretch();

        m_themeButton = new QPushButton("🌙 Dark Mode");
        connect(m_themeButton, &QPushButton::clicked, this,
                &FluentValidationDashboard::toggleTheme);
        headerLayout->addWidget(m_themeButton);

        mainLayout->addWidget(headerWidget);
    }

    void setupLeftPanel(QSplitter* splitter) {
        auto* leftWidget = new QWidget();
        auto* leftLayout = new QVBoxLayout(leftWidget);

        // Control panel
        auto* controlGroup = new QGroupBox("Validation Controls");
        auto* controlLayout = new QVBoxLayout(controlGroup);

        auto* buttonLayout = new QHBoxLayout();
        m_startButton = new QPushButton("🚀 Start Validation");
        m_stopButton = new QPushButton("⏹️ Stop");
        m_stopButton->setEnabled(false);

        buttonLayout->addWidget(m_startButton);
        buttonLayout->addWidget(m_stopButton);
        controlLayout->addLayout(buttonLayout);

        // Options
        m_strictModeCheck = new QCheckBox("Strict Mode");
        m_strictModeCheck->setToolTip(
            "Enable strict validation (all tests must pass)");

        m_verboseCheck = new QCheckBox("Verbose Output");
        m_verboseCheck->setChecked(true);
        m_verboseCheck->setToolTip("Show detailed test output");

        m_platformCheck = new QCheckBox("Platform-Specific Tests");
        m_platformCheck->setChecked(true);
        m_platformCheck->setToolTip(
            "Include platform-specific validation tests");

        controlLayout->addWidget(m_strictModeCheck);
        controlLayout->addWidget(m_verboseCheck);
        controlLayout->addWidget(m_platformCheck);

        leftLayout->addWidget(controlGroup);

        // Component overview
        auto* overviewGroup = new QGroupBox("Component Overview");
        auto* overviewLayout = new QVBoxLayout(overviewGroup);

        m_componentOverview = new QTreeWidget();
        m_componentOverview->setHeaderLabels({"Component", "Status"});
        setupComponentOverview();
        overviewLayout->addWidget(m_componentOverview);

        leftLayout->addWidget(overviewGroup);

        // Summary
        auto* summaryGroup = new QGroupBox("Summary");
        auto* summaryLayout = new QVBoxLayout(summaryGroup);

        m_totalTestsLabel = new QLabel("Total Tests: 0");
        m_passedTestsLabel = new QLabel("Passed: 0");
        m_failedTestsLabel = new QLabel("Failed: 0");
        m_successRateLabel = new QLabel("Success Rate: 0%");

        summaryLayout->addWidget(m_totalTestsLabel);
        summaryLayout->addWidget(m_passedTestsLabel);
        summaryLayout->addWidget(m_failedTestsLabel);
        summaryLayout->addWidget(m_successRateLabel);

        leftLayout->addWidget(summaryGroup);

        leftLayout->addStretch();
        splitter->addWidget(leftWidget);
    }

    void setupRightPanel(QSplitter* splitter) {
        auto* rightWidget = new QWidget();
        auto* rightLayout = new QVBoxLayout(rightWidget);

        // Progress and status
        auto* progressLayout = new QHBoxLayout();
        m_currentComponentLabel = new QLabel("Ready");
        m_progressBar = new QProgressBar();

        progressLayout->addWidget(new QLabel("Status:"));
        progressLayout->addWidget(m_currentComponentLabel);
        progressLayout->addStretch();
        progressLayout->addWidget(m_progressBar);

        rightLayout->addLayout(progressLayout);

        // Tabbed results area
        auto* tabWidget = new QTabWidget();

        // Results table tab
        auto* resultsWidget = new QWidget();
        auto* resultsLayout = new QVBoxLayout(resultsWidget);

        auto* resultsButtonLayout = new QHBoxLayout();
        auto* exportButton = new QPushButton("📊 Export Results");
        auto* clearButton = new QPushButton("🗑️ Clear Results");

        connect(exportButton, &QPushButton::clicked, this,
                &FluentValidationDashboard::exportResults);
        connect(clearButton, &QPushButton::clicked, this,
                &FluentValidationDashboard::clearResults);

        resultsButtonLayout->addWidget(exportButton);
        resultsButtonLayout->addWidget(clearButton);
        resultsButtonLayout->addStretch();
        resultsLayout->addLayout(resultsButtonLayout);

        m_resultsTable = new QTableWidget();
        m_resultsTable->setColumnCount(6);
        m_resultsTable->setHorizontalHeaderLabels(
            {"Component", "Test", "Category", "Status", "Details", "Duration"});
        m_resultsTable->horizontalHeader()->setStretchLastSection(true);
        m_resultsTable->setAlternatingRowColors(true);
        m_resultsTable->setSortingEnabled(true);

        resultsLayout->addWidget(m_resultsTable);
        tabWidget->addTab(resultsWidget, "📋 Test Results");

        // Log output tab
        m_logOutput = new QTextEdit();
        m_logOutput->setReadOnly(true);
        m_logOutput->setFont(QFont("Consolas", 9));
        tabWidget->addTab(m_logOutput, "📝 Log Output");

        rightLayout->addWidget(tabWidget);
        splitter->addWidget(rightWidget);
    }

    void setupStatusBar() {
        statusBar()->showMessage("Ready to validate FluentQt components");
    }

    void setupComponentOverview() {
        QStringList components = {
            "FluentButton",      "FluentTextInput", "FluentCheckBox",
            "FluentRadioButton", "FluentComboBox",  "FluentSlider",
            "FluentCard",        "FluentPanel",     "FluentProgressBar",
            "FluentToast",       "FluentTooltip",   "FluentBadge"};

        for (const QString& component : components) {
            auto* item = new QTreeWidgetItem(m_componentOverview);
            item->setText(0, component);
            item->setText(1, "Pending");
        }

        m_componentOverview->expandAll();
    }

    void updateComponentStatus(const QString& componentName, bool passed) {
        for (int i = 0; i < m_componentOverview->topLevelItemCount(); ++i) {
            QTreeWidgetItem* item = m_componentOverview->topLevelItem(i);
            if (item->text(0) == componentName) {
                item->setText(1, passed ? "✅ Passed" : "❌ Failed");
                break;
            }
        }
    }

    void connectSignals() {
        connect(m_startButton, &QPushButton::clicked, this,
                &FluentValidationDashboard::startValidation);
        connect(m_stopButton, &QPushButton::clicked, this,
                &FluentValidationDashboard::stopValidation);
    }

    void setupValidator() {
        connect(m_validator, &FluentComponentValidator::validationStarted, this,
                &FluentValidationDashboard::onValidationStarted);
        connect(m_validator, &FluentComponentValidator::validationCompleted,
                this, &FluentValidationDashboard::onValidationCompleted);
        connect(m_validator, &FluentComponentValidator::testCompleted, this,
                &FluentValidationDashboard::onTestCompleted);
        connect(m_validator, &FluentComponentValidator::allValidationsCompleted,
                this, &FluentValidationDashboard::onAllValidationsCompleted);
    }

private:
    FluentComponentValidator* m_validator;

    // UI Controls
    QPushButton* m_startButton;
    QPushButton* m_stopButton;
    QPushButton* m_themeButton;
    QCheckBox* m_strictModeCheck;
    QCheckBox* m_verboseCheck;
    QCheckBox* m_platformCheck;

    // Status and Progress
    QLabel* m_currentComponentLabel;
    QProgressBar* m_progressBar;

    // Results Display
    QTableWidget* m_resultsTable;
    QTextEdit* m_logOutput;
    QTreeWidget* m_componentOverview;

    // Summary
    QLabel* m_totalTestsLabel;
    QLabel* m_passedTestsLabel;
    QLabel* m_failedTestsLabel;
    QLabel* m_successRateLabel;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);
    theme.setMode(Styling::FluentThemeMode::Light);

    FluentValidationDashboard dashboard;
    dashboard.show();

    return app.exec();
}

#include "FluentComponentValidationProgram.moc"
