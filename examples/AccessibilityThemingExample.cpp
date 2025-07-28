// examples/AccessibilityThemingExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QScrollArea>
#include <QSplitter>
#include <QTabWidget>
#include <QProgressBar>
#include <QSpinBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QAccessible>
#include <QShortcut>
#include <QKeySequence>
#include <QTimer>

#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentSpinBox.h"
#include "FluentQt/Components/FluentTextBox.h"
#include "FluentQt/Accessibility/FluentAccessible.h"

using namespace FluentQt;

/**
 * @brief Comprehensive demonstration of FluentQt accessibility and theming features
 * 
 * This example showcases:
 * - Complete accessibility support with screen reader compatibility
 * - High contrast mode and visual accessibility features
 * - Comprehensive keyboard navigation patterns
 * - Advanced theming capabilities with live preview
 * - Color contrast validation and WCAG compliance
 * - Custom theme creation and export/import
 * - Accessibility testing tools and validation
 * - Real-time accessibility announcements
 */
class AccessibilityThemingExample : public QMainWindow {
    Q_OBJECT

public:
    AccessibilityThemingExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupAccessibility();
        setupUI();
        setupThemeControls();
        setupAccessibilityControls();
        setupKeyboardShortcuts();
        setupConnections();
        setupInitialTheme();
        announceApplicationReady();
    }

private slots:
    void onThemeModeChanged(int index);
    void onAccentColorChanged(int index);
    void onHighContrastToggled(bool enabled);
    void onFontSizeChanged(int size);
    void onCustomColorSelected();
    void onCustomFontSelected();
    void exportCurrentTheme();
    void importTheme();
    void resetThemeToDefaults();
    void validateAccessibility();
    void toggleScreenReaderMode();
    void showKeyboardShortcuts();
    void testColorContrast();
    void announceCurrentSettings();

private:
    void setupAccessibility();
    void setupUI();
    void setupThemeControls();
    void setupAccessibilityControls();
    void setupKeyboardShortcuts();
    void setupConnections();
    void setupInitialTheme();
    void announceApplicationReady();
    void updateThemePreview();
    void updateAccessibilityInfo();
    void validateColorContrast();
    void announceToUser(const QString& message);
    void logAccessibilityEvent(const QString& event);

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QSplitter* m_mainSplitter{nullptr};
    QTabWidget* m_controlTabs{nullptr};
    QScrollArea* m_previewArea{nullptr};
    
    // Theme Controls
    QGroupBox* m_themeGroup{nullptr};
    QComboBox* m_themeModeCombo{nullptr};
    QComboBox* m_accentColorCombo{nullptr};
    Components::FluentSlider* m_fontSizeSlider{nullptr};
    Components::FluentButton* m_customColorButton{nullptr};
    Components::FluentButton* m_customFontButton{nullptr};
    Components::FluentButton* m_exportButton{nullptr};
    Components::FluentButton* m_importButton{nullptr};
    Components::FluentButton* m_resetButton{nullptr};
    
    // Accessibility Controls
    QGroupBox* m_accessibilityGroup{nullptr};
    Components::FluentCheckBox* m_highContrastCheck{nullptr};
    Components::FluentCheckBox* m_screenReaderCheck{nullptr};
    Components::FluentCheckBox* m_keyboardNavCheck{nullptr};
    Components::FluentCheckBox* m_reducedMotionCheck{nullptr};
    Components::FluentButton* m_validateButton{nullptr};
    Components::FluentButton* m_shortcutsButton{nullptr};
    Components::FluentButton* m_contrastButton{nullptr};
    Components::FluentButton* m_announceButton{nullptr};
    
    // Preview Components
    QWidget* m_previewWidget{nullptr};
    QList<QWidget*> m_previewComponents;
    Components::FluentCard* m_previewCard{nullptr};
    Components::FluentButton* m_previewButton{nullptr};
    Components::FluentTextBox* m_previewTextBox{nullptr};
    Components::FluentComboBox* m_previewCombo{nullptr};
    Components::FluentSlider* m_previewSlider{nullptr};
    Components::FluentSpinBox* m_previewSpinBox{nullptr};
    
    // Information Display
    QTextEdit* m_accessibilityLog{nullptr};
    QLabel* m_contrastRatioLabel{nullptr};
    QLabel* m_wcagComplianceLabel{nullptr};
    QLabel* m_currentThemeLabel{nullptr};
    QProgressBar* m_accessibilityScore{nullptr};
    
    // Keyboard Shortcuts
    QList<QShortcut*> m_shortcuts;
    
    // State
    bool m_screenReaderMode{false};
    bool m_highContrastMode{false};
    QColor m_customAccentColor;
    QFont m_customFont;
    int m_accessibilityScore{100};
};

