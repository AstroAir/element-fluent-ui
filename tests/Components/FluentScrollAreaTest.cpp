// tests/Components/FluentScrollAreaTest.cpp
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentScrollArea.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentScrollAreaTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testWidgetConstructor();

    // Content management tests
    void testSetWidget();
    void testWidgetResizable();

    // Scroll bar policy tests
    void testHorizontalScrollBarPolicy();
    void testVerticalScrollBarPolicy();
    void testScrollBarPolicySignals();

    // Scroll behavior tests
    void testScrollBehavior();
    void testSmoothScrolling();
    void testScrollSpeed();
    void testElasticScrolling();

    // Scroll bar appearance tests
    void testShowScrollIndicators();
    void testAutoHideScrollBars();
    void testScrollBarWidth();
    void testScrollBarOpacity();

    // Scroll position tests
    void testScrollPosition();
    void testHorizontalScrollValue();
    void testVerticalScrollValue();
    void testScrollRange();

    // Programmatic scrolling tests
    void testScrollTo();
    void testScrollToWidget();
    void testScrollToRect();
    void testScrollBy();
    void testScrollByPixels();
    void testScrollByPages();

    // Convenience scrolling tests
    void testScrollToTop();
    void testScrollToBottom();
    void testScrollToLeft();
    void testScrollToRight();
    void testScrollToCenter();

    // Ensure visible tests
    void testEnsureVisible();
    void testEnsureWidgetVisible();
    void testCenterOn();
    void testCenterOnWidget();

    // Mouse interaction tests
    void testWheelScrolling();
    void testMouseDragScrolling();
    void testScrollBarInteraction();

    // Keyboard interaction tests
    void testKeyboardScrolling();
    void testKeyboardScrollingWithModifiers();
    void testPageUpDown();
    void testHomeEnd();

    // Animation tests
    void testScrollAnimation();
    void testAnimationDuration();
    void testAnimationEasing();

    // Auto-hide tests
    void testAutoHideOnLeave();
    void testAutoHideOnTimer();
    void testAutoHideOnHover();

    // Accessibility tests
    void testAccessibilityProperties();
    void testKeyboardNavigation();

    // Performance tests
    void testScrollPerformance();
    void testLargeContentScrolling();

    // Edge case tests
    void testEmptyContent();
    void testVeryLargeContent();
    void testNegativeScrollValues();

private:
    FluentScrollArea* createScrollArea();
    QWidget* createLargeContent();
    void simulateWheelEvent(QWidget* widget, const QPoint& delta);
    void simulateMouseDrag(QWidget* widget, const QPoint& start,
                           const QPoint& end);

private:
    FluentScrollArea* m_scrollArea{nullptr};
};

void FluentScrollAreaTest::initTestCase() {
    // Initialize theme for consistent testing
    auto& theme = FluentQt::Styling::FluentTheme::instance();
    theme.setMode(FluentQt::Styling::FluentThemeMode::Light);
}

void FluentScrollAreaTest::cleanupTestCase() {
    // Cleanup after all tests
}

void FluentScrollAreaTest::init() { m_scrollArea = createScrollArea(); }

void FluentScrollAreaTest::cleanup() {
    delete m_scrollArea;
    m_scrollArea = nullptr;
}

void FluentScrollAreaTest::testDefaultConstructor() {
    FluentScrollArea scrollArea;

    QCOMPARE(scrollArea.horizontalScrollBarPolicy(),
             FluentScrollBarPolicy::AsNeeded);
    QCOMPARE(scrollArea.verticalScrollBarPolicy(),
             FluentScrollBarPolicy::AsNeeded);
    QCOMPARE(scrollArea.scrollBehavior(), FluentScrollBehavior::Smooth);
    QVERIFY(scrollArea.smoothScrolling());
    QCOMPARE(scrollArea.scrollSpeed(), 100);
    QVERIFY(!scrollArea.elasticScrolling());
    QVERIFY(scrollArea.showScrollIndicators());
    QVERIFY(!scrollArea.autoHideScrollBars());
    QCOMPARE(scrollArea.scrollBarWidth(), 12);
    QCOMPARE(scrollArea.scrollBarOpacity(), 1.0);
}

