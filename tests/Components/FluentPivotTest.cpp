// tests/Components/FluentPivotTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentPivot.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentPivotTest : public QObject {
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
    void testTabData();

    // Appearance tests
    void testTabPosition();
    void testTabShape();
    void testTabsClosable();
    void testMovableTabs();

    // Content tests
    void testTabWidget();
    void testCornerWidget();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testTabClick();
    void testTabClose();
    void testKeyboardNavigation();

private:
    FluentPivot* m_pivot{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentPivotTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(600, 400);
}

void FluentPivotTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentPivotTest::init() {
    m_pivot = new FluentPivot(m_testWidget);
}

void FluentPivotTest::cleanup() {
    delete m_pivot;
    m_pivot = nullptr;
}

void FluentPivotTest::testDefaultConstructor() {
    QVERIFY(m_pivot != nullptr);
    QCOMPARE(m_pivot->count(), 0);
    QCOMPARE(m_pivot->currentIndex(), -1);
    QCOMPARE(m_pivot->tabPosition(), FluentPivotTabPosition::Top);
    QVERIFY(!m_pivot->tabsClosable());
    QVERIFY(!m_pivot->movableTabs());
}

void FluentPivotTest::testAddTab() {
    QSignalSpy countSpy(m_pivot, &FluentPivot::countChanged);
    QSignalSpy tabAddedSpy(m_pivot, &FluentPivot::tabAdded);
    
    // Add tab with text only
    auto* widget1 = new QWidget();
    int index1 = m_pivot->addTab(widget1, "Tab 1");
    QCOMPARE(index1, 0);
    QCOMPARE(m_pivot->count(), 1);
    QCOMPARE(m_pivot->tabText(0), "Tab 1");
    QCOMPARE(m_pivot->widget(0), widget1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(tabAddedSpy.count(), 1);
    
    // Add tab with icon and text
    QIcon testIcon(":/test/icon.png");
    auto* widget2 = new QWidget();
    int index2 = m_pivot->addTab(widget2, testIcon, "Tab 2");
    QCOMPARE(index2, 1);
    QCOMPARE(m_pivot->count(), 2);
    QCOMPARE(m_pivot->tabText(1), "Tab 2");
    QCOMPARE(m_pivot->tabIcon(1).name(), testIcon.name());
    QCOMPARE(countSpy.count(), 2);
    
    // Test current index is set for first tab
    QCOMPARE(m_pivot->currentIndex(), 0);
}

void FluentPivotTest::testRemoveTab() {
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    auto* widget3 = new QWidget();
    
    m_pivot->addTab(widget1, "Tab 1");
    m_pivot->addTab(widget2, "Tab 2");
    m_pivot->addTab(widget3, "Tab 3");
    
    QSignalSpy countSpy(m_pivot, &FluentPivot::countChanged);
    QSignalSpy tabRemovedSpy(m_pivot, &FluentPivot::tabRemoved);
    QSignalSpy currentIndexSpy(m_pivot, &FluentPivot::currentIndexChanged);
    
    m_pivot->removeTab(1); // Remove "Tab 2"
    QCOMPARE(m_pivot->count(), 2);
    QCOMPARE(m_pivot->tabText(0), "Tab 1");
    QCOMPARE(m_pivot->tabText(1), "Tab 3");
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(tabRemovedSpy.count(), 1);
    
    // Test removing invalid index
    m_pivot->removeTab(10);
    QCOMPARE(m_pivot->count(), 2); // Should remain unchanged
}

void FluentPivotTest::testClearTabs() {
    m_pivot->addTab(new QWidget(), "Tab 1");
    m_pivot->addTab(new QWidget(), "Tab 2");
    m_pivot->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy countSpy(m_pivot, &FluentPivot::countChanged);
    QSignalSpy tabsClearedSpy(m_pivot, &FluentPivot::tabsCleared);
    QSignalSpy currentIndexSpy(m_pivot, &FluentPivot::currentIndexChanged);
    
    m_pivot->clear();
    QCOMPARE(m_pivot->count(), 0);
    QCOMPARE(m_pivot->currentIndex(), -1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(tabsClearedSpy.count(), 1);
    QCOMPARE(currentIndexSpy.count(), 1);
}

void FluentPivotTest::testTabCount() {
    QCOMPARE(m_pivot->count(), 0);
    
    m_pivot->addTab(new QWidget(), "Tab 1");
    QCOMPARE(m_pivot->count(), 1);
    
    m_pivot->addTab(new QWidget(), "Tab 2");
    QCOMPARE(m_pivot->count(), 2);
    
    m_pivot->removeTab(0);
    QCOMPARE(m_pivot->count(), 1);
}

void FluentPivotTest::testCurrentIndex() {
    m_pivot->addTab(new QWidget(), "Tab 1");
    m_pivot->addTab(new QWidget(), "Tab 2");
    m_pivot->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy currentIndexSpy(m_pivot, &FluentPivot::currentIndexChanged);
    QSignalSpy currentChangedSpy(m_pivot, &FluentPivot::currentChanged);
    
    m_pivot->setCurrentIndex(1);
    QCOMPARE(m_pivot->currentIndex(), 1);
    QCOMPARE(currentIndexSpy.count(), 1);
    QCOMPARE(currentChangedSpy.count(), 1);
    
    // Test invalid index
    m_pivot->setCurrentIndex(10);
    QCOMPARE(m_pivot->currentIndex(), 1); // Should remain unchanged
    
    m_pivot->setCurrentIndex(-1);
    QCOMPARE(m_pivot->currentIndex(), -1);
    QCOMPARE(currentIndexSpy.count(), 2);
}

void FluentPivotTest::testCurrentTab() {
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    
    m_pivot->addTab(widget1, "Tab 1");
    m_pivot->addTab(widget2, "Tab 2");
    
    QCOMPARE(m_pivot->currentWidget(), widget1);
    
    m_pivot->setCurrentIndex(1);
    QCOMPARE(m_pivot->currentWidget(), widget2);
    
    // Test setting current widget
    m_pivot->setCurrentWidget(widget1);
    QCOMPARE(m_pivot->currentIndex(), 0);
    QCOMPARE(m_pivot->currentWidget(), widget1);
}

void FluentPivotTest::testSetCurrentIndex() {
    m_pivot->addTab(new QWidget(), "Tab 1");
    m_pivot->addTab(new QWidget(), "Tab 2");
    m_pivot->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy tabBarClickedSpy(m_pivot, &FluentPivot::tabBarClicked);
    
    // Test programmatic selection
    m_pivot->setCurrentIndex(2);
    QCOMPARE(m_pivot->currentIndex(), 2);
    
    // Test bounds
    m_pivot->setCurrentIndex(-5);
    QCOMPARE(m_pivot->currentIndex(), -1);
    
    m_pivot->setCurrentIndex(100);
    QCOMPARE(m_pivot->currentIndex(), -1); // Should be invalid
}

void FluentPivotTest::testTabText() {
    m_pivot->addTab(new QWidget(), "Original Text");
    
    QSignalSpy tabTextSpy(m_pivot, &FluentPivot::tabTextChanged);
    
    m_pivot->setTabText(0, "Updated Text");
    QCOMPARE(m_pivot->tabText(0), "Updated Text");
    QCOMPARE(tabTextSpy.count(), 1);
    
    // Test invalid index
    QString invalidText = m_pivot->tabText(10);
    QCOMPARE(invalidText, QString());
    
    m_pivot->setTabText(10, "Invalid");
    QCOMPARE(tabTextSpy.count(), 1); // Should not emit signal
}

void FluentPivotTest::testTabIcon() {
    QIcon originalIcon(":/test/original.png");
    m_pivot->addTab(new QWidget(), originalIcon, "Tab");
    
    QSignalSpy tabIconSpy(m_pivot, &FluentPivot::tabIconChanged);
    
    QIcon newIcon(":/test/new.png");
    m_pivot->setTabIcon(0, newIcon);
    QCOMPARE(m_pivot->tabIcon(0).name(), newIcon.name());
    QCOMPARE(tabIconSpy.count(), 1);
    
    // Test null icon
    m_pivot->setTabIcon(0, QIcon());
    QVERIFY(m_pivot->tabIcon(0).isNull());
    QCOMPARE(tabIconSpy.count(), 2);
}

void FluentPivotTest::testTabEnabled() {
    m_pivot->addTab(new QWidget(), "Tab");
    
    QSignalSpy tabEnabledSpy(m_pivot, &FluentPivot::tabEnabledChanged);
    
    QVERIFY(m_pivot->isTabEnabled(0)); // Default should be enabled
    
    m_pivot->setTabEnabled(0, false);
    QVERIFY(!m_pivot->isTabEnabled(0));
    QCOMPARE(tabEnabledSpy.count(), 1);
    
    m_pivot->setTabEnabled(0, true);
    QVERIFY(m_pivot->isTabEnabled(0));
    QCOMPARE(tabEnabledSpy.count(), 2);
}

void FluentPivotTest::testTabVisible() {
    m_pivot->addTab(new QWidget(), "Tab");
    
    QSignalSpy tabVisibleSpy(m_pivot, &FluentPivot::tabVisibleChanged);
    
    QVERIFY(m_pivot->isTabVisible(0)); // Default should be visible
    
    m_pivot->setTabVisible(0, false);
    QVERIFY(!m_pivot->isTabVisible(0));
    QCOMPARE(tabVisibleSpy.count(), 1);
    
    m_pivot->setTabVisible(0, true);
    QVERIFY(m_pivot->isTabVisible(0));
    QCOMPARE(tabVisibleSpy.count(), 2);
}

void FluentPivotTest::testTabData() {
    m_pivot->addTab(new QWidget(), "Tab");
    
    QVariant testData = QVariant::fromValue(QString("test_data"));
    m_pivot->setTabData(0, testData);
    QCOMPARE(m_pivot->tabData(0), testData);
    
    // Test invalid index
    QVariant invalidData = m_pivot->tabData(10);
    QVERIFY(!invalidData.isValid());
}

void FluentPivotTest::testTabPosition() {
    QSignalSpy positionSpy(m_pivot, &FluentPivot::tabPositionChanged);
    
    m_pivot->setTabPosition(FluentPivotTabPosition::Bottom);
    QCOMPARE(m_pivot->tabPosition(), FluentPivotTabPosition::Bottom);
    QCOMPARE(positionSpy.count(), 1);
    
    m_pivot->setTabPosition(FluentPivotTabPosition::Left);
    QCOMPARE(m_pivot->tabPosition(), FluentPivotTabPosition::Left);
    QCOMPARE(positionSpy.count(), 2);
    
    m_pivot->setTabPosition(FluentPivotTabPosition::Right);
    QCOMPARE(m_pivot->tabPosition(), FluentPivotTabPosition::Right);
    QCOMPARE(positionSpy.count(), 3);
}

void FluentPivotTest::testTabShape() {
    QSignalSpy shapeSpy(m_pivot, &FluentPivot::tabShapeChanged);
    
    m_pivot->setTabShape(FluentPivotTabShape::Triangular);
    QCOMPARE(m_pivot->tabShape(), FluentPivotTabShape::Triangular);
    QCOMPARE(shapeSpy.count(), 1);
    
    m_pivot->setTabShape(FluentPivotTabShape::Rounded);
    QCOMPARE(m_pivot->tabShape(), FluentPivotTabShape::Rounded);
    QCOMPARE(shapeSpy.count(), 2);
}

void FluentPivotTest::testTabsClosable() {
    QSignalSpy closableSpy(m_pivot, &FluentPivot::tabsClosableChanged);
    
    m_pivot->setTabsClosable(true);
    QVERIFY(m_pivot->tabsClosable());
    QCOMPARE(closableSpy.count(), 1);
    
    m_pivot->setTabsClosable(false);
    QVERIFY(!m_pivot->tabsClosable());
    QCOMPARE(closableSpy.count(), 2);
}

void FluentPivotTest::testMovableTabs() {
    QSignalSpy movableSpy(m_pivot, &FluentPivot::movableTabsChanged);
    
    m_pivot->setMovableTabs(true);
    QVERIFY(m_pivot->movableTabs());
    QCOMPARE(movableSpy.count(), 1);
    
    m_pivot->setMovableTabs(false);
    QVERIFY(!m_pivot->movableTabs());
    QCOMPARE(movableSpy.count(), 2);
}

void FluentPivotTest::testTabWidget() {
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    
    m_pivot->addTab(widget1, "Tab 1");
    m_pivot->addTab(widget2, "Tab 2");
    
    QCOMPARE(m_pivot->widget(0), widget1);
    QCOMPARE(m_pivot->widget(1), widget2);
    
    // Test invalid index
    QCOMPARE(m_pivot->widget(10), nullptr);
    
    // Test index of widget
    QCOMPARE(m_pivot->indexOf(widget1), 0);
    QCOMPARE(m_pivot->indexOf(widget2), 1);
    
    auto* nonExistentWidget = new QWidget();
    QCOMPARE(m_pivot->indexOf(nonExistentWidget), -1);
    delete nonExistentWidget;
}

void FluentPivotTest::testCornerWidget() {
    auto* cornerWidget = new QWidget();
    m_pivot->setCornerWidget(cornerWidget, Qt::TopRightCorner);
    QCOMPARE(m_pivot->cornerWidget(Qt::TopRightCorner), cornerWidget);
    
    // Test null corner widget
    m_pivot->setCornerWidget(nullptr, Qt::TopRightCorner);
    QCOMPARE(m_pivot->cornerWidget(Qt::TopRightCorner), nullptr);
}

void FluentPivotTest::testSignalEmission() {
    QSignalSpy countSpy(m_pivot, &FluentPivot::countChanged);
    QSignalSpy tabAddedSpy(m_pivot, &FluentPivot::tabAdded);
    QSignalSpy tabRemovedSpy(m_pivot, &FluentPivot::tabRemoved);
    QSignalSpy tabsClearedSpy(m_pivot, &FluentPivot::tabsCleared);
    QSignalSpy currentIndexSpy(m_pivot, &FluentPivot::currentIndexChanged);
    QSignalSpy currentChangedSpy(m_pivot, &FluentPivot::currentChanged);
    QSignalSpy tabBarClickedSpy(m_pivot, &FluentPivot::tabBarClicked);
    QSignalSpy tabCloseRequestedSpy(m_pivot, &FluentPivot::tabCloseRequested);
    QSignalSpy tabTextSpy(m_pivot, &FluentPivot::tabTextChanged);
    QSignalSpy tabIconSpy(m_pivot, &FluentPivot::tabIconChanged);
    
    // Test all signals
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    m_pivot->addTab(widget1, "Tab 1");
    m_pivot->addTab(widget2, "Tab 2");
    m_pivot->setCurrentIndex(1);
    m_pivot->setTabText(0, "Updated Tab 1");
    m_pivot->setTabIcon(0, QIcon());
    m_pivot->removeTab(1);
    m_pivot->clear();
    
    QCOMPARE(countSpy.count(), 4); // add, add, remove, clear
    QCOMPARE(tabAddedSpy.count(), 2);
    QCOMPARE(tabRemovedSpy.count(), 1);
    QCOMPARE(tabsClearedSpy.count(), 1);
    QCOMPARE(currentIndexSpy.count(), 3); // first tab, setCurrentIndex, clear
    QCOMPARE(currentChangedSpy.count(), 3);
    QCOMPARE(tabTextSpy.count(), 1);
    QCOMPARE(tabIconSpy.count(), 1);
}

void FluentPivotTest::testTabClick() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_pivot->addTab(new QWidget(), "Tab 1");
    m_pivot->addTab(new QWidget(), "Tab 2");
    m_pivot->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy tabBarClickedSpy(m_pivot, &FluentPivot::tabBarClicked);
    QSignalSpy currentIndexSpy(m_pivot, &FluentPivot::currentIndexChanged);
    
    // Simulate tab click
    m_pivot->setCurrentIndex(2);
    QCOMPARE(m_pivot->currentIndex(), 2);
    QCOMPARE(currentIndexSpy.count(), 1);
}

void FluentPivotTest::testTabClose() {
    m_pivot->setTabsClosable(true);
    m_pivot->addTab(new QWidget(), "Tab 1");
    m_pivot->addTab(new QWidget(), "Tab 2");
    
    QSignalSpy tabCloseRequestedSpy(m_pivot, &FluentPivot::tabCloseRequested);
    
    // Simulate close request
    emit m_pivot->tabCloseRequested(0);
    QCOMPARE(tabCloseRequestedSpy.count(), 1);
    QCOMPARE(tabCloseRequestedSpy.first().first().toInt(), 0);
}

void FluentPivotTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_pivot->setFocus();
    
    m_pivot->addTab(new QWidget(), "Tab 1");
    m_pivot->addTab(new QWidget(), "Tab 2");
    m_pivot->addTab(new QWidget(), "Tab 3");
    
    QSignalSpy currentIndexSpy(m_pivot, &FluentPivot::currentIndexChanged);
    
    // Test Ctrl+Tab navigation
    QTest::keyClick(m_pivot, Qt::Key_Tab, Qt::ControlModifier);
    
    // Test arrow key navigation
    QTest::keyClick(m_pivot, Qt::Key_Right);
    QTest::keyClick(m_pivot, Qt::Key_Left);
    
    // Test Home/End keys
    QTest::keyClick(m_pivot, Qt::Key_End);
    QCOMPARE(m_pivot->currentIndex(), 2);
    
    QTest::keyClick(m_pivot, Qt::Key_Home);
    QCOMPARE(m_pivot->currentIndex(), 0);
    
    QTest::qWait(100);
}

QTEST_MAIN(FluentPivotTest)
#include "FluentPivotTest.moc"
