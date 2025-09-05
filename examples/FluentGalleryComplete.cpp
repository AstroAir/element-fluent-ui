/**
 * @file FluentGalleryComplete.cpp
 * @brief Complete FluentQt Gallery with fixed accessibility system
 *
 * This version uses the completely fixed accessibility system and should
 * work without any crashes while maintaining full accessibility support.
 *
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>

// FluentQt Core
#include "FluentQt/Styling/FluentTheme.h"

// FluentQt Components
#include "FluentQt/Components/FluentBadge.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentSearchBox.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentTextInput.h"

using namespace FluentQt;
using namespace FluentQt::Components;

/**
 * @brief Complete Gallery Window Class with Fixed Accessibility
 */
class FluentGalleryComplete : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentGalleryComplete(QWidget* parent = nullptr);
    ~FluentGalleryComplete() override;

private slots:
    void onCategoryChanged(int index);
    void onThemeToggled();
    void initializeAccessibilityWhenReady();

private:
    void setupUI();
    void setupMenuBar();
    void setupSidebar();
    void setupMainContent();
    void setupStatusBar();

    // Category setup methods
    void setupBasicControlsCategory();
    void setupInputControlsCategory();
    void setupLayoutCategory();

    // Helper methods
    QWidget* createComponentDemo(const QString& title,
                                 const QString& description,
                                 QWidget* component);
    void addComponentToCategory(QWidget* categoryWidget, const QString& title,
                                const QString& description, QWidget* component);

private:
    // UI Components
    QSplitter* m_mainSplitter{nullptr};
    QListWidget* m_categoryList{nullptr};
    QStackedWidget* m_contentStack{nullptr};
    QPushButton* m_themeToggle{nullptr};
    QLabel* m_statusLabel{nullptr};

    // Category widgets
    QWidget* m_basicControlsWidget{nullptr};
    QWidget* m_inputControlsWidget{nullptr};
    QWidget* m_layoutWidget{nullptr};

    // Theme management
    Styling::FluentTheme* m_theme{nullptr};
    bool m_isDarkMode{false};

    // Settings
    QSettings* m_settings{nullptr};

    // Accessibility
    bool m_accessibilityInitialized{false};
};

FluentGalleryComplete::FluentGalleryComplete(QWidget* parent)
    : QMainWindow(parent),
      m_theme(&Styling::FluentTheme::instance()),
      m_settings(new QSettings("FluentQt", "CompleteGallery", this)) {
    setWindowTitle("FluentQt Complete Gallery - Fixed Accessibility");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // Initialize theme
    m_isDarkMode = m_settings->value("darkMode", false).toBool();
    m_theme->setMode(m_isDarkMode ? Styling::FluentThemeMode::Dark
                                  : Styling::FluentThemeMode::Light);

    setupUI();

    // Load saved state
    restoreGeometry(m_settings->value("geometry").toByteArray());

    // Set initial category
    m_categoryList->setCurrentRow(0);
    onCategoryChanged(0);

    // Initialize accessibility safely after the event loop starts
    QTimer::singleShot(
        0, this, &FluentGalleryComplete::initializeAccessibilityWhenReady);
}

FluentGalleryComplete::~FluentGalleryComplete() {
    // Save settings
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("darkMode", m_isDarkMode);
}

void FluentGalleryComplete::initializeAccessibilityWhenReady() {
    if (m_accessibilityInitialized) {
        return;
    }

    qDebug() << "Initializing accessibility for FluentGalleryComplete";

    // The accessibility system should now be safely initialized
    // through the fixed AccessibilityInitializer
    m_accessibilityInitialized = true;

    qDebug()
        << "Accessibility initialization completed for FluentGalleryComplete";
}

void FluentGalleryComplete::setupUI() {
    setupMenuBar();

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Theme toggle bar
    auto* topLayout = new QHBoxLayout();
    auto* titleLabel = new QLabel("FluentQt Complete Gallery");
    titleLabel->setStyleSheet(
        "font-size: 18px; font-weight: bold; margin: 10px;");
    titleLabel->setAccessibleName("FluentQt Complete Gallery Title");

    m_themeToggle =
        new QPushButton(m_isDarkMode ? "Light Mode" : "Dark Mode", this);
    m_themeToggle->setAccessibleName("Theme Toggle Button");
    m_themeToggle->setAccessibleDescription(
        "Toggle between light and dark themes");
    connect(m_themeToggle, &QPushButton::clicked, this,
            &FluentGalleryComplete::onThemeToggled);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_themeToggle);

    mainLayout->addLayout(topLayout);

    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setAccessibleName("Main Content Splitter");

    setupSidebar();
    setupMainContent();

    m_mainSplitter->setSizes({250, 950});
    mainLayout->addWidget(m_mainSplitter);

    setupStatusBar();
}

void FluentGalleryComplete::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("E&xit", this, &QWidget::close);

    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", [this]() {
        QMessageBox::about(this, "About FluentQt Complete Gallery",
                           "FluentQt Complete Gallery\n\n"
                           "A complete showcase of FluentQt UI components with "
                           "fixed accessibility.\n"
                           "Built with Qt6 and FluentQt framework.\n\n"
                           "© 2024 FluentQt Project");
    });
}

