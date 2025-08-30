// tests/Components/FluentContextMenuTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentContextMenu.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentContextMenuTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Menu item tests
    void testAddAction();
    void testAddSeparator();
    void testAddSubMenu();
    void testRemoveAction();
    void testClearMenu();

    // Menu properties tests
    void testMenuTitle();
    void testMenuIcon();
    void testTearOffEnabled();

    // Action properties tests
    void testActionText();
    void testActionIcon();
    void testActionEnabled();
    void testActionVisible();
    void testActionCheckable();
    void testActionChecked();

    // Popup tests
    void testPopupAt();
    void testPopupAtCursor();
    void testExec();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testActionTrigger();
    void testKeyboardNavigation();

private:
    FluentContextMenu* m_contextMenu{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentContextMenuTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentContextMenuTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentContextMenuTest::init() {
    m_contextMenu = new FluentContextMenu(m_testWidget);
}

void FluentContextMenuTest::cleanup() {
    delete m_contextMenu;
    m_contextMenu = nullptr;
}

void FluentContextMenuTest::testDefaultConstructor() {
    QVERIFY(m_contextMenu != nullptr);
    QCOMPARE(m_contextMenu->actions().count(), 0);
    QCOMPARE(m_contextMenu->title(), QString());
    QVERIFY(!m_contextMenu->isTearOffEnabled());
}

void FluentContextMenuTest::testAddAction() {
    QSignalSpy actionAddedSpy(m_contextMenu, &FluentContextMenu::actionAdded);
    
    // Add action with text only
    QAction* action1 = m_contextMenu->addAction("Action 1");
    QVERIFY(action1 != nullptr);
    QCOMPARE(action1->text(), "Action 1");
    QCOMPARE(m_contextMenu->actions().count(), 1);
    QCOMPARE(actionAddedSpy.count(), 1);
    
    // Add action with icon and text
    QIcon testIcon(":/test/icon.png");
    QAction* action2 = m_contextMenu->addAction(testIcon, "Action 2");
    QVERIFY(action2 != nullptr);
    QCOMPARE(action2->text(), "Action 2");
    QCOMPARE(action2->icon().name(), testIcon.name());
    QCOMPARE(m_contextMenu->actions().count(), 2);
    QCOMPARE(actionAddedSpy.count(), 2);
    
    // Add action with shortcut
    QAction* action3 = m_contextMenu->addAction("Action 3", QKeySequence::Copy);
    QVERIFY(action3 != nullptr);
    QCOMPARE(action3->shortcut(), QKeySequence::Copy);
    QCOMPARE(m_contextMenu->actions().count(), 3);
}

void FluentContextMenuTest::testAddSeparator() {
    QSignalSpy actionAddedSpy(m_contextMenu, &FluentContextMenu::actionAdded);
    
    m_contextMenu->addAction("Action 1");
    QAction* separator = m_contextMenu->addSeparator();
    m_contextMenu->addAction("Action 2");
    
    QVERIFY(separator != nullptr);
    QVERIFY(separator->isSeparator());
    QCOMPARE(m_contextMenu->actions().count(), 3);
    QCOMPARE(actionAddedSpy.count(), 3);
}

void FluentContextMenuTest::testAddSubMenu() {
    QSignalSpy subMenuAddedSpy(m_contextMenu, &FluentContextMenu::subMenuAdded);
    
    FluentContextMenu* subMenu = m_contextMenu->addSubMenu("Sub Menu");
    QVERIFY(subMenu != nullptr);
    QCOMPARE(subMenu->title(), "Sub Menu");
    QCOMPARE(subMenuAddedSpy.count(), 1);
    
    // Add actions to submenu
    subMenu->addAction("Sub Action 1");
    subMenu->addAction("Sub Action 2");
    QCOMPARE(subMenu->actions().count(), 2);
    
    // Add submenu with icon
    QIcon menuIcon(":/test/menu.png");
    FluentContextMenu* iconSubMenu = m_contextMenu->addSubMenu(menuIcon, "Icon Sub Menu");
    QVERIFY(iconSubMenu != nullptr);
    QCOMPARE(iconSubMenu->title(), "Icon Sub Menu");
    QCOMPARE(iconSubMenu->icon().name(), menuIcon.name());
}

void FluentContextMenuTest::testRemoveAction() {
    QAction* action1 = m_contextMenu->addAction("Action 1");
    QAction* action2 = m_contextMenu->addAction("Action 2");
    QAction* action3 = m_contextMenu->addAction("Action 3");
    
    QSignalSpy actionRemovedSpy(m_contextMenu, &FluentContextMenu::actionRemoved);
    
    m_contextMenu->removeAction(action2);
    QCOMPARE(m_contextMenu->actions().count(), 2);
    QVERIFY(!m_contextMenu->actions().contains(action2));
    QCOMPARE(actionRemovedSpy.count(), 1);
    
    // Test removing non-existent action
    QAction* nonExistentAction = new QAction("Non-existent", this);
    m_contextMenu->removeAction(nonExistentAction);
    QCOMPARE(m_contextMenu->actions().count(), 2); // Should remain unchanged
    delete nonExistentAction;
}

void FluentContextMenuTest::testClearMenu() {
    m_contextMenu->addAction("Action 1");
    m_contextMenu->addAction("Action 2");
    m_contextMenu->addSeparator();
    m_contextMenu->addAction("Action 3");
    
    QSignalSpy menuClearedSpy(m_contextMenu, &FluentContextMenu::menuCleared);
    
    m_contextMenu->clear();
    QCOMPARE(m_contextMenu->actions().count(), 0);
    QCOMPARE(menuClearedSpy.count(), 1);
}

void FluentContextMenuTest::testMenuTitle() {
    QSignalSpy titleSpy(m_contextMenu, &FluentContextMenu::titleChanged);
    
    m_contextMenu->setTitle("Context Menu");
    QCOMPARE(m_contextMenu->title(), "Context Menu");
    QCOMPARE(titleSpy.count(), 1);
    
    // Test empty title
    m_contextMenu->setTitle("");
    QCOMPARE(m_contextMenu->title(), "");
    QCOMPARE(titleSpy.count(), 2);
}

void FluentContextMenuTest::testMenuIcon() {
    QSignalSpy iconSpy(m_contextMenu, &FluentContextMenu::iconChanged);
    QIcon testIcon(":/test/menu.png");
    
    m_contextMenu->setIcon(testIcon);
    QCOMPARE(m_contextMenu->icon().name(), testIcon.name());
    QCOMPARE(iconSpy.count(), 1);
    
    // Test null icon
    m_contextMenu->setIcon(QIcon());
    QVERIFY(m_contextMenu->icon().isNull());
    QCOMPARE(iconSpy.count(), 2);
}

void FluentContextMenuTest::testTearOffEnabled() {
    QSignalSpy tearOffSpy(m_contextMenu, &FluentContextMenu::tearOffEnabledChanged);
    
    m_contextMenu->setTearOffEnabled(true);
    QVERIFY(m_contextMenu->isTearOffEnabled());
    QCOMPARE(tearOffSpy.count(), 1);
    
    m_contextMenu->setTearOffEnabled(false);
    QVERIFY(!m_contextMenu->isTearOffEnabled());
    QCOMPARE(tearOffSpy.count(), 2);
}

void FluentContextMenuTest::testActionText() {
    QAction* action = m_contextMenu->addAction("Original Text");
    
    action->setText("Updated Text");
    QCOMPARE(action->text(), "Updated Text");
    
    // Test action with mnemonic
    action->setText("&File");
    QCOMPARE(action->text(), "&File");
}

void FluentContextMenuTest::testActionIcon() {
    QAction* action = m_contextMenu->addAction("Action");
    QIcon testIcon(":/test/action.png");
    
    action->setIcon(testIcon);
    QCOMPARE(action->icon().name(), testIcon.name());
    
    // Test removing icon
    action->setIcon(QIcon());
    QVERIFY(action->icon().isNull());
}

void FluentContextMenuTest::testActionEnabled() {
    QAction* action = m_contextMenu->addAction("Action");
    
    QVERIFY(action->isEnabled()); // Default should be enabled
    
    action->setEnabled(false);
    QVERIFY(!action->isEnabled());
    
    action->setEnabled(true);
    QVERIFY(action->isEnabled());
}

void FluentContextMenuTest::testActionVisible() {
    QAction* action = m_contextMenu->addAction("Action");
    
    QVERIFY(action->isVisible()); // Default should be visible
    
    action->setVisible(false);
    QVERIFY(!action->isVisible());
    
    action->setVisible(true);
    QVERIFY(action->isVisible());
}

void FluentContextMenuTest::testActionCheckable() {
    QAction* action = m_contextMenu->addAction("Checkable Action");
    
    QVERIFY(!action->isCheckable()); // Default should not be checkable
    
    action->setCheckable(true);
    QVERIFY(action->isCheckable());
    QVERIFY(!action->isChecked()); // Should not be checked initially
    
    action->setCheckable(false);
    QVERIFY(!action->isCheckable());
}

void FluentContextMenuTest::testActionChecked() {
    QAction* action = m_contextMenu->addAction("Checkable Action");
    action->setCheckable(true);
    
    QSignalSpy toggledSpy(action, &QAction::toggled);
    
    action->setChecked(true);
    QVERIFY(action->isChecked());
    QCOMPARE(toggledSpy.count(), 1);
    
    action->setChecked(false);
    QVERIFY(!action->isChecked());
    QCOMPARE(toggledSpy.count(), 2);
}

void FluentContextMenuTest::testPopupAt() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_contextMenu->addAction("Action 1");
    m_contextMenu->addAction("Action 2");
    
    QSignalSpy aboutToShowSpy(m_contextMenu, &FluentContextMenu::aboutToShow);
    
    // Test popup at specific position
    QPoint popupPos(100, 100);
    m_contextMenu->popup(popupPos);
    
    QCOMPARE(aboutToShowSpy.count(), 1);
    QVERIFY(m_contextMenu->isVisible());
    
    m_contextMenu->hide();
}

