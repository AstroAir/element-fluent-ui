// tests/DataTableTest.cpp
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QFileDialog>
#include <QRandomGenerator>
#include <QDebug>

#include "FluentQt/Components/FluentDataTable.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

class DataTableTestWidget : public QWidget {
    Q_OBJECT

public:
    explicit DataTableTestWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupSampleData();
        connectSignals();
        
        // Set window properties
        setWindowTitle("FluentDataTable Test");
        setMinimumSize(1200, 800);
        
        // Apply Fluent theme
        FluentTheme::instance().setMode(FluentThemeMode::Light);
    }

private slots:
    void onRowClicked(int row) {
        m_statusLabel->setText(QString("Row clicked: %1").arg(row));
        m_logEdit->append(QString("Row clicked: %1").arg(row));
    }
    
    void onRowDoubleClicked(int row) {
        m_statusLabel->setText(QString("Row double-clicked: %1").arg(row));
        m_logEdit->append(QString("Row double-clicked: %1").arg(row));
    }
    
    void onSelectionChanged(const QList<int>& selectedRows) {
        m_statusLabel->setText(QString("Selection changed: %1 rows selected").arg(selectedRows.size()));
        m_logEdit->append(QString("Selection changed: %1 rows").arg(selectedRows.size()));
    }
    
    void onSortingChanged(const QString& columnId, Qt::SortOrder order) {
        QString orderText = (order == Qt::AscendingOrder) ? "Ascending" : "Descending";
        m_statusLabel->setText(QString("Sorted by %1 (%2)").arg(columnId).arg(orderText));
        m_logEdit->append(QString("Sorted by %1 (%2)").arg(columnId).arg(orderText));
    }
    
    void onFilterChanged(const QString& columnId, const QString& filter) {
        m_statusLabel->setText(QString("Filter applied to %1: '%2'").arg(columnId).arg(filter));
        m_logEdit->append(QString("Filter: %1 = '%2'").arg(columnId).arg(filter));
    }
    
    void onPageChanged(int page) {
        m_statusLabel->setText(QString("Page changed to: %1").arg(page));
        m_logEdit->append(QString("Page changed to: %1").arg(page));
    }
    
    void onPageSizeChanged(int size) {
        m_statusLabel->setText(QString("Page size changed to: %1").arg(size));
        m_logEdit->append(QString("Page size changed to: %1").arg(size));
    }
    
    void onDataChanged() {
        m_statusLabel->setText("Data changed");
        m_logEdit->append("Data changed");
    }
    
    void onThemeToggle() {
        auto& theme = FluentTheme::instance();
        auto currentMode = theme.mode();
        auto newMode = (currentMode == FluentThemeMode::Light) 
                      ? FluentThemeMode::Dark 
                      : FluentThemeMode::Light;
        theme.setMode(newMode);
        
        m_themeButton->setText(newMode == FluentThemeMode::Dark 
                              ? "Switch to Light Theme" 
                              : "Switch to Dark Theme");
    }
    
    void onSelectionModeChanged() {
        FluentDataTable::SelectionMode mode = static_cast<FluentDataTable::SelectionMode>(m_selectionModeCombo->currentIndex());
        m_dataTable->setSelectionMode(mode);
        m_statusLabel->setText(QString("Selection mode: %1").arg(m_selectionModeCombo->currentText()));
    }
    
    void onSortingToggle(bool enabled) {
        m_dataTable->setSortingEnabled(enabled);
        m_statusLabel->setText(enabled ? "Sorting enabled" : "Sorting disabled");
    }
    
    void onFilteringToggle(bool enabled) {
        m_dataTable->setFilteringEnabled(enabled);
        m_statusLabel->setText(enabled ? "Filtering enabled" : "Filtering disabled");
    }
    
    void onPaginationToggle(bool enabled) {
        m_dataTable->setPaginationEnabled(enabled);
        m_statusLabel->setText(enabled ? "Pagination enabled" : "Pagination disabled");
    }
    
    void onAlternatingRowsToggle(bool enabled) {
        m_dataTable->setAlternatingRowColors(enabled);
        m_statusLabel->setText(enabled ? "Alternating rows enabled" : "Alternating rows disabled");
    }
    
    void onGridToggle(bool enabled) {
        m_dataTable->setGridVisible(enabled);
        m_statusLabel->setText(enabled ? "Grid visible" : "Grid hidden");
    }
    
    void onGlobalFilterChanged() {
        QString filter = m_globalFilterEdit->text();
        m_dataTable->setGlobalFilter(filter);
        m_statusLabel->setText(QString("Global filter: '%1'").arg(filter));
    }
    
    void onAddSampleData() {
        addRandomRows(10);
        m_statusLabel->setText("Added 10 random rows");
    }
    
    void onClearData() {
        m_dataTable->clearData();
        m_statusLabel->setText("Data cleared");
    }
    
    void onExportCsv() {
        QString filename = QFileDialog::getSaveFileName(this, "Export to CSV", "data.csv", "CSV Files (*.csv)");
        if (!filename.isEmpty()) {
            m_dataTable->exportToCsv(filename);
            m_statusLabel->setText(QString("Exported to: %1").arg(filename));
        }
    }
    
    void onExportJson() {
        QString filename = QFileDialog::getSaveFileName(this, "Export to JSON", "data.json", "JSON Files (*.json)");
        if (!filename.isEmpty()) {
            m_dataTable->exportToJson(filename);
            m_statusLabel->setText(QString("Exported to: %1").arg(filename));
        }
    }

