#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentTreeView.h"

struct Customer {
    int id;
    QString name;
    QString email;
    QString phone;
    QString company;
    QString address;
    QString city;
    QString country;
    QString status;  // Active, Inactive, Prospect
    QDate lastContact;
    double totalValue;
    QStringList tags;
};

class CustomerManagementSystem : public QMainWindow {
    Q_OBJECT
public:
    CustomerManagementSystem(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        loadSampleData();
        connectSignals();
        setWindowTitle("Customer Management System - Element Fluent UI Demo");
        resize(1200, 800);
    }

private slots:
    void onCustomerSelected(const QModelIndex& index) {
        if (!index.isValid())
            return;

        auto* item = m_customerModel->itemFromIndex(index);
        if (!item)
            return;

        int customerId = item->data(Qt::UserRole).toInt();
        displayCustomerDetails(customerId);
    }

    void addNewCustomer() {
        clearCustomerForm();
        m_isEditing = false;
        m_saveButton->setText("Add Customer");
        m_customerForm->setEnabled(true);
        m_nameInput->setFocus();
    }

    void editCurrentCustomer() {
        if (m_currentCustomerId == -1)
            return;

        m_isEditing = true;
        m_saveButton->setText("Update Customer");
        m_customerForm->setEnabled(true);
        m_nameInput->setFocus();
    }

    void saveCustomer() {
        if (!validateCustomerForm())
            return;

        Customer customer;
        customer.name = m_nameInput->text();
        customer.email = m_emailInput->text();
        customer.phone = m_phoneInput->text();
        customer.company = m_companyInput->text();
        customer.address = m_addressInput->text();
        customer.city = m_cityInput->text();
        customer.country = m_countryCombo->currentText();
        customer.status = m_statusCombo->currentText();
        customer.lastContact = QDate::currentDate();
        customer.totalValue = m_valueInput->text().toDouble();

        m_saveButton->setLoading(true);
        m_saveButton->setText("Saving...");

        // Simulate save operation
        QTimer::singleShot(1000, [this, customer]() {
            if (m_isEditing) {
                updateCustomer(m_currentCustomerId, customer);
                FluentToastGlobal::showSuccess(
                    "Updated", "Customer updated successfully!");
            } else {
                customer.id = m_nextCustomerId++;
                m_customers.append(customer);
                addCustomerToTree(customer);
                FluentToastGlobal::showSuccess(
                    "Added", "New customer added successfully!");
            }

            m_saveButton->setLoading(false);
            m_saveButton->setText(m_isEditing ? "Update Customer"
                                              : "Add Customer");
            m_customerForm->setEnabled(false);

            refreshCustomerTree();
        });
    }

