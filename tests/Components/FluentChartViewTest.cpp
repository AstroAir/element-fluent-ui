// tests/Components/FluentChartViewTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentChartView.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentChartViewTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Chart type tests
    void testChartType();
    void testSetChartType();

    // Data tests
    void testAddDataSeries();
    void testRemoveDataSeries();
    void testClearData();
    void testDataSeriesCount();

    // Appearance tests
    void testTitle();
    void testShowLegend();
    void testShowGrid();
    void testAnimationEnabled();

    // Axis tests
    void testXAxisTitle();
    void testYAxisTitle();
    void testAxisRange();

    // Color tests
    void testColorScheme();
    void testCustomColors();

    // Interaction tests
    void testZoomEnabled();
    void testPanEnabled();
    void testTooltipEnabled();

    // Signal tests
    void testSignalEmission();

    // Rendering tests
    void testRenderChart();

private:
    FluentChartView* m_chartView{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentChartViewTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(600, 400);
}

void FluentChartViewTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentChartViewTest::init() {
    m_chartView = new FluentChartView(m_testWidget);
}

void FluentChartViewTest::cleanup() {
    delete m_chartView;
    m_chartView = nullptr;
}

void FluentChartViewTest::testDefaultConstructor() {
    QVERIFY(m_chartView != nullptr);
    QCOMPARE(m_chartView->chartType(), FluentChartType::Line);
    QCOMPARE(m_chartView->dataSeriesCount(), 0);
    QVERIFY(m_chartView->showLegend());
    QVERIFY(m_chartView->showGrid());
    QVERIFY(m_chartView->animationEnabled());
}

void FluentChartViewTest::testChartType() {
    QCOMPARE(m_chartView->chartType(), FluentChartType::Line);
}

void FluentChartViewTest::testSetChartType() {
    QSignalSpy chartTypeSpy(m_chartView, &FluentChartView::chartTypeChanged);
    
    m_chartView->setChartType(FluentChartType::Bar);
    QCOMPARE(m_chartView->chartType(), FluentChartType::Bar);
    QCOMPARE(chartTypeSpy.count(), 1);
    
    m_chartView->setChartType(FluentChartType::Pie);
    QCOMPARE(m_chartView->chartType(), FluentChartType::Pie);
    QCOMPARE(chartTypeSpy.count(), 2);
    
    m_chartView->setChartType(FluentChartType::Area);
    QCOMPARE(m_chartView->chartType(), FluentChartType::Area);
    QCOMPARE(chartTypeSpy.count(), 3);
}

