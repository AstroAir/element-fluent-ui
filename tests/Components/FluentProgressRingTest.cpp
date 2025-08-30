// tests/Components/FluentProgressRingTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentProgressRing.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentProgressRingTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Value tests
    void testValue();
    void testMinimum();
    void testMaximum();
    void testRange();

    // Progress mode tests
    void testProgressMode();
    void testIndeterminateMode();

    // Appearance tests
    void testRingSize();
    void testStrokeWidth();
    void testColor();
    void testBackgroundColor();

    // Animation tests
    void testAnimationEnabled();
    void testAnimationSpeed();
    void testAnimationDirection();

    // Text tests
    void testTextVisible();
    void testTextFormat();
    void testCustomText();

    // Signal tests
    void testSignalEmission();

    // Rendering tests
    void testRingRendering();

private:
    FluentProgressRing* m_progressRing{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentProgressRingTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentProgressRingTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentProgressRingTest::init() {
    m_progressRing = new FluentProgressRing(m_testWidget);
}

void FluentProgressRingTest::cleanup() {
    delete m_progressRing;
    m_progressRing = nullptr;
}

void FluentProgressRingTest::testDefaultConstructor() {
    QVERIFY(m_progressRing != nullptr);
    QCOMPARE(m_progressRing->value(), 0);
    QCOMPARE(m_progressRing->minimum(), 0);
    QCOMPARE(m_progressRing->maximum(), 100);
    QCOMPARE(m_progressRing->progressMode(), FluentProgressMode::Determinate);
    QVERIFY(m_progressRing->animationEnabled());
    QVERIFY(m_progressRing->textVisible());
}

void FluentProgressRingTest::testValue() {
    QSignalSpy valueSpy(m_progressRing, &FluentProgressRing::valueChanged);
    
    m_progressRing->setValue(50);
    QCOMPARE(m_progressRing->value(), 50);
    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(valueSpy.first().first().toInt(), 50);
    
    // Test bounds
    m_progressRing->setValue(150); // Above maximum
    QCOMPARE(m_progressRing->value(), 100); // Should clamp to maximum
    
    m_progressRing->setValue(-10); // Below minimum
    QCOMPARE(m_progressRing->value(), 0); // Should clamp to minimum
    
    // Test percentage
    QCOMPARE(m_progressRing->percentage(), 0.0);
    m_progressRing->setValue(25);
    QCOMPARE(m_progressRing->percentage(), 25.0);
}

void FluentProgressRingTest::testMinimum() {
    QSignalSpy rangeSpy(m_progressRing, &FluentProgressRing::rangeChanged);
    
    m_progressRing->setMinimum(10);
    QCOMPARE(m_progressRing->minimum(), 10);
    QCOMPARE(rangeSpy.count(), 1);
    
    // Test that value adjusts if below new minimum
    m_progressRing->setValue(5);
    QCOMPARE(m_progressRing->value(), 10); // Should adjust to minimum
}

void FluentProgressRingTest::testMaximum() {
    QSignalSpy rangeSpy(m_progressRing, &FluentProgressRing::rangeChanged);
    
    m_progressRing->setMaximum(200);
    QCOMPARE(m_progressRing->maximum(), 200);
    QCOMPARE(rangeSpy.count(), 1);
    
    // Test that value adjusts if above new maximum
    m_progressRing->setValue(250);
    QCOMPARE(m_progressRing->value(), 200); // Should adjust to maximum
}

void FluentProgressRingTest::testRange() {
    QSignalSpy rangeSpy(m_progressRing, &FluentProgressRing::rangeChanged);
    
    m_progressRing->setRange(20, 180);
    QCOMPARE(m_progressRing->minimum(), 20);
    QCOMPARE(m_progressRing->maximum(), 180);
    QCOMPARE(rangeSpy.count(), 1);
    
    // Test invalid range (min > max)
    m_progressRing->setRange(100, 50);
    QCOMPARE(m_progressRing->minimum(), 50); // Should swap values
    QCOMPARE(m_progressRing->maximum(), 100);
}

void FluentProgressRingTest::testProgressMode() {
    QSignalSpy progressModeSpy(m_progressRing, &FluentProgressRing::progressModeChanged);
    
    m_progressRing->setProgressMode(FluentProgressMode::Indeterminate);
    QCOMPARE(m_progressRing->progressMode(), FluentProgressMode::Indeterminate);
    QCOMPARE(progressModeSpy.count(), 1);
    
    m_progressRing->setProgressMode(FluentProgressMode::Determinate);
    QCOMPARE(m_progressRing->progressMode(), FluentProgressMode::Determinate);
    QCOMPARE(progressModeSpy.count(), 2);
}

void FluentProgressRingTest::testIndeterminateMode() {
    m_progressRing->setProgressMode(FluentProgressMode::Indeterminate);
    
    // In indeterminate mode, value should not affect display
    m_progressRing->setValue(50);
    QVERIFY(m_progressRing->isIndeterminate());
    
    // Test switching back to determinate
    m_progressRing->setProgressMode(FluentProgressMode::Determinate);
    QVERIFY(!m_progressRing->isIndeterminate());
    QCOMPARE(m_progressRing->value(), 50); // Value should be preserved
}

void FluentProgressRingTest::testRingSize() {
    QSignalSpy ringSizeSpy(m_progressRing, &FluentProgressRing::ringSizeChanged);
    
    m_progressRing->setRingSize(FluentProgressRingSize::Small);
    QCOMPARE(m_progressRing->ringSize(), FluentProgressRingSize::Small);
    QCOMPARE(ringSizeSpy.count(), 1);
    
    m_progressRing->setRingSize(FluentProgressRingSize::Large);
    QCOMPARE(m_progressRing->ringSize(), FluentProgressRingSize::Large);
    QCOMPARE(ringSizeSpy.count(), 2);
    
    m_progressRing->setRingSize(FluentProgressRingSize::ExtraLarge);
    QCOMPARE(m_progressRing->ringSize(), FluentProgressRingSize::ExtraLarge);
    QCOMPARE(ringSizeSpy.count(), 3);
}

void FluentProgressRingTest::testStrokeWidth() {
    QSignalSpy strokeWidthSpy(m_progressRing, &FluentProgressRing::strokeWidthChanged);
    
    m_progressRing->setStrokeWidth(8);
    QCOMPARE(m_progressRing->strokeWidth(), 8);
    QCOMPARE(strokeWidthSpy.count(), 1);
    
    // Test invalid stroke width
    m_progressRing->setStrokeWidth(-2);
    QVERIFY(m_progressRing->strokeWidth() > 0); // Should remain positive
}

void FluentProgressRingTest::testColor() {
    QSignalSpy colorSpy(m_progressRing, &FluentProgressRing::colorChanged);
    
    QColor testColor(255, 0, 0);
    m_progressRing->setColor(testColor);
    QCOMPARE(m_progressRing->color(), testColor);
    QCOMPARE(colorSpy.count(), 1);
    
    // Test invalid color
    m_progressRing->setColor(QColor());
    QVERIFY(m_progressRing->color().isValid()); // Should remain valid
}

void FluentProgressRingTest::testBackgroundColor() {
    QSignalSpy backgroundColorSpy(m_progressRing, &FluentProgressRing::backgroundColorChanged);
    
    QColor testColor(0, 255, 0);
    m_progressRing->setBackgroundColor(testColor);
    QCOMPARE(m_progressRing->backgroundColor(), testColor);
    QCOMPARE(backgroundColorSpy.count(), 1);
    
    // Test transparent background
    m_progressRing->setBackgroundColor(Qt::transparent);
    QCOMPARE(m_progressRing->backgroundColor(), Qt::transparent);
    QCOMPARE(backgroundColorSpy.count(), 2);
}

void FluentProgressRingTest::testAnimationEnabled() {
    QSignalSpy animationSpy(m_progressRing, &FluentProgressRing::animationEnabledChanged);
    
    m_progressRing->setAnimationEnabled(false);
    QVERIFY(!m_progressRing->animationEnabled());
    QCOMPARE(animationSpy.count(), 1);
    
    m_progressRing->setAnimationEnabled(true);
    QVERIFY(m_progressRing->animationEnabled());
    QCOMPARE(animationSpy.count(), 2);
}

void FluentProgressRingTest::testAnimationSpeed() {
    QSignalSpy speedSpy(m_progressRing, &FluentProgressRing::animationSpeedChanged);
    
    m_progressRing->setAnimationSpeed(FluentAnimationSpeed::Slow);
    QCOMPARE(m_progressRing->animationSpeed(), FluentAnimationSpeed::Slow);
    QCOMPARE(speedSpy.count(), 1);
    
    m_progressRing->setAnimationSpeed(FluentAnimationSpeed::Fast);
    QCOMPARE(m_progressRing->animationSpeed(), FluentAnimationSpeed::Fast);
    QCOMPARE(speedSpy.count(), 2);
}

void FluentProgressRingTest::testAnimationDirection() {
    QSignalSpy directionSpy(m_progressRing, &FluentProgressRing::animationDirectionChanged);
    
    m_progressRing->setAnimationDirection(FluentAnimationDirection::Counterclockwise);
    QCOMPARE(m_progressRing->animationDirection(), FluentAnimationDirection::Counterclockwise);
    QCOMPARE(directionSpy.count(), 1);
    
    m_progressRing->setAnimationDirection(FluentAnimationDirection::Clockwise);
    QCOMPARE(m_progressRing->animationDirection(), FluentAnimationDirection::Clockwise);
    QCOMPARE(directionSpy.count(), 2);
}

void FluentProgressRingTest::testTextVisible() {
    QSignalSpy textVisibleSpy(m_progressRing, &FluentProgressRing::textVisibleChanged);
    
    m_progressRing->setTextVisible(false);
    QVERIFY(!m_progressRing->textVisible());
    QCOMPARE(textVisibleSpy.count(), 1);
    
    m_progressRing->setTextVisible(true);
    QVERIFY(m_progressRing->textVisible());
    QCOMPARE(textVisibleSpy.count(), 2);
}

void FluentProgressRingTest::testTextFormat() {
    QSignalSpy textFormatSpy(m_progressRing, &FluentProgressRing::textFormatChanged);
    
    m_progressRing->setTextFormat("%p%");
    QCOMPARE(m_progressRing->textFormat(), "%p%");
    QCOMPARE(textFormatSpy.count(), 1);
    
    // Test text generation
    m_progressRing->setValue(75);
    QString text = m_progressRing->text();
    QVERIFY(text.contains("75%"));
    
    // Test custom format
    m_progressRing->setTextFormat("%v/%m");
    text = m_progressRing->text();
    QVERIFY(text.contains("75/100"));
}

void FluentProgressRingTest::testCustomText() {
    QSignalSpy customTextSpy(m_progressRing, &FluentProgressRing::customTextChanged);
    
    m_progressRing->setCustomText("Loading...");
    QCOMPARE(m_progressRing->customText(), "Loading...");
    QCOMPARE(customTextSpy.count(), 1);
    
    // Custom text should override format
    QString displayText = m_progressRing->text();
    QCOMPARE(displayText, "Loading...");
    
    // Clear custom text
    m_progressRing->setCustomText("");
    QCOMPARE(m_progressRing->customText(), "");
    QCOMPARE(customTextSpy.count(), 2);
}

void FluentProgressRingTest::testSignalEmission() {
    QSignalSpy valueSpy(m_progressRing, &FluentProgressRing::valueChanged);
    QSignalSpy rangeSpy(m_progressRing, &FluentProgressRing::rangeChanged);
    QSignalSpy progressModeSpy(m_progressRing, &FluentProgressRing::progressModeChanged);
    QSignalSpy ringSizeSpy(m_progressRing, &FluentProgressRing::ringSizeChanged);
    QSignalSpy strokeWidthSpy(m_progressRing, &FluentProgressRing::strokeWidthChanged);
    QSignalSpy colorSpy(m_progressRing, &FluentProgressRing::colorChanged);
    QSignalSpy backgroundColorSpy(m_progressRing, &FluentProgressRing::backgroundColorChanged);
    QSignalSpy animationSpy(m_progressRing, &FluentProgressRing::animationEnabledChanged);
    QSignalSpy speedSpy(m_progressRing, &FluentProgressRing::animationSpeedChanged);
    QSignalSpy directionSpy(m_progressRing, &FluentProgressRing::animationDirectionChanged);
    QSignalSpy textVisibleSpy(m_progressRing, &FluentProgressRing::textVisibleChanged);
    QSignalSpy textFormatSpy(m_progressRing, &FluentProgressRing::textFormatChanged);
    QSignalSpy customTextSpy(m_progressRing, &FluentProgressRing::customTextChanged);
    
    // Test all signals
    m_progressRing->setValue(60);
    m_progressRing->setRange(0, 200);
    m_progressRing->setProgressMode(FluentProgressMode::Indeterminate);
    m_progressRing->setRingSize(FluentProgressRingSize::Large);
    m_progressRing->setStrokeWidth(6);
    m_progressRing->setColor(Qt::blue);
    m_progressRing->setBackgroundColor(Qt::gray);
    m_progressRing->setAnimationEnabled(false);
    m_progressRing->setAnimationSpeed(FluentAnimationSpeed::Fast);
    m_progressRing->setAnimationDirection(FluentAnimationDirection::Counterclockwise);
    m_progressRing->setTextVisible(false);
    m_progressRing->setTextFormat("%v of %m");
    m_progressRing->setCustomText("Custom");
    
    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(rangeSpy.count(), 1);
    QCOMPARE(progressModeSpy.count(), 1);
    QCOMPARE(ringSizeSpy.count(), 1);
    QCOMPARE(strokeWidthSpy.count(), 1);
    QCOMPARE(colorSpy.count(), 1);
    QCOMPARE(backgroundColorSpy.count(), 1);
    QCOMPARE(animationSpy.count(), 1);
    QCOMPARE(speedSpy.count(), 1);
    QCOMPARE(directionSpy.count(), 1);
    QCOMPARE(textVisibleSpy.count(), 1);
    QCOMPARE(textFormatSpy.count(), 1);
    QCOMPARE(customTextSpy.count(), 1);
}

void FluentProgressRingTest::testRingRendering() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Test different progress values
    for (int i = 0; i <= 100; i += 25) {
        m_progressRing->setValue(i);
        QTest::qWait(50);
    }
    
    // Test indeterminate animation
    m_progressRing->setProgressMode(FluentProgressMode::Indeterminate);
    QTest::qWait(200);
    
    // Test different sizes
    m_progressRing->setProgressMode(FluentProgressMode::Determinate);
    m_progressRing->setValue(50);
    
    m_progressRing->setRingSize(FluentProgressRingSize::Small);
    QTest::qWait(50);
    
    m_progressRing->setRingSize(FluentProgressRingSize::Large);
    QTest::qWait(50);
    
    // Test different stroke widths
    m_progressRing->setStrokeWidth(2);
    QTest::qWait(50);
    
    m_progressRing->setStrokeWidth(10);
    QTest::qWait(50);
    
    // Test different colors
    m_progressRing->setColor(Qt::red);
    m_progressRing->setBackgroundColor(Qt::lightGray);
    QTest::qWait(50);
    
    // Test with text
    m_progressRing->setTextVisible(true);
    m_progressRing->setCustomText("75%");
    QTest::qWait(50);
    
    // Test animation directions
    m_progressRing->setProgressMode(FluentProgressMode::Indeterminate);
    m_progressRing->setAnimationDirection(FluentAnimationDirection::Clockwise);
    QTest::qWait(100);
    
    m_progressRing->setAnimationDirection(FluentAnimationDirection::Counterclockwise);
    QTest::qWait(100);
    
    // Test animation speeds
    m_progressRing->setAnimationSpeed(FluentAnimationSpeed::Slow);
    QTest::qWait(100);
    
    m_progressRing->setAnimationSpeed(FluentAnimationSpeed::Fast);
    QTest::qWait(100);
    
    // All rendering tests should complete without crashing
    QVERIFY(m_progressRing->isVisible());
}

QTEST_MAIN(FluentProgressRingTest)
#include "FluentProgressRingTest.moc"
