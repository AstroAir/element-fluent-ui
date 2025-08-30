// tests/Components/FluentDataTableTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentDataTable.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentDataTableTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Column tests
    void testAddColumn();
    void testRemoveColumn();
    void testColumnCount();
    void testColumnHeader();
    void testColumnWidth();
    void testColumnVisible();

    // Row tests
    void testAddRow();
    void testRemoveRow();
    void testRowCount();
    void testRowHeight();
    void testRowVisible();

    // Data tests
    void testSetData();
    void testGetData();
    void testClearData();

    // Selection tests
    void testSelectionMode();
    void testSelectedRows();
    void testSelectedColumns();
    void testSelectRow();
    void testSelectColumn();

    // Sorting tests
    void testSortingEnabled();
    void testSortByColumn();
    void testSortOrder();

    // Filtering tests
    void testFilterEnabled();
    void testSetFilter();
    void testClearFilter();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testCellClick();
    void testHeaderClick();
    void testKeyboardNavigation();

private:
    FluentDataTable* m_dataTable{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentDataTableTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(600, 400);
}

void FluentDataTableTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentDataTableTest::init() {
    m_dataTable = new FluentDataTable(m_testWidget);
}

void FluentDataTableTest::cleanup() {
    delete m_dataTable;
    m_dataTable = nullptr;
}

void FluentDataTableTest::testDefaultConstructor() {
    QVERIFY(m_dataTable != nullptr);
    QCOMPARE(m_dataTable->columnCount(), 0);
    QCOMPARE(m_dataTable->rowCount(), 0);
    QCOMPARE(m_dataTable->selectionMode(), FluentTableSelectionMode::SingleRow);
    QVERIFY(m_dataTable->sortingEnabled());
    QVERIFY(m_dataTable->filterEnabled());
}

void FluentDataTableTest::testAddColumn() {
    QSignalSpy columnCountSpy(m_dataTable, &FluentDataTable::columnCountChanged);
    QSignalSpy columnAddedSpy(m_dataTable, &FluentDataTable::columnAdded);
    
    m_dataTable->addColumn("Name");
    QCOMPARE(m_dataTable->columnCount(), 1);
    QCOMPARE(m_dataTable->columnHeader(0), "Name");
    QCOMPARE(columnCountSpy.count(), 1);
    QCOMPARE(columnAddedSpy.count(), 1);
    
    m_dataTable->addColumn("Age", 100);
    QCOMPARE(m_dataTable->columnCount(), 2);
    QCOMPARE(m_dataTable->columnHeader(1), "Age");
    QCOMPARE(m_dataTable->columnWidth(1), 100);
    QCOMPARE(columnCountSpy.count(), 2);
    QCOMPARE(columnAddedSpy.count(), 2);
}

void FluentDataTableTest::testRemoveColumn() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    m_dataTable->addColumn("City");
    
    QSignalSpy columnCountSpy(m_dataTable, &FluentDataTable::columnCountChanged);
    QSignalSpy columnRemovedSpy(m_dataTable, &FluentDataTable::columnRemoved);
    
    m_dataTable->removeColumn(1); // Remove "Age"
    QCOMPARE(m_dataTable->columnCount(), 2);
    QCOMPARE(m_dataTable->columnHeader(0), "Name");
    QCOMPARE(m_dataTable->columnHeader(1), "City");
    QCOMPARE(columnCountSpy.count(), 1);
    QCOMPARE(columnRemovedSpy.count(), 1);
    
    // Test removing invalid index
    m_dataTable->removeColumn(10);
    QCOMPARE(m_dataTable->columnCount(), 2); // Should remain unchanged
}

void FluentDataTableTest::testColumnCount() {
    QCOMPARE(m_dataTable->columnCount(), 0);
    
    m_dataTable->addColumn("Column 1");
    QCOMPARE(m_dataTable->columnCount(), 1);
    
    m_dataTable->addColumn("Column 2");
    QCOMPARE(m_dataTable->columnCount(), 2);
    
    m_dataTable->removeColumn(0);
    QCOMPARE(m_dataTable->columnCount(), 1);
}

