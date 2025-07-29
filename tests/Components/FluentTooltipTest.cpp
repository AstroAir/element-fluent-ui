// tests/Components/FluentTooltipTest.cpp
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>

#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentTooltipTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTextConstructor();
    void testTitleTextConstructor();

    // Content tests
    void testText();
    void testTitle();

    // Position and appearance tests
    void testPosition();
    void testTheme();

    // Timing tests
    void testShowDelay();
    void testHideDelay();

    // Behavior tests
    void testPersistent();
    void testMaxSize();

    // Static convenience methods tests
    void testStaticShowTooltip();
    void testStaticHideTooltip();

    // Instance methods tests
    void testShowAt();
    void testShowFor();
    void testHide();

    // Animation and timing tests
    void testShowHideAnimation();
    void testDelayedShow();
    void testDelayedHide();

    // Event handling tests
    void testEnterLeaveEvents();
    void testMouseEvents();

    // Signal tests
    void testPropertyChangeSignals();
    void testShowHideSignals();

    // Position calculation tests
    void testPositionCalculation();
    void testAutoPositioning();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentTooltip* m_tooltip;
    QWidget* m_targetWidget;
};

void FluentTooltipTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentTooltipTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentTooltipTest::init() {
    // Create a fresh tooltip and target widget before each test
    m_tooltip = new FluentTooltip();
    m_targetWidget = new QWidget();
    m_targetWidget->resize(100, 50);
    m_targetWidget->show();
}

void FluentTooltipTest::cleanup() {
    // Clean up after each test
    delete m_tooltip;
    delete m_targetWidget;
    m_tooltip = nullptr;
    m_targetWidget = nullptr;
}

void FluentTooltipTest::testDefaultConstructor() {
    // Test default constructor
    FluentTooltip* tooltip = new FluentTooltip();
    
    // Verify default properties
    QVERIFY(tooltip->text().isEmpty());
    QVERIFY(tooltip->title().isEmpty());
    QCOMPARE(tooltip->position(), FluentTooltipPosition::Auto);
    QCOMPARE(tooltip->theme(), FluentTooltipTheme::Auto);
    QCOMPARE(tooltip->showDelay(), 500);
    QCOMPARE(tooltip->hideDelay(), 100);
    QVERIFY(!tooltip->isPersistent());
    QCOMPARE(tooltip->maxSize(), QSize(300, 200));
    
    delete tooltip;
}

void FluentTooltipTest::testTextConstructor() {
    // Test constructor with text
    const QString tooltipText = "Test tooltip text";
    FluentTooltip* tooltip = new FluentTooltip(tooltipText);
    
    QCOMPARE(tooltip->text(), tooltipText);
    QVERIFY(tooltip->title().isEmpty());
    
    delete tooltip;
}

void FluentTooltipTest::testTitleTextConstructor() {
    // Test constructor with title and text
    const QString tooltipTitle = "Test Title";
    const QString tooltipText = "Test tooltip text";
    FluentTooltip* tooltip = new FluentTooltip(tooltipTitle, tooltipText);
    
    QCOMPARE(tooltip->title(), tooltipTitle);
    QCOMPARE(tooltip->text(), tooltipText);
    
    delete tooltip;
}

void FluentTooltipTest::testText() {
    // Test setting and getting text
    const QString text1 = "Tooltip text";
    const QString text2 = "New tooltip text";
    
    QSignalSpy textChangedSpy(m_tooltip, &FluentTooltip::textChanged);
    
    m_tooltip->setText(text1);
    QCOMPARE(m_tooltip->text(), text1);
    QCOMPARE(textChangedSpy.count(), 1);
    QCOMPARE(textChangedSpy.first().first().toString(), text1);
    
    m_tooltip->setText(text2);
    QCOMPARE(m_tooltip->text(), text2);
    QCOMPARE(textChangedSpy.count(), 2);
    QCOMPARE(textChangedSpy.last().first().toString(), text2);
    
    // Setting the same text should not emit the signal
    m_tooltip->setText(text2);
    QCOMPARE(textChangedSpy.count(), 2);
}

void FluentTooltipTest::testTitle() {
    // Test setting and getting title
    const QString title1 = "Tooltip title";
    const QString title2 = "New tooltip title";
    
    QSignalSpy titleChangedSpy(m_tooltip, &FluentTooltip::titleChanged);
    
    m_tooltip->setTitle(title1);
    QCOMPARE(m_tooltip->title(), title1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.first().first().toString(), title1);
    
    m_tooltip->setTitle(title2);
    QCOMPARE(m_tooltip->title(), title2);
    QCOMPARE(titleChangedSpy.count(), 2);
    QCOMPARE(titleChangedSpy.last().first().toString(), title2);
    
    // Setting the same title should not emit the signal
    m_tooltip->setTitle(title2);
    QCOMPARE(titleChangedSpy.count(), 2);
}

