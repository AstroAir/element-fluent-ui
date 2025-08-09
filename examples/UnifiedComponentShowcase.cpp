// examples/UnifiedComponentShowcase.cpp
// Comprehensive unified example demonstrating all FluentQt components with proper theme switching

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QSettings>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QKeyEvent>
#include <QtGlobal>

// FluentQt Core
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

// FluentQt Components - Basic Controls
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentToggleSwitch.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentSpinBox.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentComboBox.h"

// FluentQt Components - Layout & Navigation
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Components/FluentBreadcrumb.h"
#include "FluentQt/Components/FluentSeparator.h"

// FluentQt Components - Data Display
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Components/FluentRating.h"
#include "FluentQt/Components/FluentBadge.h"
#include "FluentQt/Components/FluentAvatar.h"
#include "FluentQt/Components/FluentTreeView.h"

// FluentQt Components - Feedback
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentToastManager.h"
#include "FluentQt/Components/FluentNotification.h"
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Components/FluentContentDialog.h"

// FluentQt Components - Advanced
#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentDatePicker.h"
#include "FluentQt/Components/FluentTimePicker.h"
#include "FluentQt/Components/FluentColorPicker.h"
#include "FluentQt/Components/FluentRichTextEditor.h"

// Accessibility helper
#include "FluentAccessibilityHelper.h"

// Visual feedback manager
#include "FluentVisualFeedbackManager.h"

using namespace FluentQt;

class UnifiedComponentShowcase : public QMainWindow {
    Q_OBJECT

public:
    UnifiedComponentShowcase(QWidget* parent = nullptr);
    ~UnifiedComponentShowcase() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onThemeChanged();
    void onThemeModeChanged();
    void onAccentColorChanged();
    void showThemeSettings();
    void showAboutDialog();
    void exportThemeSettings();
    void importThemeSettings();
    void resetToDefaults();

private:
    void setupUI();
    void setupThemeManager();
    void setupMenuBar();
    void setupStatusBar();
    void setupMainContent();
    void setupThemeControls();
    
    // Component setup methods
    void setupBasicControlsTab();
    void setupLayoutNavigationTab();
    void setupDataDisplayTab();
    void setupFeedbackTab();
    void setupAdvancedTab();
    void setupInteractionDemoTab();
    
    void updateThemeUI();
    void updateComponentStyles();
    void animateThemeTransition();
    void updateThemeStatusLabel();
    void updateResponsiveLayout();
    void updateComponentResponsiveLayouts(bool isCompact, bool isMedium, bool isLarge);
    void updateGridLayoutForCompact(QGridLayout* layout);
    void updateGridLayoutForMedium(QGridLayout* layout);
    void updateGridLayoutForLarge(QGridLayout* layout);
    void saveSettings();
    void loadSettings();
    
    // Theme management
    Styling::FluentTheme* m_theme{nullptr};
    Animation::FluentAnimator* m_animator{nullptr};
    Examples::FluentAccessibilityHelper* m_accessibilityHelper{nullptr};
    Examples::FluentVisualFeedbackManager* m_feedbackManager{nullptr};
    
    // UI Components
    QTabWidget* m_mainTabs{nullptr};
    QStatusBar* m_statusBar{nullptr};
    QLabel* m_statusLabel{nullptr};
    QLabel* m_themeStatusLabel{nullptr};
    
    // Theme controls
    QWidget* m_themeControlPanel{nullptr};
    Components::FluentButton* m_themeModeButton{nullptr};
    Components::FluentButton* m_accentColorButton{nullptr};
    Components::FluentToggleSwitch* m_highContrastSwitch{nullptr};
    Components::FluentSlider* m_opacitySlider{nullptr};
    
    // Component collections for theme updates
    QList<Components::FluentButton*> m_buttons;
    QList<Components::FluentCard*> m_cards;
    QList<Components::FluentPanel*> m_panels;
    QList<QWidget*> m_allComponents;
    
    // Settings
    QSettings* m_settings{nullptr};
    
    // Animation effects
    QGraphicsOpacityEffect* m_fadeEffect{nullptr};
    QPropertyAnimation* m_fadeAnimation{nullptr};
    
    // State tracking
    bool m_isAnimatingTheme{false};
    QString m_currentThemeMode;
    QColor m_currentAccentColor;
};

UnifiedComponentShowcase::UnifiedComponentShowcase(QWidget* parent)
    : QMainWindow(parent)
    , m_settings(new QSettings("FluentQt", "UnifiedShowcase", this))
{
    setWindowTitle("FluentQt Unified Component Showcase - Complete Theme Integration");
    setMinimumSize(1200, 800);
    resize(1600, 1000);
    
    setupThemeManager();
    setupUI();
    loadSettings();
    
    // Initial theme update
    updateThemeUI();
    
    qDebug() << "UnifiedComponentShowcase initialized successfully";
}

UnifiedComponentShowcase::~UnifiedComponentShowcase() {
    saveSettings();
}

void UnifiedComponentShowcase::setupThemeManager() {
    m_theme = &Styling::FluentTheme::instance();
    m_animator = new Animation::FluentAnimator(this);
    m_accessibilityHelper = new Examples::FluentAccessibilityHelper(this);
    m_feedbackManager = new Examples::FluentVisualFeedbackManager(this);

    // Connect to all theme change signals
    connect(m_theme, &Styling::FluentTheme::themeChanged,
            this, &UnifiedComponentShowcase::onThemeChanged);
    connect(m_theme, &Styling::FluentTheme::modeChanged,
            this, &UnifiedComponentShowcase::onThemeModeChanged);
    connect(m_theme, &Styling::FluentTheme::accentColorChanged,
            this, &UnifiedComponentShowcase::onAccentColorChanged);

    // Connect accessibility helper to theme changes
    connect(m_theme, &Styling::FluentTheme::themeChanged,
            m_accessibilityHelper, &Examples::FluentAccessibilityHelper::onThemeChanged);

    // Connect visual feedback manager to theme changes
    connect(m_theme, &Styling::FluentTheme::themeChanged,
            m_feedbackManager, &Examples::FluentVisualFeedbackManager::onThemeChanged);
    
    // Setup animation effects for theme transitions
    m_fadeEffect = new QGraphicsOpacityEffect(this);
    m_fadeAnimation = new QPropertyAnimation(m_fadeEffect, "opacity", this);
    m_fadeAnimation->setDuration(300);
    m_fadeAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    
    connect(m_fadeAnimation, &QPropertyAnimation::finished,
            this, &UnifiedComponentShowcase::updateComponentStyles);
}

void UnifiedComponentShowcase::setupUI() {
    setupMenuBar();
    setupMainContent();
    setupThemeControls();
    setupStatusBar();
}