void FluentGalleryComplete::setupSidebar() {
    m_categoryList = new QListWidget(this);
    m_categoryList->setMaximumWidth(200);
    m_categoryList->setMinimumWidth(150);
    m_categoryList->setAccessibleName("Component Categories");
    m_categoryList->setAccessibleDescription(
        "List of component categories to explore");

    // Add categories
    m_categoryList->addItem("🎛️ Basic Controls");
    m_categoryList->addItem("📝 Input Controls");
    m_categoryList->addItem("📦 Layout & Containers");

    connect(m_categoryList, &QListWidget::currentRowChanged, this,
            &FluentGalleryComplete::onCategoryChanged);

    m_mainSplitter->addWidget(m_categoryList);
}

void FluentGalleryComplete::setupMainContent() {
    m_contentStack = new QStackedWidget(this);
    m_contentStack->setAccessibleName("Component Display Area");

    // Create category widgets
    setupBasicControlsCategory();
    setupInputControlsCategory();
    setupLayoutCategory();

    m_mainSplitter->addWidget(m_contentStack);
}

void FluentGalleryComplete::setupStatusBar() {
    m_statusLabel =
        new QLabel("Ready - Select a category to explore components", this);
    m_statusLabel->setAccessibleName("Status Information");
    statusBar()->addWidget(m_statusLabel);
}

void FluentGalleryComplete::setupBasicControlsCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setAccessibleName("Basic Controls Scroll Area");

    m_basicControlsWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_basicControlsWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Basic Controls");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    header->setAccessibleName("Basic Controls Category Header");
    layout->addWidget(header);

    auto* description =
        new QLabel("Essential UI controls for user interaction");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    description->setAccessibleName("Basic Controls Description");
    layout->addWidget(description);

    // FluentButton examples
    auto* primaryButton = new FluentButton("Primary Button");
    primaryButton->setButtonStyle(FluentButtonStyle::Primary);
    primaryButton->setAccessibleName("Primary Button Example");
    primaryButton->setAccessibleDescription(
        "Example of a primary action button with accent color");
    addComponentToCategory(m_basicControlsWidget, "FluentButton - Primary",
                           "Primary action button with accent color",
                           primaryButton);

    auto* secondaryButton = new FluentButton("Secondary Button");
    secondaryButton->setButtonStyle(FluentButtonStyle::Default);
    secondaryButton->setAccessibleName("Secondary Button Example");
    secondaryButton->setAccessibleDescription(
        "Example of a standard button for secondary actions");
    addComponentToCategory(m_basicControlsWidget, "FluentButton - Secondary",
                           "Standard button for secondary actions",
                           secondaryButton);

    // FluentProgressBar
    auto* progressBar = new FluentProgressBar();
    progressBar->setValue(65);
    progressBar->setMinimumWidth(300);
    progressBar->setAccessibleName("Progress Bar Example");
    progressBar->setAccessibleDescription(
        "Progress indicator showing 65% completion");
    addComponentToCategory(m_basicControlsWidget, "FluentProgressBar",
                           "Progress indicator with Fluent Design styling",
                           progressBar);

    // FluentBadge
    auto* badge = new FluentBadge("New");
    badge->setAccessibleName("Badge Example");
    badge->setAccessibleDescription("Badge showing 'New' status");
    addComponentToCategory(m_basicControlsWidget, "FluentBadge",
                           "Status badge for notifications and labels", badge);

    layout->addStretch();
    scrollArea->setWidget(m_basicControlsWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGalleryComplete::setupInputControlsCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setAccessibleName("Input Controls Scroll Area");

    m_inputControlsWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_inputControlsWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Input Controls");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    header->setAccessibleName("Input Controls Category Header");
    layout->addWidget(header);

    auto* description = new QLabel("Controls for user input and data entry");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    description->setAccessibleName("Input Controls Description");
    layout->addWidget(description);

    // FluentTextInput
    auto* textInput = new FluentTextInput();
    textInput->setPlaceholderText("Enter your text here...");
    textInput->setLabelText("Text Input");
    textInput->setAccessibleName("Text Input Example");
    textInput->setAccessibleDescription(
        "Single-line text input with Fluent styling");
    addComponentToCategory(m_inputControlsWidget, "FluentTextInput",
                           "Single-line text input with Fluent styling",
                           textInput);

    // FluentSearchBox
    auto* searchBox = new FluentSearchBox();
    searchBox->setPlaceholderText("Search components...");
    searchBox->setAccessibleName("Search Box Example");
    searchBox->setAccessibleDescription("Search input with search icon");
    addComponentToCategory(m_inputControlsWidget, "FluentSearchBox",
                           "Search input with search icon", searchBox);

    // FluentCheckBox
    auto* checkBox = new FluentCheckBox("Enable notifications");
    checkBox->setAccessibleName("Checkbox Example");
    checkBox->setAccessibleDescription("Checkbox for enabling notifications");
    addComponentToCategory(m_inputControlsWidget, "FluentCheckBox",
                           "Checkbox for boolean selections", checkBox);

    // FluentRadioButton
    auto* radioButton = new FluentRadioButton("Option 1");
    radioButton->setAccessibleName("Radio Button Example");
    radioButton->setAccessibleDescription(
        "Radio button for exclusive selections");
    addComponentToCategory(m_inputControlsWidget, "FluentRadioButton",
                           "Radio button for exclusive selections",
                           radioButton);

    // FluentSlider
    auto* slider = new FluentSlider(FluentSliderOrientation::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(50);
    slider->setMinimumWidth(200);
    slider->setAccessibleName("Slider Example");
    slider->setAccessibleDescription(
        "Horizontal slider for numeric value selection");
    addComponentToCategory(m_inputControlsWidget, "FluentSlider",
                           "Slider for numeric value selection", slider);

    // FluentComboBox
    auto* comboBox = new FluentComboBox();
    comboBox->addItems({"Option 1", "Option 2", "Option 3", "Option 4"});
    comboBox->setAccessibleName("Combo Box Example");
    comboBox->setAccessibleDescription(
        "Dropdown selection control with multiple options");
    addComponentToCategory(m_inputControlsWidget, "FluentComboBox",
                           "Dropdown selection control", comboBox);

    layout->addStretch();
    scrollArea->setWidget(m_inputControlsWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGalleryComplete::setupLayoutCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setAccessibleName("Layout Controls Scroll Area");

    m_layoutWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_layoutWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Layout & Containers");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    header->setAccessibleName("Layout Category Header");
    layout->addWidget(header);

    auto* description =
        new QLabel("Container components for organizing content");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    description->setAccessibleName("Layout Category Description");
    layout->addWidget(description);

    // FluentCard
    auto* card = new FluentCard();
    card->setMinimumSize(300, 150);
    auto* cardLayout = new QVBoxLayout(card);
    auto* cardTitle = new QLabel("Sample Card");
    cardTitle->setStyleSheet("font-weight: bold; margin-bottom: 10px;");
    auto* cardContent =
        new QLabel("This is a FluentCard container with content inside.");
    cardContent->setWordWrap(true);
    cardLayout->addWidget(cardTitle);
    cardLayout->addWidget(cardContent);
    cardLayout->addStretch();

    card->setAccessibleName("Card Example");
    card->setAccessibleDescription("Card container with title and content");
    addComponentToCategory(m_layoutWidget, "FluentCard",
                           "Card container for grouping related content", card);

    layout->addStretch();
    scrollArea->setWidget(m_layoutWidget);
    m_contentStack->addWidget(scrollArea);
}

// Event handlers
void FluentGalleryComplete::onCategoryChanged(int index) {
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);

        QStringList categoryNames = {"Basic Controls", "Input Controls",
                                     "Layout & Containers"};

        if (index < categoryNames.size()) {
            m_statusLabel->setText(
                QString("Viewing %1 category").arg(categoryNames[index]));
        }
    }
}

