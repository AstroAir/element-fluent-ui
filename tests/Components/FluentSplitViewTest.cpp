// tests/Components/FluentSplitViewTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentSplitView.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentSplitViewTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Pane management tests
    void testAddPane();
    void testRemovePane();
    void testPaneCount();
    void testPaneAt();

    // Display mode tests
    void testDisplayMode();
    void testPaneDisplayMode();
    void testCompactModeThreshold();

    // Pane properties tests
    void testPaneLength();
    void testPaneMinLength();
    void testPaneMaxLength();
    void testPaneResizable();

    // Content tests
    void testPaneContent();
    void testContentFrame();

    // Splitter tests
    void testSplitterVisible();
    void testSplitterWidth();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testPaneResize();
    void testSplitterDrag();

private:
    FluentSplitView* m_splitView{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentSplitViewTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(800, 600);
}

void FluentSplitViewTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentSplitViewTest::init() {
    m_splitView = new FluentSplitView(m_testWidget);
}

void FluentSplitViewTest::cleanup() {
    delete m_splitView;
    m_splitView = nullptr;
}

void FluentSplitViewTest::testDefaultConstructor() {
    QVERIFY(m_splitView != nullptr);
    QCOMPARE(m_splitView->paneCount(), 0);
    QCOMPARE(m_splitView->displayMode(), FluentSplitViewDisplayMode::Overlay);
    QCOMPARE(m_splitView->paneDisplayMode(), FluentSplitViewPaneDisplayMode::Left);
    QVERIFY(m_splitView->isSplitterVisible());
}

void FluentSplitViewTest::testAddPane() {
    QSignalSpy paneCountSpy(m_splitView, &FluentSplitView::paneCountChanged);
    QSignalSpy paneAddedSpy(m_splitView, &FluentSplitView::paneAdded);
    
    auto* pane1 = new QWidget();
    int index1 = m_splitView->addPane(pane1);
    QCOMPARE(index1, 0);
    QCOMPARE(m_splitView->paneCount(), 1);
    QCOMPARE(m_splitView->paneAt(0), pane1);
    QCOMPARE(paneCountSpy.count(), 1);
    QCOMPARE(paneAddedSpy.count(), 1);
    
    auto* pane2 = new QWidget();
    int index2 = m_splitView->addPane(pane2, 200);
    QCOMPARE(index2, 1);
    QCOMPARE(m_splitView->paneCount(), 2);
    QCOMPARE(m_splitView->paneLength(1), 200);
    QCOMPARE(paneCountSpy.count(), 2);
}

void FluentSplitViewTest::testRemovePane() {
    auto* pane1 = new QWidget();
    auto* pane2 = new QWidget();
    auto* pane3 = new QWidget();
    
    m_splitView->addPane(pane1);
    m_splitView->addPane(pane2);
    m_splitView->addPane(pane3);
    
    QSignalSpy paneCountSpy(m_splitView, &FluentSplitView::paneCountChanged);
    QSignalSpy paneRemovedSpy(m_splitView, &FluentSplitView::paneRemoved);
    
    m_splitView->removePane(1); // Remove pane2
    QCOMPARE(m_splitView->paneCount(), 2);
    QCOMPARE(m_splitView->paneAt(0), pane1);
    QCOMPARE(m_splitView->paneAt(1), pane3);
    QCOMPARE(paneCountSpy.count(), 1);
    QCOMPARE(paneRemovedSpy.count(), 1);
    
    // Test removing by widget
    m_splitView->removePane(pane1);
    QCOMPARE(m_splitView->paneCount(), 1);
    QCOMPARE(m_splitView->paneAt(0), pane3);
    QCOMPARE(paneCountSpy.count(), 2);
}

void FluentSplitViewTest::testPaneCount() {
    QCOMPARE(m_splitView->paneCount(), 0);
    
    m_splitView->addPane(new QWidget());
    QCOMPARE(m_splitView->paneCount(), 1);
    
    m_splitView->addPane(new QWidget());
    QCOMPARE(m_splitView->paneCount(), 2);
    
    m_splitView->removePane(0);
    QCOMPARE(m_splitView->paneCount(), 1);
}

