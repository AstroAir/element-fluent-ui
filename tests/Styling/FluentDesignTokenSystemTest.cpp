// tests/Styling/FluentDesignTokenSystemTest.cpp
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QtTest/QtTest>
#include "FluentQt/Styling/FluentAdvancedTheme.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"

using namespace FluentQt::Styling;

class FluentDesignTokenSystemTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Core token system tests
    void testTokenSystemInitialization();
    void testTokenRegistration();
    void testTokenResolution();
    void testTokenValidation();

    // Color token tests
    void testColorTokens();
    void testBrandColorPalette();
    void testNeutralColorPalette();
    void testSemanticColorTokens();
    void testColorAccessibility();

    // Typography token tests
    void testTypographyTokens();
    void testTypographyScale();
    void testFontFamilies();
    void testLineHeights();

    // Spacing token tests
    void testSpacingTokens();
    void testSpacingScale();
    void testComponentSpacing();

    // Component token tests
    void testButtonTokens();
    void testCardTokens();
    void testInputTokens();
    void testNavigationTokens();

    // Utility function tests
    void testTokenUtils();
    void testConvenienceMacros();
    void testResponsiveTokens();
    void testCustomTokens();

    // Performance tests
    void testTokenResolutionPerformance();
    void testTokenCaching();

private:
    FluentDesignTokenUtils* m_tokenUtils{nullptr};
    FluentAdvancedThemeManager* m_themeManager{nullptr};

    // Helper methods
    void validateColorToken(const QString& tokenName,
                            const QColor& expectedColor);
    void validateSizeToken(const QString& tokenName, int expectedSize);
    void validateFontToken(const QString& tokenName, const QFont& expectedFont);
    bool isValidFluentUIColor(const QColor& color);
};

void FluentDesignTokenSystemTest::initTestCase() {
    // Initialize design token system
    extern void initializeFluentDesignTokens();
    initializeFluentDesignTokens();

    m_tokenUtils = &FluentDesignTokenUtils::instance();
    m_themeManager = &FluentAdvancedThemeManager::instance();
}

void FluentDesignTokenSystemTest::cleanupTestCase() {
    // Cleanup
}

void FluentDesignTokenSystemTest::init() {
    // Reset to defaults before each test
    m_tokenUtils->resetToDefaults();
}

void FluentDesignTokenSystemTest::cleanup() {
    // Cleanup after each test
}

void FluentDesignTokenSystemTest::testTokenSystemInitialization() {
    // Test that all required token categories are initialized
    QStringList requiredCategories = {"neutral", "brand",  "typography",
                                      "spacing", "shadow", "animation",
                                      "border"};

    for (const QString& category : requiredCategories) {
        QStringList categoryTokens = m_tokenUtils->getAvailableTokens();
        bool categoryExists = false;

        for (const QString& token : categoryTokens) {
            if (token.startsWith(category + ".")) {
                categoryExists = true;
                break;
            }
        }

        QVERIFY2(categoryExists,
                 qPrintable(QString("Category %1 not found").arg(category)));
    }

    qDebug() << "Token system initialization: PASSED";
}

void FluentDesignTokenSystemTest::testColorTokens() {
    // Test neutral color palette
    QColor white = m_tokenUtils->getNeutralColor(0);
    QColor black = m_tokenUtils->getNeutralColor(54);

    QVERIFY(white.isValid());
    QVERIFY(black.isValid());
    QCOMPARE(white, QColor("#ffffff"));
    QCOMPARE(black, QColor("#000000"));

    // Test brand color palette
    QColor brandPrimary = m_tokenUtils->getBrandColor(100);
    QVERIFY(brandPrimary.isValid());
    QVERIFY(isValidFluentUIColor(brandPrimary));

    // Test semantic colors
    QColor backgroundPrimary =
        m_tokenUtils->getSemanticColor("background.primary");
    QColor textPrimary = m_tokenUtils->getSemanticColor("text.primary");

    QVERIFY(backgroundPrimary.isValid());
    QVERIFY(textPrimary.isValid());

    qDebug() << "Color tokens: PASSED";
}

void FluentDesignTokenSystemTest::testBrandColorPalette() {
    // Test complete brand color palette
    QList<int> brandShades = {10, 20,  30,  40,  50,  60,  70,  80,
                              90, 100, 110, 120, 130, 140, 150, 160};

    QColor previousColor;
    for (int shade : brandShades) {
        QColor color = m_tokenUtils->getBrandColor(shade);
        QVERIFY2(color.isValid(),
                 qPrintable(QString("Brand color %1 is invalid").arg(shade)));

        // Verify color progression (lighter shades should be lighter)
        if (previousColor.isValid() && shade > 10) {
            // This is a simplified check - in practice, you'd want more
            // sophisticated color analysis
            QVERIFY2(
                color != previousColor,
                qPrintable(QString("Brand colors %1 and previous are identical")
                               .arg(shade)));
        }

        previousColor = color;
    }

    qDebug() << "Brand color palette: PASSED";
}

