/**
 * @file FluentGalleryFixed.cpp
 * @brief Fixed FluentQt Gallery without accessibility issues
 * 
 * This version completely bypasses the problematic accessibility system
 * to prevent QBasicTimer threading crashes.
 * 
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QSettings>
#include <QMessageBox>

// FluentQt Core - Only include essential components
#include "FluentQt/Styling/FluentTheme.h"

// FluentQt Components - Only include those that work without accessibility
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentComboBox.h"

using namespace FluentQt;
using namespace FluentQt::Components;

/**
 * @brief Fixed Gallery Window Class
 */
class FluentGalleryFixed : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentGalleryFixed(QWidget* parent = nullptr);
    ~FluentGalleryFixed() override;

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
    
    // Helper methods
    QWidget* createComponentDemo(const QString& title, const QString& description, QWidget* component);
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
    
    // Theme management
    Styling::FluentTheme* m_theme{nullptr};
    bool m_isDarkMode{false};
    
    // Settings
    QSettings* m_settings{nullptr};
};

FluentGalleryFixed::FluentGalleryFixed(QWidget* parent)
    : QMainWindow(parent),
      m_theme(&Styling::FluentTheme::instance()),
      m_settings(new QSettings("FluentQt", "FixedGallery", this)) {
    
    setWindowTitle("FluentQt Fixed Gallery - No Accessibility Issues");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // Initialize theme
    m_isDarkMode = m_settings->value("darkMode", false).toBool();
    m_theme->setMode(m_isDarkMode ? Styling::FluentThemeMode::Dark : Styling::FluentThemeMode::Light);
    
    setupUI();
    
    // Load saved state
    restoreGeometry(m_settings->value("geometry").toByteArray());
    
    // Set initial category
    m_categoryList->setCurrentRow(0);
    onCategoryChanged(0);
}

FluentGalleryFixed::~FluentGalleryFixed() {
    // Save settings
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("darkMode", m_isDarkMode);
}

void FluentGalleryFixed::setupUI() {
    setupMenuBar();
    
    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Theme toggle bar
    auto* topLayout = new QHBoxLayout();
    auto* titleLabel = new QLabel("FluentQt Fixed Gallery");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    
    m_themeToggle = new QPushButton(m_isDarkMode ? "Light Mode" : "Dark Mode", this);
    connect(m_themeToggle, &QPushButton::clicked, this, &FluentGalleryFixed::onThemeToggled);
    
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

void FluentGalleryFixed::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("E&xit", this, &QWidget::close);
    
    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", [this]() {
        QMessageBox::about(this, "About FluentQt Fixed Gallery",
            "FluentQt Fixed Gallery\n\n"
            "A showcase of FluentQt UI components without accessibility issues.\n"
            "Built with Qt6 and FluentQt framework.\n\n"
            "© 2024 FluentQt Project");
    });
}

void FluentGalleryFixed::setupSidebar() {
    m_categoryList = new QListWidget(this);
    m_categoryList->setMaximumWidth(200);
    m_categoryList->setMinimumWidth(150);
    
    // Add categories
    m_categoryList->addItem("🎛️ Basic Controls");
    m_categoryList->addItem("📝 Input Controls");
    
    connect(m_categoryList, &QListWidget::currentRowChanged, this, &FluentGalleryFixed::onCategoryChanged);
    
    m_mainSplitter->addWidget(m_categoryList);
}

void FluentGalleryFixed::setupMainContent() {
    m_contentStack = new QStackedWidget(this);
    
    // Create category widgets
    setupBasicControlsCategory();
    setupInputControlsCategory();
    
    m_mainSplitter->addWidget(m_contentStack);
}

void FluentGalleryFixed::setupStatusBar() {
    m_statusLabel = new QLabel("Ready - Select a category to explore components", this);
    statusBar()->addWidget(m_statusLabel);
}