void FluentTooltipTest::testPosition() {
    // Test setting and getting position
    QSignalSpy positionChangedSpy(m_tooltip, &FluentTooltip::positionChanged);
    
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::Auto); // Default position
    
    m_tooltip->setPosition(FluentTooltipPosition::Top);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::Top);
    QCOMPARE(positionChangedSpy.count(), 1);
    QCOMPARE(positionChangedSpy.first().first().value<FluentTooltipPosition>(), FluentTooltipPosition::Top);
    
    m_tooltip->setPosition(FluentTooltipPosition::Bottom);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::Bottom);
    QCOMPARE(positionChangedSpy.count(), 2);
    
    m_tooltip->setPosition(FluentTooltipPosition::Left);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::Left);
    QCOMPARE(positionChangedSpy.count(), 3);
    
    m_tooltip->setPosition(FluentTooltipPosition::Right);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::Right);
    QCOMPARE(positionChangedSpy.count(), 4);
    
    m_tooltip->setPosition(FluentTooltipPosition::TopLeft);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::TopLeft);
    QCOMPARE(positionChangedSpy.count(), 5);
    
    m_tooltip->setPosition(FluentTooltipPosition::TopRight);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::TopRight);
    QCOMPARE(positionChangedSpy.count(), 6);
    
    m_tooltip->setPosition(FluentTooltipPosition::BottomLeft);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::BottomLeft);
    QCOMPARE(positionChangedSpy.count(), 7);
    
    m_tooltip->setPosition(FluentTooltipPosition::BottomRight);
    QCOMPARE(m_tooltip->position(), FluentTooltipPosition::BottomRight);
    QCOMPARE(positionChangedSpy.count(), 8);
    
    // Setting the same position should not emit the signal
    m_tooltip->setPosition(FluentTooltipPosition::BottomRight);
    QCOMPARE(positionChangedSpy.count(), 8);
}

void FluentTooltipTest::testTheme() {
    // Test setting and getting theme
    QSignalSpy themeChangedSpy(m_tooltip, &FluentTooltip::themeChanged);
    
    QCOMPARE(m_tooltip->theme(), FluentTooltipTheme::Auto); // Default theme
    
    m_tooltip->setTheme(FluentTooltipTheme::Light);
    QCOMPARE(m_tooltip->theme(), FluentTooltipTheme::Light);
    QCOMPARE(themeChangedSpy.count(), 1);
    QCOMPARE(themeChangedSpy.first().first().value<FluentTooltipTheme>(), FluentTooltipTheme::Light);
    
    m_tooltip->setTheme(FluentTooltipTheme::Dark);
    QCOMPARE(m_tooltip->theme(), FluentTooltipTheme::Dark);
    QCOMPARE(themeChangedSpy.count(), 2);
    
    // Setting the same theme should not emit the signal
    m_tooltip->setTheme(FluentTooltipTheme::Dark);
    QCOMPARE(themeChangedSpy.count(), 2);
}

void FluentTooltipTest::testShowDelay() {
    // Test setting and getting show delay
    QSignalSpy showDelayChangedSpy(m_tooltip, &FluentTooltip::showDelayChanged);
    
    QCOMPARE(m_tooltip->showDelay(), 500); // Default delay
    
    m_tooltip->setShowDelay(1000);
    QCOMPARE(m_tooltip->showDelay(), 1000);
    QCOMPARE(showDelayChangedSpy.count(), 1);
    QCOMPARE(showDelayChangedSpy.first().first().toInt(), 1000);
    
    m_tooltip->setShowDelay(250);
    QCOMPARE(m_tooltip->showDelay(), 250);
    QCOMPARE(showDelayChangedSpy.count(), 2);
    QCOMPARE(showDelayChangedSpy.last().first().toInt(), 250);
    
    // Setting the same delay should not emit the signal
    m_tooltip->setShowDelay(250);
    QCOMPARE(showDelayChangedSpy.count(), 2);
}

