// tests/ElaWidgetToolsOptimizationTest.cpp
// Comprehensive tests for ElaWidgetTools-inspired optimizations

#include <QApplication>
#include <QSignalSpy>
#include <QTimer>
#include <QtTest/QtTest>

// Test the enhanced components
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentEnhancedScrollBar.h"
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Core/FluentErrorBoundary.h"

using namespace FluentQt;

class ElaWidgetToolsOptimizationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Error Boundary Tests
    void testErrorBoundaryBasicFunctionality();
    void testErrorBoundaryTimeout();
    void testErrorBoundaryRetry();
    void testErrorBoundaryErrorTypes();

    // Enhanced ScrollBar Tests
    void testEnhancedScrollBarSmoothScrolling();
    void testEnhancedScrollBarExpandOnHover();
    void testEnhancedScrollBarSpeedLimit();

    // Enhanced Animation Tests
    void testSmoothScrollAnimation();
    void testExpandAnimation();
    void testEnhancedEasingCurves();

    // Loading Indicator Enhancement Tests
    void testLoadingIndicatorErrorBoundaryIntegration();
    void testLoadingIndicatorTimeout();

    // Component Enhancement Tests
    void testFluentComponentSmoothTransitions();
    void testFluentComponentTransitionDuration();

private:
    Core::FluentErrorBoundary* m_errorBoundary{nullptr};
    Components::FluentEnhancedScrollBar* m_scrollBar{nullptr};
    Components::FluentLoadingIndicator* m_loadingIndicator{nullptr};
    Core::FluentComponent* m_component{nullptr};
};

void ElaWidgetToolsOptimizationTest::initTestCase() {
    // Initialize test environment
    qDebug() << "Starting ElaWidgetTools optimization tests";
}

void ElaWidgetToolsOptimizationTest::cleanupTestCase() {
    qDebug() << "ElaWidgetTools optimization tests completed";
}

void ElaWidgetToolsOptimizationTest::init() {
    // Create fresh instances for each test
    m_errorBoundary = new Core::FluentErrorBoundary();
    m_scrollBar = new Components::FluentEnhancedScrollBar();
    m_loadingIndicator = new Components::FluentLoadingIndicator();
    m_component = new Core::FluentComponent();
}

void ElaWidgetToolsOptimizationTest::cleanup() {
    // Clean up after each test
    delete m_errorBoundary;
    delete m_scrollBar;
    delete m_loadingIndicator;
    delete m_component;

    m_errorBoundary = nullptr;
    m_scrollBar = nullptr;
    m_loadingIndicator = nullptr;
    m_component = nullptr;
}

void ElaWidgetToolsOptimizationTest::testErrorBoundaryBasicFunctionality() {
    QVERIFY(m_errorBoundary != nullptr);

    // Test initial state
    QVERIFY(!m_errorBoundary->hasError());
    QVERIFY(m_errorBoundary->errorMessage().isEmpty());
    QVERIFY(m_errorBoundary->showRetryButton());

    // Test error catching
    QSignalSpy errorSpy(m_errorBoundary,
                        &Core::FluentErrorBoundary::errorOccurred);
    QSignalSpy hasErrorSpy(m_errorBoundary,
                           &Core::FluentErrorBoundary::hasErrorChanged);

    m_errorBoundary->catchError(
        "Test error", Core::FluentErrorBoundary::ErrorType::ComponentError);

    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(hasErrorSpy.count(), 1);
    QVERIFY(m_errorBoundary->hasError());
    QCOMPARE(m_errorBoundary->errorMessage(), QString("Test error"));

    // Test error clearing
    QSignalSpy clearedSpy(m_errorBoundary,
                          &Core::FluentErrorBoundary::errorCleared);
    m_errorBoundary->clearError();

    QCOMPARE(clearedSpy.count(), 1);
    QVERIFY(!m_errorBoundary->hasError());
    QVERIFY(m_errorBoundary->errorMessage().isEmpty());
}

void ElaWidgetToolsOptimizationTest::testErrorBoundaryTimeout() {
    QVERIFY(m_errorBoundary != nullptr);

    // Set a short timeout for testing
    m_errorBoundary->setLoadingTimeout(100);  // 100ms

    QSignalSpy errorSpy(m_errorBoundary,
                        &Core::FluentErrorBoundary::errorOccurred);

    // Start timeout
    m_errorBoundary->startLoadingTimeout();

    // Wait for timeout to trigger
    QTest::qWait(150);

    QCOMPARE(errorSpy.count(), 1);
    QVERIFY(m_errorBoundary->hasError());
    QVERIFY(m_errorBoundary->errorMessage().contains("timed out"));
}

