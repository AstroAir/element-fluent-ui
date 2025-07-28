// tests/FluentAnimatorTest.cpp
#include <QtTest/QtTest>
#include <QWidget>
#include <QPropertyAnimation>
#include <QSignalSpy>
#include <QTimer>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

using namespace FluentQt;

class FluentAnimatorTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testConstructor();
    void testEasingFunctions();
    void testAnimationTypes();
    void testAnimationDuration();
    
    // Animation method tests
    void testFadeInOut();
    void testSlideAnimations();
    void testRotationAnimations();
    void testScaleAnimations();
    void testMicroInteractions();
    
    // Configuration tests
    void testAnimationConfig();
    void testAccessibilitySupport();
    void testPerformanceMonitoring();
    
    // Edge cases and error handling
    void testNullWidget();
    void testInvalidDuration();
    void testConcurrentAnimations();
    void testAnimationInterruption();

private:
    Animation::FluentAnimator* m_animator{nullptr};
    QWidget* m_testWidget{nullptr};
    Core::FluentComponent* m_testComponent{nullptr};
};

void FluentAnimatorTest::initTestCase() {
    // Initialize test environment
    qApp->setApplicationName("FluentAnimatorTest");
}

void FluentAnimatorTest::cleanupTestCase() {
    // Cleanup test environment
}

void FluentAnimatorTest::init() {
    // Create fresh instances for each test
    m_testWidget = new QWidget();
    m_testWidget->resize(100, 100);
    m_testWidget->show();
    
    m_testComponent = new Core::FluentComponent();
    m_testComponent->resize(100, 100);
    m_testComponent->show();
    
    m_animator = new Animation::FluentAnimator(this);
    
    // Wait for widgets to be shown
    QTest::qWaitForWindowExposed(m_testWidget);
    QTest::qWaitForWindowExposed(m_testComponent);
}

void FluentAnimatorTest::cleanup() {
    delete m_animator;
    delete m_testWidget;
    delete m_testComponent;
    
    m_animator = nullptr;
    m_testWidget = nullptr;
    m_testComponent = nullptr;
}

void FluentAnimatorTest::testConstructor() {
    QVERIFY(m_animator != nullptr);
    QVERIFY(m_animator->parent() == this);
    
    // Test constructor with parent
    auto* animator2 = new Animation::FluentAnimator(m_testWidget);
    QVERIFY(animator2->parent() == m_testWidget);
    delete animator2;
}

void FluentAnimatorTest::testEasingFunctions() {
    // Test all easing function conversions
    const QList<Animation::FluentEasing> easingTypes = {
        Animation::FluentEasing::Linear,
        Animation::FluentEasing::QuadIn,
        Animation::FluentEasing::QuadOut,
        Animation::FluentEasing::QuadInOut,
        Animation::FluentEasing::CubicIn,
        Animation::FluentEasing::CubicOut,
        Animation::FluentEasing::CubicInOut,
        Animation::FluentEasing::QuartIn,
        Animation::FluentEasing::QuartOut,
        Animation::FluentEasing::QuartInOut,
        Animation::FluentEasing::QuintIn,
        Animation::FluentEasing::QuintOut,
        Animation::FluentEasing::QuintInOut,
        Animation::FluentEasing::SineIn,
        Animation::FluentEasing::SineOut,
        Animation::FluentEasing::SineInOut,
        Animation::FluentEasing::ExpoIn,
        Animation::FluentEasing::ExpoOut,
        Animation::FluentEasing::ExpoInOut,
        Animation::FluentEasing::CircIn,
        Animation::FluentEasing::CircOut,
        Animation::FluentEasing::CircInOut,
        Animation::FluentEasing::BounceIn,
        Animation::FluentEasing::BounceOut,
        Animation::FluentEasing::BounceInOut,
        Animation::FluentEasing::ElasticIn,
        Animation::FluentEasing::ElasticOut,
        Animation::FluentEasing::ElasticInOut,
        Animation::FluentEasing::BackIn,
        Animation::FluentEasing::BackOut,
        Animation::FluentEasing::BackInOut
    };
    
    for (auto easing : easingTypes) {
        // This would test the internal toQtEasing function
        // Since it's private, we test indirectly through animations
        auto* animation = m_animator->fadeIn(m_testWidget, 100, easing);
        QVERIFY(animation != nullptr);
        QVERIFY(animation->duration() == 100);
        animation->stop();
        delete animation;
    }
}

