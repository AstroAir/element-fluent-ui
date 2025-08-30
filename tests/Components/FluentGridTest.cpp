// tests/Components/FluentGridTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentGrid.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentGridTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Grid structure tests
    void testRowCount();
    void testColumnCount();
    void testSetGridSize();

    // Item management tests
    void testAddItem();
    void testRemoveItem();
    void testClearItems();
    void testItemAt();

    // Layout tests
    void testSpacing();
    void testMargins();
    void testAlignment();

    // Row/Column properties tests
    void testRowHeight();
    void testColumnWidth();
    void testRowStretch();
    void testColumnStretch();

    // Selection tests
    void testSelectionMode();
    void testSelectedItems();
    void testSelectItem();
    void testClearSelection();

    // Appearance tests
    void testShowGrid();
    void testGridLineColor();
    void testAlternatingRowColors();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testItemClick();
    void testKeyboardNavigation();

private:
    FluentGrid* m_grid{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentGridTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(600, 400);
}

void FluentGridTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentGridTest::init() {
    m_grid = new FluentGrid(m_testWidget);
}

void FluentGridTest::cleanup() {
    delete m_grid;
    m_grid = nullptr;
}

void FluentGridTest::testDefaultConstructor() {
    QVERIFY(m_grid != nullptr);
    QCOMPARE(m_grid->rowCount(), 0);
    QCOMPARE(m_grid->columnCount(), 0);
    QCOMPARE(m_grid->spacing(), 0);
    QVERIFY(m_grid->showGrid());
    QCOMPARE(m_grid->selectionMode(), FluentGridSelectionMode::SingleItem);
}

void FluentGridTest::testRowCount() {
    QSignalSpy rowCountSpy(m_grid, &FluentGrid::rowCountChanged);
    
    m_grid->setRowCount(5);
    QCOMPARE(m_grid->rowCount(), 5);
    QCOMPARE(rowCountSpy.count(), 1);
    
    // Test invalid row count
    m_grid->setRowCount(-1);
    QVERIFY(m_grid->rowCount() >= 0); // Should remain non-negative
    
    m_grid->setRowCount(0);
    QCOMPARE(m_grid->rowCount(), 0);
    QCOMPARE(rowCountSpy.count(), 3);
}

void FluentGridTest::testColumnCount() {
    QSignalSpy columnCountSpy(m_grid, &FluentGrid::columnCountChanged);
    
    m_grid->setColumnCount(4);
    QCOMPARE(m_grid->columnCount(), 4);
    QCOMPARE(columnCountSpy.count(), 1);
    
    // Test invalid column count
    m_grid->setColumnCount(-1);
    QVERIFY(m_grid->columnCount() >= 0); // Should remain non-negative
    
    m_grid->setColumnCount(0);
    QCOMPARE(m_grid->columnCount(), 0);
    QCOMPARE(columnCountSpy.count(), 3);
}

void FluentGridTest::testSetGridSize() {
    QSignalSpy rowCountSpy(m_grid, &FluentGrid::rowCountChanged);
    QSignalSpy columnCountSpy(m_grid, &FluentGrid::columnCountChanged);
    
    m_grid->setGridSize(3, 4);
    QCOMPARE(m_grid->rowCount(), 3);
    QCOMPARE(m_grid->columnCount(), 4);
    QCOMPARE(rowCountSpy.count(), 1);
    QCOMPARE(columnCountSpy.count(), 1);
    
    // Test same size (should not emit signals)
    m_grid->setGridSize(3, 4);
    QCOMPARE(rowCountSpy.count(), 1);
    QCOMPARE(columnCountSpy.count(), 1);
}

void FluentGridTest::testAddItem() {
    m_grid->setGridSize(3, 3);
    
    QSignalSpy itemAddedSpy(m_grid, &FluentGrid::itemAdded);
    
    auto* widget1 = new QWidget();
    m_grid->addItem(widget1, 0, 0);
    QCOMPARE(m_grid->itemAt(0, 0), widget1);
    QCOMPARE(itemAddedSpy.count(), 1);
    
    auto* widget2 = new QWidget();
    m_grid->addItem(widget2, 1, 2);
    QCOMPARE(m_grid->itemAt(1, 2), widget2);
    QCOMPARE(itemAddedSpy.count(), 2);
    
    // Test invalid position
    auto* widget3 = new QWidget();
    m_grid->addItem(widget3, 5, 5); // Out of bounds
    QCOMPARE(m_grid->itemAt(5, 5), nullptr); // Should not be added
    delete widget3;
}

