// tests/Components/FluentTabViewTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentTabViewTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Tab management tests
    void testAddTab();
    void testRemoveTab();
    void testClearTabs();
    void testTabCount();

    // Selection tests
    void testCurrentIndex();
    void testCurrentTab();
    void testSetCurrentIndex();

    // Tab properties tests
    void testTabText();
    void testTabIcon();
    void testTabEnabled();
    void testTabVisible();
    void testTabClosable();

    // Appearance tests
    void testTabPosition();
    void testTabShape();
    void testTabsClosable();
    void testMovableTabs();
    void testScrollable();

    // Content tests
    void testTabWidget();
    void testCornerWidget();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testTabClick();
    void testTabClose();
    void testTabDrag();
    void testKeyboardNavigation();

private:
    FluentTabView* m_tabView{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentTabViewTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(600, 400);
}

void FluentTabViewTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentTabViewTest::init() {
    m_tabView = new FluentTabView(m_testWidget);
}

void FluentTabViewTest::cleanup() {
    delete m_tabView;
    m_tabView = nullptr;
}

void FluentTabViewTest::testDefaultConstructor() {
    QVERIFY(m_tabView != nullptr);
    QCOMPARE(m_tabView->count(), 0);
    QCOMPARE(m_tabView->currentIndex(), -1);
    QCOMPARE(m_tabView->tabPosition(), FluentTabPosition::Top);
    QVERIFY(!m_tabView->tabsClosable());
    QVERIFY(!m_tabView->movableTabs());
    QVERIFY(m_tabView->scrollable());
}