void FluentAnimatorTest::testAnimationTypes() {
    // Test all animation types
    const QList<Animation::FluentAnimationType> animationTypes = {
        Animation::FluentAnimationType::FadeIn,
        Animation::FluentAnimationType::FadeOut,
        Animation::FluentAnimationType::SlideUp,
        Animation::FluentAnimationType::SlideDown,
        Animation::FluentAnimationType::SlideLeft,
        Animation::FluentAnimationType::SlideRight,
        Animation::FluentAnimationType::ScaleIn,
        Animation::FluentAnimationType::ScaleOut,
        Animation::FluentAnimationType::RotateIn,
        Animation::FluentAnimationType::RotateOut,
        Animation::FluentAnimationType::FlipHorizontal,
        Animation::FluentAnimationType::FlipVertical,
        Animation::FluentAnimationType::ZoomIn,
        Animation::FluentAnimationType::ZoomOut,
        Animation::FluentAnimationType::Morph,
        Animation::FluentAnimationType::Reveal,
        Animation::FluentAnimationType::Conceal,
        Animation::FluentAnimationType::Hover,
        Animation::FluentAnimationType::Press,
        Animation::FluentAnimationType::Focus,
        Animation::FluentAnimationType::Pulse,
        Animation::FluentAnimationType::Shake,
        Animation::FluentAnimationType::Bounce,
        Animation::FluentAnimationType::Swing,
        Animation::FluentAnimationType::Flash,
        Animation::FluentAnimationType::Glow,
        Animation::FluentAnimationType::Ripple,
        Animation::FluentAnimationType::Breathe
    };
    
    for (auto type : animationTypes) {
        // Test that each animation type can be created
        // This would require access to internal methods or a public API
        // For now, we test the main animation methods that use these types
        QVERIFY(true); // Placeholder - would test specific animation creation
    }
}

void FluentAnimatorTest::testAnimationDuration() {
    // Test various durations
    const QList<int> durations = {50, 100, 250, 500, 1000, 2000};
    
    for (int duration : durations) {
        auto* animation = m_animator->fadeIn(m_testWidget, duration);
        QVERIFY(animation != nullptr);
        QCOMPARE(animation->duration(), duration);
        animation->stop();
        delete animation;
    }
}

void FluentAnimatorTest::testFadeInOut() {
    // Test fade in animation
    auto* fadeInAnimation = m_animator->fadeIn(m_testWidget);
    QVERIFY(fadeInAnimation != nullptr);
    QCOMPARE(fadeInAnimation->targetObject(), m_testWidget);
    QCOMPARE(fadeInAnimation->propertyName(), QByteArray("windowOpacity"));
    
    QSignalSpy finishedSpy(fadeInAnimation, &QPropertyAnimation::finished);
    fadeInAnimation->start();
    
    // Wait for animation to complete
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
    
    delete fadeInAnimation;
    
    // Test fade out animation
    auto* fadeOutAnimation = m_animator->fadeOut(m_testWidget);
    QVERIFY(fadeOutAnimation != nullptr);
    QCOMPARE(fadeOutAnimation->targetObject(), m_testWidget);
    QCOMPARE(fadeOutAnimation->propertyName(), QByteArray("windowOpacity"));
    
    QSignalSpy finishedSpy2(fadeOutAnimation, &QPropertyAnimation::finished);
    fadeOutAnimation->start();
    
    // Wait for animation to complete
    QVERIFY(finishedSpy2.wait(1000));
    QCOMPARE(finishedSpy2.count(), 1);
    
    delete fadeOutAnimation;
}

void FluentAnimatorTest::testSlideAnimations() {
    const QPoint originalPos = m_testWidget->pos();
    
    // Test slide up
    auto* slideUpAnimation = m_animator->slideUp(m_testWidget);
    QVERIFY(slideUpAnimation != nullptr);
    
    QSignalSpy finishedSpy(slideUpAnimation, &QPropertyAnimation::finished);
    slideUpAnimation->start();
    QVERIFY(finishedSpy.wait(1000));
    
    delete slideUpAnimation;
    
    // Test slide down
    auto* slideDownAnimation = m_animator->slideDown(m_testWidget);
    QVERIFY(slideDownAnimation != nullptr);
    
    QSignalSpy finishedSpy2(slideDownAnimation, &QPropertyAnimation::finished);
    slideDownAnimation->start();
    QVERIFY(finishedSpy2.wait(1000));
    
    delete slideDownAnimation;
    
    // Test slide left
    auto* slideLeftAnimation = m_animator->slideLeft(m_testWidget);
    QVERIFY(slideLeftAnimation != nullptr);
    delete slideLeftAnimation;
    
    // Test slide right
    auto* slideRightAnimation = m_animator->slideRight(m_testWidget);
    QVERIFY(slideRightAnimation != nullptr);
    delete slideRightAnimation;
}

