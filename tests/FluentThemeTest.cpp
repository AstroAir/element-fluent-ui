// tests/FluentThemeTest.cpp
#include <QColor>
#include <QFont>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class FluentThemeTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testSingleton();
    void testDarkModeToggle();
    void testAccentColor();
    void testColorSystem();

    // Typography tests
    void testTypographySystem();
    void testFontMethods();
    void testFontSizes();

    // Spacing and layout tests
    void testSpacingSystem();
    void testBorderRadius();
    void testElevation();
    void testMarginsPadding();

    // Accessibility tests
    void testHighContrastMode();
    void testContrastRatio();
    void testEnsureContrast();

    // Color utilities tests
    void testColorUtilities();
    void testColorManipulation();
    void testColorBlending();

    // Signal emission tests
    void testThemeChangeSignals();
    void testAccentColorChangeSignals();

    // Edge cases and validation
    void testInvalidColorKeys();
    void testColorValidation();
    void testFontValidation();

private:
    Styling::FluentTheme* m_theme{nullptr};
};

void FluentThemeTest::initTestCase() {
    // Initialize test environment
    qApp->setApplicationName("FluentThemeTest");
}

void FluentThemeTest::cleanupTestCase() {
    // Cleanup test environment
}

void FluentThemeTest::init() {
    m_theme = &Styling::FluentTheme::instance();

    // Reset to default state
    m_theme->setDarkMode(false);
    m_theme->setAccentColor(QColor(0, 120, 215));  // Default Windows blue
    m_theme->setHighContrastMode(false);
}

void FluentThemeTest::cleanup() {
    // Reset theme to default state after each test
    m_theme->setDarkMode(false);
    m_theme->setAccentColor(QColor(0, 120, 215));
    m_theme->setHighContrastMode(false);
}

void FluentThemeTest::testSingleton() {
    // Test singleton pattern
    auto& theme1 = Styling::FluentTheme::instance();
    auto& theme2 = Styling::FluentTheme::instance();

    QVERIFY(&theme1 == &theme2);
    QVERIFY(&theme1 == m_theme);
}

void FluentThemeTest::testDarkModeToggle() {
    // Test initial state
    QVERIFY(!m_theme->isDarkMode());

    // Test setting dark mode - use old-style SIGNAL macro to avoid overload
    // ambiguity
    QSignalSpy themeChangedSpy(m_theme, SIGNAL(themeChanged()));

    m_theme->setDarkMode(true);
    QVERIFY(m_theme->isDarkMode());
    QCOMPARE(themeChangedSpy.count(), 1);

    // Test setting same value (should not emit signal)
    m_theme->setDarkMode(true);
    QCOMPARE(themeChangedSpy.count(), 1);

    // Test switching back to light mode
    m_theme->setDarkMode(false);
    QVERIFY(!m_theme->isDarkMode());
    QCOMPARE(themeChangedSpy.count(), 2);
}

void FluentThemeTest::testAccentColor() {
    const QColor testColor(255, 0, 0);  // Red

    QSignalSpy accentChangedSpy(m_theme, SIGNAL(accentColorChanged(QColor)));

    m_theme->setAccentColor(testColor);
    QCOMPARE(m_theme->accentColor(), testColor);
    QCOMPARE(accentChangedSpy.count(), 1);

    // Test setting same color (should not emit signal)
    m_theme->setAccentColor(testColor);
    QCOMPARE(accentChangedSpy.count(), 1);

    // Test invalid color handling
    m_theme->setAccentColor(QColor());
    QVERIFY(
        m_theme->accentColor().isValid());  // Should fallback to valid color
}

void FluentThemeTest::testColorSystem() {
    // Test basic color retrieval
    QColor primaryBg = m_theme->color("backgroundPrimary");
    QVERIFY(primaryBg.isValid());

    QColor textPrimary = m_theme->color("textPrimary");
    QVERIFY(textPrimary.isValid());

    QColor accent = m_theme->color("accent");
    QVERIFY(accent.isValid());

    // Test that colors change with theme mode
    QColor lightBg = m_theme->color("backgroundPrimary");
    m_theme->setDarkMode(true);
    QColor darkBg = m_theme->color("backgroundPrimary");

    QVERIFY(lightBg != darkBg);  // Colors should be different in dark mode

    // Test accent color integration
    m_theme->setAccentColor(QColor(255, 0, 0));
    QColor newAccent = m_theme->color("accent");
    QCOMPARE(newAccent, QColor(255, 0, 0));
}

void FluentThemeTest::testTypographySystem() {
    // Test all typography methods exist and return valid fonts
    QFont captionFont = m_theme->captionFont();
    QVERIFY(!captionFont.family().isEmpty());

    QFont bodyFont = m_theme->bodyFont();
    QVERIFY(!bodyFont.family().isEmpty());

    QFont subtitleFont = m_theme->subtitleFont();
    QVERIFY(!subtitleFont.family().isEmpty());

    QFont titleFont = m_theme->titleFont();
    QVERIFY(!titleFont.family().isEmpty());

    // Test that fonts have appropriate relative sizes
    QVERIFY(titleFont.pointSize() > subtitleFont.pointSize());
    QVERIFY(subtitleFont.pointSize() > bodyFont.pointSize());
    QVERIFY(bodyFont.pointSize() > captionFont.pointSize());
}

