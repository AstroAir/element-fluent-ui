// tests/Components/FluentEnhancedScrollBarTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentEnhancedScrollBar.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentEnhancedScrollBarTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testOrientationConstructor();

    // Value tests
    void testValue();
    void testMinimum();
    void testMaximum();
    void testRange();

    // Page and step tests
    void testPageStep();
    void testSingleStep();

    // Orientation tests
    void testOrientation();

    // Enhanced features tests
    void testAutoHide();
    void testSmoothScrolling();
    void testScrollBarWidth();
    void testHoverExpansion();

    // Interaction tests
    void testMouseInteraction();
    void testWheelEvent();
    void testKeyboardNavigation();

    // Animation tests
    void testScrollAnimation();
    void testHoverAnimation();

    // Signal tests
    void testSignalEmission();

    // Style tests
    void testScrollBarStyle();

private:
    FluentEnhancedScrollBar* m_scrollBar{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentEnhancedScrollBarTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentEnhancedScrollBarTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentEnhancedScrollBarTest::init() {
    m_scrollBar = new FluentEnhancedScrollBar(m_testWidget);
}

void FluentEnhancedScrollBarTest::cleanup() {
    delete m_scrollBar;
    m_scrollBar = nullptr;
}

void FluentEnhancedScrollBarTest::testDefaultConstructor() {
    QVERIFY(m_scrollBar != nullptr);
    QCOMPARE(m_scrollBar->orientation(), Qt::Vertical);
    QCOMPARE(m_scrollBar->value(), 0);
    QCOMPARE(m_scrollBar->minimum(), 0);
    QCOMPARE(m_scrollBar->maximum(), 99);
    QVERIFY(m_scrollBar->autoHide());
    QVERIFY(m_scrollBar->smoothScrolling());
}

void FluentEnhancedScrollBarTest::testOrientationConstructor() {
    auto* horizontalScrollBar = new FluentEnhancedScrollBar(Qt::Horizontal, m_testWidget);
    QCOMPARE(horizontalScrollBar->orientation(), Qt::Horizontal);
    delete horizontalScrollBar;
    
    auto* verticalScrollBar = new FluentEnhancedScrollBar(Qt::Vertical, m_testWidget);
    QCOMPARE(verticalScrollBar->orientation(), Qt::Vertical);
    delete verticalScrollBar;
}

void FluentEnhancedScrollBarTest::testValue() {
    QSignalSpy valueSpy(m_scrollBar, &FluentEnhancedScrollBar::valueChanged);
    
    m_scrollBar->setValue(50);
    QCOMPARE(m_scrollBar->value(), 50);
    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(valueSpy.first().first().toInt(), 50);
    
    // Test bounds
    m_scrollBar->setValue(150); // Above maximum
    QCOMPARE(m_scrollBar->value(), 99); // Should clamp to maximum
    
    m_scrollBar->setValue(-10); // Below minimum
    QCOMPARE(m_scrollBar->value(), 0); // Should clamp to minimum
}

void FluentEnhancedScrollBarTest::testMinimum() {
    QSignalSpy rangeSpy(m_scrollBar, &FluentEnhancedScrollBar::rangeChanged);
    
    m_scrollBar->setMinimum(10);
    QCOMPARE(m_scrollBar->minimum(), 10);
    QCOMPARE(rangeSpy.count(), 1);
    
    // Test that value adjusts if below new minimum
    m_scrollBar->setValue(5);
    QCOMPARE(m_scrollBar->value(), 10); // Should adjust to minimum
}

void FluentEnhancedScrollBarTest::testMaximum() {
    QSignalSpy rangeSpy(m_scrollBar, &FluentEnhancedScrollBar::rangeChanged);
    
    m_scrollBar->setMaximum(200);
    QCOMPARE(m_scrollBar->maximum(), 200);
    QCOMPARE(rangeSpy.count(), 1);
    
    // Test that value adjusts if above new maximum
    m_scrollBar->setValue(250);
    QCOMPARE(m_scrollBar->value(), 200); // Should adjust to maximum
}

void FluentEnhancedScrollBarTest::testRange() {
    QSignalSpy rangeSpy(m_scrollBar, &FluentEnhancedScrollBar::rangeChanged);
    
    m_scrollBar->setRange(20, 180);
    QCOMPARE(m_scrollBar->minimum(), 20);
    QCOMPARE(m_scrollBar->maximum(), 180);
    QCOMPARE(rangeSpy.count(), 1);
    
    // Test invalid range (min > max)
    m_scrollBar->setRange(100, 50);
    QCOMPARE(m_scrollBar->minimum(), 50); // Should swap values
    QCOMPARE(m_scrollBar->maximum(), 100);
}

void FluentEnhancedScrollBarTest::testPageStep() {
    QSignalSpy pageStepSpy(m_scrollBar, &FluentEnhancedScrollBar::pageStepChanged);
    
    m_scrollBar->setPageStep(20);
    QCOMPARE(m_scrollBar->pageStep(), 20);
    QCOMPARE(pageStepSpy.count(), 1);
    
    // Test invalid page step
    m_scrollBar->setPageStep(-5);
    QVERIFY(m_scrollBar->pageStep() >= 0); // Should remain non-negative
}

void FluentEnhancedScrollBarTest::testSingleStep() {
    QSignalSpy singleStepSpy(m_scrollBar, &FluentEnhancedScrollBar::singleStepChanged);
    
    m_scrollBar->setSingleStep(5);
    QCOMPARE(m_scrollBar->singleStep(), 5);
    QCOMPARE(singleStepSpy.count(), 1);
    
    // Test invalid single step
    m_scrollBar->setSingleStep(-2);
    QVERIFY(m_scrollBar->singleStep() >= 0); // Should remain non-negative
}

void FluentEnhancedScrollBarTest::testOrientation() {
    QSignalSpy orientationSpy(m_scrollBar, &FluentEnhancedScrollBar::orientationChanged);
    
    m_scrollBar->setOrientation(Qt::Horizontal);
    QCOMPARE(m_scrollBar->orientation(), Qt::Horizontal);
    QCOMPARE(orientationSpy.count(), 1);
    
    m_scrollBar->setOrientation(Qt::Vertical);
    QCOMPARE(m_scrollBar->orientation(), Qt::Vertical);
    QCOMPARE(orientationSpy.count(), 2);
}

void FluentEnhancedScrollBarTest::testAutoHide() {
    QSignalSpy autoHideSpy(m_scrollBar, &FluentEnhancedScrollBar::autoHideChanged);
    
    m_scrollBar->setAutoHide(false);
    QVERIFY(!m_scrollBar->autoHide());
    QCOMPARE(autoHideSpy.count(), 1);
    
    m_scrollBar->setAutoHide(true);
    QVERIFY(m_scrollBar->autoHide());
    QCOMPARE(autoHideSpy.count(), 2);
}

void FluentEnhancedScrollBarTest::testSmoothScrolling() {
    QSignalSpy smoothScrollingSpy(m_scrollBar, &FluentEnhancedScrollBar::smoothScrollingChanged);
    
    m_scrollBar->setSmoothScrolling(false);
    QVERIFY(!m_scrollBar->smoothScrolling());
    QCOMPARE(smoothScrollingSpy.count(), 1);
    
    m_scrollBar->setSmoothScrolling(true);
    QVERIFY(m_scrollBar->smoothScrolling());
    QCOMPARE(smoothScrollingSpy.count(), 2);
}

void FluentEnhancedScrollBarTest::testScrollBarWidth() {
    QSignalSpy widthSpy(m_scrollBar, &FluentEnhancedScrollBar::scrollBarWidthChanged);
    
    m_scrollBar->setScrollBarWidth(15);
    QCOMPARE(m_scrollBar->scrollBarWidth(), 15);
    QCOMPARE(widthSpy.count(), 1);
    
    // Test minimum width constraint
    m_scrollBar->setScrollBarWidth(2);
    QVERIFY(m_scrollBar->scrollBarWidth() >= 5); // Should have minimum width
}

void FluentEnhancedScrollBarTest::testHoverExpansion() {
    QSignalSpy hoverExpansionSpy(m_scrollBar, &FluentEnhancedScrollBar::hoverExpansionChanged);
    
    m_scrollBar->setHoverExpansion(false);
    QVERIFY(!m_scrollBar->hoverExpansion());
    QCOMPARE(hoverExpansionSpy.count(), 1);
    
    m_scrollBar->setHoverExpansion(true);
    QVERIFY(m_scrollBar->hoverExpansion());
    QCOMPARE(hoverExpansionSpy.count(), 2);
}

void FluentEnhancedScrollBarTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_scrollBar->setRange(0, 100);
    m_scrollBar->setValue(0);
    
    QSignalSpy valueSpy(m_scrollBar, &FluentEnhancedScrollBar::valueChanged);
    QSignalSpy sliderPressedSpy(m_scrollBar, &FluentEnhancedScrollBar::sliderPressed);
    QSignalSpy sliderReleasedSpy(m_scrollBar, &FluentEnhancedScrollBar::sliderReleased);
    
    // Test mouse press on slider
    QTest::mousePress(m_scrollBar, Qt::LeftButton, Qt::NoModifier, QPoint(10, 50));
    QCOMPARE(sliderPressedSpy.count(), 1);
    
    // Test mouse move (drag)
    QTest::mouseMove(m_scrollBar, QPoint(10, 60));
    
    // Test mouse release
    QTest::mouseRelease(m_scrollBar, Qt::LeftButton, Qt::NoModifier, QPoint(10, 60));
    QCOMPARE(sliderReleasedSpy.count(), 1);
    
    QTest::qWait(100);
}

