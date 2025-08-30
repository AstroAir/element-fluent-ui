// tests/Components/FluentDropdownTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentDropdown.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentDropdownTest : public QObject {
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

    // Selection tests
    void testCurrentIndex();
    void testCurrentText();
    void testCurrentData();
    void testSetCurrentIndex();

    // Appearance tests
    void testPlaceholderText();
    void testEditable();
    void testMaxVisibleItems();

    // Popup tests
    void testShowPopup();
    void testHidePopup();
    void testPopupVisible();

    // Search tests
    void testSearchEnabled();
    void testSearchText();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardNavigation();

    // Validation tests
    void testItemValidation();

private:
    FluentDropdown* m_dropdown{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentDropdownTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentDropdownTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentDropdownTest::init() {
    m_dropdown = new FluentDropdown(m_testWidget);
}

void FluentDropdownTest::cleanup() {
    delete m_dropdown;
    m_dropdown = nullptr;
}

void FluentDropdownTest::testDefaultConstructor() {
    QVERIFY(m_dropdown != nullptr);
    QCOMPARE(m_dropdown->count(), 0);
    QCOMPARE(m_dropdown->currentIndex(), -1);
    QCOMPARE(m_dropdown->currentText(), QString());
    QVERIFY(!m_dropdown->isEditable());
    QVERIFY(!m_dropdown->popupVisible());
}

void FluentDropdownTest::testAddItem() {
    QSignalSpy countSpy(m_dropdown, &FluentDropdown::countChanged);
    QSignalSpy itemAddedSpy(m_dropdown, &FluentDropdown::itemAdded);
    
    // Add item with text only
    m_dropdown->addItem("Item 1");
    QCOMPARE(m_dropdown->count(), 1);
    QCOMPARE(m_dropdown->itemText(0), "Item 1");
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(itemAddedSpy.count(), 1);
    
    // Add item with icon and text
    QIcon testIcon(":/test/icon.png");
    m_dropdown->addItem(testIcon, "Item 2");
    QCOMPARE(m_dropdown->count(), 2);
    QCOMPARE(m_dropdown->itemText(1), "Item 2");
    QCOMPARE(m_dropdown->itemIcon(1).name(), testIcon.name());
    QCOMPARE(countSpy.count(), 2);
    QCOMPARE(itemAddedSpy.count(), 2);
    
    // Add item with data
    QVariant userData = QVariant::fromValue(QString("user_data"));
    m_dropdown->addItem("Item 3", userData);
    QCOMPARE(m_dropdown->count(), 3);
    QCOMPARE(m_dropdown->itemData(2), userData);
    QCOMPARE(countSpy.count(), 3);
}

void FluentDropdownTest::testRemoveItem() {
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    m_dropdown->addItem("Item 3");
    
    QSignalSpy countSpy(m_dropdown, &FluentDropdown::countChanged);
    QSignalSpy itemRemovedSpy(m_dropdown, &FluentDropdown::itemRemoved);
    
    m_dropdown->removeItem(1); // Remove "Item 2"
    QCOMPARE(m_dropdown->count(), 2);
    QCOMPARE(m_dropdown->itemText(0), "Item 1");
    QCOMPARE(m_dropdown->itemText(1), "Item 3");
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(itemRemovedSpy.count(), 1);
    
    // Test removing invalid index
    m_dropdown->removeItem(10);
    QCOMPARE(m_dropdown->count(), 2); // Should remain unchanged
}

void FluentDropdownTest::testClearItems() {
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    m_dropdown->addItem("Item 3");
    
    QSignalSpy countSpy(m_dropdown, &FluentDropdown::countChanged);
    QSignalSpy clearedSpy(m_dropdown, &FluentDropdown::itemsCleared);
    
    m_dropdown->clear();
    QCOMPARE(m_dropdown->count(), 0);
    QCOMPARE(m_dropdown->currentIndex(), -1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(clearedSpy.count(), 1);
}

void FluentDropdownTest::testItemCount() {
    QCOMPARE(m_dropdown->count(), 0);
    
    m_dropdown->addItem("Item 1");
    QCOMPARE(m_dropdown->count(), 1);
    
    m_dropdown->addItem("Item 2");
    QCOMPARE(m_dropdown->count(), 2);
    
    m_dropdown->removeItem(0);
    QCOMPARE(m_dropdown->count(), 1);
}

void FluentDropdownTest::testCurrentIndex() {
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    m_dropdown->addItem("Item 3");
    
    QSignalSpy currentIndexSpy(m_dropdown, &FluentDropdown::currentIndexChanged);
    QSignalSpy currentTextSpy(m_dropdown, &FluentDropdown::currentTextChanged);
    
    m_dropdown->setCurrentIndex(1);
    QCOMPARE(m_dropdown->currentIndex(), 1);
    QCOMPARE(m_dropdown->currentText(), "Item 2");
    QCOMPARE(currentIndexSpy.count(), 1);
    QCOMPARE(currentTextSpy.count(), 1);
    
    // Test invalid index
    m_dropdown->setCurrentIndex(10);
    QCOMPARE(m_dropdown->currentIndex(), 1); // Should remain unchanged
    
    m_dropdown->setCurrentIndex(-1);
    QCOMPARE(m_dropdown->currentIndex(), -1);
    QCOMPARE(m_dropdown->currentText(), QString());
    QCOMPARE(currentIndexSpy.count(), 2);
}

void FluentDropdownTest::testCurrentText() {
    m_dropdown->addItem("Apple");
    m_dropdown->addItem("Banana");
    m_dropdown->addItem("Cherry");
    
    QSignalSpy currentTextSpy(m_dropdown, &FluentDropdown::currentTextChanged);
    
    m_dropdown->setCurrentText("Banana");
    QCOMPARE(m_dropdown->currentText(), "Banana");
    QCOMPARE(m_dropdown->currentIndex(), 1);
    QCOMPARE(currentTextSpy.count(), 1);
    
    // Test non-existent text
    m_dropdown->setCurrentText("Orange");
    QCOMPARE(m_dropdown->currentText(), "Banana"); // Should remain unchanged
}

void FluentDropdownTest::testCurrentData() {
    QVariant data1 = QVariant::fromValue(QString("data1"));
    QVariant data2 = QVariant::fromValue(QString("data2"));
    QVariant data3 = QVariant::fromValue(QString("data3"));
    
    m_dropdown->addItem("Item 1", data1);
    m_dropdown->addItem("Item 2", data2);
    m_dropdown->addItem("Item 3", data3);
    
    m_dropdown->setCurrentIndex(1);
    QCOMPARE(m_dropdown->currentData(), data2);
    
    m_dropdown->setCurrentIndex(0);
    QCOMPARE(m_dropdown->currentData(), data1);
    
    m_dropdown->setCurrentIndex(-1);
    QVERIFY(!m_dropdown->currentData().isValid());
}

void FluentDropdownTest::testSetCurrentIndex() {
    m_dropdown->addItem("First");
    m_dropdown->addItem("Second");
    m_dropdown->addItem("Third");
    
    QSignalSpy activatedSpy(m_dropdown, &FluentDropdown::activated);
    QSignalSpy highlightedSpy(m_dropdown, &FluentDropdown::highlighted);
    
    m_dropdown->setCurrentIndex(2);
    QCOMPARE(m_dropdown->currentIndex(), 2);
    QCOMPARE(m_dropdown->currentText(), "Third");
    
    // Test bounds
    m_dropdown->setCurrentIndex(-5);
    QCOMPARE(m_dropdown->currentIndex(), -1);
    
    m_dropdown->setCurrentIndex(100);
    QCOMPARE(m_dropdown->currentIndex(), -1); // Should be invalid
}

void FluentDropdownTest::testPlaceholderText() {
    QSignalSpy placeholderSpy(m_dropdown, &FluentDropdown::placeholderTextChanged);
    
    m_dropdown->setPlaceholderText("Select an option...");
    QCOMPARE(m_dropdown->placeholderText(), "Select an option...");
    QCOMPARE(placeholderSpy.count(), 1);
    
    // Test empty placeholder
    m_dropdown->setPlaceholderText("");
    QCOMPARE(m_dropdown->placeholderText(), "");
    QCOMPARE(placeholderSpy.count(), 2);
}

void FluentDropdownTest::testEditable() {
    QSignalSpy editableSpy(m_dropdown, &FluentDropdown::editableChanged);
    
    m_dropdown->setEditable(true);
    QVERIFY(m_dropdown->isEditable());
    QCOMPARE(editableSpy.count(), 1);
    
    m_dropdown->setEditable(false);
    QVERIFY(!m_dropdown->isEditable());
    QCOMPARE(editableSpy.count(), 2);
}

void FluentDropdownTest::testMaxVisibleItems() {
    QSignalSpy maxVisibleSpy(m_dropdown, &FluentDropdown::maxVisibleItemsChanged);
    
    m_dropdown->setMaxVisibleItems(5);
    QCOMPARE(m_dropdown->maxVisibleItems(), 5);
    QCOMPARE(maxVisibleSpy.count(), 1);
    
    // Test invalid value
    m_dropdown->setMaxVisibleItems(0);
    QVERIFY(m_dropdown->maxVisibleItems() > 0); // Should remain positive
}

void FluentDropdownTest::testShowPopup() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    
    QSignalSpy popupShownSpy(m_dropdown, &FluentDropdown::popupShown);
    
    m_dropdown->showPopup();
    QVERIFY(m_dropdown->popupVisible());
    QCOMPARE(popupShownSpy.count(), 1);
}

void FluentDropdownTest::testHidePopup() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    
    m_dropdown->showPopup();
    
    QSignalSpy popupHiddenSpy(m_dropdown, &FluentDropdown::popupHidden);
    
    m_dropdown->hidePopup();
    QVERIFY(!m_dropdown->popupVisible());
    QCOMPARE(popupHiddenSpy.count(), 1);
}

