// include/FluentQt/Components/FluentChartView.h
#pragma once

#include <QAbstractSeries>
#include <QAreaSeries>
#include <QBarSeries>
#include <QCategoryAxis>
#include <QChart>
#include <QChartView>
#include <QDateTimeAxis>
#include <QLegend>
#include <QLineSeries>
#include <QLogValueAxis>
#include <QPieSeries>
#include <QPropertyAnimation>
#include <QScatterSeries>
#include <QVBoxLayout>
#include <QValueAxis>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentChartType { Line, Bar, Area, Pie, Scatter, Mixed };

enum class FluentChartTheme { Light, Dark, Auto };

enum class FluentLegendPosition { Top, Bottom, Left, Right, Hidden };

struct FluentChartSeries {
    QString name;
    FluentChartType type;
    QList<QPointF> data;
    QColor color;
    bool visible{true};
    qreal opacity{1.0};
    QAbstractSeries* series{nullptr};
};

class FluentChartView : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(
        FluentChartTheme theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(FluentLegendPosition legendPosition READ legendPosition WRITE
                   setLegendPosition)
    Q_PROPERTY(
        bool animationEnabled READ animationEnabled WRITE setAnimationEnabled)
    Q_PROPERTY(bool zoomEnabled READ zoomEnabled WRITE setZoomEnabled)
    Q_PROPERTY(bool panEnabled READ panEnabled WRITE setPanEnabled)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing)

public:
    explicit FluentChartView(QWidget* parent = nullptr);

    // Chart properties
    QString title() const;
    void setTitle(const QString& title);

    FluentChartTheme theme() const;
    void setTheme(FluentChartTheme theme);

    FluentLegendPosition legendPosition() const;
    void setLegendPosition(FluentLegendPosition position);

    bool animationEnabled() const;
    void setAnimationEnabled(bool enabled);

    bool zoomEnabled() const;
    void setZoomEnabled(bool enabled);

    bool panEnabled() const;
    void setPanEnabled(bool enabled);

    bool antialiasing() const;
    void setAntialiasing(bool enabled);

    // Series management
    int addSeries(const FluentChartSeries& series);
    void removeSeries(int index);
    void updateSeries(int index, const FluentChartSeries& series);
    void clearSeries();

    int seriesCount() const;
    FluentChartSeries getSeriesData(int index) const;

    void setSeriesVisible(int index, bool visible);
    bool isSeriesVisible(int index) const;

    void setSeriesColor(int index, const QColor& color);
    QColor seriesColor(int index) const;

    void setSeriesOpacity(int index, qreal opacity);
    qreal seriesOpacity(int index) const;

    // Axis configuration
    void setXAxisTitle(const QString& title);
    void setYAxisTitle(const QString& title);

    void setXAxisRange(qreal min, qreal max);
    void setYAxisRange(qreal min, qreal max);

    void setXAxisType(
        const QString& type);  // "value", "category", "datetime", "log"
    void setYAxisType(const QString& type);

    void setXAxisVisible(bool visible);
    void setYAxisVisible(bool visible);

    void setGridVisible(bool visible);
    void setGridColor(const QColor& color);

    // Data operations
    void addDataPoint(int seriesIndex, const QPointF& point);
    void addDataPoints(int seriesIndex, const QList<QPointF>& points);
    void setSeriesData(int seriesIndex, const QList<QPointF>& data);
    void clearSeriesData(int seriesIndex);

    // Animation
    void animateSeriesIn(int index);
    void animateSeriesOut(int index);
    void animateToNewData(int index, const QList<QPointF>& newData);

    // Zoom and pan
    void zoomIn();
    void zoomOut();
    void zoomIn(const QRectF& rect);
    void zoomReset();

    void pan(const QPointF& offset);
    void resetPan();

    // Export
    bool exportToImage(const QString& fileName, const QSize& size = QSize());
    bool exportToPdf(const QString& fileName, const QSize& size = QSize());
    bool exportToSvg(const QString& fileName, const QSize& size = QSize());

    // Interaction
    void setToolTipEnabled(bool enabled);
    bool isToolTipEnabled() const;

    void setCrosshairEnabled(bool enabled);
    bool isCrosshairEnabled() const;

    // Chart access
    QChart* chart() const { return m_chart; }
    QChartView* chartView() const { return m_chartView; }

signals:
    void titleChanged(const QString& title);
    void themeChanged(FluentChartTheme theme);
    void seriesClicked(int index, const QPointF& point);
    void seriesHovered(int index, const QPointF& point, bool state);
    void axisRangeChanged(Qt::Orientation orientation, qreal min, qreal max);
    void zoomChanged(const QRectF& rect);
    void panChanged(const QPointF& offset);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onSeriesClicked(const QPointF& point);
    void onSeriesHovered(const QPointF& point, bool state);
    void onAxisRangeChanged(qreal min, qreal max);
    void updateTheme();
    void updateCrosshair(const QPointF& point);

private:
    void setupChart();
    void setupChartView();
    void setupContextMenu();
    void updateChartTheme();
    void updateLegend();
    void updateAxes();

    QAbstractSeries* createSeries(const FluentChartSeries& seriesData);
    void updateSeriesData(QAbstractSeries* series, const QList<QPointF>& data);
    void configureAxis(QAbstractAxis* axis, const QString& type);

    void paintCrosshair(QPainter& painter);
    void paintToolTip(QPainter& painter, const QPointF& pos,
                      const QString& text);

    QPointF mapToChart(const QPointF& point) const;
    QPointF mapFromChart(const QPointF& point) const;

private:
    QVBoxLayout* m_layout;
    QChart* m_chart;
    QChartView* m_chartView;

    FluentChartTheme m_theme{FluentChartTheme::Auto};
    FluentLegendPosition m_legendPosition{FluentLegendPosition::Bottom};

    bool m_animationEnabled{true};
    bool m_zoomEnabled{true};
    bool m_panEnabled{true};
    bool m_antialiasing{true};
    bool m_toolTipEnabled{true};
    bool m_crosshairEnabled{false};

    QList<FluentChartSeries> m_seriesData;
    QMap<int, QAbstractSeries*> m_series;

    QAbstractAxis* m_xAxis{nullptr};
    QAbstractAxis* m_yAxis{nullptr};

    // Interaction state
    bool m_panning{false};
    QPointF m_lastPanPoint;
    QPointF m_crosshairPoint;

    // Animation
    QPropertyAnimation* m_zoomAnimation{nullptr};
    QPropertyAnimation* m_panAnimation{nullptr};

    // Context menu
    QMenu* m_contextMenu{nullptr};
};

}  // namespace FluentQt::Components
