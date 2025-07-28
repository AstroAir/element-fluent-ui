// examples/SelectionComponentsExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"

using namespace FluentQt;

/**
 * @brief Comprehensive example demonstrating FluentComboBox and selection components
 * 
 * This example showcases:
 * - Different FluentComboBox styles (Standard, Editable, MultiSelect)
 * - Data binding with custom models
 * - Search and filtering capabilities
 * - Custom item rendering with icons and data
 * - Multi-selection handling
 * - Dynamic item management
 * - Integration with other form components
 * - Real-world usage patterns
 */
class SelectionComponentsExample : public QMainWindow {
    Q_OBJECT

public:
    SelectionComponentsExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupData();
        setupConnections();
        setupTheme();
        populateExampleData();
    }

private slots:
    void onBasicComboChanged(int index);
    void onEditableComboChanged(const QString& text);
    void onMultiSelectChanged();
    void onCountryChanged(int index);
    void onCityChanged(int index);
    void addCustomItem();
    void removeSelectedItem();
    void clearAllItems();
    void exportSelection();
    void importData();
    void toggleTheme();
    void resetToDefaults();
    void showSelectionSummary();

private:
    void setupUI();
    void setupData();
    void setupConnections();
    void setupTheme();
    void populateExampleData();
    void createBasicExamples();
    void createAdvancedExamples();
    void createDataBindingExample();
    void createCustomizationControls();
    void createActionButtons();
    void updateCitiesForCountry(const QString& country);
    void updateSelectionDisplay();
    void updateStatusInfo();

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QVBoxLayout* m_mainLayout{nullptr};
    
    // Basic Examples Group
    QGroupBox* m_basicGroup{nullptr};
    Components::FluentComboBox* m_standardCombo{nullptr};
    Components::FluentComboBox* m_editableCombo{nullptr};
    Components::FluentComboBox* m_multiSelectCombo{nullptr};
    QLabel* m_basicStatusLabel{nullptr};
    
    // Advanced Examples Group
    QGroupBox* m_advancedGroup{nullptr};
    Components::FluentComboBox* m_iconCombo{nullptr};
    Components::FluentComboBox* m_searchCombo{nullptr};
    Components::FluentComboBox* m_customCombo{nullptr};
    QLabel* m_advancedStatusLabel{nullptr};
    
    // Data Binding Example
    QGroupBox* m_dataBindingGroup{nullptr};
    Components::FluentComboBox* m_countryCombo{nullptr};
    Components::FluentComboBox* m_cityCombo{nullptr};
    Components::FluentComboBox* m_categoryCombo{nullptr};
    QLabel* m_locationLabel{nullptr};
    
    // Customization Controls
    QGroupBox* m_customizationGroup{nullptr};
    QSpinBox* m_maxVisibleSpin{nullptr};
    Components::FluentCheckBox* m_animatedCheck{nullptr};
    Components::FluentCheckBox* m_searchEnabledCheck{nullptr};
    Components::FluentCheckBox* m_caseSensitiveCheck{nullptr};
    QSlider* m_dropdownWidthSlider{nullptr};
    
    // Custom Item Management
    QGroupBox* m_itemManagementGroup{nullptr};
    Components::FluentTextInput* m_newItemEdit{nullptr};
    Components::FluentTextInput* m_newItemDataEdit{nullptr};
    QTextEdit* m_selectionDisplay{nullptr};
    
    // Action Buttons
    QHBoxLayout* m_actionLayout{nullptr};
    Components::FluentButton* m_addButton{nullptr};
    Components::FluentButton* m_removeButton{nullptr};
    Components::FluentButton* m_clearButton{nullptr};
    Components::FluentButton* m_exportButton{nullptr};
    Components::FluentButton* m_importButton{nullptr};
    Components::FluentButton* m_themeButton{nullptr};
    Components::FluentButton* m_resetButton{nullptr};
    Components::FluentButton* m_summaryButton{nullptr};
    
    // Data Models
    QStandardItemModel* m_countriesModel{nullptr};
    QStandardItemModel* m_citiesModel{nullptr};
    QStandardItemModel* m_categoriesModel{nullptr};
    
    // Animation
    Animation::FluentAnimator* m_animator{nullptr};
    
    // Data Storage
    QMap<QString, QStringList> m_countryToCities;
    QStringList m_sampleCategories;
    QStringList m_sampleItems;
};

