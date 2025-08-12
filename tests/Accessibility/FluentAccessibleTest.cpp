// tests/Accessibility/FluentAccessibleTest.cpp
#include <QAccessible>
#include <QApplication>
#include <QSignalSpy>
#include <QWidget>
#include <QtTest/QtTest>
#include <memory>

#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

using namespace FluentQt::Accessibility;

// Test-friendly subclass to access protected methods
class TestableFluentAccessibleInterface : public FluentAccessibleInterface {
public:
    explicit TestableFluentAccessibleInterface(QWidget* widget)
        : FluentAccessibleInterface(widget) {}

    // Expose protected methods for testing
    using FluentAccessibleInterface::isObjectValid;
    using FluentAccessibleInterface::safeObject;
    using FluentAccessibleInterface::safeWidget;
};

class FluentAccessibleTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Core functionality tests
    void testConstructorWithValidWidget();
    void testConstructorWithNullWidget();
    void testSafeObjectAccess();
    void testSafeWidgetAccess();
    void testObjectValidation();

    // Crash prevention tests
    void testTextWithDestroyedWidget();
    void testSetTextWithDestroyedWidget();
    void testRoleWithDestroyedWidget();
    void testStateWithDestroyedWidget();
    void testHelpWithDestroyedWidget();

    // Edge case tests
    void testMultipleAccessibilityInterfaces();
    void testWidgetDuringDestruction();
    void testFactoryWithInvalidObjects();
    void testConcurrentAccess();

    // Functional tests
    void testFluentButtonAccessibility();
    void testFluentCardAccessibility();
    void testKeyBindingsDescription();
    void testAccessibilityEvents();

private:
    QApplication* m_app = nullptr;
    std::unique_ptr<QWidget> m_testWidget;
    std::unique_ptr<TestableFluentAccessibleInterface> m_accessibleInterface;
};

void FluentAccessibleTest::initTestCase() {
    // Initialize accessibility
    QAccessible::setActive(true);
}

void FluentAccessibleTest::cleanupTestCase() {
    // Cleanup
}

void FluentAccessibleTest::init() {
    // Create a fresh test widget for each test
    m_testWidget = std::make_unique<QWidget>();
    m_testWidget->setObjectName("TestWidget");
}

void FluentAccessibleTest::cleanup() {
    // Clean up after each test
    m_accessibleInterface.reset();
    m_testWidget.reset();
}

void FluentAccessibleTest::testConstructorWithValidWidget() {
    // Test normal constructor usage
    QVERIFY(m_testWidget != nullptr);

    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());
    QVERIFY(m_accessibleInterface != nullptr);

    // Verify the interface is properly initialized
    QVERIFY(m_accessibleInterface->isObjectValid());
    QVERIFY(m_accessibleInterface->safeObject() != nullptr);
    QVERIFY(m_accessibleInterface->safeWidget() != nullptr);
}

void FluentAccessibleTest::testConstructorWithNullWidget() {
    // Test constructor with null widget - should not crash
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Null widget passed to constructor.*"));

    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(nullptr);
    QVERIFY(m_accessibleInterface != nullptr);

    // The interface should handle null widget gracefully
    QVERIFY(!m_accessibleInterface->isObjectValid());
    QVERIFY(m_accessibleInterface->safeObject() == nullptr);
    QVERIFY(m_accessibleInterface->safeWidget() == nullptr);
}

void FluentAccessibleTest::testSafeObjectAccess() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Test safe object access
    QObject* obj = m_accessibleInterface->safeObject();
    QVERIFY(obj != nullptr);
    QCOMPARE(obj, m_testWidget.get());

    // Test with destroyed widget
    m_testWidget.reset();

    // This should return nullptr and log a warning, not crash
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*object\\(\\) returned null pointer.*"));
    obj = m_accessibleInterface->safeObject();
    QVERIFY(obj == nullptr);
}