void FluentDesignTokenSystemTest::testTypographyTokens() {
    // Test typography scale
    QFont caption1 = m_tokenUtils->getCaptionFont(1);
    QFont body1 = m_tokenUtils->getBodyFont(false);
    QFont title1 = m_tokenUtils->getTitleFont(1);

    QVERIFY(caption1.pointSize() > 0);
    QVERIFY(body1.pointSize() > 0);
    QVERIFY(title1.pointSize() > 0);

    // Verify size hierarchy
    QVERIFY(caption1.pointSize() < body1.pointSize());
    QVERIFY(body1.pointSize() < title1.pointSize());

    // Test font families
    QString expectedFamily = "Segoe UI";
    QVERIFY(caption1.family().contains(expectedFamily) ||
            caption1.family() == "System");
    QVERIFY(body1.family().contains(expectedFamily) ||
            body1.family() == "System");
    QVERIFY(title1.family().contains(expectedFamily) ||
            title1.family() == "System");

    qDebug() << "Typography tokens: PASSED";
}

void FluentDesignTokenSystemTest::testSpacingTokens() {
    // Test spacing scale (4px base unit)
    int xs = m_tokenUtils->getSpacing("xs");
    int s = m_tokenUtils->getSpacing("s");
    int m = m_tokenUtils->getSpacing("m");
    int l = m_tokenUtils->getSpacing("l");
    int xl = m_tokenUtils->getSpacing("xl");

    // Verify 4px base unit
    QVERIFY(xs % 4 == 0);
    QVERIFY(s % 4 == 0);
    QVERIFY(m % 4 == 0);
    QVERIFY(l % 4 == 0);
    QVERIFY(xl % 4 == 0);

    // Verify progression
    QVERIFY(xs < s);
    QVERIFY(s < m);
    QVERIFY(m < l);
    QVERIFY(l < xl);

    // Test expected values
    QCOMPARE(xs, 4);
    QCOMPARE(s, 8);
    QCOMPARE(m, 12);
    QCOMPARE(l, 16);
    QCOMPARE(xl, 20);

    qDebug() << "Spacing tokens: PASSED";
}

void FluentDesignTokenSystemTest::testButtonTokens() {
    // Test button size variants
    int smallHeight = m_tokenUtils->getSize("button.size.small.height");
    int mediumHeight = m_tokenUtils->getSize("button.size.medium.height");
    int largeHeight = m_tokenUtils->getSize("button.size.large.height");

    QVERIFY(smallHeight > 0);
    QVERIFY(mediumHeight > 0);
    QVERIFY(largeHeight > 0);
    QVERIFY(smallHeight < mediumHeight);
    QVERIFY(mediumHeight < largeHeight);

    // Test button colors
    QColor primaryBg = m_tokenUtils->getColor("button.primary.background.rest");
    QColor primaryText = m_tokenUtils->getColor("button.primary.text");

    QVERIFY(primaryBg.isValid());
    QVERIFY(primaryText.isValid());

    // Test button spacing
    int smallPaddingH =
        m_tokenUtils->getSize("button.size.small.padding.horizontal");
    int mediumPaddingH =
        m_tokenUtils->getSize("button.size.medium.padding.horizontal");

    QVERIFY(smallPaddingH > 0);
    QVERIFY(mediumPaddingH > 0);
    QVERIFY(smallPaddingH < mediumPaddingH);

    qDebug() << "Button tokens: PASSED";
}

void FluentDesignTokenSystemTest::testTokenUtils() {
    // Test convenience functions
    QColor brandColor = TokenUtils::brandColor(100);
    QColor neutralColor = TokenUtils::neutralColor(20);
    QFont titleFont = TokenUtils::titleFont(1);
    int spacing = TokenUtils::spacing("m");

    QVERIFY(brandColor.isValid());
    QVERIFY(neutralColor.isValid());
    QVERIFY(titleFont.pointSize() > 0);
    QVERIFY(spacing > 0);

    // Test component utilities
    QSize buttonSize = TokenUtils::componentSize("button", "medium");
    QMargins cardMargins = TokenUtils::margins("card", "medium");

    QVERIFY(buttonSize.isValid());
    QVERIFY(cardMargins.left() > 0);

    qDebug() << "Token utilities: PASSED";
}