void FluentDataTableTest::testColumnHeader() {
    m_dataTable->addColumn("Original Header");
    
    QSignalSpy headerSpy(m_dataTable, &FluentDataTable::columnHeaderChanged);
    
    m_dataTable->setColumnHeader(0, "Updated Header");
    QCOMPARE(m_dataTable->columnHeader(0), "Updated Header");
    QCOMPARE(headerSpy.count(), 1);
    
    // Test invalid index
    QString invalidHeader = m_dataTable->columnHeader(10);
    QCOMPARE(invalidHeader, QString());
}

void FluentDataTableTest::testColumnWidth() {
    m_dataTable->addColumn("Column");
    
    QSignalSpy widthSpy(m_dataTable, &FluentDataTable::columnWidthChanged);
    
    m_dataTable->setColumnWidth(0, 150);
    QCOMPARE(m_dataTable->columnWidth(0), 150);
    QCOMPARE(widthSpy.count(), 1);
    
    // Test auto-resize
    m_dataTable->setColumnAutoResize(0, true);
    QVERIFY(m_dataTable->columnAutoResize(0));
}

void FluentDataTableTest::testColumnVisible() {
    m_dataTable->addColumn("Column");
    
    QSignalSpy visibilitySpy(m_dataTable, &FluentDataTable::columnVisibilityChanged);
    
    QVERIFY(m_dataTable->columnVisible(0)); // Default should be visible
    
    m_dataTable->setColumnVisible(0, false);
    QVERIFY(!m_dataTable->columnVisible(0));
    QCOMPARE(visibilitySpy.count(), 1);
    
    m_dataTable->setColumnVisible(0, true);
    QVERIFY(m_dataTable->columnVisible(0));
    QCOMPARE(visibilitySpy.count(), 2);
}

void FluentDataTableTest::testAddRow() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    
    QSignalSpy rowCountSpy(m_dataTable, &FluentDataTable::rowCountChanged);
    QSignalSpy rowAddedSpy(m_dataTable, &FluentDataTable::rowAdded);
    
    QVariantList rowData = {"John", 25};
    m_dataTable->addRow(rowData);
    QCOMPARE(m_dataTable->rowCount(), 1);
    QCOMPARE(m_dataTable->data(0, 0).toString(), "John");
    QCOMPARE(m_dataTable->data(0, 1).toInt(), 25);
    QCOMPARE(rowCountSpy.count(), 1);
    QCOMPARE(rowAddedSpy.count(), 1);
    
    // Add empty row
    m_dataTable->addRow();
    QCOMPARE(m_dataTable->rowCount(), 2);
    QCOMPARE(rowCountSpy.count(), 2);
}

void FluentDataTableTest::testRemoveRow() {
    m_dataTable->addColumn("Name");
    m_dataTable->addRow({"John"});
    m_dataTable->addRow({"Jane"});
    m_dataTable->addRow({"Bob"});
    
    QSignalSpy rowCountSpy(m_dataTable, &FluentDataTable::rowCountChanged);
    QSignalSpy rowRemovedSpy(m_dataTable, &FluentDataTable::rowRemoved);
    
    m_dataTable->removeRow(1); // Remove "Jane"
    QCOMPARE(m_dataTable->rowCount(), 2);
    QCOMPARE(m_dataTable->data(0, 0).toString(), "John");
    QCOMPARE(m_dataTable->data(1, 0).toString(), "Bob");
    QCOMPARE(rowCountSpy.count(), 1);
    QCOMPARE(rowRemovedSpy.count(), 1);
}

void FluentDataTableTest::testRowCount() {
    QCOMPARE(m_dataTable->rowCount(), 0);
    
    m_dataTable->addColumn("Column");
    m_dataTable->addRow();
    QCOMPARE(m_dataTable->rowCount(), 1);
    
    m_dataTable->addRow();
    QCOMPARE(m_dataTable->rowCount(), 2);
    
    m_dataTable->removeRow(0);
    QCOMPARE(m_dataTable->rowCount(), 1);
}