void AccessibilityThemingExample::setupAccessibility() {
    // Enable accessibility system
    QAccessible::setActive(true);
    
    // Set main window accessibility properties
    setAccessibleName("FluentQt Accessibility and Theming Example");
    setAccessibleDescription("Comprehensive demonstration of accessibility features and theming capabilities in FluentQt");
    
    // Initialize FluentQt accessibility
    Accessibility::initializeAccessibility();
    
    logAccessibilityEvent("Accessibility system initialized");
}

void AccessibilityThemingExample::setupUI() {
    setWindowTitle("FluentQt Accessibility & Theming Showcase");
    setMinimumSize(1000, 700);
    
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    auto* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->addWidget(m_mainSplitter);
    
    // Create control tabs (left side)
    m_controlTabs = new QTabWidget(this);
    m_controlTabs->setMinimumWidth(350);
    m_controlTabs->setMaximumWidth(400);
    
    // Set accessibility for tabs
    Accessibility::setAccessibleName(m_controlTabs, "Control Panels");
    Accessibility::setAccessibleDescription(m_controlTabs, "Tabbed interface for theme and accessibility controls");
    
    setupThemeControls();
    setupAccessibilityControls();
    
    m_mainSplitter->addWidget(m_controlTabs);
    
    // Create preview area (right side)
    m_previewArea = new QScrollArea(this);
    m_previewArea->setWidgetResizable(true);
    m_previewArea->setMinimumWidth(500);
    
    Accessibility::setAccessibleName(m_previewArea, "Theme Preview Area");
    Accessibility::setAccessibleDescription(m_previewArea, "Live preview of theme changes and component appearance");
    
    // Create preview widget
    m_previewWidget = new QWidget();
    auto* previewLayout = new QVBoxLayout(m_previewWidget);
    
    // Create preview card
    m_previewCard = new Components::FluentCard("Theme Preview", this);
    m_previewCard->setSubtitle("Live demonstration of current theme settings");
    Accessibility::setAccessibleName(m_previewCard, "Theme Preview Card");
    
    // Add various components for preview
    auto* componentsLayout = new QGridLayout();
    
    // Button preview
    m_previewButton = new Components::FluentButton("Sample Button", this);
    m_previewButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    Accessibility::setAccessibleName(m_previewButton, "Sample Button");
    Accessibility::setAccessibleDescription(m_previewButton, "Demonstrates button appearance with current theme");
    componentsLayout->addWidget(m_previewButton, 0, 0);
    
    // TextBox preview
    m_previewTextBox = new Components::FluentTextBox(this);
    m_previewTextBox->setPlaceholderText("Enter sample text...");
    Accessibility::setAccessibleName(m_previewTextBox, "Sample Text Input");
    Accessibility::setAccessibleDescription(m_previewTextBox, "Demonstrates text input appearance with current theme");
    componentsLayout->addWidget(m_previewTextBox, 0, 1);
    
    // ComboBox preview
    m_previewCombo = new Components::FluentComboBox(this);
    m_previewCombo->addItems({"Option 1", "Option 2", "Option 3", "Option 4"});
    m_previewCombo->setCurrentIndex(0);
    Accessibility::setAccessibleName(m_previewCombo, "Sample Dropdown");
    Accessibility::setAccessibleDescription(m_previewCombo, "Demonstrates dropdown appearance with current theme");
    componentsLayout->addWidget(m_previewCombo, 1, 0);
    
    // Slider preview
    m_previewSlider = new Components::FluentSlider(Qt::Horizontal, this);
    m_previewSlider->setRange(0, 100);
    m_previewSlider->setValue(50);
    Accessibility::setAccessibleName(m_previewSlider, "Sample Slider");
    Accessibility::setAccessibleDescription(m_previewSlider, "Demonstrates slider appearance with current theme");
    componentsLayout->addWidget(m_previewSlider, 1, 1);
    
    // SpinBox preview
    m_previewSpinBox = new Components::FluentSpinBox(this);
    m_previewSpinBox->setRange(0, 100);
    m_previewSpinBox->setValue(25);
    Accessibility::setAccessibleName(m_previewSpinBox, "Sample Number Input");
    Accessibility::setAccessibleDescription(m_previewSpinBox, "Demonstrates number input appearance with current theme");
    componentsLayout->addWidget(m_previewSpinBox, 2, 0);
    
    // Store preview components for easy access
    m_previewComponents = {m_previewButton, m_previewTextBox, m_previewCombo, 
                          m_previewSlider, m_previewSpinBox};
    
    m_previewCard->setContentWidget(new QWidget());
    m_previewCard->contentWidget()->setLayout(componentsLayout);
    
    previewLayout->addWidget(m_previewCard);
    previewLayout->addStretch();
    
    m_previewArea->setWidget(m_previewWidget);
    m_mainSplitter->addWidget(m_previewArea);
    
    // Set splitter proportions
    m_mainSplitter->setSizes({350, 650});
}