void SelectionComponentsExample::setupUI() {
    setWindowTitle("FluentQt Selection Components Example");
    setMinimumSize(1000, 800);
    
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(16);
    m_mainLayout->setContentsMargins(16, 16, 16, 16);
    
    // Create all UI sections
    createBasicExamples();
    createAdvancedExamples();
    createDataBindingExample();
    createCustomizationControls();
    createActionButtons();
    
    // Add stretch to push everything to the top
    m_mainLayout->addStretch();
    
    // Initialize animator
    m_animator = new Animation::FluentAnimator(this);
}

void SelectionComponentsExample::createBasicExamples() {
    m_basicGroup = new QGroupBox("Basic ComboBox Examples", this);
    auto* layout = new QGridLayout(m_basicGroup);
    
    // Standard ComboBox
    layout->addWidget(new QLabel("Standard ComboBox:"), 0, 0);
    m_standardCombo = new Components::FluentComboBox(Components::FluentComboBoxStyle::Standard, this);
    m_standardCombo->setPlaceholderText("Select an option...");
    layout->addWidget(m_standardCombo, 0, 1);
    
    // Editable ComboBox
    layout->addWidget(new QLabel("Editable ComboBox:"), 1, 0);
    m_editableCombo = new Components::FluentComboBox(Components::FluentComboBoxStyle::Editable, this);
    m_editableCombo->setPlaceholderText("Type or select...");
    m_editableCombo->setSearchEnabled(true);
    layout->addWidget(m_editableCombo, 1, 1);
    
    // Multi-Select ComboBox
    layout->addWidget(new QLabel("Multi-Select ComboBox:"), 2, 0);
    m_multiSelectCombo = new Components::FluentComboBox(Components::FluentComboBoxStyle::MultiSelect, this);
    m_multiSelectCombo->setPlaceholderText("Select multiple items...");
    m_multiSelectCombo->setMultiSelectEnabled(true);
    layout->addWidget(m_multiSelectCombo, 2, 1);
    
    // Status label
    m_basicStatusLabel = new QLabel("Select items to see status updates", this);
    m_basicStatusLabel->setStyleSheet("color: #666; font-style: italic;");
    layout->addWidget(m_basicStatusLabel, 3, 0, 1, 2);
    
    m_mainLayout->addWidget(m_basicGroup);
}

void SelectionComponentsExample::createAdvancedExamples() {
    m_advancedGroup = new QGroupBox("Advanced Features", this);
    auto* layout = new QGridLayout(m_advancedGroup);
    
    // ComboBox with icons
    layout->addWidget(new QLabel("With Icons:"), 0, 0);
    m_iconCombo = new Components::FluentComboBox(this);
    layout->addWidget(m_iconCombo, 0, 1);
    
    // ComboBox with search
    layout->addWidget(new QLabel("With Search:"), 1, 0);
    m_searchCombo = new Components::FluentComboBox(Components::FluentComboBoxStyle::Editable, this);
    m_searchCombo->setSearchEnabled(true);
    m_searchCombo->setPlaceholderText("Search items...");
    layout->addWidget(m_searchCombo, 1, 1);
    
    // Custom styled ComboBox
    layout->addWidget(new QLabel("Custom Items:"), 2, 0);
    m_customCombo = new Components::FluentComboBox(this);
    layout->addWidget(m_customCombo, 2, 1);
    
    // Status label
    m_advancedStatusLabel = new QLabel("Advanced features demonstration", this);
    m_advancedStatusLabel->setStyleSheet("color: #666; font-style: italic;");
    layout->addWidget(m_advancedStatusLabel, 3, 0, 1, 2);
    
    m_mainLayout->addWidget(m_advancedGroup);
}

void SelectionComponentsExample::createDataBindingExample() {
    m_dataBindingGroup = new QGroupBox("Data Binding & Cascading Selection", this);
    auto* layout = new QGridLayout(m_dataBindingGroup);
    
    // Country selection
    layout->addWidget(new QLabel("Country:"), 0, 0);
    m_countryCombo = new Components::FluentComboBox(this);
    m_countryCombo->setPlaceholderText("Select a country...");
    layout->addWidget(m_countryCombo, 0, 1);
    
    // City selection (depends on country)
    layout->addWidget(new QLabel("City:"), 1, 0);
    m_cityCombo = new Components::FluentComboBox(this);
    m_cityCombo->setPlaceholderText("Select a city...");
    m_cityCombo->setEnabled(false);
    layout->addWidget(m_cityCombo, 1, 1);
    
    // Category selection
    layout->addWidget(new QLabel("Category:"), 2, 0);
    m_categoryCombo = new Components::FluentComboBox(Components::FluentComboBoxStyle::MultiSelect, this);
    m_categoryCombo->setPlaceholderText("Select categories...");
    m_categoryCombo->setMultiSelectEnabled(true);
    layout->addWidget(m_categoryCombo, 2, 1);
    
    // Location display
    m_locationLabel = new QLabel("No location selected", this);
    m_locationLabel->setStyleSheet("font-weight: bold; color: #0078d4;");
    layout->addWidget(m_locationLabel, 3, 0, 1, 2);
    
    m_mainLayout->addWidget(m_dataBindingGroup);
}

