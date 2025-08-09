// examples/AdvancedThemingExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QGroupBox>
#include <QColorDialog>
#include <QProgressBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QSpinBox>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Styling/FluentAdvancedThemeManager.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

using namespace FluentQt;

class ColorPaletteWidget : public QWidget {
    Q_OBJECT

public:
    explicit ColorPaletteWidget(const QString& title, QWidget* parent = nullptr)
        : QWidget(parent), m_title(title) {
        setupUI();
    }
    
    void setPalette(const Styling::FluentColorPalette& palette) {
        m_palette = palette;
        updateColors();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Title
        auto* titleLabel = new QLabel(m_title, this);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 8px;");
        layout->addWidget(titleLabel);
        
        // Color grid
        m_colorGrid = new QGridLayout();
        m_colorGrid->setSpacing(4);
        layout->addLayout(m_colorGrid);
        
        // Create color swatches
        createColorSwatches();
    }
    
    void createColorSwatches() {
        m_colorSwatches.clear();
        
        // Clear existing layout
        QLayoutItem* item;
        while ((item = m_colorGrid->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        
        // Color definitions with names
        QList<QPair<QString, QColor*>> colorDefs = {
            {"Accent", nullptr}, {"Accent L1", nullptr}, {"Accent L2", nullptr},
            {"Success", nullptr}, {"Warning", nullptr}, {"Error", nullptr},
            {"Neutral Light", nullptr}, {"Neutral", nullptr}, {"Neutral Dark", nullptr}
        };
        
        int row = 0, col = 0;
        for (const auto& colorDef : colorDefs) {
            auto* colorSwatch = createColorSwatch(colorDef.first, QColor(128, 128, 128));
            m_colorSwatches[colorDef.first] = colorSwatch;
            m_colorGrid->addWidget(colorSwatch, row, col);
            
            col++;
            if (col >= 3) {
                col = 0;
                row++;
            }
        }
    }
    
    QWidget* createColorSwatch(const QString& name, const QColor& color) {
        auto* swatch = new QWidget(this);
        swatch->setFixedSize(80, 60);
        swatch->setStyleSheet(QString(
            "QWidget { "
            "  background-color: %1; "
            "  border: 1px solid #ccc; "
            "  border-radius: 4px; "
            "}"
        ).arg(color.name()));
        
        auto* layout = new QVBoxLayout(swatch);
        layout->setContentsMargins(4, 4, 4, 4);
        
        auto* nameLabel = new QLabel(name, swatch);
        nameLabel->setStyleSheet("color: white; font-size: 10px; font-weight: bold; background: rgba(0,0,0,0.5); padding: 2px; border-radius: 2px;");
        nameLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(nameLabel);
        
        layout->addStretch();
        
        auto* colorLabel = new QLabel(color.name(), swatch);
        colorLabel->setStyleSheet("color: white; font-size: 9px; background: rgba(0,0,0,0.5); padding: 1px; border-radius: 2px;");
        colorLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(colorLabel);
        
        return swatch;
    }
    
    void updateColors() {
        // Update color swatches with palette colors
        updateColorSwatch("Accent", m_palette.accent);
        updateColorSwatch("Accent L1", m_palette.accentLight1);
        updateColorSwatch("Accent L2", m_palette.accentLight2);
        updateColorSwatch("Success", m_palette.success);
        updateColorSwatch("Warning", m_palette.warning);
        updateColorSwatch("Error", m_palette.error);
        updateColorSwatch("Neutral Light", m_palette.neutralLight);
        updateColorSwatch("Neutral", m_palette.neutralSecondary);
        updateColorSwatch("Neutral Dark", m_palette.neutralDark);
    }
    
    void updateColorSwatch(const QString& name, const QColor& color) {
        auto it = m_colorSwatches.find(name);
        if (it != m_colorSwatches.end()) {
            QWidget* swatch = it.value();
            swatch->setStyleSheet(QString(
                "QWidget { "
                "  background-color: %1; "
                "  border: 1px solid #ccc; "
                "  border-radius: 4px; "
                "}"
            ).arg(color.name()));
            
            // Update color label
            auto* colorLabel = swatch->findChild<QLabel*>();
            if (colorLabel && colorLabel->text().startsWith("#")) {
                colorLabel->setText(color.name());
            }
        }
    }

private:
    QString m_title;
    Styling::FluentColorPalette m_palette;
    QGridLayout* m_colorGrid;
    QMap<QString, QWidget*> m_colorSwatches;
};

class ThemeControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit ThemeControlPanel(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
        updateFromCurrentTheme();
    }

signals:
    void themeChanged();
    void paletteGenerated(const Styling::FluentColorPalette& palette);

private slots:
    void onBaseColorChanged() {
        QColor color = QColorDialog::getColor(m_currentBaseColor, this, "Select Base Color");
        if (color.isValid()) {
            m_currentBaseColor = color;
            m_baseColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; border-radius: 4px;").arg(color.name()));
            generatePalette();
        }
    }
    
    void onAlgorithmChanged() {
        generatePalette();
    }
    
    void onHarmonyChanged() {
        generatePalette();
    }
    
    void onContrastChanged() {
        m_contrastLabel->setText(QString("Contrast: %1:1").arg(m_contrastSlider->value() / 10.0, 0, 'f', 1));
        generatePalette();
    }
    
    void onAccessibilityToggled() {
        generatePalette();
    }
    
    void onThemeModeChanged() {
        auto& theme = Styling::FluentTheme::instance();
        
        QString mode = m_themeModeCombo->currentText();
        if (mode == "Light") {
            theme.setMode(Styling::FluentThemeMode::Light);
        } else if (mode == "Dark") {
            theme.setMode(Styling::FluentThemeMode::Dark);
        } else if (mode == "High Contrast") {
            theme.setMode(Styling::FluentThemeMode::HighContrast);
        } else {
            theme.setMode(Styling::FluentThemeMode::System);
        }
        
        emit themeChanged();
    }
    
    void onVariantChanged() {
        auto& theme = Styling::FluentTheme::instance();
        
        QString variant = m_variantCombo->currentText();
        if (variant == "Compact") {
            theme.setVariant(Styling::FluentThemeVariant::Compact);
        } else if (variant == "Touch") {
            theme.setVariant(Styling::FluentThemeVariant::Touch);
        } else if (variant == "Dense") {
            theme.setVariant(Styling::FluentThemeVariant::Dense);
        } else if (variant == "Accessible") {
            theme.setVariant(Styling::FluentThemeVariant::Accessible);
        } else {
            theme.setVariant(Styling::FluentThemeVariant::Default);
        }
        
        emit themeChanged();
    }
    
    void onDynamicThemingToggled(bool enabled) {
        auto& themeManager = Styling::FluentAdvancedThemeManager::instance();
        
        if (enabled) {
            Styling::FluentDynamicThemeConfig config;
            config.adaptationMode = Styling::FluentThemeAdaptation::TimeOfDay;
            config.updateInterval = 300000; // 5 minutes
            config.enableSmoothing = true;
            themeManager.enableDynamicTheming(config);
        } else {
            themeManager.disableDynamicTheming();
        }
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Base color selection
        auto* colorGroup = new QGroupBox("Base Color", this);
        auto* colorLayout = new QHBoxLayout(colorGroup);
        
        m_baseColorButton = new QPushButton("Select Color", this);
        m_baseColorButton->setFixedSize(100, 30);
        m_currentBaseColor = QColor(0, 120, 215);
        m_baseColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; border-radius: 4px;").arg(m_currentBaseColor.name()));
        
        colorLayout->addWidget(new QLabel("Base Color:", this));
        colorLayout->addWidget(m_baseColorButton);
        colorLayout->addStretch();
        
        // Algorithm selection
        auto* algorithmGroup = new QGroupBox("Generation Algorithm", this);
        auto* algorithmLayout = new QVBoxLayout(algorithmGroup);
        
        m_algorithmCombo = new QComboBox(this);
        m_algorithmCombo->addItems({"Fluent Design", "Material Design", "Accessible", "Monochromatic", "Analogous", "Complementary", "Triadic"});
        
        m_harmonyCombo = new QComboBox(this);
        m_harmonyCombo->addItems({"Analogous", "Monochromatic", "Complementary", "Triadic", "Tetradic", "Split Complementary"});
        
        algorithmLayout->addWidget(new QLabel("Algorithm:", this));
        algorithmLayout->addWidget(m_algorithmCombo);
        algorithmLayout->addWidget(new QLabel("Color Harmony:", this));
        algorithmLayout->addWidget(m_harmonyCombo);
        
        // Contrast ratio
        auto* contrastGroup = new QGroupBox("Accessibility", this);
        auto* contrastLayout = new QVBoxLayout(contrastGroup);
        
        m_contrastSlider = new QSlider(Qt::Horizontal, this);
        m_contrastSlider->setRange(30, 100); // 3.0 to 10.0
        m_contrastSlider->setValue(45); // 4.5
        
        m_contrastLabel = new QLabel("Contrast: 4.5:1", this);
        
        m_accessibilityCheck = new QCheckBox("Generate Accessible Variants", this);
        m_accessibilityCheck->setChecked(true);
        
        contrastLayout->addWidget(m_contrastLabel);
        contrastLayout->addWidget(m_contrastSlider);
        contrastLayout->addWidget(m_accessibilityCheck);
        
        // Theme settings
        auto* themeGroup = new QGroupBox("Theme Settings", this);
        auto* themeLayout = new QVBoxLayout(themeGroup);
        
        m_themeModeCombo = new QComboBox(this);
        m_themeModeCombo->addItems({"System", "Light", "Dark", "High Contrast"});
        
        m_variantCombo = new QComboBox(this);
        m_variantCombo->addItems({"Default", "Compact", "Touch", "Dense", "Accessible"});
        
        m_dynamicThemingCheck = new QCheckBox("Enable Dynamic Theming", this);
        
        themeLayout->addWidget(new QLabel("Theme Mode:", this));
        themeLayout->addWidget(m_themeModeCombo);
        themeLayout->addWidget(new QLabel("Theme Variant:", this));
        themeLayout->addWidget(m_variantCombo);
        themeLayout->addWidget(m_dynamicThemingCheck);
        
        // Action buttons
        auto* buttonLayout = new QHBoxLayout();
        
        m_generateButton = new QPushButton("Generate Palette", this);
        m_exportButton = new QPushButton("Export Theme", this);
        m_importButton = new QPushButton("Import Theme", this);
        
        buttonLayout->addWidget(m_generateButton);
        buttonLayout->addWidget(m_exportButton);
        buttonLayout->addWidget(m_importButton);
        
        // Add all groups to main layout
        layout->addWidget(colorGroup);
        layout->addWidget(algorithmGroup);
        layout->addWidget(contrastGroup);
        layout->addWidget(themeGroup);
        layout->addLayout(buttonLayout);
        layout->addStretch();
    }
    
    void connectSignals() {
        connect(m_baseColorButton, &QPushButton::clicked, this, &ThemeControlPanel::onBaseColorChanged);
        connect(m_algorithmCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ThemeControlPanel::onAlgorithmChanged);
        connect(m_harmonyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ThemeControlPanel::onHarmonyChanged);
        connect(m_contrastSlider, &QSlider::valueChanged, this, &ThemeControlPanel::onContrastChanged);
        connect(m_accessibilityCheck, &QCheckBox::toggled, this, &ThemeControlPanel::onAccessibilityToggled);
        connect(m_themeModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ThemeControlPanel::onThemeModeChanged);
        connect(m_variantCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ThemeControlPanel::onVariantChanged);
        connect(m_dynamicThemingCheck, &QCheckBox::toggled, this, &ThemeControlPanel::onDynamicThemingToggled);
        connect(m_generateButton, &QPushButton::clicked, this, &ThemeControlPanel::generatePalette);
    }
    
    void updateFromCurrentTheme() {
        auto& theme = Styling::FluentTheme::instance();
        
        // Update theme mode combo
        switch (theme.mode()) {
            case Styling::FluentThemeMode::Light:
                m_themeModeCombo->setCurrentText("Light");
                break;
            case Styling::FluentThemeMode::Dark:
                m_themeModeCombo->setCurrentText("Dark");
                break;
            case Styling::FluentThemeMode::HighContrast:
                m_themeModeCombo->setCurrentText("High Contrast");
                break;
            default:
                m_themeModeCombo->setCurrentText("System");
                break;
        }
    }
    
    void generatePalette() {
        auto& themeManager = Styling::FluentAdvancedThemeManager::instance();
        
        // Create generation config
        Styling::FluentThemeGenerationConfig config;
        config.baseColor = m_currentBaseColor;
        config.contrastRatio = m_contrastSlider->value() / 10.0;
        config.generateAccessibleVariants = m_accessibilityCheck->isChecked();
        
        // Set algorithm
        QString algorithm = m_algorithmCombo->currentText();
        if (algorithm == "Material Design") {
            config.algorithm = Styling::FluentThemeGenerationAlgorithm::MaterialDesign;
        } else if (algorithm == "Accessible") {
            config.algorithm = Styling::FluentThemeGenerationAlgorithm::Accessible;
        } else if (algorithm == "Monochromatic") {
            config.algorithm = Styling::FluentThemeGenerationAlgorithm::Monochromatic;
        } else if (algorithm == "Analogous") {
            config.algorithm = Styling::FluentThemeGenerationAlgorithm::Analogous;
        } else if (algorithm == "Complementary") {
            config.algorithm = Styling::FluentThemeGenerationAlgorithm::Complementary;
        } else if (algorithm == "Triadic") {
            config.algorithm = Styling::FluentThemeGenerationAlgorithm::Triadic;
        } else {
            config.algorithm = Styling::FluentThemeGenerationAlgorithm::FluentDesign;
        }
        
        // Set harmony
        QString harmony = m_harmonyCombo->currentText();
        if (harmony == "Monochromatic") {
            config.harmony = Styling::FluentColorHarmony::Monochromatic;
        } else if (harmony == "Complementary") {
            config.harmony = Styling::FluentColorHarmony::Complementary;
        } else if (harmony == "Triadic") {
            config.harmony = Styling::FluentColorHarmony::Triadic;
        } else if (harmony == "Tetradic") {
            config.harmony = Styling::FluentColorHarmony::Tetradic;
        } else if (harmony == "Split Complementary") {
            config.harmony = Styling::FluentColorHarmony::SplitComplementary;
        } else {
            config.harmony = Styling::FluentColorHarmony::Analogous;
        }
        
        // Generate palette
        Styling::FluentColorPalette palette = themeManager.generateColorPalette(config);
        
        emit paletteGenerated(palette);
    }

private:
    QPushButton* m_baseColorButton;
    QComboBox* m_algorithmCombo;
    QComboBox* m_harmonyCombo;
    QSlider* m_contrastSlider;
    QLabel* m_contrastLabel;
    QCheckBox* m_accessibilityCheck;
    QComboBox* m_themeModeCombo;
    QComboBox* m_variantCombo;
    QCheckBox* m_dynamicThemingCheck;
    QPushButton* m_generateButton;
    QPushButton* m_exportButton;
    QPushButton* m_importButton;
    
    QColor m_currentBaseColor;
};

class AdvancedThemingMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdvancedThemingMainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        connectSignals();
        
