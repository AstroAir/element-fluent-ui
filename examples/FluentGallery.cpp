/**
 * @file FluentGallery.cpp
 * @brief Complete FluentQt Components Gallery - Showcase all available
 * components
 *
 * This is a comprehensive gallery application that demonstrates all FluentQt
 * components organized by categories with interactive examples, theme
 * switching, and search functionality.
 *
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#include <QApplication>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QCursor>
#include <QDate>
#include <QDateTime>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPoint>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

// FluentQt Core
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Styling/FluentTheme.h"

// FluentQt Components - Only include those that are currently compiled
#include "AccessibilityFix.h"
#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Components/FluentBadge.h"
#include "FluentQt/Components/FluentBasicCarousel.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentCircularProgress.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentContextMenu.h"
#include "FluentQt/Components/FluentDatePicker.h"
#include "FluentQt/Components/FluentDotProgress.h"
#include "FluentQt/Components/FluentFilePicker.h"
#include "FluentQt/Components/FluentImageView.h"
#include "FluentQt/Components/FluentIndicatorCarousel.h"
#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentResizable.h"
#include "FluentQt/Components/FluentRichTextEditor.h"
#include "FluentQt/Components/FluentSearchBox.h"
#include "FluentQt/Components/FluentSelect.h"
#include "FluentQt/Components/FluentSelectDropdown.h"
#include "FluentQt/Components/FluentSelectItem.h"
#include "FluentQt/Components/FluentSheet.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentSplitter.h"
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentTimelineItem.h"
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentToastManager.h"
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Components/FluentTouchCarousel.h"
#include "FluentQt/Components/FluentTreeView.h"

using namespace FluentQt;
using namespace FluentQt::Components;

/**
 * @brief Main Gallery Window Class
 */
class FluentGallery : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentGallery(QWidget* parent = nullptr);
    ~FluentGallery() override;

private slots:
    void onCategoryChanged(int index);
    void onSearchTextChanged(const QString& text);
    void onThemeToggled();
    void showComponentCode(const QString& componentName);
    void resetAllDemos();

private:
    void setupUI();
    void setupMenuBar();
    void setupSidebar();
    void setupMainContent();
    void setupStatusBar();

    // Category setup methods
    void setupBasicControlsCategory();
    void setupInputControlsCategory();
    void setupLayoutContainersCategory();
    void setupDataDisplayCategory();
    void setupFeedbackCategory();
    void setupNavigationCategory();
    void setupAdvancedCategory();
    void setupMediaCategory();

    // Helper methods
    QWidget* createComponentDemo(const QString& title,
                                 const QString& description,
                                 QWidget* component);
    QWidget* createCodeExample(const QString& code);
    void addComponentToCategory(QWidget* categoryWidget, const QString& title,
                                const QString& description, QWidget* component,
                                const QString& code = QString());

private:
    // UI Components
    QSplitter* m_mainSplitter{nullptr};
    QListWidget* m_categoryList{nullptr};
    QStackedWidget* m_contentStack{nullptr};
    QLineEdit* m_searchBox{nullptr};
    QPushButton* m_themeToggle{nullptr};
    QLabel* m_statusLabel{nullptr};

    // Category widgets
    QWidget* m_basicControlsWidget{nullptr};
    QWidget* m_inputControlsWidget{nullptr};
    QWidget* m_layoutContainersWidget{nullptr};
    QWidget* m_dataDisplayWidget{nullptr};
    QWidget* m_feedbackWidget{nullptr};
    QWidget* m_navigationWidget{nullptr};
    QWidget* m_advancedWidget{nullptr};
    QWidget* m_mediaWidget{nullptr};

    // Theme management
    Styling::FluentTheme* m_theme{nullptr};
    bool m_isDarkMode{false};

    // Settings
    QSettings* m_settings{nullptr};

    // Component tracking
    QList<QWidget*> m_allComponents;
    QMap<QString, QWidget*> m_componentMap;
};

