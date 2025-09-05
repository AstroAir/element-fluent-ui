/**
 * @file FluentGallerySimple.cpp
 * @brief Simplified FluentQt Components Gallery - Showcase working components
 *
 * This is a simplified gallery application that demonstrates FluentQt
 * components that are confirmed to be working and properly compiled.
 *
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
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
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

// FluentQt Core
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Styling/FluentTheme.h"

// FluentQt Components - Only include those that are confirmed working
#include "AccessibilityFix.h"
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
 * @brief Simple Gallery Window Class
 */
class FluentGallerySimple : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentGallerySimple(QWidget* parent = nullptr);
    ~FluentGallerySimple() override;

private slots:
    void onCategoryChanged(int index);
    void onThemeToggled();

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
};

FluentGallerySimple::FluentGallerySimple(QWidget* parent)
    : QMainWindow(parent),
      m_theme(&Styling::FluentTheme::instance()),
      m_settings(new QSettings("FluentQt", "SimpleGallery", this)) {
    setWindowTitle("FluentQt Simple Gallery - Working Components");
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
}

FluentGallerySimple::~FluentGallerySimple() {
    // Save settings
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("darkMode", m_isDarkMode);
}

void FluentGallerySimple::setupUI() {
    setupMenuBar();

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Theme toggle bar
    auto* topLayout = new QHBoxLayout();
    auto* titleLabel = new QLabel("FluentQt Simple Gallery");
    titleLabel->setStyleSheet(
        "font-size: 18px; font-weight: bold; margin: 10px;");

    m_themeToggle =
        new QPushButton(m_isDarkMode ? "Light Mode" : "Dark Mode", this);
    connect(m_themeToggle, &QPushButton::clicked, this,
            &FluentGallerySimple::onThemeToggled);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_themeToggle);

    mainLayout->addLayout(topLayout);

    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    setupSidebar();
    setupMainContent();

    m_mainSplitter->setSizes({250, 950});
    mainLayout->addWidget(m_mainSplitter);

    setupStatusBar();
}

void FluentGallerySimple::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("E&xit", this, &QWidget::close);

    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", [this]() {
        QMessageBox::about(this, "About FluentQt Simple Gallery",
                           "FluentQt Simple Gallery\n\n"
                           "A showcase of working FluentQt UI components.\n"
                           "Built with Qt6 and FluentQt framework.\n\n"
                           "© 2024 FluentQt Project");
    });
}

void FluentGallerySimple::setupSidebar() {
    m_categoryList = new QListWidget(this);
    m_categoryList->setMaximumWidth(200);
    m_categoryList->setMinimumWidth(150);

    // Add categories
    m_categoryList->addItem("🎛️ Basic Controls");
    m_categoryList->addItem("📝 Input Controls");
    m_categoryList->addItem("📦 Layout");

    connect(m_categoryList, &QListWidget::currentRowChanged, this,
            &FluentGallerySimple::onCategoryChanged);

    m_mainSplitter->addWidget(m_categoryList);
}

void FluentGallerySimple::setupMainContent() {
    m_contentStack = new QStackedWidget(this);

    // Create category widgets
    setupBasicControlsCategory();
    setupInputControlsCategory();
    setupLayoutCategory();

    m_mainSplitter->addWidget(m_contentStack);
}

void FluentGallerySimple::setupStatusBar() {
    m_statusLabel =
        new QLabel("Ready - Select a category to explore components", this);
    statusBar()->addWidget(m_statusLabel);
}

