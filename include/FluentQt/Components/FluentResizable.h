// include/FluentQt/Components/FluentResizable.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QWidget>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <memory>

namespace FluentQt::Components {

enum class FluentResizeDirection {
    None = 0x00,
    Horizontal = 0x01,
    Vertical = 0x02,
    Both = Horizontal | Vertical
};

enum class FluentResizeHandle {
    None = 0x00,
    Top = 0x01,
    Bottom = 0x02,
    Left = 0x04,
    Right = 0x08,
    TopLeft = Top | Left,
    TopRight = Top | Right,
    BottomLeft = Bottom | Left,
    BottomRight = Bottom | Right
};

Q_DECLARE_FLAGS(FluentResizeHandles, FluentResizeHandle)
Q_DECLARE_OPERATORS_FOR_FLAGS(FluentResizeHandles)

enum class FluentResizeMode {
    Immediate,      // Resize immediately during drag
    Preview,        // Show preview during drag, apply on release
    Smooth          // Smooth animated resize
};

class FluentResizeHandleWidget;
class FluentResizePreview;

class FluentResizable : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentResizeDirection resizeDirection READ resizeDirection WRITE setResizeDirection NOTIFY resizeDirectionChanged)
    Q_PROPERTY(FluentResizeHandles enabledHandles READ enabledHandles WRITE setEnabledHandles NOTIFY enabledHandlesChanged)
    Q_PROPERTY(FluentResizeMode resizeMode READ resizeMode WRITE setResizeMode NOTIFY resizeModeChanged)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize NOTIFY minimumSizeChanged)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize NOTIFY maximumSizeChanged)
    Q_PROPERTY(int handleSize READ handleSize WRITE setHandleSize NOTIFY handleSizeChanged)
    Q_PROPERTY(bool showHandles READ showHandles WRITE setShowHandles NOTIFY showHandlesChanged)
    Q_PROPERTY(bool smoothResize READ smoothResize WRITE setSmoothResize)
    Q_PROPERTY(bool touchEnabled READ isTouchEnabled WRITE setTouchEnabled)
    Q_PROPERTY(bool snapToGrid READ snapToGrid WRITE setSnapToGrid)
    Q_PROPERTY(int gridSize READ gridSize WRITE setGridSize)
    Q_PROPERTY(qreal aspectRatio READ aspectRatio WRITE setAspectRatio)
    Q_PROPERTY(bool maintainAspectRatio READ maintainAspectRatio WRITE setMaintainAspectRatio)

public:
    explicit FluentResizable(QWidget* parent = nullptr);
    explicit FluentResizable(QWidget* contentWidget, QWidget* parent = nullptr);
    ~FluentResizable() override;

    // Content management
    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const { return m_contentWidget; }

    // Resize direction
    FluentResizeDirection resizeDirection() const noexcept { return m_resizeDirection; }
    void setResizeDirection(FluentResizeDirection direction);

    // Handle configuration
    FluentResizeHandles enabledHandles() const noexcept { return m_enabledHandles; }
    void setEnabledHandles(FluentResizeHandles handles);

    FluentResizeMode resizeMode() const noexcept { return m_resizeMode; }
    void setResizeMode(FluentResizeMode mode);

    int handleSize() const noexcept { return m_handleSize; }
    void setHandleSize(int size);

    bool showHandles() const noexcept { return m_showHandles; }
    void setShowHandles(bool show);

    // Size constraints
    QSize minimumSize() const noexcept { return m_minimumSize; }
    void setMinimumSize(const QSize& size);

    QSize maximumSize() const noexcept { return m_maximumSize; }
    void setMaximumSize(const QSize& size);

    // Animation and behavior
    bool smoothResize() const noexcept { return m_smoothResize; }
    void setSmoothResize(bool smooth);

    bool isTouchEnabled() const noexcept { return m_touchEnabled; }
    void setTouchEnabled(bool enabled);

    // Grid snapping
    bool snapToGrid() const noexcept { return m_snapToGrid; }
    void setSnapToGrid(bool snap);

    int gridSize() const noexcept { return m_gridSize; }
    void setGridSize(int size);

    // Aspect ratio
    qreal aspectRatio() const noexcept { return m_aspectRatio; }
    void setAspectRatio(qreal ratio);

    bool maintainAspectRatio() const noexcept { return m_maintainAspectRatio; }
    void setMaintainAspectRatio(bool maintain);

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Programmatic resize
    void resizeToSize(const QSize& size, bool animated = true);
    void resizeToWidth(int width, bool animated = true);
    void resizeToHeight(int height, bool animated = true);

