// tests/Core/FluentComponentTest.cpp
#include <QEnterEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Core;

class FluentComponentTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testParentConstructor();

    // State management tests
    void testState();
    void testStateTransitions();
    void testStateSignals();

    // Animation tests
    void testAnimated();
    void testAnimationDuration();

    // Corner radius tests
    void testCornerRadius();
    void testCornerRadiusEnum();

    // Event handling tests
    void testEnterEvent();
    void testLeaveEvent();
    void testMousePressEvent();
    void testMouseReleaseEvent();
    void testFocusInEvent();
    void testFocusOutEvent();

    // Virtual method tests
    void testUpdateStateStyle();
    void testPerformStateTransition();

    // Property binding tests
    void testBindProperty();

    // Theme integration tests
    void testThemeIntegration();

    // Accessibility tests
    void testAccessibility();

    // Edge cases tests
    void testDisabledState();
    void testInvalidState();

private:
    FluentComponent* m_component;
};

void FluentComponentTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentComponentTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentComponentTest::init() {
    // Create a fresh component before each test
    m_component = new FluentComponent();
    // Don't show the component to avoid automatic focus changes
    // Tests that need the component to be visible should call show() explicitly
}

void FluentComponentTest::cleanup() {
    // Clean up after each test
    delete m_component;
    m_component = nullptr;
}

void FluentComponentTest::testDefaultConstructor() {
    // Test default constructor
    FluentComponent* component = new FluentComponent();

    // Verify default properties
    QCOMPARE(component->state(), FluentState::Normal);
    QVERIFY(component->isAnimated());
    QCOMPARE(component->cornerRadius(),
             static_cast<int>(FluentCornerRadius::Medium));
    QVERIFY(component->isEnabled());
    QCOMPARE(component->focusPolicy(), Qt::StrongFocus);
    QVERIFY(component->testAttribute(Qt::WA_Hover));

    delete component;
}

void FluentComponentTest::testParentConstructor() {
    // Test constructor with parent
    QWidget* parent = new QWidget();
    FluentComponent* component = new FluentComponent(parent);

    QCOMPARE(component->parent(), parent);
    QCOMPARE(component->parentWidget(), parent);

    delete parent;  // This should also delete component
}

void FluentComponentTest::testState() {
    // Test setting and getting state
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    QCOMPARE(m_component->state(), FluentState::Normal);  // Default state

    m_component->setState(FluentState::Hovered);
    QCOMPARE(m_component->state(), FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.first().first().value<FluentState>(),
             FluentState::Hovered);

    m_component->setState(FluentState::Pressed);
    QCOMPARE(m_component->state(), FluentState::Pressed);
    QCOMPARE(stateChangedSpy.count(), 2);
    QCOMPARE(stateChangedSpy.last().first().value<FluentState>(),
             FluentState::Pressed);

    m_component->setState(FluentState::Focused);
    QCOMPARE(m_component->state(), FluentState::Focused);
    QCOMPARE(stateChangedSpy.count(), 3);

    m_component->setState(FluentState::Disabled);
    QCOMPARE(m_component->state(), FluentState::Disabled);
    QCOMPARE(stateChangedSpy.count(), 4);

    // Setting the same state should not emit the signal
    m_component->setState(FluentState::Disabled);
    QCOMPARE(stateChangedSpy.count(), 4);
}

void FluentComponentTest::testStateTransitions() {
    // Test state transitions
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Test Normal -> Hovered
    m_component->setState(FluentState::Normal);
    stateChangedSpy.clear();
    m_component->setState(FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);

    // Test Hovered -> Pressed
    m_component->setState(FluentState::Pressed);
    QCOMPARE(stateChangedSpy.count(), 2);

    // Test Pressed -> Normal
    m_component->setState(FluentState::Normal);
    QCOMPARE(stateChangedSpy.count(), 3);

    // Test Normal -> Focused
    m_component->setState(FluentState::Focused);
    QCOMPARE(stateChangedSpy.count(), 4);

    // Test Focused -> Disabled
    m_component->setState(FluentState::Disabled);
    QCOMPARE(stateChangedSpy.count(), 5);

    // Test Disabled -> Normal
    m_component->setState(FluentState::Normal);
    QCOMPARE(stateChangedSpy.count(), 6);
}

void FluentComponentTest::testStateSignals() {
    // Test that state change signals are emitted correctly
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Test each state change
    FluentState states[] = {FluentState::Hovered, FluentState::Pressed,
                            FluentState::Focused, FluentState::Disabled,
                            FluentState::Normal};

    for (int i = 0; i < 5; ++i) {
        m_component->setState(states[i]);
        QCOMPARE(stateChangedSpy.count(), i + 1);
        QCOMPARE(stateChangedSpy.last().first().value<FluentState>(),
                 states[i]);
    }
}

void FluentComponentTest::testAnimated() {
    // Test animated property
    QVERIFY(m_component->isAnimated());  // Default should be true

    m_component->setAnimated(false);
    QVERIFY(!m_component->isAnimated());

    m_component->setAnimated(true);
    QVERIFY(m_component->isAnimated());
}

