#include <QtTest/QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QKeyEvent>
#include <QResizeEvent>

#include "FluentQt/Components/FluentSidebar.h"

using namespace FluentQt::Components;

class SidebarComponentTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testSidebarCreation();
    void testModeChanges();
    void testItemManagement();
    void testSelection();
    void testConfiguration();

    // Responsive behavior tests
    void testResponsiveBehavior();
    void testBreakpoints();
    void testAutoMode();

    // Animation tests
    void testModeTransitions();
    void testAnimationConfiguration();

    // Accessibility tests
    void testKeyboardNavigation();
    void testAccessibilityProperties();
    void testScreenReaderSupport();

    // Event handling tests
    void testItemClicks();
    void testSignalEmission();
    void testEventFiltering();

private:
    FluentSidebar* m_sidebar = nullptr;
    QWidget* m_parentWidget = nullptr;
    QApplication* m_app = nullptr;
};

void SidebarComponentTests::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char** argv = nullptr;
        m_app = new QApplication(argc, argv);
    }
}

void SidebarComponentTests::cleanupTestCase() {
    if (m_app) {
        delete m_app;
        m_app = nullptr;
    }
}

void SidebarComponentTests::init() {
    m_parentWidget = new QWidget();
    m_parentWidget->resize(1000, 600);
    
    m_sidebar = new FluentSidebar(m_parentWidget);
    m_parentWidget->show();
}

void SidebarComponentTests::cleanup() {
    if (m_parentWidget) {
        delete m_parentWidget;
        m_parentWidget = nullptr;
        m_sidebar = nullptr; // Will be deleted with parent
    }
}

void SidebarComponentTests::testSidebarCreation() {
    QVERIFY(m_sidebar != nullptr);
    QCOMPARE(m_sidebar->mode(), FluentSidebarMode::Expanded);
    QCOMPARE(m_sidebar->expandedWidth(), 280);
    QCOMPARE(m_sidebar->compactWidth(), 48);
    QVERIFY(m_sidebar->isCollapsible());
}

void SidebarComponentTests::testModeChanges() {
    QSignalSpy modeChangedSpy(m_sidebar, &FluentSidebar::modeChanged);
    
    // Test mode changes
    m_sidebar->setMode(FluentSidebarMode::Compact);
    QCOMPARE(m_sidebar->mode(), FluentSidebarMode::Compact);
    QCOMPARE(modeChangedSpy.count(), 1);
    
    m_sidebar->setMode(FluentSidebarMode::Hidden);
    QCOMPARE(m_sidebar->mode(), FluentSidebarMode::Hidden);
    QCOMPARE(modeChangedSpy.count(), 2);
    
    m_sidebar->setMode(FluentSidebarMode::Overlay);
    QCOMPARE(m_sidebar->mode(), FluentSidebarMode::Overlay);
    QCOMPARE(modeChangedSpy.count(), 3);
}

void SidebarComponentTests::testItemManagement() {
    // Test adding items
    FluentSidebarItem item1;
    item1.id = "home";
    item1.text = "Home";
    item1.icon = QIcon();
    
    m_sidebar->addItem(item1);
    QCOMPARE(m_sidebar->itemCount(), 1);
    
    FluentSidebarItem item2;
    item2.id = "settings";
    item2.text = "Settings";
    
    m_sidebar->addItem(item2);
    QCOMPARE(m_sidebar->itemCount(), 2);
    
    // Test finding items
    auto* foundItem = m_sidebar->findItem("home");
    QVERIFY(foundItem != nullptr);
    QCOMPARE(foundItem->id, "home");
    QCOMPARE(foundItem->text, "Home");
    
    // Test removing items
    m_sidebar->removeItem("home");
    QCOMPARE(m_sidebar->itemCount(), 1);
    
    foundItem = m_sidebar->findItem("home");
    QVERIFY(foundItem == nullptr);
    
    // Test clearing items
    m_sidebar->clearItems();
    QCOMPARE(m_sidebar->itemCount(), 0);
}