void FluentAccessibleTest::testSafeWidgetAccess() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Test safe widget access
    QWidget* widget = m_accessibleInterface->safeWidget();
    QVERIFY(widget != nullptr);
    QCOMPARE(widget, m_testWidget.get());

    // Test with destroyed widget
    m_testWidget.reset();

    // This should return nullptr and log a warning, not crash
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*object\\(\\) returned null pointer.*"));
    widget = m_accessibleInterface->safeWidget();
    QVERIFY(widget == nullptr);
}

void FluentAccessibleTest::testObjectValidation() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Initially should be valid
    QVERIFY(m_accessibleInterface->isObjectValid());

    // After destroying widget, should be invalid
    m_testWidget.reset();
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*object\\(\\) returned null pointer.*"));
    QVERIFY(!m_accessibleInterface->isObjectValid());
}

void FluentAccessibleTest::testTextWithDestroyedWidget() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Destroy the widget
    m_testWidget.reset();

    // These calls should not crash, just return empty strings with warnings
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Invalid object, returning empty string.*"));
    QString name = m_accessibleInterface->text(QAccessible::Name);
    QVERIFY(name.isEmpty());

    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Invalid object, returning empty string.*"));
    QString description = m_accessibleInterface->text(QAccessible::Description);
    QVERIFY(description.isEmpty());

    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Invalid object, returning empty string.*"));
    QString value = m_accessibleInterface->text(QAccessible::Value);
    QVERIFY(value.isEmpty());
}

void FluentAccessibleTest::testSetTextWithDestroyedWidget() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Destroy the widget
    m_testWidget.reset();

    // These calls should not crash, just log warnings
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Invalid object, cannot set text.*"));
    m_accessibleInterface->setText(QAccessible::Name, "TestName");

    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Invalid object, cannot set text.*"));
    m_accessibleInterface->setText(QAccessible::Description, "TestDescription");
}

void FluentAccessibleTest::testRoleWithDestroyedWidget() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Destroy the widget
    m_testWidget.reset();

    // This should not crash, just return default role with warning
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Invalid object, returning default role.*"));
    QAccessible::Role role = m_accessibleInterface->role();
    QCOMPARE(role, QAccessible::Client);
}

void FluentAccessibleTest::testStateWithDestroyedWidget() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Destroy the widget
    m_testWidget.reset();

    // This should not crash, just return default state
    QAccessible::State state = m_accessibleInterface->state();
    // Default state should be mostly empty/false
    QVERIFY(!state.checkable);
    QVERIFY(!state.checked);
}

void FluentAccessibleTest::testHelpWithDestroyedWidget() {
    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Destroy the widget
    m_testWidget.reset();

    // This should not crash, just return empty help with warning
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Invalid object, returning empty help.*"));
    QString help = m_accessibleInterface->help();
    QVERIFY(help.isEmpty());
}

void FluentAccessibleTest::testFactoryWithInvalidObjects() {
    // Note: This test would need access to the actual factory implementation
    // For now, we'll just test that our interface handles invalid objects
    // gracefully

    // Test with null widget
    QTest::ignoreMessage(
        QtWarningMsg,
        QRegularExpression(".*Null widget passed to constructor.*"));
    auto nullInterface =
        std::make_unique<TestableFluentAccessibleInterface>(nullptr);
    QVERIFY(!nullInterface->isObjectValid());
}

void FluentAccessibleTest::testFluentButtonAccessibility() {
    // This test would require FluentButton to be available
    // For now, we'll test with a regular widget that has button-like properties
    m_testWidget->setProperty("text", "Test Button");
    m_testWidget->setProperty("checkable", true);
    m_testWidget->setProperty("checked", false);

    m_accessibleInterface =
        std::make_unique<TestableFluentAccessibleInterface>(m_testWidget.get());

    // Test role determination
    QAccessible::Role role = m_accessibleInterface->role();
    QCOMPARE(role, QAccessible::Client);  // Default for non-Fluent widgets

    // Test description generation
    QString description = m_accessibleInterface->description();
    QVERIFY(!description.isEmpty());
}

QTEST_MAIN(FluentAccessibleTest)
#include "FluentAccessibleTest.moc"
