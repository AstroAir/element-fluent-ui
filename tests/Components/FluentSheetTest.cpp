// tests/Components/FluentSheetTest.cpp
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentSheet.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentSheetTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testDirectionConstructor();
    void testDirectionSizeConstructor();

    // Factory method tests
    void testCreateBottomSheet();
    void testCreateTopSheet();
    void testCreateLeftSheet();
    void testCreateRightSheet();

    // Direction and size tests
    void testDirection();
    void testSheetSize();
    void testCustomSize();
    void testDirectionSignal();
    void testSheetSizeSignal();

    // Behavior tests
    void testBehavior();
    void testBehaviorSignal();

    // Overlay tests
    void testOverlayVisible();
    void testOverlayOpacity();

    // Animation tests
    void testAnimationDuration();
    void testEasingCurve();

    // Interaction tests
    void testCloseOnOverlayClick();
    void testCloseOnEscape();
    void testDragToClose();
    void testAutoResize();

    // Content management tests
    void testSetContentWidget();
    void testTitle();
    void testSubtitle();

    // State tests
    void testIsOpen();
    void testIsAnimating();

    // Open/close tests
    void testOpen();
    void testClose();
    void testToggle();
    void testOpenWithAnimation();
    void testCloseWithAnimation();

    // Signal tests
    void testAboutToOpenSignal();
    void testOpenedSignal();
    void testAboutToCloseSignal();
    void testClosedSignal();
    void testOverlayClickedSignal();

    // Mouse interaction tests
    void testMouseDragToClose();
    void testMouseDragThreshold();
    void testMouseDragCancel();

    // Keyboard interaction tests
    void testEscapeKeyClose();
    void testKeyboardNavigation();

    // Focus management tests
    void testFocusCapture();
    void testFocusRestore();
    void testFocusChain();

    // Geometry tests
    void testSheetGeometry();
    void testOverlayGeometry();
    void testParentResize();

    // Animation tests
    void testOpenAnimation();
    void testCloseAnimation();
    void testAnimationInterruption();

    // Accessibility tests
    void testAccessibilityProperties();
    void testAccessibilityFocus();

    // Performance tests
    void testOpenClosePerformance();
    void testAnimationPerformance();

    // Edge case tests
    void testNoParent();
    void testMultipleSheets();
    void testRapidOpenClose();

private:
    FluentSheet* createSheet(
        FluentSheetDirection direction = FluentSheetDirection::Bottom);
    QMainWindow* createMainWindow();
    void simulateMouseDrag(QWidget* widget, const QPoint& start,
                           const QPoint& end);
    void simulateKeyPress(QWidget* widget, Qt::Key key);
    void waitForAnimation(FluentSheet* sheet);

private:
    FluentSheet* m_sheet{nullptr};
    QMainWindow* m_mainWindow{nullptr};
};

void FluentSheetTest::initTestCase() {
    // Initialize theme for consistent testing
    auto& theme = FluentQt::Styling::FluentTheme::instance();
    theme.setMode(FluentQt::Styling::FluentThemeMode::Light);
}

void FluentSheetTest::cleanupTestCase() {
    // Cleanup after all tests
}

void FluentSheetTest::init() {
    m_mainWindow = createMainWindow();
    m_sheet = createSheet();
}

void FluentSheetTest::cleanup() {
    delete m_sheet;
    delete m_mainWindow;
    m_sheet = nullptr;
    m_mainWindow = nullptr;
}

void FluentSheetTest::testDefaultConstructor() {
    FluentSheet sheet;

    QCOMPARE(sheet.direction(), FluentSheetDirection::Bottom);
    QCOMPARE(sheet.sheetSize(), FluentSheetSize::Medium);
    QCOMPARE(sheet.behavior(), FluentSheetBehavior::Modal);
    QVERIFY(sheet.overlayVisible());
    QCOMPARE(sheet.overlayOpacity(), 0.5);
    QCOMPARE(sheet.animationDuration(), 300);
    QCOMPARE(sheet.easingCurve(), QEasingCurve::OutCubic);
    QVERIFY(sheet.closeOnOverlayClick());
    QVERIFY(sheet.closeOnEscape());
    QVERIFY(sheet.dragToClose());
    QVERIFY(sheet.autoResize());
    QVERIFY(!sheet.isOpen());
}

