// src/Components/FluentChartView.cpp
#include "FluentQt/Components/FluentChartView.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QPdfWriter>
#include <QApplication>
#include <QPixmap>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QBarSeries>
#include <QAreaSeries>
#include <QPieSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QDateTimeAxis>
#include <QLegend>
#include <QGraphicsLayout>

using namespace FluentQt::Components;
using namespace FluentQt::Core;

FluentChartView::FluentChartView(QWidget* parent)
    : FluentComponent(parent)
    , m_layout(new QVBoxLayout(this))
    , m_chart(new QChart())
    , m_chartView(new QChartView(m_chart))
{
    setupChart();
    setupChartView();
    setupContextMenu();
    
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_chartView);
    
    updateChartTheme();
    updateLegend();
}

QString FluentChartView::title() const {
    return m_chart->title();
}

void FluentChartView::setTitle(const QString& title) {
    if (m_chart->title() != title) {
        m_chart->setTitle(title);
        emit titleChanged(title);
    }
}

FluentChartTheme FluentChartView::theme() const {
    return m_theme;
}

void FluentChartView::setTheme(FluentChartTheme theme) {
    if (m_theme != theme) {
        m_theme = theme;
        updateChartTheme();
        emit themeChanged(theme);
    }
}

FluentLegendPosition FluentChartView::legendPosition() const {
    return m_legendPosition;
}

void FluentChartView::setLegendPosition(FluentLegendPosition position) {
    if (m_legendPosition != position) {
        m_legendPosition = position;
        updateLegend();
    }
}

bool FluentChartView::animationEnabled() const {
    return m_animationEnabled;
}

void FluentChartView::setAnimationEnabled(bool enabled) {
    if (m_animationEnabled != enabled) {
        m_animationEnabled = enabled;
        m_chart->setAnimationOptions(enabled ? QChart::SeriesAnimations : QChart::NoAnimation);
    }
}

bool FluentChartView::zoomEnabled() const {
    return m_zoomEnabled;
}

void FluentChartView::setZoomEnabled(bool enabled) {
    m_zoomEnabled = enabled;
    m_chartView->setRubberBand(enabled ? QChartView::RectangleRubberBand : QChartView::NoRubberBand);
}

bool FluentChartView::panEnabled() const {
    return m_panEnabled;
}

void FluentChartView::setPanEnabled(bool enabled) {
    m_panEnabled = enabled;
}

bool FluentChartView::antialiasing() const {
    return m_antialiasing;
}

void FluentChartView::setAntialiasing(bool enabled) {
    if (m_antialiasing != enabled) {
        m_antialiasing = enabled;
        m_chartView->setRenderHint(QPainter::Antialiasing, enabled);
    }
}

int FluentChartView::addSeries(const FluentChartSeries& series) {
    int index = m_seriesData.size();
    m_seriesData.append(series);
    
    QAbstractSeries* qtSeries = createSeries(series);
    if (qtSeries) {
        m_series[index] = qtSeries;
        m_chart->addSeries(qtSeries);
        
        // Connect signals based on series type
        if (auto* lineSeries = qobject_cast<QLineSeries*>(qtSeries)) {
            connect(lineSeries, &QLineSeries::clicked, this, &FluentChartView::onSeriesClicked);
            connect(lineSeries, &QLineSeries::hovered, this, &FluentChartView::onSeriesHovered);
        }
        // Note: Other series types have different signal signatures
    }
    
    return index;
}

void FluentChartView::removeSeries(int index) {
    if (index >= 0 && index < m_seriesData.size()) {
        if (m_series.contains(index)) {
            m_chart->removeSeries(m_series[index]);
            delete m_series[index];
            m_series.remove(index);
        }
        m_seriesData.removeAt(index);
    }
}

void FluentChartView::updateSeries(int index, const FluentChartSeries& series) {
    if (index >= 0 && index < m_seriesData.size()) {
        m_seriesData[index] = series;
        if (m_series.contains(index)) {
            updateSeriesData(m_series[index], series.data);
        }
    }
}