void SidebarComponentTests::testSelection() {
    // Add test items
    FluentSidebarItem item1;
    item1.id = "item1";
    item1.text = "Item 1";
    m_sidebar->addItem(item1);
    
    FluentSidebarItem item2;
    item2.id = "item2";
    item2.text = "Item 2";
    m_sidebar->addItem(item2);
    
    QSignalSpy selectionChangedSpy(m_sidebar, &FluentSidebar::itemSelected);
    
    // Test selection
    m_sidebar->setSelectedItem("item1");
    QCOMPARE(m_sidebar->selectedItemId(), "item1");
    QCOMPARE(selectionChangedSpy.count(), 1);
    
    m_sidebar->setSelectedItem("item2");
    QCOMPARE(m_sidebar->selectedItemId(), "item2");
    QCOMPARE(selectionChangedSpy.count(), 2);
    
    // Test clearing selection
    m_sidebar->clearSelection();
    QVERIFY(m_sidebar->selectedItemId().isEmpty());
}

void SidebarComponentTests::testConfiguration() {
    // Test width configuration
    m_sidebar->setExpandedWidth(320);
    QCOMPARE(m_sidebar->expandedWidth(), 320);
    
    m_sidebar->setCompactWidth(60);
    QCOMPARE(m_sidebar->compactWidth(), 60);
    
    // Test behavior configuration
    m_sidebar->setCollapsible(false);
    QVERIFY(!m_sidebar->isCollapsible());
    
    m_sidebar->setAutoHide(true);
    QVERIFY(m_sidebar->autoHide());
    
    // Test animation configuration
    m_sidebar->setAnimationDuration(500);
    m_sidebar->setAnimationEasing(QEasingCurve::InOutQuad);
}

void SidebarComponentTests::testResponsiveBehavior() {
    m_sidebar->enableResponsiveBehavior(true);
    m_sidebar->setResponsiveBreakpoints(768, 480);
    m_sidebar->setMode(FluentSidebarMode::Auto);
    
    QSignalSpy modeChangedSpy(m_sidebar, &FluentSidebar::modeChanged);
    
    // Simulate window resize to trigger responsive behavior
    m_parentWidget->resize(400, 600); // Below compact breakpoint
    QResizeEvent resizeEvent(QSize(400, 600), QSize(1000, 600));
    QApplication::sendEvent(m_parentWidget, &resizeEvent);
    
    // Process events to allow responsive timer to trigger
    QTest::qWait(150);
    
    // Should have changed to compact or hidden mode
    QVERIFY(m_sidebar->mode() == FluentSidebarMode::Compact || 
            m_sidebar->mode() == FluentSidebarMode::Hidden);
}

void SidebarComponentTests::testBreakpoints() {
    m_sidebar->setResponsiveBreakpoints(800, 500);
    m_sidebar->enableResponsiveBehavior(true);
    m_sidebar->setMode(FluentSidebarMode::Auto);
    
    // Test different window sizes
    QSignalSpy modeChangedSpy(m_sidebar, &FluentSidebar::modeChanged);
    
    // Large screen - should be expanded
    m_parentWidget->resize(1200, 600);
    QTest::qWait(150);
    
    // Medium screen - should be compact
    m_parentWidget->resize(700, 600);
    QTest::qWait(150);
    
    // Small screen - should be hidden
    m_parentWidget->resize(400, 600);
    QTest::qWait(150);
    
    QVERIFY(modeChangedSpy.count() >= 1);
}

void SidebarComponentTests::testModeTransitions() {
    QSignalSpy expandedSpy(m_sidebar, &FluentSidebar::expanded);
    QSignalSpy collapsedSpy(m_sidebar, &FluentSidebar::collapsed);
    
    // Test expand/collapse
    m_sidebar->collapse();
    QVERIFY(collapsedSpy.wait(1000));
    QCOMPARE(collapsedSpy.count(), 1);
    
    m_sidebar->expand();
    QVERIFY(expandedSpy.wait(1000));
    QCOMPARE(expandedSpy.count(), 1);
    
    // Test toggle
    m_sidebar->toggle();
    QVERIFY(collapsedSpy.wait(1000));
    QCOMPARE(collapsedSpy.count(), 2);
}