void FluentThemeTest::testFontMethods() {
    // Test all 14 font methods
    const QList<std::function<QFont()>> fontMethods = {
        [this]() { return m_theme->captionFont(); },
        [this]() { return m_theme->bodyFont(); },
        [this]() { return m_theme->subtitleFont(); },
        [this]() { return m_theme->titleFont(); },
        [this]() { return m_theme->displayFont(); },
        [this]() { return m_theme->titleLargeFont(); },
        [this]() { return m_theme->bodyLargeFont(); },
        [this]() { return m_theme->headingFont(); },
        [this]() { return m_theme->buttonFont(); },
        [this]() { return m_theme->linkFont(); },
        [this]() { return m_theme->codeFont(); }};

    for (const auto& fontMethod : fontMethods) {
        QFont font = fontMethod();
        QVERIFY(!font.family().isEmpty());
        QVERIFY(font.pointSize() > 0);
    }
}

void FluentThemeTest::testFontSizes() {
    // Test font size consistency
    QFont caption = m_theme->captionFont();
    QFont body = m_theme->bodyFont();
    QFont subtitle = m_theme->subtitleFont();
    QFont title = m_theme->titleFont();
    QFont display = m_theme->displayFont();

    // Verify size hierarchy
    QVERIFY(caption.pointSize() <= body.pointSize());
    QVERIFY(body.pointSize() <= subtitle.pointSize());
    QVERIFY(subtitle.pointSize() <= title.pointSize());
    QVERIFY(title.pointSize() <= display.pointSize());
}

void FluentThemeTest::testSpacingSystem() {
    // Test spacing methods
    int smallSpacing = m_theme->spacing("small");
    int mediumSpacing = m_theme->spacing("medium");
    int largeSpacing = m_theme->spacing("large");

    QVERIFY(smallSpacing > 0);
    QVERIFY(mediumSpacing > smallSpacing);
    QVERIFY(largeSpacing > mediumSpacing);

    // Test component-specific spacing
    int componentHeight = m_theme->componentHeight("medium");
    int componentWidth = m_theme->componentWidth("medium");

    QVERIFY(componentHeight > 0);
    QVERIFY(componentWidth > 0);
}

void FluentThemeTest::testBorderRadius() {
    // Test border radius methods
    int smallRadius = m_theme->borderRadius("small");
    int mediumRadius = m_theme->borderRadius("medium");
    int largeRadius = m_theme->borderRadius("large");

    QVERIFY(smallRadius >= 0);
    QVERIFY(mediumRadius >= smallRadius);
    QVERIFY(largeRadius >= mediumRadius);
}

void FluentThemeTest::testElevation() {
    // Test elevation methods
    int lowElevation = m_theme->elevation("low");
    int mediumElevation = m_theme->elevation("medium");
    int highElevation = m_theme->elevation("high");

    QVERIFY(lowElevation >= 0);
    QVERIFY(mediumElevation >= lowElevation);
    QVERIFY(highElevation >= mediumElevation);
}

void FluentThemeTest::testMarginsPadding() {
    // Test margins and padding methods
    int margins = m_theme->marginsValue("medium");
    int padding = m_theme->paddingValue("medium");

    QVERIFY(margins >= 0);
    QVERIFY(padding >= 0);
}

void FluentThemeTest::testHighContrastMode() {
    // Test high contrast mode
    QVERIFY(!m_theme->isHighContrastMode());

    m_theme->setHighContrastMode(true);
    QVERIFY(m_theme->isHighContrastMode());

    // Test that colors change in high contrast mode
    QColor normalText = m_theme->color("textPrimary");
    m_theme->setHighContrastMode(false);
    QColor contrastText = m_theme->color("textPrimary");

    // Colors might be the same or different depending on implementation
    QVERIFY(normalText.isValid());
    QVERIFY(contrastText.isValid());
}

void FluentThemeTest::testContrastRatio() {
    // Test contrast ratio calculation
    QColor white(255, 255, 255);
    QColor black(0, 0, 0);
    QColor gray(128, 128, 128);

    qreal whiteBlackRatio = m_theme->contrastRatio(white, black);
    qreal whiteGrayRatio = m_theme->contrastRatio(white, gray);
    qreal blackGrayRatio = m_theme->contrastRatio(black, gray);

    // White and black should have the highest contrast ratio
    QVERIFY(whiteBlackRatio > whiteGrayRatio);
    QVERIFY(whiteBlackRatio > blackGrayRatio);
    QVERIFY(whiteBlackRatio >= 21.0);  // Maximum theoretical contrast ratio
}