void ElaWidgetToolsOptimizationTest::testErrorBoundaryRetry() {
    QVERIFY(m_errorBoundary != nullptr);

    // Set up retry callback
    bool retryCallbackCalled = false;
    m_errorBoundary->setRetryCallback(
        [&retryCallbackCalled]() { retryCallbackCalled = true; });

    // Trigger error
    m_errorBoundary->catchError(
        "Test error", Core::FluentErrorBoundary::ErrorType::NetworkError);

    // Test retry signal
    QSignalSpy retrySpy(m_errorBoundary,
                        &Core::FluentErrorBoundary::retryRequested);

    // Simulate retry button click (this would normally be done through UI)
    emit m_errorBoundary->retryRequested();

    QCOMPARE(retrySpy.count(), 1);
    QVERIFY(retryCallbackCalled);
}

void ElaWidgetToolsOptimizationTest::testErrorBoundaryErrorTypes() {
    QVERIFY(m_errorBoundary != nullptr);

    // Test different error types
    QSignalSpy errorSpy(m_errorBoundary,
                        &Core::FluentErrorBoundary::errorOccurred);

    m_errorBoundary->catchError(
        "Network error", Core::FluentErrorBoundary::ErrorType::NetworkError);
    QCOMPARE(errorSpy.count(), 1);

    m_errorBoundary->clearError();

    m_errorBoundary->catchError(
        "Validation error",
        Core::FluentErrorBoundary::ErrorType::ValidationError);
    QCOMPARE(errorSpy.count(), 2);

    m_errorBoundary->clearError();

    m_errorBoundary->catchError(
        "Loading timeout",
        Core::FluentErrorBoundary::ErrorType::LoadingTimeout);
    QCOMPARE(errorSpy.count(), 3);
}

void ElaWidgetToolsOptimizationTest::testEnhancedScrollBarSmoothScrolling() {
    QVERIFY(m_scrollBar != nullptr);

    // Test smooth scrolling property
    QVERIFY(m_scrollBar->isSmoothScrolling());  // Should be enabled by default

    QSignalSpy smoothScrollingSpy(
        m_scrollBar,
        &Components::FluentEnhancedScrollBar::smoothScrollingChanged);

    m_scrollBar->setSmoothScrolling(false);
    QCOMPARE(smoothScrollingSpy.count(), 1);
    QVERIFY(!m_scrollBar->isSmoothScrolling());

    m_scrollBar->setSmoothScrolling(true);
    QCOMPARE(smoothScrollingSpy.count(), 2);
    QVERIFY(m_scrollBar->isSmoothScrolling());

    // Test smooth scroll methods
    m_scrollBar->setRange(0, 100);
    m_scrollBar->setValue(0);

    QSignalSpy finishedSpy(
        m_scrollBar,
        &Components::FluentEnhancedScrollBar::smoothScrollFinished);

    m_scrollBar->smoothScrollTo(50, 100);  // Scroll to middle in 100ms

    // Wait for animation to complete
    QTest::qWait(150);

    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(m_scrollBar->value(), 50);
}

void ElaWidgetToolsOptimizationTest::testEnhancedScrollBarExpandOnHover() {
    QVERIFY(m_scrollBar != nullptr);

    // Test expand on hover property
    QVERIFY(m_scrollBar->expandOnHover());  // Should be enabled by default

    QSignalSpy expandSpy(
        m_scrollBar,
        &Components::FluentEnhancedScrollBar::expandOnHoverChanged);

    m_scrollBar->setExpandOnHover(false);
    QCOMPARE(expandSpy.count(), 1);
    QVERIFY(!m_scrollBar->expandOnHover());

    m_scrollBar->setExpandOnHover(true);
    QCOMPARE(expandSpy.count(), 2);
    QVERIFY(m_scrollBar->expandOnHover());
}

void ElaWidgetToolsOptimizationTest::testEnhancedScrollBarSpeedLimit() {
    QVERIFY(m_scrollBar != nullptr);

    // Test speed limit property
    QCOMPARE(m_scrollBar->speedLimit(), 20.0);  // Default value

    QSignalSpy speedSpy(
        m_scrollBar, &Components::FluentEnhancedScrollBar::speedLimitChanged);

    m_scrollBar->setSpeedLimit(15.0);
    QCOMPARE(speedSpy.count(), 1);
    QCOMPARE(m_scrollBar->speedLimit(), 15.0);

    m_scrollBar->setSpeedLimit(25.0);
    QCOMPARE(speedSpy.count(), 2);
    QCOMPARE(m_scrollBar->speedLimit(), 25.0);
}