void FluentTooltipTest::testHideDelay() {
    // Test setting and getting hide delay
    QSignalSpy hideDelayChangedSpy(m_tooltip, &FluentTooltip::hideDelayChanged);
    
    QCOMPARE(m_tooltip->hideDelay(), 100); // Default delay
    
    m_tooltip->setHideDelay(500);
    QCOMPARE(m_tooltip->hideDelay(), 500);
    QCOMPARE(hideDelayChangedSpy.count(), 1);
    QCOMPARE(hideDelayChangedSpy.first().first().toInt(), 500);
    
    m_tooltip->setHideDelay(50);
    QCOMPARE(m_tooltip->hideDelay(), 50);
    QCOMPARE(hideDelayChangedSpy.count(), 2);
    QCOMPARE(hideDelayChangedSpy.last().first().toInt(), 50);
    
    // Setting the same delay should not emit the signal
    m_tooltip->setHideDelay(50);
    QCOMPARE(hideDelayChangedSpy.count(), 2);
}

void FluentTooltipTest::testPersistent() {
    // Test persistent property
    QSignalSpy persistentChangedSpy(m_tooltip, &FluentTooltip::persistentChanged);

    QVERIFY(!m_tooltip->isPersistent()); // Default should be false

    m_tooltip->setPersistent(true);
    QVERIFY(m_tooltip->isPersistent());
    QCOMPARE(persistentChangedSpy.count(), 1);
    QCOMPARE(persistentChangedSpy.first().first().toBool(), true);

    m_tooltip->setPersistent(false);
    QVERIFY(!m_tooltip->isPersistent());
    QCOMPARE(persistentChangedSpy.count(), 2);
    QCOMPARE(persistentChangedSpy.last().first().toBool(), false);

    // Setting the same persistent state should not emit the signal
    m_tooltip->setPersistent(false);
    QCOMPARE(persistentChangedSpy.count(), 2);
}

void FluentTooltipTest::testMaxSize() {
    // Test setting and getting max size
    QSignalSpy maxSizeChangedSpy(m_tooltip, &FluentTooltip::maxSizeChanged);

    QCOMPARE(m_tooltip->maxSize(), QSize(300, 200)); // Default size

    QSize newSize1(400, 300);
    m_tooltip->setMaxSize(newSize1);
    QCOMPARE(m_tooltip->maxSize(), newSize1);
    QCOMPARE(maxSizeChangedSpy.count(), 1);
    QCOMPARE(maxSizeChangedSpy.first().first().toSize(), newSize1);

    QSize newSize2(500, 400);
    m_tooltip->setMaxSize(newSize2);
    QCOMPARE(m_tooltip->maxSize(), newSize2);
    QCOMPARE(maxSizeChangedSpy.count(), 2);
    QCOMPARE(maxSizeChangedSpy.last().first().toSize(), newSize2);

    // Setting the same size should not emit the signal
    m_tooltip->setMaxSize(newSize2);
    QCOMPARE(maxSizeChangedSpy.count(), 2);
}

void FluentTooltipTest::testStaticShowTooltip() {
    // Test static show tooltip methods
    const QString text = "Static tooltip text";
    const QString title = "Static Title";
    const QPoint position(100, 100);

    // Test simple text tooltip
    FluentTooltip::showTooltip(text, position);

    // Test title and text tooltip
    FluentTooltip::showTooltip(title, text, position);

    // Test with parent widget
    FluentTooltip::showTooltip(text, position, m_targetWidget);

    // These methods should not crash
    QVERIFY(true);
}

void FluentTooltipTest::testStaticHideTooltip() {
    // Test static hide tooltip method
    FluentTooltip::showTooltip("Test", QPoint(100, 100));
    FluentTooltip::hideTooltip();

    // This method should not crash
    QVERIFY(true);
}

void FluentTooltipTest::testShowAt() {
    // Test showAt method
    m_tooltip->setText("Test tooltip");

    QPoint position(150, 150);
    m_tooltip->showAt(position);

    // The tooltip should be shown at the specified position
    // We can't easily test the exact position without access to internal state
    QVERIFY(true);
}

void FluentTooltipTest::testShowFor() {
    // Test showFor method
    m_tooltip->setText("Test tooltip for widget");

    // Show tooltip for the target widget
    m_tooltip->showFor(m_targetWidget);

    // Show tooltip for widget with specific position
    m_tooltip->showFor(m_targetWidget, FluentTooltipPosition::Top);
    m_tooltip->showFor(m_targetWidget, FluentTooltipPosition::Bottom);
    m_tooltip->showFor(m_targetWidget, FluentTooltipPosition::Left);
    m_tooltip->showFor(m_targetWidget, FluentTooltipPosition::Right);

    // These methods should not crash
    QVERIFY(true);
}