void UnifiedComponentShowcase::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Export Theme...", this, &UnifiedComponentShowcase::exportThemeSettings);
    fileMenu->addAction("&Import Theme...", this, &UnifiedComponentShowcase::importThemeSettings);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close);
    
    auto* themeMenu = menuBar()->addMenu("&Theme");
    themeMenu->addAction("&Settings...", this, &UnifiedComponentShowcase::showThemeSettings);
    themeMenu->addAction("&Toggle Mode", m_theme, &Styling::FluentTheme::toggleThemeMode);
    themeMenu->addSeparator();
    themeMenu->addAction("&Reset to Defaults", this, &UnifiedComponentShowcase::resetToDefaults);
    
    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, &UnifiedComponentShowcase::showAboutDialog);
}

void UnifiedComponentShowcase::setupMainContent() {
    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create main tab widget
    m_mainTabs = new QTabWidget(this);
    m_mainTabs->setTabPosition(QTabWidget::North);
    m_mainTabs->setMovable(true);
    m_mainTabs->setTabsClosable(false);
    
    // Setup all component tabs
    setupBasicControlsTab();
    setupLayoutNavigationTab();
    setupDataDisplayTab();
    setupFeedbackTab();
    setupAdvancedTab();
    setupInteractionDemoTab();
    
    mainLayout->addWidget(m_mainTabs);
}

void UnifiedComponentShowcase::setupBasicControlsTab() {
    auto* tab = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tab);

    auto* layout = new QGridLayout(tab);
    layout->setSpacing(m_theme->spacing("medium"));

    // Buttons section
    auto* buttonGroup = new QGroupBox("Buttons", tab);
    auto* buttonLayout = new QHBoxLayout(buttonGroup);

    auto* primaryButton = new Components::FluentButton("Primary", buttonGroup);
    primaryButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    m_buttons.append(primaryButton);
    buttonLayout->addWidget(primaryButton);

    auto* secondaryButton = new Components::FluentButton("Secondary", buttonGroup);
    secondaryButton->setButtonStyle(Components::FluentButtonStyle::Default);
    m_buttons.append(secondaryButton);
    buttonLayout->addWidget(secondaryButton);

    auto* accentButton = new Components::FluentButton("Accent", buttonGroup);
    accentButton->setButtonStyle(Components::FluentButtonStyle::Accent);
    m_buttons.append(accentButton);
    buttonLayout->addWidget(accentButton);

    auto* subtleButton = new Components::FluentButton("Subtle", buttonGroup);
    subtleButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
    m_buttons.append(subtleButton);
    buttonLayout->addWidget(subtleButton);

    layout->addWidget(buttonGroup, 0, 0, 1, 2);

    // Input controls section
    auto* inputGroup = new QGroupBox("Input Controls", tab);
    auto* inputLayout = new QGridLayout(inputGroup);

    auto* textInput = new Components::FluentTextInput(inputGroup);
    textInput->setPlaceholderText("Enter text here...");
    textInput->setLabelText("Text Input");
    inputLayout->addWidget(textInput, 0, 0, 1, 2);

    auto* checkbox = new Components::FluentCheckBox("Enable notifications", inputGroup);
    inputLayout->addWidget(checkbox, 1, 0);

    auto* radioButton = new Components::FluentRadioButton("Option 1", inputGroup);
    inputLayout->addWidget(radioButton, 1, 1);

    auto* toggleSwitch = new Components::FluentToggleSwitch(inputGroup);
    toggleSwitch->setText("Dark mode");
    connect(toggleSwitch, &Components::FluentToggleSwitch::toggled,
            [this](bool checked) { m_theme->setDarkMode(checked); });
    inputLayout->addWidget(toggleSwitch, 2, 0);

    auto* slider = new Components::FluentSlider(Components::FluentSliderOrientation::Horizontal, inputGroup);
    slider->setRange(0, 100);
    slider->setValue(50);
    inputLayout->addWidget(slider, 2, 1);

    auto* spinBox = new Components::FluentSpinBox(inputGroup);
    spinBox->setRange(0.0, 1000.0);
    spinBox->setValue(42.0);
    inputLayout->addWidget(spinBox, 3, 0);

    auto* comboBox = new Components::FluentComboBox(inputGroup);
    QStringList items;
    items << "Option 1" << "Option 2" << "Option 3" << "Option 4";
    comboBox->addItems(items);
    inputLayout->addWidget(comboBox, 3, 1);

    layout->addWidget(inputGroup, 1, 0, 1, 2);

    // Add to component tracking
    m_allComponents.append(tab);

    m_mainTabs->addTab(scrollArea, "Basic Controls");
}

void UnifiedComponentShowcase::setupLayoutNavigationTab() {
    auto* tab = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tab);

    auto* layout = new QVBoxLayout(tab);
    layout->setSpacing(m_theme->spacing("medium"));

    // Breadcrumb navigation
    auto* breadcrumb = new Components::FluentBreadcrumb(tab);
    breadcrumb->addItem("Home");
    breadcrumb->addItem("Components");
    breadcrumb->addItem("Layout");
    breadcrumb->addItem("Current Page");
    breadcrumb->setCurrentIndex(3); // Set current page as active

    // Connect to navigation signals
    connect(breadcrumb, QOverload<int>::of(&Components::FluentBreadcrumb::itemClicked),
            [this](int index) {
                m_statusLabel->setText(QString("Breadcrumb item %1 clicked").arg(index));
            });

    layout->addWidget(breadcrumb);

    // Cards section
    auto* cardsLayout = new QHBoxLayout();

    auto* card1 = new Components::FluentCard("Feature Card", "This card demonstrates the FluentCard component with theme integration.", tab);
    card1->setHeaderIcon(QIcon(":/icons/feature.png"));
    card1->setSelectable(true);
    m_cards.append(card1);
    cardsLayout->addWidget(card1);

    auto* card2 = new Components::FluentCard("Settings Card", "Configure application settings and preferences.", tab);
    card2->setHeaderIcon(QIcon(":/icons/settings.png"));
    card2->setSelectable(true);
    m_cards.append(card2);
    cardsLayout->addWidget(card2);

    auto* card3 = new Components::FluentCard("Info Card", "View detailed information and statistics.", tab);
    card3->setHeaderIcon(QIcon(":/icons/info.png"));
    card3->setSelectable(true);
    m_cards.append(card3);
    cardsLayout->addWidget(card3);

    layout->addLayout(cardsLayout);

    // Panels section
    auto* panelsLayout = new QHBoxLayout();

    auto* panel1 = new Components::FluentPanel("Navigation Panel", tab);
    auto* panelLayout1 = panel1->createVerticalLayout();
    panelLayout1->addWidget(new QLabel("Navigation items go here"));
    panelLayout1->addWidget(new Components::FluentButton("Navigate"));
    m_panels.append(panel1);
    panelsLayout->addWidget(panel1);

    auto* panel2 = new Components::FluentPanel("Content Panel", tab);
    auto* panelLayout2 = panel2->createVerticalLayout();
    panelLayout2->addWidget(new QLabel("Main content area"));
    auto* separator = new Components::FluentSeparator();
    separator->setOrientation(Components::FluentSeparatorOrientation::Horizontal);
    panelLayout2->addWidget(separator);
    panelLayout2->addWidget(new QLabel("Additional content"));
    m_panels.append(panel2);
    panelsLayout->addWidget(panel2);

    layout->addLayout(panelsLayout);

    // Add to component tracking
    m_allComponents.append(tab);

    m_mainTabs->addTab(scrollArea, "Layout & Navigation");
}