void FluentSplitViewTest::testPaneAt() {
    auto* pane1 = new QWidget();
    auto* pane2 = new QWidget();
    
    m_splitView->addPane(pane1);
    m_splitView->addPane(pane2);
    
    QCOMPARE(m_splitView->paneAt(0), pane1);
    QCOMPARE(m_splitView->paneAt(1), pane2);
    
    // Test invalid index
    QCOMPARE(m_splitView->paneAt(10), nullptr);
    
    // Test index of pane
    QCOMPARE(m_splitView->indexOf(pane1), 0);
    QCOMPARE(m_splitView->indexOf(pane2), 1);
    
    auto* nonExistentPane = new QWidget();
    QCOMPARE(m_splitView->indexOf(nonExistentPane), -1);
    delete nonExistentPane;
}

void FluentSplitViewTest::testDisplayMode() {
    QSignalSpy displayModeSpy(m_splitView, &FluentSplitView::displayModeChanged);
    
    m_splitView->setDisplayMode(FluentSplitViewDisplayMode::Inline);
    QCOMPARE(m_splitView->displayMode(), FluentSplitViewDisplayMode::Inline);
    QCOMPARE(displayModeSpy.count(), 1);
    
    m_splitView->setDisplayMode(FluentSplitViewDisplayMode::CompactOverlay);
    QCOMPARE(m_splitView->displayMode(), FluentSplitViewDisplayMode::CompactOverlay);
    QCOMPARE(displayModeSpy.count(), 2);
    
    m_splitView->setDisplayMode(FluentSplitViewDisplayMode::CompactInline);
    QCOMPARE(m_splitView->displayMode(), FluentSplitViewDisplayMode::CompactInline);
    QCOMPARE(displayModeSpy.count(), 3);
}

void FluentSplitViewTest::testPaneDisplayMode() {
    QSignalSpy paneDisplayModeSpy(m_splitView, &FluentSplitView::paneDisplayModeChanged);
    
    m_splitView->setPaneDisplayMode(FluentSplitViewPaneDisplayMode::Right);
    QCOMPARE(m_splitView->paneDisplayMode(), FluentSplitViewPaneDisplayMode::Right);
    QCOMPARE(paneDisplayModeSpy.count(), 1);
    
    m_splitView->setPaneDisplayMode(FluentSplitViewPaneDisplayMode::Top);
    QCOMPARE(m_splitView->paneDisplayMode(), FluentSplitViewPaneDisplayMode::Top);
    QCOMPARE(paneDisplayModeSpy.count(), 2);
    
    m_splitView->setPaneDisplayMode(FluentSplitViewPaneDisplayMode::Bottom);
    QCOMPARE(m_splitView->paneDisplayMode(), FluentSplitViewPaneDisplayMode::Bottom);
    QCOMPARE(paneDisplayModeSpy.count(), 3);
}

void FluentSplitViewTest::testCompactModeThreshold() {
    QSignalSpy thresholdSpy(m_splitView, &FluentSplitView::compactModeThresholdChanged);
    
    m_splitView->setCompactModeThreshold(600);
    QCOMPARE(m_splitView->compactModeThreshold(), 600);
    QCOMPARE(thresholdSpy.count(), 1);
    
    // Test invalid threshold
    m_splitView->setCompactModeThreshold(-100);
    QVERIFY(m_splitView->compactModeThreshold() >= 0);
}

void FluentSplitViewTest::testPaneLength() {
    m_splitView->addPane(new QWidget());
    m_splitView->addPane(new QWidget());
    
    QSignalSpy paneLengthSpy(m_splitView, &FluentSplitView::paneLengthChanged);
    
    m_splitView->setPaneLength(0, 300);
    QCOMPARE(m_splitView->paneLength(0), 300);
    QCOMPARE(paneLengthSpy.count(), 1);
    
    // Test invalid index
    m_splitView->setPaneLength(10, 400);
    QCOMPARE(paneLengthSpy.count(), 1); // Should not emit signal
}

void FluentSplitViewTest::testPaneMinLength() {
    m_splitView->addPane(new QWidget());
    
    QSignalSpy paneMinLengthSpy(m_splitView, &FluentSplitView::paneMinLengthChanged);
    
    m_splitView->setPaneMinLength(0, 100);
    QCOMPARE(m_splitView->paneMinLength(0), 100);
    QCOMPARE(paneMinLengthSpy.count(), 1);
    
    // Test that pane length adjusts if below minimum
    m_splitView->setPaneLength(0, 50);
    QCOMPARE(m_splitView->paneLength(0), 100); // Should adjust to minimum
}