void FluentGalleryFixed::setupBasicControlsCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    
    m_basicControlsWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_basicControlsWidget);
    layout->setSpacing(20);
    
    // Category header
    auto* header = new QLabel("Basic Controls");
    header->setStyleSheet("font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);
    
    auto* description = new QLabel("Essential UI controls for user interaction");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);
    
    // FluentButton examples
    auto* primaryButton = new FluentButton("Primary Button");
    primaryButton->setButtonStyle(FluentButtonStyle::Primary);
    addComponentToCategory(m_basicControlsWidget, "FluentButton - Primary", 
        "Primary action button with accent color", primaryButton);
    
    auto* secondaryButton = new FluentButton("Secondary Button");
    secondaryButton->setButtonStyle(FluentButtonStyle::Default);
    addComponentToCategory(m_basicControlsWidget, "FluentButton - Secondary", 
        "Standard button for secondary actions", secondaryButton);
    
    // FluentProgressBar
    auto* progressBar = new FluentProgressBar();
    progressBar->setValue(65);
    progressBar->setMinimumWidth(300);
    addComponentToCategory(m_basicControlsWidget, "FluentProgressBar", 
        "Progress indicator with Fluent Design styling", progressBar);
    
    layout->addStretch();
    scrollArea->setWidget(m_basicControlsWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGalleryFixed::setupInputControlsCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    
    m_inputControlsWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_inputControlsWidget);
    layout->setSpacing(20);
    
    // Category header
    auto* header = new QLabel("Input Controls");
    header->setStyleSheet("font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);
    
    auto* description = new QLabel("Controls for user input and data entry");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);
    
    // FluentTextInput
    auto* textInput = new FluentTextInput();
    textInput->setPlaceholderText("Enter your text here...");
    textInput->setLabelText("Text Input");
    addComponentToCategory(m_inputControlsWidget, "FluentTextInput", 
        "Single-line text input with Fluent styling", textInput);
    
    // FluentCheckBox
    auto* checkBox = new FluentCheckBox("Enable notifications");
    addComponentToCategory(m_inputControlsWidget, "FluentCheckBox", 
        "Checkbox for boolean selections", checkBox);
    
    // FluentRadioButton
    auto* radioButton = new FluentRadioButton("Option 1");
    addComponentToCategory(m_inputControlsWidget, "FluentRadioButton", 
        "Radio button for exclusive selections", radioButton);
    
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

// Event handlers
void FluentGalleryFixed::onCategoryChanged(int index) {
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);
        
        QStringList categoryNames = {"Basic Controls", "Input Controls"};
        
        if (index < categoryNames.size()) {
            m_statusLabel->setText(QString("Viewing %1 category").arg(categoryNames[index]));
        }
    }
}

void FluentGalleryFixed::onThemeToggled() {
    m_isDarkMode = !m_isDarkMode;
    m_theme->setMode(m_isDarkMode ? Styling::FluentThemeMode::Dark : Styling::FluentThemeMode::Light);
    m_themeToggle->setText(m_isDarkMode ? "Light Mode" : "Dark Mode");
    
    m_statusLabel->setText(QString("Switched to %1 mode").arg(m_isDarkMode ? "dark" : "light"));
}

// Helper methods
QWidget* FluentGalleryFixed::createComponentDemo(const QString& title, const QString& description, QWidget* component) {
    auto* demoWidget = new QWidget();
    auto* layout = new QVBoxLayout(demoWidget);
    
    // Title
    auto* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; margin-bottom: 5px;");
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

void FluentGalleryFixed::addComponentToCategory(QWidget* categoryWidget, const QString& title, 
                                               const QString& description, QWidget* component) {
    auto* demoWidget = createComponentDemo(title, description, component);
    categoryWidget->layout()->addWidget(demoWidget);
}

// Main function
int main(int argc, char* argv[]) {
    // Set environment variables BEFORE creating QApplication
    qputenv("FLUENTQT_SKIP_PROCESS_DETECTION", "1");
    qputenv("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION", "1");
    qputenv("QT_ACCESSIBILITY", "0");  // Disable Qt accessibility completely
    
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("FluentQt Fixed Gallery");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt");
    app.setOrganizationDomain("fluentqt.org");
    
    // Initialize FluentQt theme system
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
    
    // Create and show the gallery
    FluentGalleryFixed gallery;
    gallery.show();
    
    return app.exec();
}

#include "FluentGalleryFixed.moc"