private:
    void setupUI() {
        auto* mainLayout = new QHBoxLayout(this);
        
        // Left panel - data table
        auto* leftPanel = new QWidget(this);
        leftPanel->setMinimumWidth(800);
        setupLeftPanel(leftPanel);
        mainLayout->addWidget(leftPanel);
        
        // Right panel - controls and log
        auto* rightPanel = new QWidget(this);
        rightPanel->setFixedWidth(350);
        setupRightPanel(rightPanel);
        mainLayout->addWidget(rightPanel);
    }
    
    void setupLeftPanel(QWidget* parent) {
        auto* layout = new QVBoxLayout(parent);
        
        // Title
        auto* titleLabel = new QLabel("FluentDataTable Component Test");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        layout->addWidget(titleLabel);
        
        // Status label
        m_statusLabel = new QLabel("Use controls to interact with the data table");
        m_statusLabel->setStyleSheet("color: blue; margin: 5px;");
        layout->addWidget(m_statusLabel);
        
        // Global filter
        auto* filterLayout = new QHBoxLayout();
        filterLayout->addWidget(new QLabel("Global Filter:"));
        m_globalFilterEdit = new QLineEdit();
        m_globalFilterEdit->setPlaceholderText("Search all columns...");
        filterLayout->addWidget(m_globalFilterEdit);
        layout->addLayout(filterLayout);
        
        // Data table
        m_dataTable = new FluentDataTable(this);
        layout->addWidget(m_dataTable);
    }
    
    void setupRightPanel(QWidget* parent) {
        auto* layout = new QVBoxLayout(parent);
        
        // Control panel
        setupControlPanel(layout);
        
        // Action buttons
        setupActionButtons(layout);
        
        // Event log
        auto* logGroup = new QGroupBox("Event Log");
        auto* logLayout = new QVBoxLayout(logGroup);
        
        m_logEdit = new QTextEdit(this);
        m_logEdit->setMaximumHeight(200);
        m_logEdit->setReadOnly(true);
        logLayout->addWidget(m_logEdit);
        
        auto* clearLogButton = new QPushButton("Clear Log");
        connect(clearLogButton, &QPushButton::clicked, m_logEdit, &QTextEdit::clear);
        logLayout->addWidget(clearLogButton);
        
        layout->addWidget(logGroup);
        
        layout->addStretch();
    }
    
    void setupControlPanel(QVBoxLayout* mainLayout) {
        auto* controlGroup = new QGroupBox("Controls");
        auto* controlLayout = new QVBoxLayout(controlGroup);
        
        // Theme toggle
        m_themeButton = new QPushButton("Switch to Dark Theme");
        controlLayout->addWidget(m_themeButton);
        
        controlLayout->addSpacing(10);
        
        // Selection mode
        auto* selectionLayout = new QHBoxLayout();
        selectionLayout->addWidget(new QLabel("Selection:"));
        m_selectionModeCombo = new QComboBox();
        m_selectionModeCombo->addItems({"None", "Single", "Multiple"});
        m_selectionModeCombo->setCurrentIndex(1); // Single
        selectionLayout->addWidget(m_selectionModeCombo);
        controlLayout->addLayout(selectionLayout);
        
        // Feature toggles
        m_sortingCheckBox = new QCheckBox("Enable Sorting");
        m_sortingCheckBox->setChecked(true);
        controlLayout->addWidget(m_sortingCheckBox);
        
        m_filteringCheckBox = new QCheckBox("Enable Filtering");
        m_filteringCheckBox->setChecked(true);
        controlLayout->addWidget(m_filteringCheckBox);
        
        m_paginationCheckBox = new QCheckBox("Enable Pagination");
        m_paginationCheckBox->setChecked(true);
        controlLayout->addWidget(m_paginationCheckBox);
        
        m_alternatingRowsCheckBox = new QCheckBox("Alternating Row Colors");
        m_alternatingRowsCheckBox->setChecked(true);
        controlLayout->addWidget(m_alternatingRowsCheckBox);
        
        m_gridCheckBox = new QCheckBox("Show Grid");
        m_gridCheckBox->setChecked(true);
        controlLayout->addWidget(m_gridCheckBox);
        
        mainLayout->addWidget(controlGroup);
    }
    
    void setupActionButtons(QVBoxLayout* mainLayout) {
        auto* actionGroup = new QGroupBox("Actions");
        auto* actionLayout = new QVBoxLayout(actionGroup);
        
        auto* addDataButton = new QPushButton("Add Sample Data");
        connect(addDataButton, &QPushButton::clicked, this, &DataTableTestWidget::onAddSampleData);
        actionLayout->addWidget(addDataButton);
        
        auto* clearDataButton = new QPushButton("Clear Data");
        connect(clearDataButton, &QPushButton::clicked, this, &DataTableTestWidget::onClearData);
        actionLayout->addWidget(clearDataButton);
        
        actionLayout->addSpacing(10);
        
        auto* exportCsvButton = new QPushButton("Export to CSV");
        connect(exportCsvButton, &QPushButton::clicked, this, &DataTableTestWidget::onExportCsv);
        actionLayout->addWidget(exportCsvButton);
        
        auto* exportJsonButton = new QPushButton("Export to JSON");
        connect(exportJsonButton, &QPushButton::clicked, this, &DataTableTestWidget::onExportJson);
        actionLayout->addWidget(exportJsonButton);
        
        mainLayout->addWidget(actionGroup);
    }
    
    void setupSampleData() {
        // Define columns
        QList<FluentTableColumn> columns;
        
        FluentTableColumn idColumn;
        idColumn.id = "id";
        idColumn.field = "id";
        idColumn.title = "ID";
        idColumn.width = 60;
        idColumn.alignment = Qt::AlignCenter;
        columns.append(idColumn);
        
        FluentTableColumn nameColumn;
        nameColumn.id = "name";
        nameColumn.field = "name";
        nameColumn.title = "Name";
        nameColumn.width = 150;
        columns.append(nameColumn);
        
        FluentTableColumn emailColumn;
        emailColumn.id = "email";
        emailColumn.field = "email";
        emailColumn.title = "Email";
        emailColumn.width = 200;
        columns.append(emailColumn);
        
        FluentTableColumn ageColumn;
        ageColumn.id = "age";
        ageColumn.field = "age";
        ageColumn.title = "Age";
        ageColumn.width = 80;
        ageColumn.alignment = Qt::AlignCenter;
        columns.append(ageColumn);
        
        FluentTableColumn departmentColumn;
        departmentColumn.id = "department";
        departmentColumn.field = "department";
        departmentColumn.title = "Department";
        departmentColumn.width = 120;
        columns.append(departmentColumn);
        
        FluentTableColumn salaryColumn;
        salaryColumn.id = "salary";
        salaryColumn.field = "salary";
        salaryColumn.title = "Salary";
        salaryColumn.width = 100;
        salaryColumn.alignment = Qt::AlignRight;
        salaryColumn.formatter = [](const QVariant& value) {
            return QString("$%1").arg(value.toInt());
        };
        columns.append(salaryColumn);
        
        m_dataTable->setColumns(columns);
        
        // Add sample data
        addRandomRows(50);
    }
    
    void addRandomRows(int count) {
        QStringList names = {"John Doe", "Jane Smith", "Bob Johnson", "Alice Brown", "Charlie Wilson", 
                            "Diana Davis", "Eve Miller", "Frank Garcia", "Grace Lee", "Henry Taylor"};
        QStringList departments = {"Engineering", "Marketing", "Sales", "HR", "Finance", "Operations"};
        
        QList<FluentTableRow> newRows;
        
        for (int i = 0; i < count; ++i) {
            FluentTableRow row;
            
            int id = QRandomGenerator::global()->bounded(1000, 9999);
            QString name = names[QRandomGenerator::global()->bounded(names.size())];
            QString email = QString("%1@company.com").arg(name.toLower().replace(" ", "."));
            int age = QRandomGenerator::global()->bounded(22, 65);
            QString department = departments[QRandomGenerator::global()->bounded(departments.size())];
            int salary = QRandomGenerator::global()->bounded(40000, 120000);
            
            row.data["id"] = id;
            row.data["name"] = name;
            row.data["email"] = email;
            row.data["age"] = age;
            row.data["department"] = department;
            row.data["salary"] = salary;
            
            // Add some color variation
            if (salary > 80000) {
                row.backgroundColor = QColor(200, 255, 200); // Light green for high salary
            } else if (age < 30) {
                row.textColor = QColor(0, 100, 200); // Blue for young employees
            }
            
            newRows.append(row);
        }
        
        // Add rows to existing data
        QList<FluentTableRow> currentData = m_dataTable->data();
        currentData.append(newRows);
        m_dataTable->setData(currentData);
    }
    
    void connectSignals() {
        // Data table signals
        connect(m_dataTable, &FluentDataTable::rowClicked,
                this, &DataTableTestWidget::onRowClicked);
        connect(m_dataTable, &FluentDataTable::rowDoubleClicked,
                this, &DataTableTestWidget::onRowDoubleClicked);
        connect(m_dataTable, &FluentDataTable::selectionChanged,
                this, &DataTableTestWidget::onSelectionChanged);
        connect(m_dataTable, &FluentDataTable::sortingChanged,
                this, &DataTableTestWidget::onSortingChanged);
        connect(m_dataTable, &FluentDataTable::filterChanged,
                this, &DataTableTestWidget::onFilterChanged);
        connect(m_dataTable, &FluentDataTable::pageChanged,
                this, &DataTableTestWidget::onPageChanged);
        connect(m_dataTable, &FluentDataTable::pageSizeChanged,
                this, &DataTableTestWidget::onPageSizeChanged);
        connect(m_dataTable, &FluentDataTable::dataChanged,
                this, &DataTableTestWidget::onDataChanged);
        
        // Control signals
        connect(m_themeButton, &QPushButton::clicked,
                this, &DataTableTestWidget::onThemeToggle);
        connect(m_selectionModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &DataTableTestWidget::onSelectionModeChanged);
        connect(m_sortingCheckBox, &QCheckBox::toggled,
                this, &DataTableTestWidget::onSortingToggle);
        connect(m_filteringCheckBox, &QCheckBox::toggled,
                this, &DataTableTestWidget::onFilteringToggle);
        connect(m_paginationCheckBox, &QCheckBox::toggled,
                this, &DataTableTestWidget::onPaginationToggle);
        connect(m_alternatingRowsCheckBox, &QCheckBox::toggled,
                this, &DataTableTestWidget::onAlternatingRowsToggle);
        connect(m_gridCheckBox, &QCheckBox::toggled,
                this, &DataTableTestWidget::onGridToggle);
        connect(m_globalFilterEdit, &QLineEdit::textChanged,
                this, &DataTableTestWidget::onGlobalFilterChanged);
    }

private:
    // UI components
    QLabel* m_statusLabel{nullptr};
    QPushButton* m_themeButton{nullptr};
    QComboBox* m_selectionModeCombo{nullptr};
    QCheckBox* m_sortingCheckBox{nullptr};
    QCheckBox* m_filteringCheckBox{nullptr};
    QCheckBox* m_paginationCheckBox{nullptr};
    QCheckBox* m_alternatingRowsCheckBox{nullptr};
    QCheckBox* m_gridCheckBox{nullptr};
    QLineEdit* m_globalFilterEdit{nullptr};
    
    // Data table
    FluentDataTable* m_dataTable{nullptr};
    
    // Log
    QTextEdit* m_logEdit{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    DataTableTestWidget window;
    window.show();
    
    return app.exec();
}

#include "DataTableTest.moc"
