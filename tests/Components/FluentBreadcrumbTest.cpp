// tests/Components/FluentBreadcrumbTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentBreadcrumb.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentBreadcrumbTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Item management tests
    void testAddItem();
    void testRemoveItem();
    void testClearItems();
    void testItemCount();

    // Navigation tests
    void testItemClick();
    void testCurrentItem();
    void testNavigateToItem();

    // Appearance tests
    void testSeparator();
    void testMaxVisibleItems();
    void testShowRoot();

    // Content tests
    void testItemText();
    void testItemIcon();
    void testItemData();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testKeyboardNavigation();
    void testMouseInteraction();

private:
    FluentBreadcrumb* m_breadcrumb{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentBreadcrumbTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentBreadcrumbTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentBreadcrumbTest::init() {
    m_breadcrumb = new FluentBreadcrumb(m_testWidget);
}

void FluentBreadcrumbTest::cleanup() {
    delete m_breadcrumb;
    m_breadcrumb = nullptr;
}

void FluentBreadcrumbTest::testDefaultConstructor() {
    QVERIFY(m_breadcrumb != nullptr);
    QCOMPARE(m_breadcrumb->itemCount(), 0);
    QCOMPARE(m_breadcrumb->currentItem(), -1);
    QVERIFY(m_breadcrumb->showRoot());
    QVERIFY(m_breadcrumb->maxVisibleItems() > 0);
}

void FluentBreadcrumbTest::testAddItem() {
    QSignalSpy itemCountSpy(m_breadcrumb, &FluentBreadcrumb::itemCountChanged);
    QSignalSpy itemAddedSpy(m_breadcrumb, &FluentBreadcrumb::itemAdded);
    
    m_breadcrumb->addItem("Home");
    QCOMPARE(m_breadcrumb->itemCount(), 1);
    QCOMPARE(itemCountSpy.count(), 1);
    QCOMPARE(itemAddedSpy.count(), 1);
    
    m_breadcrumb->addItem("Documents", QIcon(":/icons/folder.png"));
    QCOMPARE(m_breadcrumb->itemCount(), 2);
    QCOMPARE(itemCountSpy.count(), 2);
    QCOMPARE(itemAddedSpy.count(), 2);
    
    // Test adding with data
    QVariant userData = QVariant::fromValue(QString("path/to/documents"));
    m_breadcrumb->addItem("Projects", QIcon(), userData);
    QCOMPARE(m_breadcrumb->itemCount(), 3);
}

void FluentBreadcrumbTest::testRemoveItem() {
    // Add some items first
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->addItem("Projects");
    
    QSignalSpy itemCountSpy(m_breadcrumb, &FluentBreadcrumb::itemCountChanged);
    QSignalSpy itemRemovedSpy(m_breadcrumb, &FluentBreadcrumb::itemRemoved);
    
    m_breadcrumb->removeItem(1); // Remove "Documents"
    QCOMPARE(m_breadcrumb->itemCount(), 2);
    QCOMPARE(itemCountSpy.count(), 1);
    QCOMPARE(itemRemovedSpy.count(), 1);
    
    // Test removing invalid index
    m_breadcrumb->removeItem(10);
    QCOMPARE(m_breadcrumb->itemCount(), 2); // Should remain unchanged
}

void FluentBreadcrumbTest::testClearItems() {
    // Add some items first
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->addItem("Projects");
    
    QSignalSpy itemCountSpy(m_breadcrumb, &FluentBreadcrumb::itemCountChanged);
    QSignalSpy clearedSpy(m_breadcrumb, &FluentBreadcrumb::itemsCleared);
    
    m_breadcrumb->clearItems();
    QCOMPARE(m_breadcrumb->itemCount(), 0);
    QCOMPARE(m_breadcrumb->currentItem(), -1);
    QCOMPARE(itemCountSpy.count(), 1);
    QCOMPARE(clearedSpy.count(), 1);
}

void FluentBreadcrumbTest::testItemCount() {
    QCOMPARE(m_breadcrumb->itemCount(), 0);
    
    m_breadcrumb->addItem("Item 1");
    QCOMPARE(m_breadcrumb->itemCount(), 1);
    
    m_breadcrumb->addItem("Item 2");
    QCOMPARE(m_breadcrumb->itemCount(), 2);
    
    m_breadcrumb->removeItem(0);
    QCOMPARE(m_breadcrumb->itemCount(), 1);
}

void FluentBreadcrumbTest::testItemClick() {
    // Add some items
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->addItem("Projects");
    
    QSignalSpy itemClickedSpy(m_breadcrumb, &FluentBreadcrumb::itemClicked);
    QSignalSpy currentItemSpy(m_breadcrumb, &FluentBreadcrumb::currentItemChanged);
    
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Simulate clicking on an item (this would typically be done through mouse events)
    m_breadcrumb->navigateToItem(1);
    
    QCOMPARE(m_breadcrumb->currentItem(), 1);
    QCOMPARE(currentItemSpy.count(), 1);
}

void FluentBreadcrumbTest::testCurrentItem() {
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->addItem("Projects");
    
    QSignalSpy currentItemSpy(m_breadcrumb, &FluentBreadcrumb::currentItemChanged);
    
    m_breadcrumb->setCurrentItem(1);
    QCOMPARE(m_breadcrumb->currentItem(), 1);
    QCOMPARE(currentItemSpy.count(), 1);
    
    // Test invalid index
    m_breadcrumb->setCurrentItem(10);
    QCOMPARE(m_breadcrumb->currentItem(), 1); // Should remain unchanged
    
    m_breadcrumb->setCurrentItem(-1);
    QCOMPARE(m_breadcrumb->currentItem(), -1);
    QCOMPARE(currentItemSpy.count(), 2);
}

void FluentBreadcrumbTest::testNavigateToItem() {
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->addItem("Projects");
    
    QSignalSpy navigationSpy(m_breadcrumb, &FluentBreadcrumb::navigationRequested);
    QSignalSpy currentItemSpy(m_breadcrumb, &FluentBreadcrumb::currentItemChanged);
    
    m_breadcrumb->navigateToItem(2);
    QCOMPARE(m_breadcrumb->currentItem(), 2);
    QCOMPARE(currentItemSpy.count(), 1);
    QCOMPARE(navigationSpy.count(), 1);
    
    // Check signal parameters
    QList<QVariant> arguments = navigationSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 2);
}