void ElaWidgetToolsOptimizationTest::testSmoothScrollAnimation() {
    // Test the new smooth scroll animation method
    auto* testWidget = new QWidget();
    testWidget->show();

    auto animation =
        Animation::FluentAnimator::smoothScroll(testWidget, 0, 100, 200);

    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 200);
    QCOMPARE(animation->startValue().toInt(), 0);
    QCOMPARE(animation->endValue().toInt(), 100);

    delete testWidget;
}

void ElaWidgetToolsOptimizationTest::testExpandAnimation() {
    // Test the new expand animation method
    auto* testWidget = new QWidget();
    testWidget->resize(100, 50);
    testWidget->show();

    auto expandAnimation =
        Animation::FluentAnimator::expandAnimation(testWidget, true, 300);

    QVERIFY(expandAnimation != nullptr);
    QCOMPARE(expandAnimation->duration(),
             350);  // Should be slightly longer for expand

    auto collapseAnimation =
        Animation::FluentAnimator::expandAnimation(testWidget, false, 300);

    QVERIFY(collapseAnimation != nullptr);
    QCOMPARE(collapseAnimation->duration(), 300);

    delete testWidget;
}

void ElaWidgetToolsOptimizationTest::testEnhancedEasingCurves() {
    // Test that enhanced animations use proper easing curves
    auto* testWidget = new QWidget();
    testWidget->show();

    auto animation = Animation::FluentAnimator::smoothScroll(
        testWidget, 0, 100, 200, Animation::FluentEasing::EaseOutSine);

    QVERIFY(animation != nullptr);
    // The easing curve should be set correctly (we can't easily test the exact
    // curve, but we can verify it's not linear)
    QVERIFY(animation->easingCurve().type() != QEasingCurve::Linear);

    delete testWidget;
}

void ElaWidgetToolsOptimizationTest::
    testLoadingIndicatorErrorBoundaryIntegration() {
    QVERIFY(m_loadingIndicator != nullptr);
    QVERIFY(m_errorBoundary != nullptr);

    // Test error boundary integration
    m_loadingIndicator->setErrorBoundary(m_errorBoundary);
    QCOMPARE(m_loadingIndicator->errorBoundary(), m_errorBoundary);

    // Test timeout setting
    m_loadingIndicator->setLoadingTimeout(5000);
    QCOMPARE(m_loadingIndicator->loadingTimeout(), 5000);
}

void ElaWidgetToolsOptimizationTest::testLoadingIndicatorTimeout() {
    QVERIFY(m_loadingIndicator != nullptr);
    QVERIFY(m_errorBoundary != nullptr);

    // Set up error boundary with short timeout
    m_loadingIndicator->setErrorBoundary(m_errorBoundary);
    m_loadingIndicator->setLoadingTimeout(100);  // 100ms

    QSignalSpy errorSpy(m_errorBoundary,
                        &Core::FluentErrorBoundary::errorOccurred);

    // Start loading
    m_loadingIndicator->start();

    // Wait for timeout
    QTest::qWait(150);

    // Error boundary should have caught the timeout
    QCOMPARE(errorSpy.count(), 1);
    QVERIFY(m_errorBoundary->hasError());
}

void ElaWidgetToolsOptimizationTest::testFluentComponentSmoothTransitions() {
    QVERIFY(m_component != nullptr);

    // Test smooth transitions property
    QVERIFY(m_component->smoothTransitions());  // Should be enabled by default

    QSignalSpy transitionSpy(m_component,
                             &Core::FluentComponent::smoothTransitionsChanged);

    m_component->setSmoothTransitions(false);
    QCOMPARE(transitionSpy.count(), 1);
    QVERIFY(!m_component->smoothTransitions());

    m_component->setSmoothTransitions(true);
    QCOMPARE(transitionSpy.count(), 2);
    QVERIFY(m_component->smoothTransitions());
}

void ElaWidgetToolsOptimizationTest::testFluentComponentTransitionDuration() {
    QVERIFY(m_component != nullptr);

    // Test transition duration property
    QCOMPARE(m_component->transitionDuration(), 200);  // Default value

    QSignalSpy durationSpy(m_component,
                           &Core::FluentComponent::transitionDurationChanged);

    m_component->setTransitionDuration(300);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(m_component->transitionDuration(), 300);

    m_component->setTransitionDuration(150);
    QCOMPARE(durationSpy.count(), 2);
    QCOMPARE(m_component->transitionDuration(), 150);
}

QTEST_MAIN(ElaWidgetToolsOptimizationTest)
#include "ElaWidgetToolsOptimizationTest.moc"
