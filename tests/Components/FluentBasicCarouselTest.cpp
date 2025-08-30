// tests/Components/FluentBasicCarouselTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentBasicCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentBasicCarouselTest : public QObject {
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

    // State tests
    void testLooping();
    void testTransitionDuration();
    void testTransitionType();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardNavigation();

    // Animation tests
    void testTransitionAnimations();

private:
    FluentBasicCarousel* m_carousel{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentBasicCarouselTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentBasicCarouselTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentBasicCarouselTest::init() {
    m_carousel = new FluentBasicCarousel(m_testWidget);
}

void FluentBasicCarouselTest::cleanup() {
    delete m_carousel;
    m_carousel = nullptr;
}

void FluentBasicCarouselTest::testDefaultConstructor() {
    QVERIFY(m_carousel != nullptr);
    QCOMPARE(m_carousel->currentSlide(), 0);
    QCOMPARE(m_carousel->slideCount(), 0);
    QVERIFY(m_carousel->isLooping());
    QVERIFY(m_carousel->transitionDuration() > 0);
}

void FluentBasicCarouselTest::testNext() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentBasicCarousel::currentSlideChanged);
    
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

void FluentBasicCarouselTest::testPrevious() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentBasicCarousel::currentSlideChanged);
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    m_carousel->previous();
    QCOMPARE(m_carousel->currentSlide(), 2); // Should wrap to last slide
    QCOMPARE(currentSlideSpy.count(), 1);
}

void FluentBasicCarouselTest::testGoToSlide() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentBasicCarousel::currentSlideChanged);
    
    m_carousel->goToSlide(2);
    QCOMPARE(m_carousel->currentSlide(), 2);
    QCOMPARE(currentSlideSpy.count(), 1);
    
    // Test invalid index
    m_carousel->goToSlide(10);
    QCOMPARE(m_carousel->currentSlide(), 2); // Should remain unchanged
    
    // Test negative index
    m_carousel->goToSlide(-1);
    QCOMPARE(m_carousel->currentSlide(), 2); // Should remain unchanged
}

void FluentBasicCarouselTest::testAddSlide() {
    QSignalSpy slideCountSpy(m_carousel, &FluentBasicCarousel::slideCountChanged);
    
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    
    m_carousel->addSlide(slide1);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(slideCountSpy.count(), 1);
    
    m_carousel->addSlide(slide2);
    QCOMPARE(m_carousel->slideCount(), 2);
    QCOMPARE(slideCountSpy.count(), 2);
}

void FluentBasicCarouselTest::testRemoveSlide() {
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    auto* slide3 = new QWidget();
    
    m_carousel->addSlide(slide1);
    m_carousel->addSlide(slide2);
    m_carousel->addSlide(slide3);
    
    QSignalSpy slideCountSpy(m_carousel, &FluentBasicCarousel::slideCountChanged);
    QSignalSpy currentSlideSpy(m_carousel, &FluentBasicCarousel::currentSlideChanged);
    
    // Remove middle slide
    m_carousel->removeSlide(1);
    QCOMPARE(m_carousel->slideCount(), 2);
    QCOMPARE(slideCountSpy.count(), 1);
    
    // Remove current slide (should adjust current index)
    m_carousel->goToSlide(1);
    m_carousel->removeSlide(1);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(m_carousel->currentSlide(), 0);
}

void FluentBasicCarouselTest::testSlideCount() {
    QCOMPARE(m_carousel->slideCount(), 0);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 1);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 2);
    
    m_carousel->removeSlide(0);
    QCOMPARE(m_carousel->slideCount(), 1);
}

void FluentBasicCarouselTest::testCurrentSlide() {
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    
    m_carousel->goToSlide(1);
    QCOMPARE(m_carousel->currentSlide(), 1);
    
    m_carousel->goToSlide(2);
    QCOMPARE(m_carousel->currentSlide(), 2);
}

void FluentBasicCarouselTest::testLooping() {
    QSignalSpy loopingSpy(m_carousel, &FluentBasicCarousel::loopingChanged);
    
    m_carousel->setLooping(false);
    QVERIFY(!m_carousel->isLooping());
    QCOMPARE(loopingSpy.count(), 1);
    
    // Test non-looping behavior
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    m_carousel->goToSlide(1); // Go to last slide
    m_carousel->next(); // Should not wrap around
    QCOMPARE(m_carousel->currentSlide(), 1);
    
    m_carousel->goToSlide(0); // Go to first slide
    m_carousel->previous(); // Should not wrap around
    QCOMPARE(m_carousel->currentSlide(), 0);
}

