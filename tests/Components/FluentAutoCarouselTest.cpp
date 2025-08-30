// tests/Components/FluentAutoCarouselTest.cpp
#include <QSignalSpy>
#include <QTimer>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentAutoCarouselTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Auto-play tests
    void testAutoPlay();
    void testAutoPlayInterval();
    void testPauseOnHover();

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
    void testPlayState();
    void testLooping();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardNavigation();

private:
    FluentAutoCarousel* m_carousel{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentAutoCarouselTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentAutoCarouselTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentAutoCarouselTest::init() {
    m_carousel = new FluentAutoCarousel(m_testWidget);
}

void FluentAutoCarouselTest::cleanup() {
    delete m_carousel;
    m_carousel = nullptr;
}

void FluentAutoCarouselTest::testDefaultConstructor() {
    QVERIFY(m_carousel != nullptr);
    QCOMPARE(m_carousel->currentSlide(), 0);
    QCOMPARE(m_carousel->slideCount(), 0);
    QVERIFY(m_carousel->isAutoPlay());
    QVERIFY(m_carousel->isLooping());
}

void FluentAutoCarouselTest::testAutoPlay() {
    QSignalSpy autoPlaySpy(m_carousel, &FluentAutoCarousel::autoPlayChanged);
    
    m_carousel->setAutoPlay(false);
    QVERIFY(!m_carousel->isAutoPlay());
    QCOMPARE(autoPlaySpy.count(), 1);
    
    m_carousel->setAutoPlay(true);
    QVERIFY(m_carousel->isAutoPlay());
    QCOMPARE(autoPlaySpy.count(), 2);
}

void FluentAutoCarouselTest::testAutoPlayInterval() {
    QSignalSpy intervalSpy(m_carousel, &FluentAutoCarousel::intervalChanged);
    
    m_carousel->setInterval(2000);
    QCOMPARE(m_carousel->interval(), 2000);
    QCOMPARE(intervalSpy.count(), 1);
}

void FluentAutoCarouselTest::testPauseOnHover() {
    QSignalSpy pauseHoverSpy(m_carousel, &FluentAutoCarousel::pauseOnHoverChanged);
    
    m_carousel->setPauseOnHover(false);
    QVERIFY(!m_carousel->pauseOnHover());
    QCOMPARE(pauseHoverSpy.count(), 1);
}

void FluentAutoCarouselTest::testNext() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentAutoCarousel::currentSlideChanged);
    
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

void FluentAutoCarouselTest::testPrevious() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentAutoCarousel::currentSlideChanged);
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    m_carousel->previous();
    QCOMPARE(m_carousel->currentSlide(), 2); // Should wrap to last slide
    QCOMPARE(currentSlideSpy.count(), 1);
}

void FluentAutoCarouselTest::testGoToSlide() {
    // Add test slides
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentAutoCarousel::currentSlideChanged);
    
    m_carousel->goToSlide(2);
    QCOMPARE(m_carousel->currentSlide(), 2);
    QCOMPARE(currentSlideSpy.count(), 1);
    
    // Test invalid index
    m_carousel->goToSlide(10);
    QCOMPARE(m_carousel->currentSlide(), 2); // Should remain unchanged
}

void FluentAutoCarouselTest::testAddSlide() {
    QSignalSpy slideCountSpy(m_carousel, &FluentAutoCarousel::slideCountChanged);
    
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    
    m_carousel->addSlide(slide1);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(slideCountSpy.count(), 1);
    
    m_carousel->addSlide(slide2);
    QCOMPARE(m_carousel->slideCount(), 2);
    QCOMPARE(slideCountSpy.count(), 2);
}

void FluentAutoCarouselTest::testRemoveSlide() {
    auto* slide1 = new QWidget();
    auto* slide2 = new QWidget();
    
    m_carousel->addSlide(slide1);
    m_carousel->addSlide(slide2);
    
    QSignalSpy slideCountSpy(m_carousel, &FluentAutoCarousel::slideCountChanged);
    
    m_carousel->removeSlide(0);
    QCOMPARE(m_carousel->slideCount(), 1);
    QCOMPARE(slideCountSpy.count(), 1);
}

