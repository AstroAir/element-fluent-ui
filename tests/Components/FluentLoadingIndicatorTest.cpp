// tests/Components/FluentLoadingIndicatorTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentLoadingIndicatorTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // State tests
    void testActive();
    void testStartStop();

    // Appearance tests
    void testIndicatorType();
    void testSize();
    void testColor();
    void testBackgroundColor();

    // Animation tests
    void testAnimationSpeed();
    void testAnimationDirection();
    void testAnimationDuration();

    // Text tests
    void testText();
    void testTextPosition();
    void testTextVisible();

    // Progress tests
    void testProgressMode();
    void testProgressValue();
    void testProgressRange();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testVisibilityStates();

private:
    FluentLoadingIndicator* m_indicator{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentLoadingIndicatorTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentLoadingIndicatorTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentLoadingIndicatorTest::init() {
    m_indicator = new FluentLoadingIndicator(m_testWidget);
}

void FluentLoadingIndicatorTest::cleanup() {
    delete m_indicator;
    m_indicator = nullptr;
}

void FluentLoadingIndicatorTest::testDefaultConstructor() {
    QVERIFY(m_indicator != nullptr);
    QVERIFY(!m_indicator->isActive());
    QCOMPARE(m_indicator->indicatorType(), FluentLoadingIndicatorType::Spinner);
    QCOMPARE(m_indicator->size(), FluentLoadingIndicatorSize::Medium);
    QVERIFY(m_indicator->textVisible());
    QCOMPARE(m_indicator->progressMode(), FluentProgressMode::Indeterminate);
}

void FluentLoadingIndicatorTest::testActive() {
    QSignalSpy activeSpy(m_indicator, &FluentLoadingIndicator::activeChanged);
    
    m_indicator->setActive(true);
    QVERIFY(m_indicator->isActive());
    QCOMPARE(activeSpy.count(), 1);
    
    m_indicator->setActive(false);
    QVERIFY(!m_indicator->isActive());
    QCOMPARE(activeSpy.count(), 2);
}

void FluentLoadingIndicatorTest::testStartStop() {
    QSignalSpy activeSpy(m_indicator, &FluentLoadingIndicator::activeChanged);
    QSignalSpy startedSpy(m_indicator, &FluentLoadingIndicator::started);
    QSignalSpy stoppedSpy(m_indicator, &FluentLoadingIndicator::stopped);
    
    m_indicator->start();
    QVERIFY(m_indicator->isActive());
    QCOMPARE(activeSpy.count(), 1);
    QCOMPARE(startedSpy.count(), 1);
    
    m_indicator->stop();
    QVERIFY(!m_indicator->isActive());
    QCOMPARE(activeSpy.count(), 2);
    QCOMPARE(stoppedSpy.count(), 1);
    
    // Test toggle
    m_indicator->toggle();
    QVERIFY(m_indicator->isActive());
    QCOMPARE(activeSpy.count(), 3);
    
    m_indicator->toggle();
    QVERIFY(!m_indicator->isActive());
    QCOMPARE(activeSpy.count(), 4);
}

void FluentLoadingIndicatorTest::testIndicatorType() {
    QSignalSpy typeSpy(m_indicator, &FluentLoadingIndicator::indicatorTypeChanged);
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Dots);
    QCOMPARE(m_indicator->indicatorType(), FluentLoadingIndicatorType::Dots);
    QCOMPARE(typeSpy.count(), 1);
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::ProgressBar);
    QCOMPARE(m_indicator->indicatorType(), FluentLoadingIndicatorType::ProgressBar);
    QCOMPARE(typeSpy.count(), 2);
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Ring);
    QCOMPARE(m_indicator->indicatorType(), FluentLoadingIndicatorType::Ring);
    QCOMPARE(typeSpy.count(), 3);
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Pulse);
    QCOMPARE(m_indicator->indicatorType(), FluentLoadingIndicatorType::Pulse);
    QCOMPARE(typeSpy.count(), 4);
}