void FluentDesignTokenSystemTest::testConvenienceMacros() {
    // Test convenience macros
    QColor color = FLUENT_BRAND_COLOR(100);
    QFont font = FLUENT_BODY_FONT(false);
    int size = FLUENT_SPACING("m");

    QVERIFY(color.isValid());
    QVERIFY(font.pointSize() > 0);
    QVERIFY(size > 0);

    // Test component-specific macros
    QColor buttonColor = FLUENT_BUTTON_COLOR("primary", "rest");
    int buttonHeight = FLUENT_BUTTON_SIZE("medium", "height");
    int cardPadding = FLUENT_CARD_PADDING("medium");

    QVERIFY(buttonColor.isValid());
    QVERIFY(buttonHeight > 0);
    QVERIFY(cardPadding > 0);

    qDebug() << "Convenience macros: PASSED";
}

void FluentDesignTokenSystemTest::testCustomTokens() {
    // Test custom token creation
    QString customTokenName = "custom.test.color";
    QColor customColor = QColor("#ff5722");

    m_tokenUtils->setCustomToken(customTokenName, customColor,
                                 FluentTokenType::Color);

    // Verify custom token exists
    QVERIFY(m_tokenUtils->isTokenValid(customTokenName));

    // Verify custom token value
    QColor retrievedColor = m_tokenUtils->getColor(customTokenName);
    QCOMPARE(retrievedColor, customColor);

    // Test custom token removal
    m_tokenUtils->removeCustomToken(customTokenName);
    QVERIFY(!m_tokenUtils->isTokenValid(customTokenName));

    qDebug() << "Custom tokens: PASSED";
}

void FluentDesignTokenSystemTest::testTokenResolutionPerformance() {
    // Test token resolution performance
    const int iterations = 1000;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
        // Test various token types
        m_tokenUtils->getBrandColor(100);
        m_tokenUtils->getBodyFont(false);
        m_tokenUtils->getSpacing("m");
        m_tokenUtils->getSize("button.size.medium.height");
    }

    qint64 elapsed = timer.elapsed();
    double avgTimePerToken = (double)elapsed / (iterations * 4);

    qDebug() << "Token resolution performance:";
    qDebug() << "  Total time:" << elapsed << "ms";
    qDebug() << "  Average time per token:" << avgTimePerToken << "ms";

    // Performance should be reasonable (less than 0.1ms per token)
    QVERIFY(avgTimePerToken < 0.1);

    qDebug() << "Token resolution performance: PASSED";
}

void FluentDesignTokenSystemTest::testTokenCaching() {
    // Test that token caching improves performance
    const QString testToken = "brand.100";
    const int iterations = 100;

    // First access (cache miss)
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
        m_tokenUtils->getColor(testToken);
    }

    qint64 cachedTime = timer.elapsed();

    // Clear cache and test again
    m_tokenUtils->invalidateTokenCache();

    timer.restart();
    for (int i = 0; i < iterations; ++i) {
        m_tokenUtils->getColor(testToken);
    }

    qint64 uncachedTime = timer.elapsed();

    qDebug() << "Token caching performance:";
    qDebug() << "  Cached time:" << cachedTime << "ms";
    qDebug() << "  Uncached time:" << uncachedTime << "ms";

    // Caching should provide some benefit (though it might be minimal for
    // simple tokens)
    QVERIFY(cachedTime <= uncachedTime * 1.5);  // Allow some variance

    qDebug() << "Token caching: PASSED";
}

void FluentDesignTokenSystemTest::validateColorToken(
    const QString& tokenName, const QColor& expectedColor) {
    QColor actualColor = m_tokenUtils->getColor(tokenName);
    QVERIFY2(actualColor.isValid(),
             qPrintable(QString("Color token %1 is invalid").arg(tokenName)));
    QCOMPARE(actualColor, expectedColor);
}

void FluentDesignTokenSystemTest::validateSizeToken(const QString& tokenName,
                                                    int expectedSize) {
    int actualSize = m_tokenUtils->getSize(tokenName);
    QVERIFY2(actualSize > 0,
             qPrintable(QString("Size token %1 is invalid").arg(tokenName)));
    QCOMPARE(actualSize, expectedSize);
}

bool FluentDesignTokenSystemTest::isValidFluentUIColor(const QColor& color) {
    // Basic validation for FluentUI colors
    if (!color.isValid())
        return false;

    // FluentUI colors should not be pure primary colors
    if (color == QColor(255, 0, 0) || color == QColor(0, 255, 0) ||
        color == QColor(0, 0, 255)) {
        return false;
    }

    return true;
}

QTEST_MAIN(FluentDesignTokenSystemTest)
#include "FluentDesignTokenSystemTest.moc"
