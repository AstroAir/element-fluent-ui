// tests/Compliance/FluentDesignSystemComplianceTest.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "FluentQt/Styling/FluentAdvancedTheme.h"
#include "FluentQt/Accessibility/FluentAccessibilityCompliance.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

using namespace FluentQt::Styling;
using namespace FluentQt::Accessibility;
using namespace FluentQt::Components;

class FluentDesignSystemComplianceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Design token compliance tests
    void testColorTokenCompliance();
    void testTypographyTokenCompliance();
    void testSpacingTokenCompliance();
    void testShadowTokenCompliance();
    void testAnimationTokenCompliance();
    void testBorderTokenCompliance();

    // Accessibility compliance tests
    void testColorContrastCompliance();
    void testKeyboardNavigationCompliance();
    void testScreenReaderCompliance();
    void testTouchTargetCompliance();
    void testFocusManagementCompliance();
    void testAriaAttributeCompliance();

    // Component compliance tests
    void testButtonDesignCompliance();
    void testCardDesignCompliance();
    void testTypographyConsistency();
    void testSpacingConsistency();

    // Theme compliance tests
    void testLightThemeCompliance();
    void testDarkThemeCompliance();
    void testHighContrastCompliance();
    void testCustomThemeCompliance();

private:
    QWidget* m_testWidget{nullptr};
    FluentAdvancedThemeManager* m_themeManager{nullptr};
    FluentAccessibilityComplianceManager* m_accessibilityManager{nullptr};
    
    // Helper methods
    void validateDesignToken(const QString& tokenName, FluentTokenType expectedType);
    void validateColorContrast(const QColor& foreground, const QColor& background, FluentWCAGLevel level);
    void validateComponentAccessibility(QWidget* component);
    bool isFluentUICompliantColor(const QColor& color);
    bool isFluentUICompliantFont(const QFont& font);
};

void FluentDesignSystemComplianceTest::initTestCase() {
    // Initialize design tokens
    extern void initializeFluentDesignTokens();
    initializeFluentDesignTokens();
    
    // Initialize accessibility system
    initializeFluentAccessibility();
    
    m_themeManager = &FluentAdvancedThemeManager::instance();
    m_accessibilityManager = &FluentAccessibilityComplianceManager::instance();
    
    // Set strict compliance configuration
    FluentAccessibilityConfig config;
    config.targetLevel = FluentWCAGLevel::AA;
    config.enableAutomaticFixes = false; // We want to test manual compliance
    config.enableRealTimeValidation = true;
    m_accessibilityManager->setConfiguration(config);
}

void FluentDesignSystemComplianceTest::cleanupTestCase() {
    // Cleanup
}

void FluentDesignSystemComplianceTest::init() {
    m_testWidget = new QWidget;
    m_testWidget->resize(800, 600);
}

void FluentDesignSystemComplianceTest::cleanup() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentDesignSystemComplianceTest::testColorTokenCompliance() {
    // Test neutral color palette compliance
    QStringList neutralTokens = {
        "neutral.0", "neutral.10", "neutral.20", "neutral.30", "neutral.40", "neutral.50"
    };
    
    for (const QString& token : neutralTokens) {
        QVariant value = m_themeManager->getToken(token);
        QVERIFY2(value.isValid(), qPrintable(QString("Token %1 not found").arg(token)));
        QVERIFY2(value.canConvert<QColor>(), qPrintable(QString("Token %1 is not a color").arg(token)));
        
        QColor color = value.value<QColor>();
        QVERIFY2(color.isValid(), qPrintable(QString("Token %1 has invalid color").arg(token)));
    }
    
    // Test brand color palette compliance
    QStringList brandTokens = {
        "brand.10", "brand.60", "brand.100", "brand.140"
    };
    
    for (const QString& token : brandTokens) {
        QVariant value = m_themeManager->getToken(token);
        QVERIFY2(value.isValid(), qPrintable(QString("Brand token %1 not found").arg(token)));
        
        QColor color = value.value<QColor>();
        QVERIFY2(isFluentUICompliantColor(color), 
                qPrintable(QString("Brand color %1 is not FluentUI compliant").arg(token)));
    }
    
    // Test semantic color tokens
    QStringList semanticTokens = {
        "color.background.primary", "color.text.primary", "color.border.primary"
    };
    
    for (const QString& token : semanticTokens) {
        QVariant value = m_themeManager->resolveToken(token);
        QVERIFY2(value.isValid(), qPrintable(QString("Semantic token %1 not resolved").arg(token)));
    }
    
    qDebug() << "Color token compliance: PASSED";
}

