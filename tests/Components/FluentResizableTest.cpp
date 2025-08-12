// tests/Components/FluentResizableTest.cpp
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QTouchEvent>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentResizable.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentResizableTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testContentConstructor();

    // Content management tests
    void testSetContentWidget();
    void testContentWidgetProperty();

    // Resize direction tests
    void testResizeDirection();
    void testResizeDirectionSignal();

    // Handle configuration tests
    void testEnabledHandles();
    void testHandleSize();
    void testShowHandles();

    // Size constraint tests
    void testMinimumSize();
    void testMaximumSize();
    void testSizeConstraints();

    // Animation and behavior tests
    void testSmoothResize();
    void testTouchEnabled();
    void testSnapToGrid();
    void testAspectRatio();

    // Programmatic resize tests
    void testResizeToSize();
    void testResizeToWidth();
    void testResizeToHeight();

    // Slot tests
    void testResetToMinimumSize();
    void testResetToMaximumSize();
    void testToggleHandleVisibility();

    // Mouse interaction tests
    void testMouseResize();
    void testMouseResizeConstraints();
    void testMouseResizeCancel();

    // Keyboard interaction tests
    void testKeyboardResize();
    void testKeyboardResizeWithModifiers();
    void testEscapeKey();

    // Touch interaction tests
    void testTouchResize();
    void testTouchResizeMultipoint();

    // Grid snapping tests
    void testGridSnapping();
    void testGridSnappingDisabled();

    // Aspect ratio tests
    void testAspectRatioMaintenance();
    void testAspectRatioWithDifferentHandles();

    // Animation tests
    void testResizeAnimation();
    void testAnimationDuration();
    void testAnimationEasing();

    // Accessibility tests
    void testAccessibilityProperties();
    void testKeyboardNavigation();

    // Performance tests
    void testResizePerformance();
    void testLargeContentPerformance();

    // Edge case tests
    void testZeroSize();
    void testNegativeSize();
    void testExtremelyLargeSize();

private:
    FluentResizable* createResizable();
    QWidget* createTestContent();
    void simulateMouseDrag(QWidget* widget, const QPoint& start,
                           const QPoint& end);
    void simulateKeyPress(QWidget* widget, Qt::Key key,
                          Qt::KeyboardModifiers modifiers = Qt::NoModifier);

private:
    FluentResizable* m_resizable{nullptr};
};

void FluentResizableTest::initTestCase() {
    // Initialize theme for consistent testing
    auto& theme = FluentQt::Styling::FluentTheme::instance();
    theme.setMode(FluentQt::Styling::FluentThemeMode::Light);
}

void FluentResizableTest::cleanupTestCase() {
    // Cleanup after all tests
}

void FluentResizableTest::init() { m_resizable = createResizable(); }

void FluentResizableTest::cleanup() {
    delete m_resizable;
    m_resizable = nullptr;
}

void FluentResizableTest::testDefaultConstructor() {
    FluentResizable resizable;

    QCOMPARE(resizable.resizeDirection(), FluentResizeDirection::Both);
    QCOMPARE(resizable.resizeMode(), FluentResizeMode::Immediate);
    QCOMPARE(resizable.handleSize(), 8);
    QVERIFY(resizable.showHandles());
    QVERIFY(resizable.smoothResize());
    QVERIFY(resizable.isTouchEnabled());
    QCOMPARE(resizable.minimumSize(), QSize(50, 50));
    QVERIFY(!resizable.snapToGrid());
    QCOMPARE(resizable.gridSize(), 10);
    QCOMPARE(resizable.aspectRatio(), 0.0);
    QVERIFY(!resizable.maintainAspectRatio());
}

void FluentResizableTest::testContentConstructor() {
    QLabel* content = new QLabel("Test Content");
    FluentResizable resizable(content);

    QCOMPARE(resizable.contentWidget(), content);
    QCOMPARE(content->parent(), &resizable);
}

void FluentResizableTest::testSetContentWidget() {
    QLabel* content1 = new QLabel("Content 1");
    QLabel* content2 = new QLabel("Content 2");

    m_resizable->setContentWidget(content1);
    QCOMPARE(m_resizable->contentWidget(), content1);
    QCOMPARE(content1->parent(), m_resizable);

    m_resizable->setContentWidget(content2);
    QCOMPARE(m_resizable->contentWidget(), content2);
    QCOMPARE(content2->parent(), m_resizable);
    QVERIFY(content1->parent() != m_resizable);

    delete content1;
    delete content2;
}

void FluentResizableTest::testContentWidgetProperty() {
    QLabel* content = new QLabel("Test");

    QSignalSpy spy(m_resizable, &FluentResizable::resizeFinished);
    m_resizable->setContentWidget(content);

    QCOMPARE(m_resizable->contentWidget(), content);
}

void FluentResizableTest::testResizeDirection() {
    QCOMPARE(m_resizable->resizeDirection(), FluentResizeDirection::Both);

    m_resizable->setResizeDirection(FluentResizeDirection::Horizontal);
    QCOMPARE(m_resizable->resizeDirection(), FluentResizeDirection::Horizontal);

    m_resizable->setResizeDirection(FluentResizeDirection::Vertical);
    QCOMPARE(m_resizable->resizeDirection(), FluentResizeDirection::Vertical);

    m_resizable->setResizeDirection(FluentResizeDirection::None);
    QCOMPARE(m_resizable->resizeDirection(), FluentResizeDirection::None);
}