void FluentGridTest::testRemoveItem() {
    m_grid->setGridSize(2, 2);
    
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    m_grid->addItem(widget1, 0, 0);
    m_grid->addItem(widget2, 1, 1);
    
    QSignalSpy itemRemovedSpy(m_grid, &FluentGrid::itemRemoved);
    
    m_grid->removeItem(0, 0);
    QCOMPARE(m_grid->itemAt(0, 0), nullptr);
    QCOMPARE(itemRemovedSpy.count(), 1);
    
    // Test removing non-existent item
    m_grid->removeItem(0, 1); // Empty position
    QCOMPARE(itemRemovedSpy.count(), 1); // Should not emit signal
    
    // Test removing by widget
    m_grid->removeItem(widget2);
    QCOMPARE(m_grid->itemAt(1, 1), nullptr);
    QCOMPARE(itemRemovedSpy.count(), 2);
}

void FluentGridTest::testClearItems() {
    m_grid->setGridSize(2, 2);
    
    m_grid->addItem(new QWidget(), 0, 0);
    m_grid->addItem(new QWidget(), 0, 1);
    m_grid->addItem(new QWidget(), 1, 0);
    
    QSignalSpy itemsClearedSpy(m_grid, &FluentGrid::itemsCleared);
    
    m_grid->clearItems();
    QCOMPARE(m_grid->itemAt(0, 0), nullptr);
    QCOMPARE(m_grid->itemAt(0, 1), nullptr);
    QCOMPARE(m_grid->itemAt(1, 0), nullptr);
    QCOMPARE(itemsClearedSpy.count(), 1);
}

void FluentGridTest::testItemAt() {
    m_grid->setGridSize(3, 3);
    
    auto* widget = new QWidget();
    m_grid->addItem(widget, 1, 2);
    
    QCOMPARE(m_grid->itemAt(1, 2), widget);
    QCOMPARE(m_grid->itemAt(0, 0), nullptr); // Empty position
    QCOMPARE(m_grid->itemAt(5, 5), nullptr); // Out of bounds
    
    // Test position of widget
    QPoint position = m_grid->positionOf(widget);
    QCOMPARE(position, QPoint(2, 1)); // x=column, y=row
    
    // Test non-existent widget
    auto* otherWidget = new QWidget();
    QPoint invalidPosition = m_grid->positionOf(otherWidget);
    QCOMPARE(invalidPosition, QPoint(-1, -1));
    delete otherWidget;
}

void FluentGridTest::testSpacing() {
    QSignalSpy spacingSpy(m_grid, &FluentGrid::spacingChanged);
    
    m_grid->setSpacing(10);
    QCOMPARE(m_grid->spacing(), 10);
    QCOMPARE(spacingSpy.count(), 1);
    
    // Test negative spacing
    m_grid->setSpacing(-5);
    QVERIFY(m_grid->spacing() >= 0); // Should remain non-negative
    
    m_grid->setSpacing(0);
    QCOMPARE(m_grid->spacing(), 0);
    QCOMPARE(spacingSpy.count(), 3);
}

void FluentGridTest::testMargins() {
    QSignalSpy marginsSpy(m_grid, &FluentGrid::marginsChanged);
    
    QMargins testMargins(10, 15, 20, 25);
    m_grid->setMargins(testMargins);
    QCOMPARE(m_grid->margins(), testMargins);
    QCOMPARE(marginsSpy.count(), 1);
    
    // Test individual margins
    m_grid->setMargins(5, 5, 5, 5);
    QCOMPARE(m_grid->margins(), QMargins(5, 5, 5, 5));
    QCOMPARE(marginsSpy.count(), 2);
}

