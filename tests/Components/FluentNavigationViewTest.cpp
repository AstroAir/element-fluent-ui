// tests/Components/FluentNavigationViewTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentNavigationViewTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Menu item tests
    void testAddMenuItem();
    void testRemoveMenuItem();
    void testClearMenuItems();
    void testMenuItemCount();

    // Selection tests
    void testSelectedItem();
    void testSelectItem();
    void testClearSelection();

    // Display mode tests
    void testDisplayMode();
    void testPaneDisplayMode();
    void testCompactModeThreshold();

    // Header/Footer tests
    void testHeader();
    void testFooter();
    void testSettingsItem();

    // Pane tests
    void testPaneTitle();
    void testPaneToggleButton();
    void testIsPaneOpen();

    // Content tests
    void testContentFrame();
    void testAutoSuggestBox();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testItemClick();
    void testKeyboardNavigation();
    void testPaneToggle();

private:
    FluentNavigationView* m_navigationView{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentNavigationViewTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(800, 600);
}

void FluentNavigationViewTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentNavigationViewTest::init() {
    m_navigationView = new FluentNavigationView(m_testWidget);
}

void FluentNavigationViewTest::cleanup() {
    delete m_navigationView;
    m_navigationView = nullptr;
}

void FluentNavigationViewTest::testDefaultConstructor() {
    QVERIFY(m_navigationView != nullptr);
    QCOMPARE(m_navigationView->menuItemCount(), 0);
    QCOMPARE(m_navigationView->selectedItem(), nullptr);
    QCOMPARE(m_navigationView->displayMode(), FluentNavigationViewDisplayMode::Expanded);
    QCOMPARE(m_navigationView->paneDisplayMode(), FluentNavigationPaneDisplayMode::Auto);
    QVERIFY(m_navigationView->isPaneOpen());
}

void FluentNavigationViewTest::testAddMenuItem() {
    QSignalSpy itemCountSpy(m_navigationView, &FluentNavigationView::menuItemCountChanged);
    QSignalSpy itemAddedSpy(m_navigationView, &FluentNavigationView::menuItemAdded);
    
    // Add item with text only
    auto* item1 = m_navigationView->addMenuItem("Home");
    QVERIFY(item1 != nullptr);
    QCOMPARE(item1->text(), "Home");
    QCOMPARE(m_navigationView->menuItemCount(), 1);
    QCOMPARE(itemCountSpy.count(), 1);
    QCOMPARE(itemAddedSpy.count(), 1);
    
    // Add item with icon and text
    QIcon homeIcon(":/icons/home.png");
    auto* item2 = m_navigationView->addMenuItem(homeIcon, "Dashboard");
    QVERIFY(item2 != nullptr);
    QCOMPARE(item2->text(), "Dashboard");
    QCOMPARE(item2->icon().name(), homeIcon.name());
    QCOMPARE(m_navigationView->menuItemCount(), 2);
    QCOMPARE(itemCountSpy.count(), 2);
    
    // Add separator
    auto* separator = m_navigationView->addSeparator();
    QVERIFY(separator != nullptr);
    QVERIFY(separator->isSeparator());
    QCOMPARE(m_navigationView->menuItemCount(), 3);
    
    // Add submenu
    auto* submenu = m_navigationView->addSubMenu("Settings");
    QVERIFY(submenu != nullptr);
    QCOMPARE(submenu->text(), "Settings");
    QCOMPARE(m_navigationView->menuItemCount(), 4);
}