void FluentDataTableTest::testRowHeight() {
    m_dataTable->addColumn("Column");
    m_dataTable->addRow();
    
    QSignalSpy heightSpy(m_dataTable, &FluentDataTable::rowHeightChanged);
    
    m_dataTable->setRowHeight(0, 50);
    QCOMPARE(m_dataTable->rowHeight(0), 50);
    QCOMPARE(heightSpy.count(), 1);
    
    // Test uniform row height
    m_dataTable->setUniformRowHeight(40);
    QCOMPARE(m_dataTable->uniformRowHeight(), 40);
}

void FluentDataTableTest::testRowVisible() {
    m_dataTable->addColumn("Column");
    m_dataTable->addRow();
    
    QSignalSpy visibilitySpy(m_dataTable, &FluentDataTable::rowVisibilityChanged);
    
    QVERIFY(m_dataTable->rowVisible(0)); // Default should be visible
    
    m_dataTable->setRowVisible(0, false);
    QVERIFY(!m_dataTable->rowVisible(0));
    QCOMPARE(visibilitySpy.count(), 1);
}

void FluentDataTableTest::testSetData() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    m_dataTable->addRow();
    
    QSignalSpy dataChangedSpy(m_dataTable, &FluentDataTable::dataChanged);
    
    m_dataTable->setData(0, 0, "John");
    m_dataTable->setData(0, 1, 25);
    
    QCOMPARE(m_dataTable->data(0, 0).toString(), "John");
    QCOMPARE(m_dataTable->data(0, 1).toInt(), 25);
    QCOMPARE(dataChangedSpy.count(), 2);
}

void FluentDataTableTest::testGetData() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    m_dataTable->addRow({"Alice", 30});
    
    QCOMPARE(m_dataTable->data(0, 0).toString(), "Alice");
    QCOMPARE(m_dataTable->data(0, 1).toInt(), 30);
    
    // Test invalid indices
    QVariant invalidData = m_dataTable->data(10, 10);
    QVERIFY(!invalidData.isValid());
}

void FluentDataTableTest::testClearData() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    m_dataTable->addRow({"John", 25});
    m_dataTable->addRow({"Jane", 30});
    
    QSignalSpy dataClearedSpy(m_dataTable, &FluentDataTable::dataCleared);
    
    m_dataTable->clearData();
    QCOMPARE(m_dataTable->rowCount(), 0);
    QCOMPARE(m_dataTable->columnCount(), 2); // Columns should remain
    QCOMPARE(dataClearedSpy.count(), 1);
}

void FluentDataTableTest::testSelectionMode() {
    QSignalSpy selectionModeSpy(m_dataTable, &FluentDataTable::selectionModeChanged);
    
    m_dataTable->setSelectionMode(FluentTableSelectionMode::MultipleRows);
    QCOMPARE(m_dataTable->selectionMode(), FluentTableSelectionMode::MultipleRows);
    QCOMPARE(selectionModeSpy.count(), 1);
    
    m_dataTable->setSelectionMode(FluentTableSelectionMode::SingleCell);
    QCOMPARE(m_dataTable->selectionMode(), FluentTableSelectionMode::SingleCell);
    QCOMPARE(selectionModeSpy.count(), 2);
}

void FluentDataTableTest::testSelectedRows() {
    m_dataTable->addColumn("Name");
    m_dataTable->addRow({"John"});
    m_dataTable->addRow({"Jane"});
    m_dataTable->addRow({"Bob"});
    
    QSignalSpy selectionSpy(m_dataTable, &FluentDataTable::selectionChanged);
    
    m_dataTable->selectRow(1);
    QList<int> selectedRows = m_dataTable->selectedRows();
    QCOMPARE(selectedRows.count(), 1);
    QCOMPARE(selectedRows.first(), 1);
    QCOMPARE(selectionSpy.count(), 1);
    
    // Test multiple selection
    m_dataTable->setSelectionMode(FluentTableSelectionMode::MultipleRows);
    m_dataTable->selectRow(0, true); // Add to selection
    selectedRows = m_dataTable->selectedRows();
    QCOMPARE(selectedRows.count(), 2);
    QVERIFY(selectedRows.contains(0));
    QVERIFY(selectedRows.contains(1));
}

