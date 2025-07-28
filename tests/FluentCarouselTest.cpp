// tests/FluentCarouselTest.cpp
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QGestureEvent>
#include <QSwipeGesture>
#include <QPanGesture>

#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentBasicCarousel.h"
#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Components/FluentIndicatorCarousel.h"
#include "FluentQt/Components/FluentTouchCarousel.h"

using namespace FluentQt::Components;

class FluentCarouselTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Base FluentCarousel tests
    void testCarouselConstructor();
    void testCarouselBasicFunctionality();
    void testCarouselItemManagement();
    void testCarouselNavigation();
    void testCarouselConfiguration();
    void testCarouselStates();
    void testCarouselTransitions();
    void testCarouselAutoPlay();
    void testCarouselKeyboardNavigation();
    void testCarouselAccessibility();
    void testCarouselSignals();
    void testCarouselEdgeCases();

    // FluentBasicCarousel tests
    void testBasicCarouselConstructor();
    void testBasicCarouselNavigation();
    void testBasicCarouselButtons();
    void testBasicCarouselKeyboard();

    // FluentAutoCarousel tests
    void testAutoCarouselConstructor();
    void testAutoCarouselAutoPlay();
    void testAutoCarouselControls();
    void testAutoCarouselProgress();
    void testAutoCarouselPauseResume();

    // FluentIndicatorCarousel tests
    void testIndicatorCarouselConstructor();
    void testIndicatorCarouselDots();
    void testIndicatorCarouselLines();
    void testIndicatorCarouselNumbers();
    void testIndicatorCarouselThumbnails();
    void testIndicatorCarouselInteraction();

    // FluentTouchCarousel tests
    void testTouchCarouselConstructor();
    void testTouchCarouselGestures();
    void testTouchCarouselSwipe();
    void testTouchCarouselPan();
    void testTouchCarouselMomentum();

    // Integration tests
    void testCarouselThemeIntegration();
    void testCarouselPerformance();
    void testCarouselMemoryManagement();

private:
    FluentCarousel* m_carousel{nullptr};
    FluentBasicCarousel* m_basicCarousel{nullptr};
    FluentAutoCarousel* m_autoCarousel{nullptr};
    FluentIndicatorCarousel* m_indicatorCarousel{nullptr};
    FluentTouchCarousel* m_touchCarousel{nullptr};

    // Helper methods
    void addTestItems(FluentCarousel* carousel, int count = 5);
    QWidget* createTestWidget(const QString& text, const QColor& color = Qt::blue);
    void simulateKeyPress(QWidget* widget, Qt::Key key);
    void simulateMouseClick(QWidget* widget, const QPoint& pos);
    void simulateSwipeGesture(QWidget* widget, QSwipeGesture::SwipeDirection direction);
};

void FluentCarouselTest::initTestCase() {
    qApp->setApplicationName("FluentCarouselTest");
    
    // Initialize theme system if needed
    // FluentTheme::instance().setMode(FluentThemeMode::Light);
}

void FluentCarouselTest::cleanupTestCase() {
    // Global cleanup
}

void FluentCarouselTest::init() {
    // Create test instances
    m_carousel = new FluentCarousel();
    m_basicCarousel = new FluentBasicCarousel();
    m_autoCarousel = new FluentAutoCarousel();
    m_indicatorCarousel = new FluentIndicatorCarousel();
    m_touchCarousel = new FluentTouchCarousel();

    // Show widgets for proper testing
    m_carousel->show();
    m_basicCarousel->show();
    m_autoCarousel->show();
    m_indicatorCarousel->show();
    m_touchCarousel->show();

    // Wait for widgets to be exposed
    QTest::qWaitForWindowExposed(m_carousel);
    QTest::qWaitForWindowExposed(m_basicCarousel);
    QTest::qWaitForWindowExposed(m_autoCarousel);
    QTest::qWaitForWindowExposed(m_indicatorCarousel);
    QTest::qWaitForWindowExposed(m_touchCarousel);
}

void FluentCarouselTest::cleanup() {
    delete m_carousel;
    delete m_basicCarousel;
    delete m_autoCarousel;
    delete m_indicatorCarousel;
    delete m_touchCarousel;

    m_carousel = nullptr;
    m_basicCarousel = nullptr;
    m_autoCarousel = nullptr;
    m_indicatorCarousel = nullptr;
    m_touchCarousel = nullptr;
}