void FluentBreadcrumbTest::testSeparator() {
    QSignalSpy separatorSpy(m_breadcrumb, &FluentBreadcrumb::separatorChanged);
    
    m_breadcrumb->setSeparator(" > ");
    QCOMPARE(m_breadcrumb->separator(), " > ");
    QCOMPARE(separatorSpy.count(), 1);
    
    m_breadcrumb->setSeparator(" / ");
    QCOMPARE(m_breadcrumb->separator(), " / ");
    QCOMPARE(separatorSpy.count(), 2);
}

void FluentBreadcrumbTest::testMaxVisibleItems() {
    QSignalSpy maxVisibleSpy(m_breadcrumb, &FluentBreadcrumb::maxVisibleItemsChanged);
    
    m_breadcrumb->setMaxVisibleItems(5);
    QCOMPARE(m_breadcrumb->maxVisibleItems(), 5);
    QCOMPARE(maxVisibleSpy.count(), 1);
    
    // Test invalid value
    m_breadcrumb->setMaxVisibleItems(0);
    QVERIFY(m_breadcrumb->maxVisibleItems() > 0); // Should remain positive
}

void FluentBreadcrumbTest::testShowRoot() {
    QSignalSpy showRootSpy(m_breadcrumb, &FluentBreadcrumb::showRootChanged);
    
    m_breadcrumb->setShowRoot(false);
    QVERIFY(!m_breadcrumb->showRoot());
    QCOMPARE(showRootSpy.count(), 1);
    
    m_breadcrumb->setShowRoot(true);
    QVERIFY(m_breadcrumb->showRoot());
    QCOMPARE(showRootSpy.count(), 2);
}

void FluentBreadcrumbTest::testItemText() {
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    
    QString text = m_breadcrumb->itemText(0);
    QCOMPARE(text, "Home");
    
    text = m_breadcrumb->itemText(1);
    QCOMPARE(text, "Documents");
    
    // Test invalid index
    text = m_breadcrumb->itemText(10);
    QCOMPARE(text, QString());
    
    // Test setting item text
    m_breadcrumb->setItemText(0, "Root");
    text = m_breadcrumb->itemText(0);
    QCOMPARE(text, "Root");
}

void FluentBreadcrumbTest::testItemIcon() {
    QIcon homeIcon(":/icons/home.png");
    QIcon folderIcon(":/icons/folder.png");
    
    m_breadcrumb->addItem("Home", homeIcon);
    m_breadcrumb->addItem("Documents", folderIcon);
    
    QIcon retrievedIcon = m_breadcrumb->itemIcon(0);
    QCOMPARE(retrievedIcon.name(), homeIcon.name());
    
    // Test setting item icon
    QIcon newIcon(":/icons/new.png");
    m_breadcrumb->setItemIcon(0, newIcon);
    retrievedIcon = m_breadcrumb->itemIcon(0);
    QCOMPARE(retrievedIcon.name(), newIcon.name());
}