void FluentSheetTest::testDirectionConstructor() {
    FluentSheet sheet(FluentSheetDirection::Top);
    QCOMPARE(sheet.direction(), FluentSheetDirection::Top);
}

void FluentSheetTest::testDirectionSizeConstructor() {
    FluentSheet sheet(FluentSheetDirection::Left, FluentSheetSize::Large);
    QCOMPARE(sheet.direction(), FluentSheetDirection::Left);
    QCOMPARE(sheet.sheetSize(), FluentSheetSize::Large);
}

void FluentSheetTest::testCreateBottomSheet() {
    FluentSheet* sheet = FluentSheet::createBottomSheet();
    QCOMPARE(sheet->direction(), FluentSheetDirection::Bottom);
    delete sheet;
}

void FluentSheetTest::testCreateTopSheet() {
    FluentSheet* sheet = FluentSheet::createTopSheet();
    QCOMPARE(sheet->direction(), FluentSheetDirection::Top);
    delete sheet;
}

void FluentSheetTest::testCreateLeftSheet() {
    FluentSheet* sheet = FluentSheet::createLeftSheet();
    QCOMPARE(sheet->direction(), FluentSheetDirection::Left);
    delete sheet;
}

void FluentSheetTest::testCreateRightSheet() {
    FluentSheet* sheet = FluentSheet::createRightSheet();
    QCOMPARE(sheet->direction(), FluentSheetDirection::Right);
    delete sheet;
}

void FluentSheetTest::testDirection() {
    QCOMPARE(m_sheet->direction(), FluentSheetDirection::Bottom);

    m_sheet->setDirection(FluentSheetDirection::Top);
    QCOMPARE(m_sheet->direction(), FluentSheetDirection::Top);

    m_sheet->setDirection(FluentSheetDirection::Left);
    QCOMPARE(m_sheet->direction(), FluentSheetDirection::Left);

    m_sheet->setDirection(FluentSheetDirection::Right);
    QCOMPARE(m_sheet->direction(), FluentSheetDirection::Right);
}

void FluentSheetTest::testSheetSize() {
    QCOMPARE(m_sheet->sheetSize(), FluentSheetSize::Medium);

    m_sheet->setSheetSize(FluentSheetSize::Small);
    QCOMPARE(m_sheet->sheetSize(), FluentSheetSize::Small);

    m_sheet->setSheetSize(FluentSheetSize::Large);
    QCOMPARE(m_sheet->sheetSize(), FluentSheetSize::Large);

    m_sheet->setSheetSize(FluentSheetSize::Full);
    QCOMPARE(m_sheet->sheetSize(), FluentSheetSize::Full);

    m_sheet->setSheetSize(FluentSheetSize::Custom);
    QCOMPARE(m_sheet->sheetSize(), FluentSheetSize::Custom);
}

void FluentSheetTest::testCustomSize() {
    QSize customSize(400, 300);
    m_sheet->setCustomSize(customSize);
    QCOMPARE(m_sheet->customSize(), customSize);
}

void FluentSheetTest::testDirectionSignal() {
    QSignalSpy spy(m_sheet, &FluentSheet::directionChanged);

    m_sheet->setDirection(FluentSheetDirection::Top);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().first().value<FluentSheetDirection>(),
             FluentSheetDirection::Top);

    // Setting same direction should not emit signal
    m_sheet->setDirection(FluentSheetDirection::Top);
    QCOMPARE(spy.count(), 1);
}

void FluentSheetTest::testSheetSizeSignal() {
    QSignalSpy spy(m_sheet, &FluentSheet::sheetSizeChanged);

    m_sheet->setSheetSize(FluentSheetSize::Large);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().first().value<FluentSheetSize>(),
             FluentSheetSize::Large);
}

