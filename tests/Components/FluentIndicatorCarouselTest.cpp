// tests/Components/FluentIndicatorCarouselTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentIndicatorCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentIndicatorCarouselTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Navigation tests
    void testNext();
    void testPrevious();
    void testGoToSlide();

    // Content tests
    void testAddSlide();
    void testRemoveSlide();
    void testSlideCount();
    void testCurrentSlide();

    // Indicator tests
    void testIndicatorPosition();
    void testIndicatorStyle();
    void testIndicatorSize();
    void testIndicatorSpacing();

    // Auto-play tests
    void testAutoPlay();
    void testAutoPlayInterval();
    void testPauseOnHover();

    // State tests
    void testLooping();
    void testTransitionDuration();
    void testTransitionType();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testIndicatorClick();
    void testKeyboardNavigation();
    void testMouseInteraction();

private:
    FluentIndicatorCarousel* m_carousel{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentIndicatorCarouselTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentIndicatorCarouselTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentIndicatorCarouselTest::init() {
    m_carousel = new FluentIndicatorCarousel(m_testWidget);
}

void FluentIndicatorCarouselTest::cleanup() {
    delete m_carousel;
    m_carousel = nullptr;
}

void FluentIndicatorCarouselTest::testDefaultConstructor() {
    QVERIFY(m_carousel != nullptr);
    QCOMPARE(m_carousel->currentSlide(), 0);
    QCOMPARE(m_carousel->slideCount(), 0);
    QCOMPARE(m_carousel->indicatorPosition(), FluentCarouselIndicatorPosition::Bottom);
    QCOMPARE(m_carousel->indicatorStyle(), FluentCarouselIndicatorStyle::Dots);
    QVERIFY(m_carousel->isLooping());
    QVERIFY(m_carousel->isAutoPlay());
}

void FluentIndicatorCarouselTest::testNext() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentIndicatorCarousel::currentSlideChanged);
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    m_carousel->next();
    QCOMPARE(m_carousel->currentSlide(), 1);
    QCOMPARE(currentSlideSpy.count(), 1);
    
    m_carousel->next();
    QCOMPARE(m_carousel->currentSlide(), 2);
    
    // Test looping
    m_carousel->next();
    QCOMPARE(m_carousel->currentSlide(), 0);
}

void FluentIndicatorCarouselTest::testPrevious() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentIndicatorCarousel::currentSlideChanged);
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    m_carousel->previous();
    QCOMPARE(m_carousel->currentSlide(), 2); // Should wrap to last slide
    QCOMPARE(currentSlideSpy.count(), 1);
}

void FluentIndicatorCarouselTest::testGoToSlide() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentIndicatorCarousel::currentSlideChanged);
    
    m_carousel->goToSlide(2);
    QCOMPARE(m_carousel->currentSlide(), 2);
    QCOMPARE(currentSlideSpy.count(), 1);
    
    // Test invalid index
    m_carousel->goToSlide(10);
    QCOMPARE(m_carousel->currentSlide(), 2); // Should remain unchanged
}

void FluentIndicatorCarouselTest::testAddSlide() {
    QSignalSpy slideCountSpy(m_carousel, &FluentIndicatorCarousel::slideCountChanged);
    QSignalSpy slideAddedSpy(m_carousel, &FluentIndicatorCarousel::slideAdded);
    
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    
    m_carousel->addSlide(slide1);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(slideCountSpy.count(), 1);
    QCOMPARE(slideAddedSpy.count(), 1);
    
    m_carousel->addSlide(slide2);
    QCOMPARE(m_carousel->slideCount(), 2);
    QCOMPARE(slideCountSpy.count(), 2);
    QCOMPARE(slideAddedSpy.count(), 2);
}

void FluentIndicatorCarouselTest::testRemoveSlide() {
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    
    m_carousel->addSlide(slide1);
    m_carousel->addSlide(slide2);
    
    QSignalSpy slideCountSpy(m_carousel, &FluentIndicatorCarousel::slideCountChanged);
    QSignalSpy slideRemovedSpy(m_carousel, &FluentIndicatorCarousel::slideRemoved);
    
    m_carousel->removeSlide(0);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(slideCountSpy.count(), 1);
    QCOMPARE(slideRemovedSpy.count(), 1);
}