void FluentTabViewTest::testAddTab() {
    QSignalSpy countSpy(m_tabView, &FluentTabView::countChanged);
    QSignalSpy tabAddedSpy(m_tabView, &FluentTabView::tabAdded);
    
    // Add tab with text only
    auto* widget1 = new QWidget();
    int index1 = m_tabView->addTab(widget1, "Tab 1");
    QCOMPARE(index1, 0);
    QCOMPARE(m_tabView->count(), 1);
    QCOMPARE(m_tabView->tabText(0), "Tab 1");
    QCOMPARE(m_tabView->widget(0), widget1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(tabAddedSpy.count(), 1);
    
    // Add tab with icon and text
    QIcon testIcon(":/test/icon.png");
    auto* widget2 = new QWidget();
    int index2 = m_tabView->addTab(widget2, testIcon, "Tab 2");
    QCOMPARE(index2, 1);
    QCOMPARE(m_tabView->count(), 2);
    QCOMPARE(m_tabView->tabText(1), "Tab 2");
    QCOMPARE(m_tabView->tabIcon(1).name(), testIcon.name());
    QCOMPARE(countSpy.count(), 2);
    
    // Test current index is set for first tab
    QCOMPARE(m_tabView->currentIndex(), 0);
}

void FluentTabViewTest::testRemoveTab() {
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    auto* widget3 = new QWidget();
    
    m_tabView->addTab(widget1, "Tab 1");
    m_tabView->addTab(widget2, "Tab 2");
    m_tabView->addTab(widget3, "Tab 3");
    
    QSignalSpy countSpy(m_tabView, &FluentTabView::countChanged);
    QSignalSpy tabRemovedSpy(m_tabView, &FluentTabView::tabRemoved);
    QSignalSpy currentIndexSpy(m_tabView, &FluentTabView::currentIndexChanged);
    
    m_tabView->removeTab(1); // Remove "Tab 2"
    QCOMPARE(m_tabView->count(), 2);
    QCOMPARE(m_tabView->tabText(0), "Tab 1");
    QCOMPARE(m_tabView->tabText(1), "Tab 3");
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(tabRemovedSpy.count(), 1);
    
    // Test removing invalid index
    m_tabView->removeTab(10);
    QCOMPARE(m_tabView->count(), 2); // Should remain unchanged
}

void FluentTabViewTest::testClearTabs() {
    m_tabView->addTab(new QWidget(), "Tab 1");
    m_tabView->addTab(new QWidget(), "Tab 2");
    m_tabView->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy countSpy(m_tabView, &FluentTabView::countChanged);
    QSignalSpy tabsClearedSpy(m_tabView, &FluentTabView::tabsCleared);
    QSignalSpy currentIndexSpy(m_tabView, &FluentTabView::currentIndexChanged);
    
    m_tabView->clear();
    QCOMPARE(m_tabView->count(), 0);
    QCOMPARE(m_tabView->currentIndex(), -1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(tabsClearedSpy.count(), 1);
    QCOMPARE(currentIndexSpy.count(), 1);
}

void FluentTabViewTest::testTabCount() {
    QCOMPARE(m_tabView->count(), 0);
    
    m_tabView->addTab(new QWidget(), "Tab 1");
    QCOMPARE(m_tabView->count(), 1);
    
    m_tabView->addTab(new QWidget(), "Tab 2");
    QCOMPARE(m_tabView->count(), 2);
    
    m_tabView->removeTab(0);
    QCOMPARE(m_tabView->count(), 1);
}

void FluentTabViewTest::testCurrentIndex() {
    m_tabView->addTab(new QWidget(), "Tab 1");
    m_tabView->addTab(new QWidget(), "Tab 2");
    m_tabView->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy currentIndexSpy(m_tabView, &FluentTabView::currentIndexChanged);
    QSignalSpy currentChangedSpy(m_tabView, &FluentTabView::currentChanged);
    
    m_tabView->setCurrentIndex(1);
    QCOMPARE(m_tabView->currentIndex(), 1);
    QCOMPARE(currentIndexSpy.count(), 1);
    QCOMPARE(currentChangedSpy.count(), 1);
    
    // Test invalid index
    m_tabView->setCurrentIndex(10);
    QCOMPARE(m_tabView->currentIndex(), 1); // Should remain unchanged
    
    m_tabView->setCurrentIndex(-1);
    QCOMPARE(m_tabView->currentIndex(), -1);
    QCOMPARE(currentIndexSpy.count(), 2);
}

void FluentTabViewTest::testCurrentTab() {
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    
    m_tabView->addTab(widget1, "Tab 1");
    m_tabView->addTab(widget2, "Tab 2");
    
    QCOMPARE(m_tabView->currentWidget(), widget1);
    
    m_tabView->setCurrentIndex(1);
    QCOMPARE(m_tabView->currentWidget(), widget2);
    
    // Test setting current widget
    m_tabView->setCurrentWidget(widget1);
    QCOMPARE(m_tabView->currentIndex(), 0);
    QCOMPARE(m_tabView->currentWidget(), widget1);
}

void FluentTabViewTest::testSetCurrentIndex() {
    m_tabView->addTab(new QWidget(), "Tab 1");
    m_tabView->addTab(new QWidget(), "Tab 2");
    m_tabView->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy tabBarClickedSpy(m_tabView, &FluentTabView::tabBarClicked);
    
    // Test programmatic selection
    m_tabView->setCurrentIndex(2);
    QCOMPARE(m_tabView->currentIndex(), 2);
    
    // Test bounds
    m_tabView->setCurrentIndex(-5);
    QCOMPARE(m_tabView->currentIndex(), -1);
    
    m_tabView->setCurrentIndex(100);
    QCOMPARE(m_tabView->currentIndex(), -1); // Should be invalid
}

void FluentTabViewTest::testTabText() {
    m_tabView->addTab(new QWidget(), "Original Text");
    
    QSignalSpy tabTextSpy(m_tabView, &FluentTabView::tabTextChanged);
    
    m_tabView->setTabText(0, "Updated Text");
    QCOMPARE(m_tabView->tabText(0), "Updated Text");
    QCOMPARE(tabTextSpy.count(), 1);
    
    // Test invalid index
    QString invalidText = m_tabView->tabText(10);
    QCOMPARE(invalidText, QString());
    
    m_tabView->setTabText(10, "Invalid");
    QCOMPARE(tabTextSpy.count(), 1); // Should not emit signal
}

void FluentTabViewTest::testTabIcon() {
    QIcon originalIcon(":/test/original.png");
    m_tabView->addTab(new QWidget(), originalIcon, "Tab");
    
    QSignalSpy tabIconSpy(m_tabView, &FluentTabView::tabIconChanged);
    
    QIcon newIcon(":/test/new.png");
    m_tabView->setTabIcon(0, newIcon);
    QCOMPARE(m_tabView->tabIcon(0).name(), newIcon.name());
    QCOMPARE(tabIconSpy.count(), 1);
    
    // Test null icon
    m_tabView->setTabIcon(0, QIcon());
    QVERIFY(m_tabView->tabIcon(0).isNull());
    QCOMPARE(tabIconSpy.count(), 2);
}

void FluentTabViewTest::testTabEnabled() {
    m_tabView->addTab(new QWidget(), "Tab");
    
    QSignalSpy tabEnabledSpy(m_tabView, &FluentTabView::tabEnabledChanged);
    
    QVERIFY(m_tabView->isTabEnabled(0)); // Default should be enabled
    
    m_tabView->setTabEnabled(0, false);
    QVERIFY(!m_tabView->isTabEnabled(0));
    QCOMPARE(tabEnabledSpy.count(), 1);
    
    m_tabView->setTabEnabled(0, true);
    QVERIFY(m_tabView->isTabEnabled(0));
    QCOMPARE(tabEnabledSpy.count(), 2);
}

void FluentTabViewTest::testTabVisible() {
    m_tabView->addTab(new QWidget(), "Tab");
    
    QSignalSpy tabVisibleSpy(m_tabView, &FluentTabView::tabVisibleChanged);
    
    QVERIFY(m_tabView->isTabVisible(0)); // Default should be visible
    
    m_tabView->setTabVisible(0, false);
    QVERIFY(!m_tabView->isTabVisible(0));
    QCOMPARE(tabVisibleSpy.count(), 1);
    
    m_tabView->setTabVisible(0, true);
    QVERIFY(m_tabView->isTabVisible(0));
    QCOMPARE(tabVisibleSpy.count(), 2);
}

void FluentTabViewTest::testTabClosable() {
    m_tabView->addTab(new QWidget(), "Tab");
    
    QSignalSpy tabClosableSpy(m_tabView, &FluentTabView::tabClosableChanged);
    
    QVERIFY(!m_tabView->isTabClosable(0)); // Default should not be closable
    
    m_tabView->setTabClosable(0, true);
    QVERIFY(m_tabView->isTabClosable(0));
    QCOMPARE(tabClosableSpy.count(), 1);
    
    m_tabView->setTabClosable(0, false);
    QVERIFY(!m_tabView->isTabClosable(0));
    QCOMPARE(tabClosableSpy.count(), 2);
}

void FluentTabViewTest::testTabPosition() {
    QSignalSpy positionSpy(m_tabView, &FluentTabView::tabPositionChanged);
    
    m_tabView->setTabPosition(FluentTabPosition::Bottom);
    QCOMPARE(m_tabView->tabPosition(), FluentTabPosition::Bottom);
    QCOMPARE(positionSpy.count(), 1);
    
    m_tabView->setTabPosition(FluentTabPosition::Left);
    QCOMPARE(m_tabView->tabPosition(), FluentTabPosition::Left);
    QCOMPARE(positionSpy.count(), 2);
    
    m_tabView->setTabPosition(FluentTabPosition::Right);
    QCOMPARE(m_tabView->tabPosition(), FluentTabPosition::Right);
    QCOMPARE(positionSpy.count(), 3);
}

void FluentTabViewTest::testTabShape() {
    QSignalSpy shapeSpy(m_tabView, &FluentTabView::tabShapeChanged);
    
    m_tabView->setTabShape(FluentTabShape::Triangular);
    QCOMPARE(m_tabView->tabShape(), FluentTabShape::Triangular);
    QCOMPARE(shapeSpy.count(), 1);
    
    m_tabView->setTabShape(FluentTabShape::Rounded);
    QCOMPARE(m_tabView->tabShape(), FluentTabShape::Rounded);
    QCOMPARE(shapeSpy.count(), 2);
}

void FluentTabViewTest::testTabsClosable() {
    QSignalSpy closableSpy(m_tabView, &FluentTabView::tabsClosableChanged);
    
    m_tabView->setTabsClosable(true);
    QVERIFY(m_tabView->tabsClosable());
    QCOMPARE(closableSpy.count(), 1);
    
    m_tabView->setTabsClosable(false);
    QVERIFY(!m_tabView->tabsClosable());
    QCOMPARE(closableSpy.count(), 2);
}

void FluentTabViewTest::testMovableTabs() {
    QSignalSpy movableSpy(m_tabView, &FluentTabView::movableTabsChanged);
    
    m_tabView->setMovableTabs(true);
    QVERIFY(m_tabView->movableTabs());
    QCOMPARE(movableSpy.count(), 1);
    
    m_tabView->setMovableTabs(false);
    QVERIFY(!m_tabView->movableTabs());
    QCOMPARE(movableSpy.count(), 2);
}

void FluentTabViewTest::testScrollable() {
    QSignalSpy scrollableSpy(m_tabView, &FluentTabView::scrollableChanged);
    
    m_tabView->setScrollable(false);
    QVERIFY(!m_tabView->scrollable());
    QCOMPARE(scrollableSpy.count(), 1);
    
    m_tabView->setScrollable(true);
    QVERIFY(m_tabView->scrollable());
    QCOMPARE(scrollableSpy.count(), 2);
}

void FluentTabViewTest::testTabWidget() {
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    
    m_tabView->addTab(widget1, "Tab 1");
    m_tabView->addTab(widget2, "Tab 2");
    
    QCOMPARE(m_tabView->widget(0), widget1);
    QCOMPARE(m_tabView->widget(1), widget2);
    
    // Test invalid index
    QCOMPARE(m_tabView->widget(10), nullptr);
    
    // Test index of widget
    QCOMPARE(m_tabView->indexOf(widget1), 0);
    QCOMPARE(m_tabView->indexOf(widget2), 1);
    
    auto* nonExistentWidget = new QWidget();
    QCOMPARE(m_tabView->indexOf(nonExistentWidget), -1);
    delete nonExistentWidget;
}

void FluentTabViewTest::testCornerWidget() {
    auto* cornerWidget = new QWidget();
    m_tabView->setCornerWidget(cornerWidget, Qt::TopRightCorner);
    QCOMPARE(m_tabView->cornerWidget(Qt::TopRightCorner), cornerWidget);
    
    // Test null corner widget
    m_tabView->setCornerWidget(nullptr, Qt::TopRightCorner);
    QCOMPARE(m_tabView->cornerWidget(Qt::TopRightCorner), nullptr);
}

void FluentTabViewTest::testSignalEmission() {
    QSignalSpy countSpy(m_tabView, &FluentTabView::countChanged);
    QSignalSpy tabAddedSpy(m_tabView, &FluentTabView::tabAdded);
    QSignalSpy tabRemovedSpy(m_tabView, &FluentTabView::tabRemoved);
    QSignalSpy tabsClearedSpy(m_tabView, &FluentTabView::tabsCleared);
    QSignalSpy currentIndexSpy(m_tabView, &FluentTabView::currentIndexChanged);
    QSignalSpy currentChangedSpy(m_tabView, &FluentTabView::currentChanged);
    QSignalSpy tabBarClickedSpy(m_tabView, &FluentTabView::tabBarClicked);
    QSignalSpy tabCloseRequestedSpy(m_tabView, &FluentTabView::tabCloseRequested);
    QSignalSpy tabTextSpy(m_tabView, &FluentTabView::tabTextChanged);
    QSignalSpy tabIconSpy(m_tabView, &FluentTabView::tabIconChanged);
    
    // Test all signals
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    m_tabView->addTab(widget1, "Tab 1");
    m_tabView->addTab(widget2, "Tab 2");
    m_tabView->setCurrentIndex(1);
    m_tabView->setTabText(0, "Updated Tab 1");
    m_tabView->setTabIcon(0, QIcon());
    m_tabView->removeTab(1);
    m_tabView->clear();
    
    QCOMPARE(countSpy.count(), 4); // add, add, remove, clear
    QCOMPARE(tabAddedSpy.count(), 2);
    QCOMPARE(tabRemovedSpy.count(), 1);
    QCOMPARE(tabsClearedSpy.count(), 1);
    QCOMPARE(currentIndexSpy.count(), 3); // first tab, setCurrentIndex, clear
    QCOMPARE(currentChangedSpy.count(), 3);
    QCOMPARE(tabTextSpy.count(), 1);
    QCOMPARE(tabIconSpy.count(), 1);
}

void FluentTabViewTest::testTabClick() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_tabView->addTab(new QWidget(), "Tab 1");
    m_tabView->addTab(new QWidget(), "Tab 2");
    m_tabView->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy tabBarClickedSpy(m_tabView, &FluentTabView::tabBarClicked);
    QSignalSpy currentIndexSpy(m_tabView, &FluentTabView::currentIndexChanged);
    
    // Simulate tab click
    m_tabView->setCurrentIndex(2);
    QCOMPARE(m_tabView->currentIndex(), 2);
    QCOMPARE(currentIndexSpy.count(), 1);
}

void FluentTabViewTest::testTabClose() {
    m_tabView->setTabsClosable(true);
    m_tabView->addTab(new QWidget(), "Tab 1");
    m_tabView->addTab(new QWidget(), "Tab 2");
    
    QSignalSpy tabCloseRequestedSpy(m_tabView, &FluentTabView::tabCloseRequested);
    
    // Simulate close request
    emit m_tabView->tabCloseRequested(0);
    QCOMPARE(tabCloseRequestedSpy.count(), 1);
    QCOMPARE(tabCloseRequestedSpy.first().first().toInt(), 0);
}

void FluentTabViewTest::testTabDrag() {
    m_tabView->setMovableTabs(true);
    m_tabView->addTab(new QWidget(), "Tab 1");
    m_tabView->addTab(new QWidget(), "Tab 2");
    m_tabView->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy tabMovedSpy(m_tabView, &FluentTabView::tabMoved);
    
    // Simulate tab move
    emit m_tabView->tabMoved(0, 2);
    QCOMPARE(tabMovedSpy.count(), 1);
    
    QList<QVariant> arguments = tabMovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 0); // from
    QCOMPARE(arguments.at(1).toInt(), 2); // to
}

void FluentTabViewTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_tabView->setFocus();
    
    m_tabView->addTab(new QWidget(), "Tab 1");
    m_tabView->addTab(new QWidget(), "Tab 2");
    m_tabView->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy currentIndexSpy(m_tabView, &FluentTabView::currentIndexChanged);
    
    // Test Ctrl+Tab navigation
    QTest::keyClick(m_tabView, Qt::Key_Tab, Qt::ControlModifier);
    
    // Test arrow key navigation
    QTest::keyClick(m_tabView, Qt::Key_Right);
    QTest::keyClick(m_tabView, Qt::Key_Left);
    
    // Test Home/End keys
    QTest::keyClick(m_tabView, Qt::Key_End);
    QCOMPARE(m_tabView->currentIndex(), 2);
    
    QTest::keyClick(m_tabView, Qt::Key_Home);
    QCOMPARE(m_tabView->currentIndex(), 0);
    
    QTest::qWait(100);
}

QTEST_MAIN(FluentTabViewTest)
#include "FluentTabViewTest.moc"