void FluentGallerySimple::setupBasicControlsCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_basicControlsWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_basicControlsWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Basic Controls");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description =
        new QLabel("Essential UI controls for user interaction");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentButton examples
    auto* primaryButton = new FluentButton("Primary Button");
    primaryButton->setButtonStyle(FluentButtonStyle::Primary);
    addComponentToCategory(m_basicControlsWidget, "FluentButton - Primary",
                           "Primary action button with accent color",
                           primaryButton);

    auto* secondaryButton = new FluentButton("Secondary Button");
    secondaryButton->setButtonStyle(FluentButtonStyle::Default);
    addComponentToCategory(m_basicControlsWidget, "FluentButton - Secondary",
                           "Standard button for secondary actions",
                           secondaryButton);

    // FluentProgressBar
    auto* progressBar = new FluentProgressBar();
    progressBar->setValue(65);
    progressBar->setMinimumWidth(300);
    addComponentToCategory(m_basicControlsWidget, "FluentProgressBar",
                           "Progress indicator with Fluent Design styling",
                           progressBar);

    // FluentBadge
    auto* badge = new FluentBadge("New");
    addComponentToCategory(m_basicControlsWidget, "FluentBadge",
                           "Small status indicator or counter", badge);

    layout->addStretch();
    scrollArea->setWidget(m_basicControlsWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallerySimple::setupInputControlsCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_inputControlsWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_inputControlsWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Input Controls");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description = new QLabel("Controls for user input and data entry");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentTextInput
    auto* textInput = new FluentTextInput();
    textInput->setPlaceholderText("Enter your text here...");
    textInput->setLabelText("Text Input");
    addComponentToCategory(m_inputControlsWidget, "FluentTextInput",
                           "Single-line text input with Fluent styling",
                           textInput);

    // FluentSearchBox
    auto* searchBox = new FluentSearchBox();
    searchBox->setPlaceholderText("Search...");
    addComponentToCategory(m_inputControlsWidget, "FluentSearchBox",
                           "Search input with search icon", searchBox);

    // FluentCheckBox
    auto* checkBox = new FluentCheckBox("Enable notifications");
    addComponentToCategory(m_inputControlsWidget, "FluentCheckBox",
                           "Checkbox for boolean selections", checkBox);

    // FluentRadioButton
    auto* radioButton = new FluentRadioButton("Option 1");
    addComponentToCategory(m_inputControlsWidget, "FluentRadioButton",
                           "Radio button for exclusive selections",
                           radioButton);

    // FluentSlider
    auto* slider = new FluentSlider(FluentSliderOrientation::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(50);
    slider->setMinimumWidth(200);
    addComponentToCategory(m_inputControlsWidget, "FluentSlider",
                           "Slider for numeric value selection", slider);

    // FluentComboBox
    auto* comboBox = new FluentComboBox();
    comboBox->addItems({"Option 1", "Option 2", "Option 3", "Option 4"});
    addComponentToCategory(m_inputControlsWidget, "FluentComboBox",
                           "Dropdown selection control", comboBox);

    layout->addStretch();
    scrollArea->setWidget(m_inputControlsWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallerySimple::setupLayoutCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_layoutWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_layoutWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Layout Components");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description = new QLabel("Components for organizing content");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentCard
    auto* card = new FluentCard(
        "Sample Card",
        "This is a sample card component with title and content.");
    card->setMinimumSize(300, 150);
    addComponentToCategory(m_layoutWidget, "FluentCard",
                           "Card container for grouping related content", card);

    layout->addStretch();
    scrollArea->setWidget(m_layoutWidget);
    m_contentStack->addWidget(scrollArea);
}

// Event handlers
void FluentGallerySimple::onCategoryChanged(int index) {
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);

        QStringList categoryNames = {"Basic Controls", "Input Controls",
                                     "Layout"};

        if (index < categoryNames.size()) {
            m_statusLabel->setText(
                QString("Viewing %1 category").arg(categoryNames[index]));
        }
    }
}

void FluentGallerySimple::onThemeToggled() {
    m_isDarkMode = !m_isDarkMode;
    m_theme->setMode(m_isDarkMode ? Styling::FluentThemeMode::Dark
                                  : Styling::FluentThemeMode::Light);
    m_themeToggle->setText(m_isDarkMode ? "Light Mode" : "Dark Mode");

    m_statusLabel->setText(
        QString("Switched to %1 mode").arg(m_isDarkMode ? "dark" : "light"));
}

// Helper methods
QWidget* FluentGallerySimple::createComponentDemo(const QString& title,
                                                  const QString& description,
                                                  QWidget* component) {
    auto* demoWidget = new QWidget();
    auto* layout = new QVBoxLayout(demoWidget);

    // Title
    auto* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "font-weight: bold; font-size: 16px; margin-bottom: 5px;");
    layout->addWidget(titleLabel);

    // Description
    auto* descLabel = new QLabel(description);
    descLabel->setStyleSheet("color: gray; margin-bottom: 10px;");
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);

    // Component
    layout->addWidget(component);

    // Add some spacing
    layout->addSpacing(20);

    return demoWidget;
}

void FluentGallerySimple::addComponentToCategory(QWidget* categoryWidget,
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
    app.setApplicationName("FluentQt Simple Gallery");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt");
    app.setOrganizationDomain("fluentqt.org");

    // Apply accessibility fix to prevent threading issues
    FluentQt::AccessibilityFix::disableProblematicAccessibility();

    // Initialize FluentQt theme system
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);

    // Create and show the gallery
    FluentGallerySimple gallery;
    gallery.show();

    // Initialize accessibility safely after the event loop starts
    FluentQt::AccessibilityFix::initializeAccessibilitySafely();

    return app.exec();
}

#include "FluentGallerySimple.moc"
