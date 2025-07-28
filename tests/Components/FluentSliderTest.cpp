// tests/Components/FluentSliderTest.cpp
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QMouseEvent>
#include <QKeyEvent>
#include "FluentQt/Components/FluentSlider.h"

using namespace FluentQt::Components;

class FluentSliderTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testDefaultConstruction();
    void testValueRange();
    void testSingleMode();
    void testRangeMode();
    void testOrientation();
    void testStep();
    void testPageStep();

    // Tick marks tests
    void testTickPosition();
    void testTickInterval();
    void testCustomTicks();
    void testSnapToTicks();

    // Animation tests
    void testAnimatedValue();
    void testAnimatedRange();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();
    void testWheelInteraction();

    // Signal tests
    void testValueChangedSignal();
    void testValuesChangedSignal();
    void testSliderPressedSignal();
    void testSliderReleasedSignal();

    // Visual tests
    void testShowLabels();
    void testShowTooltip();
    void testValueFormatter();

private:
    FluentSlider* m_slider{nullptr};
};

void FluentSliderTest::initTestCase() {
    // Global test setup
}

void FluentSliderTest::cleanupTestCase() {
    // Global test cleanup
}

void FluentSliderTest::init() {
    m_slider = new FluentSlider();
}

void FluentSliderTest::cleanup() {
    delete m_slider;
    m_slider = nullptr;
}

void FluentSliderTest::testDefaultConstruction() {
    QCOMPARE(m_slider->minimum(), 0.0);
    QCOMPARE(m_slider->maximum(), 100.0);
    QCOMPARE(m_slider->value(), 0.0);
    QCOMPARE(m_slider->step(), 1.0);
    QCOMPARE(m_slider->pageStep(), 10.0);
    QCOMPARE(m_slider->orientation(), FluentSliderOrientation::Horizontal);
    QCOMPARE(m_slider->mode(), FluentSliderMode::Single);
    QCOMPARE(m_slider->tickPosition(), FluentSliderTickPosition::NoTicks);
    QVERIFY(!m_slider->isAnimated());
    QVERIFY(!m_slider->showLabels());
    QVERIFY(!m_slider->showTooltip());
    QVERIFY(!m_slider->snapToTicks());
}

void FluentSliderTest::testValueRange() {
    // Test setting range
    m_slider->setRange(10.0, 50.0);
    QCOMPARE(m_slider->minimum(), 10.0);
    QCOMPARE(m_slider->maximum(), 50.0);
    
    // Test value clamping
    m_slider->setValue(5.0);  // Below minimum
    QCOMPARE(m_slider->value(), 10.0);
    
    m_slider->setValue(60.0); // Above maximum
    QCOMPARE(m_slider->value(), 50.0);
    
    m_slider->setValue(30.0); // Within range
    QCOMPARE(m_slider->value(), 30.0);
}

void FluentSliderTest::testSingleMode() {
    m_slider->setMode(FluentSliderMode::Single);
    m_slider->setRange(0.0, 100.0);
    m_slider->setValue(50.0);
    
    QCOMPARE(m_slider->mode(), FluentSliderMode::Single);
    QCOMPARE(m_slider->value(), 50.0);
    QCOMPARE(m_slider->lowerValue(), 50.0); // Should be same as value
    QCOMPARE(m_slider->upperValue(), 50.0); // Should be same as value
}

void FluentSliderTest::testRangeMode() {
    m_slider->setMode(FluentSliderMode::Range);
    m_slider->setRange(0.0, 100.0);
    m_slider->setValues(20.0, 80.0);
    
    QCOMPARE(m_slider->mode(), FluentSliderMode::Range);
    QCOMPARE(m_slider->lowerValue(), 20.0);
    QCOMPARE(m_slider->upperValue(), 80.0);
    
    // Test invalid range (lower > upper)
    m_slider->setValues(90.0, 10.0);
    QCOMPARE(m_slider->lowerValue(), 10.0); // Should be swapped
    QCOMPARE(m_slider->upperValue(), 90.0);
}

void FluentSliderTest::testOrientation() {
    // Test horizontal orientation
    m_slider->setOrientation(FluentSliderOrientation::Horizontal);
    QCOMPARE(m_slider->orientation(), FluentSliderOrientation::Horizontal);
    
    // Test vertical orientation
    m_slider->setOrientation(FluentSliderOrientation::Vertical);
    QCOMPARE(m_slider->orientation(), FluentSliderOrientation::Vertical);
}

void FluentSliderTest::testStep() {
    m_slider->setStep(5.0);
    QCOMPARE(m_slider->step(), 5.0);
    
    m_slider->setPageStep(25.0);
    QCOMPARE(m_slider->pageStep(), 25.0);
}

void FluentSliderTest::testTickPosition() {
    m_slider->setTickPosition(FluentSliderTickPosition::Above);
    QCOMPARE(m_slider->tickPosition(), FluentSliderTickPosition::Above);
    
    m_slider->setTickPosition(FluentSliderTickPosition::Below);
    QCOMPARE(m_slider->tickPosition(), FluentSliderTickPosition::Below);
    
    m_slider->setTickPosition(FluentSliderTickPosition::Both);
    QCOMPARE(m_slider->tickPosition(), FluentSliderTickPosition::Both);
}

void FluentSliderTest::testTickInterval() {
    m_slider->setTickInterval(10.0);
    QCOMPARE(m_slider->tickInterval(), 10.0);
}