void UnifiedComponentShowcase::setupDataDisplayTab() {
    auto* tab = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tab);

    auto* layout = new QGridLayout(tab);
    layout->setSpacing(m_theme->spacing("medium"));

    // Progress indicators section
    auto* progressGroup = new QGroupBox("Progress Indicators", tab);
    auto* progressLayout = new QVBoxLayout(progressGroup);

    auto* progressBar = new Components::FluentProgressBar(progressGroup);
    progressBar->setValue(65);
    progressBar->setText("Loading progress...");
    progressBar->setTextVisible(true);
    progressLayout->addWidget(progressBar);

    auto* loadingIndicator = new Components::FluentLoadingIndicator(progressGroup);
    loadingIndicator->setText("Processing data...");
    loadingIndicator->setTextVisible(true);
    loadingIndicator->start();
    progressLayout->addWidget(loadingIndicator);

    layout->addWidget(progressGroup, 0, 0);

    // Rating and badges section
    auto* ratingGroup = new QGroupBox("Rating & Badges", tab);
    auto* ratingLayout = new QHBoxLayout(ratingGroup);

    auto* rating = new Components::FluentRating(4.5, 5.0, ratingGroup);
    rating->setRatingSize(Components::FluentRatingSize::Medium);
    ratingLayout->addWidget(rating);

    auto* badge = new Components::FluentBadge("New", ratingGroup);
    badge->setBadgeStyle(Components::FluentBadgeStyle::Filled);
    badge->setStatus(Components::FluentBadgeStatus::Success);
    ratingLayout->addWidget(badge);

    auto* avatar = new Components::FluentAvatar(ratingGroup);
    avatar->setInitials("JD");
    avatar->setAvatarSize(Components::FluentAvatarSize::Medium);
    ratingLayout->addWidget(avatar);

    layout->addWidget(ratingGroup, 0, 1);

    // Tree view section
    auto* treeGroup = new QGroupBox("Tree View", tab);
    auto* treeLayout = new QVBoxLayout(treeGroup);

    auto* treeView = new Components::FluentTreeView(treeGroup);
    // Add some sample data to the tree view using FluentTreeView API
    std::vector<Components::FluentTreeColumn> columns = {
        {"Name", 200, true, true, Qt::AlignLeft},
        {"Type", 120, true, true, Qt::AlignLeft},
        {"Size", 80, true, true, Qt::AlignRight}
    };
    treeView->setColumns(columns);

    auto* rootItem = treeView->addTopLevelItem("Documents");
    rootItem->setText(1, "Folder");
    rootItem->setText(2, "");

    auto* childItem1 = treeView->addChildItem(rootItem, "Report.pdf");
    childItem1->setText(1, "PDF Document");
    childItem1->setText(2, "2.5 MB");

    auto* childItem2 = treeView->addChildItem(rootItem, "Presentation.pptx");
    childItem2->setText(1, "PowerPoint");
    childItem2->setText(2, "5.1 MB");

    treeView->expandAll();
    treeLayout->addWidget(treeView);

    layout->addWidget(treeGroup, 1, 0, 1, 2);

    // Add to component tracking
    m_allComponents.append(tab);

    m_mainTabs->addTab(scrollArea, "Data Display");
}

void UnifiedComponentShowcase::setupFeedbackTab() {
    auto* tab = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tab);

    auto* layout = new QVBoxLayout(tab);
    layout->setSpacing(m_theme->spacing("medium"));

    // Toast notifications section
    auto* toastGroup = new QGroupBox("Toast Notifications", tab);
    auto* toastLayout = new QHBoxLayout(toastGroup);

    auto* infoToastBtn = new Components::FluentButton("Show Info Toast", toastGroup);
    connect(infoToastBtn, &Components::FluentButton::clicked, [this]() {
        Components::FluentToastManager::instance().showInfo(
            "Information", "This is an informational message.");
    });
    toastLayout->addWidget(infoToastBtn);

    auto* successToastBtn = new Components::FluentButton("Show Success Toast", toastGroup);
    connect(successToastBtn, &Components::FluentButton::clicked, [this]() {
        Components::FluentToastManager::instance().showSuccess(
            "Success", "Operation completed successfully!");
    });
    toastLayout->addWidget(successToastBtn);

    auto* warningToastBtn = new Components::FluentButton("Show Warning Toast", toastGroup);
    connect(warningToastBtn, &Components::FluentButton::clicked, [this]() {
        Components::FluentToastManager::instance().showWarning(
            "Warning", "Please check your input data.");
    });
    toastLayout->addWidget(warningToastBtn);

    auto* errorToastBtn = new Components::FluentButton("Show Error Toast", toastGroup);
    connect(errorToastBtn, &Components::FluentButton::clicked, [this]() {
        Components::FluentToastManager::instance().showError(
            "Error", "An error occurred while processing.");
    });
    toastLayout->addWidget(errorToastBtn);

    layout->addWidget(toastGroup);

    // Notification section
    auto* notificationGroup = new QGroupBox("Notifications", tab);
    auto* notificationLayout = new QVBoxLayout(notificationGroup);

    auto* notification = new Components::FluentNotification(
        Components::FluentNotificationType::Info,
        "System Update Available",
        "A new system update is ready to install. Click here to learn more.",
        notificationGroup);
    notification->setIcon(QIcon(":/icons/update.png"));
    notificationLayout->addWidget(notification);

    layout->addWidget(notificationGroup);

    // Dialog section
    auto* dialogGroup = new QGroupBox("Dialogs", tab);
    auto* dialogLayout = new QHBoxLayout(dialogGroup);

    auto* messageDialogBtn = new Components::FluentButton("Show Message Dialog", dialogGroup);
    connect(messageDialogBtn, &Components::FluentButton::clicked, [this]() {
        Components::FluentContentDialog::showMessageDialog(
            this, "Information", "This is a sample message dialog with theme integration.",
            "OK", "Cancel");
    });
    dialogLayout->addWidget(messageDialogBtn);

    auto* confirmDialogBtn = new Components::FluentButton("Show Confirm Dialog", dialogGroup);
    connect(confirmDialogBtn, &Components::FluentButton::clicked, [this]() {
        auto result = Components::FluentContentDialog::showConfirmDialog(
            this, "Confirm Action", "Are you sure you want to proceed with this action?",
            "Yes", "No");
        if (result == Components::FluentDialogResult::Primary) {
            m_statusLabel->setText("User confirmed the action");
        }
    });
    dialogLayout->addWidget(confirmDialogBtn);

    layout->addWidget(dialogGroup);

    // Add to component tracking
    m_allComponents.append(tab);

    m_mainTabs->addTab(scrollArea, "Feedback");
}