void FluentContextMenuTest::testPopupAtCursor() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_contextMenu->addAction("Action 1");
    m_contextMenu->addAction("Action 2");
    
    QSignalSpy aboutToShowSpy(m_contextMenu, &FluentContextMenu::aboutToShow);
    
    // Test popup at cursor position
    m_contextMenu->popup(QCursor::pos());
    
    QCOMPARE(aboutToShowSpy.count(), 1);
    QVERIFY(m_contextMenu->isVisible());
    
    m_contextMenu->hide();
}

void FluentContextMenuTest::testExec() {
    m_contextMenu->addAction("Action 1");
    m_contextMenu->addAction("Action 2");
    
    // Test exec (non-blocking for unit tests)
    // In a real scenario, exec() would block until an action is selected
    // For testing, we just verify it can be called without crashing
    QTimer::singleShot(100, m_contextMenu, &FluentContextMenu::hide);
    QAction* selectedAction = m_contextMenu->exec(QPoint(100, 100));
    
    // selectedAction might be null if no action was selected
    Q_UNUSED(selectedAction)
}

void FluentContextMenuTest::testSignalEmission() {
    QSignalSpy actionAddedSpy(m_contextMenu, &FluentContextMenu::actionAdded);
    QSignalSpy actionRemovedSpy(m_contextMenu, &FluentContextMenu::actionRemoved);
    QSignalSpy subMenuAddedSpy(m_contextMenu, &FluentContextMenu::subMenuAdded);
    QSignalSpy menuClearedSpy(m_contextMenu, &FluentContextMenu::menuCleared);
    QSignalSpy titleSpy(m_contextMenu, &FluentContextMenu::titleChanged);
    QSignalSpy iconSpy(m_contextMenu, &FluentContextMenu::iconChanged);
    QSignalSpy tearOffSpy(m_contextMenu, &FluentContextMenu::tearOffEnabledChanged);
    QSignalSpy aboutToShowSpy(m_contextMenu, &FluentContextMenu::aboutToShow);
    QSignalSpy aboutToHideSpy(m_contextMenu, &FluentContextMenu::aboutToHide);
    
    // Test all signals
    QAction* action = m_contextMenu->addAction("Test Action");
    m_contextMenu->addSubMenu("Sub Menu");
    m_contextMenu->removeAction(action);
    m_contextMenu->setTitle("Test Menu");
    m_contextMenu->setIcon(QIcon());
    m_contextMenu->setTearOffEnabled(true);
    m_contextMenu->clear();
    
    // Show and hide to test visibility signals
    m_contextMenu->addAction("Action"); // Add action for show/hide test
    m_contextMenu->show();
    m_contextMenu->hide();
    
    QCOMPARE(actionAddedSpy.count(), 2); // One from addAction, one from re-adding for show test
    QCOMPARE(actionRemovedSpy.count(), 1);
    QCOMPARE(subMenuAddedSpy.count(), 1);
    QCOMPARE(menuClearedSpy.count(), 1);
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(iconSpy.count(), 1);
    QCOMPARE(tearOffSpy.count(), 1);
    QCOMPARE(aboutToShowSpy.count(), 1);
    QCOMPARE(aboutToHideSpy.count(), 1);
}