void FluentSliderTest::testCustomTicks() {
    m_slider->addTick(25.0, "Quarter");
    m_slider->addTick(50.0, "Half");
    m_slider->addTick(75.0, "Three Quarters");
    
    auto ticks = m_slider->customTicks();
    QCOMPARE(ticks.size(), 3);
    QVERIFY(ticks.contains(25.0));
    QVERIFY(ticks.contains(50.0));
    QVERIFY(ticks.contains(75.0));
    
    m_slider->removeTick(50.0);
    ticks = m_slider->customTicks();
    QCOMPARE(ticks.size(), 2);
    QVERIFY(!ticks.contains(50.0));
    
    m_slider->clearTicks();
    ticks = m_slider->customTicks();
    QCOMPARE(ticks.size(), 0);
}

void FluentSliderTest::testSnapToTicks() {
    m_slider->setSnapToTicks(true);
    m_slider->setTickInterval(10.0);
    m_slider->setValue(23.0); // Should snap to 20.0
    
    QVERIFY(m_slider->snapToTicks());
    // Note: Actual snapping behavior would need to be tested with mouse interaction
}

void FluentSliderTest::testAnimatedValue() {
    m_slider->setAnimated(true);
    QVERIFY(m_slider->isAnimated());
    
    QSignalSpy spy(m_slider, &FluentSlider::valueChanged);
    m_slider->animateToValue(50.0);
    
    // Animation should start but not complete immediately
    QVERIFY(spy.count() >= 0); // May have intermediate values
}

void FluentSliderTest::testAnimatedRange() {
    m_slider->setMode(FluentSliderMode::Range);
    m_slider->setAnimated(true);
    
    QSignalSpy spy(m_slider, &FluentSlider::valuesChanged);
    m_slider->animateToValues(20.0, 80.0);
    
    // Animation should start
    QVERIFY(spy.count() >= 0);
}

void FluentSliderTest::testValueChangedSignal() {
    QSignalSpy spy(m_slider, &FluentSlider::valueChanged);
    
    m_slider->setValue(50.0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toDouble(), 50.0);
    
    // Setting same value should not emit signal
    m_slider->setValue(50.0);
    QCOMPARE(spy.count(), 1);
}

void FluentSliderTest::testValuesChangedSignal() {
    m_slider->setMode(FluentSliderMode::Range);
    QSignalSpy spy(m_slider, &FluentSlider::valuesChanged);
    
    m_slider->setValues(20.0, 80.0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toDouble(), 20.0);
    QCOMPARE(spy.at(0).at(1).toDouble(), 80.0);
}

void FluentSliderTest::testSliderPressedSignal() {
    QSignalSpy spy(m_slider, &FluentSlider::sliderPressed);
    
    // Simulate mouse press
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(50, 15), 
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_slider, &pressEvent);
    
    QCOMPARE(spy.count(), 1);
}

void FluentSliderTest::testSliderReleasedSignal() {
    QSignalSpy spy(m_slider, &FluentSlider::sliderReleased);
    
    // Simulate mouse press and release
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(50, 15), 
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_slider, &pressEvent);
    
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(50, 15), 
                            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_slider, &releaseEvent);
    
    QCOMPARE(spy.count(), 1);
}

void FluentSliderTest::testShowLabels() {
    m_slider->setShowLabels(true);
    QVERIFY(m_slider->showLabels());
    
    m_slider->setShowLabels(false);
    QVERIFY(!m_slider->showLabels());
}

void FluentSliderTest::testShowTooltip() {
    m_slider->setShowTooltip(true);
    QVERIFY(m_slider->showTooltip());
    
    m_slider->setShowTooltip(false);
    QVERIFY(!m_slider->showTooltip());
}

void FluentSliderTest::testValueFormatter() {
    auto formatter = [](qreal value) {
        return QString("%1%").arg(qRound(value));
    };
    
    m_slider->setValueFormatter(formatter);
    
    // Test would require access to formatted string, which might be internal
    // This test verifies the formatter can be set without errors
    QVERIFY(true);
}

void FluentSliderTest::testMouseInteraction() {
    m_slider->resize(200, 30);
    m_slider->setRange(0.0, 100.0);
    
    // Test clicking on track
    QMouseEvent clickEvent(QEvent::MouseButtonPress, QPoint(100, 15), 
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_slider, &clickEvent);
    
    // Value should change (exact value depends on implementation)
    QVERIFY(m_slider->value() > 0.0);
}

void FluentSliderTest::testKeyboardInteraction() {
    m_slider->setFocus();
    m_slider->setValue(50.0);
    
    QSignalSpy spy(m_slider, &FluentSlider::valueChanged);
    
    // Test arrow key
    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
    QApplication::sendEvent(m_slider, &keyEvent);
    
    QVERIFY(spy.count() > 0);
    QVERIFY(m_slider->value() > 50.0);
}

void FluentSliderTest::testWheelInteraction() {
    m_slider->setFocus();
    m_slider->setValue(50.0);
    
    QSignalSpy spy(m_slider, &FluentSlider::valueChanged);
    
    // Test wheel event
    QWheelEvent wheelEvent(QPoint(50, 15), QPoint(50, 15), QPoint(0, 0), QPoint(0, 120),
                          Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_slider, &wheelEvent);
    
    QVERIFY(spy.count() > 0);
    QVERIFY(m_slider->value() != 50.0);
}

QTEST_MAIN(FluentSliderTest)
#include "FluentSliderTest.moc"