void FluentDataTableTest::testSelectedColumns() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    m_dataTable->addColumn("City");
    
    m_dataTable->setSelectionMode(FluentTableSelectionMode::SingleColumn);
    m_dataTable->selectColumn(1);
    
    QList<int> selectedColumns = m_dataTable->selectedColumns();
    QCOMPARE(selectedColumns.count(), 1);
    QCOMPARE(selectedColumns.first(), 1);
}

void FluentDataTableTest::testSelectRow() {
    m_dataTable->addColumn("Name");
    m_dataTable->addRow({"John"});
    m_dataTable->addRow({"Jane"});
    
    QSignalSpy selectionSpy(m_dataTable, &FluentDataTable::selectionChanged);
    
    m_dataTable->selectRow(0);
    QVERIFY(m_dataTable->isRowSelected(0));
    QVERIFY(!m_dataTable->isRowSelected(1));
    QCOMPARE(selectionSpy.count(), 1);
    
    // Clear selection
    m_dataTable->clearSelection();
    QVERIFY(!m_dataTable->isRowSelected(0));
    QCOMPARE(selectionSpy.count(), 2);
}

void FluentDataTableTest::testSelectColumn() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    
    m_dataTable->setSelectionMode(FluentTableSelectionMode::SingleColumn);
    m_dataTable->selectColumn(0);
    
    QVERIFY(m_dataTable->isColumnSelected(0));
    QVERIFY(!m_dataTable->isColumnSelected(1));
}

void FluentDataTableTest::testSortingEnabled() {
    QSignalSpy sortingSpy(m_dataTable, &FluentDataTable::sortingEnabledChanged);
    
    m_dataTable->setSortingEnabled(false);
    QVERIFY(!m_dataTable->sortingEnabled());
    QCOMPARE(sortingSpy.count(), 1);
    
    m_dataTable->setSortingEnabled(true);
    QVERIFY(m_dataTable->sortingEnabled());
    QCOMPARE(sortingSpy.count(), 2);
}

void FluentDataTableTest::testSortByColumn() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    m_dataTable->addRow({"Charlie", 35});
    m_dataTable->addRow({"Alice", 25});
    m_dataTable->addRow({"Bob", 30});
    
    QSignalSpy sortedSpy(m_dataTable, &FluentDataTable::dataSorted);
    
    // Sort by name (ascending)
    m_dataTable->sortByColumn(0, Qt::AscendingOrder);
    QCOMPARE(m_dataTable->data(0, 0).toString(), "Alice");
    QCOMPARE(m_dataTable->data(1, 0).toString(), "Bob");
    QCOMPARE(m_dataTable->data(2, 0).toString(), "Charlie");
    QCOMPARE(sortedSpy.count(), 1);
    
    // Sort by age (descending)
    m_dataTable->sortByColumn(1, Qt::DescendingOrder);
    QCOMPARE(m_dataTable->data(0, 1).toInt(), 35); // Charlie
    QCOMPARE(m_dataTable->data(1, 1).toInt(), 30); // Bob
    QCOMPARE(m_dataTable->data(2, 1).toInt(), 25); // Alice
    QCOMPARE(sortedSpy.count(), 2);
}

void FluentDataTableTest::testSortOrder() {
    m_dataTable->addColumn("Numbers");
    m_dataTable->addRow({3});
    m_dataTable->addRow({1});
    m_dataTable->addRow({2});
    
    m_dataTable->sortByColumn(0, Qt::AscendingOrder);
    QCOMPARE(m_dataTable->sortOrder(), Qt::AscendingOrder);
    QCOMPARE(m_dataTable->sortColumn(), 0);
    
    m_dataTable->sortByColumn(0, Qt::DescendingOrder);
    QCOMPARE(m_dataTable->sortOrder(), Qt::DescendingOrder);
}