void FluentEnhancedScrollBarTest::testWheelEvent() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_scrollBar->setRange(0, 100);
    m_scrollBar->setValue(50);
    
    QSignalSpy valueSpy(m_scrollBar, &FluentEnhancedScrollBar::valueChanged);
    
    // Test wheel scroll up
    QWheelEvent wheelUpEvent(QPointF(10, 10), QPointF(10, 10), 
                            QPoint(0, 0), QPoint(0, 120), 
                            Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_scrollBar, &wheelUpEvent);
    
    // Test wheel scroll down
    QWheelEvent wheelDownEvent(QPointF(10, 10), QPointF(10, 10), 
                              QPoint(0, 0), QPoint(0, -120), 
                              Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_scrollBar, &wheelDownEvent);
    
    QTest::qWait(100);
}

void FluentEnhancedScrollBarTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_scrollBar->setFocus();
    
    m_scrollBar->setRange(0, 100);
    m_scrollBar->setValue(50);
    
    QSignalSpy valueSpy(m_scrollBar, &FluentEnhancedScrollBar::valueChanged);
    
    // Test arrow keys
    QTest::keyClick(m_scrollBar, Qt::Key_Down);
    QTest::keyClick(m_scrollBar, Qt::Key_Up);
    QTest::keyClick(m_scrollBar, Qt::Key_Right);
    QTest::keyClick(m_scrollBar, Qt::Key_Left);
    
    // Test Page Up/Down
    QTest::keyClick(m_scrollBar, Qt::Key_PageDown);
    QTest::keyClick(m_scrollBar, Qt::Key_PageUp);
    
    // Test Home/End
    QTest::keyClick(m_scrollBar, Qt::Key_Home);
    QCOMPARE(m_scrollBar->value(), 0);
    
    QTest::keyClick(m_scrollBar, Qt::Key_End);
    QCOMPARE(m_scrollBar->value(), 100);
    
    QVERIFY(valueSpy.count() >= 2); // At least Home and End should trigger
}