void FluentNavigationViewTest::testRemoveMenuItem() {
    auto* item1 = m_navigationView->addMenuItem("Item 1");
    auto* item2 = m_navigationView->addMenuItem("Item 2");
    auto* item3 = m_navigationView->addMenuItem("Item 3");
    
    QSignalSpy itemCountSpy(m_navigationView, &FluentNavigationView::menuItemCountChanged);
    QSignalSpy itemRemovedSpy(m_navigationView, &FluentNavigationView::menuItemRemoved);
    
    m_navigationView->removeMenuItem(item2);
    QCOMPARE(m_navigationView->menuItemCount(), 2);
    QCOMPARE(itemCountSpy.count(), 1);
    QCOMPARE(itemRemovedSpy.count(), 1);
    
    // Test removing by index
    m_navigationView->removeMenuItem(0);
    QCOMPARE(m_navigationView->menuItemCount(), 1);
    QCOMPARE(itemCountSpy.count(), 2);
    
    // Test removing invalid item
    auto* invalidItem = new FluentNavigationViewItem("Invalid");
    m_navigationView->removeMenuItem(invalidItem);
    QCOMPARE(m_navigationView->menuItemCount(), 1); // Should remain unchanged
    delete invalidItem;
}

void FluentNavigationViewTest::testClearMenuItems() {
    m_navigationView->addMenuItem("Item 1");
    m_navigationView->addMenuItem("Item 2");
    m_navigationView->addMenuItem("Item 3");
    
    QSignalSpy itemCountSpy(m_navigationView, &FluentNavigationView::menuItemCountChanged);
    QSignalSpy itemsClearedSpy(m_navigationView, &FluentNavigationView::menuItemsCleared);
    
    m_navigationView->clearMenuItems();
    QCOMPARE(m_navigationView->menuItemCount(), 0);
    QCOMPARE(m_navigationView->selectedItem(), nullptr);
    QCOMPARE(itemCountSpy.count(), 1);
    QCOMPARE(itemsClearedSpy.count(), 1);
}

void FluentNavigationViewTest::testMenuItemCount() {
    QCOMPARE(m_navigationView->menuItemCount(), 0);
    
    m_navigationView->addMenuItem("Item 1");
    QCOMPARE(m_navigationView->menuItemCount(), 1);
    
    m_navigationView->addMenuItem("Item 2");
    QCOMPARE(m_navigationView->menuItemCount(), 2);
    
    m_navigationView->removeMenuItem(0);
    QCOMPARE(m_navigationView->menuItemCount(), 1);
}

void FluentNavigationViewTest::testSelectedItem() {
    auto* item1 = m_navigationView->addMenuItem("Item 1");
    auto* item2 = m_navigationView->addMenuItem("Item 2");
    auto* item3 = m_navigationView->addMenuItem("Item 3");
    
    QSignalSpy selectionSpy(m_navigationView, &FluentNavigationView::selectedItemChanged);
    
    m_navigationView->setSelectedItem(item2);
    QCOMPARE(m_navigationView->selectedItem(), item2);
    QCOMPARE(selectionSpy.count(), 1);
    
    // Test selecting invalid item
    auto* invalidItem = new FluentNavigationViewItem("Invalid");
    m_navigationView->setSelectedItem(invalidItem);
    QCOMPARE(m_navigationView->selectedItem(), item2); // Should remain unchanged
    delete invalidItem;
}

void FluentNavigationViewTest::testSelectItem() {
    auto* item1 = m_navigationView->addMenuItem("Item 1");
    auto* item2 = m_navigationView->addMenuItem("Item 2");
    
    QSignalSpy selectionSpy(m_navigationView, &FluentNavigationView::selectedItemChanged);
    QSignalSpy itemInvokedSpy(m_navigationView, &FluentNavigationView::itemInvoked);
    
    m_navigationView->selectItem(1); // Select by index
    QCOMPARE(m_navigationView->selectedItem(), item2);
    QCOMPARE(selectionSpy.count(), 1);
    
    // Test invoking item
    m_navigationView->invokeItem(item1);
    QCOMPARE(itemInvokedSpy.count(), 1);
}

void FluentNavigationViewTest::testClearSelection() {
    auto* item = m_navigationView->addMenuItem("Item");
    m_navigationView->setSelectedItem(item);
    
    QSignalSpy selectionSpy(m_navigationView, &FluentNavigationView::selectedItemChanged);
    
    m_navigationView->clearSelection();
    QCOMPARE(m_navigationView->selectedItem(), nullptr);
    QCOMPARE(selectionSpy.count(), 1);
}