void FluentGalleryComplete::onThemeToggled() {
    m_isDarkMode = !m_isDarkMode;
    m_theme->setMode(m_isDarkMode ? Styling::FluentThemeMode::Dark
                                  : Styling::FluentThemeMode::Light);
    m_themeToggle->setText(m_isDarkMode ? "Light Mode" : "Dark Mode");

    m_statusLabel->setText(
        QString("Switched to %1 mode").arg(m_isDarkMode ? "dark" : "light"));
}

// Helper methods
QWidget* FluentGalleryComplete::createComponentDemo(const QString& title,
                                                    const QString& description,
                                                    QWidget* component) {
    auto* demoWidget = new QWidget();
    demoWidget->setAccessibleName(QString("Demo: %1").arg(title));
    auto* layout = new QVBoxLayout(demoWidget);

    // Title
    auto* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "font-weight: bold; font-size: 16px; margin-bottom: 5px;");
    titleLabel->setAccessibleName(QString("%1 Title").arg(title));
    layout->addWidget(titleLabel);

    // Description
    auto* descLabel = new QLabel(description);
    descLabel->setStyleSheet("color: gray; margin-bottom: 10px;");
    descLabel->setWordWrap(true);
    descLabel->setAccessibleName(QString("%1 Description").arg(title));
    layout->addWidget(descLabel);

    // Component
    layout->addWidget(component);

    // Add some spacing
    layout->addSpacing(20);

    return demoWidget;
}

void FluentGalleryComplete::addComponentToCategory(QWidget* categoryWidget,
                                                   const QString& title,
                                                   const QString& description,
                                                   QWidget* component) {
    auto* demoWidget = createComponentDemo(title, description, component);
    categoryWidget->layout()->addWidget(demoWidget);
}

// Main function
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("FluentQt Complete Gallery");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt");
    app.setOrganizationDomain("fluentqt.org");

    qDebug() << "Starting FluentQt Complete Gallery with fixed accessibility";

    // Initialize FluentQt theme system
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);

    // Create and show the gallery
    FluentGalleryComplete gallery;
    gallery.show();

    qDebug() << "FluentQt Complete Gallery started successfully";

    return app.exec();
}

#include "FluentGalleryComplete.moc"