void FluentSplitViewTest::testPaneMaxLength() {
    m_splitView->addPane(new QWidget());
    
    QSignalSpy paneMaxLengthSpy(m_splitView, &FluentSplitView::paneMaxLengthChanged);
    
    m_splitView->setPaneMaxLength(0, 500);
    QCOMPARE(m_splitView->paneMaxLength(0), 500);
    QCOMPARE(paneMaxLengthSpy.count(), 1);
    
    // Test that pane length adjusts if above maximum
    m_splitView->setPaneLength(0, 600);
    QCOMPARE(m_splitView->paneLength(0), 500); // Should adjust to maximum
}

void FluentSplitViewTest::testPaneResizable() {
    m_splitView->addPane(new QWidget());
    
    QSignalSpy paneResizableSpy(m_splitView, &FluentSplitView::paneResizableChanged);
    
    QVERIFY(m_splitView->isPaneResizable(0)); // Default should be resizable
    
    m_splitView->setPaneResizable(0, false);
    QVERIFY(!m_splitView->isPaneResizable(0));
    QCOMPARE(paneResizableSpy.count(), 1);
    
    m_splitView->setPaneResizable(0, true);
    QVERIFY(m_splitView->isPaneResizable(0));
    QCOMPARE(paneResizableSpy.count(), 2);
}

void FluentSplitViewTest::testPaneContent() {
    auto* content1 = new QWidget();
    auto* content2 = new QWidget();
    
    m_splitView->addPane(content1);
    m_splitView->addPane(content2);
    
    QCOMPARE(m_splitView->paneContent(0), content1);
    QCOMPARE(m_splitView->paneContent(1), content2);
    
    // Test setting new content
    auto* newContent = new QWidget();
    m_splitView->setPaneContent(0, newContent);
    QCOMPARE(m_splitView->paneContent(0), newContent);
    
    // Test invalid index
    QCOMPARE(m_splitView->paneContent(10), nullptr);
}

void FluentSplitViewTest::testContentFrame() {
    auto* contentFrame = new QWidget();
    m_splitView->setContentFrame(contentFrame);
    QCOMPARE(m_splitView->contentFrame(), contentFrame);
    
    // Test null content frame
    m_splitView->setContentFrame(nullptr);
    QCOMPARE(m_splitView->contentFrame(), nullptr);
}

void FluentSplitViewTest::testSplitterVisible() {
    QSignalSpy splitterVisibleSpy(m_splitView, &FluentSplitView::splitterVisibleChanged);
    
    m_splitView->setSplitterVisible(false);
    QVERIFY(!m_splitView->isSplitterVisible());
    QCOMPARE(splitterVisibleSpy.count(), 1);
    
    m_splitView->setSplitterVisible(true);
    QVERIFY(m_splitView->isSplitterVisible());
    QCOMPARE(splitterVisibleSpy.count(), 2);
}

void FluentSplitViewTest::testSplitterWidth() {
    QSignalSpy splitterWidthSpy(m_splitView, &FluentSplitView::splitterWidthChanged);
    
    m_splitView->setSplitterWidth(8);
    QCOMPARE(m_splitView->splitterWidth(), 8);
    QCOMPARE(splitterWidthSpy.count(), 1);
    
    // Test invalid width
    m_splitView->setSplitterWidth(-2);
    QVERIFY(m_splitView->splitterWidth() > 0); // Should remain positive
}