void FluentScrollAreaTest::testWidgetConstructor() {
    QLabel* content = new QLabel("Test Content");
    FluentScrollArea scrollArea(
        content, nullptr);  // Explicitly use the widget constructor

    QCOMPARE(scrollArea.widget(), content);
    QCOMPARE(content->parent(), &scrollArea);
}

void FluentScrollAreaTest::testSetWidget() {
    QLabel* content1 = new QLabel("Content 1");
    QLabel* content2 = new QLabel("Content 2");

    m_scrollArea->setWidget(content1);
    QCOMPARE(m_scrollArea->widget(), content1);
    QCOMPARE(content1->parent(), m_scrollArea);

    m_scrollArea->setWidget(content2);
    QCOMPARE(m_scrollArea->widget(), content2);
    QCOMPARE(content2->parent(), m_scrollArea);

    delete content1;
    delete content2;
}

void FluentScrollAreaTest::testWidgetResizable() {
    m_scrollArea->setWidgetResizable(true);
    QVERIFY(m_scrollArea->widgetResizable());

    m_scrollArea->setWidgetResizable(false);
    QVERIFY(!m_scrollArea->widgetResizable());
}

void FluentScrollAreaTest::testHorizontalScrollBarPolicy() {
    QSignalSpy spy(m_scrollArea,
                   &FluentScrollArea::horizontalScrollBarPolicyChanged);

    m_scrollArea->setHorizontalScrollBarPolicy(
        FluentScrollBarPolicy::AlwaysOff);
    QCOMPARE(m_scrollArea->horizontalScrollBarPolicy(),
             FluentScrollBarPolicy::AlwaysOff);
    QCOMPARE(spy.count(), 1);

    m_scrollArea->setHorizontalScrollBarPolicy(FluentScrollBarPolicy::AlwaysOn);
    QCOMPARE(m_scrollArea->horizontalScrollBarPolicy(),
             FluentScrollBarPolicy::AlwaysOn);
    QCOMPARE(spy.count(), 2);
}

void FluentScrollAreaTest::testVerticalScrollBarPolicy() {
    QSignalSpy spy(m_scrollArea,
                   &FluentScrollArea::verticalScrollBarPolicyChanged);

    m_scrollArea->setVerticalScrollBarPolicy(FluentScrollBarPolicy::AutoHide);
    QCOMPARE(m_scrollArea->verticalScrollBarPolicy(),
             FluentScrollBarPolicy::AutoHide);
    QCOMPARE(spy.count(), 1);
}

void FluentScrollAreaTest::testScrollBarPolicySignals() {
    QSignalSpy hSpy(m_scrollArea,
                    &FluentScrollArea::horizontalScrollBarPolicyChanged);
    QSignalSpy vSpy(m_scrollArea,
                    &FluentScrollArea::verticalScrollBarPolicyChanged);

    // Setting same policy should not emit signal
    m_scrollArea->setHorizontalScrollBarPolicy(
        m_scrollArea->horizontalScrollBarPolicy());
    QCOMPARE(hSpy.count(), 0);

    m_scrollArea->setVerticalScrollBarPolicy(
        m_scrollArea->verticalScrollBarPolicy());
    QCOMPARE(vSpy.count(), 0);
}

void FluentScrollAreaTest::testScrollBehavior() {
    QSignalSpy spy(m_scrollArea, &FluentScrollArea::scrollBehaviorChanged);

    m_scrollArea->setScrollBehavior(FluentScrollBehavior::Kinetic);
    QCOMPARE(m_scrollArea->scrollBehavior(), FluentScrollBehavior::Kinetic);
    QCOMPARE(spy.count(), 1);

    m_scrollArea->setScrollBehavior(FluentScrollBehavior::Instant);
    QCOMPARE(m_scrollArea->scrollBehavior(), FluentScrollBehavior::Instant);
    QCOMPARE(spy.count(), 2);
}

void FluentScrollAreaTest::testSmoothScrolling() {
    m_scrollArea->setSmoothScrolling(false);
    QVERIFY(!m_scrollArea->smoothScrolling());

    m_scrollArea->setSmoothScrolling(true);
    QVERIFY(m_scrollArea->smoothScrolling());
}

void FluentScrollAreaTest::testScrollSpeed() {
    m_scrollArea->setScrollSpeed(200);
    QCOMPARE(m_scrollArea->scrollSpeed(), 200);

    // Invalid speed should be clamped
    m_scrollArea->setScrollSpeed(-10);
    QCOMPARE(m_scrollArea->scrollSpeed(), 1);
}