void FluentChartView::clearSeries() {
    m_chart->removeAllSeries();
    qDeleteAll(m_series.values());
    m_series.clear();
    m_seriesData.clear();
}

int FluentChartView::seriesCount() const {
    return m_seriesData.size();
}

FluentChartSeries FluentChartView::getSeriesData(int index) const {
    if (index >= 0 && index < m_seriesData.size()) {
        return m_seriesData[index];
    }
    return FluentChartSeries{};
}

void FluentChartView::setSeriesVisible(int index, bool visible) {
    if (index >= 0 && index < m_seriesData.size()) {
        m_seriesData[index].visible = visible;
        if (m_series.contains(index)) {
            m_series[index]->setVisible(visible);
        }
    }
}

bool FluentChartView::isSeriesVisible(int index) const {
    if (index >= 0 && index < m_seriesData.size()) {
        return m_seriesData[index].visible;
    }
    return false;
}

void FluentChartView::setSeriesColor(int index, const QColor& color) {
    if (index >= 0 && index < m_seriesData.size()) {
        m_seriesData[index].color = color;
        if (m_series.contains(index)) {
            // Set color based on series type
            auto* series = m_series[index];
            if (auto* lineSeries = qobject_cast<QLineSeries*>(series)) {
                lineSeries->setColor(color);
            } else if (auto* scatterSeries = qobject_cast<QScatterSeries*>(series)) {
                scatterSeries->setColor(color);
            }
            // Add more series types as needed
        }
    }
}

QColor FluentChartView::seriesColor(int index) const {
    if (index >= 0 && index < m_seriesData.size()) {
        return m_seriesData[index].color;
    }
    return QColor();
}

void FluentChartView::setSeriesOpacity(int index, qreal opacity) {
    if (index >= 0 && index < m_seriesData.size()) {
        m_seriesData[index].opacity = opacity;
        if (m_series.contains(index)) {
            m_series[index]->setOpacity(opacity);
        }
    }
}

qreal FluentChartView::seriesOpacity(int index) const {
    if (index >= 0 && index < m_seriesData.size()) {
        return m_seriesData[index].opacity;
    }
    return 1.0;
}

void FluentChartView::setXAxisTitle(const QString& title) {
    if (m_xAxis) {
        m_xAxis->setTitleText(title);
    }
}

void FluentChartView::setYAxisTitle(const QString& title) {
    if (m_yAxis) {
        m_yAxis->setTitleText(title);
    }
}

void FluentChartView::setXAxisRange(qreal min, qreal max) {
    if (auto* valueAxis = qobject_cast<QValueAxis*>(m_xAxis)) {
        valueAxis->setRange(min, max);
    }
}

void FluentChartView::setYAxisRange(qreal min, qreal max) {
    if (auto* valueAxis = qobject_cast<QValueAxis*>(m_yAxis)) {
        valueAxis->setRange(min, max);
    }
}

void FluentChartView::setXAxisType(const QString& type) {
    // Implementation for axis type switching would go here
    Q_UNUSED(type)
}

void FluentChartView::setYAxisType(const QString& type) {
    // Implementation for axis type switching would go here
    Q_UNUSED(type)
}

void FluentChartView::setXAxisVisible(bool visible) {
    if (m_xAxis) {
        m_xAxis->setVisible(visible);
    }
}

void FluentChartView::setYAxisVisible(bool visible) {
    if (m_yAxis) {
        m_yAxis->setVisible(visible);
    }
}

void FluentChartView::setGridVisible(bool visible) {
    if (m_xAxis) {
        m_xAxis->setGridLineVisible(visible);
    }
    if (m_yAxis) {
        m_yAxis->setGridLineVisible(visible);
    }
}

void FluentChartView::setGridColor(const QColor& color) {
    if (m_xAxis) {
        m_xAxis->setGridLineColor(color);
    }
    if (m_yAxis) {
        m_yAxis->setGridLineColor(color);
    }
}