void FluentLoadingIndicatorTest::testSize() {
    QSignalSpy sizeSpy(m_indicator, &FluentLoadingIndicator::sizeChanged);
    
    m_indicator->setSize(FluentLoadingIndicatorSize::Small);
    QCOMPARE(m_indicator->size(), FluentLoadingIndicatorSize::Small);
    QCOMPARE(sizeSpy.count(), 1);
    
    m_indicator->setSize(FluentLoadingIndicatorSize::Large);
    QCOMPARE(m_indicator->size(), FluentLoadingIndicatorSize::Large);
    QCOMPARE(sizeSpy.count(), 2);
    
    m_indicator->setSize(FluentLoadingIndicatorSize::ExtraLarge);
    QCOMPARE(m_indicator->size(), FluentLoadingIndicatorSize::ExtraLarge);
    QCOMPARE(sizeSpy.count(), 3);
}

void FluentLoadingIndicatorTest::testColor() {
    QSignalSpy colorSpy(m_indicator, &FluentLoadingIndicator::colorChanged);
    
    QColor testColor(255, 0, 0);
    m_indicator->setColor(testColor);
    QCOMPARE(m_indicator->color(), testColor);
    QCOMPARE(colorSpy.count(), 1);
    
    // Test invalid color
    m_indicator->setColor(QColor());
    QVERIFY(m_indicator->color().isValid()); // Should remain valid
}

void FluentLoadingIndicatorTest::testBackgroundColor() {
    QSignalSpy backgroundColorSpy(m_indicator, &FluentLoadingIndicator::backgroundColorChanged);
    
    QColor testColor(0, 255, 0);
    m_indicator->setBackgroundColor(testColor);
    QCOMPARE(m_indicator->backgroundColor(), testColor);
    QCOMPARE(backgroundColorSpy.count(), 1);
    
    // Test transparent background
    m_indicator->setBackgroundColor(Qt::transparent);
    QCOMPARE(m_indicator->backgroundColor(), Qt::transparent);
    QCOMPARE(backgroundColorSpy.count(), 2);
}

void FluentLoadingIndicatorTest::testAnimationSpeed() {
    QSignalSpy speedSpy(m_indicator, &FluentLoadingIndicator::animationSpeedChanged);
    
    m_indicator->setAnimationSpeed(FluentAnimationSpeed::Slow);
    QCOMPARE(m_indicator->animationSpeed(), FluentAnimationSpeed::Slow);
    QCOMPARE(speedSpy.count(), 1);
    
    m_indicator->setAnimationSpeed(FluentAnimationSpeed::Fast);
    QCOMPARE(m_indicator->animationSpeed(), FluentAnimationSpeed::Fast);
    QCOMPARE(speedSpy.count(), 2);
    
    m_indicator->setAnimationSpeed(FluentAnimationSpeed::Normal);
    QCOMPARE(m_indicator->animationSpeed(), FluentAnimationSpeed::Normal);
    QCOMPARE(speedSpy.count(), 3);
}

void FluentLoadingIndicatorTest::testAnimationDirection() {
    QSignalSpy directionSpy(m_indicator, &FluentLoadingIndicator::animationDirectionChanged);
    
    m_indicator->setAnimationDirection(FluentAnimationDirection::Counterclockwise);
    QCOMPARE(m_indicator->animationDirection(), FluentAnimationDirection::Counterclockwise);
    QCOMPARE(directionSpy.count(), 1);
    
    m_indicator->setAnimationDirection(FluentAnimationDirection::Clockwise);
    QCOMPARE(m_indicator->animationDirection(), FluentAnimationDirection::Clockwise);
    QCOMPARE(directionSpy.count(), 2);
}

void FluentLoadingIndicatorTest::testAnimationDuration() {
    QSignalSpy durationSpy(m_indicator, &FluentLoadingIndicator::animationDurationChanged);
    
    m_indicator->setAnimationDuration(2000);
    QCOMPARE(m_indicator->animationDuration(), 2000);
    QCOMPARE(durationSpy.count(), 1);
    
    // Test invalid duration
    m_indicator->setAnimationDuration(-500);
    QVERIFY(m_indicator->animationDuration() > 0); // Should remain positive
}

void FluentLoadingIndicatorTest::testText() {
    QSignalSpy textSpy(m_indicator, &FluentLoadingIndicator::textChanged);
    
    m_indicator->setText("Loading...");
    QCOMPARE(m_indicator->text(), "Loading...");
    QCOMPARE(textSpy.count(), 1);
    
    // Test empty text
    m_indicator->setText("");
    QCOMPARE(m_indicator->text(), "");
    QCOMPARE(textSpy.count(), 2);
    
    // Test HTML text
    m_indicator->setText("<b>Loading</b> data...");
    QCOMPARE(m_indicator->text(), "<b>Loading</b> data...");
    QCOMPARE(textSpy.count(), 3);
}