void FluentBreadcrumbTest::testItemData() {
    QVariant data1 = QVariant::fromValue(QString("/home"));
    QVariant data2 = QVariant::fromValue(QString("/home/documents"));
    
    m_breadcrumb->addItem("Home", QIcon(), data1);
    m_breadcrumb->addItem("Documents", QIcon(), data2);
    
    QVariant retrievedData = m_breadcrumb->itemData(0);
    QCOMPARE(retrievedData.toString(), "/home");
    
    retrievedData = m_breadcrumb->itemData(1);
    QCOMPARE(retrievedData.toString(), "/home/documents");
    
    // Test setting item data
    QVariant newData = QVariant::fromValue(QString("/root"));
    m_breadcrumb->setItemData(0, newData);
    retrievedData = m_breadcrumb->itemData(0);
    QCOMPARE(retrievedData.toString(), "/root");
}

void FluentBreadcrumbTest::testSignalEmission() {
    QSignalSpy itemCountSpy(m_breadcrumb, &FluentBreadcrumb::itemCountChanged);
    QSignalSpy itemAddedSpy(m_breadcrumb, &FluentBreadcrumb::itemAdded);
    QSignalSpy itemRemovedSpy(m_breadcrumb, &FluentBreadcrumb::itemRemoved);
    QSignalSpy itemsCleared(m_breadcrumb, &FluentBreadcrumb::itemsCleared);
    QSignalSpy currentItemSpy(m_breadcrumb, &FluentBreadcrumb::currentItemChanged);
    QSignalSpy itemClickedSpy(m_breadcrumb, &FluentBreadcrumb::itemClicked);
    QSignalSpy navigationSpy(m_breadcrumb, &FluentBreadcrumb::navigationRequested);
    QSignalSpy separatorSpy(m_breadcrumb, &FluentBreadcrumb::separatorChanged);
    QSignalSpy maxVisibleSpy(m_breadcrumb, &FluentBreadcrumb::maxVisibleItemsChanged);
    QSignalSpy showRootSpy(m_breadcrumb, &FluentBreadcrumb::showRootChanged);
    
    // Test all signals
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->removeItem(1);
    m_breadcrumb->setCurrentItem(0);
    m_breadcrumb->navigateToItem(0);
    m_breadcrumb->setSeparator(" > ");
    m_breadcrumb->setMaxVisibleItems(5);
    m_breadcrumb->setShowRoot(false);
    m_breadcrumb->clearItems();
    
    QCOMPARE(itemCountSpy.count(), 4); // add, add, remove, clear
    QCOMPARE(itemAddedSpy.count(), 2);
    QCOMPARE(itemRemovedSpy.count(), 1);
    QCOMPARE(itemsCleared.count(), 1);
    QCOMPARE(currentItemSpy.count(), 2); // setCurrentItem, clearItems
    QCOMPARE(navigationSpy.count(), 1);
    QCOMPARE(separatorSpy.count(), 1);
    QCOMPARE(maxVisibleSpy.count(), 1);
    QCOMPARE(showRootSpy.count(), 1);
}

void FluentBreadcrumbTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_breadcrumb->setFocus();
    
    // Add items for testing
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->addItem("Projects");
    
    QSignalSpy currentItemSpy(m_breadcrumb, &FluentBreadcrumb::currentItemChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_breadcrumb, Qt::Key_Right);
    QTest::keyClick(m_breadcrumb, Qt::Key_Left);
    
    // Test Home/End keys
    QTest::keyClick(m_breadcrumb, Qt::Key_End);
    QTest::keyClick(m_breadcrumb, Qt::Key_Home);
    
    // Test Enter key
    QTest::keyClick(m_breadcrumb, Qt::Key_Return);
}

void FluentBreadcrumbTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add items for testing
    m_breadcrumb->addItem("Home");
    m_breadcrumb->addItem("Documents");
    m_breadcrumb->addItem("Projects");
    
    QSignalSpy itemClickedSpy(m_breadcrumb, &FluentBreadcrumb::itemClicked);
    
    // Test mouse click on breadcrumb
    QTest::mouseClick(m_breadcrumb, Qt::LeftButton);
    
    // Mouse interaction testing is limited without knowing exact item positions
    // In a real implementation, this would test clicking on specific items
}

QTEST_MAIN(FluentBreadcrumbTest)
#include "FluentBreadcrumbTest.moc"