void FluentResizableTest::testResizeDirectionSignal() {
    QSignalSpy spy(m_resizable, &FluentResizable::resizeDirectionChanged);

    m_resizable->setResizeDirection(FluentResizeDirection::Horizontal);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().first().value<FluentResizeDirection>(),
             FluentResizeDirection::Horizontal);

    // Setting same direction should not emit signal
    m_resizable->setResizeDirection(FluentResizeDirection::Horizontal);
    QCOMPARE(spy.count(), 1);
}

void FluentResizableTest::testEnabledHandles() {
    FluentResizeHandles handles =
        FluentResizeHandle::Top | FluentResizeHandle::Bottom;

    QSignalSpy spy(m_resizable, &FluentResizable::enabledHandlesChanged);
    m_resizable->setEnabledHandles(handles);

    QCOMPARE(m_resizable->enabledHandles(), handles);
    QCOMPARE(spy.count(), 1);
}

void FluentResizableTest::testHandleSize() {
    QSignalSpy spy(m_resizable, &FluentResizable::handleSizeChanged);

    m_resizable->setHandleSize(12);
    QCOMPARE(m_resizable->handleSize(), 12);
    QCOMPARE(spy.count(), 1);

    // Invalid size should be ignored
    m_resizable->setHandleSize(-5);
    QCOMPARE(m_resizable->handleSize(), 12);
    QCOMPARE(spy.count(), 1);
}

void FluentResizableTest::testShowHandles() {
    QSignalSpy spy(m_resizable, &FluentResizable::showHandlesChanged);

    m_resizable->setShowHandles(false);
    QVERIFY(!m_resizable->showHandles());
    QCOMPARE(spy.count(), 1);

    m_resizable->setShowHandles(true);
    QVERIFY(m_resizable->showHandles());
    QCOMPARE(spy.count(), 2);
}

void FluentResizableTest::testMinimumSize() {
    QSize minSize(100, 80);
    QSignalSpy spy(m_resizable, &FluentResizable::minimumSizeChanged);

    m_resizable->setMinimumSize(minSize);
    QCOMPARE(m_resizable->minimumSize(), minSize);
    QCOMPARE(spy.count(), 1);
}

void FluentResizableTest::testMaximumSize() {
    QSize maxSize(800, 600);
    QSignalSpy spy(m_resizable, &FluentResizable::maximumSizeChanged);

    m_resizable->setMaximumSize(maxSize);
    QCOMPARE(m_resizable->maximumSize(), maxSize);
    QCOMPARE(spy.count(), 1);
}

void FluentResizableTest::testSizeConstraints() {
    m_resizable->setMinimumSize(QSize(100, 100));
    m_resizable->setMaximumSize(QSize(500, 400));

    // Test resize within constraints
    m_resizable->resizeToSize(QSize(300, 250), false);
    QCOMPARE(m_resizable->size(), QSize(300, 250));

    // Test resize below minimum
    m_resizable->resizeToSize(QSize(50, 50), false);
    QCOMPARE(m_resizable->size(), QSize(100, 100));

    // Test resize above maximum
    m_resizable->resizeToSize(QSize(800, 600), false);
    QCOMPARE(m_resizable->size(), QSize(500, 400));
}

void FluentResizableTest::testSmoothResize() {
    m_resizable->setSmoothResize(false);
    QVERIFY(!m_resizable->smoothResize());

    m_resizable->setSmoothResize(true);
    QVERIFY(m_resizable->smoothResize());
}

void FluentResizableTest::testTouchEnabled() {
    m_resizable->setTouchEnabled(false);
    QVERIFY(!m_resizable->isTouchEnabled());

    m_resizable->setTouchEnabled(true);
    QVERIFY(m_resizable->isTouchEnabled());
}

void FluentResizableTest::testSnapToGrid() {
    m_resizable->setSnapToGrid(true);
    QVERIFY(m_resizable->snapToGrid());

    m_resizable->setSnapToGrid(false);
    QVERIFY(!m_resizable->snapToGrid());
}

void FluentResizableTest::testAspectRatio() {
    m_resizable->setAspectRatio(1.5);
    QCOMPARE(m_resizable->aspectRatio(), 1.5);

    m_resizable->setMaintainAspectRatio(true);
    QVERIFY(m_resizable->maintainAspectRatio());
}

FluentResizable* FluentResizableTest::createResizable() {
    return new FluentResizable();
}

QWidget* FluentResizableTest::createTestContent() {
    QLabel* label = new QLabel("Test Content");
    label->setMinimumSize(100, 100);
    return label;
}

void FluentResizableTest::simulateMouseDrag(QWidget* widget,
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

void FluentResizableTest::simulateKeyPress(QWidget* widget, Qt::Key key,
                                           Qt::KeyboardModifiers modifiers) {
    QKeyEvent keyEvent(QEvent::KeyPress, key, modifiers);
    QApplication::sendEvent(widget, &keyEvent);
}

QTEST_MAIN(FluentResizableTest)
#include "FluentResizableTest.moc"