void SelectionComponentsExample::createCustomizationControls() {
    m_customizationGroup = new QGroupBox("Customization Controls", this);
    auto* layout = new QGridLayout(m_customizationGroup);

    // Max visible items
    layout->addWidget(new QLabel("Max Visible Items:"), 0, 0);
    m_maxVisibleSpin = new QSpinBox(this);
    m_maxVisibleSpin->setRange(3, 20);
    m_maxVisibleSpin->setValue(10);
    layout->addWidget(m_maxVisibleSpin, 0, 1);

    // Animation enabled
    layout->addWidget(new QLabel("Enable Animations:"), 1, 0);
    m_animatedCheck = new Components::FluentCheckBox("Smooth animations", this);
    m_animatedCheck->setChecked(true);
    layout->addWidget(m_animatedCheck, 1, 1);

    // Search enabled
    layout->addWidget(new QLabel("Enable Search:"), 2, 0);
    m_searchEnabledCheck = new Components::FluentCheckBox("Enable search filtering", this);
    m_searchEnabledCheck->setChecked(true);
    layout->addWidget(m_searchEnabledCheck, 2, 1);

    // Case sensitive search
    layout->addWidget(new QLabel("Case Sensitive:"), 3, 0);
    m_caseSensitiveCheck = new Components::FluentCheckBox("Case sensitive search", this);
    m_caseSensitiveCheck->setChecked(false);
    layout->addWidget(m_caseSensitiveCheck, 3, 1);

    // Item Management Section
    m_itemManagementGroup = new QGroupBox("Dynamic Item Management", this);
    auto* itemLayout = new QVBoxLayout(m_itemManagementGroup);

    // Add new item controls
    auto* addItemLayout = new QHBoxLayout();
    m_newItemEdit = new Components::FluentTextInput(this);
    m_newItemEdit->setPlaceholderText("New item text...");
    m_newItemDataEdit = new Components::FluentTextInput(this);
    m_newItemDataEdit->setPlaceholderText("Item data (optional)...");

    addItemLayout->addWidget(new QLabel("Add Item:"));
    addItemLayout->addWidget(m_newItemEdit);
    addItemLayout->addWidget(m_newItemDataEdit);
    itemLayout->addLayout(addItemLayout);

    // Selection display
    itemLayout->addWidget(new QLabel("Current Selection:"));
    m_selectionDisplay = new QTextEdit(this);
    m_selectionDisplay->setMaximumHeight(100);
    m_selectionDisplay->setReadOnly(true);
    m_selectionDisplay->setPlaceholderText("Selection details will appear here...");
    itemLayout->addWidget(m_selectionDisplay);

    layout->addWidget(m_itemManagementGroup, 4, 0, 1, 2);
    m_mainLayout->addWidget(m_customizationGroup);
}

void SelectionComponentsExample::createActionButtons() {
    m_actionLayout = new QHBoxLayout();

    // Item management buttons
    m_addButton = new Components::FluentButton("Add Item", this);
    m_addButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    m_actionLayout->addWidget(m_addButton);

    m_removeButton = new Components::FluentButton("Remove Selected", this);
    m_removeButton->setButtonStyle(Components::FluentButtonStyle::Default);
    m_actionLayout->addWidget(m_removeButton);

    m_clearButton = new Components::FluentButton("Clear All", this);
    m_clearButton->setButtonStyle(Components::FluentButtonStyle::Default);
    m_actionLayout->addWidget(m_clearButton);

    m_actionLayout->addStretch();

    // Data operations
    m_exportButton = new Components::FluentButton("Export Data", this);
    m_exportButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
    m_actionLayout->addWidget(m_exportButton);

    m_importButton = new Components::FluentButton("Import Data", this);
    m_importButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
    m_actionLayout->addWidget(m_importButton);

    m_actionLayout->addStretch();

    // Utility buttons
    m_summaryButton = new Components::FluentButton("Show Summary", this);
    m_summaryButton->setButtonStyle(Components::FluentButtonStyle::Accent);
    m_actionLayout->addWidget(m_summaryButton);

    m_themeButton = new Components::FluentButton("Toggle Theme", this);
    m_themeButton->setButtonStyle(Components::FluentButtonStyle::Hyperlink);
    m_actionLayout->addWidget(m_themeButton);

    m_resetButton = new Components::FluentButton("Reset", this);
    m_resetButton->setButtonStyle(Components::FluentButtonStyle::Outline);
    m_actionLayout->addWidget(m_resetButton);

    m_mainLayout->addLayout(m_actionLayout);
}

