// tests/Components/FluentColorPickerTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentColorPicker.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentColorPickerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testColorConstructor();

    // Color tests
    void testCurrentColor();
    void testSetColor();
    void testColorFromHsv();
    void testColorFromRgb();

    // Picker mode tests
    void testPickerMode();
    void testColorFormat();

    // Palette tests
    void testShowPalette();
    void testCustomPalette();
    void testRecentColors();

    // Alpha tests
    void testAlphaEnabled();
    void testAlphaValue();

    // Interaction tests
    void testColorSelection();
    void testMouseInteraction();
    void testKeyboardNavigation();

    // Signal tests
    void testSignalEmission();

    // Validation tests
    void testColorValidation();

private:
    FluentColorPicker* m_colorPicker{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentColorPickerTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentColorPickerTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentColorPickerTest::init() {
    m_colorPicker = new FluentColorPicker(m_testWidget);
}

void FluentColorPickerTest::cleanup() {
    delete m_colorPicker;
    m_colorPicker = nullptr;
}

void FluentColorPickerTest::testDefaultConstructor() {
    QVERIFY(m_colorPicker != nullptr);
    QCOMPARE(m_colorPicker->currentColor(), QColor(Qt::white));
    QCOMPARE(m_colorPicker->pickerMode(), FluentColorPickerMode::Wheel);
    QVERIFY(m_colorPicker->showPalette());
    QVERIFY(m_colorPicker->alphaEnabled());
}

void FluentColorPickerTest::testColorConstructor() {
    QColor testColor(255, 128, 64);
    auto* colorPicker = new FluentColorPicker(testColor, m_testWidget);
    QCOMPARE(colorPicker->currentColor(), testColor);
    delete colorPicker;
}

void FluentColorPickerTest::testCurrentColor() {
    QColor initialColor = m_colorPicker->currentColor();
    QVERIFY(initialColor.isValid());
}

void FluentColorPickerTest::testSetColor() {
    QSignalSpy colorSpy(m_colorPicker, &FluentColorPicker::colorChanged);
    
    QColor testColor(255, 0, 0); // Red
    m_colorPicker->setColor(testColor);
    QCOMPARE(m_colorPicker->currentColor(), testColor);
    QCOMPARE(colorSpy.count(), 1);
    QCOMPARE(colorSpy.first().first().value<QColor>(), testColor);
    
    // Test setting same color (should not emit signal)
    m_colorPicker->setColor(testColor);
    QCOMPARE(colorSpy.count(), 1);
    
    // Test setting different color
    QColor blueColor(0, 0, 255);
    m_colorPicker->setColor(blueColor);
    QCOMPARE(m_colorPicker->currentColor(), blueColor);
    QCOMPARE(colorSpy.count(), 2);
}

void FluentColorPickerTest::testColorFromHsv() {
    QSignalSpy colorSpy(m_colorPicker, &FluentColorPicker::colorChanged);
    
    // Set color using HSV values
    m_colorPicker->setColorFromHsv(120, 255, 255); // Green
    QColor resultColor = m_colorPicker->currentColor();
    
    QVERIFY(resultColor.isValid());
    QCOMPARE(resultColor.hue(), 120);
    QCOMPARE(resultColor.saturation(), 255);
    QCOMPARE(resultColor.value(), 255);
    QCOMPARE(colorSpy.count(), 1);
}

void FluentColorPickerTest::testColorFromRgb() {
    QSignalSpy colorSpy(m_colorPicker, &FluentColorPicker::colorChanged);
    
    // Set color using RGB values
    m_colorPicker->setColorFromRgb(128, 64, 192);
    QColor resultColor = m_colorPicker->currentColor();
    
    QVERIFY(resultColor.isValid());
    QCOMPARE(resultColor.red(), 128);
    QCOMPARE(resultColor.green(), 64);
    QCOMPARE(resultColor.blue(), 192);
    QCOMPARE(colorSpy.count(), 1);
}

void FluentColorPickerTest::testPickerMode() {
    QSignalSpy modeSpy(m_colorPicker, &FluentColorPicker::pickerModeChanged);
    
    m_colorPicker->setPickerMode(FluentColorPickerMode::Square);
    QCOMPARE(m_colorPicker->pickerMode(), FluentColorPickerMode::Square);
    QCOMPARE(modeSpy.count(), 1);
    
    m_colorPicker->setPickerMode(FluentColorPickerMode::Sliders);
    QCOMPARE(m_colorPicker->pickerMode(), FluentColorPickerMode::Sliders);
    QCOMPARE(modeSpy.count(), 2);
    
    m_colorPicker->setPickerMode(FluentColorPickerMode::Wheel);
    QCOMPARE(m_colorPicker->pickerMode(), FluentColorPickerMode::Wheel);
    QCOMPARE(modeSpy.count(), 3);
}

void FluentColorPickerTest::testColorFormat() {
    QSignalSpy formatSpy(m_colorPicker, &FluentColorPicker::colorFormatChanged);
    
    m_colorPicker->setColorFormat(FluentColorFormat::Hex);
    QCOMPARE(m_colorPicker->colorFormat(), FluentColorFormat::Hex);
    QCOMPARE(formatSpy.count(), 1);
    
    m_colorPicker->setColorFormat(FluentColorFormat::Rgb);
    QCOMPARE(m_colorPicker->colorFormat(), FluentColorFormat::Rgb);
    QCOMPARE(formatSpy.count(), 2);
    
    m_colorPicker->setColorFormat(FluentColorFormat::Hsv);
    QCOMPARE(m_colorPicker->colorFormat(), FluentColorFormat::Hsv);
    QCOMPARE(formatSpy.count(), 3);
}

void FluentColorPickerTest::testShowPalette() {
    QSignalSpy paletteSpy(m_colorPicker, &FluentColorPicker::showPaletteChanged);
    
    m_colorPicker->setShowPalette(false);
    QVERIFY(!m_colorPicker->showPalette());
    QCOMPARE(paletteSpy.count(), 1);
    
    m_colorPicker->setShowPalette(true);
    QVERIFY(m_colorPicker->showPalette());
    QCOMPARE(paletteSpy.count(), 2);
}

void FluentColorPickerTest::testCustomPalette() {
    QSignalSpy paletteSpy(m_colorPicker, &FluentColorPicker::customPaletteChanged);
    
    QList<QColor> customColors = {
        Qt::red, Qt::green, Qt::blue, Qt::yellow,
        Qt::cyan, Qt::magenta, Qt::black, Qt::white
    };
    
    m_colorPicker->setCustomPalette(customColors);
    QCOMPARE(m_colorPicker->customPalette(), customColors);
    QCOMPARE(paletteSpy.count(), 1);
    
    // Test empty palette
    m_colorPicker->setCustomPalette(QList<QColor>());
    QVERIFY(m_colorPicker->customPalette().isEmpty());
    QCOMPARE(paletteSpy.count(), 2);
}

void FluentColorPickerTest::testRecentColors() {
    QSignalSpy recentSpy(m_colorPicker, &FluentColorPicker::recentColorsChanged);
    
    // Add colors to recent list
    m_colorPicker->addRecentColor(Qt::red);
    m_colorPicker->addRecentColor(Qt::green);
    m_colorPicker->addRecentColor(Qt::blue);
    
    QList<QColor> recentColors = m_colorPicker->recentColors();
    QVERIFY(recentColors.contains(Qt::red));
    QVERIFY(recentColors.contains(Qt::green));
    QVERIFY(recentColors.contains(Qt::blue));
    QVERIFY(recentSpy.count() >= 3);
    
    // Clear recent colors
    m_colorPicker->clearRecentColors();
    QVERIFY(m_colorPicker->recentColors().isEmpty());
    QCOMPARE(recentSpy.count(), 4);
}

void FluentColorPickerTest::testAlphaEnabled() {
    QSignalSpy alphaSpy(m_colorPicker, &FluentColorPicker::alphaEnabledChanged);
    
    m_colorPicker->setAlphaEnabled(false);
    QVERIFY(!m_colorPicker->alphaEnabled());
    QCOMPARE(alphaSpy.count(), 1);
    
    m_colorPicker->setAlphaEnabled(true);
    QVERIFY(m_colorPicker->alphaEnabled());
    QCOMPARE(alphaSpy.count(), 2);
}

void FluentColorPickerTest::testAlphaValue() {
    QSignalSpy colorSpy(m_colorPicker, &FluentColorPicker::colorChanged);
    
    // Set alpha value
    m_colorPicker->setAlpha(128);
    QColor currentColor = m_colorPicker->currentColor();
    QCOMPARE(currentColor.alpha(), 128);
    QCOMPARE(colorSpy.count(), 1);
    
    // Test alpha bounds
    m_colorPicker->setAlpha(300); // Should clamp to 255
    currentColor = m_colorPicker->currentColor();
    QCOMPARE(currentColor.alpha(), 255);
    
    m_colorPicker->setAlpha(-10); // Should clamp to 0
    currentColor = m_colorPicker->currentColor();
    QCOMPARE(currentColor.alpha(), 0);
}

void FluentColorPickerTest::testColorSelection() {
    QSignalSpy colorSelectedSpy(m_colorPicker, &FluentColorPicker::colorSelected);
    QSignalSpy colorAcceptedSpy(m_colorPicker, &FluentColorPicker::colorAccepted);
    
    // Simulate color selection
    QColor selectedColor(255, 128, 64);
    m_colorPicker->selectColor(selectedColor);
    
    QCOMPARE(colorSelectedSpy.count(), 1);
    QCOMPARE(colorSelectedSpy.first().first().value<QColor>(), selectedColor);
    
    // Simulate color acceptance
    m_colorPicker->acceptColor();
    QCOMPARE(colorAcceptedSpy.count(), 1);
}

void FluentColorPickerTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QSignalSpy colorSpy(m_colorPicker, &FluentColorPicker::colorChanged);
    
    // Test mouse click on color picker
    QTest::mouseClick(m_colorPicker, Qt::LeftButton, Qt::NoModifier, QPoint(50, 50));
    
    // Mouse interaction testing is limited without knowing exact picker layout
    // In a real implementation, this would test clicking on specific color areas
    QTest::qWait(100);
}

void FluentColorPickerTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_colorPicker->setFocus();
    
    QSignalSpy colorSpy(m_colorPicker, &FluentColorPicker::colorChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_colorPicker, Qt::Key_Right);
    QTest::keyClick(m_colorPicker, Qt::Key_Left);
    QTest::keyClick(m_colorPicker, Qt::Key_Up);
    QTest::keyClick(m_colorPicker, Qt::Key_Down);
    
    // Test Enter key for selection
    QTest::keyClick(m_colorPicker, Qt::Key_Return);
    
    // Test Escape key for cancellation
    QTest::keyClick(m_colorPicker, Qt::Key_Escape);
    
    QTest::qWait(100);
}

void FluentColorPickerTest::testSignalEmission() {
    QSignalSpy colorSpy(m_colorPicker, &FluentColorPicker::colorChanged);
    QSignalSpy modeSpy(m_colorPicker, &FluentColorPicker::pickerModeChanged);
    QSignalSpy formatSpy(m_colorPicker, &FluentColorPicker::colorFormatChanged);
    QSignalSpy paletteSpy(m_colorPicker, &FluentColorPicker::showPaletteChanged);
    QSignalSpy customPaletteSpy(m_colorPicker, &FluentColorPicker::customPaletteChanged);
    QSignalSpy recentSpy(m_colorPicker, &FluentColorPicker::recentColorsChanged);
    QSignalSpy alphaSpy(m_colorPicker, &FluentColorPicker::alphaEnabledChanged);
    QSignalSpy colorSelectedSpy(m_colorPicker, &FluentColorPicker::colorSelected);
    QSignalSpy colorAcceptedSpy(m_colorPicker, &FluentColorPicker::colorAccepted);
    QSignalSpy colorRejectedSpy(m_colorPicker, &FluentColorPicker::colorRejected);
    
    // Test all signals
    m_colorPicker->setColor(Qt::red);
    m_colorPicker->setPickerMode(FluentColorPickerMode::Square);
    m_colorPicker->setColorFormat(FluentColorFormat::Hex);
    m_colorPicker->setShowPalette(false);
    m_colorPicker->setCustomPalette({Qt::red, Qt::green});
    m_colorPicker->addRecentColor(Qt::blue);
    m_colorPicker->setAlphaEnabled(false);
    m_colorPicker->selectColor(Qt::yellow);
    m_colorPicker->acceptColor();
    m_colorPicker->rejectColor();
    
    QCOMPARE(colorSpy.count(), 1);
    QCOMPARE(modeSpy.count(), 1);
    QCOMPARE(formatSpy.count(), 1);
    QCOMPARE(paletteSpy.count(), 1);
    QCOMPARE(customPaletteSpy.count(), 1);
    QCOMPARE(recentSpy.count(), 1);
    QCOMPARE(alphaSpy.count(), 1);
    QCOMPARE(colorSelectedSpy.count(), 1);
    QCOMPARE(colorAcceptedSpy.count(), 1);
    QCOMPARE(colorRejectedSpy.count(), 1);
}

void FluentColorPickerTest::testColorValidation() {
    // Test valid colors
    QVERIFY(m_colorPicker->isValidColor(Qt::red));
    QVERIFY(m_colorPicker->isValidColor(QColor(255, 128, 64)));
    QVERIFY(m_colorPicker->isValidColor(QColor(0, 0, 0, 128))); // With alpha
    
    // Test invalid colors
    QVERIFY(!m_colorPicker->isValidColor(QColor())); // Invalid color
    
    // Test color string parsing
    QColor parsedColor = m_colorPicker->colorFromString("#FF8040");
    QVERIFY(parsedColor.isValid());
    QCOMPARE(parsedColor.red(), 255);
    QCOMPARE(parsedColor.green(), 128);
    QCOMPARE(parsedColor.blue(), 64);
    
    // Test color to string conversion
    QString colorString = m_colorPicker->colorToString(Qt::red, FluentColorFormat::Hex);
    QCOMPARE(colorString.toUpper(), "#FF0000");
    
    colorString = m_colorPicker->colorToString(Qt::red, FluentColorFormat::Rgb);
    QCOMPARE(colorString, "rgb(255, 0, 0)");
}

QTEST_MAIN(FluentColorPickerTest)
#include "FluentColorPickerTest.moc"