FluentGallery::FluentGallery(QWidget* parent)
    : QMainWindow(parent),
      m_theme(&Styling::FluentTheme::instance()),
      m_settings(new QSettings("FluentQt", "Gallery", this)) {
    setWindowTitle("FluentQt Components Gallery - Complete Showcase");
    setMinimumSize(1400, 900);
    resize(1600, 1000);

    // Initialize theme
    m_isDarkMode = m_settings->value("darkMode", false).toBool();
    m_theme->setMode(m_isDarkMode ? Styling::FluentThemeMode::Dark
                                  : Styling::FluentThemeMode::Light);

    setupUI();

    // Load saved state
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());

    // Set initial category
    m_categoryList->setCurrentRow(0);
    onCategoryChanged(0);
}

FluentGallery::~FluentGallery() {
    // Save settings
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
    m_settings->setValue("darkMode", m_isDarkMode);
}

void FluentGallery::setupUI() {
    setupMenuBar();

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Search bar
    auto* searchLayout = new QHBoxLayout();
    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("Search components...");
    connect(m_searchBox, &QLineEdit::textChanged, this,
            &FluentGallery::onSearchTextChanged);

    m_themeToggle =
        new QPushButton(m_isDarkMode ? "Light Mode" : "Dark Mode", this);
    connect(m_themeToggle, &QPushButton::clicked, this,
            &FluentGallery::onThemeToggled);

    searchLayout->addWidget(new QLabel("Search:", this));
    searchLayout->addWidget(m_searchBox);
    searchLayout->addStretch();
    searchLayout->addWidget(m_themeToggle);

    mainLayout->addLayout(searchLayout);

    // Main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    setupSidebar();
    setupMainContent();

    m_mainSplitter->setSizes({300, 1100});
    mainLayout->addWidget(m_mainSplitter);

    setupStatusBar();
}

void FluentGallery::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Reset All Demos", this,
                        &FluentGallery::resetAllDemos);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &QWidget::close);

    auto* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("Toggle &Theme", QKeySequence("Ctrl+T"), this,
                        &FluentGallery::onThemeToggled);

    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About FluentQt", [this]() {
        QMessageBox::about(
            this, "About FluentQt Gallery",
            "FluentQt Components Gallery\n\n"
            "A comprehensive showcase of all FluentQt UI components.\n"
            "Built with Qt6 and FluentQt framework.\n\n"
            "© 2024 FluentQt Project");
    });
}

void FluentGallery::setupSidebar() {
    m_categoryList = new QListWidget(this);
    m_categoryList->setMaximumWidth(250);
    m_categoryList->setMinimumWidth(200);

    // Add categories
    m_categoryList->addItem("🎛️ Basic Controls");
    m_categoryList->addItem("📝 Input Controls");
    m_categoryList->addItem("📦 Layout & Containers");
    m_categoryList->addItem("📊 Data Display");
    m_categoryList->addItem("💬 Feedback");
    m_categoryList->addItem("🧭 Navigation");
    m_categoryList->addItem("⚡ Advanced");
    m_categoryList->addItem("🎨 Media");

    connect(m_categoryList, &QListWidget::currentRowChanged, this,
            &FluentGallery::onCategoryChanged);

    m_mainSplitter->addWidget(m_categoryList);
}

void FluentGallery::setupMainContent() {
    m_contentStack = new QStackedWidget(this);

    // Create category widgets
    setupBasicControlsCategory();
    setupInputControlsCategory();
    setupLayoutContainersCategory();
    setupDataDisplayCategory();
    setupFeedbackCategory();
    setupNavigationCategory();
    setupAdvancedCategory();
    setupMediaCategory();

    m_mainSplitter->addWidget(m_contentStack);
}