void FluentContextMenuTest::testActionTrigger() {
    QAction* action1 = m_contextMenu->addAction("Action 1");
    QAction* action2 = m_contextMenu->addAction("Action 2");
    
    QSignalSpy action1Spy(action1, &QAction::triggered);
    QSignalSpy action2Spy(action2, &QAction::triggered);
    QSignalSpy menuTriggeredSpy(m_contextMenu, &FluentContextMenu::triggered);
    
    // Trigger actions programmatically
    action1->trigger();
    QCOMPARE(action1Spy.count(), 1);
    QCOMPARE(menuTriggeredSpy.count(), 1);
    
    action2->trigger();
    QCOMPARE(action2Spy.count(), 1);
    QCOMPARE(menuTriggeredSpy.count(), 2);
    
    // Test disabled action (should not trigger)
    action1->setEnabled(false);
    action1->trigger();
    QCOMPARE(action1Spy.count(), 1); // Should remain 1
}

void FluentContextMenuTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QAction* action1 = m_contextMenu->addAction("&Action 1");
    QAction* action2 = m_contextMenu->addAction("A&ction 2");
    QAction* action3 = m_contextMenu->addAction("Act&ion 3");
    
    action1->setShortcut(QKeySequence("Ctrl+1"));
    action2->setShortcut(QKeySequence("Ctrl+2"));
    
    m_contextMenu->show();
    QTest::qWaitForWindowExposed(m_contextMenu);
    
    QSignalSpy action1Spy(action1, &QAction::triggered);
    QSignalSpy action2Spy(action2, &QAction::triggered);
    
    // Test arrow key navigation
    QTest::keyClick(m_contextMenu, Qt::Key_Down);
    QTest::keyClick(m_contextMenu, Qt::Key_Up);
    
    // Test mnemonic keys
    QTest::keyClick(m_contextMenu, Qt::Key_A, Qt::AltModifier);
    
    // Test Enter key to trigger selected action
    QTest::keyClick(m_contextMenu, Qt::Key_Return);
    
    // Test Escape key to close menu
    QTest::keyClick(m_contextMenu, Qt::Key_Escape);
    
    m_contextMenu->hide();
}

QTEST_MAIN(FluentContextMenuTest)
#include "FluentContextMenuTest.moc"