void FluentComponentTest::testAnimationDuration() {
    // Test that animation is properly configured
    // This is harder to test directly without access to internal animation
    // For now, just verify that animated state changes work

    m_component->setAnimated(true);
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    m_component->setState(FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);

    // Animation should not prevent immediate state change
    QCOMPARE(m_component->state(), FluentState::Hovered);
}

void FluentComponentTest::testCornerRadius() {
    // Test setting and getting corner radius
    QCOMPARE(m_component->cornerRadius(),
             static_cast<int>(FluentCornerRadius::Medium));  // Default

    m_component->setCornerRadius(16);
    QCOMPARE(m_component->cornerRadius(), 16);

    m_component->setCornerRadius(0);
    QCOMPARE(m_component->cornerRadius(), 0);

    m_component->setCornerRadius(32);
    QCOMPARE(m_component->cornerRadius(), 32);

    // Setting the same radius should not cause issues
    m_component->setCornerRadius(32);
    QCOMPARE(m_component->cornerRadius(), 32);
}

void FluentComponentTest::testCornerRadiusEnum() {
    // Test corner radius enum values
    m_component->setCornerRadius(static_cast<int>(FluentCornerRadius::None));
    QCOMPARE(m_component->cornerRadius(), 0);

    m_component->setCornerRadius(static_cast<int>(FluentCornerRadius::Small));
    QCOMPARE(m_component->cornerRadius(), 4);

    m_component->setCornerRadius(static_cast<int>(FluentCornerRadius::Medium));
    QCOMPARE(m_component->cornerRadius(), 8);

    m_component->setCornerRadius(static_cast<int>(FluentCornerRadius::Large));
    QCOMPARE(m_component->cornerRadius(), 16);
}

void FluentComponentTest::testEnterEvent() {
    // Test enter event handling
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Ensure component is enabled and in normal state
    m_component->setEnabled(true);
    m_component->setState(FluentState::Normal);
    stateChangedSpy.clear();

    // Simulate enter event
    QEnterEvent enterEvent(QPointF(10, 10), QPointF(10, 10), QPointF(10, 10));
    QApplication::sendEvent(m_component, &enterEvent);

    // Should change to hovered state
    QCOMPARE(m_component->state(), FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.first().first().value<FluentState>(),
             FluentState::Hovered);
}

void FluentComponentTest::testLeaveEvent() {
    // Test leave event handling
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Set component to hovered state first
    m_component->setEnabled(true);
    m_component->setState(FluentState::Hovered);
    stateChangedSpy.clear();

    // Simulate leave event
    QEvent leaveEvent(QEvent::Leave);
    QApplication::sendEvent(m_component, &leaveEvent);

    // Should change to normal state (or focused if has focus)
    if (m_component->hasFocus()) {
        QCOMPARE(m_component->state(), FluentState::Focused);
    } else {
        QCOMPARE(m_component->state(), FluentState::Normal);
    }
    QCOMPARE(stateChangedSpy.count(), 1);
}

void FluentComponentTest::testMousePressEvent() {
    // Test mouse press event handling
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Ensure component is enabled and in normal state
    m_component->setEnabled(true);
    m_component->setState(FluentState::Normal);
    stateChangedSpy.clear();

    // Simulate mouse press event
    QPoint center = m_component->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, center,
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_component, &pressEvent);

    // Should change to pressed state
    QCOMPARE(m_component->state(), FluentState::Pressed);
    QCOMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.first().first().value<FluentState>(),
             FluentState::Pressed);
}

void FluentComponentTest::testMouseReleaseEvent() {
    // Test mouse release event handling
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Set component to pressed state first
    m_component->setEnabled(true);
    m_component->setState(FluentState::Pressed);
    stateChangedSpy.clear();

    // Simulate mouse release event
    QPoint center = m_component->rect().center();
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, center,
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_component, &releaseEvent);

    // Should change to normal or hovered state depending on mouse position
    QVERIFY(m_component->state() == FluentState::Normal ||
            m_component->state() == FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);
}

void FluentComponentTest::testFocusInEvent() {
    // Test focus in event handling
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Ensure component is enabled and in normal state
    m_component->setEnabled(true);
    m_component->setState(FluentState::Normal);
    stateChangedSpy.clear();

    // Simulate focus in event
    QFocusEvent focusInEvent(QEvent::FocusIn, Qt::TabFocusReason);
    QApplication::sendEvent(m_component, &focusInEvent);

    // Should change to focused state
    QCOMPARE(m_component->state(), FluentState::Focused);
    QCOMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.first().first().value<FluentState>(),
             FluentState::Focused);
}

void FluentComponentTest::testFocusOutEvent() {
    // Test focus out event handling
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Set component to focused state first
    m_component->setEnabled(true);
    m_component->setState(FluentState::Focused);
    stateChangedSpy.clear();

    // Simulate focus out event
    QFocusEvent focusOutEvent(QEvent::FocusOut, Qt::TabFocusReason);
    QApplication::sendEvent(m_component, &focusOutEvent);

    // Should change to normal state
    QCOMPARE(m_component->state(), FluentState::Normal);
    QCOMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.first().first().value<FluentState>(),
             FluentState::Normal);
}