void FluentNavigationViewTest::testDisplayMode() {
    QSignalSpy displayModeSpy(m_navigationView, &FluentNavigationView::displayModeChanged);
    
    m_navigationView->setDisplayMode(FluentNavigationViewDisplayMode::Compact);
    QCOMPARE(m_navigationView->displayMode(), FluentNavigationViewDisplayMode::Compact);
    QCOMPARE(displayModeSpy.count(), 1);
    
    m_navigationView->setDisplayMode(FluentNavigationViewDisplayMode::Minimal);
    QCOMPARE(m_navigationView->displayMode(), FluentNavigationViewDisplayMode::Minimal);
    QCOMPARE(displayModeSpy.count(), 2);
    
    m_navigationView->setDisplayMode(FluentNavigationViewDisplayMode::Expanded);
    QCOMPARE(m_navigationView->displayMode(), FluentNavigationViewDisplayMode::Expanded);
    QCOMPARE(displayModeSpy.count(), 3);
}

void FluentNavigationViewTest::testPaneDisplayMode() {
    QSignalSpy paneDisplayModeSpy(m_navigationView, &FluentNavigationView::paneDisplayModeChanged);
    
    m_navigationView->setPaneDisplayMode(FluentNavigationPaneDisplayMode::Left);
    QCOMPARE(m_navigationView->paneDisplayMode(), FluentNavigationPaneDisplayMode::Left);
    QCOMPARE(paneDisplayModeSpy.count(), 1);
    
    m_navigationView->setPaneDisplayMode(FluentNavigationPaneDisplayMode::Top);
    QCOMPARE(m_navigationView->paneDisplayMode(), FluentNavigationPaneDisplayMode::Top);
    QCOMPARE(paneDisplayModeSpy.count(), 2);
    
    m_navigationView->setPaneDisplayMode(FluentNavigationPaneDisplayMode::LeftCompact);
    QCOMPARE(m_navigationView->paneDisplayMode(), FluentNavigationPaneDisplayMode::LeftCompact);
    QCOMPARE(paneDisplayModeSpy.count(), 3);
}

void FluentNavigationViewTest::testCompactModeThreshold() {
    QSignalSpy thresholdSpy(m_navigationView, &FluentNavigationView::compactModeThresholdChanged);
    
    m_navigationView->setCompactModeThreshold(600);
    QCOMPARE(m_navigationView->compactModeThreshold(), 600);
    QCOMPARE(thresholdSpy.count(), 1);
    
    // Test invalid threshold
    m_navigationView->setCompactModeThreshold(-100);
    QVERIFY(m_navigationView->compactModeThreshold() >= 0);
}

void FluentNavigationViewTest::testHeader() {
    QSignalSpy headerSpy(m_navigationView, &FluentNavigationView::headerChanged);
    
    auto* headerWidget = new QWidget();
    m_navigationView->setHeader(headerWidget);
    QCOMPARE(m_navigationView->header(), headerWidget);
    QCOMPARE(headerSpy.count(), 1);
    
    // Test null header
    m_navigationView->setHeader(nullptr);
    QCOMPARE(m_navigationView->header(), nullptr);
    QCOMPARE(headerSpy.count(), 2);
}

void FluentNavigationViewTest::testFooter() {
    QSignalSpy footerSpy(m_navigationView, &FluentNavigationView::footerChanged);
    
    auto* footerWidget = new QWidget();
    m_navigationView->setFooter(footerWidget);
    QCOMPARE(m_navigationView->footer(), footerWidget);
    QCOMPARE(footerSpy.count(), 1);
    
    // Test null footer
    m_navigationView->setFooter(nullptr);
    QCOMPARE(m_navigationView->footer(), nullptr);
    QCOMPARE(footerSpy.count(), 2);
}

void FluentNavigationViewTest::testSettingsItem() {
    QSignalSpy settingsItemSpy(m_navigationView, &FluentNavigationView::settingsItemChanged);
    
    auto* settingsItem = new FluentNavigationViewItem("Settings");
    m_navigationView->setSettingsItem(settingsItem);
    QCOMPARE(m_navigationView->settingsItem(), settingsItem);
    QCOMPARE(settingsItemSpy.count(), 1);
    
    // Test null settings item
    m_navigationView->setSettingsItem(nullptr);
    QCOMPARE(m_navigationView->settingsItem(), nullptr);
    QCOMPARE(settingsItemSpy.count(), 2);
}