void FluentLoadingIndicatorTest::testTextPosition() {
    QSignalSpy textPositionSpy(m_indicator, &FluentLoadingIndicator::textPositionChanged);
    
    m_indicator->setTextPosition(FluentLoadingTextPosition::Top);
    QCOMPARE(m_indicator->textPosition(), FluentLoadingTextPosition::Top);
    QCOMPARE(textPositionSpy.count(), 1);
    
    m_indicator->setTextPosition(FluentLoadingTextPosition::Left);
    QCOMPARE(m_indicator->textPosition(), FluentLoadingTextPosition::Left);
    QCOMPARE(textPositionSpy.count(), 2);
    
    m_indicator->setTextPosition(FluentLoadingTextPosition::Right);
    QCOMPARE(m_indicator->textPosition(), FluentLoadingTextPosition::Right);
    QCOMPARE(textPositionSpy.count(), 3);
    
    m_indicator->setTextPosition(FluentLoadingTextPosition::Bottom);
    QCOMPARE(m_indicator->textPosition(), FluentLoadingTextPosition::Bottom);
    QCOMPARE(textPositionSpy.count(), 4);
}

void FluentLoadingIndicatorTest::testTextVisible() {
    QSignalSpy textVisibleSpy(m_indicator, &FluentLoadingIndicator::textVisibleChanged);
    
    m_indicator->setTextVisible(false);
    QVERIFY(!m_indicator->textVisible());
    QCOMPARE(textVisibleSpy.count(), 1);
    
    m_indicator->setTextVisible(true);
    QVERIFY(m_indicator->textVisible());
    QCOMPARE(textVisibleSpy.count(), 2);
}

void FluentLoadingIndicatorTest::testProgressMode() {
    QSignalSpy progressModeSpy(m_indicator, &FluentLoadingIndicator::progressModeChanged);
    
    m_indicator->setProgressMode(FluentProgressMode::Determinate);
    QCOMPARE(m_indicator->progressMode(), FluentProgressMode::Determinate);
    QCOMPARE(progressModeSpy.count(), 1);
    
    m_indicator->setProgressMode(FluentProgressMode::Indeterminate);
    QCOMPARE(m_indicator->progressMode(), FluentProgressMode::Indeterminate);
    QCOMPARE(progressModeSpy.count(), 2);
}

void FluentLoadingIndicatorTest::testProgressValue() {
    m_indicator->setProgressMode(FluentProgressMode::Determinate);
    
    QSignalSpy progressValueSpy(m_indicator, &FluentLoadingIndicator::progressValueChanged);
    
    m_indicator->setProgressValue(50);
    QCOMPARE(m_indicator->progressValue(), 50);
    QCOMPARE(progressValueSpy.count(), 1);
    
    // Test bounds
    m_indicator->setProgressValue(150); // Above maximum
    QCOMPARE(m_indicator->progressValue(), 100); // Should clamp to maximum
    
    m_indicator->setProgressValue(-10); // Below minimum
    QCOMPARE(m_indicator->progressValue(), 0); // Should clamp to minimum
}

void FluentLoadingIndicatorTest::testProgressRange() {
    QSignalSpy progressRangeSpy(m_indicator, &FluentLoadingIndicator::progressRangeChanged);
    
    m_indicator->setProgressRange(0, 200);
    QCOMPARE(m_indicator->progressMinimum(), 0);
    QCOMPARE(m_indicator->progressMaximum(), 200);
    QCOMPARE(progressRangeSpy.count(), 1);
    
    // Test individual setters
    m_indicator->setProgressMinimum(10);
    QCOMPARE(m_indicator->progressMinimum(), 10);
    QCOMPARE(progressRangeSpy.count(), 2);
    
    m_indicator->setProgressMaximum(150);
    QCOMPARE(m_indicator->progressMaximum(), 150);
    QCOMPARE(progressRangeSpy.count(), 3);
    
    // Test invalid range
    m_indicator->setProgressRange(100, 50); // min > max
    QVERIFY(m_indicator->progressMinimum() <= m_indicator->progressMaximum());
}

