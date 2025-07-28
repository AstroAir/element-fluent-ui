// include/FluentQt/Components/FluentSplitter.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QSplitter>
#include <QSplitterHandle>
#include <QPropertyAnimation>
#include <QTimer>

namespace FluentQt::Components {

enum class FluentSplitterResizeMode {
    Interactive,
    Animated,
    Smooth
};

class FluentSplitterHandle : public QSplitterHandle {
    Q_OBJECT

public:
    explicit FluentSplitterHandle(Qt::Orientation orientation, QSplitter* parent = nullptr);

    Qt::Orientation orientation() const { return m_orientation; }
    void setOrientation(Qt::Orientation orientation);

    bool isCollapsible() const { return m_collapsible; }
    void setCollapsible(bool collapsible);

    int handleWidth() const { return m_handleWidth; }
    void setHandleWidth(int width);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    QSize sizeHint() const override;

signals:
    void handleMoved(int delta);
    void handlePressed();
    void handleReleased();
    void handleDoubleClicked();
    void collapseRequested();

private:
    void paintHandle(QPainter& painter, const QRect& rect);
    void paintGrip(QPainter& painter, const QRect& rect);
    void paintCollapseButton(QPainter& painter, const QRect& rect);
    
    QRect getGripRect() const;
    QRect getCollapseButtonRect() const;

private:
    Qt::Orientation m_orientation;
    bool m_collapsible{true};
    int m_handleWidth{8};
    bool m_hovered{false};
    bool m_pressed{false};
    QPoint m_pressPos;
};

class FluentSplitter : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(FluentSplitterResizeMode resizeMode READ resizeMode WRITE setResizeMode)
    Q_PROPERTY(bool opaqueResize READ opaqueResize WRITE setOpaqueResize)
    Q_PROPERTY(bool collapsible READ collapsible WRITE setCollapsible)
    Q_PROPERTY(int handleWidth READ handleWidth WRITE setHandleWidth)
    Q_PROPERTY(bool animatedResize READ animatedResize WRITE setAnimatedResize)

public:
    explicit FluentSplitter(QWidget* parent = nullptr);
    explicit FluentSplitter(Qt::Orientation orientation, QWidget* parent = nullptr);

    // Orientation
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    // Resize mode
    FluentSplitterResizeMode resizeMode() const { return m_resizeMode; }
    void setResizeMode(FluentSplitterResizeMode mode);

    bool opaqueResize() const;
    void setOpaqueResize(bool opaque);

    bool collapsible() const;
    void setCollapsible(bool collapsible);

    int handleWidth() const;
    void setHandleWidth(int width);

    bool animatedResize() const { return m_animatedResize; }
    void setAnimatedResize(bool animated);

    // Widget management
    void addWidget(QWidget* widget);
    void insertWidget(int index, QWidget* widget);
    void removeWidget(QWidget* widget);
    QWidget* widget(int index) const;
    int count() const;
    int indexOf(QWidget* widget) const;

    // Sizes
    QList<int> sizes() const;
    void setSizes(const QList<int>& sizes);
    void setSizes(const QList<int>& sizes, bool animated);

    // Proportional sizes
    QList<double> proportionalSizes() const;
    void setProportionalSizes(const QList<double>& proportions);
    void setProportionalSizes(const QList<double>& proportions, bool animated);

    // Collapse/expand
    void collapseWidget(int index);
    void expandWidget(int index);
    void toggleWidget(int index);
    bool isWidgetCollapsed(int index) const;

    // Responsive behavior
    void setResponsiveBreakpoints(const QList<int>& breakpoints);
    QList<int> responsiveBreakpoints() const;
    
    void setResponsiveLayout(int breakpoint, const QList<int>& sizes);
    void setResponsiveLayout(int breakpoint, const QList<double>& proportions);

    // Constraints
    void setMinimumSize(int index, int size);
    int minimumSize(int index) const;
    
    void setMaximumSize(int index, int size);
    int maximumSize(int index) const;
    
    void setStretchFactor(int index, int stretch);
    int stretchFactor(int index) const;

    // Persistence
    QByteArray saveState() const;
    bool restoreState(const QByteArray& state);

    // Utilities
    int totalSize() const;
    int availableSize() const;
    QRect widgetRect(int index) const;

public slots:
    void refresh();
    void resetToDefaults();
    void distributeEvenly();
    void distributeEvenly(bool animated);

signals:
    void orientationChanged(Qt::Orientation orientation);
    void splitterMoved(int pos, int index);
    void widgetCollapsed(int index);
    void widgetExpanded(int index);
    void responsiveLayoutChanged(int breakpoint);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void childEvent(QChildEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void onHandleMoved(int delta);
    void onHandlePressed();
    void onHandleReleased();
    void onHandleDoubleClicked();
    void onCollapseRequested();
    void onAnimationFinished();
    void checkResponsiveLayout();

private:
    void setupSplitter();
    void setupLayout();
    void updateHandles();
    void updateResponsiveLayout();
    void animateToSizes(const QList<int>& targetSizes);

    FluentSplitterHandle* createHandle();
    FluentSplitterHandle* handleAt(int index) const;
    int handleIndex(FluentSplitterHandle* handle) const;

    QList<int> calculateSizes(const QList<double>& proportions) const;
    QList<double> calculateProportions(const QList<int>& sizes) const;

private:
    // Core splitter functionality through composition
    QSplitter* m_splitter{nullptr};

    // FluentSplitter specific properties
    FluentSplitterResizeMode m_resizeMode{FluentSplitterResizeMode::Interactive};
    bool m_animatedResize{false};

    // Custom handles
    QList<FluentSplitterHandle*> m_handles;

    // Responsive layout
    QList<int> m_responsiveBreakpoints;
    QMap<int, QList<int>> m_responsiveLayouts;
    QMap<int, QList<double>> m_responsiveProportions;
    int m_currentBreakpoint{-1};

    // Animation
    QPropertyAnimation* m_resizeAnimation{nullptr};
    QList<int> m_targetSizes;

    // Interaction state
    bool m_resizing{false};
    int m_activeHandle{-1};
    QPoint m_lastPos;

    // Timer for responsive updates
    QTimer* m_responsiveTimer{nullptr};
};

} // namespace FluentQt::Components