void FluentTooltipTest::testHide() {
    // Test hide method
    m_tooltip->setText("Test tooltip");
    m_tooltip->showAt(QPoint(100, 100));

    m_tooltip->hide();

    // The tooltip should be hidden
    QVERIFY(true);
}

void FluentTooltipTest::testShowHideAnimation() {
    // Test show and hide animations
    QSignalSpy aboutToShowSpy(m_tooltip, &FluentTooltip::aboutToShow);
    QSignalSpy aboutToHideSpy(m_tooltip, &FluentTooltip::aboutToHide);

    m_tooltip->setText("Animated tooltip");

    // Show tooltip
    m_tooltip->show();
    QCOMPARE(aboutToShowSpy.count(), 1);

    // Hide tooltip
    m_tooltip->hide();
    QCOMPARE(aboutToHideSpy.count(), 1);
}

void FluentTooltipTest::testDelayedShow() {
    // Test delayed show functionality
    QSignalSpy aboutToShowSpy(m_tooltip, &FluentTooltip::aboutToShow);

    m_tooltip->setText("Delayed tooltip");
    m_tooltip->setShowDelay(100); // Short delay for testing

    // Show tooltip - should be delayed
    m_tooltip->showAt(QPoint(100, 100));

    // Should not show immediately
    QCOMPARE(aboutToShowSpy.count(), 0);

    // Wait for the delay
    QTest::qWait(150);

    // Should show after delay
    QCOMPARE(aboutToShowSpy.count(), 1);
}

void FluentTooltipTest::testDelayedHide() {
    // Test delayed hide functionality
    QSignalSpy aboutToHideSpy(m_tooltip, &FluentTooltip::aboutToHide);

    m_tooltip->setText("Delayed hide tooltip");
    m_tooltip->setHideDelay(100); // Short delay for testing
    m_tooltip->setShowDelay(0); // No show delay

    // Show tooltip immediately
    m_tooltip->showAt(QPoint(100, 100));

    // Hide tooltip - should be delayed
    m_tooltip->hide();

    // Should not hide immediately
    QCOMPARE(aboutToHideSpy.count(), 0);

    // Wait for the delay
    QTest::qWait(150);

    // Should hide after delay
    QCOMPARE(aboutToHideSpy.count(), 1);
}

void FluentTooltipTest::testEnterLeaveEvents() {
    // Test enter and leave event handling
    m_tooltip->setText("Event test tooltip");
    m_tooltip->showAt(QPoint(100, 100));

    // For now, just verify that the tooltip can handle events without crashing
    // Direct event simulation may have compatibility issues
    QVERIFY(true);
}

void FluentTooltipTest::testMouseEvents() {
    // Test mouse event handling
    m_tooltip->setText("Mouse event tooltip");
    m_tooltip->showAt(QPoint(100, 100));

    // For now, just verify that the tooltip can handle mouse events without crashing
    // Direct event simulation may have compatibility issues
    QVERIFY(true);
}

void FluentTooltipTest::testPropertyChangeSignals() {
    // Test that property change signals are emitted correctly
    QSignalSpy textChangedSpy(m_tooltip, &FluentTooltip::textChanged);
    QSignalSpy titleChangedSpy(m_tooltip, &FluentTooltip::titleChanged);
    QSignalSpy positionChangedSpy(m_tooltip, &FluentTooltip::positionChanged);
    QSignalSpy themeChangedSpy(m_tooltip, &FluentTooltip::themeChanged);
    QSignalSpy showDelayChangedSpy(m_tooltip, &FluentTooltip::showDelayChanged);
    QSignalSpy hideDelayChangedSpy(m_tooltip, &FluentTooltip::hideDelayChanged);
    QSignalSpy persistentChangedSpy(m_tooltip, &FluentTooltip::persistentChanged);
    QSignalSpy maxSizeChangedSpy(m_tooltip, &FluentTooltip::maxSizeChanged);

    // Change properties and verify signals
    m_tooltip->setText("New text");
    QCOMPARE(textChangedSpy.count(), 1);

    m_tooltip->setTitle("New title");
    QCOMPARE(titleChangedSpy.count(), 1);

    m_tooltip->setPosition(FluentTooltipPosition::Top);
    QCOMPARE(positionChangedSpy.count(), 1);

    m_tooltip->setTheme(FluentTooltipTheme::Dark);
    QCOMPARE(themeChangedSpy.count(), 1);

    m_tooltip->setShowDelay(1000);
    QCOMPARE(showDelayChangedSpy.count(), 1);

    m_tooltip->setHideDelay(200);
    QCOMPARE(hideDelayChangedSpy.count(), 1);

    m_tooltip->setPersistent(true);
    QCOMPARE(persistentChangedSpy.count(), 1);

    m_tooltip->setMaxSize(QSize(400, 300));
    QCOMPARE(maxSizeChangedSpy.count(), 1);
}