void AccessibilityThemingExample::setupThemeControls() {
    auto* themeTab = new QWidget();
    auto* layout = new QVBoxLayout(themeTab);

    // Theme Mode Group
    m_themeGroup = new QGroupBox("Theme Configuration", this);
    auto* themeLayout = new QVBoxLayout(m_themeGroup);

    // Theme mode selection
    auto* modeLayout = new QHBoxLayout();
    modeLayout->addWidget(new QLabel("Theme Mode:", this));
    m_themeModeCombo = new QComboBox(this);
    m_themeModeCombo->addItems({"Light", "Dark", "System"});
    m_themeModeCombo->setCurrentIndex(0);
    Accessibility::setAccessibleName(m_themeModeCombo, "Theme Mode Selection");
    Accessibility::setAccessibleDescription(m_themeModeCombo, "Choose between light, dark, or system theme mode");
    modeLayout->addWidget(m_themeModeCombo);
    themeLayout->addLayout(modeLayout);

    // Accent color selection
    auto* accentLayout = new QHBoxLayout();
    accentLayout->addWidget(new QLabel("Accent Color:", this));
    m_accentColorCombo = new QComboBox(this);
    m_accentColorCombo->addItems({
        "Blue", "Purple", "Teal", "Green", "Yellow",
        "Orange", "Red", "Pink", "Custom"
    });
    m_accentColorCombo->setCurrentIndex(0);
    Accessibility::setAccessibleName(m_accentColorCombo, "Accent Color Selection");
    Accessibility::setAccessibleDescription(m_accentColorCombo, "Choose the primary accent color for the theme");
    accentLayout->addWidget(m_accentColorCombo);
    themeLayout->addLayout(accentLayout);

    // Font size adjustment
    auto* fontLayout = new QHBoxLayout();
    fontLayout->addWidget(new QLabel("Font Size:", this));
    m_fontSizeSlider = new Components::FluentSlider(Qt::Horizontal, this);
    m_fontSizeSlider->setRange(8, 24);
    m_fontSizeSlider->setValue(14);
    Accessibility::setAccessibleName(m_fontSizeSlider, "Font Size Adjustment");
    Accessibility::setAccessibleDescription(m_fontSizeSlider, "Adjust the base font size from 8 to 24 points");
    fontLayout->addWidget(m_fontSizeSlider);
    auto* fontSizeLabel = new QLabel("14pt", this);
    fontLayout->addWidget(fontSizeLabel);
    themeLayout->addLayout(fontLayout);

    // Connect font size slider to label
    connect(m_fontSizeSlider, &Components::FluentSlider::valueChanged,
            fontSizeLabel, [fontSizeLabel](int value) {
        fontSizeLabel->setText(QString("%1pt").arg(value));
    });

    // Custom color button
    m_customColorButton = new Components::FluentButton("Select Custom Color", this);
    m_customColorButton->setButtonStyle(Components::FluentButtonStyle::Outline);
    Accessibility::setAccessibleName(m_customColorButton, "Custom Color Selector");
    Accessibility::setAccessibleDescription(m_customColorButton, "Open color picker to select a custom accent color");
    themeLayout->addWidget(m_customColorButton);

    // Custom font button
    m_customFontButton = new Components::FluentButton("Select Custom Font", this);
    m_customFontButton->setButtonStyle(Components::FluentButtonStyle::Outline);
    Accessibility::setAccessibleName(m_customFontButton, "Custom Font Selector");
    Accessibility::setAccessibleDescription(m_customFontButton, "Open font dialog to select a custom font family");
    themeLayout->addWidget(m_customFontButton);

    layout->addWidget(m_themeGroup);

    // Theme Management Group
    auto* managementGroup = new QGroupBox("Theme Management", this);
    auto* managementLayout = new QVBoxLayout(managementGroup);

    m_exportButton = new Components::FluentButton("Export Theme", this);
    m_exportButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    Accessibility::setAccessibleName(m_exportButton, "Export Theme");
    Accessibility::setAccessibleDescription(m_exportButton, "Export current theme settings to a file");
    managementLayout->addWidget(m_exportButton);

    m_importButton = new Components::FluentButton("Import Theme", this);
    m_importButton->setButtonStyle(Components::FluentButtonStyle::Default);
    Accessibility::setAccessibleName(m_importButton, "Import Theme");
    Accessibility::setAccessibleDescription(m_importButton, "Import theme settings from a file");
    managementLayout->addWidget(m_importButton);

    m_resetButton = new Components::FluentButton("Reset to Defaults", this);
    m_resetButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
    Accessibility::setAccessibleName(m_resetButton, "Reset Theme");
    Accessibility::setAccessibleDescription(m_resetButton, "Reset all theme settings to default values");
    managementLayout->addWidget(m_resetButton);

    layout->addWidget(managementGroup);

    // Theme Information Group
    auto* infoGroup = new QGroupBox("Theme Information", this);
    auto* infoLayout = new QVBoxLayout(infoGroup);

    m_currentThemeLabel = new QLabel("Current: Light Theme with Blue Accent", this);
    m_currentThemeLabel->setWordWrap(true);
    Accessibility::setAccessibleName(m_currentThemeLabel, "Current Theme Information");
    infoLayout->addWidget(m_currentThemeLabel);

    m_contrastRatioLabel = new QLabel("Contrast Ratio: 4.5:1 (WCAG AA)", this);
    Accessibility::setAccessibleName(m_contrastRatioLabel, "Color Contrast Information");
    infoLayout->addWidget(m_contrastRatioLabel);

    m_wcagComplianceLabel = new QLabel("WCAG Compliance: AA Level", this);
    Accessibility::setAccessibleName(m_wcagComplianceLabel, "WCAG Compliance Status");
    infoLayout->addWidget(m_wcagComplianceLabel);

    layout->addWidget(infoGroup);
    layout->addStretch();

    m_controlTabs->addTab(themeTab, "Theme");
}