void SelectionComponentsExample::setupData() {
    // Initialize data models
    m_countriesModel = new QStandardItemModel(this);
    m_citiesModel = new QStandardItemModel(this);
    m_categoriesModel = new QStandardItemModel(this);

    // Sample country-city data
    m_countryToCities["United States"] = {"New York", "Los Angeles", "Chicago", "Houston", "Phoenix", "Philadelphia"};
    m_countryToCities["United Kingdom"] = {"London", "Manchester", "Birmingham", "Liverpool", "Leeds", "Sheffield"};
    m_countryToCities["Germany"] = {"Berlin", "Hamburg", "Munich", "Cologne", "Frankfurt", "Stuttgart"};
    m_countryToCities["France"] = {"Paris", "Marseille", "Lyon", "Toulouse", "Nice", "Nantes"};
    m_countryToCities["Japan"] = {"Tokyo", "Osaka", "Yokohama", "Nagoya", "Sapporo", "Kobe"};
    m_countryToCities["Canada"] = {"Toronto", "Montreal", "Vancouver", "Calgary", "Edmonton", "Ottawa"};

    // Sample categories
    m_sampleCategories = {
        "Technology", "Business", "Science", "Arts", "Sports", "Travel",
        "Food", "Health", "Education", "Entertainment", "Finance", "Environment"
    };

    // Sample items for various combos
    m_sampleItems = {
        "Apple", "Banana", "Cherry", "Date", "Elderberry", "Fig", "Grape",
        "Honeydew", "Kiwi", "Lemon", "Mango", "Orange", "Papaya", "Quince"
    };
}

void SelectionComponentsExample::populateExampleData() {
    // Populate basic combos
    m_standardCombo->addItems({"Option 1", "Option 2", "Option 3", "Option 4", "Option 5"});
    m_editableCombo->addItems(m_sampleItems);
    m_multiSelectCombo->addItems({"Item A", "Item B", "Item C", "Item D", "Item E", "Item F"});

    // Populate advanced combos with icons and custom data
    for (int i = 0; i < 8; ++i) {
        QString text = QString("Priority %1").arg(i + 1);
        QIcon icon; // In a real app, you'd load actual icons
        QVariant data = i + 1;
        m_iconCombo->addItem(text, icon, data);
    }

    // Populate search combo with many items
    QStringList searchItems;
    for (int i = 0; i < 50; ++i) {
        searchItems << QString("Search Item %1").arg(i + 1, 2, 10, QChar('0'));
    }
    m_searchCombo->addItems(searchItems);

    // Populate custom combo with separators and disabled items
    m_customCombo->addItem("Enabled Item 1");
    m_customCombo->addItem("Enabled Item 2");
    m_customCombo->addSeparator();
    m_customCombo->addItem("Disabled Item");
    m_customCombo->setItemEnabled(m_customCombo->count() - 1, false);
    m_customCombo->addItem("Another Enabled Item");

    // Populate country combo
    for (auto it = m_countryToCities.begin(); it != m_countryToCities.end(); ++it) {
        m_countryCombo->addItem(it.key());
    }

    // Populate category combo
    m_categoryCombo->addItems(m_sampleCategories);
}