void FluentIndicatorCarouselTest::testSlideCount() {
    QCOMPARE(m_carousel->slideCount(), 0);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 1);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 2);
}

void FluentIndicatorCarouselTest::testCurrentSlide() {
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    
    m_carousel->goToSlide(1);
    QCOMPARE(m_carousel->currentSlide(), 1);
}

void FluentIndicatorCarouselTest::testIndicatorPosition() {
    QSignalSpy positionSpy(m_carousel, &FluentIndicatorCarousel::indicatorPositionChanged);
    
    m_carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Top);
    QCOMPARE(m_carousel->indicatorPosition(), FluentCarouselIndicatorPosition::Top);
    QCOMPARE(positionSpy.count(), 1);
    
    m_carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Left);
    QCOMPARE(m_carousel->indicatorPosition(), FluentCarouselIndicatorPosition::Left);
    QCOMPARE(positionSpy.count(), 2);
    
    m_carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Right);
    QCOMPARE(m_carousel->indicatorPosition(), FluentCarouselIndicatorPosition::Right);
    QCOMPARE(positionSpy.count(), 3);
}

void FluentIndicatorCarouselTest::testIndicatorStyle() {
    QSignalSpy styleSpy(m_carousel, &FluentIndicatorCarousel::indicatorStyleChanged);
    
    m_carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Lines);
    QCOMPARE(m_carousel->indicatorStyle(), FluentCarouselIndicatorStyle::Lines);
    QCOMPARE(styleSpy.count(), 1);
    
    m_carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Numbers);
    QCOMPARE(m_carousel->indicatorStyle(), FluentCarouselIndicatorStyle::Numbers);
    QCOMPARE(styleSpy.count(), 2);
    
    m_carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Thumbnails);
    QCOMPARE(m_carousel->indicatorStyle(), FluentCarouselIndicatorStyle::Thumbnails);
    QCOMPARE(styleSpy.count(), 3);
}

void FluentIndicatorCarouselTest::testIndicatorSize() {
    QSignalSpy sizeSpy(m_carousel, &FluentIndicatorCarousel::indicatorSizeChanged);
    
    m_carousel->setIndicatorSize(QSize(12, 12));
    QCOMPARE(m_carousel->indicatorSize(), QSize(12, 12));
    QCOMPARE(sizeSpy.count(), 1);
    
    // Test invalid size
    m_carousel->setIndicatorSize(QSize(-5, -5));
    QVERIFY(m_carousel->indicatorSize().width() > 0);
    QVERIFY(m_carousel->indicatorSize().height() > 0);
}

void FluentIndicatorCarouselTest::testIndicatorSpacing() {
    QSignalSpy spacingSpy(m_carousel, &FluentIndicatorCarousel::indicatorSpacingChanged);
    
    m_carousel->setIndicatorSpacing(8);
    QCOMPARE(m_carousel->indicatorSpacing(), 8);
    QCOMPARE(spacingSpy.count(), 1);
    
    // Test negative spacing
    m_carousel->setIndicatorSpacing(-2);
    QVERIFY(m_carousel->indicatorSpacing() >= 0); // Should remain non-negative
}

void FluentIndicatorCarouselTest::testAutoPlay() {
    QSignalSpy autoPlaySpy(m_carousel, &FluentIndicatorCarousel::autoPlayChanged);
    
    m_carousel->setAutoPlay(false);
    QVERIFY(!m_carousel->isAutoPlay());
    QCOMPARE(autoPlaySpy.count(), 1);
    
    m_carousel->setAutoPlay(true);
    QVERIFY(m_carousel->isAutoPlay());
    QCOMPARE(autoPlaySpy.count(), 2);
}

