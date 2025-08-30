// tests/Components/FluentCarouselTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentCarouselTest : public QObject {
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
    void testShowIndicators();
    void testIndicatorPosition();
    void testIndicatorStyle();

    // Navigation controls tests
    void testShowNavigationButtons();
    void testNavigationButtonStyle();

    // State tests
    void testLooping();
    void testTransitionDuration();
    void testTransitionType();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardNavigation();

private:
    FluentCarousel* m_carousel{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentCarouselTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentCarouselTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentCarouselTest::init() {
    m_carousel = new FluentCarousel(m_testWidget);
}

void FluentCarouselTest::cleanup() {
    delete m_carousel;
    m_carousel = nullptr;
}

void FluentCarouselTest::testDefaultConstructor() {
    QVERIFY(m_carousel != nullptr);
    QCOMPARE(m_carousel->currentSlide(), 0);
    QCOMPARE(m_carousel->slideCount(), 0);
    QVERIFY(m_carousel->isLooping());
    QVERIFY(m_carousel->showIndicators());
    QVERIFY(m_carousel->showNavigationButtons());
}

void FluentCarouselTest::testNext() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentCarousel::currentSlideChanged);
    
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

void FluentCarouselTest::testPrevious() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentCarousel::currentSlideChanged);
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    m_carousel->previous();
    QCOMPARE(m_carousel->currentSlide(), 2); // Should wrap to last slide
    QCOMPARE(currentSlideSpy.count(), 1);
}

void FluentCarouselTest::testGoToSlide() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentCarousel::currentSlideChanged);
    
    m_carousel->goToSlide(2);
    QCOMPARE(m_carousel->currentSlide(), 2);
    QCOMPARE(currentSlideSpy.count(), 1);
    
    // Test invalid index
    m_carousel->goToSlide(10);
    QCOMPARE(m_carousel->currentSlide(), 2); // Should remain unchanged
}

void FluentCarouselTest::testAddSlide() {
    QSignalSpy slideCountSpy(m_carousel, &FluentCarousel::slideCountChanged);
    
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    
    m_carousel->addSlide(slide1);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(slideCountSpy.count(), 1);
    
    m_carousel->addSlide(slide2);
    QCOMPARE(m_carousel->slideCount(), 2);
    QCOMPARE(slideCountSpy.count(), 2);
}

void FluentCarouselTest::testRemoveSlide() {
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    
    m_carousel->addSlide(slide1);
    m_carousel->addSlide(slide2);
    
    QSignalSpy slideCountSpy(m_carousel, &FluentCarousel::slideCountChanged);
    
    m_carousel->removeSlide(0);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(slideCountSpy.count(), 1);
}

void FluentCarouselTest::testSlideCount() {
    QCOMPARE(m_carousel->slideCount(), 0);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 1);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 2);
}

void FluentCarouselTest::testCurrentSlide() {
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    
    m_carousel->goToSlide(1);
    QCOMPARE(m_carousel->currentSlide(), 1);
}

void FluentCarouselTest::testShowIndicators() {
    QSignalSpy indicatorsSpy(m_carousel, &FluentCarousel::showIndicatorsChanged);
    
    m_carousel->setShowIndicators(false);
    QVERIFY(!m_carousel->showIndicators());
    QCOMPARE(indicatorsSpy.count(), 1);
    
    m_carousel->setShowIndicators(true);
    QVERIFY(m_carousel->showIndicators());
    QCOMPARE(indicatorsSpy.count(), 2);
}

void FluentCarouselTest::testIndicatorPosition() {
    QSignalSpy positionSpy(m_carousel, &FluentCarousel::indicatorPositionChanged);
    
    m_carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Top);
    QCOMPARE(m_carousel->indicatorPosition(), FluentCarouselIndicatorPosition::Top);
    QCOMPARE(positionSpy.count(), 1);
    
    m_carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Left);
    QCOMPARE(m_carousel->indicatorPosition(), FluentCarouselIndicatorPosition::Left);
    QCOMPARE(positionSpy.count(), 2);
}

void FluentCarouselTest::testIndicatorStyle() {
    QSignalSpy styleSpy(m_carousel, &FluentCarousel::indicatorStyleChanged);
    
    m_carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Lines);
    QCOMPARE(m_carousel->indicatorStyle(), FluentCarouselIndicatorStyle::Lines);
    QCOMPARE(styleSpy.count(), 1);
    
    m_carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Numbers);
    QCOMPARE(m_carousel->indicatorStyle(), FluentCarouselIndicatorStyle::Numbers);
    QCOMPARE(styleSpy.count(), 2);
}

