// tests/FluentAnimatorTestFixed.cpp
#include <QApplication>
#include <QPropertyAnimation>
#include <QWidget>
#include <QtTest/QtTest>
#include <memory>

#include "FluentQt/Animation/FluentAnimator.h"

using namespace FluentQt::Animation;

class FluentAnimatorTestFixed : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructor();
    void testFadeAnimations();
    void testSlideAnimations();
    void testScaleAnimations();
    void testRotationAnimations();
    void testAnimationConfig();

private:
    QWidget* m_testWidget;
    FluentAnimator* m_animator;
};

void FluentAnimatorTestFixed::initTestCase() {
    // Initialize test environment
    m_testWidget = nullptr;
    m_animator = nullptr;
}

void FluentAnimatorTestFixed::cleanupTestCase() {
    // Cleanup test environment
}

void FluentAnimatorTestFixed::init() {
    // Setup for each test
    m_testWidget = new QWidget();
    m_testWidget->resize(100, 100);
    m_testWidget->show();

    m_animator = new FluentAnimator(this);
}

void FluentAnimatorTestFixed::cleanup() {
    // Cleanup after each test
    delete m_testWidget;
    m_testWidget = nullptr;

    delete m_animator;
    m_animator = nullptr;
}

void FluentAnimatorTestFixed::testConstructor() {
    // Test default constructor
    auto* animator = new FluentAnimator();
    QVERIFY(animator != nullptr);
    delete animator;

    // Test constructor with parent
    auto* animator2 = new FluentAnimator(m_testWidget);
    QVERIFY(animator2->parent() == m_testWidget);
    delete animator2;
}

void FluentAnimatorTestFixed::testFadeAnimations() {
    // Test fade in animation with config
    auto fadeInAnimation = FluentAnimator::fadeIn(m_testWidget);
    QVERIFY(fadeInAnimation != nullptr);

    // The animation targets the QGraphicsOpacityEffect, not the widget directly
    auto* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(fadeInAnimation->targetObject());
    QVERIFY(opacityEffect != nullptr);
    QCOMPARE(fadeInAnimation->propertyName(), QByteArray("opacity"));

    // Test fade in with duration and easing
    auto fadeInAnimation2 =
        FluentAnimator::fadeIn(m_testWidget, 500, FluentEasing::EaseOut);
    QVERIFY(fadeInAnimation2 != nullptr);
    QCOMPARE(fadeInAnimation2->duration(), 500);

    // Test fade out animation
    auto fadeOutAnimation = FluentAnimator::fadeOut(m_testWidget);
    QVERIFY(fadeOutAnimation != nullptr);

    // The animation targets the QGraphicsOpacityEffect, not the widget directly
    auto* opacityEffect2 =
        qobject_cast<QGraphicsOpacityEffect*>(fadeOutAnimation->targetObject());
    QVERIFY(opacityEffect2 != nullptr);
    QCOMPARE(fadeOutAnimation->propertyName(), QByteArray("opacity"));
}

void FluentAnimatorTestFixed::testSlideAnimations() {
    // Test slide animations
    auto slideUpAnimation = FluentAnimator::slideUp(m_testWidget);
    QVERIFY(slideUpAnimation != nullptr);

    auto slideDownAnimation = FluentAnimator::slideDown(m_testWidget);
    QVERIFY(slideDownAnimation != nullptr);

    auto slideLeftAnimation = FluentAnimator::slideLeft(m_testWidget);
    QVERIFY(slideLeftAnimation != nullptr);

    auto slideRightAnimation = FluentAnimator::slideRight(m_testWidget);
    QVERIFY(slideRightAnimation != nullptr);
}

void FluentAnimatorTestFixed::testScaleAnimations() {
    // Test scale animations
    auto scaleInAnimation = FluentAnimator::scaleIn(m_testWidget);
    QVERIFY(scaleInAnimation != nullptr);

    auto scaleOutAnimation = FluentAnimator::scaleOut(m_testWidget);
    QVERIFY(scaleOutAnimation != nullptr);
}

void FluentAnimatorTestFixed::testRotationAnimations() {
    // Test rotation animations
    auto rotateInAnimation = FluentAnimator::rotateIn(m_testWidget);
    QVERIFY(rotateInAnimation != nullptr);

    auto rotateOutAnimation = FluentAnimator::rotateOut(m_testWidget);
    QVERIFY(rotateOutAnimation != nullptr);
}

void FluentAnimatorTestFixed::testAnimationConfig() {
    // Test animation configuration
    FluentAnimationConfig config;
    config.duration = std::chrono::milliseconds(1000);
    config.easing = FluentEasing::EaseInOut;
    config.delay = std::chrono::milliseconds(100);
    config.loops = 2;
    config.reverseOnComplete = true;
    config.autoStart = false;

    auto animation = FluentAnimator::fadeIn(m_testWidget, config);
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 1000);
    QCOMPARE(animation->loopCount(), 2);
}

QTEST_MAIN(FluentAnimatorTestFixed)
#include "FluentAnimatorTestFixed.moc"