void AccessibilityThemingExample::setupAccessibilityControls() {
    auto* accessibilityTab = new QWidget();
    auto* layout = new QVBoxLayout(accessibilityTab);

    // Accessibility Features Group
    m_accessibilityGroup = new QGroupBox("Accessibility Features", this);
    auto* accessibilityLayout = new QVBoxLayout(m_accessibilityGroup);

    // High contrast mode
    m_highContrastCheck = new Components::FluentCheckBox("Enable High Contrast Mode", this);
    Accessibility::setAccessibleName(m_highContrastCheck, "High Contrast Mode Toggle");
    Accessibility::setAccessibleDescription(m_highContrastCheck, "Enable high contrast colors for better visibility");
    accessibilityLayout->addWidget(m_highContrastCheck);

    // Screen reader mode
    m_screenReaderCheck = new Components::FluentCheckBox("Screen Reader Mode", this);
    Accessibility::setAccessibleName(m_screenReaderCheck, "Screen Reader Mode Toggle");
    Accessibility::setAccessibleDescription(m_screenReaderCheck, "Enable enhanced screen reader support and announcements");
    accessibilityLayout->addWidget(m_screenReaderCheck);

    // Keyboard navigation
    m_keyboardNavCheck = new Components::FluentCheckBox("Enhanced Keyboard Navigation", this);
    m_keyboardNavCheck->setChecked(true);
    Accessibility::setAccessibleName(m_keyboardNavCheck, "Keyboard Navigation Toggle");
    Accessibility::setAccessibleDescription(m_keyboardNavCheck, "Enable enhanced keyboard navigation features");
    accessibilityLayout->addWidget(m_keyboardNavCheck);

    // Reduced motion
    m_reducedMotionCheck = new Components::FluentCheckBox("Respect Reduced Motion", this);
    m_reducedMotionCheck->setChecked(true);
    Accessibility::setAccessibleName(m_reducedMotionCheck, "Reduced Motion Toggle");
    Accessibility::setAccessibleDescription(m_reducedMotionCheck, "Reduce or disable animations for users with motion sensitivity");
    accessibilityLayout->addWidget(m_reducedMotionCheck);

    layout->addWidget(m_accessibilityGroup);

    // Accessibility Tools Group
    auto* toolsGroup = new QGroupBox("Accessibility Tools", this);
    auto* toolsLayout = new QVBoxLayout(toolsGroup);

    m_validateButton = new Components::FluentButton("Validate Accessibility", this);
    m_validateButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    Accessibility::setAccessibleName(m_validateButton, "Accessibility Validator");
    Accessibility::setAccessibleDescription(m_validateButton, "Run accessibility validation checks on the current interface");
    toolsLayout->addWidget(m_validateButton);

    m_shortcutsButton = new Components::FluentButton("Show Keyboard Shortcuts", this);
    m_shortcutsButton->setButtonStyle(Components::FluentButtonStyle::Default);
    Accessibility::setAccessibleName(m_shortcutsButton, "Keyboard Shortcuts Help");
    Accessibility::setAccessibleDescription(m_shortcutsButton, "Display available keyboard shortcuts and navigation help");
    toolsLayout->addWidget(m_shortcutsButton);

    m_contrastButton = new Components::FluentButton("Test Color Contrast", this);
    m_contrastButton->setButtonStyle(Components::FluentButtonStyle::Default);
    Accessibility::setAccessibleName(m_contrastButton, "Color Contrast Tester");
    Accessibility::setAccessibleDescription(m_contrastButton, "Test color contrast ratios for WCAG compliance");
    toolsLayout->addWidget(m_contrastButton);

    m_announceButton = new Components::FluentButton("Announce Current Settings", this);
    m_announceButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
    Accessibility::setAccessibleName(m_announceButton, "Settings Announcer");
    Accessibility::setAccessibleDescription(m_announceButton, "Announce current theme and accessibility settings to screen readers");
    toolsLayout->addWidget(m_announceButton);

    layout->addWidget(toolsGroup);

    // Accessibility Score Group
    auto* scoreGroup = new QGroupBox("Accessibility Score", this);
    auto* scoreLayout = new QVBoxLayout(scoreGroup);

    m_accessibilityScore = new QProgressBar(this);
    m_accessibilityScore->setRange(0, 100);
    m_accessibilityScore->setValue(85);
    m_accessibilityScore->setFormat("Accessibility Score: %p%");
    Accessibility::setAccessibleName(m_accessibilityScore, "Accessibility Score");
    Accessibility::setAccessibleDescription(m_accessibilityScore, "Current accessibility compliance score out of 100");
    scoreLayout->addWidget(m_accessibilityScore);

    layout->addWidget(scoreGroup);

    // Accessibility Log Group
    auto* logGroup = new QGroupBox("Accessibility Log", this);
    auto* logLayout = new QVBoxLayout(logGroup);

    m_accessibilityLog = new QTextEdit(this);
    m_accessibilityLog->setMaximumHeight(150);
    m_accessibilityLog->setReadOnly(true);
    Accessibility::setAccessibleName(m_accessibilityLog, "Accessibility Event Log");
    Accessibility::setAccessibleDescription(m_accessibilityLog, "Log of accessibility events and announcements");
    logLayout->addWidget(m_accessibilityLog);

    layout->addWidget(logGroup);
    layout->addStretch();

    m_controlTabs->addTab(accessibilityTab, "Accessibility");
}