void FluentDesignSystemComplianceTest::testTypographyTokenCompliance() {
    // Test Microsoft FluentUI typography scale
    QStringList typographyTokens = {
        "typography.caption1", "typography.body1", "typography.subtitle1", 
        "typography.title1", "typography.title2", "typography.title3"
    };
    
    for (const QString& token : typographyTokens) {
        QVariant value = m_themeManager->getToken(token);
        QVERIFY2(value.isValid(), qPrintable(QString("Typography token %1 not found").arg(token)));
        QVERIFY2(value.canConvert<QFont>(), qPrintable(QString("Token %1 is not a font").arg(token)));
        
        QFont font = value.value<QFont>();
        QVERIFY2(isFluentUICompliantFont(font), 
                qPrintable(QString("Font %1 is not FluentUI compliant").arg(token)));
    }
    
    // Test line height tokens
    QStringList lineHeightTokens = {
        "typography.lineHeight.100", "typography.lineHeight.300", "typography.lineHeight.500"
    };
    
    for (const QString& token : lineHeightTokens) {
        QVariant value = m_themeManager->getToken(token);
        QVERIFY2(value.isValid(), qPrintable(QString("Line height token %1 not found").arg(token)));
        
        double lineHeight = value.toDouble();
        QVERIFY2(lineHeight >= 1.0 && lineHeight <= 2.0, 
                qPrintable(QString("Line height %1 is out of acceptable range").arg(lineHeight)));
    }
    
    qDebug() << "Typography token compliance: PASSED";
}

void FluentDesignSystemComplianceTest::testSpacingTokenCompliance() {
    // Test Microsoft FluentUI spacing scale (4px base unit)
    QStringList spacingTokens = {
        "spacing.xs", "spacing.s", "spacing.m", "spacing.l", "spacing.xl", "spacing.xxl"
    };
    
    QList<int> expectedValues = {4, 8, 12, 16, 20, 24};
    
    for (int i = 0; i < spacingTokens.size(); ++i) {
        const QString& token = spacingTokens[i];
        QVariant value = m_themeManager->getToken(token);
        QVERIFY2(value.isValid(), qPrintable(QString("Spacing token %1 not found").arg(token)));
        
        int spacing = value.toInt();
        QCOMPARE(spacing, expectedValues[i]);
        
        // Verify it's a multiple of 4 (FluentUI base unit)
        QVERIFY2(spacing % 4 == 0, qPrintable(QString("Spacing %1 is not a multiple of 4px").arg(spacing)));
    }
    
    // Test component-specific spacing
    QStringList componentSpacingTokens = {
        "spacing.button.padding.horizontal", "spacing.card.padding"
    };
    
    for (const QString& token : componentSpacingTokens) {
        QVariant resolved = m_themeManager->resolveToken(token);
        QVERIFY2(resolved.isValid(), qPrintable(QString("Component spacing %1 not resolved").arg(token)));
    }
    
    qDebug() << "Spacing token compliance: PASSED";
}

void FluentDesignSystemComplianceTest::testColorContrastCompliance() {
    // Test common color combinations for WCAG AA compliance
    QColor background = m_themeManager->resolveToken("color.background.primary").value<QColor>();
    QColor textPrimary = m_themeManager->resolveToken("color.text.primary").value<QColor>();
    QColor textSecondary = m_themeManager->resolveToken("color.text.secondary").value<QColor>();
    
    // Primary text on primary background
    validateColorContrast(textPrimary, background, FluentWCAGLevel::AA);
    
    // Secondary text on primary background
    validateColorContrast(textSecondary, background, FluentWCAGLevel::AA);
    
    // Brand colors
    QColor brandPrimary = m_themeManager->getToken("brand.100").value<QColor>();
    validateColorContrast(brandPrimary, background, FluentWCAGLevel::AA);
    
    qDebug() << "Color contrast compliance: PASSED";
}

void FluentDesignSystemComplianceTest::testKeyboardNavigationCompliance() {
    // Create test components
    auto* button1 = new FluentButton("Button 1", m_testWidget);
    auto* button2 = new FluentButton("Button 2", m_testWidget);
    auto* button3 = new FluentButton("Button 3", m_testWidget);
    
    auto* layout = new QVBoxLayout(m_testWidget);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Test tab order
    button1->setFocus();
    QVERIFY(button1->hasFocus());
    
    // Simulate Tab key
    QTest::keyPress(m_testWidget, Qt::Key_Tab);
    QApplication::processEvents();
    QVERIFY(button2->hasFocus());
    
    QTest::keyPress(m_testWidget, Qt::Key_Tab);
    QApplication::processEvents();
    QVERIFY(button3->hasFocus());
    
    // Test Shift+Tab (reverse)
    QTest::keyPress(m_testWidget, Qt::Key_Tab, Qt::ShiftModifier);
    QApplication::processEvents();
    QVERIFY(button2->hasFocus());
    
    qDebug() << "Keyboard navigation compliance: PASSED";
}

void FluentDesignSystemComplianceTest::testTouchTargetCompliance() {
    // Create test button
    auto* button = new FluentButton("Touch Test", m_testWidget);
    button->resize(30, 30); // Initially too small
    
    // Register with accessibility manager
    FluentComponentAccessibility metadata;
    metadata.role = "button";
    metadata.label = "Touch Test Button";
    m_accessibilityManager->registerComponent(button, metadata);
    
    // Enable automatic fixes
    FluentAccessibilityConfig config = m_accessibilityManager->getConfiguration();
    config.enableAutomaticFixes = true;
    config.minimumTouchTargetSize = 44.0;
    m_accessibilityManager->setConfiguration(config);
    
    // Apply fixes
    m_accessibilityManager->applyAutomaticFixes(button);
    
    // Verify touch target size
    QSize size = button->size();
    QVERIFY2(size.width() >= 44, "Touch target width is too small");
    QVERIFY2(size.height() >= 44, "Touch target height is too small");
    
    qDebug() << "Touch target compliance: PASSED";
}