void FluentChartViewTest::testAddDataSeries() {
    QSignalSpy dataSeriesCountSpy(m_chartView, &FluentChartView::dataSeriesCountChanged);
    QSignalSpy dataChangedSpy(m_chartView, &FluentChartView::dataChanged);
    
    QList<QPointF> data1 = {{0, 10}, {1, 20}, {2, 15}, {3, 25}};
    QList<QPointF> data2 = {{0, 5}, {1, 15}, {2, 10}, {3, 20}};
    
    m_chartView->addDataSeries("Series 1", data1);
    QCOMPARE(m_chartView->dataSeriesCount(), 1);
    QCOMPARE(dataSeriesCountSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    
    m_chartView->addDataSeries("Series 2", data2);
    QCOMPARE(m_chartView->dataSeriesCount(), 2);
    QCOMPARE(dataSeriesCountSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
}

void FluentChartViewTest::testRemoveDataSeries() {
    // Add some data first
    QList<QPointF> data1 = {{0, 10}, {1, 20}, {2, 15}};
    QList<QPointF> data2 = {{0, 5}, {1, 15}, {2, 10}};
    
    m_chartView->addDataSeries("Series 1", data1);
    m_chartView->addDataSeries("Series 2", data2);
    
    QSignalSpy dataSeriesCountSpy(m_chartView, &FluentChartView::dataSeriesCountChanged);
    QSignalSpy dataChangedSpy(m_chartView, &FluentChartView::dataChanged);
    
    m_chartView->removeDataSeries(0);
    QCOMPARE(m_chartView->dataSeriesCount(), 1);
    QCOMPARE(dataSeriesCountSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    
    // Test removing invalid index
    m_chartView->removeDataSeries(10);
    QCOMPARE(m_chartView->dataSeriesCount(), 1); // Should remain unchanged
}

void FluentChartViewTest::testClearData() {
    // Add some data first
    QList<QPointF> data = {{0, 10}, {1, 20}, {2, 15}};
    m_chartView->addDataSeries("Series 1", data);
    m_chartView->addDataSeries("Series 2", data);
    
    QSignalSpy dataSeriesCountSpy(m_chartView, &FluentChartView::dataSeriesCountChanged);
    QSignalSpy dataChangedSpy(m_chartView, &FluentChartView::dataChanged);
    
    m_chartView->clearData();
    QCOMPARE(m_chartView->dataSeriesCount(), 0);
    QCOMPARE(dataSeriesCountSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
}

void FluentChartViewTest::testDataSeriesCount() {
    QCOMPARE(m_chartView->dataSeriesCount(), 0);
    
    QList<QPointF> data = {{0, 10}, {1, 20}};
    m_chartView->addDataSeries("Series 1", data);
    QCOMPARE(m_chartView->dataSeriesCount(), 1);
    
    m_chartView->addDataSeries("Series 2", data);
    QCOMPARE(m_chartView->dataSeriesCount(), 2);
    
    m_chartView->removeDataSeries(0);
    QCOMPARE(m_chartView->dataSeriesCount(), 1);
}

void FluentChartViewTest::testTitle() {
    QSignalSpy titleSpy(m_chartView, &FluentChartView::titleChanged);
    
    m_chartView->setTitle("Test Chart");
    QCOMPARE(m_chartView->title(), "Test Chart");
    QCOMPARE(titleSpy.count(), 1);
    
    m_chartView->setTitle("Updated Chart Title");
    QCOMPARE(m_chartView->title(), "Updated Chart Title");
    QCOMPARE(titleSpy.count(), 2);
}

void FluentChartViewTest::testShowLegend() {
    QSignalSpy legendSpy(m_chartView, &FluentChartView::showLegendChanged);
    
    m_chartView->setShowLegend(false);
    QVERIFY(!m_chartView->showLegend());
    QCOMPARE(legendSpy.count(), 1);
    
    m_chartView->setShowLegend(true);
    QVERIFY(m_chartView->showLegend());
    QCOMPARE(legendSpy.count(), 2);
}

void FluentChartViewTest::testShowGrid() {
    QSignalSpy gridSpy(m_chartView, &FluentChartView::showGridChanged);
    
    m_chartView->setShowGrid(false);
    QVERIFY(!m_chartView->showGrid());
    QCOMPARE(gridSpy.count(), 1);
    
    m_chartView->setShowGrid(true);
    QVERIFY(m_chartView->showGrid());
    QCOMPARE(gridSpy.count(), 2);
}

void FluentChartViewTest::testAnimationEnabled() {
    QSignalSpy animationSpy(m_chartView, &FluentChartView::animationEnabledChanged);
    
    m_chartView->setAnimationEnabled(false);
    QVERIFY(!m_chartView->animationEnabled());
    QCOMPARE(animationSpy.count(), 1);
    
    m_chartView->setAnimationEnabled(true);
    QVERIFY(m_chartView->animationEnabled());
    QCOMPARE(animationSpy.count(), 2);
}

void FluentChartViewTest::testXAxisTitle() {
    QSignalSpy xAxisSpy(m_chartView, &FluentChartView::xAxisTitleChanged);
    
    m_chartView->setXAxisTitle("Time");
    QCOMPARE(m_chartView->xAxisTitle(), "Time");
    QCOMPARE(xAxisSpy.count(), 1);
    
    m_chartView->setXAxisTitle("X Values");
    QCOMPARE(m_chartView->xAxisTitle(), "X Values");
    QCOMPARE(xAxisSpy.count(), 2);
}

void FluentChartViewTest::testYAxisTitle() {
    QSignalSpy yAxisSpy(m_chartView, &FluentChartView::yAxisTitleChanged);
    
    m_chartView->setYAxisTitle("Value");
    QCOMPARE(m_chartView->yAxisTitle(), "Value");
    QCOMPARE(yAxisSpy.count(), 1);
    
    m_chartView->setYAxisTitle("Y Values");
    QCOMPARE(m_chartView->yAxisTitle(), "Y Values");
    QCOMPARE(yAxisSpy.count(), 2);
}

void FluentChartViewTest::testAxisRange() {
    QSignalSpy xRangeSpy(m_chartView, &FluentChartView::xAxisRangeChanged);
    QSignalSpy yRangeSpy(m_chartView, &FluentChartView::yAxisRangeChanged);
    
    m_chartView->setXAxisRange(0, 100);
    QCOMPARE(m_chartView->xAxisMin(), 0.0);
    QCOMPARE(m_chartView->xAxisMax(), 100.0);
    QCOMPARE(xRangeSpy.count(), 1);
    
    m_chartView->setYAxisRange(-50, 50);
    QCOMPARE(m_chartView->yAxisMin(), -50.0);
    QCOMPARE(m_chartView->yAxisMax(), 50.0);
    QCOMPARE(yRangeSpy.count(), 1);
}

void FluentChartViewTest::testColorScheme() {
    QSignalSpy colorSchemeSpy(m_chartView, &FluentChartView::colorSchemeChanged);
    
    m_chartView->setColorScheme(FluentChartColorScheme::Vibrant);
    QCOMPARE(m_chartView->colorScheme(), FluentChartColorScheme::Vibrant);
    QCOMPARE(colorSchemeSpy.count(), 1);
    
    m_chartView->setColorScheme(FluentChartColorScheme::Pastel);
    QCOMPARE(m_chartView->colorScheme(), FluentChartColorScheme::Pastel);
    QCOMPARE(colorSchemeSpy.count(), 2);
}

void FluentChartViewTest::testCustomColors() {
    QSignalSpy customColorsSpy(m_chartView, &FluentChartView::customColorsChanged);
    
    QList<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::yellow};
    m_chartView->setCustomColors(colors);
    QCOMPARE(m_chartView->customColors(), colors);
    QCOMPARE(customColorsSpy.count(), 1);
}

void FluentChartViewTest::testZoomEnabled() {
    QSignalSpy zoomSpy(m_chartView, &FluentChartView::zoomEnabledChanged);
    
    m_chartView->setZoomEnabled(true);
    QVERIFY(m_chartView->zoomEnabled());
    QCOMPARE(zoomSpy.count(), 1);
    
    m_chartView->setZoomEnabled(false);
    QVERIFY(!m_chartView->zoomEnabled());
    QCOMPARE(zoomSpy.count(), 2);
}

void FluentChartViewTest::testPanEnabled() {
    QSignalSpy panSpy(m_chartView, &FluentChartView::panEnabledChanged);
    
    m_chartView->setPanEnabled(true);
    QVERIFY(m_chartView->panEnabled());
    QCOMPARE(panSpy.count(), 1);
    
    m_chartView->setPanEnabled(false);
    QVERIFY(!m_chartView->panEnabled());
    QCOMPARE(panSpy.count(), 2);
}

void FluentChartViewTest::testTooltipEnabled() {
    QSignalSpy tooltipSpy(m_chartView, &FluentChartView::tooltipEnabledChanged);
    
    m_chartView->setTooltipEnabled(false);
    QVERIFY(!m_chartView->tooltipEnabled());
    QCOMPARE(tooltipSpy.count(), 1);
    
    m_chartView->setTooltipEnabled(true);
    QVERIFY(m_chartView->tooltipEnabled());
    QCOMPARE(tooltipSpy.count(), 2);
}

void FluentChartViewTest::testSignalEmission() {
    QSignalSpy chartTypeSpy(m_chartView, &FluentChartView::chartTypeChanged);
    QSignalSpy dataSeriesCountSpy(m_chartView, &FluentChartView::dataSeriesCountChanged);
    QSignalSpy dataChangedSpy(m_chartView, &FluentChartView::dataChanged);
    QSignalSpy titleSpy(m_chartView, &FluentChartView::titleChanged);
    QSignalSpy legendSpy(m_chartView, &FluentChartView::showLegendChanged);
    QSignalSpy gridSpy(m_chartView, &FluentChartView::showGridChanged);
    QSignalSpy animationSpy(m_chartView, &FluentChartView::animationEnabledChanged);
    QSignalSpy xAxisSpy(m_chartView, &FluentChartView::xAxisTitleChanged);
    QSignalSpy yAxisSpy(m_chartView, &FluentChartView::yAxisTitleChanged);
    QSignalSpy colorSchemeSpy(m_chartView, &FluentChartView::colorSchemeChanged);
    
    // Test all signals
    m_chartView->setChartType(FluentChartType::Bar);
    QList<QPointF> data = {{0, 10}, {1, 20}};
    m_chartView->addDataSeries("Test", data);
    m_chartView->setTitle("Test Chart");
    m_chartView->setShowLegend(false);
    m_chartView->setShowGrid(false);
    m_chartView->setAnimationEnabled(false);
    m_chartView->setXAxisTitle("X");
    m_chartView->setYAxisTitle("Y");
    m_chartView->setColorScheme(FluentChartColorScheme::Vibrant);
    
    QCOMPARE(chartTypeSpy.count(), 1);
    QCOMPARE(dataSeriesCountSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(legendSpy.count(), 1);
    QCOMPARE(gridSpy.count(), 1);
    QCOMPARE(animationSpy.count(), 1);
    QCOMPARE(xAxisSpy.count(), 1);
    QCOMPARE(yAxisSpy.count(), 1);
    QCOMPARE(colorSchemeSpy.count(), 1);
}

void FluentChartViewTest::testRenderChart() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    // Add some test data
    QList<QPointF> data = {{0, 10}, {1, 20}, {2, 15}, {3, 25}, {4, 18}};
    m_chartView->addDataSeries("Test Series", data);
    m_chartView->setTitle("Test Chart");
    
    // Test different chart types render without crashing
    m_chartView->setChartType(FluentChartType::Line);
    QTest::qWait(100);
    
    m_chartView->setChartType(FluentChartType::Bar);
    QTest::qWait(100);
    
    m_chartView->setChartType(FluentChartType::Area);
    QTest::qWait(100);
    
    // Test that the chart renders (basic smoke test)
    QVERIFY(m_chartView->isVisible());
}

QTEST_MAIN(FluentChartViewTest)
#include "FluentChartViewTest.moc"