void UnifiedComponentShowcase::setupAdvancedTab() {
    auto* tab = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tab);

    auto* layout = new QVBoxLayout(tab);
    layout->setSpacing(m_theme->spacing("medium"));

    // Calendar section
    auto* calendarGroup = new QGroupBox("Calendar & Date/Time", tab);
    auto* calendarLayout = new QHBoxLayout(calendarGroup);

    auto* calendar = new Components::FluentCalendar(calendarGroup);
    calendar->setSelectedDate(QDate::currentDate());
    calendarLayout->addWidget(calendar);

    auto* dateTimeLayout = new QVBoxLayout();

    auto* datePicker = new Components::FluentDatePicker(calendarGroup);
    datePicker->setDate(QDate::currentDate());
    dateTimeLayout->addWidget(datePicker);

    auto* timePicker = new Components::FluentTimePicker(calendarGroup);
    timePicker->setTime(QTime::currentTime());
    dateTimeLayout->addWidget(timePicker);

    calendarLayout->addLayout(dateTimeLayout);
    layout->addWidget(calendarGroup);

    // Color picker section
    auto* colorGroup = new QGroupBox("Color Picker", tab);
    auto* colorLayout = new QHBoxLayout(colorGroup);

    auto* colorPicker = new Components::FluentColorPicker(colorGroup);
    colorPicker->setColor(m_theme->color("accent"));
    connect(colorPicker, &Components::FluentColorPicker::colorChanged,
            [this](const QColor& color) {
                m_statusLabel->setText(QString("Selected color: %1").arg(color.name()));
            });
    colorLayout->addWidget(colorPicker);

    layout->addWidget(colorGroup);

    // Carousel section
    auto* carouselGroup = new QGroupBox("Carousel", tab);
    auto* carouselLayout = new QVBoxLayout(carouselGroup);

    auto* carousel = new Components::FluentCarousel(carouselGroup);
    carousel->setMinimumHeight(200);

    // Add some sample items to the carousel
    for (int i = 1; i <= 5; ++i) {
        auto* item = new QWidget();
        auto* itemLayout = new QVBoxLayout(item);
        auto* label = new QLabel(QString("Slide %1").arg(i));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(QString("background-color: %1; color: white; padding: 20px; border-radius: 8px;")
                           .arg(QColor::fromHsv(i * 60, 180, 200).name()));
        itemLayout->addWidget(label);
        carousel->addItem(item);
    }

    carouselLayout->addWidget(carousel);
    layout->addWidget(carouselGroup);

    // Add to component tracking
    m_allComponents.append(tab);

    m_mainTabs->addTab(scrollArea, "Advanced");
}