void AccessibilityThemingExample::setupKeyboardShortcuts() {
    // Theme shortcuts
    auto* themeShortcut = new QShortcut(QKeySequence("Ctrl+T"), this);
    connect(themeShortcut, &QShortcut::activated, [this]() {
        m_controlTabs->setCurrentIndex(0);
        announceToUser("Switched to Theme tab");
    });
    m_shortcuts.append(themeShortcut);

    // Accessibility shortcuts
    auto* accessibilityShortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
    connect(accessibilityShortcut, &QShortcut::activated, [this]() {
        m_controlTabs->setCurrentIndex(1);
        announceToUser("Switched to Accessibility tab");
    });
    m_shortcuts.append(accessibilityShortcut);

    // High contrast toggle
    auto* contrastShortcut = new QShortcut(QKeySequence("Ctrl+H"), this);
    connect(contrastShortcut, &QShortcut::activated, [this]() {
        m_highContrastCheck->setChecked(!m_highContrastCheck->isChecked());
    });
    m_shortcuts.append(contrastShortcut);

    // Theme mode cycling
    auto* modeShortcut = new QShortcut(QKeySequence("Ctrl+M"), this);
    connect(modeShortcut, &QShortcut::activated, [this]() {
        int currentIndex = m_themeModeCombo->currentIndex();
        int nextIndex = (currentIndex + 1) % m_themeModeCombo->count();
        m_themeModeCombo->setCurrentIndex(nextIndex);
    });
    m_shortcuts.append(modeShortcut);

    // Announce settings
    auto* announceShortcut = new QShortcut(QKeySequence("Ctrl+Shift+A"), this);
    connect(announceShortcut, &QShortcut::activated, this, &AccessibilityThemingExample::announceCurrentSettings);
    m_shortcuts.append(announceShortcut);

    // Show shortcuts help
    auto* helpShortcut = new QShortcut(QKeySequence("F1"), this);
    connect(helpShortcut, &QShortcut::activated, this, &AccessibilityThemingExample::showKeyboardShortcuts);
    m_shortcuts.append(helpShortcut);
}