// Base FluentCarousel tests
void FluentCarouselTest::testCarouselConstructor() {
    // Test default constructor
    auto* carousel1 = new FluentCarousel();
    QVERIFY(carousel1 != nullptr);
    QCOMPARE(carousel1->itemCount(), 0);
    QCOMPARE(carousel1->currentIndex(), -1);
    QCOMPARE(carousel1->orientation(), FluentCarouselOrientation::Horizontal);
    QCOMPARE(carousel1->transition(), FluentCarouselTransition::Slide);
    QVERIFY(!carousel1->isAutoPlayEnabled());
    delete carousel1;

    // Test constructor with parent
    auto* parent = new QWidget();
    auto* carousel2 = new FluentCarousel(parent);
    QVERIFY(carousel2->parent() == parent);
    delete parent; // Should also delete carousel2

    // Test constructor with configuration
    FluentCarouselConfig config;
    config.orientation = FluentCarouselOrientation::Vertical;
    config.transition = FluentCarouselTransition::Fade;
    config.autoPlay = FluentCarouselAutoPlay::Enabled;
    config.autoPlayInterval = 2000;

    auto* carousel3 = new FluentCarousel(config);
    QCOMPARE(carousel3->orientation(), FluentCarouselOrientation::Vertical);
    QCOMPARE(carousel3->transition(), FluentCarouselTransition::Fade);
    QVERIFY(carousel3->isAutoPlayEnabled());
    QCOMPARE(carousel3->autoPlayInterval(), 2000);
    delete carousel3;
}

void FluentCarouselTest::testCarouselBasicFunctionality() {
    // Test basic properties
    QVERIFY(m_carousel != nullptr);
    QCOMPARE(m_carousel->itemCount(), 0);
    QCOMPARE(m_carousel->currentIndex(), -1);

    // Add test items
    addTestItems(m_carousel, 3);
    QCOMPARE(m_carousel->itemCount(), 3);
    QCOMPARE(m_carousel->currentIndex(), 0);

    // Test navigation
    m_carousel->next();
    QCOMPARE(m_carousel->currentIndex(), 1);

    m_carousel->previous();
    QCOMPARE(m_carousel->currentIndex(), 0);

    // Test direct navigation
    m_carousel->goToIndex(2);
    QCOMPARE(m_carousel->currentIndex(), 2);
}

void FluentCarouselTest::testCarouselItemManagement() {
    // Test adding items
    auto* widget1 = createTestWidget("Item 1", Qt::red);
    auto* widget2 = createTestWidget("Item 2", Qt::green);
    auto* widget3 = createTestWidget("Item 3", Qt::blue);

    m_carousel->addItem(widget1);
    QCOMPARE(m_carousel->itemCount(), 1);
    QCOMPARE(m_carousel->currentIndex(), 0);

    m_carousel->addItem(widget2);
    m_carousel->addItem(widget3);
    QCOMPARE(m_carousel->itemCount(), 3);

    // Test inserting items
    auto* widget4 = createTestWidget("Item 4", Qt::yellow);
    m_carousel->insertItem(1, widget4);
    QCOMPARE(m_carousel->itemCount(), 4);

    // Test removing items
    m_carousel->removeItem(widget2);
    QCOMPARE(m_carousel->itemCount(), 3);

    // Test removing by index
    m_carousel->removeItemAt(0);
    QCOMPARE(m_carousel->itemCount(), 2);

    // Test clearing all items
    m_carousel->clear();
    QCOMPARE(m_carousel->itemCount(), 0);
    QCOMPARE(m_carousel->currentIndex(), -1);
}

void FluentCarouselTest::testCarouselNavigation() {
    addTestItems(m_carousel, 5);

    // Test next navigation
    QSignalSpy currentIndexChangedSpy(m_carousel, &FluentCarousel::currentIndexChanged);
    QSignalSpy itemChangedSpy(m_carousel, &FluentCarousel::itemChanged);

    m_carousel->next();
    QCOMPARE(m_carousel->currentIndex(), 1);
    QCOMPARE(currentIndexChangedSpy.count(), 1);
    QCOMPARE(itemChangedSpy.count(), 1);

    // Test previous navigation
    m_carousel->previous();
    QCOMPARE(m_carousel->currentIndex(), 0);
    QCOMPARE(currentIndexChangedSpy.count(), 2);

    // Test wrap around
    m_carousel->setWrapAround(true);
    m_carousel->previous(); // Should wrap to last item
    QCOMPARE(m_carousel->currentIndex(), 4);

    m_carousel->next(); // Should wrap to first item
    QCOMPARE(m_carousel->currentIndex(), 0);

    // Test no wrap around
    m_carousel->setWrapAround(false);
    m_carousel->previous(); // Should stay at first item
    QCOMPARE(m_carousel->currentIndex(), 0);

    m_carousel->goToIndex(4);
    m_carousel->next(); // Should stay at last item
    QCOMPARE(m_carousel->currentIndex(), 4);
}