void UnifiedComponentShowcase::setupInteractionDemoTab() {
    auto* tab = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tab);

    auto* layout = new QVBoxLayout(tab);
    layout->setSpacing(m_theme->spacing("medium"));

    // Form interaction demo
    auto* formGroup = new QGroupBox("Interactive Form Demo", tab);
    auto* formLayout = new QGridLayout(formGroup);

    auto* nameInput = new Components::FluentTextInput(formGroup);
    nameInput->setLabelText("Full Name");
    nameInput->setPlaceholderText("Enter your full name");
    formLayout->addWidget(nameInput, 0, 0, 1, 2);

    auto* emailInput = new Components::FluentTextInput(formGroup);
    emailInput->setLabelText("Email Address");
    emailInput->setPlaceholderText("Enter your email");
    formLayout->addWidget(emailInput, 1, 0, 1, 2);

    auto* subscribeCheck = new Components::FluentCheckBox("Subscribe to newsletter", formGroup);
    formLayout->addWidget(subscribeCheck, 2, 0);

    auto* termsCheck = new Components::FluentCheckBox("I agree to the terms", formGroup);
    formLayout->addWidget(termsCheck, 2, 1);

    auto* submitButton = new Components::FluentButton("Submit Form", formGroup);
    submitButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    connect(submitButton, &Components::FluentButton::clicked, [this, nameInput, emailInput]() {
        if (nameInput->text().isEmpty() || emailInput->text().isEmpty()) {
            Components::FluentToastManager::instance().showWarning(
                "Validation Error", "Please fill in all required fields.");
        } else {
            Components::FluentToastManager::instance().showSuccess(
                "Form Submitted", "Thank you for your submission!");
        }
    });
    formLayout->addWidget(submitButton, 3, 0, 1, 2);

    layout->addWidget(formGroup);

    // Theme interaction demo
    auto* themeGroup = new QGroupBox("Theme Interaction Demo", tab);
    auto* themeLayout = new QGridLayout(themeGroup);

    auto* modeLabel = new QLabel("Theme Mode:", themeGroup);
    themeLayout->addWidget(modeLabel, 0, 0);

    auto* modeCombo = new Components::FluentComboBox(themeGroup);
    modeCombo->addItems({"Light", "Dark", "System"});
    modeCombo->setCurrentText(m_theme->isDarkMode() ? "Dark" : "Light");
    connect(modeCombo, QOverload<const QString&>::of(&Components::FluentComboBox::currentTextChanged),
            [this](const QString& text) {
                if (text == "Light") {
                    m_theme->setMode(Styling::FluentThemeMode::Light);
                } else if (text == "Dark") {
                    m_theme->setMode(Styling::FluentThemeMode::Dark);
                } else {
                    m_theme->setMode(Styling::FluentThemeMode::System);
                }
            });
    themeLayout->addWidget(modeCombo, 0, 1);

    auto* accentLabel = new QLabel("Accent Color:", themeGroup);
    themeLayout->addWidget(accentLabel, 1, 0);

    auto* accentCombo = new Components::FluentComboBox(themeGroup);
    accentCombo->addItems({"Blue", "Purple", "Green", "Orange", "Red", "Yellow"});
    connect(accentCombo, QOverload<const QString&>::of(&Components::FluentComboBox::currentTextChanged),
            [this](const QString& text) {
                if (text == "Blue") m_theme->setAccentColor(Styling::FluentAccentColor::Blue);
                else if (text == "Purple") m_theme->setAccentColor(Styling::FluentAccentColor::Purple);
                else if (text == "Green") m_theme->setAccentColor(Styling::FluentAccentColor::Green);
                else if (text == "Orange") m_theme->setAccentColor(Styling::FluentAccentColor::Orange);
                else if (text == "Red") m_theme->setAccentColor(Styling::FluentAccentColor::Red);
                else if (text == "Yellow") m_theme->setAccentColor(Styling::FluentAccentColor::Yellow);
            });
    themeLayout->addWidget(accentCombo, 1, 1);

    layout->addWidget(themeGroup);

    // Data manipulation demo
    auto* dataGroup = new QGroupBox("Data Manipulation Demo", tab);
    auto* dataLayout = new QVBoxLayout(dataGroup);

    // Create a simple data table simulation
    auto* dataTable = new QWidget(dataGroup);
    auto* tableLayout = new QGridLayout(dataTable);

    // Headers
    tableLayout->addWidget(new QLabel("Name"), 0, 0);
    tableLayout->addWidget(new QLabel("Email"), 0, 1);
    tableLayout->addWidget(new QLabel("Status"), 0, 2);
    tableLayout->addWidget(new QLabel("Actions"), 0, 3);

    // Sample data rows
    for (int i = 1; i <= 3; ++i) {
        auto* nameLabel = new QLabel(QString("User %1").arg(i));
        auto* emailLabel = new QLabel(QString("user%1@example.com").arg(i));
        auto* statusToggle = new Components::FluentToggleSwitch(dataTable);
        statusToggle->setChecked(i % 2 == 0);
        statusToggle->setText(statusToggle->isChecked() ? "Active" : "Inactive");

        auto* editButton = new Components::FluentButton("Edit", dataTable);
        editButton->setButtonStyle(Components::FluentButtonStyle::Subtle);

        connect(statusToggle, &Components::FluentToggleSwitch::toggled, [statusToggle](bool checked) {
            statusToggle->setText(checked ? "Active" : "Inactive");
        });

        connect(editButton, &Components::FluentButton::clicked, [this, i]() {
            Components::FluentToastManager::instance().showInfo(
                "Edit User", QString("Editing user %1...").arg(i));
        });

        tableLayout->addWidget(nameLabel, i, 0);
        tableLayout->addWidget(emailLabel, i, 1);
        tableLayout->addWidget(statusToggle, i, 2);
        tableLayout->addWidget(editButton, i, 3);
    }

    dataLayout->addWidget(dataTable);

    // Add/Remove controls
    auto* dataControlsLayout = new QHBoxLayout();

    auto* addButton = new Components::FluentButton("Add User", dataGroup);
    addButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    connect(addButton, &Components::FluentButton::clicked, [this]() {
        Components::FluentToastManager::instance().showSuccess(
            "User Added", "New user has been added to the system.");
    });
    dataControlsLayout->addWidget(addButton);

    auto* removeButton = new Components::FluentButton("Remove Selected", dataGroup);
    connect(removeButton, &Components::FluentButton::clicked, [this]() {
        auto result = Components::FluentContentDialog::showConfirmDialog(
            this, "Confirm Deletion",
            "Are you sure you want to remove the selected users?",
            "Delete", "Cancel");
        if (result == Components::FluentDialogResult::Primary) {
            Components::FluentToastManager::instance().showWarning(
                "Users Removed", "Selected users have been removed.");
        }
    });
    dataControlsLayout->addWidget(removeButton);

    dataControlsLayout->addStretch();
    dataLayout->addLayout(dataControlsLayout);

    layout->addWidget(dataGroup);

    // Navigation flow demo
    auto* navGroup = new QGroupBox("Navigation Flow Demo", tab);
    auto* navLayout = new QHBoxLayout(navGroup);

    // Step indicators
    auto* step1Button = new Components::FluentButton("Step 1: Setup", navGroup);
    auto* step2Button = new Components::FluentButton("Step 2: Configure", navGroup);
    auto* step3Button = new Components::FluentButton("Step 3: Review", navGroup);
    auto* step4Button = new Components::FluentButton("Step 4: Complete", navGroup);

    QList<Components::FluentButton*> stepButtons = {step1Button, step2Button, step3Button, step4Button};
    int currentStep = 0;

    // Update step button styles
    auto updateStepButtons = [stepButtons](int current) {
        for (int i = 0; i < stepButtons.size(); ++i) {
            if (i < current) {
                stepButtons[i]->setButtonStyle(Components::FluentButtonStyle::Accent);
                stepButtons[i]->setText(stepButtons[i]->text().split(":")[0] + ": ✓");
            } else if (i == current) {
                stepButtons[i]->setButtonStyle(Components::FluentButtonStyle::Primary);
            } else {
                stepButtons[i]->setButtonStyle(Components::FluentButtonStyle::Default);
            }
        }
    };

    // Navigation controls
    auto* prevButton = new Components::FluentButton("Previous", navGroup);
    auto* nextButton = new Components::FluentButton("Next", navGroup);

    connect(prevButton, &Components::FluentButton::clicked, [&currentStep, updateStepButtons, this]() {
        if (currentStep > 0) {
            currentStep--;
            updateStepButtons(currentStep);
            m_statusLabel->setText(QString("Navigated to step %1").arg(currentStep + 1));
        }
    });

    connect(nextButton, &Components::FluentButton::clicked, [&currentStep, updateStepButtons, stepButtons, this]() {
        if (currentStep < stepButtons.size() - 1) {
            currentStep++;
            updateStepButtons(currentStep);
            m_statusLabel->setText(QString("Navigated to step %1").arg(currentStep + 1));
        }
    });

    // Initial setup
    updateStepButtons(currentStep);

    for (auto* button : stepButtons) {
        navLayout->addWidget(button);
    }
    navLayout->addStretch();
    navLayout->addWidget(prevButton);
    navLayout->addWidget(nextButton);

    layout->addWidget(navGroup);

    // Add to component tracking
    m_allComponents.append(tab);

    m_mainTabs->addTab(scrollArea, "Interactions");
}