void FluentChartView::addDataPoint(int seriesIndex, const QPointF& point) {
    if (seriesIndex >= 0 && seriesIndex < m_seriesData.size()) {
        m_seriesData[seriesIndex].data.append(point);
        if (m_series.contains(seriesIndex)) {
            updateSeriesData(m_series[seriesIndex], m_seriesData[seriesIndex].data);
        }
    }
}

void FluentChartView::addDataPoints(int seriesIndex, const QList<QPointF>& points) {
    if (seriesIndex >= 0 && seriesIndex < m_seriesData.size()) {
        m_seriesData[seriesIndex].data.append(points);
        if (m_series.contains(seriesIndex)) {
            updateSeriesData(m_series[seriesIndex], m_seriesData[seriesIndex].data);
        }
    }
}

void FluentChartView::setSeriesData(int seriesIndex, const QList<QPointF>& data) {
    if (seriesIndex >= 0 && seriesIndex < m_seriesData.size()) {
        m_seriesData[seriesIndex].data = data;
        if (m_series.contains(seriesIndex)) {
            updateSeriesData(m_series[seriesIndex], data);
        }
    }
}

void FluentChartView::clearSeriesData(int seriesIndex) {
    if (seriesIndex >= 0 && seriesIndex < m_seriesData.size()) {
        m_seriesData[seriesIndex].data.clear();
        if (m_series.contains(seriesIndex)) {
            updateSeriesData(m_series[seriesIndex], QList<QPointF>());
        }
    }
}

// Animation methods
void FluentChartView::animateSeriesIn(int index) {
    Q_UNUSED(index)
    // Animation implementation would go here
}

void FluentChartView::animateSeriesOut(int index) {
    Q_UNUSED(index)
    // Animation implementation would go here
}

void FluentChartView::animateToNewData(int index, const QList<QPointF>& newData) {
    Q_UNUSED(index)
    Q_UNUSED(newData)
    // Animation implementation would go here
}

// Zoom and pan methods
void FluentChartView::zoomIn() {
    m_chartView->chart()->zoomIn();
}

void FluentChartView::zoomOut() {
    m_chartView->chart()->zoomOut();
}

void FluentChartView::zoomIn(const QRectF& rect) {
    m_chartView->chart()->zoomIn(rect);
}

void FluentChartView::zoomReset() {
    m_chartView->chart()->zoomReset();
}

void FluentChartView::pan(const QPointF& offset) {
    m_chartView->chart()->scroll(offset.x(), offset.y());
}

void FluentChartView::resetPan() {
    m_chartView->chart()->zoomReset();
}

// Export methods
bool FluentChartView::exportToImage(const QString& fileName, const QSize& size) {
    QSize exportSize = size.isValid() ? size : QSize(800, 600);
    QPixmap pixmap(exportSize);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    m_chartView->render(&painter);

    return pixmap.save(fileName);
}

bool FluentChartView::exportToPdf(const QString& fileName, const QSize& size) {
    Q_UNUSED(size)
    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));

    QPainter painter(&writer);
    m_chartView->render(&painter);

    return true;
}

bool FluentChartView::exportToSvg(const QString& fileName, const QSize& size) {
    Q_UNUSED(fileName)
    Q_UNUSED(size)
    // SVG export not implemented yet
    return false;
}

// Interaction methods
void FluentChartView::setToolTipEnabled(bool enabled) {
    m_toolTipEnabled = enabled;
}

bool FluentChartView::isToolTipEnabled() const {
    return m_toolTipEnabled;
}

void FluentChartView::setCrosshairEnabled(bool enabled) {
    m_crosshairEnabled = enabled;
    QWidget::update();
}

bool FluentChartView::isCrosshairEnabled() const {
    return m_crosshairEnabled;
}

// Event handlers
void FluentChartView::paintEvent(QPaintEvent* event) {
    Core::FluentComponent::paintEvent(event);

    if (m_crosshairEnabled) {
        QPainter painter(this);
        paintCrosshair(painter);
    }
}

void FluentChartView::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
}