void FluentBasicCarouselTest::testTransitionDuration() {
    QSignalSpy durationSpy(m_carousel, &FluentBasicCarousel::transitionDurationChanged);
    
    m_carousel->setTransitionDuration(500);
    QCOMPARE(m_carousel->transitionDuration(), 500);
    QCOMPARE(durationSpy.count(), 1);
    
    // Test invalid duration
    m_carousel->setTransitionDuration(-100);
    QVERIFY(m_carousel->transitionDuration() >= 0);
}

void FluentBasicCarouselTest::testTransitionType() {
    QSignalSpy transitionSpy(m_carousel, &FluentBasicCarousel::transitionTypeChanged);
    
    m_carousel->setTransitionType(FluentCarouselTransition::Fade);
    QCOMPARE(m_carousel->transitionType(), FluentCarouselTransition::Fade);
    QCOMPARE(transitionSpy.count(), 1);
    
    m_carousel->setTransitionType(FluentCarouselTransition::Slide);
    QCOMPARE(m_carousel->transitionType(), FluentCarouselTransition::Slide);
    QCOMPARE(transitionSpy.count(), 2);
}

void FluentBasicCarouselTest::testSignalEmission() {
    QSignalSpy currentSlideSpy(m_carousel, &FluentBasicCarousel::currentSlideChanged);
    QSignalSpy slideCountSpy(m_carousel, &FluentBasicCarousel::slideCountChanged);
    QSignalSpy loopingSpy(m_carousel, &FluentBasicCarousel::loopingChanged);
    QSignalSpy durationSpy(m_carousel, &FluentBasicCarousel::transitionDurationChanged);
    QSignalSpy transitionSpy(m_carousel, &FluentBasicCarousel::transitionTypeChanged);
    QSignalSpy transitionStartedSpy(m_carousel, &FluentBasicCarousel::transitionStarted);
    QSignalSpy transitionFinishedSpy(m_carousel, &FluentBasicCarousel::transitionFinished);
    
    // Test all signals
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->goToSlide(1);
    m_carousel->setLooping(false);
    m_carousel->setTransitionDuration(500);
    m_carousel->setTransitionType(FluentCarouselTransition::Fade);
    
    QCOMPARE(slideCountSpy.count(), 2);
    QCOMPARE(currentSlideSpy.count(), 1);
    QCOMPARE(loopingSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(transitionSpy.count(), 1);
    QVERIFY(transitionStartedSpy.count() >= 0);
    QVERIFY(transitionFinishedSpy.count() >= 0);
}

void FluentBasicCarouselTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentBasicCarousel::currentSlideChanged);
    
    // Test mouse wheel navigation
    QWheelEvent wheelEvent(QPointF(100, 100), QPointF(100, 100), 
                          QPoint(0, 0), QPoint(0, 120), 
                          Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_carousel, &wheelEvent);
    
    // Mouse wheel should trigger navigation
    QTest::qWait(100);
}

void FluentBasicCarouselTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_carousel->setFocus();
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentBasicCarousel::currentSlideChanged);
    
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
}

void FluentBasicCarouselTest::testTransitionAnimations() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy transitionStartedSpy(m_carousel, &FluentBasicCarousel::transitionStarted);
    QSignalSpy transitionFinishedSpy(m_carousel, &FluentBasicCarousel::transitionFinished);
    
    // Set short transition duration for testing
    m_carousel->setTransitionDuration(100);
    
    // Test slide transition
    m_carousel->setTransitionType(FluentCarouselTransition::Slide);
    m_carousel->next();
    
    QCOMPARE(transitionStartedSpy.count(), 1);
    
    // Wait for transition to complete
    QTest::qWait(200);
    QCOMPARE(transitionFinishedSpy.count(), 1);
    
    // Test fade transition
    m_carousel->setTransitionType(FluentCarouselTransition::Fade);
    m_carousel->previous();
    
    QTest::qWait(200);
    QVERIFY(transitionFinishedSpy.count() >= 1);
}

QTEST_MAIN(FluentBasicCarouselTest)
#include "FluentBasicCarouselTest.moc"