void UnifiedComponentShowcase::setupThemeControls() {
    m_themeControlPanel = new QWidget(this);
    auto* layout = new QHBoxLayout(m_themeControlPanel);
    layout->setContentsMargins(m_theme->spacing("medium"), m_theme->spacing("small"),
                              m_theme->spacing("medium"), m_theme->spacing("small"));

    // Theme mode toggle
    m_themeModeButton = new Components::FluentButton("Toggle Theme", m_themeControlPanel);
    m_themeModeButton->setButtonStyle(Components::FluentButtonStyle::Accent);
    connect(m_themeModeButton, &Components::FluentButton::clicked,
            m_theme, &Styling::FluentTheme::toggleThemeMode);
    layout->addWidget(m_themeModeButton);

    // Accent color button
    m_accentColorButton = new Components::FluentButton("Change Accent", m_themeControlPanel);
    connect(m_accentColorButton, &Components::FluentButton::clicked,
            this, &UnifiedComponentShowcase::onAccentColorChanged);
    layout->addWidget(m_accentColorButton);

    // High contrast toggle
    m_highContrastSwitch = new Components::FluentToggleSwitch(m_themeControlPanel);
    m_highContrastSwitch->setText("High Contrast");
    m_highContrastSwitch->setChecked(m_theme->isHighContrastMode());
    connect(m_highContrastSwitch, &Components::FluentToggleSwitch::toggled,
            m_theme, &Styling::FluentTheme::setHighContrastMode);
    layout->addWidget(m_highContrastSwitch);

    // Opacity slider
    auto* opacityLabel = new QLabel("Opacity:", m_themeControlPanel);
    layout->addWidget(opacityLabel);

    m_opacitySlider = new Components::FluentSlider(Components::FluentSliderOrientation::Horizontal, m_themeControlPanel);
    m_opacitySlider->setRange(50, 100);
    m_opacitySlider->setValue(100);
    m_opacitySlider->setMinimumWidth(100);
    connect(m_opacitySlider, &Components::FluentSlider::valueChanged,
            [this](int value) {
                setWindowOpacity(value / 100.0);
                m_statusLabel->setText(QString("Window opacity: %1%").arg(value));
            });
    layout->addWidget(m_opacitySlider);

    layout->addStretch();

    // Add theme control panel to main layout
    auto* centralWidget = this->centralWidget();
    auto* mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
    if (mainLayout) {
        mainLayout->insertWidget(0, m_themeControlPanel);
    }
}

void UnifiedComponentShowcase::setupStatusBar() {
    m_statusBar = statusBar();

    m_statusLabel = new QLabel("FluentQt Unified Showcase Ready", this);
    m_statusBar->addWidget(m_statusLabel);

    m_statusBar->addPermanentWidget(new QLabel("|"));

    m_themeStatusLabel = new QLabel(this);
    updateThemeStatusLabel();
    m_statusBar->addPermanentWidget(m_themeStatusLabel);

    m_statusBar->addPermanentWidget(new QLabel("|"));

    auto* versionLabel = new QLabel("FluentQt v1.0", this);
    m_statusBar->addPermanentWidget(versionLabel);
}

void UnifiedComponentShowcase::updateThemeStatusLabel() {
    if (m_themeStatusLabel) {
        QString modeText = m_theme->isDarkMode() ? "Dark" : "Light";
        if (m_theme->mode() == Styling::FluentThemeMode::System) {
            modeText = "System (" + modeText + ")";
        }
        m_themeStatusLabel->setText(QString("Theme: %1").arg(modeText));
    }
}

void UnifiedComponentShowcase::onThemeChanged() {
    if (!m_isAnimatingTheme) {
        animateThemeTransition();
    }
    updateThemeUI();
}

void UnifiedComponentShowcase::onThemeModeChanged() {
    updateThemeStatusLabel();
    m_statusLabel->setText("Theme mode changed");
}

void UnifiedComponentShowcase::onAccentColorChanged() {
    // Cycle through accent colors
    static int colorIndex = 0;
    const QList<Styling::FluentAccentColor> colors = {
        Styling::FluentAccentColor::Blue,
        Styling::FluentAccentColor::Purple,
        Styling::FluentAccentColor::Green,
        Styling::FluentAccentColor::Orange,
        Styling::FluentAccentColor::Red,
        Styling::FluentAccentColor::Yellow
    };

    m_theme->setAccentColor(colors[colorIndex % colors.size()]);
    colorIndex++;

    m_statusLabel->setText("Accent color changed");
}

void UnifiedComponentShowcase::updateThemeUI() {
    // Update main window background
    setStyleSheet(QString("QMainWindow { background-color: %1; }")
                 .arg(m_theme->color("backgroundPrimary").name()));

    // Update theme control panel background
    if (m_themeControlPanel) {
        m_themeControlPanel->setStyleSheet(QString("QWidget { background-color: %1; border-bottom: 1px solid %2; }")
                                         .arg(m_theme->color("backgroundSecondary").name())
                                         .arg(m_theme->color("strokeDefault").name()));
    }

    // Update theme mode button text
    if (m_themeModeButton) {
        m_themeModeButton->setText(m_theme->isDarkMode() ? "Switch to Light" : "Switch to Dark");
    }

    // Update high contrast switch
    if (m_highContrastSwitch) {
        m_highContrastSwitch->setChecked(m_theme->isHighContrastMode());
    }

    updateThemeStatusLabel();
}

void UnifiedComponentShowcase::updateComponentResponsiveLayouts(bool isCompact, bool isMedium, bool isLarge) {
    // Update card layouts
    for (auto* card : m_cards) {
        if (!card) continue;

        if (isCompact) {
            // Stack cards vertically on small screens
            card->setMinimumWidth(200);
            card->setMaximumWidth(QWIDGETSIZE_MAX);
        } else if (isMedium) {
            // Medium width cards
            card->setMinimumWidth(250);
            card->setMaximumWidth(400);
        } else {
            // Full width cards on large screens
            card->setMinimumWidth(300);
            card->setMaximumWidth(500);
        }
    }

    // Update button layouts
    for (auto* button : m_buttons) {
        if (!button) continue;

        if (isCompact) {
            // Smaller buttons on compact screens
            button->setMinimumHeight(32);
            button->setMinimumWidth(80);
        } else {
            // Standard button sizes
            button->setMinimumHeight(36);
            button->setMinimumWidth(100);
        }
    }

    // Update panel layouts
    for (auto* panel : m_panels) {
        if (!panel) continue;

        if (isCompact) {
            // Stack panels vertically
            panel->setMinimumWidth(200);
        } else {
            // Side-by-side panels
            panel->setMinimumWidth(300);
        }
    }

    // Update tab content layouts based on screen size
    for (int i = 0; i < m_mainTabs->count(); ++i) {
        QWidget* tabWidget = m_mainTabs->widget(i);
        if (!tabWidget) continue;

        // Find scroll areas within tabs
        auto scrollAreas = tabWidget->findChildren<QScrollArea*>();
        for (auto* scrollArea : scrollAreas) {
            QWidget* scrollContent = scrollArea->widget();
            if (!scrollContent) continue;

            // Update content layout based on screen size
            auto* layout = scrollContent->layout();
            if (auto* gridLayout = qobject_cast<QGridLayout*>(layout)) {
                if (isCompact) {
                    // Single column layout for compact screens
                    updateGridLayoutForCompact(gridLayout);
                } else if (isMedium) {
                    // Two column layout for medium screens
                    updateGridLayoutForMedium(gridLayout);
                } else {
                    // Multi-column layout for large screens
                    updateGridLayoutForLarge(gridLayout);
                }
            }
        }
    }
}