void FluentGallery::setupStatusBar() {
    m_statusLabel =
        new QLabel("Ready - Select a category to explore components", this);
    statusBar()->addWidget(m_statusLabel);

    auto* componentCount = new QLabel(
        QString("Total Components: %1").arg(m_allComponents.size()), this);
    statusBar()->addPermanentWidget(componentCount);
}

void FluentGallery::setupBasicControlsCategory() {
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
    addComponentToCategory(
        m_basicControlsWidget, "FluentButton - Primary",
        "Primary action button with accent color", primaryButton,
        "auto* button = new FluentButton(\"Primary Button\");\n"
        "button->setButtonStyle(FluentButtonStyle::Primary);");

    auto* secondaryButton = new FluentButton("Secondary Button");
    secondaryButton->setButtonStyle(FluentButtonStyle::Default);
    addComponentToCategory(
        m_basicControlsWidget, "FluentButton - Secondary",
        "Standard button for secondary actions", secondaryButton,
        "auto* button = new FluentButton(\"Secondary Button\");\n"
        "button->setButtonStyle(FluentButtonStyle::Default);");

    auto* accentButton = new FluentButton("Accent Button");
    accentButton->setButtonStyle(FluentButtonStyle::Accent);
    addComponentToCategory(
        m_basicControlsWidget, "FluentButton - Accent",
        "Accent button for highlighted actions", accentButton,
        "auto* button = new FluentButton(\"Accent Button\");\n"
        "button->setButtonStyle(FluentButtonStyle::Accent);");

    auto* subtleButton = new FluentButton("Subtle Button");
    subtleButton->setButtonStyle(FluentButtonStyle::Subtle);
    addComponentToCategory(
        m_basicControlsWidget, "FluentButton - Subtle",
        "Subtle button for minimal emphasis", subtleButton,
        "auto* button = new FluentButton(\"Subtle Button\");\n"
        "button->setButtonStyle(FluentButtonStyle::Subtle);");

    // FluentProgressBar
    auto* progressBar = new FluentProgressBar();
    progressBar->setValue(65);
    progressBar->setMinimumWidth(300);
    addComponentToCategory(m_basicControlsWidget, "FluentProgressBar",
                           "Progress indicator with Fluent Design styling",
                           progressBar,
                           "auto* progressBar = new FluentProgressBar();\n"
                           "progressBar->setValue(65);");

    // FluentBadge
    auto* badge = new FluentBadge("New");
    // badge->setBadgeStyle(FluentBadgeStyle::Accent); // API may not exist
    addComponentToCategory(m_basicControlsWidget, "FluentBadge",
                           "Small status indicator or counter", badge,
                           "auto* badge = new FluentBadge(\"New\");");

    layout->addStretch();
    scrollArea->setWidget(m_basicControlsWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallery::setupInputControlsCategory() {
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
    addComponentToCategory(
        m_inputControlsWidget, "FluentTextInput",
        "Single-line text input with Fluent styling", textInput,
        "auto* textInput = new FluentTextInput();\n"
        "textInput->setPlaceholderText(\"Enter your text here...\");\n"
        "textInput->setLabelText(\"Text Input\");");

    // FluentSearchBox
    auto* searchBox = new FluentSearchBox();
    searchBox->setPlaceholderText("Search...");
    addComponentToCategory(m_inputControlsWidget, "FluentSearchBox",
                           "Search input with search icon and suggestions",
                           searchBox,
                           "auto* searchBox = new FluentSearchBox();\n"
                           "searchBox->setPlaceholderText(\"Search...\");");

    // FluentCheckBox
    auto* checkBox = new FluentCheckBox("Enable notifications");
    addComponentToCategory(
        m_inputControlsWidget, "FluentCheckBox",
        "Checkbox for boolean selections", checkBox,
        "auto* checkBox = new FluentCheckBox(\"Enable notifications\");");

    // FluentRadioButton
    auto* radioButton = new FluentRadioButton("Option 1");
    addComponentToCategory(
        m_inputControlsWidget, "FluentRadioButton",
        "Radio button for exclusive selections", radioButton,
        "auto* radioButton = new FluentRadioButton(\"Option 1\");");

    // FluentSlider
    auto* slider = new FluentSlider(FluentSliderOrientation::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(50);
    slider->setMinimumWidth(200);
    addComponentToCategory(
        m_inputControlsWidget, "FluentSlider",
        "Slider for numeric value selection", slider,
        "auto* slider = new "
        "FluentSlider(FluentSliderOrientation::Horizontal);\n"
        "slider->setRange(0, 100);\n"
        "slider->setValue(50);");

    // FluentComboBox
    auto* comboBox = new FluentComboBox();
    comboBox->addItems({"Option 1", "Option 2", "Option 3", "Option 4"});
    addComponentToCategory(
        m_inputControlsWidget, "FluentComboBox", "Dropdown selection control",
        comboBox,
        "auto* comboBox = new FluentComboBox();\n"
        "comboBox->addItems({\"Option 1\", \"Option 2\", \"Option 3\"});");

    layout->addStretch();
    scrollArea->setWidget(m_inputControlsWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallery::setupLayoutContainersCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_layoutContainersWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_layoutContainersWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Layout & Containers");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description =
        new QLabel("Components for organizing and structuring content");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentCard
    auto* card = new FluentCard(
        "Sample Card",
        "This is a sample card component with title and content.");
    card->setMinimumSize(300, 150);
    addComponentToCategory(m_layoutContainersWidget, "FluentCard",
                           "Card container for grouping related content", card,
                           "auto* card = new FluentCard(\"Sample Card\", "
                           "\"This is a sample card component.\");\n"
                           "card->setMinimumSize(300, 150);");

    // FluentTabView
    auto* tabView = new FluentTabView();
    tabView->addTab("Tab 1", new QLabel("Content 1"));
    tabView->addTab("Tab 2", new QLabel("Content 2"));
    tabView->addTab("Tab 3", new QLabel("Content 3"));
    tabView->setMinimumSize(400, 200);
    addComponentToCategory(
        m_layoutContainersWidget, "FluentTabView",
        "Tabbed interface for organizing content", tabView,
        "auto* tabView = new FluentTabView();\n"
        "tabView->addTab(\"Tab 1\", new QLabel(\"Content 1\"));\n"
        "tabView->addTab(\"Tab 2\", new QLabel(\"Content 2\"));");

    // FluentSplitter
    auto* splitter = new FluentSplitter(Qt::Horizontal);
    splitter->addWidget(new QLabel("Left Panel"));
    splitter->addWidget(new QLabel("Right Panel"));
    splitter->resize(400, 100);  // Use resize instead of setMinimumSize
    addComponentToCategory(
        m_layoutContainersWidget, "FluentSplitter",
        "Resizable splitter for dividing content", splitter,
        "auto* splitter = new FluentSplitter(Qt::Horizontal);\n"
        "splitter->addWidget(new QLabel(\"Left Panel\"));\n"
        "splitter->addWidget(new QLabel(\"Right Panel\"));");

    // FluentSheet
    auto* sheet = new FluentSheet();
    sheet->setTitle("Sample Sheet");
    // sheet->setContent(new QLabel("Sheet content goes here")); // API may not
    // exist
    sheet->setMinimumSize(300, 200);
    addComponentToCategory(m_layoutContainersWidget, "FluentSheet",
                           "Modal sheet for displaying content", sheet,
                           "auto* sheet = new FluentSheet();\n"
                           "sheet->setTitle(\"Sample Sheet\");");

    // FluentResizable
    auto* resizableContent =
        new QLabel("Resizable Content\nDrag the corners to resize");
    resizableContent->setAlignment(Qt::AlignCenter);
    resizableContent->setStyleSheet(
        "background-color: lightblue; padding: 20px; border: 2px dashed gray;");
    auto* resizable = new FluentResizable(resizableContent, nullptr);
    resizable->setMinimumSize(QSize(200, 100));
    resizable->resize(250, 120);
    addComponentToCategory(
        m_layoutContainersWidget, "FluentResizable",
        "Resizable container with drag handles", resizable,
        "auto* resizable = new FluentResizable(contentWidget);\n"
        "resizable->setMinimumSize(QSize(200, 100));");

    layout->addStretch();
    scrollArea->setWidget(m_layoutContainersWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallery::setupDataDisplayCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_dataDisplayWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_dataDisplayWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Data Display");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description =
        new QLabel("Components for displaying and visualizing data");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentTreeView
    auto* treeView = new FluentTreeView();
    // Add some sample data - simplified since setModel may not exist
    treeView->setMinimumSize(300, 200);
    addComponentToCategory(m_dataDisplayWidget, "FluentTreeView",
                           "Hierarchical tree view for structured data",
                           treeView, "auto* treeView = new FluentTreeView();");

    // FluentRichTextEditor
    auto* richTextEditor = new FluentRichTextEditor();
    richTextEditor->setPlainText(
        "This is a rich text editor with Fluent Design styling.");
    richTextEditor->setMinimumSize(400, 150);
    addComponentToCategory(
        m_dataDisplayWidget, "FluentRichTextEditor",
        "Rich text editor with formatting capabilities", richTextEditor,
        "auto* richTextEditor = new FluentRichTextEditor();\n"
        "richTextEditor->setPlainText(\"Sample text...\");");

    // FluentImageView
    auto* imageView = new FluentImageView();
    // Create a sample colored pixmap
    QPixmap samplePixmap(200, 150);
    samplePixmap.fill(QColor(100, 150, 200));
    imageView->setPixmap(samplePixmap);
    addComponentToCategory(m_dataDisplayWidget, "FluentImageView",
                           "Image display component with zoom and pan",
                           imageView,
                           "auto* imageView = new FluentImageView();\n"
                           "imageView->setPixmap(pixmap);");

    layout->addStretch();
    scrollArea->setWidget(m_dataDisplayWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallery::setupFeedbackCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_feedbackWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_feedbackWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Feedback");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description =
        new QLabel("Components for providing user feedback and notifications");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentToast
    auto* toastButton = new QPushButton("Show Toast");
    connect(toastButton, &QPushButton::clicked, [this]() {
        auto* toast = FluentToast::createInfo(
            "Sample Toast", "This is a sample toast message!");
        toast->show();
    });
    addComponentToCategory(
        m_feedbackWidget, "FluentToast", "Temporary notification message",
        toastButton,
        "auto* toast = FluentToast::createInfo(\"Title\", \"Message\");\n"
        "toast->show();");

    // FluentTooltip
    auto* tooltipButton = new QPushButton("Hover for Tooltip");
    tooltipButton->setToolTip("This is a tooltip with Fluent styling");
    addComponentToCategory(m_feedbackWidget, "FluentTooltip",
                           "Contextual help text on hover", tooltipButton,
                           "button->setToolTip(\"Help text\");");

    // FluentCircularProgress
    auto* circularProgress = new FluentCircularProgress();
    // circularProgress->setValue(75); // API may not exist
    circularProgress->setMinimumSize(100, 100);
    addComponentToCategory(m_feedbackWidget, "FluentCircularProgress",
                           "Circular progress indicator", circularProgress,
                           "auto* progress = new FluentCircularProgress();");

    // FluentDotProgress
    auto* dotProgress = new FluentDotProgress();
    // dotProgress->setDotCount(5); // API may not exist
    // dotProgress->setCurrentDot(2); // API may not exist
    addComponentToCategory(m_feedbackWidget, "FluentDotProgress",
                           "Dot-based progress indicator", dotProgress,
                           "auto* dotProgress = new FluentDotProgress();");

    layout->addStretch();
    scrollArea->setWidget(m_feedbackWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallery::setupNavigationCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_navigationWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_navigationWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Navigation");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description =
        new QLabel("Components for navigation and menu systems");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentNavigationView
    auto* navView = new FluentNavigationView();
    navView->addMenuItem("Home", QIcon());
    navView->addMenuItem("Documents", QIcon());
    navView->addMenuItem("Settings", QIcon());
    navView->setMinimumSize(200, 200);
    addComponentToCategory(m_navigationWidget, "FluentNavigationView",
                           "Side navigation panel", navView,
                           "auto* navView = new FluentNavigationView();\n"
                           "navView->addMenuItem(\"Home\", QIcon());\n"
                           "navView->addMenuItem(\"Documents\", QIcon());");

    // FluentContextMenu - Temporarily disabled due to linking issues
    auto* contextMenuButton = new QPushButton("Context Menu (Coming Soon)");
    contextMenuButton->setEnabled(false);
    addComponentToCategory(m_navigationWidget, "FluentContextMenu",
                           "Right-click context menu (implementation pending)",
                           contextMenuButton,
                           "// FluentContextMenu implementation pending");

    layout->addStretch();
    scrollArea->setWidget(m_navigationWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallery::setupAdvancedCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_advancedWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_advancedWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Advanced");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description = new QLabel("Advanced and specialized components");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentCalendar
    auto* calendar = new FluentCalendar();
    calendar->setSelectedDate(QDate::currentDate());
    calendar->setMinimumSize(300, 250);
    addComponentToCategory(m_advancedWidget, "FluentCalendar",
                           "Calendar widget with Fluent styling", calendar,
                           "auto* calendar = new FluentCalendar();\n"
                           "calendar->setSelectedDate(QDate::currentDate());");

    // FluentDatePicker
    auto* datePicker = new FluentDatePicker();
    // datePicker->setDate(QDate::currentDate()); // API may not exist
    addComponentToCategory(m_advancedWidget, "FluentDatePicker",
                           "Date selection control", datePicker,
                           "auto* datePicker = new FluentDatePicker();");

    // FluentFilePicker
    auto* filePicker = new FluentFilePicker();
    // filePicker->setFileMode(QFileDialog::ExistingFile); // API may not exist
    addComponentToCategory(m_advancedWidget, "FluentFilePicker",
                           "File selection dialog", filePicker,
                           "auto* filePicker = new FluentFilePicker();");

    // FluentSelect
    auto* select = new FluentSelect();
    select->addItem("Option 1");
    select->addItem("Option 2");
    select->addItem("Option 3");
    addComponentToCategory(m_advancedWidget, "FluentSelect",
                           "Advanced selection control with descriptions",
                           select,
                           "auto* select = new FluentSelect();\n"
                           "select->addItem(\"Option 1\");");

    // FluentTimeline
    auto* timeline = new FluentTimeline();
    timeline->addItem(
        new FluentTimelineItem("Event 1", "First event description"));
    timeline->addItem(
        new FluentTimelineItem("Event 2", "Second event description"));
    timeline->addItem(
        new FluentTimelineItem("Event 3", "Third event description"));
    timeline->setMinimumSize(400, 200);
    addComponentToCategory(m_advancedWidget, "FluentTimeline",
                           "Timeline component for chronological events",
                           timeline,
                           "auto* timeline = new FluentTimeline();\n"
                           "timeline->addItem(new "
                           "FluentTimelineItem(\"Event\", \"Description\"));");

    layout->addStretch();
    scrollArea->setWidget(m_advancedWidget);
    m_contentStack->addWidget(scrollArea);
}

void FluentGallery::setupMediaCategory() {
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    m_mediaWidget = new QWidget();
    auto* layout = new QVBoxLayout(m_mediaWidget);
    layout->setSpacing(20);

    // Category header
    auto* header = new QLabel("Media");
    header->setStyleSheet(
        "font-size: 24px; font-weight: bold; margin: 10px 0;");
    layout->addWidget(header);

    auto* description = new QLabel("Media and carousel components");
    description->setStyleSheet("color: gray; margin-bottom: 20px;");
    layout->addWidget(description);

    // FluentCarousel
    auto* carousel = new FluentCarousel();
    carousel->setMinimumHeight(200);
    for (int i = 1; i <= 5; ++i) {
        auto* item = new QWidget();
        auto* itemLayout = new QVBoxLayout(item);
        auto* label = new QLabel(QString("Slide %1").arg(i));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(
            QString("background-color: %1; color: white; padding: 20px; "
                    "border-radius: 8px;")
                .arg(QColor::fromHsv(i * 60, 180, 200).name()));
        itemLayout->addWidget(label);
        carousel->addItem(item);
    }
    addComponentToCategory(m_mediaWidget, "FluentCarousel",
                           "Basic carousel for content slides", carousel,
                           "auto* carousel = new FluentCarousel();\n"
                           "carousel->addItem(widget);");

    // FluentAutoCarousel
    auto* autoCarousel = new FluentAutoCarousel();
    autoCarousel->setMinimumHeight(150);
    autoCarousel->setAutoPlayInterval(3000);
    for (int i = 1; i <= 3; ++i) {
        auto* item = new QLabel(QString("Auto Slide %1").arg(i));
        item->setAlignment(Qt::AlignCenter);
        item->setStyleSheet(QString("background-color: %1; color: white; "
                                    "padding: 15px; border-radius: 6px;")
                                .arg(QColor::fromHsv(i * 90, 160, 180).name()));
        autoCarousel->addItem(item);
    }
    addComponentToCategory(m_mediaWidget, "FluentAutoCarousel",
                           "Auto-playing carousel", autoCarousel,
                           "auto* autoCarousel = new FluentAutoCarousel();\n"
                           "autoCarousel->setAutoPlayInterval(3000);\n"
                           "autoCarousel->addItem(widget);");

    // FluentBasicCarousel
    auto* basicCarousel = new FluentBasicCarousel();
    basicCarousel->setMinimumHeight(120);
    for (int i = 1; i <= 4; ++i) {
        auto* item = new QLabel(QString("Basic %1").arg(i));
        item->setAlignment(Qt::AlignCenter);
        item->setStyleSheet(QString("background-color: %1; color: white; "
                                    "padding: 10px; border-radius: 4px;")
                                .arg(QColor::fromHsv(i * 45, 140, 160).name()));
        basicCarousel->addItem(item);
    }
    addComponentToCategory(m_mediaWidget, "FluentBasicCarousel",
                           "Basic carousel without auto-play", basicCarousel,
                           "auto* basicCarousel = new FluentBasicCarousel();\n"
                           "basicCarousel->addItem(widget);");

    // FluentIndicatorCarousel
    auto* indicatorCarousel = new FluentIndicatorCarousel();
    indicatorCarousel->setMinimumHeight(140);
    for (int i = 1; i <= 3; ++i) {
        auto* item = new QLabel(QString("Indicator %1").arg(i));
        item->setAlignment(Qt::AlignCenter);
        item->setStyleSheet(
            QString("background-color: %1; color: white; padding: 12px; "
                    "border-radius: 5px;")
                .arg(QColor::fromHsv(i * 120, 150, 170).name()));
        indicatorCarousel->addItem(item);
    }
    addComponentToCategory(
        m_mediaWidget, "FluentIndicatorCarousel",
        "Carousel with page indicators", indicatorCarousel,
        "auto* indicatorCarousel = new FluentIndicatorCarousel();\n"
        "indicatorCarousel->addItem(widget);");

    // FluentTouchCarousel
    auto* touchCarousel = new FluentTouchCarousel();
    touchCarousel->setMinimumHeight(130);
    for (int i = 1; i <= 4; ++i) {
        auto* item = new QLabel(QString("Touch %1").arg(i));
        item->setAlignment(Qt::AlignCenter);
        item->setStyleSheet(QString("background-color: %1; color: white; "
                                    "padding: 11px; border-radius: 4px;")
                                .arg(QColor::fromHsv(i * 75, 130, 150).name()));
        touchCarousel->addItem(item);
    }
    addComponentToCategory(m_mediaWidget, "FluentTouchCarousel",
                           "Touch-optimized carousel", touchCarousel,
                           "auto* touchCarousel = new FluentTouchCarousel();\n"
                           "touchCarousel->addItem(widget);");

    layout->addStretch();
    scrollArea->setWidget(m_mediaWidget);
    m_contentStack->addWidget(scrollArea);
}

// Event handlers
void FluentGallery::onCategoryChanged(int index) {
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);

        QStringList categoryNames = {
            "Basic Controls", "Input Controls", "Layout & Containers",
            "Data Display",   "Feedback",       "Navigation",
            "Advanced",       "Media"};

        if (index < categoryNames.size()) {
            m_statusLabel->setText(
                QString("Viewing %1 category").arg(categoryNames[index]));
        }
    }
}

void FluentGallery::onSearchTextChanged(const QString& text) {
    // Simple search implementation - could be enhanced
    m_statusLabel->setText(
        text.isEmpty() ? "Ready" : QString("Searching for: %1").arg(text));

    // TODO: Implement actual search functionality
    // This would involve filtering components based on the search text
}

void FluentGallery::onThemeToggled() {
    m_isDarkMode = !m_isDarkMode;
    m_theme->setMode(m_isDarkMode ? Styling::FluentThemeMode::Dark
                                  : Styling::FluentThemeMode::Light);
    m_themeToggle->setText(m_isDarkMode ? "Light Mode" : "Dark Mode");

    m_statusLabel->setText(
        QString("Switched to %1 mode").arg(m_isDarkMode ? "dark" : "light"));
}

void FluentGallery::showComponentCode(const QString& componentName) {
    // This would show a dialog with code examples for the component
    QMessageBox::information(
        this, "Component Code",
        QString("Code example for %1 would be shown here.").arg(componentName));
}

void FluentGallery::resetAllDemos() {
    m_statusLabel->setText("All demos reset");
    // Reset any demo states if needed
}

// Helper methods
QWidget* FluentGallery::createComponentDemo(const QString& title,
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

QWidget* FluentGallery::createCodeExample(const QString& code) {
    auto* codeWidget = new QTextEdit();
    codeWidget->setPlainText(code);
    codeWidget->setMaximumHeight(100);
    codeWidget->setStyleSheet(
        "background-color: #f5f5f5; font-family: 'Courier New', monospace; "
        "font-size: 10px;");
    codeWidget->setReadOnly(true);
    return codeWidget;
}

void FluentGallery::addComponentToCategory(QWidget* categoryWidget,
                                           const QString& title,
                                           const QString& description,
                                           QWidget* component,
                                           const QString& code) {
    auto* demoWidget = createComponentDemo(title, description, component);

    if (!code.isEmpty()) {
        auto* codeExample = createCodeExample(code);
        demoWidget->layout()->addWidget(codeExample);
    }

    categoryWidget->layout()->addWidget(demoWidget);

    // Track components
    m_allComponents.append(component);
    m_componentMap[title] = component;
}

// Main function and MOC include
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("FluentQt Gallery");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt");
    app.setOrganizationDomain("fluentqt.org");

    // Apply accessibility fix to prevent threading issues
    FluentQt::AccessibilityFix::disableProblematicAccessibility();

    // Initialize FluentQt theme system
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);

    // Create and show the gallery
    FluentGallery gallery;
    gallery.show();

    // Initialize accessibility safely after the event loop starts
    FluentQt::AccessibilityFix::initializeAccessibilitySafely();

    return app.exec();
}

#include "FluentGallery.moc"