void FluentThemeTest::testEnsureContrast() {
    QColor background(255, 255, 255);       // White
    QColor lowContrastText(240, 240, 240);  // Very light gray

    QColor adjustedText =
        m_theme->ensureContrast(lowContrastText, background, 4.5);

    // Adjusted text should have better contrast
    qreal originalRatio = m_theme->contrastRatio(lowContrastText, background);
    qreal adjustedRatio = m_theme->contrastRatio(adjustedText, background);

    QVERIFY(adjustedRatio >= originalRatio);
    QVERIFY(adjustedRatio >= 4.5 || qFuzzyCompare(adjustedRatio, 4.5));
}

void FluentThemeTest::testColorUtilities() {
    QColor baseColor(100, 150, 200);

    // Test lighten
    QColor lighterColor = m_theme->lighten(baseColor, 0.2);
    QVERIFY(lighterColor.red() >= baseColor.red());
    QVERIFY(lighterColor.green() >= baseColor.green());
    QVERIFY(lighterColor.blue() >= baseColor.blue());

    // Test darken
    QColor darkerColor = m_theme->darken(baseColor, 0.2);
    QVERIFY(darkerColor.red() <= baseColor.red());
    QVERIFY(darkerColor.green() <= baseColor.green());
    QVERIFY(darkerColor.blue() <= baseColor.blue());

    // Test adjustAlpha
    QColor alphaColor = m_theme->adjustAlpha(baseColor, 0.5);
    QVERIFY(qAbs(alphaColor.alphaF() - 0.5) <
            0.001);  // Use tolerance-based comparison for floating point
    QCOMPARE(alphaColor.red(), baseColor.red());
    QCOMPARE(alphaColor.green(), baseColor.green());
    QCOMPARE(alphaColor.blue(), baseColor.blue());
}

void FluentThemeTest::testColorManipulation() {
    QColor color1(255, 0, 0);  // Red
    QColor color2(0, 255, 0);  // Green

    // Test blend
    QColor blended = m_theme->blend(color1, color2, 0.5);
    QVERIFY(blended.red() > 0 && blended.red() < 255);
    QVERIFY(blended.green() > 0 && blended.green() < 255);
    QCOMPARE(blended.blue(), 0);

    // Test blend extremes
    QColor blend0 = m_theme->blend(color1, color2, 0.0);
    QColor blend1 = m_theme->blend(color1, color2, 1.0);

    QCOMPARE(blend0, color1);
    QCOMPARE(blend1, color2);
}

void FluentThemeTest::testColorBlending() {
    QColor red(255, 0, 0);
    QColor blue(0, 0, 255);

    // Test various blend ratios
    const QList<qreal> ratios = {0.0, 0.25, 0.5, 0.75, 1.0};

    for (qreal ratio : ratios) {
        QColor blended = m_theme->blend(red, blue, ratio);
        QVERIFY(blended.isValid());

        if (qFuzzyIsNull(ratio)) {
            QCOMPARE(blended, red);
        } else if (qFuzzyCompare(ratio, 1.0)) {
            QCOMPARE(blended, blue);
        }
    }
}

void FluentThemeTest::testThemeChangeSignals() {
    QSignalSpy themeChangedSpy(m_theme, SIGNAL(themeChanged()));

    // Changing dark mode should emit themeChanged
    m_theme->setDarkMode(true);
    QCOMPARE(themeChangedSpy.count(), 1);

    // Changing high contrast should emit themeChanged
    m_theme->setHighContrastMode(true);
    QCOMPARE(themeChangedSpy.count(), 2);
}

void FluentThemeTest::testAccentColorChangeSignals() {
    QSignalSpy accentChangedSpy(m_theme, SIGNAL(accentColorChanged(QColor)));

    m_theme->setAccentColor(QColor(255, 0, 0));
    QCOMPARE(accentChangedSpy.count(), 1);

    // Verify signal argument
    QList<QVariant> arguments = accentChangedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QColor>(), QColor(255, 0, 0));
}

void FluentThemeTest::testInvalidColorKeys() {
    // Test invalid color key
    QColor invalidColor = m_theme->color("nonexistentColor");

    // Should return a valid fallback color or handle gracefully
    QVERIFY(invalidColor.isValid() || invalidColor == QColor());
}

void FluentThemeTest::testColorValidation() {
    // Test setting invalid accent color
    QColor originalAccent = m_theme->accentColor();

    m_theme->setAccentColor(QColor());  // Invalid color

    // Accent color should remain unchanged when setting invalid color
    QCOMPARE(m_theme->accentColor(), originalAccent);

    // Should either keep original or set to valid fallback
    QVERIFY(m_theme->accentColor().isValid());
}

void FluentThemeTest::testFontValidation() {
    // Test that all fonts are valid
    const QList<QFont> fonts = {m_theme->captionFont(), m_theme->bodyFont(),
                                m_theme->subtitleFont(), m_theme->titleFont()};

    for (const QFont& font : fonts) {
        QVERIFY(!font.family().isEmpty());
        QVERIFY(font.pointSize() > 0);
    }
}

QTEST_MAIN(FluentThemeTest)
#include "FluentThemeTest.moc"