void AccessibilityThemingExample::setupConnections() {
    // Theme controls
    connect(m_themeModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AccessibilityThemingExample::onThemeModeChanged);
    connect(m_accentColorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AccessibilityThemingExample::onAccentColorChanged);
    connect(m_fontSizeSlider, &Components::FluentSlider::valueChanged,
            this, &AccessibilityThemingExample::onFontSizeChanged);
    connect(m_customColorButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::onCustomColorSelected);
    connect(m_customFontButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::onCustomFontSelected);
    connect(m_exportButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::exportCurrentTheme);
    connect(m_importButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::importTheme);
    connect(m_resetButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::resetThemeToDefaults);

    // Accessibility controls
    connect(m_highContrastCheck, &Components::FluentCheckBox::toggled,
            this, &AccessibilityThemingExample::onHighContrastToggled);
    connect(m_screenReaderCheck, &Components::FluentCheckBox::toggled,
            this, &AccessibilityThemingExample::toggleScreenReaderMode);
    connect(m_validateButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::validateAccessibility);
    connect(m_shortcutsButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::showKeyboardShortcuts);
    connect(m_contrastButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::testColorContrast);
    connect(m_announceButton, &Components::FluentButton::clicked,
            this, &AccessibilityThemingExample::announceCurrentSettings);

    // Theme change notifications
    auto& theme = Styling::FluentTheme::instance();
    connect(&theme, &Styling::FluentTheme::themeChanged, this, &AccessibilityThemingExample::updateThemePreview);
    connect(&theme, &Styling::FluentTheme::themeChanged, this, &AccessibilityThemingExample::updateAccessibilityInfo);
}

void AccessibilityThemingExample::setupInitialTheme() {
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::Light);
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    updateThemePreview();
    updateAccessibilityInfo();
}

void AccessibilityThemingExample::announceApplicationReady() {
    QTimer::singleShot(500, [this]() {
        announceToUser("FluentQt Accessibility and Theming Example loaded. Press F1 for keyboard shortcuts.");
        logAccessibilityEvent("Application ready - all accessibility features initialized");
    });
}

// Slot implementations
void AccessibilityThemingExample::onThemeModeChanged(int index) {
    auto& theme = Styling::FluentTheme::instance();

    switch (index) {
        case 0: // Light
            theme.setMode(Styling::FluentThemeMode::Light);
            announceToUser("Switched to Light theme mode");
            break;
        case 1: // Dark
            theme.setMode(Styling::FluentThemeMode::Dark);
            announceToUser("Switched to Dark theme mode");
            break;
        case 2: // System
            theme.setMode(Styling::FluentThemeMode::System);
            announceToUser("Switched to System theme mode");
            break;
    }

    logAccessibilityEvent(QString("Theme mode changed to: %1").arg(m_themeModeCombo->currentText()));
}

void AccessibilityThemingExample::onAccentColorChanged(int index) {
    auto& theme = Styling::FluentTheme::instance();

    if (index == 8) { // Custom color
        if (m_customAccentColor.isValid()) {
            // Apply custom color (would need custom implementation)
            announceToUser("Applied custom accent color");
        } else {
            onCustomColorSelected();
        }
        return;
    }

    // Standard accent colors
    const QList<Styling::FluentAccentColor> colors = {
        Styling::FluentAccentColor::Blue,
        Styling::FluentAccentColor::Purple,
        Styling::FluentAccentColor::Teal,
        Styling::FluentAccentColor::Green,
        Styling::FluentAccentColor::Yellow,
        Styling::FluentAccentColor::Orange,
        Styling::FluentAccentColor::Red,
        Styling::FluentAccentColor::Pink
    };

    if (index >= 0 && index < colors.size()) {
        theme.setAccentColor(colors[index]);
        announceToUser(QString("Changed accent color to %1").arg(m_accentColorCombo->currentText()));
        logAccessibilityEvent(QString("Accent color changed to: %1").arg(m_accentColorCombo->currentText()));
    }
}

void AccessibilityThemingExample::onHighContrastToggled(bool enabled) {
    auto& theme = Styling::FluentTheme::instance();
    theme.setHighContrastMode(enabled);
    m_highContrastMode = enabled;

    announceToUser(enabled ? "High contrast mode enabled" : "High contrast mode disabled");
    logAccessibilityEvent(QString("High contrast mode %1").arg(enabled ? "enabled" : "disabled"));

    // Update accessibility score
    m_accessibilityScore->setValue(enabled ? 95 : 85);
}

void AccessibilityThemingExample::onFontSizeChanged(int size) {
    // Apply font size change to application
    QFont appFont = QApplication::font();
    appFont.setPointSize(size);
    QApplication::setFont(appFont);

    announceToUser(QString("Font size changed to %1 points").arg(size));
    logAccessibilityEvent(QString("Font size changed to: %1pt").arg(size));
}