void FluentLoadingIndicatorTest::testSignalEmission() {
    QSignalSpy activeSpy(m_indicator, &FluentLoadingIndicator::activeChanged);
    QSignalSpy startedSpy(m_indicator, &FluentLoadingIndicator::started);
    QSignalSpy stoppedSpy(m_indicator, &FluentLoadingIndicator::stopped);
    QSignalSpy typeSpy(m_indicator, &FluentLoadingIndicator::indicatorTypeChanged);
    QSignalSpy sizeSpy(m_indicator, &FluentLoadingIndicator::sizeChanged);
    QSignalSpy colorSpy(m_indicator, &FluentLoadingIndicator::colorChanged);
    QSignalSpy backgroundColorSpy(m_indicator, &FluentLoadingIndicator::backgroundColorChanged);
    QSignalSpy speedSpy(m_indicator, &FluentLoadingIndicator::animationSpeedChanged);
    QSignalSpy directionSpy(m_indicator, &FluentLoadingIndicator::animationDirectionChanged);
    QSignalSpy durationSpy(m_indicator, &FluentLoadingIndicator::animationDurationChanged);
    QSignalSpy textSpy(m_indicator, &FluentLoadingIndicator::textChanged);
    QSignalSpy textPositionSpy(m_indicator, &FluentLoadingIndicator::textPositionChanged);
    QSignalSpy textVisibleSpy(m_indicator, &FluentLoadingIndicator::textVisibleChanged);
    QSignalSpy progressModeSpy(m_indicator, &FluentLoadingIndicator::progressModeChanged);
    QSignalSpy progressValueSpy(m_indicator, &FluentLoadingIndicator::progressValueChanged);
    QSignalSpy progressRangeSpy(m_indicator, &FluentLoadingIndicator::progressRangeChanged);
    
    // Test all signals
    m_indicator->start();
    m_indicator->stop();
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Dots);
    m_indicator->setSize(FluentLoadingIndicatorSize::Large);
    m_indicator->setColor(Qt::red);
    m_indicator->setBackgroundColor(Qt::blue);
    m_indicator->setAnimationSpeed(FluentAnimationSpeed::Fast);
    m_indicator->setAnimationDirection(FluentAnimationDirection::Counterclockwise);
    m_indicator->setAnimationDuration(1500);
    m_indicator->setText("Loading...");
    m_indicator->setTextPosition(FluentLoadingTextPosition::Top);
    m_indicator->setTextVisible(false);
    m_indicator->setProgressMode(FluentProgressMode::Determinate);
    m_indicator->setProgressValue(75);
    m_indicator->setProgressRange(0, 200);
    
    QCOMPARE(activeSpy.count(), 2); // start, stop
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(stoppedSpy.count(), 1);
    QCOMPARE(typeSpy.count(), 1);
    QCOMPARE(sizeSpy.count(), 1);
    QCOMPARE(colorSpy.count(), 1);
    QCOMPARE(backgroundColorSpy.count(), 1);
    QCOMPARE(speedSpy.count(), 1);
    QCOMPARE(directionSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(textSpy.count(), 1);
    QCOMPARE(textPositionSpy.count(), 1);
    QCOMPARE(textVisibleSpy.count(), 1);
    QCOMPARE(progressModeSpy.count(), 1);
    QCOMPARE(progressValueSpy.count(), 1);
    QCOMPARE(progressRangeSpy.count(), 1);
}

void FluentLoadingIndicatorTest::testVisibilityStates() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Test that indicator becomes visible when started
    m_indicator->start();
    QVERIFY(m_indicator->isVisible());
    
    // Test animation is running
    QVERIFY(m_indicator->isActive());
    
    // Wait for animation
    QTest::qWait(100);
    
    // Test that indicator can be hidden when stopped
    m_indicator->stop();
    QVERIFY(!m_indicator->isActive());
    
    // Test different indicator types render without crashing
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Spinner);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Dots);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::ProgressBar);
    m_indicator->setProgressMode(FluentProgressMode::Determinate);
    m_indicator->setProgressValue(50);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Ring);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
    
    m_indicator->setIndicatorType(FluentLoadingIndicatorType::Pulse);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
    
    // Test with text
    m_indicator->setText("Loading data...");
    m_indicator->setTextVisible(true);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
    
    // Test different sizes
    m_indicator->setSize(FluentLoadingIndicatorSize::Small);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
    
    m_indicator->setSize(FluentLoadingIndicatorSize::Large);
    m_indicator->start();
    QTest::qWait(50);
    m_indicator->stop();
}

QTEST_MAIN(FluentLoadingIndicatorTest)
#include "FluentLoadingIndicatorTest.moc"