void FluentDropdownTest::testPopupVisible() {
    m_dropdown->addItem("Item 1");
    
    QVERIFY(!m_dropdown->popupVisible()); // Default should be hidden
    
    m_dropdown->showPopup();
    QVERIFY(m_dropdown->popupVisible());
    
    m_dropdown->hidePopup();
    QVERIFY(!m_dropdown->popupVisible());
}

void FluentDropdownTest::testSearchEnabled() {
    QSignalSpy searchSpy(m_dropdown, &FluentDropdown::searchEnabledChanged);
    
    m_dropdown->setSearchEnabled(true);
    QVERIFY(m_dropdown->searchEnabled());
    QCOMPARE(searchSpy.count(), 1);
    
    m_dropdown->setSearchEnabled(false);
    QVERIFY(!m_dropdown->searchEnabled());
    QCOMPARE(searchSpy.count(), 2);
}

void FluentDropdownTest::testSearchText() {
    m_dropdown->addItem("Apple");
    m_dropdown->addItem("Apricot");
    m_dropdown->addItem("Banana");
    m_dropdown->addItem("Cherry");
    
    m_dropdown->setSearchEnabled(true);
    
    QSignalSpy searchTextSpy(m_dropdown, &FluentDropdown::searchTextChanged);
    
    m_dropdown->setSearchText("Ap");
    QCOMPARE(m_dropdown->searchText(), "Ap");
    QCOMPARE(searchTextSpy.count(), 1);
    
    // Test search filtering (implementation dependent)
    // In a real implementation, this might filter visible items
}