void FluentEnhancedScrollBarTest::testScrollAnimation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_scrollBar->setRange(0, 100);
    m_scrollBar->setValue(0);
    m_scrollBar->setSmoothScrolling(true);
    
    QSignalSpy animationStartedSpy(m_scrollBar, &FluentEnhancedScrollBar::animationStarted);
    QSignalSpy animationFinishedSpy(m_scrollBar, &FluentEnhancedScrollBar::animationFinished);
    
    // Test animated scroll
    m_scrollBar->animateToValue(80);
    QCOMPARE(animationStartedSpy.count(), 1);
    
    // Wait for animation to complete
    QTest::qWait(500);
    QCOMPARE(m_scrollBar->value(), 80);
    QCOMPARE(animationFinishedSpy.count(), 1);
    
    // Test immediate scroll (no animation)
    m_scrollBar->setSmoothScrolling(false);
    m_scrollBar->setValue(20);
    QCOMPARE(m_scrollBar->value(), 20); // Should be immediate
}

void FluentEnhancedScrollBarTest::testHoverAnimation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_scrollBar->setHoverExpansion(true);
    
    QSignalSpy hoverEnteredSpy(m_scrollBar, &FluentEnhancedScrollBar::hoverEntered);
    QSignalSpy hoverLeftSpy(m_scrollBar, &FluentEnhancedScrollBar::hoverLeft);
    
    // Test hover enter
    QTest::mouseMove(m_scrollBar, QPoint(10, 10));
    QTest::qWait(100);
    
    // Test hover leave
    QTest::mouseMove(m_testWidget, QPoint(200, 200));
    QTest::qWait(100);
    
    // Hover events might not be triggered in test environment
    // but we can test that the methods don't crash
}