void FluentCarouselTest::testCarouselConfiguration() {
    FluentCarouselConfig config;
    config.orientation = FluentCarouselOrientation::Vertical;
    config.transition = FluentCarouselTransition::Scale;
    config.transitionDuration = 500;
    config.autoPlay = FluentCarouselAutoPlay::Enabled;
    config.autoPlayInterval = 3000;
    config.wrapAround = false;
    config.showNavigationButtons = false;

    m_carousel->setConfiguration(config);

    QCOMPARE(m_carousel->orientation(), FluentCarouselOrientation::Vertical);
    QCOMPARE(m_carousel->transition(), FluentCarouselTransition::Scale);
    QCOMPARE(m_carousel->transitionDuration(), 500);
    QVERIFY(m_carousel->isAutoPlayEnabled());
    QCOMPARE(m_carousel->autoPlayInterval(), 3000);
    QVERIFY(!m_carousel->wrapAround());
    QVERIFY(!m_carousel->showNavigationButtons());
}

void FluentCarouselTest::testCarouselStates() {
    addTestItems(m_carousel, 3);

    // Test enabled/disabled state
    QVERIFY(m_carousel->isEnabled());
    m_carousel->setEnabled(false);
    QVERIFY(!m_carousel->isEnabled());

    // Navigation should be disabled
    int currentIndex = m_carousel->currentIndex();
    m_carousel->next();
    QCOMPARE(m_carousel->currentIndex(), currentIndex); // Should not change

    m_carousel->setEnabled(true);
    QVERIFY(m_carousel->isEnabled());

    // Test focus state
    m_carousel->setFocus();
    QVERIFY(m_carousel->hasFocus());
}

// Helper methods
void FluentCarouselTest::addTestItems(FluentCarousel* carousel, int count) {
    for (int i = 0; i < count; ++i) {
        auto* widget = createTestWidget(QString("Item %1").arg(i + 1), 
                                      QColor::fromHsv(i * 60, 255, 255));
        carousel->addItem(widget);
    }
}

QWidget* FluentCarouselTest::createTestWidget(const QString& text, const QColor& color) {
    auto* widget = new QWidget();
    widget->setFixedSize(200, 150);
    widget->setStyleSheet(QString("background-color: %1; color: white;").arg(color.name()));
    
    auto* label = new QLabel(text, widget);
    label->setAlignment(Qt::AlignCenter);
    label->setGeometry(widget->rect());
    
    return widget;
}

void FluentCarouselTest::simulateKeyPress(QWidget* widget, Qt::Key key) {
    QKeyEvent keyEvent(QEvent::KeyPress, key, Qt::NoModifier);
    QApplication::sendEvent(widget, &keyEvent);
}