void SelectionComponentsExample::setupConnections() {
    // Basic combo connections
    connect(m_standardCombo, QOverload<int>::of(&Components::FluentComboBox::currentIndexChanged),
            this, &SelectionComponentsExample::onBasicComboChanged);

    connect(m_editableCombo, &Components::FluentComboBox::currentTextChanged,
            this, &SelectionComponentsExample::onEditableComboChanged);

    connect(m_multiSelectCombo, &Components::FluentComboBox::selectionChanged,
            this, &SelectionComponentsExample::onMultiSelectChanged);

    // Data binding connections
    connect(m_countryCombo, QOverload<int>::of(&Components::FluentComboBox::currentIndexChanged),
            this, &SelectionComponentsExample::onCountryChanged);

    connect(m_cityCombo, QOverload<int>::of(&Components::FluentComboBox::currentIndexChanged),
            this, &SelectionComponentsExample::onCityChanged);

    // Customization controls
    connect(m_maxVisibleSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        m_standardCombo->setMaxVisibleItems(value);
        m_editableCombo->setMaxVisibleItems(value);
        m_multiSelectCombo->setMaxVisibleItems(value);
        updateStatusInfo();
    });

    connect(m_animatedCheck, &Components::FluentCheckBox::toggled, [this](bool enabled) {
        m_standardCombo->setAnimated(enabled);
        m_editableCombo->setAnimated(enabled);
        m_multiSelectCombo->setAnimated(enabled);
        updateStatusInfo();
    });

    connect(m_searchEnabledCheck, &Components::FluentCheckBox::toggled, [this](bool enabled) {
        m_editableCombo->setSearchEnabled(enabled);
        m_searchCombo->setSearchEnabled(enabled);
        updateStatusInfo();
    });

    connect(m_caseSensitiveCheck, &Components::FluentCheckBox::toggled, [this](bool enabled) {
        m_editableCombo->setCaseSensitiveSearch(enabled);
        m_searchCombo->setCaseSensitiveSearch(enabled);
        updateStatusInfo();
    });

    // Action button connections
    connect(m_addButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::addCustomItem);
    connect(m_removeButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::removeSelectedItem);
    connect(m_clearButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::clearAllItems);
    connect(m_exportButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::exportSelection);
    connect(m_importButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::importData);
    connect(m_themeButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::toggleTheme);
    connect(m_resetButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::resetToDefaults);
    connect(m_summaryButton, &Components::FluentButton::clicked, this, &SelectionComponentsExample::showSelectionSummary);

    // Update displays when selections change
    connect(m_standardCombo, QOverload<int>::of(&Components::FluentComboBox::currentIndexChanged),
            this, &SelectionComponentsExample::updateSelectionDisplay);
    connect(m_multiSelectCombo, &Components::FluentComboBox::selectionChanged,
            this, &SelectionComponentsExample::updateSelectionDisplay);
    connect(m_categoryCombo, &Components::FluentComboBox::selectionChanged,
            this, &SelectionComponentsExample::updateSelectionDisplay);
}

void SelectionComponentsExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
    theme.setAccentColor(Styling::FluentAccentColor::Blue); // Windows blue
}

// Slot implementations
void SelectionComponentsExample::onBasicComboChanged(int index) {
    if (index >= 0) {
        QString text = m_standardCombo->itemText(index);
        QVariant data = m_standardCombo->itemData(index);
        m_basicStatusLabel->setText(QString("Selected: %1 (Index: %2, Data: %3)")
                                   .arg(text).arg(index).arg(data.toString()));

        // Animate the status label
        m_animator->pulseEffect(m_basicStatusLabel);
    } else {
        m_basicStatusLabel->setText("No selection");
    }
}

void SelectionComponentsExample::onEditableComboChanged(const QString& text) {
    m_basicStatusLabel->setText(QString("Editable text: '%1'").arg(text));

    // Show search functionality
    if (m_editableCombo->isSearchEnabled() && !text.isEmpty()) {
        int matchCount = 0;
        for (int i = 0; i < m_editableCombo->count(); ++i) {
            if (m_editableCombo->itemText(i).contains(text, Qt::CaseInsensitive)) {
                matchCount++;
            }
        }
        m_basicStatusLabel->setText(m_basicStatusLabel->text() +
                                   QString(" (%1 matches found)").arg(matchCount));
    }
}

void SelectionComponentsExample::onMultiSelectChanged() {
    QStringList selectedTexts = m_multiSelectCombo->selectedTexts();
    QList<int> selectedIndexes = m_multiSelectCombo->selectedIndexes();

    if (selectedTexts.isEmpty()) {
        m_basicStatusLabel->setText("No items selected");
    } else {
        m_basicStatusLabel->setText(QString("Selected %1 items: %2")
                                   .arg(selectedTexts.count())
                                   .arg(selectedTexts.join(", ")));
    }

    // Animate selection change
    if (!selectedTexts.isEmpty()) {
        m_animator->slideIn(m_basicStatusLabel, QPoint(0, -10));
    }
}

void SelectionComponentsExample::onCountryChanged(int index) {
    if (index >= 0) {
        QString country = m_countryCombo->itemText(index);
        updateCitiesForCountry(country);
        m_cityCombo->setEnabled(true);
        m_cityCombo->setCurrentIndex(-1); // Clear city selection
        m_locationLabel->setText(QString("Country: %1").arg(country));

        // Animate the country selection
        m_animator->slideIn(m_cityCombo, QPoint(-20, 0));
    } else {
        m_cityCombo->clear();
        m_cityCombo->setEnabled(false);
        m_locationLabel->setText("No location selected");
    }
}