public slots:
    void resetToMinimumSize();
    void resetToMaximumSize();
    void resetToSizeHint();
    void toggleHandleVisibility();

signals:
    void resizeDirectionChanged(FluentResizeDirection direction);
    void enabledHandlesChanged(FluentResizeHandles handles);
    void resizeModeChanged(FluentResizeMode mode);
    void minimumSizeChanged(const QSize& size);
    void maximumSizeChanged(const QSize& size);
    void handleSizeChanged(int size);
    void showHandlesChanged(bool show);
    void resizeStarted(const QSize& startSize);
    void resizing(const QSize& currentSize);
    void resizeFinished(const QSize& finalSize);
    void resizeCancelled();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    
    // Touch events
    bool event(QEvent* event) override;
    void touchEvent(QTouchEvent* event);

    // Keyboard events
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from, Core::FluentState to) override;

private slots:
    void onResizeAnimationValueChanged(const QVariant& value);
    void onResizeAnimationFinished();
    void onHandleHoverChanged(bool hovered);
    void onThemeChanged();

private:
    // Setup methods
    void setupLayout();
    void setupHandles();
    void setupAnimations();
    void setupAccessibility();

    // Handle management
    void updateHandlePositions();
    void updateHandleVisibility();
    void updateHandleCursors();
    FluentResizeHandle getHandleAt(const QPoint& pos) const;
    QRect getHandleRect(FluentResizeHandle handle) const;
    Qt::CursorShape getCursorForHandle(FluentResizeHandle handle) const;

    // Resize logic
    void startResize(FluentResizeHandle handle, const QPoint& startPos);
    void updateResize(const QPoint& currentPos);
    void finishResize();
    void cancelResize();
    QSize calculateNewSize(FluentResizeHandle handle, const QPoint& delta) const;
    QSize constrainSize(const QSize& size) const;
    QSize snapSizeToGrid(const QSize& size) const;
    QSize maintainAspectRatioSize(const QSize& size, FluentResizeHandle handle) const;

    // Animation methods
    void animateToSize(const QSize& targetSize);
    void updateResizePreview(const QSize& previewSize);

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintHandles(QPainter& painter);
    void paintHandle(QPainter& painter, FluentResizeHandle handle, const QRect& rect);
    void paintResizePreview(QPainter& painter);
    void paintGridLines(QPainter& painter);

    // Style methods
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getHandleColor() const;
    QColor getHandleHoverColor() const;
    QColor getPreviewColor() const;
    QPen getBorderPen() const;
    QPen getPreviewPen() const;

    // Utility methods
    void updateContentGeometry();
    void updateCursor();
    bool isResizing() const { return m_resizing; }
    QPoint mapToGlobal(const QPoint& pos) const;
    QPoint mapFromGlobal(const QPoint& pos) const;

private:
    // Content
    QWidget* m_contentWidget{nullptr};

    // Configuration
    FluentResizeDirection m_resizeDirection{FluentResizeDirection::Both};
    FluentResizeHandles m_enabledHandles{static_cast<FluentResizeHandles>(FluentResizeHandle::BottomRight)};
    FluentResizeMode m_resizeMode{FluentResizeMode::Immediate};
    int m_handleSize{8};
    bool m_showHandles{true};
    bool m_smoothResize{true};
    bool m_touchEnabled{true};

    // Size constraints
    QSize m_minimumSize{50, 50};
    QSize m_maximumSize{QWIDGETSIZE_MAX, QWIDGETSIZE_MAX};

    // Grid and aspect ratio
    bool m_snapToGrid{false};
    int m_gridSize{10};
    qreal m_aspectRatio{0.0}; // 0.0 means no fixed aspect ratio
    bool m_maintainAspectRatio{false};

    // Resize state
    bool m_resizing{false};
    FluentResizeHandle m_activeHandle{FluentResizeHandle::None};
    QPoint m_resizeStartPos;
    QSize m_resizeStartSize;
    QPoint m_lastMousePos;

    // Handle widgets
    std::array<std::unique_ptr<FluentResizeHandleWidget>, 8> m_handles;
    FluentResizeHandle m_hoveredHandle{FluentResizeHandle::None};

    // Animation
    std::unique_ptr<QPropertyAnimation> m_resizeAnimation;
    std::unique_ptr<FluentResizePreview> m_resizePreview;

    // Touch support
    int m_activeTouchId{-1};
    QPointF m_touchStartPos;

    // Performance optimization
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

} // namespace FluentQt::Components