void FluentSheetTest::testBehavior() {
    QCOMPARE(m_sheet->behavior(), FluentSheetBehavior::Modal);

    m_sheet->setBehavior(FluentSheetBehavior::Modeless);
    QCOMPARE(m_sheet->behavior(), FluentSheetBehavior::Modeless);

    m_sheet->setBehavior(FluentSheetBehavior::Persistent);
    QCOMPARE(m_sheet->behavior(), FluentSheetBehavior::Persistent);
}

void FluentSheetTest::testBehaviorSignal() {
    QSignalSpy spy(m_sheet, &FluentSheet::behaviorChanged);

    m_sheet->setBehavior(FluentSheetBehavior::Modeless);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().first().value<FluentSheetBehavior>(),
             FluentSheetBehavior::Modeless);
}

void FluentSheetTest::testOverlayVisible() {
    QVERIFY(m_sheet->overlayVisible());

    m_sheet->setOverlayVisible(false);
    QVERIFY(!m_sheet->overlayVisible());

    m_sheet->setOverlayVisible(true);
    QVERIFY(m_sheet->overlayVisible());
}

void FluentSheetTest::testOverlayOpacity() {
    QCOMPARE(m_sheet->overlayOpacity(), 0.5);

    m_sheet->setOverlayOpacity(0.8);
    QCOMPARE(m_sheet->overlayOpacity(), 0.8);

    // Values should be clamped to [0.0, 1.0]
    m_sheet->setOverlayOpacity(1.5);
    QCOMPARE(m_sheet->overlayOpacity(), 1.0);

    m_sheet->setOverlayOpacity(-0.5);
    QCOMPARE(m_sheet->overlayOpacity(), 0.0);
}

void FluentSheetTest::testAnimationDuration() {
    QCOMPARE(m_sheet->animationDuration(), 300);

    m_sheet->setAnimationDuration(500);
    QCOMPARE(m_sheet->animationDuration(), 500);

    // Negative duration should be ignored
    m_sheet->setAnimationDuration(-100);
    QCOMPARE(m_sheet->animationDuration(), 500);
}

void FluentSheetTest::testEasingCurve() {
    QCOMPARE(m_sheet->easingCurve(), QEasingCurve::OutCubic);

    m_sheet->setEasingCurve(QEasingCurve::InOutQuad);
    QCOMPARE(m_sheet->easingCurve(), QEasingCurve::InOutQuad);
}

void FluentSheetTest::testSetContentWidget() {
    QLabel* content = new QLabel("Test Content");

    m_sheet->setContentWidget(content);
    QCOMPARE(m_sheet->contentWidget(), content);
}

void FluentSheetTest::testTitle() {
    QString title = "Test Title";
    m_sheet->setTitle(title);
    QCOMPARE(m_sheet->title(), title);
}

void FluentSheetTest::testSubtitle() {
    QString subtitle = "Test Subtitle";
    m_sheet->setSubtitle(subtitle);
    QCOMPARE(m_sheet->subtitle(), subtitle);
}

void FluentSheetTest::testIsOpen() {
    QVERIFY(!m_sheet->isOpen());

    m_sheet->open();
    QVERIFY(m_sheet->isOpen());

    m_sheet->close();
    waitForAnimation(m_sheet);
    QVERIFY(!m_sheet->isOpen());
}

FluentSheet* FluentSheetTest::createSheet(FluentSheetDirection direction) {
    return new FluentSheet(direction, m_mainWindow);
}

QMainWindow* FluentSheetTest::createMainWindow() {
    QMainWindow* window = new QMainWindow();
    window->resize(800, 600);
    window->show();
    return window;
}

void FluentSheetTest::simulateMouseDrag(QWidget* widget, const QPoint& start,
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

void FluentSheetTest::simulateKeyPress(QWidget* widget, Qt::Key key) {
    QKeyEvent keyEvent(QEvent::KeyPress, key, Qt::NoModifier);
    QApplication::sendEvent(widget, &keyEvent);
}

void FluentSheetTest::waitForAnimation(FluentSheet* sheet) {
    QTest::qWait(sheet->animationDuration() + 100);
}

QTEST_MAIN(FluentSheetTest)
#include "FluentSheetTest.moc"