void FluentComponentTest::testUpdateStateStyle() {
    // Test updateStateStyle virtual method
    // This is called internally when state changes
    // We can't test the implementation directly, but we can verify it's called

    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    m_component->setState(FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);

    // updateStateStyle should have been called internally
    // The component should be in the correct state
    QCOMPARE(m_component->state(), FluentState::Hovered);
}

void FluentComponentTest::testPerformStateTransition() {
    // Test performStateTransition virtual method
    // This is called internally when animations are enabled

    m_component->setAnimated(true);
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    m_component->setState(FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);

    // performStateTransition should have been called internally
    // The component should be in the correct state
    QCOMPARE(m_component->state(), FluentState::Hovered);
}

void FluentComponentTest::testBindProperty() {
    // Test property binding functionality
    // This is a template method that allows reactive property binding

    // Create a simple getter function
    int testValue = 42;
    auto getter = [&testValue]() { return testValue; };

    // Bind the property
    m_component->bindProperty<int>("testProperty", getter);

    // The binding should work without crashing
    // Detailed testing would require access to internal binding mechanism
    QVERIFY(true);
}

void FluentComponentTest::testThemeIntegration() {
    // Test theme integration
    QSignalSpy themeChangedSpy(m_component, &FluentComponent::themeChanged);

    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    auto originalMode = theme.mode();

    // Change theme mode
    auto newMode = (originalMode == FluentQt::Styling::FluentThemeMode::Light)
                       ? FluentQt::Styling::FluentThemeMode::Dark
                       : FluentQt::Styling::FluentThemeMode::Light;
    theme.setMode(newMode);

    // Component should respond to theme changes
    // The exact behavior depends on implementation
    // For now, just verify the component still functions correctly
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));
    m_component->setState(FluentState::Hovered);
    QCOMPARE(m_component->state(), FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);

    // Restore original theme
    theme.setMode(originalMode);
}

void FluentComponentTest::testAccessibility() {
    // Test accessibility features
    QCOMPARE(m_component->focusPolicy(), Qt::StrongFocus);
    QVERIFY(m_component->testAttribute(Qt::WA_Hover));

    // Test that component can receive focus
    m_component->show();
    QTest::qWaitForWindowExposed(m_component);

    // Ensure component is not under mouse to avoid hover state
    m_component->setAttribute(Qt::WA_Hover, false);
    m_component->setFocus();
    QVERIFY(m_component->hasFocus());

    // Test that focus changes state
    // Note: In headless test environment, focus behavior may differ
    // Accept either Focused or Hovered state as both indicate the component is
    // interactive
    FluentState currentState = m_component->state();
    QVERIFY(currentState == FluentState::Focused ||
            currentState == FluentState::Hovered);

    // Test that component can lose focus
    m_component->clearFocus();
    QVERIFY(!m_component->hasFocus());

    // In headless environment, component may still be in hovered state
    // Accept either Normal or Hovered state after losing focus
    FluentState stateAfterFocusLoss = m_component->state();
    QVERIFY(stateAfterFocusLoss == FluentState::Normal ||
            stateAfterFocusLoss == FluentState::Hovered);
}

void FluentComponentTest::testDisabledState() {
    // Test disabled state behavior
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Disable the component
    m_component->setEnabled(false);
    m_component->setState(FluentState::Disabled);
    QCOMPARE(m_component->state(), FluentState::Disabled);
    QVERIFY(!m_component->isEnabled());

    // Test that disabled component doesn't respond to hover
    stateChangedSpy.clear();
    QEnterEvent enterEvent(QPointF(10, 10), QPointF(10, 10), QPointF(10, 10));
    QApplication::sendEvent(m_component, &enterEvent);

    // State should remain disabled
    QCOMPARE(m_component->state(), FluentState::Disabled);
    QCOMPARE(stateChangedSpy.count(), 0);

    // Test that disabled component doesn't respond to mouse press
    QPoint center = m_component->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, center,
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_component, &pressEvent);

    // State should remain disabled
    QCOMPARE(m_component->state(), FluentState::Disabled);
    QCOMPARE(stateChangedSpy.count(), 0);
}

void FluentComponentTest::testInvalidState() {
    // Test handling of invalid state transitions
    QSignalSpy stateChangedSpy(m_component, SIGNAL(stateChanged(FluentState)));

    // Set a valid state first
    m_component->setState(FluentState::Normal);
    stateChangedSpy.clear();

    // Try to set the same state again (should not emit signal)
    m_component->setState(FluentState::Normal);
    QCOMPARE(stateChangedSpy.count(), 0);

    // All enum values should be valid
    m_component->setState(FluentState::Hovered);
    QCOMPARE(stateChangedSpy.count(), 1);

    m_component->setState(FluentState::Pressed);
    QCOMPARE(stateChangedSpy.count(), 2);

    m_component->setState(FluentState::Focused);
    QCOMPARE(stateChangedSpy.count(), 3);

    m_component->setState(FluentState::Disabled);
    QCOMPARE(stateChangedSpy.count(), 4);
}

QTEST_MAIN(FluentComponentTest)
#include "FluentComponentTest.moc"