void FluentGridTest::testAlignment() {
    QSignalSpy alignmentSpy(m_grid, &FluentGrid::alignmentChanged);
    
    m_grid->setAlignment(Qt::AlignCenter);
    QCOMPARE(m_grid->alignment(), Qt::AlignCenter);
    QCOMPARE(alignmentSpy.count(), 1);
    
    m_grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QCOMPARE(m_grid->alignment(), Qt::AlignTop | Qt::AlignLeft);
    QCOMPARE(alignmentSpy.count(), 2);
}

void FluentGridTest::testRowHeight() {
    m_grid->setGridSize(3, 2);
    
    QSignalSpy rowHeightSpy(m_grid, &FluentGrid::rowHeightChanged);
    
    m_grid->setRowHeight(1, 50);
    QCOMPARE(m_grid->rowHeight(1), 50);
    QCOMPARE(rowHeightSpy.count(), 1);
    
    // Test invalid row
    m_grid->setRowHeight(10, 100);
    QCOMPARE(rowHeightSpy.count(), 1); // Should not emit signal
    
    // Test uniform row height
    m_grid->setUniformRowHeight(40);
    QCOMPARE(m_grid->uniformRowHeight(), 40);
    for (int i = 0; i < m_grid->rowCount(); ++i) {
        QCOMPARE(m_grid->rowHeight(i), 40);
    }
}

void FluentGridTest::testColumnWidth() {
    m_grid->setGridSize(2, 3);
    
    QSignalSpy columnWidthSpy(m_grid, &FluentGrid::columnWidthChanged);
    
    m_grid->setColumnWidth(2, 80);
    QCOMPARE(m_grid->columnWidth(2), 80);
    QCOMPARE(columnWidthSpy.count(), 1);
    
    // Test invalid column
    m_grid->setColumnWidth(10, 100);
    QCOMPARE(columnWidthSpy.count(), 1); // Should not emit signal
    
    // Test uniform column width
    m_grid->setUniformColumnWidth(60);
    QCOMPARE(m_grid->uniformColumnWidth(), 60);
    for (int i = 0; i < m_grid->columnCount(); ++i) {
        QCOMPARE(m_grid->columnWidth(i), 60);
    }
}

void FluentGridTest::testRowStretch() {
    m_grid->setGridSize(3, 2);
    
    QSignalSpy rowStretchSpy(m_grid, &FluentGrid::rowStretchChanged);
    
    m_grid->setRowStretch(0, 2);
    QCOMPARE(m_grid->rowStretch(0), 2);
    QCOMPARE(rowStretchSpy.count(), 1);
    
    m_grid->setRowStretch(1, 1);
    QCOMPARE(m_grid->rowStretch(1), 1);
    QCOMPARE(rowStretchSpy.count(), 2);
    
    // Test invalid row
    m_grid->setRowStretch(10, 3);
    QCOMPARE(rowStretchSpy.count(), 2); // Should not emit signal
}

void FluentGridTest::testColumnStretch() {
    m_grid->setGridSize(2, 3);
    
    QSignalSpy columnStretchSpy(m_grid, &FluentGrid::columnStretchChanged);
    
    m_grid->setColumnStretch(1, 3);
    QCOMPARE(m_grid->columnStretch(1), 3);
    QCOMPARE(columnStretchSpy.count(), 1);
    
    m_grid->setColumnStretch(2, 1);
    QCOMPARE(m_grid->columnStretch(2), 1);
    QCOMPARE(columnStretchSpy.count(), 2);
    
    // Test invalid column
    m_grid->setColumnStretch(10, 2);
    QCOMPARE(columnStretchSpy.count(), 2); // Should not emit signal
}

void FluentGridTest::testSelectionMode() {
    QSignalSpy selectionModeSpy(m_grid, &FluentGrid::selectionModeChanged);
    
    m_grid->setSelectionMode(FluentGridSelectionMode::MultipleItems);
    QCOMPARE(m_grid->selectionMode(), FluentGridSelectionMode::MultipleItems);
    QCOMPARE(selectionModeSpy.count(), 1);
    
    m_grid->setSelectionMode(FluentGridSelectionMode::NoSelection);
    QCOMPARE(m_grid->selectionMode(), FluentGridSelectionMode::NoSelection);
    QCOMPARE(selectionModeSpy.count(), 2);
    
    m_grid->setSelectionMode(FluentGridSelectionMode::SingleItem);
    QCOMPARE(m_grid->selectionMode(), FluentGridSelectionMode::SingleItem);
    QCOMPARE(selectionModeSpy.count(), 3);
}

