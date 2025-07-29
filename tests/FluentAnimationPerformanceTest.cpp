// tests/FluentAnimationPerformanceTest.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPropertyAnimation>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Core/FluentPerformance.h"

using namespace FluentQt;

class FluentAnimationPerformanceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Initialize test environment
        m_testWidget = new QWidget;
        m_testWidget->resize(400, 300);
        m_testWidget->show();
        
        // Wait for widget to be fully rendered
        QTest::qWaitForWindowExposed(m_testWidget);
    }
    
    void cleanupTestCase() {
        delete m_testWidget;
    }
    
    void testAnimationTiming() {
        // Test that animations complete within expected timeframes
        auto* button = new Components::FluentButton("Test", m_testWidget);
        button->show();
        
        QElapsedTimer timer;
        timer.start();
        
        Animation::FluentAnimationConfig config;
        config.duration = std::chrono::milliseconds(150);
        
        auto animation = Animation::FluentAnimator::hoverEffect(button, config);
        
        QSignalSpy finishedSpy(animation.get(), &QPropertyAnimation::finished);
        animation->start();
        
        // Wait for animation to complete
        QVERIFY(finishedSpy.wait(300)); // Allow some tolerance
        
        qint64 elapsed = timer.elapsed();
        
        // Verify timing is within acceptable range (±50ms tolerance)
        QVERIFY(elapsed >= 100 && elapsed <= 200);
        
        delete button;
    }
    
    void testAnimationPerformance() {
        // Test multiple simultaneous animations
        QList<Components::FluentButton*> buttons;
        QList<std::unique_ptr<QPropertyAnimation>> animations;
        
        const int buttonCount = 20;
        
        // Create multiple buttons
        for (int i = 0; i < buttonCount; ++i) {
            auto* button = new Components::FluentButton(QString("Button %1").arg(i), m_testWidget);
            button->move(i * 20, i * 15);
            button->show();
            buttons.append(button);
        }
        
        QElapsedTimer timer;
        timer.start();
        
        // Start animations simultaneously
        for (auto* button : buttons) {
            Animation::FluentAnimationConfig config;
            config.duration = std::chrono::milliseconds(300);
            config.useHardwareAcceleration = true;
            
            auto animation = Animation::FluentAnimator::scaleIn(button, config);
            animations.append(std::move(animation));
        }
        
        // Start all animations
        for (auto& animation : animations) {
            animation->start();
        }
        
        // Wait for all animations to complete
        QTimer::singleShot(500, [&]() {
            qint64 elapsed = timer.elapsed();
            
            // Performance test: should complete within reasonable time
            QVERIFY(elapsed < 600); // Should not take much longer than animation duration
            
            // Cleanup
            qDeleteAll(buttons);
        });
        
        QTest::qWait(600);
    }
    
    void testReducedMotionCompliance() {
        auto* button = new Components::FluentButton("Test", m_testWidget);
        button->show();
        
        Animation::FluentAnimationConfig config;
        config.duration = std::chrono::milliseconds(300);
        config.respectReducedMotion = true;
        
        // Mock reduced motion preference
        // In a real implementation, this would check system settings
        bool originalReducedMotion = Animation::FluentAnimator::shouldRespectReducedMotion();
        
        // Test with reduced motion enabled
        if (originalReducedMotion) {
            auto animation = Animation::FluentAnimator::fadeIn(button, config);
            
            // Animation should have zero duration when reduced motion is enabled
            QCOMPARE(animation->duration(), 0);
        }
        
        delete button;
    }
    
    void testEasingCurveCompliance() {
        // Test that easing curves match Fluent Design standards
        auto* button = new Components::FluentButton("Test", m_testWidget);
        button->show();
        
        Animation::FluentAnimationConfig config;
        config.easing = Animation::FluentEasing::EaseOutCubic;
        
        auto animation = Animation::FluentAnimator::hoverEffect(button, config);
        
        // Verify the easing curve is correctly applied
        QEasingCurve expectedCurve(QEasingCurve::OutCubic);
        QCOMPARE(animation->easingCurve().type(), expectedCurve.type());
        
        delete button;
    }
    
    void testAnimationCleanup() {
        // Test that animations properly clean up resources
        auto* button = new Components::FluentButton("Test", m_testWidget);
        button->show();
        
        Animation::FluentAnimationConfig config;
        config.duration = std::chrono::milliseconds(100);
        
        auto animation = Animation::FluentAnimator::fadeIn(button, config);
        
        QSignalSpy finishedSpy(animation.get(), &QPropertyAnimation::finished);
        animation->start();
        
        // Wait for animation to complete
        QVERIFY(finishedSpy.wait(200));
        
        // Verify that graphics effects are cleaned up
        QVERIFY(button->graphicsEffect() == nullptr);
        
        delete button;
    }
    
    void testRippleEffect() {
        auto* button = new Components::FluentButton("Test", m_testWidget);
        button->resize(100, 40);
        button->show();
        
        QElapsedTimer timer;
        timer.start();
        
        auto rippleAnimation = Animation::FluentAnimator::rippleEffect(
            button, button->rect().center());
        
        QSignalSpy finishedSpy(rippleAnimation.get(), &QSequentialAnimationGroup::finished);
        rippleAnimation->start();
        
        // Wait for ripple animation to complete
        QVERIFY(finishedSpy.wait(800)); // Ripple takes ~600ms
        
        qint64 elapsed = timer.elapsed();
        
        // Verify ripple timing
        QVERIFY(elapsed >= 500 && elapsed <= 700);
        
        delete button;
    }
    
    void benchmarkAnimationThroughput() {
        // Benchmark how many animations can be handled simultaneously
        const int maxAnimations = 100;
        QList<Components::FluentCard*> cards;
        QList<std::unique_ptr<QPropertyAnimation>> animations;
        
        QElapsedTimer timer;
        timer.start();
        
        // Create and animate many cards
        for (int i = 0; i < maxAnimations; ++i) {
            auto* card = new Components::FluentCard(m_testWidget);
            card->setTitle(QString("Card %1").arg(i));
            card->move(i % 10 * 80, i / 10 * 60);
            card->resize(70, 50);
            card->show();
            cards.append(card);
            
            Animation::FluentAnimationConfig config;
            config.duration = std::chrono::milliseconds(200);
            config.useHardwareAcceleration = true;
            
            auto animation = Animation::FluentAnimator::fadeIn(card, config);
            animations.append(std::move(animation));
        }
        
        // Start all animations
        for (auto& animation : animations) {
            animation->start();
        }
        
        // Measure setup time
        qint64 setupTime = timer.elapsed();
        
        // Wait for animations to complete
        QTest::qWait(400);
        
        qint64 totalTime = timer.elapsed();
        
        // Performance assertions
        QVERIFY(setupTime < 100); // Setup should be fast
        QVERIFY(totalTime < 500); // Total time should be reasonable
        
        // Cleanup
        qDeleteAll(cards);
        
        qDebug() << "Animation throughput test:";
        qDebug() << "  Animations:" << maxAnimations;
        qDebug() << "  Setup time:" << setupTime << "ms";
        qDebug() << "  Total time:" << totalTime << "ms";
        qDebug() << "  Avg per animation:" << (double)totalTime / maxAnimations << "ms";
    }

private:
    QWidget* m_testWidget = nullptr;
};

QTEST_MAIN(FluentAnimationPerformanceTest)
#include "FluentAnimationPerformanceTest.moc"