void FluentNavigationViewTest::testPaneTitle() {
    QSignalSpy paneTitleSpy(m_navigationView, &FluentNavigationView::paneTitleChanged);
    
    m_navigationView->setPaneTitle("Navigation");
    QCOMPARE(m_navigationView->paneTitle(), "Navigation");
    QCOMPARE(paneTitleSpy.count(), 1);
    
    // Test empty title
    m_navigationView->setPaneTitle("");
    QCOMPARE(m_navigationView->paneTitle(), "");
    QCOMPARE(paneTitleSpy.count(), 2);
}

void FluentNavigationViewTest::testPaneToggleButton() {
    QSignalSpy toggleButtonSpy(m_navigationView, &FluentNavigationView::paneToggleButtonVisibleChanged);
    
    m_navigationView->setPaneToggleButtonVisible(false);
    QVERIFY(!m_navigationView->isPaneToggleButtonVisible());
    QCOMPARE(toggleButtonSpy.count(), 1);
    
    m_navigationView->setPaneToggleButtonVisible(true);
    QVERIFY(m_navigationView->isPaneToggleButtonVisible());
    QCOMPARE(toggleButtonSpy.count(), 2);
}

void FluentNavigationViewTest::testIsPaneOpen() {
    QSignalSpy paneOpenSpy(m_navigationView, &FluentNavigationView::isPaneOpenChanged);
    
    m_navigationView->setIsPaneOpen(false);
    QVERIFY(!m_navigationView->isPaneOpen());
    QCOMPARE(paneOpenSpy.count(), 1);
    
    m_navigationView->setIsPaneOpen(true);
    QVERIFY(m_navigationView->isPaneOpen());
    QCOMPARE(paneOpenSpy.count(), 2);
}

void FluentNavigationViewTest::testContentFrame() {
    auto* contentWidget = new QWidget();
    m_navigationView->setContentFrame(contentWidget);
    QCOMPARE(m_navigationView->contentFrame(), contentWidget);
    
    // Test null content
    m_navigationView->setContentFrame(nullptr);
    QCOMPARE(m_navigationView->contentFrame(), nullptr);
}

void FluentNavigationViewTest::testAutoSuggestBox() {
    QSignalSpy autoSuggestSpy(m_navigationView, &FluentNavigationView::autoSuggestBoxChanged);
    
    auto* autoSuggestBox = new QWidget(); // Placeholder for FluentAutoSuggestBox
    m_navigationView->setAutoSuggestBox(autoSuggestBox);
    QCOMPARE(m_navigationView->autoSuggestBox(), autoSuggestBox);
    QCOMPARE(autoSuggestSpy.count(), 1);
    
    // Test null auto suggest box
    m_navigationView->setAutoSuggestBox(nullptr);
    QCOMPARE(m_navigationView->autoSuggestBox(), nullptr);
    QCOMPARE(autoSuggestSpy.count(), 2);
}