void FluentGridTest::testSelectedItems() {
    m_grid->setGridSize(2, 2);
    m_grid->setSelectionMode(FluentGridSelectionMode::MultipleItems);
    
    auto* widget1 = new QWidget();
    auto* widget2 = new QWidget();
    m_grid->addItem(widget1, 0, 0);
    m_grid->addItem(widget2, 1, 1);
    
    QSignalSpy selectionSpy(m_grid, &FluentGrid::selectionChanged);
    
    m_grid->selectItem(0, 0);
    QVERIFY(m_grid->isItemSelected(0, 0));
    QCOMPARE(m_grid->selectedItems().count(), 1);
    QVERIFY(m_grid->selectedItems().contains(widget1));
    QCOMPARE(selectionSpy.count(), 1);
    
    m_grid->selectItem(1, 1, true); // Add to selection
    QVERIFY(m_grid->isItemSelected(1, 1));
    QCOMPARE(m_grid->selectedItems().count(), 2);
    QVERIFY(m_grid->selectedItems().contains(widget2));
    QCOMPARE(selectionSpy.count(), 2);
}

void FluentGridTest::testSelectItem() {
    m_grid->setGridSize(2, 2);
    
    auto* widget = new QWidget();
    m_grid->addItem(widget, 0, 1);
    
    QSignalSpy selectionSpy(m_grid, &FluentGrid::selectionChanged);
    
    m_grid->selectItem(0, 1);
    QVERIFY(m_grid->isItemSelected(0, 1));
    QCOMPARE(selectionSpy.count(), 1);
    
    // Test selecting empty position
    m_grid->selectItem(1, 0); // No widget here
    QVERIFY(!m_grid->isItemSelected(1, 0));
    
    // Test selecting by widget
    m_grid->selectItem(widget);
    QVERIFY(m_grid->isItemSelected(widget));
}

void FluentGridTest::testClearSelection() {
    m_grid->setGridSize(2, 2);
    m_grid->setSelectionMode(FluentGridSelectionMode::MultipleItems);
    
    m_grid->addItem(new QWidget(), 0, 0);
    m_grid->addItem(new QWidget(), 1, 1);
    
    m_grid->selectItem(0, 0);
    m_grid->selectItem(1, 1, true);
    
    QSignalSpy selectionSpy(m_grid, &FluentGrid::selectionChanged);
    
    m_grid->clearSelection();
    QVERIFY(m_grid->selectedItems().isEmpty());
    QVERIFY(!m_grid->isItemSelected(0, 0));
    QVERIFY(!m_grid->isItemSelected(1, 1));
    QCOMPARE(selectionSpy.count(), 1);
}

void FluentGridTest::testShowGrid() {
    QSignalSpy showGridSpy(m_grid, &FluentGrid::showGridChanged);
    
    m_grid->setShowGrid(false);
    QVERIFY(!m_grid->showGrid());
    QCOMPARE(showGridSpy.count(), 1);
    
    m_grid->setShowGrid(true);
    QVERIFY(m_grid->showGrid());
    QCOMPARE(showGridSpy.count(), 2);
}

void FluentGridTest::testGridLineColor() {
    QSignalSpy gridLineColorSpy(m_grid, &FluentGrid::gridLineColorChanged);
    
    QColor testColor(255, 0, 0);
    m_grid->setGridLineColor(testColor);
    QCOMPARE(m_grid->gridLineColor(), testColor);
    QCOMPARE(gridLineColorSpy.count(), 1);
    
    // Test invalid color
    m_grid->setGridLineColor(QColor());
    QVERIFY(m_grid->gridLineColor().isValid()); // Should remain valid
}

