// include/FluentQt/Components/FluentSheet.h
#pragma once

#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>
#include <memory>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentSheetDirection { Top, Bottom, Left, Right };

enum class FluentSheetSize {
    Small,   // 25% of screen
    Medium,  // 50% of screen
    Large,   // 75% of screen
    Full,    // 100% of screen
    Custom   // User-defined size
};

enum class FluentSheetBehavior {
    Modal,      // Blocks interaction with parent
    Modeless,   // Allows interaction with parent
    Persistent  // Stays open until explicitly closed
};

class FluentSheetOverlay;
class FluentSheetContent;

class FluentSheet : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentSheetDirection direction READ direction WRITE setDirection
                   NOTIFY directionChanged)
    Q_PROPERTY(FluentSheetSize sheetSize READ sheetSize WRITE setSheetSize
                   NOTIFY sheetSizeChanged)
    Q_PROPERTY(FluentSheetBehavior behavior READ behavior WRITE setBehavior
                   NOTIFY behaviorChanged)
    Q_PROPERTY(
        bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    Q_PROPERTY(bool overlayVisible READ overlayVisible WRITE setOverlayVisible)
    Q_PROPERTY(qreal overlayOpacity READ overlayOpacity WRITE setOverlayOpacity)
    Q_PROPERTY(
        int animationDuration READ animationDuration WRITE setAnimationDuration)
    Q_PROPERTY(
        QEasingCurve::Type easingCurve READ easingCurve WRITE setEasingCurve)
    Q_PROPERTY(bool closeOnOverlayClick READ closeOnOverlayClick WRITE
                   setCloseOnOverlayClick)
    Q_PROPERTY(bool closeOnEscape READ closeOnEscape WRITE setCloseOnEscape)
    Q_PROPERTY(bool dragToClose READ dragToClose WRITE setDragToClose)
    Q_PROPERTY(QSize customSize READ customSize WRITE setCustomSize)
    Q_PROPERTY(bool autoResize READ autoResize WRITE setAutoResize)

public:
    explicit FluentSheet(QWidget* parent = nullptr);
    explicit FluentSheet(FluentSheetDirection direction,
                         QWidget* parent = nullptr);
    explicit FluentSheet(FluentSheetDirection direction, FluentSheetSize size,
                         QWidget* parent = nullptr);
    ~FluentSheet() override;

    // Direction and size
    FluentSheetDirection direction() const noexcept { return m_direction; }
    void setDirection(FluentSheetDirection direction);

    FluentSheetSize sheetSize() const noexcept { return m_sheetSize; }
    void setSheetSize(FluentSheetSize size);

    QSize customSize() const noexcept { return m_customSize; }
    void setCustomSize(const QSize& size);

    // Behavior
    FluentSheetBehavior behavior() const noexcept { return m_behavior; }
    void setBehavior(FluentSheetBehavior behavior);

    // Overlay properties
    bool overlayVisible() const noexcept { return m_overlayVisible; }
    void setOverlayVisible(bool visible);

    qreal overlayOpacity() const noexcept { return m_overlayOpacity; }
    void setOverlayOpacity(qreal opacity);

    // Animation properties
    int animationDuration() const noexcept { return m_animationDuration; }
    void setAnimationDuration(int duration);

    QEasingCurve::Type easingCurve() const noexcept { return m_easingCurve; }
    void setEasingCurve(QEasingCurve::Type curve);

    // Interaction properties
    bool closeOnOverlayClick() const noexcept { return m_closeOnOverlayClick; }
    void setCloseOnOverlayClick(bool close);

    bool closeOnEscape() const noexcept { return m_closeOnEscape; }
    void setCloseOnEscape(bool close);

    bool dragToClose() const noexcept { return m_dragToClose; }
    void setDragToClose(bool drag);

    bool autoResize() const noexcept { return m_autoResize; }
    void setAutoResize(bool resize);

    // Content management
    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const;

    void setTitle(const QString& title);
    QString title() const;

    void setSubtitle(const QString& subtitle);
    QString subtitle() const;

    // State queries
    bool isOpen() const { return m_isOpen; }
    bool isAnimating() const;

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Convenience methods
    static FluentSheet* createBottomSheet(QWidget* parent = nullptr);
    static FluentSheet* createTopSheet(QWidget* parent = nullptr);
    static FluentSheet* createLeftSheet(QWidget* parent = nullptr);
    static FluentSheet* createRightSheet(QWidget* parent = nullptr);

public slots:
    void open();
    void close();
    void toggle();
    void openWithAnimation();
    void closeWithAnimation();

signals:
    void directionChanged(FluentSheetDirection direction);
    void sheetSizeChanged(FluentSheetSize size);
    void behaviorChanged(FluentSheetBehavior behavior);
    void visibilityChanged(bool visible);
    void aboutToOpen();
    void opened();
    void aboutToClose();
    void closed();
    void overlayClicked();
    void dragStarted();
    void dragFinished();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;

    // Focus management
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

private slots:
    void onOpenAnimationValueChanged(const QVariant& value);
    void onOpenAnimationFinished();
    void onCloseAnimationValueChanged(const QVariant& value);
    void onCloseAnimationFinished();
    void onOverlayClicked();
    void onThemeChanged();

private:
    // Setup methods
    void setupSheet();
    void setupOverlay();
    void setupContent();
    void setupAnimations();
    void setupAccessibility();
    void setupElevationEffect();

    // Animation methods
    void startOpenAnimation();
    void startCloseAnimation();
    void stopAllAnimations();
    QRect calculateOpenGeometry() const;
    QRect calculateClosedGeometry() const;
    QSize calculateSheetSize() const;

    // Overlay methods
    void showOverlay();
    void hideOverlay();
    void updateOverlayGeometry();

    // Content methods
    void updateContentGeometry();
    void updateContentLayout();

    // Drag handling
    void startDrag(const QPoint& startPos);
    void updateDrag(const QPoint& currentPos);
    void finishDrag();
    void cancelDrag();
    bool shouldCloseFromDrag(const QPoint& delta) const;

    // Focus management
    void captureFocus();
    void restoreFocus();
    void updateFocusChain();

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintShadow(QPainter& painter, const QRect& rect);

    // Style methods
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getShadowColor() const;
    QPen getBorderPen() const;

    // Utility methods
    void updateGeometry();
    void updateParentGeometry();
    QWidget* findTopLevelParent() const;
    bool isVerticalDirection() const;
    bool isHorizontalDirection() const;

private:
    // Configuration
    FluentSheetDirection m_direction{FluentSheetDirection::Bottom};
    FluentSheetSize m_sheetSize{FluentSheetSize::Medium};
    FluentSheetBehavior m_behavior{FluentSheetBehavior::Modal};
    QSize m_customSize{400, 300};

    // Overlay properties
    bool m_overlayVisible{true};
    qreal m_overlayOpacity{0.5};

    // Animation properties
    int m_animationDuration{300};
    QEasingCurve::Type m_easingCurve{QEasingCurve::OutCubic};

    // Interaction properties
    bool m_closeOnOverlayClick{true};
    bool m_closeOnEscape{true};
    bool m_dragToClose{true};
    bool m_autoResize{true};

    // State
    bool m_isOpen{false};
    bool m_isDragging{false};
    QPoint m_dragStartPos;
    QPoint m_dragCurrentPos;

    // Components
    std::unique_ptr<FluentSheetOverlay> m_overlay;
    std::unique_ptr<FluentSheetContent> m_content;

    // Animations
    std::unique_ptr<QPropertyAnimation> m_openAnimation;
    std::unique_ptr<QPropertyAnimation> m_closeAnimation;
    std::unique_ptr<QPropertyAnimation> m_overlayAnimation;
    std::unique_ptr<QParallelAnimationGroup> m_animationGroup;

    // Focus management
    QWidget* m_previousFocusWidget{nullptr};
    QList<QWidget*> m_focusChain;

    // Performance optimization
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

}  // namespace FluentQt::Components