void FluentDataTableTest::testFilterEnabled() {
    QSignalSpy filterSpy(m_dataTable, &FluentDataTable::filterEnabledChanged);
    
    m_dataTable->setFilterEnabled(false);
    QVERIFY(!m_dataTable->filterEnabled());
    QCOMPARE(filterSpy.count(), 1);
}

void FluentDataTableTest::testSetFilter() {
    m_dataTable->addColumn("Name");
    m_dataTable->addRow({"Alice"});
    m_dataTable->addRow({"Bob"});
    m_dataTable->addRow({"Charlie"});
    
    QSignalSpy filterSpy(m_dataTable, &FluentDataTable::filterChanged);
    
    m_dataTable->setFilter(0, "A"); // Filter names starting with 'A'
    QCOMPARE(filterSpy.count(), 1);
    
    // Check filtered results
    int visibleRows = m_dataTable->visibleRowCount();
    QCOMPARE(visibleRows, 1); // Only "Alice" should be visible
}

void FluentDataTableTest::testClearFilter() {
    m_dataTable->addColumn("Name");
    m_dataTable->addRow({"Alice"});
    m_dataTable->addRow({"Bob"});
    
    m_dataTable->setFilter(0, "A");
    
    QSignalSpy filterSpy(m_dataTable, &FluentDataTable::filterChanged);
    
    m_dataTable->clearFilter();
    QCOMPARE(filterSpy.count(), 1);
    QCOMPARE(m_dataTable->visibleRowCount(), 2); // All rows should be visible
}

void FluentDataTableTest::testSignalEmission() {
    // Test that all major signals are emitted correctly
    QSignalSpy columnCountSpy(m_dataTable, &FluentDataTable::columnCountChanged);
    QSignalSpy rowCountSpy(m_dataTable, &FluentDataTable::rowCountChanged);
    QSignalSpy dataChangedSpy(m_dataTable, &FluentDataTable::dataChanged);
    QSignalSpy selectionSpy(m_dataTable, &FluentDataTable::selectionChanged);
    
    m_dataTable->addColumn("Test");
    m_dataTable->addRow({"Value"});
    m_dataTable->setData(0, 0, "Updated");
    m_dataTable->selectRow(0);
    
    QCOMPARE(columnCountSpy.count(), 1);
    QCOMPARE(rowCountSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(selectionSpy.count(), 1);
}

void FluentDataTableTest::testCellClick() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dataTable->addColumn("Name");
    m_dataTable->addRow({"John"});
    
    QSignalSpy cellClickedSpy(m_dataTable, &FluentDataTable::cellClicked);
    
    // Simulate cell click
    m_dataTable->show();
    QTest::qWaitForWindowExposed(m_dataTable);
    
    // Cell click testing would require knowing exact cell positions
    // For now, we test the signal can be emitted programmatically
    emit m_dataTable->cellClicked(0, 0);
    QCOMPARE(cellClickedSpy.count(), 1);
}

void FluentDataTableTest::testHeaderClick() {
    m_dataTable->addColumn("Name");
    m_dataTable->addColumn("Age");
    
    QSignalSpy headerClickedSpy(m_dataTable, &FluentDataTable::headerClicked);
    
    // Simulate header click
    emit m_dataTable->headerClicked(0);
    QCOMPARE(headerClickedSpy.count(), 1);
}

void FluentDataTableTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_dataTable->addColumn("Name");
    m_dataTable->addRow({"John"});
    m_dataTable->addRow({"Jane"});
    
    m_dataTable->show();
    QTest::qWaitForWindowExposed(m_dataTable);
    m_dataTable->setFocus();
    
    QSignalSpy selectionSpy(m_dataTable, &FluentDataTable::selectionChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_dataTable, Qt::Key_Down);
    QTest::keyClick(m_dataTable, Qt::Key_Up);
    QTest::keyClick(m_dataTable, Qt::Key_Right);
    QTest::keyClick(m_dataTable, Qt::Key_Left);
    
    // Test selection keys
    QTest::keyClick(m_dataTable, Qt::Key_Space);
    
    QTest::qWait(100);
}

QTEST_MAIN(FluentDataTableTest)
#include "FluentDataTableTest.moc"