void FluentScrollAreaTest::testElasticScrolling() {
    m_scrollArea->setElasticScrolling(true);
    QVERIFY(m_scrollArea->elasticScrolling());

    m_scrollArea->setElasticScrolling(false);
    QVERIFY(!m_scrollArea->elasticScrolling());
}

void FluentScrollAreaTest::testShowScrollIndicators() {
    m_scrollArea->setShowScrollIndicators(false);
    QVERIFY(!m_scrollArea->showScrollIndicators());

    m_scrollArea->setShowScrollIndicators(true);
    QVERIFY(m_scrollArea->showScrollIndicators());
}

void FluentScrollAreaTest::testAutoHideScrollBars() {
    m_scrollArea->setAutoHideScrollBars(true);
    QVERIFY(m_scrollArea->autoHideScrollBars());

    m_scrollArea->setAutoHideScrollBars(false);
    QVERIFY(!m_scrollArea->autoHideScrollBars());
}

void FluentScrollAreaTest::testScrollBarWidth() {
    m_scrollArea->setScrollBarWidth(16);
    QCOMPARE(m_scrollArea->scrollBarWidth(), 16);

    // Invalid width should be ignored
    m_scrollArea->setScrollBarWidth(-5);
    QCOMPARE(m_scrollArea->scrollBarWidth(), 16);
}

void FluentScrollAreaTest::testScrollBarOpacity() {
    m_scrollArea->setScrollBarOpacity(0.5);
    QCOMPARE(m_scrollArea->scrollBarOpacity(), 0.5);

    // Values should be clamped to [0.0, 1.0]
    m_scrollArea->setScrollBarOpacity(1.5);
    QCOMPARE(m_scrollArea->scrollBarOpacity(), 1.0);

    m_scrollArea->setScrollBarOpacity(-0.5);
    QCOMPARE(m_scrollArea->scrollBarOpacity(), 0.0);
}

void FluentScrollAreaTest::testScrollPosition() {
    QWidget* content = createLargeContent();
    m_scrollArea->setWidget(content);
    m_scrollArea->resize(300, 200);

    QSignalSpy spy(m_scrollArea, &FluentScrollArea::scrollPositionChanged);

    m_scrollArea->setScrollPosition(QPoint(50, 100), false);
    QCOMPARE(m_scrollArea->scrollPosition(), QPoint(50, 100));
    QVERIFY(spy.count() > 0);
}

void FluentScrollAreaTest::testScrollRange() {
    QWidget* content = createLargeContent();
    m_scrollArea->setWidget(content);
    m_scrollArea->resize(300, 200);

    const QSize range = m_scrollArea->scrollRange();
    QVERIFY(range.width() >= 0);
    QVERIFY(range.height() >= 0);
}

FluentScrollArea* FluentScrollAreaTest::createScrollArea() {
    return new FluentScrollArea();
}

QWidget* FluentScrollAreaTest::createLargeContent() {
    QWidget* widget = new QWidget();
    widget->setMinimumSize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(widget);
    for (int i = 0; i < 20; ++i) {
        layout->addWidget(new QLabel(QString("Label %1").arg(i)));
    }

    return widget;
}

void FluentScrollAreaTest::simulateWheelEvent(QWidget* widget,
                                              const QPoint& delta) {
    QWheelEvent wheelEvent(QPointF(widget->width() / 2, widget->height() / 2),
                           QPointF(widget->width() / 2, widget->height() / 2),
                           QPoint(), delta * 8 * 15, Qt::NoButton,
                           Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(widget, &wheelEvent);
}

void FluentScrollAreaTest::simulateMouseDrag(QWidget* widget,
                                             const QPoint& start,
                                             const QPoint& end) {
    QMouseEvent pressEvent(QEvent::MouseButtonPress, start, Qt::LeftButton,
                           Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &pressEvent);

    QMouseEvent moveEvent(QEvent::MouseMove, end, Qt::LeftButton,
                          Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &moveEvent);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, end, Qt::LeftButton,
                             Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &releaseEvent);
}

QTEST_MAIN(FluentScrollAreaTest)
#include "FluentScrollAreaTest.moc"