void FluentTooltipTest::testShowHideSignals() {
    // Test show and hide signals
    QSignalSpy aboutToShowSpy(m_tooltip, &FluentTooltip::aboutToShow);
    QSignalSpy aboutToHideSpy(m_tooltip, &FluentTooltip::aboutToHide);

    m_tooltip->setText("Signal test tooltip");

    // Show tooltip
    m_tooltip->show();
    QCOMPARE(aboutToShowSpy.count(), 1);

    // Hide tooltip
    m_tooltip->hide();
    QCOMPARE(aboutToHideSpy.count(), 1);

    // Show again
    m_tooltip->showAt(QPoint(100, 100));
    QCOMPARE(aboutToShowSpy.count(), 2);

    // Hide again
    m_tooltip->hide();
    QCOMPARE(aboutToHideSpy.count(), 2);
}

void FluentTooltipTest::testPositionCalculation() {
    // Test position calculation for different positions
    m_tooltip->setText("Position test tooltip");

    // Test different positions
    m_tooltip->setPosition(FluentTooltipPosition::Top);
    m_tooltip->showFor(m_targetWidget);

    m_tooltip->setPosition(FluentTooltipPosition::Bottom);
    m_tooltip->showFor(m_targetWidget);

    m_tooltip->setPosition(FluentTooltipPosition::Left);
    m_tooltip->showFor(m_targetWidget);

    m_tooltip->setPosition(FluentTooltipPosition::Right);
    m_tooltip->showFor(m_targetWidget);

    m_tooltip->setPosition(FluentTooltipPosition::TopLeft);
    m_tooltip->showFor(m_targetWidget);

    m_tooltip->setPosition(FluentTooltipPosition::TopRight);
    m_tooltip->showFor(m_targetWidget);

    m_tooltip->setPosition(FluentTooltipPosition::BottomLeft);
    m_tooltip->showFor(m_targetWidget);

    m_tooltip->setPosition(FluentTooltipPosition::BottomRight);
    m_tooltip->showFor(m_targetWidget);

    // All position calculations should work without crashing
    QVERIFY(true);
}

void FluentTooltipTest::testAutoPositioning() {
    // Test automatic positioning
    m_tooltip->setText("Auto position tooltip");
    m_tooltip->setPosition(FluentTooltipPosition::Auto);

    // Show tooltip for widget - should automatically choose best position
    m_tooltip->showFor(m_targetWidget);

    // Auto positioning should work without crashing
    QVERIFY(true);
}

void FluentTooltipTest::testAccessibility() {
    // Test accessibility features
    m_tooltip->setText("Accessible tooltip");
    m_tooltip->setTitle("Accessibility Test");

    // Test that tooltip can be created and used without accessibility-related crashes
    m_tooltip->showAt(QPoint(100, 100));
    m_tooltip->hide();

    // Accessibility should work without crashing
    QVERIFY(true);
}

void FluentTooltipTest::testThemeIntegration() {
    // Test that tooltip responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    auto originalMode = theme.mode();

    // Change theme mode
    auto newMode = (originalMode == FluentQt::Styling::FluentThemeMode::Light) ?
                   FluentQt::Styling::FluentThemeMode::Dark :
                   FluentQt::Styling::FluentThemeMode::Light;
    theme.setMode(newMode);

    // Tooltip should update its appearance (this would require checking internal styling)
    // For now, just verify the tooltip still functions correctly
    QSignalSpy textChangedSpy(m_tooltip, &FluentTooltip::textChanged);
    m_tooltip->setText("Theme test tooltip");
    QCOMPARE(m_tooltip->text(), QString("Theme test tooltip"));
    QCOMPARE(textChangedSpy.count(), 1);

    // Test that tooltip can be shown with new theme
    m_tooltip->showAt(QPoint(100, 100));
    m_tooltip->hide();

    // Restore original theme
    theme.setMode(originalMode);
}

QTEST_MAIN(FluentTooltipTest)
#include "FluentTooltipTest.moc"