void FluentCarouselTest::simulateMouseClick(QWidget* widget, const QPoint& pos) {
    QMouseEvent pressEvent(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(widget, &pressEvent);
    QApplication::sendEvent(widget, &releaseEvent);
}

void FluentCarouselTest::simulateSwipeGesture(QWidget* widget, QSwipeGesture::SwipeDirection direction) {
    Q_UNUSED(widget)
    Q_UNUSED(direction)
    // Gesture simulation would require more complex setup
    // This is a placeholder for gesture testing
}

void FluentCarouselTest::testCarouselTransitions() {
    addTestItems(m_carousel, 3);

    // Test slide transition
    m_carousel->setTransition(FluentCarouselTransition::Slide);
    QCOMPARE(m_carousel->transition(), FluentCarouselTransition::Slide);

    QSignalSpy transitionStartedSpy(m_carousel, &FluentCarousel::transitionStarted);
    QSignalSpy transitionFinishedSpy(m_carousel, &FluentCarousel::transitionFinished);

    m_carousel->next();
    QCOMPARE(transitionStartedSpy.count(), 1);

    // Wait for transition to complete
    QTest::qWait(m_carousel->transitionDuration() + 100);
    QCOMPARE(transitionFinishedSpy.count(), 1);

    // Test fade transition
    m_carousel->setTransition(FluentCarouselTransition::Fade);
    QCOMPARE(m_carousel->transition(), FluentCarouselTransition::Fade);

    // Test scale transition
    m_carousel->setTransition(FluentCarouselTransition::Scale);
    QCOMPARE(m_carousel->transition(), FluentCarouselTransition::Scale);

    // Test transition duration
    m_carousel->setTransitionDuration(500);
    QCOMPARE(m_carousel->transitionDuration(), 500);
}

void FluentCarouselTest::testCarouselAutoPlay() {
    addTestItems(m_carousel, 3);

    // Test auto-play enable/disable
    QVERIFY(!m_carousel->isAutoPlayEnabled());

    m_carousel->setAutoPlayEnabled(true);
    QVERIFY(m_carousel->isAutoPlayEnabled());

    // Test auto-play interval
    m_carousel->setAutoPlayInterval(1000);
    QCOMPARE(m_carousel->autoPlayInterval(), 1000);

    // Test auto-play progression
    QSignalSpy autoPlayProgressSpy(m_carousel, &FluentCarousel::autoPlayProgressChanged);

    m_carousel->startAutoPlay();
    QVERIFY(m_carousel->isAutoPlayActive());

    // Wait for auto-play to progress
    QTest::qWait(1200);
    QCOMPARE(m_carousel->currentIndex(), 1);

    // Test pause/resume
    m_carousel->pauseAutoPlay();
    QVERIFY(!m_carousel->isAutoPlayActive());

    m_carousel->resumeAutoPlay();
    QVERIFY(m_carousel->isAutoPlayActive());

    m_carousel->stopAutoPlay();
    QVERIFY(!m_carousel->isAutoPlayActive());
}

void FluentCarouselTest::testCarouselKeyboardNavigation() {
    addTestItems(m_carousel, 3);
    m_carousel->setFocus();

    // Test arrow key navigation
    simulateKeyPress(m_carousel, Qt::Key_Right);
    QCOMPARE(m_carousel->currentIndex(), 1);

    simulateKeyPress(m_carousel, Qt::Key_Left);
    QCOMPARE(m_carousel->currentIndex(), 0);

    // Test vertical orientation
    m_carousel->setOrientation(FluentCarouselOrientation::Vertical);

    simulateKeyPress(m_carousel, Qt::Key_Down);
    QCOMPARE(m_carousel->currentIndex(), 1);

    simulateKeyPress(m_carousel, Qt::Key_Up);
    QCOMPARE(m_carousel->currentIndex(), 0);

    // Test Home/End keys
    simulateKeyPress(m_carousel, Qt::Key_End);
    QCOMPARE(m_carousel->currentIndex(), 2);

    simulateKeyPress(m_carousel, Qt::Key_Home);
    QCOMPARE(m_carousel->currentIndex(), 0);

    // Test Page Up/Down
    simulateKeyPress(m_carousel, Qt::Key_PageDown);
    QCOMPARE(m_carousel->currentIndex(), 1);

    simulateKeyPress(m_carousel, Qt::Key_PageUp);
    QCOMPARE(m_carousel->currentIndex(), 0);
}

void FluentCarouselTest::testCarouselAccessibility() {
    addTestItems(m_carousel, 3);

    // Test accessibility properties
    QVERIFY(!m_carousel->accessibleName().isEmpty());
    QVERIFY(!m_carousel->accessibleDescription().isEmpty());

    // Test ARIA attributes
    QString ariaLabel = m_carousel->property("aria-label").toString();
    QVERIFY(!ariaLabel.isEmpty());

    QString ariaRole = m_carousel->property("aria-role").toString();
    QCOMPARE(ariaRole, QString("tablist"));

    // Test current item accessibility
    QString ariaCurrent = m_carousel->property("aria-current").toString();
    QVERIFY(!ariaCurrent.isEmpty());

    // Test live region updates
    QSignalSpy accessibilityUpdateSpy(m_carousel, &FluentCarousel::accessibilityUpdateRequested);
    m_carousel->next();
    QCOMPARE(accessibilityUpdateSpy.count(), 1);
}

void FluentCarouselTest::testCarouselSignals() {
    addTestItems(m_carousel, 3);

    // Test currentIndexChanged signal
    QSignalSpy currentIndexChangedSpy(m_carousel, &FluentCarousel::currentIndexChanged);
    m_carousel->next();
    QCOMPARE(currentIndexChangedSpy.count(), 1);
    QCOMPARE(currentIndexChangedSpy.first().first().toInt(), 1);

    // Test itemChanged signal
    QSignalSpy itemChangedSpy(m_carousel, &FluentCarousel::itemChanged);
    m_carousel->previous();
    QCOMPARE(itemChangedSpy.count(), 1);

    // Test itemAdded signal
    QSignalSpy itemAddedSpy(m_carousel, &FluentCarousel::itemAdded);
    auto* newWidget = createTestWidget("New Item", Qt::magenta);
    m_carousel->addItem(newWidget);
    QCOMPARE(itemAddedSpy.count(), 1);

    // Test itemRemoved signal
    QSignalSpy itemRemovedSpy(m_carousel, &FluentCarousel::itemRemoved);
    m_carousel->removeItem(newWidget);
    QCOMPARE(itemRemovedSpy.count(), 1);

    // Test configurationChanged signal
    QSignalSpy configChangedSpy(m_carousel, &FluentCarousel::configurationChanged);
    FluentCarouselConfig newConfig;
    newConfig.orientation = FluentCarouselOrientation::Vertical;
    m_carousel->setConfiguration(newConfig);
    QCOMPARE(configChangedSpy.count(), 1);
}

void FluentCarouselTest::testCarouselEdgeCases() {
    // Test empty carousel
    QCOMPARE(m_carousel->itemCount(), 0);
    QCOMPARE(m_carousel->currentIndex(), -1);

    // Navigation on empty carousel should not crash
    m_carousel->next();
    m_carousel->previous();
    m_carousel->goToIndex(0);
    QCOMPARE(m_carousel->currentIndex(), -1);

    // Test invalid index navigation
    addTestItems(m_carousel, 3);
    m_carousel->goToIndex(-1);
    QCOMPARE(m_carousel->currentIndex(), 0); // Should stay at current

    m_carousel->goToIndex(10);
    QCOMPARE(m_carousel->currentIndex(), 0); // Should stay at current

    // Test null widget addition
    m_carousel->addItem(nullptr);
    QCOMPARE(m_carousel->itemCount(), 3); // Should not add null

    // Test removing non-existent widget
    auto* nonExistentWidget = createTestWidget("Non-existent", Qt::black);
    m_carousel->removeItem(nonExistentWidget);
    QCOMPARE(m_carousel->itemCount(), 3); // Should not change
    delete nonExistentWidget;

    // Test removing by invalid index
    m_carousel->removeItemAt(-1);
    m_carousel->removeItemAt(10);
    QCOMPARE(m_carousel->itemCount(), 3); // Should not change

    // Test configuration with invalid values
    FluentCarouselConfig invalidConfig;
    invalidConfig.transitionDuration = -100; // Invalid negative duration
    invalidConfig.autoPlayInterval = 0; // Invalid zero interval

    m_carousel->setConfiguration(invalidConfig);
    // Should use default/minimum values
    QVERIFY(m_carousel->transitionDuration() > 0);
    QVERIFY(m_carousel->autoPlayInterval() > 0);
}

// FluentBasicCarousel tests
void FluentCarouselTest::testBasicCarouselConstructor() {
    // Test default constructor
    auto* basicCarousel1 = new FluentBasicCarousel();
    QVERIFY(basicCarousel1 != nullptr);
    QVERIFY(basicCarousel1->showNavigationButtons());
    delete basicCarousel1;

    // Test constructor with parent
    auto* parent = new QWidget();
    auto* basicCarousel2 = new FluentBasicCarousel(parent);
    QVERIFY(basicCarousel2->parent() == parent);
    delete parent;

    // Test constructor with configuration
    FluentCarouselConfig config;
    config.showNavigationButtons = false;
    auto* basicCarousel3 = new FluentBasicCarousel(config);
    QVERIFY(!basicCarousel3->showNavigationButtons());
    delete basicCarousel3;
}

void FluentCarouselTest::testBasicCarouselNavigation() {
    addTestItems(m_basicCarousel, 3);

    // Test navigation methods
    m_basicCarousel->next();
    QCOMPARE(m_basicCarousel->currentIndex(), 1);

    m_basicCarousel->previous();
    QCOMPARE(m_basicCarousel->currentIndex(), 0);

    // Test navigation button states
    QVERIFY(m_basicCarousel->showNavigationButtons());

    // At first item, previous button should be disabled (if wrap-around is off)
    m_basicCarousel->setWrapAround(false);
    QCOMPARE(m_basicCarousel->currentIndex(), 0);
    // Previous button should be disabled - this would need access to internal buttons

    // At last item, next button should be disabled
    m_basicCarousel->goToIndex(2);
    QCOMPARE(m_basicCarousel->currentIndex(), 2);
    // Next button should be disabled - this would need access to internal buttons
}

QTEST_MAIN(FluentCarouselTest)
#include "FluentCarouselTest.moc"