void AccessibilityThemingExample::onCustomColorSelected() {
    QColor color = QColorDialog::getColor(m_customAccentColor.isValid() ? m_customAccentColor : Qt::blue,
                                         this, "Select Custom Accent Color");
    if (color.isValid()) {
        m_customAccentColor = color;
        m_accentColorCombo->setCurrentIndex(8); // Custom option
        announceToUser("Custom accent color selected");
        logAccessibilityEvent(QString("Custom accent color selected: %1").arg(color.name()));
    }
}

void AccessibilityThemingExample::onCustomFontSelected() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_customFont.isValid() ? m_customFont : QApplication::font(),
                                     this, "Select Custom Font");
    if (ok) {
        m_customFont = font;
        QApplication::setFont(font);
        announceToUser(QString("Custom font selected: %1").arg(font.family()));
        logAccessibilityEvent(QString("Custom font selected: %1 %2pt").arg(font.family()).arg(font.pointSize()));
    }
}

void AccessibilityThemingExample::exportCurrentTheme() {
    auto& theme = Styling::FluentTheme::instance();
    QString themeData = theme.exportTheme();

    // In a real implementation, this would save to a file
    m_accessibilityLog->append("Theme exported successfully");
    announceToUser("Current theme exported");
    logAccessibilityEvent("Theme configuration exported");
}

void AccessibilityThemingExample::importTheme() {
    // In a real implementation, this would load from a file
    announceToUser("Theme import functionality would open file dialog");
    logAccessibilityEvent("Theme import requested");
}

void AccessibilityThemingExample::resetThemeToDefaults() {
    auto& theme = Styling::FluentTheme::instance();
    theme.resetToDefaults();

    // Reset UI controls
    m_themeModeCombo->setCurrentIndex(0);
    m_accentColorCombo->setCurrentIndex(0);
    m_fontSizeSlider->setValue(14);
    m_highContrastCheck->setChecked(false);

    // Reset custom values
    m_customAccentColor = QColor();
    m_customFont = QFont();

    announceToUser("Theme reset to default settings");
    logAccessibilityEvent("Theme reset to defaults");
}

void AccessibilityThemingExample::validateAccessibility() {
    // Perform accessibility validation
    int score = 85;
    QStringList issues;

    // Check contrast ratios
    auto& theme = Styling::FluentTheme::instance();
    if (!theme.isHighContrastMode()) {
        issues.append("Consider enabling high contrast mode for better visibility");
        score -= 5;
    }

    // Check font size
    if (m_fontSizeSlider->value() < 12) {
        issues.append("Font size below recommended minimum of 12pt");
        score -= 10;
    }

    // Check keyboard navigation
    if (!m_keyboardNavCheck->isChecked()) {
        issues.append("Enhanced keyboard navigation is disabled");
        score -= 10;
    }

    // Update score
    m_accessibilityScore->setValue(score);

    // Report results
    QString report = QString("Accessibility validation complete. Score: %1/100").arg(score);
    if (!issues.isEmpty()) {
        report += "\nIssues found:\n• " + issues.join("\n• ");
    }

    m_accessibilityLog->append(report);
    announceToUser(QString("Accessibility validation complete. Score: %1 out of 100").arg(score));
    logAccessibilityEvent(QString("Accessibility validation completed - Score: %1").arg(score));
}

void AccessibilityThemingExample::toggleScreenReaderMode(bool enabled) {
    m_screenReaderMode = enabled;

    if (enabled) {
        // Enable enhanced screen reader features
        announceToUser("Screen reader mode enabled. Enhanced announcements activated.");
        logAccessibilityEvent("Screen reader mode enabled");
    } else {
        announceToUser("Screen reader mode disabled");
        logAccessibilityEvent("Screen reader mode disabled");
    }
}

void AccessibilityThemingExample::showKeyboardShortcuts() {
    QString shortcuts =
        "Keyboard Shortcuts:\n\n"
        "Ctrl+T - Switch to Theme tab\n"
        "Ctrl+A - Switch to Accessibility tab\n"
        "Ctrl+H - Toggle high contrast mode\n"
        "Ctrl+M - Cycle theme mode\n"
        "Ctrl+Shift+A - Announce current settings\n"
        "F1 - Show this help\n\n"
        "Navigation:\n"
        "Tab - Move to next control\n"
        "Shift+Tab - Move to previous control\n"
        "Space - Activate buttons and checkboxes\n"
        "Enter - Activate default button\n"
        "Arrow keys - Navigate within controls";

    m_accessibilityLog->setPlainText(shortcuts);
    announceToUser("Keyboard shortcuts displayed in accessibility log");
    logAccessibilityEvent("Keyboard shortcuts help displayed");
}