void SelectionComponentsExample::onCityChanged(int index) {
    if (index >= 0) {
        QString city = m_cityCombo->itemText(index);
        QString country = m_countryCombo->currentText();
        m_locationLabel->setText(QString("Location: %1, %2").arg(city, country));

        // Animate the location update
        m_animator->fadeIn(m_locationLabel);
    }
}

void SelectionComponentsExample::updateCitiesForCountry(const QString& country) {
    m_cityCombo->clear();

    if (m_countryToCities.contains(country)) {
        const QStringList& cities = m_countryToCities[country];
        m_cityCombo->addItems(cities);

        // Animate the city combo appearing
        m_animator->slideIn(m_cityCombo, QPoint(0, -10));
    }
}

void SelectionComponentsExample::addCustomItem() {
    QString itemText = m_newItemEdit->text().trimmed();
    QString itemData = m_newItemDataEdit->text().trimmed();

    if (itemText.isEmpty()) {
        m_newItemEdit->setFocus();
        m_animator->shakeEffect(m_newItemEdit);
        return;
    }

    // Add to the custom combo
    if (itemData.isEmpty()) {
        m_customCombo->addItem(itemText);
    } else {
        m_customCombo->addItem(itemText, itemData);
    }

    // Clear the input fields
    m_newItemEdit->clear();
    m_newItemDataEdit->clear();

    // Update display and animate
    updateSelectionDisplay();
    m_animator->pulseEffect(m_customCombo);

    // Show success feedback
    m_advancedStatusLabel->setText(QString("Added item: %1").arg(itemText));
    m_advancedStatusLabel->setStyleSheet("color: green;");

    // Reset status after 3 seconds
    QTimer::singleShot(3000, [this]() {
        m_advancedStatusLabel->setText("Advanced features demonstration");
        m_advancedStatusLabel->setStyleSheet("color: #666; font-style: italic;");
    });
}

void SelectionComponentsExample::removeSelectedItem() {
    // Remove from the combo that has current focus or selection
    Components::FluentComboBox* targetCombo = nullptr;

    if (m_standardCombo->hasFocus() && m_standardCombo->currentIndex() >= 0) {
        targetCombo = m_standardCombo;
    } else if (m_customCombo->hasFocus() && m_customCombo->currentIndex() >= 0) {
        targetCombo = m_customCombo;
    } else if (m_multiSelectCombo->selectedIndexes().count() > 0) {
        targetCombo = m_multiSelectCombo;
    }

    if (targetCombo) {
        if (targetCombo == m_multiSelectCombo) {
            // Remove all selected items from multi-select
            QList<int> selectedIndexes = targetCombo->selectedIndexes();
            std::sort(selectedIndexes.rbegin(), selectedIndexes.rend()); // Remove from end to start

            for (int index : selectedIndexes) {
                targetCombo->removeItem(index);
            }

            m_basicStatusLabel->setText(QString("Removed %1 items").arg(selectedIndexes.count()));
        } else {
            // Remove current item from single-select combo
            int currentIndex = targetCombo->currentIndex();
            QString itemText = targetCombo->itemText(currentIndex);
            targetCombo->removeItem(currentIndex);

            if (targetCombo == m_standardCombo) {
                m_basicStatusLabel->setText(QString("Removed: %1").arg(itemText));
            } else {
                m_advancedStatusLabel->setText(QString("Removed: %1").arg(itemText));
            }
        }

        // Animate the removal
        m_animator->slideOut(targetCombo, QPoint(0, -10));

        updateSelectionDisplay();
    } else {
        // No selection to remove
        m_basicStatusLabel->setText("No item selected to remove");
        m_basicStatusLabel->setStyleSheet("color: orange;");

        QTimer::singleShot(2000, [this]() {
            m_basicStatusLabel->setStyleSheet("color: #666; font-style: italic;");
        });
    }
}

void SelectionComponentsExample::clearAllItems() {
    // Clear all combos
    m_standardCombo->clear();
    m_editableCombo->clear();
    m_multiSelectCombo->clear();
    m_iconCombo->clear();
    m_searchCombo->clear();
    m_customCombo->clear();
    m_countryCombo->clear();
    m_cityCombo->clear();
    m_categoryCombo->clear();

    // Reset states
    m_cityCombo->setEnabled(false);
    m_locationLabel->setText("No location selected");
    m_basicStatusLabel->setText("All items cleared");
    m_advancedStatusLabel->setText("All items cleared");

    // Animate the clearing
    m_animator->fadeOut(m_basicGroup);
    m_animator->fadeOut(m_advancedGroup);
    m_animator->fadeOut(m_dataBindingGroup);

    // Fade back in after a short delay
    QTimer::singleShot(300, [this]() {
        m_animator->fadeIn(m_basicGroup);
        m_animator->fadeIn(m_advancedGroup);
        m_animator->fadeIn(m_dataBindingGroup);

        // Repopulate with default data
        populateExampleData();
    });

    updateSelectionDisplay();
}