void FluentIndicatorCarouselTest::testAutoPlayInterval() {
    QSignalSpy intervalSpy(m_carousel, &FluentIndicatorCarousel::intervalChanged);
    
    m_carousel->setInterval(2000);
    QCOMPARE(m_carousel->interval(), 2000);
    QCOMPARE(intervalSpy.count(), 1);
    
    // Test invalid interval
    m_carousel->setInterval(-500);
    QVERIFY(m_carousel->interval() > 0); // Should remain positive
}

void FluentIndicatorCarouselTest::testPauseOnHover() {
    QSignalSpy pauseHoverSpy(m_carousel, &FluentIndicatorCarousel::pauseOnHoverChanged);
    
    m_carousel->setPauseOnHover(false);
    QVERIFY(!m_carousel->pauseOnHover());
    QCOMPARE(pauseHoverSpy.count(), 1);
    
    m_carousel->setPauseOnHover(true);
    QVERIFY(m_carousel->pauseOnHover());
    QCOMPARE(pauseHoverSpy.count(), 2);
}

void FluentIndicatorCarouselTest::testLooping() {
    QSignalSpy loopingSpy(m_carousel, &FluentIndicatorCarousel::loopingChanged);
    
    m_carousel->setLooping(false);
    QVERIFY(!m_carousel->isLooping());
    QCOMPARE(loopingSpy.count(), 1);
    
    // Test non-looping behavior
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    m_carousel->goToSlide(1); // Go to last slide
    m_carousel->next(); // Should not wrap around
    QCOMPARE(m_carousel->currentSlide(), 1);
}

void FluentIndicatorCarouselTest::testTransitionDuration() {
    QSignalSpy durationSpy(m_carousel, &FluentIndicatorCarousel::transitionDurationChanged);
    
    m_carousel->setTransitionDuration(500);
    QCOMPARE(m_carousel->transitionDuration(), 500);
    QCOMPARE(durationSpy.count(), 1);
    
    // Test invalid duration
    m_carousel->setTransitionDuration(-100);
    QVERIFY(m_carousel->transitionDuration() >= 0);
}

void FluentIndicatorCarouselTest::testTransitionType() {
    QSignalSpy transitionSpy(m_carousel, &FluentIndicatorCarousel::transitionTypeChanged);
    
    m_carousel->setTransitionType(FluentCarouselTransition::Fade);
    QCOMPARE(m_carousel->transitionType(), FluentCarouselTransition::Fade);
    QCOMPARE(transitionSpy.count(), 1);
    
    m_carousel->setTransitionType(FluentCarouselTransition::Slide);
    QCOMPARE(m_carousel->transitionType(), FluentCarouselTransition::Slide);
    QCOMPARE(transitionSpy.count(), 2);
}

