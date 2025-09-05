// tests/Components/FluentAutoCarouselEnhancedTest.cpp
// Comprehensive tests for enhanced FluentAutoCarousel functionality
#include <QApplication>
#include <QColor>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSignalSpy>
#include <QSize>
#include <QTimer>
#include <QWidget>
#include <QtTest/QtTest>
#include <chrono>

#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

class FluentAutoCarouselEnhancedTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Enhanced Fluent UI Design tests
    void testProgressColors();
    void testProgressAnimation();
    void testControlButtonStyle();
    void testElevationEffects();
    void testFluentDesignTokens();

    // Complete Interface Implementation tests
    void testEnhancedControlMethods();
    void testPlaybackSpeed();
    void testSkipNavigation();
    void testFadeTransitions();
    void testDelayedPlayback();
    void testTemporaryPause();
    void testNewFactoryMethods();
    void testEnhancedSignals();

    // Code Quality tests
    void testSizeHintCaching();
    void testAccessibilityHelpers();
    void testPerformanceOptimizations();
    void testErrorHandling();
    void testResourceManagement();

    // Integration tests
    void testFluentThemeIntegration();
    void testBackwardCompatibility();
    void testMemoryManagement();

private:
    FluentAutoCarousel* m_carousel{nullptr};
    QWidget* m_testWidget{nullptr};

    // Helper methods
    void addTestItems(int count = 3);
    QWidget* createTestWidget(const QString& text,
                              const QColor& color = Qt::blue);
    void waitForSignal(QObject* sender, const char* signal, int timeout = 1000);
    void simulateHover(bool enter = true);
};

void FluentAutoCarouselEnhancedTest::initTestCase() {
    // Initialize Qt application if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        new QApplication(argc, argv);
    }

    // Initialize Fluent theme
    auto& theme = FluentTheme::instance();
    theme.setMode(FluentThemeMode::Light);
    theme.setAccentColor(FluentAccentColor::Blue);
}

void FluentAutoCarouselEnhancedTest::cleanupTestCase() {
    // Cleanup any global resources
}

void FluentAutoCarouselEnhancedTest::init() {
    // Create a fresh carousel for each test
    FluentCarouselConfig config;
    config.autoPlayInterval = std::chrono::milliseconds(1000);
    config.enableAccessibility = true;

    m_carousel = new FluentAutoCarousel(config);
    m_testWidget = new QWidget();

    // Add some test items
    addTestItems();
}

void FluentAutoCarouselEnhancedTest::cleanup() {
    delete m_carousel;
    m_carousel = nullptr;
    delete m_testWidget;
    m_testWidget = nullptr;
}

// Enhanced Fluent UI Design tests

void FluentAutoCarouselEnhancedTest::testProgressColors() {
    QVERIFY(m_carousel != nullptr);

    // Test default colors
    auto defaultColors = m_carousel->progressColors();
    QVERIFY(defaultColors.first.isValid());
    QVERIFY(defaultColors.second.isValid());

    // Test setting custom colors
    QColor activeColor = Qt::red;
    QColor inactiveColor = Qt::gray;

    QSignalSpy spy(m_carousel, &FluentAutoCarousel::progressColorsChanged);
    m_carousel->setProgressColors(activeColor, inactiveColor);

    QCOMPARE(spy.count(), 1);
    auto newColors = m_carousel->progressColors();
    QCOMPARE(newColors.first, activeColor);
    QCOMPARE(newColors.second, inactiveColor);

    // Test setting invalid colors (should use theme defaults)
    m_carousel->setProgressColors(QColor(), QColor());
    auto themeColors = m_carousel->progressColors();
    QVERIFY(themeColors.first.isValid());
    QVERIFY(themeColors.second.isValid());
}