void SelectionComponentsExample::exportSelection() {
    QJsonObject exportData;

    // Export current selections
    exportData["standardCombo"] = QJsonObject{
        {"currentIndex", m_standardCombo->currentIndex()},
        {"currentText", m_standardCombo->currentText()},
        {"currentData", m_standardCombo->currentData().toString()}
    };

    exportData["editableCombo"] = QJsonObject{
        {"currentText", m_editableCombo->currentText()}
    };

    QJsonArray multiSelectArray;
    for (const QString& text : m_multiSelectCombo->selectedTexts()) {
        multiSelectArray.append(text);
    }
    exportData["multiSelectCombo"] = multiSelectArray;

    exportData["location"] = QJsonObject{
        {"country", m_countryCombo->currentText()},
        {"city", m_cityCombo->currentText()}
    };

    QJsonArray categoriesArray;
    for (const QString& category : m_categoryCombo->selectedTexts()) {
        categoriesArray.append(category);
    }
    exportData["categories"] = categoriesArray;

    // Convert to formatted JSON string
    QJsonDocument doc(exportData);
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    // Display in selection display area
    m_selectionDisplay->setPlainText(jsonString);

    // Show success message
    m_basicStatusLabel->setText("Selection data exported to display area");
    m_basicStatusLabel->setStyleSheet("color: green;");

    // Animate the export
    m_animator->pulseEffect(m_selectionDisplay);

    QTimer::singleShot(3000, [this]() {
        m_basicStatusLabel->setStyleSheet("color: #666; font-style: italic;");
    });
}

void SelectionComponentsExample::importData() {
    // Sample import data - in a real app, this would come from a file or API
    QJsonObject sampleData{
        {"fruits", QJsonArray{"Apple", "Banana", "Cherry", "Date", "Elderberry"}},
        {"colors", QJsonArray{"Red", "Green", "Blue", "Yellow", "Purple"}},
        {"countries", QJsonArray{"USA", "UK", "Germany", "France", "Japan"}},
        {"categories", QJsonArray{"Technology", "Science", "Arts", "Sports"}}
    };

    // Clear existing items
    m_editableCombo->clear();
    m_searchCombo->clear();

    // Import fruits to editable combo
    QJsonArray fruits = sampleData["fruits"].toArray();
    for (const QJsonValue& fruit : fruits) {
        m_editableCombo->addItem(fruit.toString());
    }

    // Import colors to search combo
    QJsonArray colors = sampleData["colors"].toArray();
    for (const QJsonValue& color : colors) {
        m_searchCombo->addItem(color.toString());
    }

    // Show success message
    m_advancedStatusLabel->setText("Sample data imported successfully");
    m_advancedStatusLabel->setStyleSheet("color: green;");

    // Animate the import
    m_animator->slideIn(m_editableCombo, QPoint(-20, 0));
    m_animator->slideIn(m_searchCombo, QPoint(20, 0));

    updateSelectionDisplay();

    QTimer::singleShot(3000, [this]() {
        m_advancedStatusLabel->setText("Advanced features demonstration");
        m_advancedStatusLabel->setStyleSheet("color: #666; font-style: italic;");
    });
}

void SelectionComponentsExample::toggleTheme() {
    auto& theme = Styling::FluentTheme::instance();
    auto newMode = (theme.mode() == Styling::FluentThemeMode::Light)
        ? Styling::FluentThemeMode::Dark
        : Styling::FluentThemeMode::Light;
    theme.setMode(newMode);

    // Update button text
    m_themeButton->setText(newMode == Styling::FluentThemeMode::Dark ? "Light Theme" : "Dark Theme");

    // Animate theme change
    m_animator->fadeOut(m_centralWidget);
    QTimer::singleShot(200, [this]() {
        m_animator->fadeIn(m_centralWidget);
    });
}