void FluentIndicatorCarouselTest::testSignalEmission() {
    QSignalSpy currentSlideSpy(m_carousel, &FluentIndicatorCarousel::currentSlideChanged);
    QSignalSpy slideCountSpy(m_carousel, &FluentIndicatorCarousel::slideCountChanged);
    QSignalSpy slideAddedSpy(m_carousel, &FluentIndicatorCarousel::slideAdded);
    QSignalSpy slideRemovedSpy(m_carousel, &FluentIndicatorCarousel::slideRemoved);
    QSignalSpy positionSpy(m_carousel, &FluentIndicatorCarousel::indicatorPositionChanged);
    QSignalSpy styleSpy(m_carousel, &FluentIndicatorCarousel::indicatorStyleChanged);
    QSignalSpy sizeSpy(m_carousel, &FluentIndicatorCarousel::indicatorSizeChanged);
    QSignalSpy spacingSpy(m_carousel, &FluentIndicatorCarousel::indicatorSpacingChanged);
    QSignalSpy autoPlaySpy(m_carousel, &FluentIndicatorCarousel::autoPlayChanged);
    QSignalSpy intervalSpy(m_carousel, &FluentIndicatorCarousel::intervalChanged);
    QSignalSpy pauseHoverSpy(m_carousel, &FluentIndicatorCarousel::pauseOnHoverChanged);
    QSignalSpy loopingSpy(m_carousel, &FluentIndicatorCarousel::loopingChanged);
    QSignalSpy durationSpy(m_carousel, &FluentIndicatorCarousel::transitionDurationChanged);
    QSignalSpy transitionSpy(m_carousel, &FluentIndicatorCarousel::transitionTypeChanged);
    
    // Test all signals
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->goToSlide(1);
    m_carousel->removeSlide(0);
    m_carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Top);
    m_carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Lines);
    m_carousel->setIndicatorSize(QSize(10, 10));
    m_carousel->setIndicatorSpacing(5);
    m_carousel->setAutoPlay(false);
    m_carousel->setInterval(2000);
    m_carousel->setPauseOnHover(false);
    m_carousel->setLooping(false);
    m_carousel->setTransitionDuration(300);
    m_carousel->setTransitionType(FluentCarouselTransition::Fade);
    
    QCOMPARE(slideCountSpy.count(), 3); // add, add, remove
    QCOMPARE(slideAddedSpy.count(), 2);
    QCOMPARE(slideRemovedSpy.count(), 1);
    QCOMPARE(currentSlideSpy.count(), 2); // goToSlide, removeSlide adjustment
    QCOMPARE(positionSpy.count(), 1);
    QCOMPARE(styleSpy.count(), 1);
    QCOMPARE(sizeSpy.count(), 1);
    QCOMPARE(spacingSpy.count(), 1);
    QCOMPARE(autoPlaySpy.count(), 1);
    QCOMPARE(intervalSpy.count(), 1);
    QCOMPARE(pauseHoverSpy.count(), 1);
    QCOMPARE(loopingSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(transitionSpy.count(), 1);
}

void FluentIndicatorCarouselTest::testIndicatorClick() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentIndicatorCarousel::currentSlideChanged);
    QSignalSpy indicatorClickedSpy(m_carousel, &FluentIndicatorCarousel::indicatorClicked);
    
    // Simulate indicator click
    m_carousel->clickIndicator(2);
    QCOMPARE(m_carousel->currentSlide(), 2);
    QCOMPARE(currentSlideSpy.count(), 1);
    QCOMPARE(indicatorClickedSpy.count(), 1);
    
    // Test clicking invalid indicator
    m_carousel->clickIndicator(10);
    QCOMPARE(m_carousel->currentSlide(), 2); // Should remain unchanged
}

void FluentIndicatorCarouselTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_carousel->setFocus();
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentIndicatorCarousel::currentSlideChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_carousel, Qt::Key_Right);
    QCOMPARE(currentSlideSpy.count(), 1);
    QCOMPARE(m_carousel->currentSlide(), 1);
    
    QTest::keyClick(m_carousel, Qt::Key_Left);
    QCOMPARE(currentSlideSpy.count(), 2);
    QCOMPARE(m_carousel->currentSlide(), 0);
    
    // Test Home/End keys
    QTest::keyClick(m_carousel, Qt::Key_End);
    QCOMPARE(m_carousel->currentSlide(), 2);
    
    QTest::keyClick(m_carousel, Qt::Key_Home);
    QCOMPARE(m_carousel->currentSlide(), 0);
    
    // Test number keys for direct navigation
    QTest::keyClick(m_carousel, Qt::Key_2);
    QCOMPARE(m_carousel->currentSlide(), 1); // 0-based index
}

void FluentIndicatorCarouselTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentIndicatorCarousel::currentSlideChanged);
    
    // Test mouse click on carousel area
    QTest::mouseClick(m_carousel, Qt::LeftButton);
    
    // Test hover behavior for auto-play pause
    m_carousel->setAutoPlay(true);
    m_carousel->setPauseOnHover(true);
    
    QTest::mouseMove(m_carousel);
    QTest::qWait(100);
    
    // Test mouse wheel navigation
    QWheelEvent wheelEvent(QPointF(100, 100), QPointF(100, 100), 
                          QPoint(0, 0), QPoint(0, 120), 
                          Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_carousel, &wheelEvent);
    
    QTest::qWait(100);
}

QTEST_MAIN(FluentIndicatorCarouselTest)
#include "FluentIndicatorCarouselTest.moc"