void FluentCarouselTest::testShowNavigationButtons() {
    QSignalSpy navButtonsSpy(m_carousel, &FluentCarousel::showNavigationButtonsChanged);
    
    m_carousel->setShowNavigationButtons(false);
    QVERIFY(!m_carousel->showNavigationButtons());
    QCOMPARE(navButtonsSpy.count(), 1);
    
    m_carousel->setShowNavigationButtons(true);
    QVERIFY(m_carousel->showNavigationButtons());
    QCOMPARE(navButtonsSpy.count(), 2);
}

void FluentCarouselTest::testNavigationButtonStyle() {
    QSignalSpy navStyleSpy(m_carousel, &FluentCarousel::navigationButtonStyleChanged);
    
    m_carousel->setNavigationButtonStyle(FluentCarouselNavigationStyle::Overlay);
    QCOMPARE(m_carousel->navigationButtonStyle(), FluentCarouselNavigationStyle::Overlay);
    QCOMPARE(navStyleSpy.count(), 1);
    
    m_carousel->setNavigationButtonStyle(FluentCarouselNavigationStyle::Outside);
    QCOMPARE(m_carousel->navigationButtonStyle(), FluentCarouselNavigationStyle::Outside);
    QCOMPARE(navStyleSpy.count(), 2);
}

void FluentCarouselTest::testLooping() {
    QSignalSpy loopingSpy(m_carousel, &FluentCarousel::loopingChanged);
    
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

void FluentCarouselTest::testTransitionDuration() {
    QSignalSpy durationSpy(m_carousel, &FluentCarousel::transitionDurationChanged);
    
    m_carousel->setTransitionDuration(500);
    QCOMPARE(m_carousel->transitionDuration(), 500);
    QCOMPARE(durationSpy.count(), 1);
}

void FluentCarouselTest::testTransitionType() {
    QSignalSpy transitionSpy(m_carousel, &FluentCarousel::transitionTypeChanged);
    
    m_carousel->setTransitionType(FluentCarouselTransition::Fade);
    QCOMPARE(m_carousel->transitionType(), FluentCarouselTransition::Fade);
    QCOMPARE(transitionSpy.count(), 1);
    
    m_carousel->setTransitionType(FluentCarouselTransition::Slide);
    QCOMPARE(m_carousel->transitionType(), FluentCarouselTransition::Slide);
    QCOMPARE(transitionSpy.count(), 2);
}

void FluentCarouselTest::testSignalEmission() {
    QSignalSpy currentSlideSpy(m_carousel, &FluentCarousel::currentSlideChanged);
    QSignalSpy slideCountSpy(m_carousel, &FluentCarousel::slideCountChanged);
    QSignalSpy indicatorsSpy(m_carousel, &FluentCarousel::showIndicatorsChanged);
    QSignalSpy positionSpy(m_carousel, &FluentCarousel::indicatorPositionChanged);
    QSignalSpy styleSpy(m_carousel, &FluentCarousel::indicatorStyleChanged);
    QSignalSpy navButtonsSpy(m_carousel, &FluentCarousel::showNavigationButtonsChanged);
    QSignalSpy navStyleSpy(m_carousel, &FluentCarousel::navigationButtonStyleChanged);
    QSignalSpy loopingSpy(m_carousel, &FluentCarousel::loopingChanged);
    QSignalSpy durationSpy(m_carousel, &FluentCarousel::transitionDurationChanged);
    QSignalSpy transitionSpy(m_carousel, &FluentCarousel::transitionTypeChanged);
    
    // Test all signals
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->goToSlide(1);
    m_carousel->setShowIndicators(false);
    m_carousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Top);
    m_carousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Lines);
    m_carousel->setShowNavigationButtons(false);
    m_carousel->setNavigationButtonStyle(FluentCarouselNavigationStyle::Overlay);
    m_carousel->setLooping(false);
    m_carousel->setTransitionDuration(500);
    m_carousel->setTransitionType(FluentCarouselTransition::Fade);
    
    QCOMPARE(slideCountSpy.count(), 2);
    QCOMPARE(currentSlideSpy.count(), 1);
    QCOMPARE(indicatorsSpy.count(), 1);
    QCOMPARE(positionSpy.count(), 1);
    QCOMPARE(styleSpy.count(), 1);
    QCOMPARE(navButtonsSpy.count(), 1);
    QCOMPARE(navStyleSpy.count(), 1);
    QCOMPARE(loopingSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(transitionSpy.count(), 1);
}

void FluentCarouselTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentCarousel::currentSlideChanged);
    
    // Test mouse click on navigation buttons
    QTest::mouseClick(m_carousel, Qt::LeftButton);
    
    // Test mouse wheel navigation
    QWheelEvent wheelEvent(QPointF(100, 100), QPointF(100, 100), 
                          QPoint(0, 0), QPoint(0, 120), 
                          Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_carousel, &wheelEvent);
    
    QTest::qWait(100);
}

void FluentCarouselTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_carousel->setFocus();
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentCarousel::currentSlideChanged);
    
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

QTEST_MAIN(FluentCarouselTest)
#include "FluentCarouselTest.moc"