void FluentDesignSystemComplianceTest::testButtonDesignCompliance() {
    auto* button = new FluentButton("Test Button", m_testWidget);
    button->setButtonStyle(FluentButtonStyle::Primary);
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Test button uses design tokens
    QPalette palette = button->palette();
    QColor buttonColor = palette.color(QPalette::Button);
    
    // Verify button follows FluentUI design specifications
    QVERIFY2(buttonColor.isValid(), "Button color is invalid");
    
    // Test button spacing
    QMargins margins = button->contentsMargins();
    int horizontalPadding = margins.left() + margins.right();
    int verticalPadding = margins.top() + margins.bottom();
    
    // Should use design token spacing
    int expectedHorizontal = m_themeManager->resolveToken("spacing.button.padding.horizontal").toInt() * 2;
    int expectedVertical = m_themeManager->resolveToken("spacing.button.padding.vertical").toInt() * 2;
    
    QVERIFY2(abs(horizontalPadding - expectedHorizontal) <= 4, "Button horizontal padding doesn't match design tokens");
    QVERIFY2(abs(verticalPadding - expectedVertical) <= 4, "Button vertical padding doesn't match design tokens");
    
    qDebug() << "Button design compliance: PASSED";
}

void FluentDesignSystemComplianceTest::testHighContrastCompliance() {
    // Enable high contrast mode
    m_accessibilityManager->enableHighContrastMode(true);
    
    // Create test components
    auto* button = new FluentButton("High Contrast Test", m_testWidget);
    auto* label = new QLabel("Test Label", m_testWidget);
    
    // Register components
    FluentComponentAccessibility buttonMetadata;
    buttonMetadata.role = "button";
    buttonMetadata.label = "High Contrast Test Button";
    m_accessibilityManager->registerComponent(button, buttonMetadata);
    
    FluentComponentAccessibility labelMetadata;
    labelMetadata.role = "text";
    labelMetadata.label = "Test Label";
    m_accessibilityManager->registerComponent(label, labelMetadata);
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Verify high contrast colors are applied
    QPalette buttonPalette = button->palette();
    QPalette labelPalette = label->palette();
    
    QColor buttonBg = buttonPalette.color(QPalette::Button);
    QColor buttonText = buttonPalette.color(QPalette::ButtonText);
    QColor labelBg = labelPalette.color(QPalette::Window);
    QColor labelText = labelPalette.color(QPalette::WindowText);
    
    // Verify extreme contrast ratios in high contrast mode
    double buttonContrast = m_accessibilityManager->calculateContrastRatio(buttonText, buttonBg);
    double labelContrast = m_accessibilityManager->calculateContrastRatio(labelText, labelBg);
    
    QVERIFY2(buttonContrast >= 7.0, "High contrast button doesn't meet AAA contrast ratio");
    QVERIFY2(labelContrast >= 7.0, "High contrast label doesn't meet AAA contrast ratio");
    
    qDebug() << "High contrast compliance: PASSED";
}

void FluentDesignSystemComplianceTest::validateColorContrast(const QColor& foreground, 
                                                           const QColor& background, 
                                                           FluentWCAGLevel level) {
    double ratio = m_accessibilityManager->calculateContrastRatio(foreground, background);
    
    double requiredRatio = 4.5; // WCAG AA
    if (level == FluentWCAGLevel::AAA) {
        requiredRatio = 7.0;
    }
    
    QVERIFY2(ratio >= requiredRatio, 
            qPrintable(QString("Contrast ratio %1 doesn't meet WCAG %2 requirement of %3")
                      .arg(ratio).arg(level == FluentWCAGLevel::AA ? "AA" : "AAA").arg(requiredRatio)));
}

bool FluentDesignSystemComplianceTest::isFluentUICompliantColor(const QColor& color) {
    // Basic validation - FluentUI colors should be valid and not pure red/green/blue
    if (!color.isValid()) return false;
    
    // FluentUI typically uses more sophisticated colors than pure primaries
    if (color == QColor(255, 0, 0) || color == QColor(0, 255, 0) || color == QColor(0, 0, 255)) {
        return false;
    }
    
    return true;
}

bool FluentDesignSystemComplianceTest::isFluentUICompliantFont(const QFont& font) {
    // FluentUI primarily uses Segoe UI family
    QString family = font.family().toLower();
    
    // Accept Segoe UI and common fallbacks
    QStringList acceptableFamilies = {"segoe ui", "system", "arial", "helvetica", "sans-serif"};
    
    for (const QString& acceptable : acceptableFamilies) {
        if (family.contains(acceptable)) {
            return true;
        }
    }
    
    return false;
}

QTEST_MAIN(FluentDesignSystemComplianceTest)
#include "FluentDesignSystemComplianceTest.moc"