void FluentDropdownTest::testSignalEmission() {
    QSignalSpy countSpy(m_dropdown, &FluentDropdown::countChanged);
    QSignalSpy itemAddedSpy(m_dropdown, &FluentDropdown::itemAdded);
    QSignalSpy itemRemovedSpy(m_dropdown, &FluentDropdown::itemRemoved);
    QSignalSpy itemsClearedSpy(m_dropdown, &FluentDropdown::itemsCleared);
    QSignalSpy currentIndexSpy(m_dropdown, &FluentDropdown::currentIndexChanged);
    QSignalSpy currentTextSpy(m_dropdown, &FluentDropdown::currentTextChanged);
    QSignalSpy activatedSpy(m_dropdown, &FluentDropdown::activated);
    QSignalSpy highlightedSpy(m_dropdown, &FluentDropdown::highlighted);
    QSignalSpy placeholderSpy(m_dropdown, &FluentDropdown::placeholderTextChanged);
    QSignalSpy editableSpy(m_dropdown, &FluentDropdown::editableChanged);
    
    // Test all signals
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    m_dropdown->setCurrentIndex(1);
    m_dropdown->removeItem(0);
    m_dropdown->setPlaceholderText("Select...");
    m_dropdown->setEditable(true);
    m_dropdown->clear();
    
    QCOMPARE(countSpy.count(), 4); // add, add, remove, clear
    QCOMPARE(itemAddedSpy.count(), 2);
    QCOMPARE(itemRemovedSpy.count(), 1);
    QCOMPARE(itemsClearedSpy.count(), 1);
    QCOMPARE(currentIndexSpy.count(), 2); // setCurrentIndex, clear
    QCOMPARE(currentTextSpy.count(), 2); // setCurrentIndex, clear
    QCOMPARE(placeholderSpy.count(), 1);
    QCOMPARE(editableSpy.count(), 1);
}

void FluentDropdownTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    m_dropdown->addItem("Item 3");
    
    QSignalSpy activatedSpy(m_dropdown, &FluentDropdown::activated);
    QSignalSpy popupShownSpy(m_dropdown, &FluentDropdown::popupShown);
    
    // Test mouse click to show popup
    QTest::mouseClick(m_dropdown, Qt::LeftButton);
    
    // Mouse interaction testing is limited without knowing exact dropdown layout
    // In a real implementation, this would test clicking on dropdown button
    QTest::qWait(100);
}

void FluentDropdownTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_dropdown->setFocus();
    
    m_dropdown->addItem("Item 1");
    m_dropdown->addItem("Item 2");
    m_dropdown->addItem("Item 3");
    
    QSignalSpy currentIndexSpy(m_dropdown, &FluentDropdown::currentIndexChanged);
    QSignalSpy activatedSpy(m_dropdown, &FluentDropdown::activated);
    
    // Test arrow key navigation
    QTest::keyClick(m_dropdown, Qt::Key_Down);
    QCOMPARE(currentIndexSpy.count(), 1);
    QCOMPARE(m_dropdown->currentIndex(), 0);
    
    QTest::keyClick(m_dropdown, Qt::Key_Down);
    QCOMPARE(currentIndexSpy.count(), 2);
    QCOMPARE(m_dropdown->currentIndex(), 1);
    
    QTest::keyClick(m_dropdown, Qt::Key_Up);
    QCOMPARE(currentIndexSpy.count(), 3);
    QCOMPARE(m_dropdown->currentIndex(), 0);
    
    // Test Enter key
    QTest::keyClick(m_dropdown, Qt::Key_Return);
    
    // Test Space key to open popup
    QTest::keyClick(m_dropdown, Qt::Key_Space);
    
    // Test Escape key to close popup
    QTest::keyClick(m_dropdown, Qt::Key_Escape);
    
    // Test Home/End keys
    QTest::keyClick(m_dropdown, Qt::Key_End);
    QCOMPARE(m_dropdown->currentIndex(), 2);
    
    QTest::keyClick(m_dropdown, Qt::Key_Home);
    QCOMPARE(m_dropdown->currentIndex(), 0);
}

void FluentDropdownTest::testItemValidation() {
    // Test item text validation
    m_dropdown->addItem("Valid Item");
    QCOMPARE(m_dropdown->itemText(0), "Valid Item");
    
    // Test empty item text
    m_dropdown->addItem("");
    QCOMPARE(m_dropdown->itemText(1), "");
    
    // Test item data validation
    QVariant validData = QVariant::fromValue(42);
    m_dropdown->addItem("Item with data", validData);
    QCOMPARE(m_dropdown->itemData(2), validData);
    
    // Test invalid indices
    QString invalidText = m_dropdown->itemText(100);
    QCOMPARE(invalidText, QString());
    
    QVariant invalidData = m_dropdown->itemData(-1);
    QVERIFY(!invalidData.isValid());
    
    // Test item modification
    m_dropdown->setItemText(0, "Modified Item");
    QCOMPARE(m_dropdown->itemText(0), "Modified Item");
    
    QVariant newData = QVariant::fromValue(QString("new_data"));
    m_dropdown->setItemData(0, newData);
    QCOMPARE(m_dropdown->itemData(0), newData);
    
    // Test item icon
    QIcon itemIcon(":/test/item.png");
    m_dropdown->setItemIcon(0, itemIcon);
    QCOMPARE(m_dropdown->itemIcon(0).name(), itemIcon.name());
}

QTEST_MAIN(FluentDropdownTest)
#include "FluentDropdownTest.moc"