void UnifiedComponentShowcase::updateGridLayoutForCompact(QGridLayout* layout) {
    if (!layout) return;

    // Force single column layout
    int itemCount = layout->count();
    for (int i = 0; i < itemCount; ++i) {
        QLayoutItem* item = layout->itemAt(i);
        if (item && item->widget()) {
            int row, col, rowSpan, colSpan;
            layout->getItemPosition(i, &row, &col, &rowSpan, &colSpan);

            // Move all items to column 0
            if (col != 0) {
                layout->removeItem(item);
                layout->addWidget(item->widget(), row + col, 0, rowSpan, 1);
            }
        }
    }
}

void UnifiedComponentShowcase::updateGridLayoutForMedium(QGridLayout* layout) {
    if (!layout) return;

    // Allow up to 2 columns
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);

    // Hide columns beyond 2
    for (int col = 2; col < layout->columnCount(); ++col) {
        for (int row = 0; row < layout->rowCount(); ++row) {
            QLayoutItem* item = layout->itemAtPosition(row, col);
            if (item && item->widget()) {
                item->widget()->setVisible(false);
            }
        }
    }
}

void UnifiedComponentShowcase::updateGridLayoutForLarge(QGridLayout* layout) {
    if (!layout) return;

    // Show all columns and distribute space evenly
    for (int col = 0; col < layout->columnCount(); ++col) {
        layout->setColumnStretch(col, 1);

        // Make sure all widgets are visible
        for (int row = 0; row < layout->rowCount(); ++row) {
            QLayoutItem* item = layout->itemAtPosition(row, col);
            if (item && item->widget()) {
                item->widget()->setVisible(true);
            }
        }
    }
}

void UnifiedComponentShowcase::updateComponentStyles() {
    // This method is called after theme transitions to update component styles
    for (auto* component : m_allComponents) {
        if (component) {
            component->update();
        }
    }

    // Update specific component collections
    for (auto* button : m_buttons) {
        if (button) {
            button->update();
        }
    }

    for (auto* card : m_cards) {
        if (card) {
            card->update();
        }
    }

    for (auto* panel : m_panels) {
        if (panel) {
            panel->update();
        }
    }
}

void UnifiedComponentShowcase::animateThemeTransition() {
    if (m_isAnimatingTheme) return;

    m_isAnimatingTheme = true;

    // Apply fade effect to main content
    if (m_mainTabs) {
        m_mainTabs->setGraphicsEffect(m_fadeEffect);
        m_fadeAnimation->setStartValue(1.0);
        m_fadeAnimation->setEndValue(0.3);
        m_fadeAnimation->start();

        // After fade completes, update styles and fade back in
        QTimer::singleShot(300, [this]() {
            updateComponentStyles();
            m_fadeAnimation->setStartValue(0.3);
            m_fadeAnimation->setEndValue(1.0);
            m_fadeAnimation->start();

            QTimer::singleShot(300, [this]() {
                m_isAnimatingTheme = false;
                m_mainTabs->setGraphicsEffect(nullptr);
            });
        });
    }
}

void UnifiedComponentShowcase::showThemeSettings() {
    auto result = Components::FluentContentDialog::showMessageDialog(
        this, "Theme Settings",
        "Configure theme preferences and appearance options.\n\n"
        "Use the theme controls in the toolbar to adjust:\n"
        "• Theme mode (Light/Dark/System)\n"
        "• Accent color\n"
        "• High contrast mode\n"
        "• Window opacity",
        "OK", "Reset", "Cancel");

    if (result == Components::FluentDialogResult::Secondary) {
        resetToDefaults();
    }
}

void UnifiedComponentShowcase::showAboutDialog() {
    QString aboutText =
        "FluentQt Unified Component Showcase\n\n"
        "A comprehensive demonstration of all FluentQt components with:\n\n"
        "🎨 THEME SYSTEM:\n"
        "• Light, Dark, and System theme modes\n"
        "• Six built-in accent colors with smooth transitions\n"
        "• High contrast mode for accessibility\n"
        "• Real-time theme updates across all components\n"
        "• Theme persistence and settings management\n\n"
        "🧩 COMPONENT LIBRARY:\n"
        "• Basic Controls: Buttons, inputs, checkboxes, sliders\n"
        "• Layout & Navigation: Cards, panels, breadcrumbs\n"
        "• Data Display: Progress bars, ratings, badges, tree views\n"
        "• Feedback: Toasts, notifications, dialogs\n"
        "• Advanced: Calendar, color picker, carousel\n"
        "• Interactive Demos: Form validation, data manipulation\n\n"
        "♿ ACCESSIBILITY:\n"
        "• Screen reader support and announcements\n"
        "• Keyboard navigation throughout\n"
        "• High contrast mode compatibility\n"
        "• Focus indicators and ARIA labels\n\n"
        "📱 RESPONSIVE DESIGN:\n"
        "• Adaptive layouts for different screen sizes\n"
        "• Component scaling and spacing adjustments\n"
        "• Mobile-friendly interactions\n\n"
        "🔧 DEVELOPER FEATURES:\n"
        "• Comprehensive API examples\n"
        "• Integration guidelines and best practices\n"
        "• Performance optimizations\n"
        "• Extensive documentation\n\n"
        "Built with FluentQt v1.0\n"
        "Based on Microsoft Fluent Design System\n\n"
        "KEYBOARD SHORTCUTS:\n"
        "• F1: Show this help\n"
        "• Ctrl+T: Toggle theme mode\n"
        "• Ctrl+H: Toggle high contrast\n"
        "• Tab: Navigate between controls\n"
        "• Enter/Space: Activate focused element";

    Components::FluentContentDialog::showMessageDialog(
        this, "About FluentQt Unified Showcase", aboutText,
        "OK", "Documentation", "GitHub");
}