    void deleteCurrentCustomer() {
        if (m_currentCustomerId == -1)
            return;

        auto reply = QMessageBox::question(
            this, "Confirm Delete",
            "Are you sure you want to delete this customer?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            m_customers.removeIf([this](const Customer& c) {
                return c.id == m_currentCustomerId;
            });
            refreshCustomerTree();
            clearCustomerForm();
            m_currentCustomerId = -1;
            FluentToastGlobal::showInfo("Deleted",
                                        "Customer deleted successfully.");
        }
    }

    void searchCustomers() {
        QString searchTerm = m_searchInput->text().toLower();
        filterCustomerTree(searchTerm);
    }

    void exportCustomers() {
        QString fileName = QFileDialog::getSaveFileName(
            this, "Export Customers", "customers.json", "JSON Files (*.json)");
        if (fileName.isEmpty())
            return;

        m_exportButton->setLoading(true);
        m_exportButton->setText("Exporting...");

        // Simulate export operation
        QTimer::singleShot(1500, [this, fileName]() {
            QJsonArray customersArray;
            for (const auto& customer : m_customers) {
                QJsonObject customerObj;
                customerObj["id"] = customer.id;
                customerObj["name"] = customer.name;
                customerObj["email"] = customer.email;
                customerObj["phone"] = customer.phone;
                customerObj["company"] = customer.company;
                customerObj["address"] = customer.address;
                customerObj["city"] = customer.city;
                customerObj["country"] = customer.country;
                customerObj["status"] = customer.status;
                customerObj["totalValue"] = customer.totalValue;
                customersArray.append(customerObj);
            }

            QJsonDocument doc(customersArray);
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(doc.toJson());
                FluentToastGlobal::showSuccess(
                    "Exported", QString("Exported %1 customers to %2")
                                    .arg(m_customers.size())
                                    .arg(fileName));
            } else {
                FluentToastGlobal::showError("Export Failed",
                                             "Could not write to file.");
            }

            m_exportButton->setLoading(false);
            m_exportButton->setText("Export");
        });
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        auto* layout = new QVBoxLayout(centralWidget);

        // Toolbar
        setupToolbar(layout);

        // Main content splitter
        auto* splitter = new QSplitter(Qt::Horizontal);

        // Left side - Customer list
        setupCustomerList(splitter);

        // Right side - Customer details
        setupCustomerDetails(splitter);

        splitter->setSizes({400, 600});
        layout->addWidget(splitter);

        // Status bar
        setupStatusBar(layout);
    }

    void setupToolbar(QVBoxLayout* layout) {
        auto* toolbarCard = new FluentCard;
        auto* toolbarLayout = new QHBoxLayout;

        auto* titleLabel = new QLabel("Customer Management");
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");

        m_searchInput = new FluentTextInput();
        m_searchInput->setPlaceholderText("Search customers...");
        m_searchInput->setInputType(FluentTextInputType::Search);
        m_searchInput->setMaximumWidth(300);

        auto* addButton = new FluentButton("+ Add Customer");
        addButton->setButtonStyle(FluentButtonStyle::Primary);
        connect(addButton, &FluentButton::clicked, this,
                &CustomerManagementSystem::addNewCustomer);

        m_exportButton = new FluentButton("Export");
        m_exportButton->setButtonStyle(FluentButtonStyle::Subtle);

        toolbarLayout->addWidget(titleLabel);
        toolbarLayout->addStretch();
        toolbarLayout->addWidget(m_searchInput);
        toolbarLayout->addWidget(addButton);
        toolbarLayout->addWidget(m_exportButton);

        toolbarCard->setContentWidget(new QWidget);
        toolbarCard->contentWidget()->setLayout(toolbarLayout);

        layout->addWidget(toolbarCard);
    }

    void setupCustomerList(QSplitter* splitter) {
        auto* listCard = new FluentCard("Customers");
        auto* listLayout = new QVBoxLayout;

        m_customerTree = new FluentTreeView;
        m_customerModel = new QStandardItemModel(this);
        m_customerModel->setHorizontalHeaderLabels(
            {"Name", "Company", "Status", "Value"});

        m_customerTree->setModel(m_customerModel);
        m_customerTree->setAlternatingRowColors(true);
        m_customerTree->setRootIsDecorated(false);
        m_customerTree->setSortingEnabled(true);

        // Configure column widths
        m_customerTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        m_customerTree->header()->setSectionResizeMode(
            1, QHeaderView::ResizeToContents);
        m_customerTree->header()->setSectionResizeMode(
            2, QHeaderView::ResizeToContents);
        m_customerTree->header()->setSectionResizeMode(
            3, QHeaderView::ResizeToContents);

        listLayout->addWidget(m_customerTree);

        listCard->setContentWidget(new QWidget);
        listCard->contentWidget()->setLayout(listLayout);

        splitter->addWidget(listCard);
    }

    void setupCustomerDetails(QSplitter* splitter) {
        auto* detailsCard = new FluentCard("Customer Details");
        auto* detailsLayout = new QVBoxLayout;

        // Customer form
        m_customerForm = new QWidget;
        auto* formLayout = new QGridLayout(m_customerForm);

        // Basic information
        formLayout->addWidget(new QLabel("Name:"), 0, 0);
        m_nameInput = new FluentTextInput();
        formLayout->addWidget(m_nameInput, 0, 1);

        formLayout->addWidget(new QLabel("Email:"), 0, 2);
        m_emailInput = new FluentTextInput();
        m_emailInput->setInputType(FluentTextInputType::Email);
        formLayout->addWidget(m_emailInput, 0, 3);

        formLayout->addWidget(new QLabel("Phone:"), 1, 0);
        m_phoneInput = new FluentTextInput();
        formLayout->addWidget(m_phoneInput, 1, 1);

        formLayout->addWidget(new QLabel("Company:"), 1, 2);
        m_companyInput = new FluentTextInput();
        formLayout->addWidget(m_companyInput, 1, 3);

        formLayout->addWidget(new QLabel("Address:"), 2, 0);
        m_addressInput = new FluentTextInput();
        formLayout->addWidget(m_addressInput, 2, 1, 1, 3);

        formLayout->addWidget(new QLabel("City:"), 3, 0);
        m_cityInput = new FluentTextInput();
        formLayout->addWidget(m_cityInput, 3, 1);

        formLayout->addWidget(new QLabel("Country:"), 3, 2);
        m_countryCombo = new FluentComboBox();
        m_countryCombo->addItems(
            {"USA", "Canada", "UK", "Germany", "France", "Australia", "Japan"});
        formLayout->addWidget(m_countryCombo, 3, 3);

        formLayout->addWidget(new QLabel("Status:"), 4, 0);
        m_statusCombo = new FluentComboBox();
        m_statusCombo->addItems({"Active", "Inactive", "Prospect"});
        formLayout->addWidget(m_statusCombo, 4, 1);

        formLayout->addWidget(new QLabel("Total Value:"), 4, 2);
        m_valueInput = new FluentTextInput();
        m_valueInput->setPlaceholderText("0.00");
        formLayout->addWidget(m_valueInput, 4, 3);

        // Action buttons
        auto* buttonLayout = new QHBoxLayout;

        m_saveButton = new FluentButton("Add Customer");
        m_saveButton->setButtonStyle(FluentButtonStyle::Primary);

        auto* editButton = new FluentButton("Edit");
        editButton->setButtonStyle(FluentButtonStyle::Default);
        connect(editButton, &FluentButton::clicked, this,
                &CustomerManagementSystem::editCurrentCustomer);

        auto* deleteButton = new FluentButton("Delete");
        deleteButton->setButtonStyle(FluentButtonStyle::Subtle);
        connect(deleteButton, &FluentButton::clicked, this,
                &CustomerManagementSystem::deleteCurrentCustomer);

        auto* cancelButton = new FluentButton("Cancel");
        cancelButton->setButtonStyle(FluentButtonStyle::Subtle);
        connect(cancelButton, &FluentButton::clicked, [this]() {
            m_customerForm->setEnabled(false);
            if (m_currentCustomerId != -1) {
                displayCustomerDetails(m_currentCustomerId);
            } else {
                clearCustomerForm();
            }
        });

        buttonLayout->addWidget(m_saveButton);
        buttonLayout->addWidget(editButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(cancelButton);

        detailsLayout->addWidget(m_customerForm);
        detailsLayout->addLayout(buttonLayout);
        detailsLayout->addStretch();

        detailsCard->setContentWidget(new QWidget);
        detailsCard->contentWidget()->setLayout(detailsLayout);

        splitter->addWidget(detailsCard);

        // Initially disable form
        m_customerForm->setEnabled(false);
    }

    void setupStatusBar(QVBoxLayout* layout) {
        auto* statusPanel = new FluentPanel;
        statusPanel->setPanelType(FluentPanelType::Surface);
        statusPanel->setMaximumHeight(30);

        auto* statusLayout = statusPanel->createHorizontalLayout();

        m_statusLabel = new QLabel("Ready");
        m_statusLabel->setStyleSheet("color: gray; font-size: 12px;");

        m_customerCountLabel = new QLabel("0 customers");
        m_customerCountLabel->setStyleSheet("color: gray; font-size: 12px;");

        statusLayout->addWidget(m_statusLabel);
        statusLayout->addStretch();
        statusLayout->addWidget(m_customerCountLabel);

        layout->addWidget(statusPanel);
    }

    void loadSampleData() {
        m_nextCustomerId = 1;
        m_currentCustomerId = -1;
        m_isEditing = false;

        // Add sample customers
        QList<Customer> sampleCustomers = {{1,
                                            "John Smith",
                                            "john.smith@email.com",
                                            "+1-555-0123",
                                            "Tech Corp",
                                            "123 Main St",
                                            "New York",
                                            "USA",
                                            "Active",
                                            QDate::currentDate().addDays(-30),
                                            15000.0,
                                            {"VIP", "Tech"}},
                                           {2,
                                            "Sarah Johnson",
                                            "sarah.j@company.com",
                                            "+1-555-0124",
                                            "Design Studio",
                                            "456 Oak Ave",
                                            "Los Angeles",
                                            "USA",
                                            "Active",
                                            QDate::currentDate().addDays(-15),
                                            8500.0,
                                            {"Design", "Creative"}},
                                           {3,
                                            "Michael Brown",
                                            "m.brown@business.com",
                                            "+1-555-0125",
                                            "Business Solutions",
                                            "789 Pine St",
                                            "Chicago",
                                            "USA",
                                            "Prospect",
                                            QDate::currentDate().addDays(-7),
                                            0.0,
                                            {"Prospect"}},
                                           {4,
                                            "Emily Davis",
                                            "emily.davis@startup.com",
                                            "+1-555-0126",
                                            "Startup Inc",
                                            "321 Elm St",
                                            "Austin",
                                            "USA",
                                            "Active",
                                            QDate::currentDate().addDays(-45),
                                            22000.0,
                                            {"Startup", "Tech"}},
                                           {5,
                                            "David Wilson",
                                            "d.wilson@enterprise.com",
                                            "+1-555-0127",
                                            "Enterprise Corp",
                                            "654 Maple Dr",
                                            "Seattle",
                                            "USA",
                                            "Inactive",
                                            QDate::currentDate().addDays(-90),
                                            45000.0,
                                            {"Enterprise"}}};

        m_customers = sampleCustomers;
        m_nextCustomerId = 6;

        refreshCustomerTree();
    }

    void refreshCustomerTree() {
        m_customerModel->clear();
        m_customerModel->setHorizontalHeaderLabels(
            {"Name", "Company", "Status", "Value"});

        for (const auto& customer : m_customers) {
            addCustomerToTree(customer);
        }

        updateCustomerCount();
    }

    void addCustomerToTree(const Customer& customer) {
        auto* nameItem = new QStandardItem(customer.name);
        auto* companyItem = new QStandardItem(customer.company);
        auto* statusItem = new QStandardItem(customer.status);
        auto* valueItem = new QStandardItem(
            QString("$%1").arg(customer.totalValue, 0, 'f', 2));

        // Set status color
        if (customer.status == "Active") {
            statusItem->setForeground(QColor("#107c10"));
        } else if (customer.status == "Inactive") {
            statusItem->setForeground(QColor("#d83b01"));
        } else {
            statusItem->setForeground(QColor("#0078d4"));
        }

        nameItem->setData(customer.id, Qt::UserRole);

        m_customerModel->appendRow(
            {nameItem, companyItem, statusItem, valueItem});
    }

    void displayCustomerDetails(int customerId) {
        auto it = std::find_if(
            m_customers.begin(), m_customers.end(),
            [customerId](const Customer& c) { return c.id == customerId; });

        if (it != m_customers.end()) {
            m_currentCustomerId = customerId;

            m_nameInput->setText(it->name);
            m_emailInput->setText(it->email);
            m_phoneInput->setText(it->phone);
            m_companyInput->setText(it->company);
            m_addressInput->setText(it->address);
            m_cityInput->setText(it->city);
            m_countryCombo->setCurrentText(it->country);
            m_statusCombo->setCurrentText(it->status);
            m_valueInput->setText(QString::number(it->totalValue, 'f', 2));
        }
    }

    void clearCustomerForm() {
        m_nameInput->clear();
        m_emailInput->clear();
        m_phoneInput->clear();
        m_companyInput->clear();
        m_addressInput->clear();
        m_cityInput->clear();
        m_countryCombo->setCurrentIndex(0);
        m_statusCombo->setCurrentIndex(0);
        m_valueInput->clear();
    }

    bool validateCustomerForm() {
        if (m_nameInput->text().isEmpty()) {
            FluentToastGlobal::showError("Validation Error",
                                         "Customer name is required.");
            m_nameInput->setFocus();
            return false;
        }

        if (m_emailInput->text().isEmpty()) {
            FluentToastGlobal::showError("Validation Error",
                                         "Email address is required.");
            m_emailInput->setFocus();
            return false;
        }

        return true;
    }

    void updateCustomer(int customerId, const Customer& updatedCustomer) {
        auto it = std::find_if(
            m_customers.begin(), m_customers.end(),
            [customerId](const Customer& c) { return c.id == customerId; });

        if (it != m_customers.end()) {
            *it = updatedCustomer;
            it->id = customerId;  // Preserve ID
        }
    }

    void filterCustomerTree(const QString& searchTerm) {
        for (int i = 0; i < m_customerModel->rowCount(); ++i) {
            auto* nameItem = m_customerModel->item(i, 0);
            auto* companyItem = m_customerModel->item(i, 1);

            bool matches = nameItem->text().toLower().contains(searchTerm) ||
                           companyItem->text().toLower().contains(searchTerm);

            m_customerTree->setRowHidden(i, QModelIndex(), !matches);
        }
    }

    void updateCustomerCount() {
        m_customerCountLabel->setText(
            QString("%1 customers").arg(m_customers.size()));
    }

    void connectSignals() {
        connect(m_customerTree, &QTreeView::clicked, this,
                &CustomerManagementSystem::onCustomerSelected);
        connect(m_saveButton, &FluentButton::clicked, this,
                &CustomerManagementSystem::saveCustomer);
        connect(m_exportButton, &FluentButton::clicked, this,
                &CustomerManagementSystem::exportCustomers);

        // Search with debouncing
        auto* searchTimer = new QTimer(this);
        searchTimer->setSingleShot(true);
        searchTimer->setInterval(300);
        connect(searchTimer, &QTimer::timeout, this,
                &CustomerManagementSystem::searchCustomers);

        connect(m_searchInput, &FluentTextInput::textChanged,
                [searchTimer]() { searchTimer->start(); });
    }

    // UI Components
    FluentTreeView* m_customerTree;
    QStandardItemModel* m_customerModel;
    FluentTextInput* m_searchInput;
    FluentButton* m_exportButton;

    QWidget* m_customerForm;
    FluentTextInput* m_nameInput;
    FluentTextInput* m_emailInput;
    FluentTextInput* m_phoneInput;
    FluentTextInput* m_companyInput;
    FluentTextInput* m_addressInput;
    FluentTextInput* m_cityInput;
    FluentComboBox* m_countryCombo;
    FluentComboBox* m_statusCombo;
    FluentTextInput* m_valueInput;
    FluentButton* m_saveButton;

    QLabel* m_statusLabel;
    QLabel* m_customerCountLabel;

    // Data
    QList<Customer> m_customers;
    int m_nextCustomerId;
    int m_currentCustomerId;
    bool m_isEditing;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    CustomerManagementSystem cms;
    cms.show();

    return app.exec();
}

#include "main.moc"