void FluentNavigationViewTest::testSignalEmission() {
    QSignalSpy itemCountSpy(m_navigationView, &FluentNavigationView::menuItemCountChanged);
    QSignalSpy itemAddedSpy(m_navigationView, &FluentNavigationView::menuItemAdded);
    QSignalSpy itemRemovedSpy(m_navigationView, &FluentNavigationView::menuItemRemoved);
    QSignalSpy itemsClearedSpy(m_navigationView, &FluentNavigationView::menuItemsCleared);
    QSignalSpy selectionSpy(m_navigationView, &FluentNavigationView::selectedItemChanged);
    QSignalSpy itemInvokedSpy(m_navigationView, &FluentNavigationView::itemInvoked);
    QSignalSpy displayModeSpy(m_navigationView, &FluentNavigationView::displayModeChanged);
    QSignalSpy paneDisplayModeSpy(m_navigationView, &FluentNavigationView::paneDisplayModeChanged);
    QSignalSpy paneTitleSpy(m_navigationView, &FluentNavigationView::paneTitleChanged);
    QSignalSpy paneOpenSpy(m_navigationView, &FluentNavigationView::isPaneOpenChanged);
    
    // Test all signals
    auto* item1 = m_navigationView->addMenuItem("Item 1");
    auto* item2 = m_navigationView->addMenuItem("Item 2");
    m_navigationView->setSelectedItem(item1);
    m_navigationView->invokeItem(item2);
    m_navigationView->removeMenuItem(item1);
    m_navigationView->setDisplayMode(FluentNavigationViewDisplayMode::Compact);
    m_navigationView->setPaneDisplayMode(FluentNavigationPaneDisplayMode::Top);
    m_navigationView->setPaneTitle("Test Navigation");
    m_navigationView->setIsPaneOpen(false);
    m_navigationView->clearMenuItems();
    
    QCOMPARE(itemCountSpy.count(), 4); // add, add, remove, clear
    QCOMPARE(itemAddedSpy.count(), 2);
    QCOMPARE(itemRemovedSpy.count(), 1);
    QCOMPARE(itemsClearedSpy.count(), 1);
    QCOMPARE(selectionSpy.count(), 2); // setSelectedItem, clearMenuItems
    QCOMPARE(itemInvokedSpy.count(), 1);
    QCOMPARE(displayModeSpy.count(), 1);
    QCOMPARE(paneDisplayModeSpy.count(), 1);
    QCOMPARE(paneTitleSpy.count(), 1);
    QCOMPARE(paneOpenSpy.count(), 1);
}

void FluentNavigationViewTest::testItemClick() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    auto* item1 = m_navigationView->addMenuItem("Home");
    auto* item2 = m_navigationView->addMenuItem("Settings");
    
    QSignalSpy selectionSpy(m_navigationView, &FluentNavigationView::selectedItemChanged);
    QSignalSpy itemInvokedSpy(m_navigationView, &FluentNavigationView::itemInvoked);
    
    // Simulate item click
    m_navigationView->invokeItem(item1);
    QCOMPARE(itemInvokedSpy.count(), 1);
    
    // Test selection change
    m_navigationView->setSelectedItem(item2);
    QCOMPARE(selectionSpy.count(), 1);
    QCOMPARE(m_navigationView->selectedItem(), item2);
}

void FluentNavigationViewTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_navigationView->setFocus();
    
    m_navigationView->addMenuItem("Item 1");
    m_navigationView->addMenuItem("Item 2");
    m_navigationView->addMenuItem("Item 3");
    
    QSignalSpy selectionSpy(m_navigationView, &FluentNavigationView::selectedItemChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_navigationView, Qt::Key_Down);
    QTest::keyClick(m_navigationView, Qt::Key_Up);
    
    // Test Enter key for selection
    QTest::keyClick(m_navigationView, Qt::Key_Return);
    
    // Test Tab navigation
    QTest::keyClick(m_navigationView, Qt::Key_Tab);
    
    QTest::qWait(100);
}

void FluentNavigationViewTest::testPaneToggle() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QSignalSpy paneOpenSpy(m_navigationView, &FluentNavigationView::isPaneOpenChanged);
    QSignalSpy paneToggledSpy(m_navigationView, &FluentNavigationView::paneToggled);
    
    // Test pane toggle
    bool initialState = m_navigationView->isPaneOpen();
    m_navigationView->togglePane();
    QCOMPARE(m_navigationView->isPaneOpen(), !initialState);
    QCOMPARE(paneOpenSpy.count(), 1);
    QCOMPARE(paneToggledSpy.count(), 1);
    
    // Toggle again
    m_navigationView->togglePane();
    QCOMPARE(m_navigationView->isPaneOpen(), initialState);
    QCOMPARE(paneOpenSpy.count(), 2);
    QCOMPARE(paneToggledSpy.count(), 2);
}

QTEST_MAIN(FluentNavigationViewTest)
#include "FluentNavigationViewTest.moc"