void FluentChartView::mousePressEvent(QMouseEvent* event) {
    if (m_panEnabled && event->button() == Qt::LeftButton) {
        m_panning = true;
        m_lastPanPoint = event->pos();
    }
    Core::FluentComponent::mousePressEvent(event);
}

void FluentChartView::mouseMoveEvent(QMouseEvent* event) {
    if (m_panning && (event->buttons() & Qt::LeftButton)) {
        QPointF delta = event->pos() - m_lastPanPoint;
        pan(delta);
        m_lastPanPoint = event->pos();
    }

    if (m_crosshairEnabled) {
        m_crosshairPoint = event->pos();
        QWidget::update();
    }

    Core::FluentComponent::mouseMoveEvent(event);
}

void FluentChartView::wheelEvent(QWheelEvent* event) {
    if (m_zoomEnabled) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
    }
    Core::FluentComponent::wheelEvent(event);
}

void FluentChartView::contextMenuEvent(QContextMenuEvent* event) {
    if (m_contextMenu) {
        m_contextMenu->exec(event->globalPos());
    }
    Core::FluentComponent::contextMenuEvent(event);
}

// Slots
void FluentChartView::onSeriesClicked(const QPointF& point) {
    // Find which series was clicked
    QAbstractSeries* series = qobject_cast<QAbstractSeries*>(sender());
    if (series) {
        for (auto it = m_series.begin(); it != m_series.end(); ++it) {
            if (it.value() == series) {
                emit seriesClicked(it.key(), point);
                break;
            }
        }
    }
}

void FluentChartView::onSeriesHovered(const QPointF& point, bool state) {
    // Find which series was hovered
    QAbstractSeries* series = qobject_cast<QAbstractSeries*>(sender());
    if (series) {
        for (auto it = m_series.begin(); it != m_series.end(); ++it) {
            if (it.value() == series) {
                emit seriesHovered(it.key(), point, state);
                break;
            }
        }
    }
}

void FluentChartView::onAxisRangeChanged(qreal min, qreal max) {
    QAbstractAxis* axis = qobject_cast<QAbstractAxis*>(sender());
    if (axis == m_xAxis) {
        emit axisRangeChanged(Qt::Horizontal, min, max);
    } else if (axis == m_yAxis) {
        emit axisRangeChanged(Qt::Vertical, min, max);
    }
}

void FluentChartView::updateTheme() {
    updateChartTheme();
}

void FluentChartView::updateCrosshair(const QPointF& point) {
    m_crosshairPoint = point;
    QWidget::update();
}

// Private methods
void FluentChartView::setupChart() {
    m_chart->setTheme(QChart::ChartThemeLight);
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    // Create default axes
    m_xAxis = new QValueAxis();
    m_yAxis = new QValueAxis();
    m_chart->addAxis(m_xAxis, Qt::AlignBottom);
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);
}

void FluentChartView::setupChartView() {
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);
}

void FluentChartView::setupContextMenu() {
    m_contextMenu = new QMenu(this);

    QAction* zoomInAction = m_contextMenu->addAction("Zoom In");
    QAction* zoomOutAction = m_contextMenu->addAction("Zoom Out");
    QAction* zoomResetAction = m_contextMenu->addAction("Reset Zoom");
    m_contextMenu->addSeparator();
    QAction* exportAction = m_contextMenu->addAction("Export...");

    connect(zoomInAction, &QAction::triggered, this, QOverload<>::of(&FluentChartView::zoomIn));
    connect(zoomOutAction, &QAction::triggered, this, &FluentChartView::zoomOut);
    connect(zoomResetAction, &QAction::triggered, this, &FluentChartView::zoomReset);
    connect(exportAction, &QAction::triggered, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Export Chart", "", "PNG Files (*.png)");
        if (!fileName.isEmpty()) {
            exportToImage(fileName);
        }
    });
}

void FluentChartView::updateChartTheme() {
    switch (m_theme) {
    case FluentChartTheme::Light:
        m_chart->setTheme(QChart::ChartThemeLight);
        break;
    case FluentChartTheme::Dark:
        m_chart->setTheme(QChart::ChartThemeDark);
        break;
    case FluentChartTheme::Auto:
        // Use system theme
        m_chart->setTheme(QChart::ChartThemeLight);
        break;
    }
}