        // Generate initial palette
        m_controlPanel->generatePalette();
    }

private slots:
    void onThemeChanged() {
        updateThemeDisplay();
    }
    
    void onPaletteGenerated(const Styling::FluentColorPalette& palette) {
        m_lightPaletteWidget->setPalette(palette);
        m_darkPaletteWidget->setPalette(palette);
        updateValidationResults(palette);
    }

private:
    void setupUI() {
        setWindowTitle("FluentQt Advanced Theming Demo");
        setMinimumSize(1200, 800);
        
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QHBoxLayout(centralWidget);
        
        // Left panel - Controls
        m_controlPanel = new ThemeControlPanel(this);
        m_controlPanel->setMaximumWidth(300);
        
        // Right panel - Preview
        auto* previewWidget = new QWidget(this);
        auto* previewLayout = new QVBoxLayout(previewWidget);
        
        // Palette preview
        auto* paletteTabWidget = new QTabWidget(this);
        
        m_lightPaletteWidget = new ColorPaletteWidget("Light Theme Palette", this);
        m_darkPaletteWidget = new ColorPaletteWidget("Dark Theme Palette", this);
        
        paletteTabWidget->addTab(m_lightPaletteWidget, "Light Palette");
        paletteTabWidget->addTab(m_darkPaletteWidget, "Dark Palette");
        
        // Validation results
        auto* validationGroup = new QGroupBox("Theme Validation", this);
        auto* validationLayout = new QVBoxLayout(validationGroup);
        
        m_validationText = new QTextEdit(this);
        m_validationText->setMaximumHeight(150);
        m_validationText->setReadOnly(true);
        
        validationLayout->addWidget(m_validationText);
        
        // Component preview
        auto* componentGroup = new QGroupBox("Component Preview", this);
        auto* componentLayout = new QVBoxLayout(componentGroup);
        
        createComponentPreview(componentLayout);
        
        previewLayout->addWidget(paletteTabWidget);
        previewLayout->addWidget(validationGroup);
        previewLayout->addWidget(componentGroup);
        
        mainLayout->addWidget(m_controlPanel);
        mainLayout->addWidget(previewWidget, 1);
        
        // Status bar
        statusBar()->showMessage("Ready - Advanced theming system loaded");
    }
    
    void createComponentPreview(QVBoxLayout* layout) {
        // Create sample components to show theme application
        auto* buttonLayout = new QHBoxLayout();
        
        auto* primaryButton = new QPushButton("Primary Button", this);
        auto* secondaryButton = new QPushButton("Secondary Button", this);
        auto* dangerButton = new QPushButton("Danger Button", this);
        
        buttonLayout->addWidget(primaryButton);
        buttonLayout->addWidget(secondaryButton);
        buttonLayout->addWidget(dangerButton);
        buttonLayout->addStretch();
        
        auto* progressBar = new QProgressBar(this);
        progressBar->setValue(65);
        
        auto* sampleText = new QLabel("Sample text with current theme colors", this);
        
        layout->addLayout(buttonLayout);
        layout->addWidget(progressBar);
        layout->addWidget(sampleText);
        layout->addStretch();
    }
    
    void connectSignals() {
        connect(m_controlPanel, &ThemeControlPanel::themeChanged, this, &AdvancedThemingMainWindow::onThemeChanged);
        connect(m_controlPanel, &ThemeControlPanel::paletteGenerated, this, &AdvancedThemingMainWindow::onPaletteGenerated);
        
        // Connect to theme manager signals
        auto& themeManager = Styling::FluentAdvancedThemeManager::instance();
        connect(&themeManager, &Styling::FluentAdvancedThemeManager::colorPaletteGenerated,
                this, &AdvancedThemingMainWindow::onPaletteGenerated);
    }
    
    void updateThemeDisplay() {
        // Update the display based on current theme
        auto& theme = Styling::FluentTheme::instance();
        
        QString modeText;
        switch (theme.mode()) {
            case Styling::FluentThemeMode::Light:
                modeText = "Light Mode";
                break;
            case Styling::FluentThemeMode::Dark:
                modeText = "Dark Mode";
                break;
            case Styling::FluentThemeMode::HighContrast:
                modeText = "High Contrast Mode";
                break;
            default:
                modeText = "System Mode";
                break;
        }
        
        statusBar()->showMessage(QString("Current theme: %1").arg(modeText));
    }
    
    void updateValidationResults(const Styling::FluentColorPalette& palette) {
        auto& themeManager = Styling::FluentAdvancedThemeManager::instance();
        
        // Validate the palette
        auto validationResult = themeManager.validateTheme(palette);
        
        QString resultText;
        resultText += QString("Validation Status: %1\n").arg(validationResult.isValid ? "PASSED" : "FAILED");
        resultText += QString("Accessibility Score: %1/100\n").arg(validationResult.accessibilityScore, 0, 'f', 1);
        resultText += QString("Performance Score: %1/100\n\n").arg(validationResult.performanceScore, 0, 'f', 1);
        
        if (!validationResult.errors.isEmpty()) {
            resultText += "Errors:\n";
            for (const QString& error : validationResult.errors) {
                resultText += QString("• %1\n").arg(error);
            }
            resultText += "\n";
        }
        
        if (!validationResult.warnings.isEmpty()) {
            resultText += "Warnings:\n";
            for (const QString& warning : validationResult.warnings) {
                resultText += QString("• %1\n").arg(warning);
            }
            resultText += "\n";
        }
        
        if (!validationResult.suggestions.isEmpty()) {
            resultText += "Suggestions:\n";
            for (const QString& suggestion : validationResult.suggestions) {
                resultText += QString("• %1\n").arg(suggestion);
            }
        }
        
        m_validationText->setText(resultText);
    }

private:
    ThemeControlPanel* m_controlPanel;
    ColorPaletteWidget* m_lightPaletteWidget;
    ColorPaletteWidget* m_darkPaletteWidget;
    QTextEdit* m_validationText;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize theme system
    auto& theme = Styling::FluentTheme::instance();
    auto& themeManager = Styling::FluentAdvancedThemeManager::instance();
    
    // Set up default theme
    theme.setMode(Styling::FluentThemeMode::System);
    theme.setAccentColor(Styling::FluentAccentColor::Blue);
    
    AdvancedThemingMainWindow window;
    window.show();
    
    return app.exec();
}

#include "AdvancedThemingExample.moc"