void FluentEnhancedScrollBarTest::testSignalEmission() {
    QSignalSpy valueSpy(m_scrollBar, &FluentEnhancedScrollBar::valueChanged);
    QSignalSpy rangeSpy(m_scrollBar, &FluentEnhancedScrollBar::rangeChanged);
    QSignalSpy pageStepSpy(m_scrollBar, &FluentEnhancedScrollBar::pageStepChanged);
    QSignalSpy singleStepSpy(m_scrollBar, &FluentEnhancedScrollBar::singleStepChanged);
    QSignalSpy orientationSpy(m_scrollBar, &FluentEnhancedScrollBar::orientationChanged);
    QSignalSpy autoHideSpy(m_scrollBar, &FluentEnhancedScrollBar::autoHideChanged);
    QSignalSpy smoothScrollingSpy(m_scrollBar, &FluentEnhancedScrollBar::smoothScrollingChanged);
    QSignalSpy widthSpy(m_scrollBar, &FluentEnhancedScrollBar::scrollBarWidthChanged);
    QSignalSpy hoverExpansionSpy(m_scrollBar, &FluentEnhancedScrollBar::hoverExpansionChanged);
    
    // Test all signals
    m_scrollBar->setValue(25);
    m_scrollBar->setRange(10, 90);
    m_scrollBar->setPageStep(15);
    m_scrollBar->setSingleStep(3);
    m_scrollBar->setOrientation(Qt::Horizontal);
    m_scrollBar->setAutoHide(false);
    m_scrollBar->setSmoothScrolling(false);
    m_scrollBar->setScrollBarWidth(12);
    m_scrollBar->setHoverExpansion(false);
    
    QCOMPARE(valueSpy.count(), 1);
    QCOMPARE(rangeSpy.count(), 1);
    QCOMPARE(pageStepSpy.count(), 1);
    QCOMPARE(singleStepSpy.count(), 1);
    QCOMPARE(orientationSpy.count(), 1);
    QCOMPARE(autoHideSpy.count(), 1);
    QCOMPARE(smoothScrollingSpy.count(), 1);
    QCOMPARE(widthSpy.count(), 1);
    QCOMPARE(hoverExpansionSpy.count(), 1);
}

void FluentEnhancedScrollBarTest::testScrollBarStyle() {
    QSignalSpy styleSpy(m_scrollBar, &FluentEnhancedScrollBar::scrollBarStyleChanged);
    
    m_scrollBar->setScrollBarStyle(FluentScrollBarStyle::Modern);
    QCOMPARE(m_scrollBar->scrollBarStyle(), FluentScrollBarStyle::Modern);
    QCOMPARE(styleSpy.count(), 1);
    
    m_scrollBar->setScrollBarStyle(FluentScrollBarStyle::Classic);
    QCOMPARE(m_scrollBar->scrollBarStyle(), FluentScrollBarStyle::Classic);
    QCOMPARE(styleSpy.count(), 2);
    
    m_scrollBar->setScrollBarStyle(FluentScrollBarStyle::Minimal);
    QCOMPARE(m_scrollBar->scrollBarStyle(), FluentScrollBarStyle::Minimal);
    QCOMPARE(styleSpy.count(), 3);
}

QTEST_MAIN(FluentEnhancedScrollBarTest)
#include "FluentEnhancedScrollBarTest.moc"