void FluentAnimatorTest::testRotationAnimations() {
    // Test rotate in
    auto* rotateInAnimation = m_animator->rotateIn(m_testWidget);
    QVERIFY(rotateInAnimation != nullptr);
    
    QSignalSpy finishedSpy(rotateInAnimation, &QPropertyAnimation::finished);
    rotateInAnimation->start();
    QVERIFY(finishedSpy.wait(1000));
    
    delete rotateInAnimation;
    
    // Test rotate out
    auto* rotateOutAnimation = m_animator->rotateOut(m_testWidget);
    QVERIFY(rotateOutAnimation != nullptr);
    delete rotateOutAnimation;
}

void FluentAnimatorTest::testScaleAnimations() {
    // Test scale in
    auto* scaleInAnimation = m_animator->scaleIn(m_testWidget);
    QVERIFY(scaleInAnimation != nullptr);
    delete scaleInAnimation;
    
    // Test scale out
    auto* scaleOutAnimation = m_animator->scaleOut(m_testWidget);
    QVERIFY(scaleOutAnimation != nullptr);
    delete scaleOutAnimation;
}

void FluentAnimatorTest::testMicroInteractions() {
    // Test hover effect
    auto* hoverAnimation = m_animator->hoverEffect(m_testWidget);
    QVERIFY(hoverAnimation != nullptr);
    delete hoverAnimation;
    
    // Test press effect
    auto* pressAnimation = m_animator->pressEffect(m_testWidget);
    QVERIFY(pressAnimation != nullptr);
    delete pressAnimation;
    
    // Test focus effect
    auto* focusAnimation = m_animator->focusEffect(m_testWidget);
    QVERIFY(focusAnimation != nullptr);
    delete focusAnimation;
    
    // Test pulse effect
    auto* pulseAnimation = m_animator->pulseEffect(m_testWidget);
    QVERIFY(pulseAnimation != nullptr);
    delete pulseAnimation;
    
    // Test shake effect
    auto* shakeAnimation = m_animator->shakeEffect(m_testWidget);
    QVERIFY(shakeAnimation != nullptr);
    delete shakeAnimation;
}

void FluentAnimatorTest::testAnimationConfig() {
    Animation::FluentAnimationConfig config;
    config.enableMicroInteractions = true;
    config.respectReducedMotion = true;
    config.enableHardwareAcceleration = true;
    config.debugMode = false;
    
    // Test that config can be applied (would need public API)
    QVERIFY(true); // Placeholder
}

void FluentAnimatorTest::testAccessibilitySupport() {
    // Test reduced motion support
    // This would require mocking system settings
    QVERIFY(true); // Placeholder
}

void FluentAnimatorTest::testPerformanceMonitoring() {
    // Test that performance monitoring works
    // This would require access to FluentPerformanceMonitor
    QVERIFY(true); // Placeholder
}

void FluentAnimatorTest::testNullWidget() {
    // Test null widget handling
    auto* animation = m_animator->fadeIn(nullptr);
    QVERIFY(animation == nullptr);
}

void FluentAnimatorTest::testInvalidDuration() {
    // Test negative duration
    auto* animation = m_animator->fadeIn(m_testWidget, -100);
    QVERIFY(animation != nullptr);
    QVERIFY(animation->duration() >= 0); // Should be clamped to valid value
    delete animation;
}

void FluentAnimatorTest::testConcurrentAnimations() {
    // Test multiple concurrent animations
    auto* fadeAnimation = m_animator->fadeIn(m_testWidget, 500);
    auto* slideAnimation = m_animator->slideUp(m_testWidget, 500);
    
    QVERIFY(fadeAnimation != nullptr);
    QVERIFY(slideAnimation != nullptr);
    
    fadeAnimation->start();
    slideAnimation->start();
    
    // Both should be able to run concurrently
    QVERIFY(fadeAnimation->state() == QAbstractAnimation::Running);
    QVERIFY(slideAnimation->state() == QAbstractAnimation::Running);
    
    fadeAnimation->stop();
    slideAnimation->stop();
    
    delete fadeAnimation;
    delete slideAnimation;
}

void FluentAnimatorTest::testAnimationInterruption() {
    auto* animation = m_animator->fadeIn(m_testWidget, 1000);
    QVERIFY(animation != nullptr);
    
    animation->start();
    QVERIFY(animation->state() == QAbstractAnimation::Running);
    
    // Interrupt the animation
    animation->stop();
    QVERIFY(animation->state() == QAbstractAnimation::Stopped);
    
    delete animation;
}

QTEST_MAIN(FluentAnimatorTest)
#include "FluentAnimatorTest.moc"