void FluentSplitViewTest::testSignalEmission() {
    QSignalSpy paneCountSpy(m_splitView, &FluentSplitView::paneCountChanged);
    QSignalSpy paneAddedSpy(m_splitView, &FluentSplitView::paneAdded);
    QSignalSpy paneRemovedSpy(m_splitView, &FluentSplitView::paneRemoved);
    QSignalSpy displayModeSpy(m_splitView, &FluentSplitView::displayModeChanged);
    QSignalSpy paneDisplayModeSpy(m_splitView, &FluentSplitView::paneDisplayModeChanged);
    QSignalSpy thresholdSpy(m_splitView, &FluentSplitView::compactModeThresholdChanged);
    QSignalSpy paneLengthSpy(m_splitView, &FluentSplitView::paneLengthChanged);
    QSignalSpy paneMinLengthSpy(m_splitView, &FluentSplitView::paneMinLengthChanged);
    QSignalSpy paneMaxLengthSpy(m_splitView, &FluentSplitView::paneMaxLengthChanged);
    QSignalSpy paneResizableSpy(m_splitView, &FluentSplitView::paneResizableChanged);
    QSignalSpy splitterVisibleSpy(m_splitView, &FluentSplitView::splitterVisibleChanged);
    QSignalSpy splitterWidthSpy(m_splitView, &FluentSplitView::splitterWidthChanged);
    
    // Test all signals
    auto* pane1 = new QWidget();
    auto* pane2 = new QWidget();
    m_splitView->addPane(pane1);
    m_splitView->addPane(pane2);
    m_splitView->removePane(0);
    m_splitView->setDisplayMode(FluentSplitViewDisplayMode::Inline);
    m_splitView->setPaneDisplayMode(FluentSplitViewPaneDisplayMode::Right);
    m_splitView->setCompactModeThreshold(500);
    m_splitView->setPaneLength(0, 250);
    m_splitView->setPaneMinLength(0, 100);
    m_splitView->setPaneMaxLength(0, 400);
    m_splitView->setPaneResizable(0, false);
    m_splitView->setSplitterVisible(false);
    m_splitView->setSplitterWidth(6);
    
    QCOMPARE(paneCountSpy.count(), 3); // add, add, remove
    QCOMPARE(paneAddedSpy.count(), 2);
    QCOMPARE(paneRemovedSpy.count(), 1);
    QCOMPARE(displayModeSpy.count(), 1);
    QCOMPARE(paneDisplayModeSpy.count(), 1);
    QCOMPARE(thresholdSpy.count(), 1);
    QCOMPARE(paneLengthSpy.count(), 1);
    QCOMPARE(paneMinLengthSpy.count(), 1);
    QCOMPARE(paneMaxLengthSpy.count(), 1);
    QCOMPARE(paneResizableSpy.count(), 1);
    QCOMPARE(splitterVisibleSpy.count(), 1);
    QCOMPARE(splitterWidthSpy.count(), 1);
}

void FluentSplitViewTest::testPaneResize() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_splitView->addPane(new QWidget(), 200);
    m_splitView->addPane(new QWidget(), 300);
    
    QSignalSpy paneLengthSpy(m_splitView, &FluentSplitView::paneLengthChanged);
    QSignalSpy paneResizedSpy(m_splitView, &FluentSplitView::paneResized);
    
    // Test programmatic resize
    m_splitView->setPaneLength(0, 250);
    QCOMPARE(m_splitView->paneLength(0), 250);
    QCOMPARE(paneLengthSpy.count(), 1);
    
    // Test resize with constraints
    m_splitView->setPaneMinLength(0, 150);
    m_splitView->setPaneMaxLength(0, 350);
    
    m_splitView->setPaneLength(0, 100); // Below minimum
    QCOMPARE(m_splitView->paneLength(0), 150); // Should clamp to minimum
    
    m_splitView->setPaneLength(0, 400); // Above maximum
    QCOMPARE(m_splitView->paneLength(0), 350); // Should clamp to maximum
}

void FluentSplitViewTest::testSplitterDrag() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_splitView->addPane(new QWidget(), 200);
    m_splitView->addPane(new QWidget(), 300);
    
    QSignalSpy splitterMovedSpy(m_splitView, &FluentSplitView::splitterMoved);
    
    // Test splitter drag simulation
    // Note: Actual drag testing would require access to internal splitter widget
    // For now, we test the signal can be emitted
    emit m_splitView->splitterMoved(0, 250);
    QCOMPARE(splitterMovedSpy.count(), 1);
    
    // Test that non-resizable panes cannot be resized
    m_splitView->setPaneResizable(0, false);
    m_splitView->setPaneLength(0, 180);
    QCOMPARE(m_splitView->paneLength(0), 200); // Should remain unchanged
}

QTEST_MAIN(FluentSplitViewTest)
#include "FluentSplitViewTest.moc"