void UnifiedComponentShowcase::keyPressEvent(QKeyEvent* event) {
    // Handle keyboard shortcuts
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_T:
            // Ctrl+T: Toggle theme mode
            m_theme->toggleThemeMode();
            m_statusLabel->setText("Theme toggled via keyboard shortcut");
            event->accept();
            return;

        case Qt::Key_H:
            // Ctrl+H: Toggle high contrast
            m_theme->setHighContrastMode(!m_theme->isHighContrastMode());
            m_statusLabel->setText("High contrast toggled via keyboard shortcut");
            event->accept();
            return;

        case Qt::Key_Plus:
        case Qt::Key_Equal:
            // Ctrl++: Increase opacity
            if (m_opacitySlider) {
                int newValue = qMin(100, m_opacitySlider->value() + 10);
                m_opacitySlider->setValue(newValue);
                event->accept();
                return;
            }
            break;

        case Qt::Key_Minus:
            // Ctrl+-: Decrease opacity
            if (m_opacitySlider) {
                int newValue = qMax(50, m_opacitySlider->value() - 10);
                m_opacitySlider->setValue(newValue);
                event->accept();
                return;
            }
            break;
        }
    }

    switch (event->key()) {
    case Qt::Key_F1:
        // F1: Show help
        showAboutDialog();
        event->accept();
        return;

    case Qt::Key_Escape:
        // Escape: Clear focus or close dialogs
        if (focusWidget()) {
            focusWidget()->clearFocus();
            m_statusLabel->setText("Focus cleared");
        }
        event->accept();
        return;
    }

    // Let parent handle other keys
    QMainWindow::keyPressEvent(event);
}

void UnifiedComponentShowcase::exportThemeSettings() {
    QString themeData = m_theme->exportTheme();
    // In a real application, you would save this to a file
    m_statusLabel->setText("Theme settings exported");

    Components::FluentToastManager::instance().showSuccess(
        "Export Complete", "Theme settings have been exported successfully.");
}

void UnifiedComponentShowcase::importThemeSettings() {
    // In a real application, you would load from a file
    m_statusLabel->setText("Theme settings imported");

    Components::FluentToastManager::instance().showInfo(
        "Import Complete", "Theme settings have been imported successfully.");
}

void UnifiedComponentShowcase::resetToDefaults() {
    m_theme->resetToDefaults();
    m_statusLabel->setText("Theme reset to defaults");

    Components::FluentToastManager::instance().showInfo(
        "Reset Complete", "All theme settings have been reset to defaults.");
}

void UnifiedComponentShowcase::closeEvent(QCloseEvent* event) {
    saveSettings();
    QMainWindow::closeEvent(event);
}

void UnifiedComponentShowcase::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateResponsiveLayout();
}

void UnifiedComponentShowcase::updateResponsiveLayout() {
    int windowWidth = width();
    int windowHeight = height();

    // Define breakpoints
    bool isCompact = windowWidth < 800;
    bool isMedium = windowWidth >= 800 && windowWidth < 1200;
    bool isLarge = windowWidth >= 1200;
    bool isTall = windowHeight >= 600;

    // Update theme control panel visibility and layout
    if (m_themeControlPanel) {
        if (isCompact) {
            // Hide theme controls on very small screens
            m_themeControlPanel->setVisible(false);
        } else {
            m_themeControlPanel->setVisible(true);

            // Adjust control panel layout based on width
            auto* layout = qobject_cast<QHBoxLayout*>(m_themeControlPanel->layout());
            if (layout) {
                if (isMedium) {
                    // Compact spacing for medium screens
                    layout->setSpacing(m_theme->spacing("small"));
                } else if (isLarge) {
                    // Normal spacing for large screens
                    layout->setSpacing(m_theme->spacing("medium"));
                }
            }
        }
    }

    // Update main tabs layout
    if (m_mainTabs) {
        if (isCompact) {
            // Stack tabs vertically on small screens
            m_mainTabs->setTabPosition(QTabWidget::West);
            m_mainTabs->setUsesScrollButtons(true);
        } else {
            // Horizontal tabs for larger screens
            m_mainTabs->setTabPosition(QTabWidget::North);
            m_mainTabs->setUsesScrollButtons(false);
        }
    }

    // Update component layouts within tabs
    updateComponentResponsiveLayouts(isCompact, isMedium, isLarge);

    // Update status bar
    if (statusBar()) {
        if (isCompact) {
            // Hide some status bar elements on small screens
            if (m_themeStatusLabel) {
                m_themeStatusLabel->setVisible(false);
            }
        } else {
            if (m_themeStatusLabel) {
                m_themeStatusLabel->setVisible(true);
            }
        }
    }

    // Announce layout change for accessibility
    if (m_accessibilityHelper) {
        QString layoutMode = isCompact ? "compact" : (isMedium ? "medium" : "large");
        Examples::FluentAccessibilityHelper::announceToScreenReader(
            QString("Layout changed to %1 mode").arg(layoutMode));
    }
}

void UnifiedComponentShowcase::saveSettings() {
    if (!m_settings) return;

    m_settings->setValue("theme/mode", static_cast<int>(m_theme->mode()));
    m_settings->setValue("theme/accentColor", static_cast<int>(m_theme->accentColor()));
    m_settings->setValue("theme/highContrast", m_theme->isHighContrastMode());
    m_settings->setValue("window/geometry", saveGeometry());
    m_settings->setValue("window/state", saveState());
    m_settings->setValue("window/opacity", windowOpacity());

    if (m_opacitySlider) {
        m_settings->setValue("ui/opacity", m_opacitySlider->value());
    }
}

void UnifiedComponentShowcase::loadSettings() {
    if (!m_settings) return;

    // Load theme settings
    auto mode = static_cast<Styling::FluentThemeMode>(
        m_settings->value("theme/mode", static_cast<int>(Styling::FluentThemeMode::System)).toInt());
    auto accentColor = static_cast<Styling::FluentAccentColor>(
        m_settings->value("theme/accentColor", static_cast<int>(Styling::FluentAccentColor::Blue)).toInt());
    bool highContrast = m_settings->value("theme/highContrast", false).toBool();

    m_theme->setMode(mode);
    m_theme->setAccentColor(accentColor);
    m_theme->setHighContrastMode(highContrast);

    // Load window settings
    restoreGeometry(m_settings->value("window/geometry").toByteArray());
    restoreState(m_settings->value("window/state").toByteArray());

    qreal opacity = m_settings->value("window/opacity", 1.0).toReal();
    setWindowOpacity(opacity);

    if (m_opacitySlider) {
        int opacityValue = m_settings->value("ui/opacity", 100).toInt();
        m_opacitySlider->setValue(opacityValue);
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("FluentQt Unified Showcase");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt");
    app.setOrganizationDomain("fluentqt.org");

    // Initialize FluentQt theme system
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);

    // Create and show the unified showcase
    UnifiedComponentShowcase showcase;
    showcase.show();

    return app.exec();
}