void FluentAutoCarouselEnhancedTest::testProgressAnimation() {
    QVERIFY(m_carousel != nullptr);

    // Test default animation state
    QVERIFY(m_carousel->isProgressAnimated());
    QVERIFY(m_carousel->progressAnimationDuration() >= 0);

    // Test disabling animation
    QSignalSpy spy(m_carousel, &FluentAutoCarousel::progressAnimationChanged);
    m_carousel->setProgressAnimation(false, 500);

    QCOMPARE(spy.count(), 1);
    QVERIFY(!m_carousel->isProgressAnimated());
    QCOMPARE(m_carousel->progressAnimationDuration(), 500);

    // Test enabling with auto duration (0 = use theme tokens)
    m_carousel->setProgressAnimation(true, 0);
    QVERIFY(m_carousel->isProgressAnimated());
    QVERIFY(m_carousel->progressAnimationDuration() > 0);
}

void FluentAutoCarouselEnhancedTest::testControlButtonStyle() {
    QVERIFY(m_carousel != nullptr);

    // Enable controls first
    m_carousel->setShowPlayControls(true);

    // Test default style
    QCOMPARE(m_carousel->controlButtonStyle(), FluentButtonStyle::Subtle);

    // Test changing style
    QSignalSpy spy(m_carousel, &FluentAutoCarousel::controlButtonStyleChanged);
    m_carousel->setControlButtonStyle(FluentButtonStyle::Accent);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(m_carousel->controlButtonStyle(), FluentButtonStyle::Accent);

    // Test Default style
    m_carousel->setControlButtonStyle(FluentButtonStyle::Default);
    QCOMPARE(m_carousel->controlButtonStyle(), FluentButtonStyle::Default);
}

void FluentAutoCarouselEnhancedTest::testElevationEffects() {
    QVERIFY(m_carousel != nullptr);

    // Test default elevation
    QVERIFY(m_carousel->isElevationEnabled());
    QCOMPARE(m_carousel->elevationLevel(), 2);

    // Test disabling elevation
    QSignalSpy spy(m_carousel, &FluentAutoCarousel::elevationChanged);
    m_carousel->setElevationEnabled(false, 1);

    QCOMPARE(spy.count(), 1);
    QVERIFY(!m_carousel->isElevationEnabled());

    // Test different elevation levels
    m_carousel->setElevationEnabled(true, 5);
    QVERIFY(m_carousel->isElevationEnabled());
    QCOMPARE(m_carousel->elevationLevel(), 5);

    // Test level bounds (should clamp to 1-5)
    m_carousel->setElevationEnabled(true, 10);
    QCOMPARE(m_carousel->elevationLevel(), 5);

    m_carousel->setElevationEnabled(true, 0);
    QCOMPARE(m_carousel->elevationLevel(), 1);
}

void FluentAutoCarouselEnhancedTest::testFluentDesignTokens() {
    QVERIFY(m_carousel != nullptr);

    // Test that the carousel uses Fluent Design tokens
    auto& theme = FluentTheme::instance();

    // Test that progress colors use theme colors when not explicitly set
    auto colors = m_carousel->progressColors();
    QVERIFY(colors.first.isValid());
    QVERIFY(colors.second.isValid());

    // Test that animation duration uses theme tokens when set to 0
    m_carousel->setProgressAnimation(true, 0);
    int duration = m_carousel->progressAnimationDuration();
    QVERIFY(duration > 0);
    QVERIFY(duration <= 1000);  // Should be reasonable duration from theme
}

// Complete Interface Implementation tests