void AccessibilityThemingExample::testColorContrast() {
    auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Test common color combinations
    QStringList results;

    // Text on background
    qreal textBgRatio = theme.contrastRatio(palette.textPrimary, palette.backgroundPrimary);
    results.append(QString("Text/Background: %1:1 (%2)")
                  .arg(textBgRatio, 0, 'f', 1)
                  .arg(textBgRatio >= 4.5 ? "PASS" : "FAIL"));

    // Accent on background
    QColor accentColor = theme.color("accent");
    qreal accentBgRatio = theme.contrastRatio(accentColor, palette.backgroundPrimary);
    results.append(QString("Accent/Background: %1:1 (%2)")
                  .arg(accentBgRatio, 0, 'f', 1)
                  .arg(accentBgRatio >= 3.0 ? "PASS" : "FAIL"));

    QString report = "Color Contrast Test Results:\n" + results.join("\n");
    m_accessibilityLog->append(report);

    announceToUser("Color contrast test completed. Results displayed in log.");
    logAccessibilityEvent("Color contrast validation performed");
}

void AccessibilityThemingExample::announceCurrentSettings() {
    QString settings = QString(
        "Current settings: %1 theme mode, %2 accent color, %3 point font size. "
        "High contrast %4, Screen reader mode %5."
    ).arg(m_themeModeCombo->currentText())
     .arg(m_accentColorCombo->currentText())
     .arg(m_fontSizeSlider->value())
     .arg(m_highContrastCheck->isChecked() ? "enabled" : "disabled")
     .arg(m_screenReaderCheck->isChecked() ? "enabled" : "disabled");

    announceToUser(settings);
    logAccessibilityEvent("Current settings announced");
}

// Utility methods
void AccessibilityThemingExample::updateThemePreview() {
    // Update theme information display
    auto& theme = Styling::FluentTheme::instance();
    QString modeText = m_themeModeCombo->currentText();
    QString accentText = m_accentColorCombo->currentText();

    m_currentThemeLabel->setText(QString("Current: %1 Theme with %2 Accent").arg(modeText, accentText));

    // Update preview components with current theme
    for (auto* component : m_previewComponents) {
        component->update();
    }

    validateColorContrast();
    logAccessibilityEvent("Theme preview updated");
}

void AccessibilityThemingExample::updateAccessibilityInfo() {
    validateColorContrast();

    // Update WCAG compliance status
    auto& theme = Styling::FluentTheme::instance();
    bool highContrast = theme.isHighContrastMode();
    int fontSize = m_fontSizeSlider->value();

    QString compliance = "AA Level";
    if (highContrast && fontSize >= 14) {
        compliance = "AAA Level";
    } else if (fontSize < 12) {
        compliance = "Below AA";
    }

    m_wcagComplianceLabel->setText(QString("WCAG Compliance: %1").arg(compliance));
    logAccessibilityEvent(QString("Accessibility info updated - WCAG: %1").arg(compliance));
}

void AccessibilityThemingExample::validateColorContrast() {
    auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Calculate primary text contrast ratio
    qreal ratio = theme.contrastRatio(palette.textPrimary, palette.backgroundPrimary);

    QString status;
    if (ratio >= 7.0) {
        status = "AAA";
    } else if (ratio >= 4.5) {
        status = "AA";
    } else if (ratio >= 3.0) {
        status = "A";
    } else {
        status = "FAIL";
    }

    m_contrastRatioLabel->setText(QString("Contrast Ratio: %1:1 (WCAG %2)")
                                 .arg(ratio, 0, 'f', 1)
                                 .arg(status));
}

void AccessibilityThemingExample::announceToUser(const QString& message) {
    if (m_screenReaderMode || QAccessible::isActive()) {
        Accessibility::announceToScreenReader(message);
    }

    // Also log the announcement
    logAccessibilityEvent(QString("Announced: %1").arg(message));
}

void AccessibilityThemingExample::logAccessibilityEvent(const QString& event) {
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, event);

    m_accessibilityLog->append(logEntry);

    // Keep log size manageable
    if (m_accessibilityLog->document()->lineCount() > 50) {
        QTextCursor cursor = m_accessibilityLog->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 10);
        cursor.removeSelectedText();
    }

    // Auto-scroll to bottom
    QTextCursor cursor = m_accessibilityLog->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_accessibilityLog->setTextCursor(cursor);
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    // Enable accessibility
    QAccessible::setActive(true);

    AccessibilityThemingExample example;
    example.show();

    return app.exec();
}

#include "AccessibilityThemingExample.moc"