void SelectionComponentsExample::resetToDefaults() {
    // Reset all customization controls
    m_maxVisibleSpin->setValue(10);
    m_animatedCheck->setChecked(true);
    m_searchEnabledCheck->setChecked(true);
    m_caseSensitiveCheck->setChecked(false);

    // Clear input fields
    m_newItemEdit->clear();
    m_newItemDataEdit->clear();

    // Reset selections
    m_standardCombo->setCurrentIndex(-1);
    m_editableCombo->setCurrentText("");
    m_multiSelectCombo->setSelectedIndexes({});
    m_countryCombo->setCurrentIndex(-1);
    m_cityCombo->setCurrentIndex(-1);
    m_categoryCombo->setSelectedIndexes({});

    // Reset status labels
    m_basicStatusLabel->setText("Reset to defaults completed");
    m_advancedStatusLabel->setText("Advanced features demonstration");
    m_locationLabel->setText("No location selected");

    // Animate reset
    m_animator->pulseEffect(m_customizationGroup);

    updateSelectionDisplay();
}

void SelectionComponentsExample::showSelectionSummary() {
    QString summary = "=== SELECTION SUMMARY ===\n\n";

    // Basic selections
    summary += "BASIC SELECTIONS:\n";
    summary += QString("• Standard: %1 (Index: %2)\n")
               .arg(m_standardCombo->currentText())
               .arg(m_standardCombo->currentIndex());
    summary += QString("• Editable: %1\n").arg(m_editableCombo->currentText());
    summary += QString("• Multi-Select: %1 items selected\n").arg(m_multiSelectCombo->selectedTexts().count());

    // Advanced selections
    summary += "\nADVANCED SELECTIONS:\n";
    summary += QString("• Icon Combo: %1\n").arg(m_iconCombo->currentText());
    summary += QString("• Search Combo: %1\n").arg(m_searchCombo->currentText());
    summary += QString("• Custom Combo: %1\n").arg(m_customCombo->currentText());

    // Location data
    summary += "\nLOCATION DATA:\n";
    summary += QString("• Country: %1\n").arg(m_countryCombo->currentText());
    summary += QString("• City: %1\n").arg(m_cityCombo->currentText());
    summary += QString("• Categories: %1\n").arg(m_categoryCombo->selectedTexts().join(", "));

    // Configuration
    summary += "\nCONFIGURATION:\n";
    summary += QString("• Max Visible Items: %1\n").arg(m_maxVisibleSpin->value());
    summary += QString("• Animations: %1\n").arg(m_animatedCheck->isChecked() ? "Enabled" : "Disabled");
    summary += QString("• Search: %1\n").arg(m_searchEnabledCheck->isChecked() ? "Enabled" : "Disabled");
    summary += QString("• Case Sensitive: %1\n").arg(m_caseSensitiveCheck->isChecked() ? "Yes" : "No");

    // Display summary
    m_selectionDisplay->setPlainText(summary);

    // Animate summary display
    m_animator->slideIn(m_selectionDisplay, QPoint(0, 20));
}

void SelectionComponentsExample::updateSelectionDisplay() {
    QString display = "CURRENT SELECTIONS:\n\n";

    if (m_standardCombo->currentIndex() >= 0) {
        display += QString("Standard: %1\n").arg(m_standardCombo->currentText());
    }

    if (!m_editableCombo->currentText().isEmpty()) {
        display += QString("Editable: %1\n").arg(m_editableCombo->currentText());
    }

    QStringList multiSelected = m_multiSelectCombo->selectedTexts();
    if (!multiSelected.isEmpty()) {
        display += QString("Multi-Select: %1\n").arg(multiSelected.join(", "));
    }

    if (m_countryCombo->currentIndex() >= 0) {
        display += QString("Country: %1\n").arg(m_countryCombo->currentText());
    }

    if (m_cityCombo->currentIndex() >= 0) {
        display += QString("City: %1\n").arg(m_cityCombo->currentText());
    }

    QStringList categories = m_categoryCombo->selectedTexts();
    if (!categories.isEmpty()) {
        display += QString("Categories: %1\n").arg(categories.join(", "));
    }

    if (display == "CURRENT SELECTIONS:\n\n") {
        display = "No selections made yet...";
    }

    m_selectionDisplay->setPlainText(display);
}

void SelectionComponentsExample::updateStatusInfo() {
    // This method can be used to update status information when settings change
    QString info = QString("Settings updated - Max visible: %1, Animations: %2")
                   .arg(m_maxVisibleSpin->value())
                   .arg(m_animatedCheck->isChecked() ? "On" : "Off");

    m_basicStatusLabel->setText(info);
    m_basicStatusLabel->setStyleSheet("color: blue;");

    QTimer::singleShot(2000, [this]() {
        m_basicStatusLabel->setStyleSheet("color: #666; font-style: italic;");
    });
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    SelectionComponentsExample example;
    example.show();

    return app.exec();
}

#include "SelectionComponentsExample.moc"