void SidebarComponentTests::testKeyboardNavigation() {
    // Add test items
    FluentSidebarItem item1;
    item1.id = "item1";
    item1.text = "Item 1";
    m_sidebar->addItem(item1);
    
    FluentSidebarItem item2;
    item2.id = "item2";
    item2.text = "Item 2";
    m_sidebar->addItem(item2);
    
    FluentSidebarItem item3;
    item3.id = "item3";
    item3.text = "Item 3";
    m_sidebar->addItem(item3);
    
    m_sidebar->setSelectedItem("item1");
    m_sidebar->setFocus();
    
    QSignalSpy selectionChangedSpy(m_sidebar, &FluentSidebar::itemSelected);
    
    // Test down arrow navigation
    QKeyEvent downEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QApplication::sendEvent(m_sidebar, &downEvent);
    
    // Should select next item
    QVERIFY(selectionChangedSpy.count() >= 1);
    
    // Test up arrow navigation
    QKeyEvent upEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QApplication::sendEvent(m_sidebar, &upEvent);
    
    // Test Enter key activation
    QSignalSpy itemClickedSpy(m_sidebar, &FluentSidebar::itemClicked);
    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_sidebar, &enterEvent);
    
    QVERIFY(itemClickedSpy.count() >= 1);
}

void SidebarComponentTests::testAccessibilityProperties() {
    m_sidebar->setAccessibleName("Main Navigation");
    m_sidebar->setAccessibleDescription("Primary navigation sidebar");
    
    QCOMPARE(m_sidebar->accessibleName(), "Main Navigation");
    QCOMPARE(m_sidebar->accessibleDescription(), "Primary navigation sidebar");
}

void SidebarComponentTests::testItemClicks() {
    // Add test item
    FluentSidebarItem item;
    item.id = "test-item";
    item.text = "Test Item";
    
    bool callbackExecuted = false;
    item.onClicked = [&callbackExecuted]() {
        callbackExecuted = true;
    };
    
    m_sidebar->addItem(item);
    
    QSignalSpy itemClickedSpy(m_sidebar, &FluentSidebar::itemClicked);
    QSignalSpy itemSelectedSpy(m_sidebar, &FluentSidebar::itemSelected);
    
    // Simulate item click (this would normally be done through the UI)
    m_sidebar->setSelectedItem("test-item");
    
    QCOMPARE(itemSelectedSpy.count(), 1);
    QCOMPARE(m_sidebar->selectedItemId(), "test-item");
}

void SidebarComponentTests::testSignalEmission() {
    QSignalSpy modeChangedSpy(m_sidebar, &FluentSidebar::modeChanged);
    QSignalSpy expandedSpy(m_sidebar, &FluentSidebar::expanded);
    QSignalSpy collapsedSpy(m_sidebar, &FluentSidebar::collapsed);
    QSignalSpy overlayShownSpy(m_sidebar, &FluentSidebar::overlayShown);
    QSignalSpy overlayHiddenSpy(m_sidebar, &FluentSidebar::overlayHidden);
    
    // Test all mode change signals
    m_sidebar->setMode(FluentSidebarMode::Compact);
    QVERIFY(modeChangedSpy.count() >= 1);
    
    m_sidebar->setMode(FluentSidebarMode::Overlay);
    QVERIFY(modeChangedSpy.count() >= 2);
    
    m_sidebar->setMode(FluentSidebarMode::Hidden);
    QVERIFY(modeChangedSpy.count() >= 3);
    
    m_sidebar->setMode(FluentSidebarMode::Expanded);
    QVERIFY(modeChangedSpy.count() >= 4);
}

void SidebarComponentTests::testAnimationConfiguration() {
    // Test that animation settings can be configured
    m_sidebar->setAnimationDuration(1000);
    m_sidebar->setAnimationEasing(QEasingCurve::InOutBounce);
    
    // Test mode change with custom animation
    QElapsedTimer timer;
    timer.start();
    
    m_sidebar->setMode(FluentSidebarMode::Compact);
    
    // Animation should take approximately the configured duration
    // (This is a simplified test - in practice, we'd need to wait for the animation)
    QVERIFY(timer.elapsed() < 100); // Should start immediately
}

QTEST_MAIN(SidebarComponentTests)
#include "SidebarComponentTests.moc"