void FluentAutoCarouselTest::testSlideCount() {
    QCOMPARE(m_carousel->slideCount(), 0);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 1);
    
    m_carousel->addSlide(new QWidget());
    QCOMPARE(m_carousel->slideCount(), 2);
}

void FluentAutoCarouselTest::testCurrentSlide() {
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QCOMPARE(m_carousel->currentSlide(), 0);
    
    m_carousel->goToSlide(1);
    QCOMPARE(m_carousel->currentSlide(), 1);
}

void FluentAutoCarouselTest::testPlayState() {
    QSignalSpy playStateSpy(m_carousel, &FluentAutoCarousel::playStateChanged);
    
    m_carousel->pause();
    QCOMPARE(m_carousel->playState(), FluentCarouselPlayState::Paused);
    QCOMPARE(playStateSpy.count(), 1);
    
    m_carousel->play();
    QCOMPARE(m_carousel->playState(), FluentCarouselPlayState::Playing);
    QCOMPARE(playStateSpy.count(), 2);
    
    m_carousel->stop();
    QCOMPARE(m_carousel->playState(), FluentCarouselPlayState::Stopped);
    QCOMPARE(playStateSpy.count(), 3);
}

void FluentAutoCarouselTest::testLooping() {
    QSignalSpy loopingSpy(m_carousel, &FluentAutoCarousel::loopingChanged);
    
    m_carousel->setLooping(false);
    QVERIFY(!m_carousel->isLooping());
    QCOMPARE(loopingSpy.count(), 1);
}

void FluentAutoCarouselTest::testSignalEmission() {
    QSignalSpy autoPlaySpy(m_carousel, &FluentAutoCarousel::autoPlayChanged);
    QSignalSpy intervalSpy(m_carousel, &FluentAutoCarousel::intervalChanged);
    QSignalSpy pauseHoverSpy(m_carousel, &FluentAutoCarousel::pauseOnHoverChanged);
    QSignalSpy currentSlideSpy(m_carousel, &FluentAutoCarousel::currentSlideChanged);
    QSignalSpy slideCountSpy(m_carousel, &FluentAutoCarousel::slideCountChanged);
    QSignalSpy playStateSpy(m_carousel, &FluentAutoCarousel::playStateChanged);
    QSignalSpy loopingSpy(m_carousel, &FluentAutoCarousel::loopingChanged);
    
    // Test all signals
    m_carousel->setAutoPlay(false);
    m_carousel->setInterval(2000);
    m_carousel->setPauseOnHover(false);
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->goToSlide(1);
    m_carousel->pause();
    m_carousel->setLooping(false);
    
    QCOMPARE(autoPlaySpy.count(), 1);
    QCOMPARE(intervalSpy.count(), 1);
    QCOMPARE(pauseHoverSpy.count(), 1);
    QCOMPARE(currentSlideSpy.count(), 1);
    QCOMPARE(slideCountSpy.count(), 2);
    QCOMPARE(playStateSpy.count(), 1);
    QCOMPARE(loopingSpy.count(), 1);
}

void FluentAutoCarouselTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    // Test hover pause
    m_carousel->setPauseOnHover(true);
    m_carousel->setAutoPlay(true);
    
    QTest::mouseMove(m_carousel);
    // Should pause on hover when pauseOnHover is true
}

void FluentAutoCarouselTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_carousel->setFocus();
    
    // Add slides for testing
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    m_carousel->addSlide(new QWidget());
    
    QSignalSpy currentSlideSpy(m_carousel, &FluentAutoCarousel::currentSlideChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_carousel, Qt::Key_Right);
    QCOMPARE(currentSlideSpy.count(), 1);
    QCOMPARE(m_carousel->currentSlide(), 1);
    
    QTest::keyClick(m_carousel, Qt::Key_Left);
    QCOMPARE(currentSlideSpy.count(), 2);
    QCOMPARE(m_carousel->currentSlide(), 0);
}

QTEST_MAIN(FluentAutoCarouselTest)
#include "FluentAutoCarouselTest.moc"