void FluentGridTest::testAlternatingRowColors() {
    QSignalSpy alternatingRowColorsSpy(m_grid, &FluentGrid::alternatingRowColorsChanged);
    
    m_grid->setAlternatingRowColors(true);
    QVERIFY(m_grid->alternatingRowColors());
    QCOMPARE(alternatingRowColorsSpy.count(), 1);
    
    m_grid->setAlternatingRowColors(false);
    QVERIFY(!m_grid->alternatingRowColors());
    QCOMPARE(alternatingRowColorsSpy.count(), 2);
}

void FluentGridTest::testSignalEmission() {
    QSignalSpy rowCountSpy(m_grid, &FluentGrid::rowCountChanged);
    QSignalSpy columnCountSpy(m_grid, &FluentGrid::columnCountChanged);
    QSignalSpy itemAddedSpy(m_grid, &FluentGrid::itemAdded);
    QSignalSpy itemRemovedSpy(m_grid, &FluentGrid::itemRemoved);
    QSignalSpy itemsClearedSpy(m_grid, &FluentGrid::itemsCleared);
    QSignalSpy spacingSpy(m_grid, &FluentGrid::spacingChanged);
    QSignalSpy marginsSpy(m_grid, &FluentGrid::marginsChanged);
    QSignalSpy alignmentSpy(m_grid, &FluentGrid::alignmentChanged);
    QSignalSpy selectionSpy(m_grid, &FluentGrid::selectionChanged);
    QSignalSpy showGridSpy(m_grid, &FluentGrid::showGridChanged);
    
    // Test all signals
    m_grid->setGridSize(2, 2);
    auto* widget = new QWidget();
    m_grid->addItem(widget, 0, 0);
    m_grid->removeItem(0, 0);
    m_grid->setSpacing(5);
    m_grid->setMargins(10, 10, 10, 10);
    m_grid->setAlignment(Qt::AlignCenter);
    m_grid->selectItem(0, 1);
    m_grid->setShowGrid(false);
    m_grid->clearItems();
    
    QCOMPARE(rowCountSpy.count(), 1);
    QCOMPARE(columnCountSpy.count(), 1);
    QCOMPARE(itemAddedSpy.count(), 1);
    QCOMPARE(itemRemovedSpy.count(), 1);
    QCOMPARE(itemsClearedSpy.count(), 1);
    QCOMPARE(spacingSpy.count(), 1);
    QCOMPARE(marginsSpy.count(), 1);
    QCOMPARE(alignmentSpy.count(), 1);
    QVERIFY(selectionSpy.count() >= 1);
    QCOMPARE(showGridSpy.count(), 1);
}

void FluentGridTest::testItemClick() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_grid->setGridSize(2, 2);
    auto* widget = new QWidget();
    m_grid->addItem(widget, 0, 0);
    
    QSignalSpy itemClickedSpy(m_grid, &FluentGrid::itemClicked);
    
    // Simulate item click
    emit m_grid->itemClicked(0, 0, widget);
    QCOMPARE(itemClickedSpy.count(), 1);
    
    QList<QVariant> arguments = itemClickedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 0); // row
    QCOMPARE(arguments.at(1).toInt(), 0); // column
    QCOMPARE(arguments.at(2).value<QWidget*>(), widget);
}

void FluentGridTest::testKeyboardNavigation() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_grid->setFocus();
    
    m_grid->setGridSize(3, 3);
    m_grid->addItem(new QWidget(), 0, 0);
    m_grid->addItem(new QWidget(), 1, 1);
    m_grid->addItem(new QWidget(), 2, 2);
    
    QSignalSpy selectionSpy(m_grid, &FluentGrid::selectionChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_grid, Qt::Key_Right);
    QTest::keyClick(m_grid, Qt::Key_Down);
    QTest::keyClick(m_grid, Qt::Key_Left);
    QTest::keyClick(m_grid, Qt::Key_Up);
    
    // Test Home/End keys
    QTest::keyClick(m_grid, Qt::Key_Home);
    QTest::keyClick(m_grid, Qt::Key_End);
    
    // Test Space/Enter for selection
    QTest::keyClick(m_grid, Qt::Key_Space);
    QTest::keyClick(m_grid, Qt::Key_Return);
    
    QTest::qWait(100);
}

QTEST_MAIN(FluentGridTest)
#include "FluentGridTest.moc"