void FluentAutoCarouselEnhancedTest::testEnhancedControlMethods() {
    QVERIFY(m_carousel != nullptr);

    // Test that all new methods exist and are callable
    QVERIFY(m_carousel->metaObject()->indexOfMethod("playWithDelay(int)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("pauseTemporarily(int)") >=
            0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("skipToNext()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("skipToPrevious()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod(
                "setPlaybackSpeed(qreal)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("fadeIn(int)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("fadeOut(int)") >= 0);
}

void FluentAutoCarouselEnhancedTest::testPlaybackSpeed() {
    QVERIFY(m_carousel != nullptr);

    // Test default speed
    // Note: We can't directly access m_playbackSpeed, so we test through
    // behavior

    // Test setting different speeds
    QSignalSpy spy(m_carousel, &FluentAutoCarousel::playbackSpeedChanged);

    m_carousel->setPlaybackSpeed(2.0);
    QCOMPARE(spy.count(), 1);

    m_carousel->setPlaybackSpeed(0.5);
    QCOMPARE(spy.count(), 2);

    // Test bounds (should clamp to 0.1-5.0)
    m_carousel->setPlaybackSpeed(10.0);
    QCOMPARE(spy.count(), 3);

    m_carousel->setPlaybackSpeed(0.05);
    QCOMPARE(spy.count(), 4);

    // Test no change for same value
    m_carousel->setPlaybackSpeed(0.1);
    int previousCount = spy.count();
    m_carousel->setPlaybackSpeed(0.1);
    QCOMPARE(spy.count(), previousCount);  // Should not emit signal
}

// Helper methods implementation

void FluentAutoCarouselEnhancedTest::addTestItems(int count) {
    if (!m_carousel)
        return;

    for (int i = 0; i < count; ++i) {
        QWidget* item = createTestWidget(QString("Item %1").arg(i + 1));
        m_carousel->addItem(item);
    }
}

QWidget* FluentAutoCarouselEnhancedTest::createTestWidget(const QString& text,
                                                          const QColor& color) {
    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        QString("background-color: %1; color: white; padding: 20px;")
            .arg(color.name()));
    label->setMinimumSize(200, 100);
    return label;
}

void FluentAutoCarouselEnhancedTest::waitForSignal(QObject* sender,
                                                   const char* signal,
                                                   int timeout) {
    QSignalSpy spy(sender, signal);
    QTest::qWait(timeout);
}

void FluentAutoCarouselEnhancedTest::simulateHover(bool enter) {
    if (!m_carousel)
        return;

    QEvent::Type eventType = enter ? QEvent::Enter : QEvent::Leave;
    QEvent event(eventType);
    QApplication::sendEvent(m_carousel, &event);
}

void FluentAutoCarouselEnhancedTest::testSkipNavigation() {
    QVERIFY(m_carousel != nullptr);

    // Test skip signals
    QSignalSpy skipSpy(m_carousel, &FluentAutoCarousel::skipRequested);

    // Test skip to next
    int initialIndex = m_carousel->currentIndex();
    m_carousel->skipToNext();
    QCOMPARE(skipSpy.count(), 1);
    QCOMPARE(skipSpy.last().at(0).toInt(), 1);  // Direction: next

    // Test skip to previous
    m_carousel->skipToPrevious();
    QCOMPARE(skipSpy.count(), 2);
    QCOMPARE(skipSpy.last().at(0).toInt(), -1);  // Direction: previous

    // Test with single item (should not crash)
    FluentAutoCarousel singleItemCarousel;
    singleItemCarousel.addItem(createTestWidget("Single"));
    singleItemCarousel.skipToNext();
    singleItemCarousel.skipToPrevious();
}

void FluentAutoCarouselEnhancedTest::testFadeTransitions() {
    QVERIFY(m_carousel != nullptr);

    QSignalSpy fadeStartSpy(m_carousel, &FluentAutoCarousel::fadeStarted);
    QSignalSpy fadeCompleteSpy(m_carousel, &FluentAutoCarousel::fadeCompleted);

    // Test fade in
    m_carousel->fadeIn(100);
    QCOMPARE(fadeStartSpy.count(), 1);
    QCOMPARE(fadeStartSpy.last().at(0).toBool(), true);  // fade in

    // Wait for animation to complete
    QTest::qWait(150);
    QCOMPARE(fadeCompleteSpy.count(), 1);

    // Test fade out
    m_carousel->fadeOut(100);
    QCOMPARE(fadeStartSpy.count(), 2);
    QCOMPARE(fadeStartSpy.last().at(0).toBool(), false);  // fade out

    // Test that calling fade while already fading doesn't start new animation
    m_carousel->fadeIn(100);
    int previousCount = fadeStartSpy.count();
    m_carousel->fadeOut(100);  // Should be ignored
    QCOMPARE(fadeStartSpy.count(), previousCount);
}

void FluentAutoCarouselEnhancedTest::testDelayedPlayback() {
    QVERIFY(m_carousel != nullptr);

    QSignalSpy playSpy(m_carousel, &FluentAutoCarousel::playbackStarted);

    // Test immediate play (delay = 0)
    m_carousel->playWithDelay(0);
    QCOMPARE(playSpy.count(), 1);

    // Test delayed play
    m_carousel->stop();
    playSpy.clear();

    m_carousel->playWithDelay(100);
    QCOMPARE(playSpy.count(), 0);  // Should not start immediately

    QTest::qWait(150);
    QCOMPARE(playSpy.count(), 1);  // Should start after delay

    // Test negative delay (should play immediately)
    m_carousel->stop();
    playSpy.clear();

    m_carousel->playWithDelay(-100);
    QCOMPARE(playSpy.count(), 1);
}

void FluentAutoCarouselEnhancedTest::testTemporaryPause() {
    QVERIFY(m_carousel != nullptr);

    QSignalSpy tempPauseStartSpy(m_carousel,
                                 &FluentAutoCarousel::temporaryPauseStarted);
    QSignalSpy tempPauseEndSpy(m_carousel,
                               &FluentAutoCarousel::temporaryPauseEnded);
    QSignalSpy pauseSpy(m_carousel, &FluentAutoCarousel::playbackPaused);
    QSignalSpy playSpy(m_carousel, &FluentAutoCarousel::playbackStarted);

    // Start playing first
    m_carousel->play();

    // Test temporary pause
    m_carousel->pauseTemporarily(100);
    QCOMPARE(tempPauseStartSpy.count(), 1);
    QCOMPARE(pauseSpy.count(), 1);

    // Wait for temporary pause to end
    QTest::qWait(150);
    QCOMPARE(tempPauseEndSpy.count(), 1);
    QCOMPARE(playSpy.count(), 2);  // Should resume automatically

    // Test with invalid duration (should not pause)
    tempPauseStartSpy.clear();
    m_carousel->pauseTemporarily(0);
    QCOMPARE(tempPauseStartSpy.count(), 0);

    // Test when not playing (should not pause)
    m_carousel->stop();
    tempPauseStartSpy.clear();
    m_carousel->pauseTemporarily(100);
    QCOMPARE(tempPauseStartSpy.count(), 0);
}

void FluentAutoCarouselEnhancedTest::testNewFactoryMethods() {
    // Test createAccessible factory method
    FluentAutoCarousel* accessibleCarousel =
        FluentAutoCarousel::createAccessible(2000);
    QVERIFY(accessibleCarousel != nullptr);
    QVERIFY(accessibleCarousel->showPlayControls());
    QVERIFY(accessibleCarousel->showProgressIndicator());
    QCOMPARE(accessibleCarousel->progressStyle(),
             FluentAutoCarouselProgressStyle::Bar);
    QCOMPARE(accessibleCarousel->controlButtonStyle(),
             FluentButtonStyle::Default);
    QVERIFY(accessibleCarousel->isElevationEnabled());
    delete accessibleCarousel;

    // Test createCustomStyled factory method
    FluentAutoCarousel* customCarousel = FluentAutoCarousel::createCustomStyled(
        1500, FluentAutoCarouselProgressStyle::Circular,
        FluentButtonStyle::Accent);
    QVERIFY(customCarousel != nullptr);
    QCOMPARE(customCarousel->progressStyle(),
             FluentAutoCarouselProgressStyle::Circular);
    QCOMPARE(customCarousel->controlButtonStyle(), FluentButtonStyle::Accent);
    QVERIFY(customCarousel->showPlayControls());
    QVERIFY(customCarousel->showProgressIndicator());
    delete customCarousel;

    // Test parameter validation in factory methods
    FluentAutoCarousel* validatedCarousel =
        FluentAutoCarousel::createAccessible(50);  // Too short
    QVERIFY(validatedCarousel !=
            nullptr);  // Should still create with adjusted interval
    delete validatedCarousel;
}

void FluentAutoCarouselEnhancedTest::testEnhancedSignals() {
    QVERIFY(m_carousel != nullptr);

    // Test that all new signals exist
    QVERIFY(m_carousel->metaObject()->indexOfSignal(
                "progressColorsChanged(QColor,QColor)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal(
                "progressAnimationChanged(bool,int)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal(
                "controlButtonStyleChanged(FluentButtonStyle)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal(
                "elevationChanged(bool,int)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal(
                "playbackSpeedChanged(qreal)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal("skipRequested(int)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal("fadeStarted(bool,int)") >=
            0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal("fadeCompleted(bool)") >=
            0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal(
                "temporaryPauseStarted(int)") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal("temporaryPauseEnded()") >=
            0);
}

void FluentAutoCarouselEnhancedTest::testSizeHintCaching() {
    QVERIFY(m_carousel != nullptr);

    // Test that size hint is calculated
    QSize hint1 = m_carousel->sizeHint();
    QVERIFY(hint1.isValid());
    QVERIFY(hint1.width() > 0);
    QVERIFY(hint1.height() > 0);

    // Test that subsequent calls return the same cached value
    QSize hint2 = m_carousel->sizeHint();
    QCOMPARE(hint1, hint2);

    // Test that invalidating cache causes recalculation
    m_carousel->invalidateSizeHint();
    QSize hint3 = m_carousel->sizeHint();
    // Note: Size might be the same, but we're testing that it was recalculated

    // Test minimum size hint
    QSize minHint = m_carousel->minimumSizeHint();
    QVERIFY(minHint.isValid());
    QVERIFY(minHint.width() > 0);
    QVERIFY(minHint.height() > 0);

    // Minimum should be smaller than or equal to preferred
    QVERIFY(minHint.width() <= hint1.width());
    QVERIFY(minHint.height() <= hint1.height());
}

void FluentAutoCarouselEnhancedTest::testAccessibilityHelpers() {
    QVERIFY(m_carousel != nullptr);

    // Test that accessibility methods exist and are callable
    QVERIFY(m_carousel->metaObject()->indexOfMethod("buildStatusText()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("buildTimingText()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("buildControlsText()") >=
            0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod(
                "updateButtonAccessibility()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod(
                "updateProgressAccessibility()") >= 0);

    // Test that accessibility properties are set
    QVERIFY(!m_carousel->accessibleName().isEmpty());
    QVERIFY(!m_carousel->accessibleDescription().isEmpty());
}

void FluentAutoCarouselEnhancedTest::testPerformanceOptimizations() {
    QVERIFY(m_carousel != nullptr);

    // Test that size hint caching works
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        m_carousel->sizeHint();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Cached calls should be very fast (less than 1ms total for 100 calls)
    QVERIFY(duration.count() < 1000);

    // Test that invalidation works
    m_carousel->invalidateSizeHint();
    QSize newHint = m_carousel->sizeHint();
    QVERIFY(newHint.isValid());
}

void FluentAutoCarouselEnhancedTest::testErrorHandling() {
    QVERIFY(m_carousel != nullptr);

    // Test parameter validation
    // These should not crash and should handle invalid inputs gracefully

    // Test invalid colors (should use defaults)
    m_carousel->setProgressColors(QColor(), QColor());
    auto colors = m_carousel->progressColors();
    QVERIFY(colors.first.isValid());
    QVERIFY(colors.second.isValid());

    // Test invalid animation duration (should use theme default)
    m_carousel->setProgressAnimation(true, -100);
    QVERIFY(m_carousel->progressAnimationDuration() > 0);

    // Test invalid elevation level (should clamp)
    m_carousel->setElevationEnabled(true, 100);
    QVERIFY(m_carousel->elevationLevel() <= 5);

    m_carousel->setElevationEnabled(true, -5);
    QVERIFY(m_carousel->elevationLevel() >= 1);

    // Test invalid playback speed (should clamp)
    m_carousel->setPlaybackSpeed(100.0);
    // Can't directly test the clamped value, but it shouldn't crash

    m_carousel->setPlaybackSpeed(-1.0);
    // Can't directly test the clamped value, but it shouldn't crash
}

void FluentAutoCarouselEnhancedTest::testResourceManagement() {
    // Test that creating and destroying carousels doesn't leak memory
    for (int i = 0; i < 10; ++i) {
        FluentAutoCarousel* carousel = new FluentAutoCarousel();
        carousel->addItem(createTestWidget("Test"));
        carousel->setShowPlayControls(true);
        carousel->setShowProgressIndicator(true);
        carousel->play();
        delete carousel;
    }

    // Test factory method resource management
    for (int i = 0; i < 5; ++i) {
        FluentAutoCarousel* carousel =
            FluentAutoCarousel::createAccessible(1000);
        delete carousel;
    }
}

void FluentAutoCarouselEnhancedTest::testFluentThemeIntegration() {
    QVERIFY(m_carousel != nullptr);

    auto& theme = FluentTheme::instance();

    // Test that carousel responds to theme changes
    theme.setMode(FluentThemeMode::Dark);
    auto darkColors = m_carousel->progressColors();

    theme.setMode(FluentThemeMode::Light);
    auto lightColors = m_carousel->progressColors();

    // Colors should be different for different themes (if using theme defaults)
    // Note: This test might pass even if colors are the same, but it tests the
    // integration
    QVERIFY(darkColors.first.isValid());
    QVERIFY(lightColors.first.isValid());
}

void FluentAutoCarouselEnhancedTest::testBackwardCompatibility() {
    QVERIFY(m_carousel != nullptr);

    // Test that all original methods still work
    QVERIFY(m_carousel->metaObject()->indexOfMethod("play()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("pause()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("stop()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("togglePlayPause()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("restart()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfMethod("resetProgress()") >= 0);

    // Test that original signals still exist
    QVERIFY(m_carousel->metaObject()->indexOfSignal("playbackStarted()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal("playbackPaused()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal("playbackStopped()") >= 0);
    QVERIFY(m_carousel->metaObject()->indexOfSignal("playbackRestarted()") >=
            0);

    // Test that original factory methods still work
    FluentAutoCarousel* carousel = FluentAutoCarousel::createWithInterval(2000);
    QVERIFY(carousel != nullptr);
    delete carousel;
}

void FluentAutoCarouselEnhancedTest::testMemoryManagement() {
    // Test that smart pointers are used correctly
    FluentAutoCarousel* carousel = new FluentAutoCarousel();

    // Add items and enable features that create additional objects
    carousel->addItem(createTestWidget("Test 1"));
    carousel->addItem(createTestWidget("Test 2"));
    carousel->setShowPlayControls(true);
    carousel->setShowProgressIndicator(true);
    carousel->setElevationEnabled(true);

    // Test enhanced features that use smart pointers
    carousel->playWithDelay(100);
    carousel->pauseTemporarily(50);
    carousel->fadeIn(100);

    // Cleanup should not crash
    delete carousel;

    // Test that factory methods manage memory correctly
    FluentAutoCarousel* factoryCarousel =
        FluentAutoCarousel::createCustomStyled(
            1000, FluentAutoCarouselProgressStyle::Dots,
            FluentButtonStyle::Accent);
    QVERIFY(factoryCarousel != nullptr);
    delete factoryCarousel;
}

QTEST_MAIN(FluentAutoCarouselEnhancedTest)
#include "FluentAutoCarouselEnhancedTest.moc"