void FluentChartView::updateLegend() {
    QLegend* legend = m_chart->legend();

    switch (m_legendPosition) {
    case FluentLegendPosition::Top:
        legend->setAlignment(Qt::AlignTop);
        legend->setVisible(true);
        break;
    case FluentLegendPosition::Bottom:
        legend->setAlignment(Qt::AlignBottom);
        legend->setVisible(true);
        break;
    case FluentLegendPosition::Left:
        legend->setAlignment(Qt::AlignLeft);
        legend->setVisible(true);
        break;
    case FluentLegendPosition::Right:
        legend->setAlignment(Qt::AlignRight);
        legend->setVisible(true);
        break;
    case FluentLegendPosition::Hidden:
        legend->setVisible(false);
        break;
    }
}

void FluentChartView::updateAxes() {
    // Update axis properties based on current settings
}

QAbstractSeries* FluentChartView::createSeries(const FluentChartSeries& seriesData) {
    QAbstractSeries* series = nullptr;

    switch (seriesData.type) {
    case FluentChartType::Line: {
        auto* lineSeries = new QLineSeries();
        for (const QPointF& point : seriesData.data) {
            lineSeries->append(point);
        }
        series = lineSeries;
        break;
    }
    case FluentChartType::Bar: {
        auto* barSeries = new QBarSeries();
        // Bar series implementation would go here
        series = barSeries;
        break;
    }
    case FluentChartType::Scatter: {
        auto* scatterSeries = new QScatterSeries();
        for (const QPointF& point : seriesData.data) {
            scatterSeries->append(point);
        }
        series = scatterSeries;
        break;
    }
    case FluentChartType::Area: {
        auto* areaSeries = new QAreaSeries();
        // Area series implementation would go here
        series = areaSeries;
        break;
    }
    case FluentChartType::Pie: {
        auto* pieSeries = new QPieSeries();
        // Pie series implementation would go here
        series = pieSeries;
        break;
    }
    default:
        break;
    }

    if (series) {
        series->setName(seriesData.name);
        // Set color based on series type
        if (auto* lineSeries = qobject_cast<QLineSeries*>(series)) {
            lineSeries->setColor(seriesData.color);
        } else if (auto* scatterSeries = qobject_cast<QScatterSeries*>(series)) {
            scatterSeries->setColor(seriesData.color);
        }
        series->setOpacity(seriesData.opacity);
        series->setVisible(seriesData.visible);
    }

    return series;
}

void FluentChartView::updateSeriesData(QAbstractSeries* series, const QList<QPointF>& data) {
    if (auto* lineSeries = qobject_cast<QLineSeries*>(series)) {
        lineSeries->clear();
        for (const QPointF& point : data) {
            lineSeries->append(point);
        }
    } else if (auto* scatterSeries = qobject_cast<QScatterSeries*>(series)) {
        scatterSeries->clear();
        for (const QPointF& point : data) {
            scatterSeries->append(point);
        }
    }
    // Add other series types as needed
}

void FluentChartView::configureAxis(QAbstractAxis* axis, const QString& type) {
    Q_UNUSED(axis)
    Q_UNUSED(type)
    // Axis configuration implementation would go here
}

void FluentChartView::paintCrosshair(QPainter& painter) {
    Q_UNUSED(painter)
    // Crosshair painting implementation would go here
}

void FluentChartView::paintToolTip(QPainter& painter, const QPointF& pos, const QString& text) {
    Q_UNUSED(painter)
    Q_UNUSED(pos)
    Q_UNUSED(text)
    // Tooltip painting implementation would go here
}

QPointF FluentChartView::mapToChart(const QPointF& point) const {
    return m_chartView->chart()->mapToValue(point);
}

QPointF FluentChartView::mapFromChart(const QPointF& point) const {
    return m_chartView->chart()->mapToPosition(point);
}

#include "FluentChartView.moc"
