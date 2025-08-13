// include/FluentQt/Components/FluentCarousel.h
#pragma once

#include <QEasingCurve>
#include <QElapsedTimer>
#include <QGestureEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPanGesture>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QStackedWidget>
#include <QSwipeGesture>
#include <QTimer>
#include <QTouchEvent>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>
#include <functional>
#include <memory>
#include <vector>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

// Forward declarations
class FluentCarouselItem;
class FluentCarouselIndicator;
class FluentCarouselNavigation;

/**
 * @brief Carousel transition effects
 */
enum class FluentCarouselTransition {
    Slide,      // Horizontal slide transition
    Fade,       // Fade in/out transition
    Scale,      // Scale transition
    Flip,       // 3D flip transition
    Cube,       // 3D cube transition
    Coverflow,  // Coverflow-style transition
    Stack,      // Stack transition
    Parallax,   // Parallax scrolling effect
    Custom      // Custom transition
};

/**
 * @brief Carousel navigation style
 */
enum class FluentCarouselNavigationStyle {
    None,        // No navigation controls
    Arrows,      // Previous/Next arrow buttons
    Dots,        // Indicator dots
    Numbers,     // Numeric indicators
    Thumbnails,  // Thumbnail navigation
    Combined     // Arrows + Dots
};

/**
 * @brief Carousel orientation
 */
enum class FluentCarouselOrientation {
    Horizontal,  // Left-to-right navigation
    Vertical     // Top-to-bottom navigation
};

/**
 * @brief Auto-play behavior
 */
enum class FluentCarouselAutoPlay {
    None,      // No auto-play
    Forward,   // Auto-play forward
    Backward,  // Auto-play backward
    PingPong,  // Auto-play forward then backward
    // Aliases for test compatibility
    Disabled = None,
    Enabled = Forward
};

/**
 * @brief Touch/swipe gesture sensitivity
 */
enum class FluentCarouselGestureSensitivity {
    Low,     // Requires more movement to trigger
    Medium,  // Default sensitivity
    High,    // Triggers with minimal movement
    Custom   // Custom threshold values
};

/**
 * @brief Carousel item alignment
 */
enum class FluentCarouselAlignment {
    Start,   // Align to start (left/top)
    Center,  // Center alignment
    End      // Align to end (right/bottom)
};

/**
 * @brief Configuration structure for carousel behavior
 */
struct FluentCarouselConfig {
    // Basic settings
    FluentCarouselTransition transition{FluentCarouselTransition::Slide};
    FluentCarouselNavigationStyle navigationStyle{
        FluentCarouselNavigationStyle::Combined};
    FluentCarouselOrientation orientation{
        FluentCarouselOrientation::Horizontal};
    FluentCarouselAlignment alignment{FluentCarouselAlignment::Center};

    // Animation settings
    std::chrono::milliseconds transitionDuration{300};
    Animation::FluentEasing transitionEasing{
        Animation::FluentEasing::EaseOutCubic};

    // Auto-play settings
    FluentCarouselAutoPlay autoPlay{FluentCarouselAutoPlay::None};
    std::chrono::milliseconds autoPlayInterval{3000};
    bool pauseOnHover{true};
    bool pauseOnFocus{true};

    // Touch/gesture settings
    bool enableTouch{true};
    bool enableSwipe{true};
    FluentCarouselGestureSensitivity gestureSensitivity{
        FluentCarouselGestureSensitivity::Medium};
    qreal swipeThreshold{0.3};  // Percentage of width/height to trigger swipe
    qreal velocityThreshold{500.0};  // Pixels per second

    // Visual settings
    bool showIndicators{true};
    bool showNavigation{true};
    bool enableKeyboard{true};
    bool enableWheel{false};
    bool infinite{true};     // Loop back to start/end
    bool centerMode{false};  // Show partial previous/next items

    // Aliases for test compatibility (using separate fields)
    bool showNavigationButtons{true};
    bool wrapAround{true};

    // Performance settings
    bool lazyLoad{false};  // Load items on demand
    int preloadCount{1};   // Number of items to preload
    bool useHardwareAcceleration{true};

    // Accessibility settings
    bool enableAccessibility{true};
    QString ariaLabel{"Carousel"};
    bool announceChanges{true};

    // Layout settings
    int itemSpacing{0};  // Space between items
    QMargins contentMargins{0, 0, 0, 0};
    int visibleItems{1};  // Number of items visible at once

    // Default constructor
    FluentCarouselConfig() = default;
};

/**
 * @brief Carousel item data structure
 */
struct FluentCarouselItemData {
    QWidget* widget{nullptr};
    QString title;
    QString description;
    QIcon icon;
    QVariant userData;
    bool enabled{true};
    bool visible{true};

    FluentCarouselItemData() = default;
    explicit FluentCarouselItemData(QWidget* w) : widget(w) {}
    FluentCarouselItemData(QWidget* w, const QString& t)
        : widget(w), title(t) {}
};

/**
 * @brief Base carousel component implementing Microsoft Fluent Design
 * principles
 *
 * FluentCarousel provides a flexible, animated carousel widget with support
 * for:
 * - Multiple transition effects and navigation styles
 * - Touch/swipe gestures for mobile devices
 * - Auto-play functionality with configurable timing
 * - Keyboard navigation and accessibility features
 * - Responsive design and theme integration
 * - Performance optimizations for large datasets
 */
class FluentCarousel : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY
                   currentIndexChanged)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
    Q_PROPERTY(FluentCarouselTransition transition READ transition WRITE
                   setTransition NOTIFY transitionChanged)
    Q_PROPERTY(
        FluentCarouselNavigationStyle navigationStyle READ navigationStyle WRITE
            setNavigationStyle NOTIFY navigationStyleChanged)
    Q_PROPERTY(FluentCarouselOrientation orientation READ orientation WRITE
                   setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(bool autoPlay READ isAutoPlayEnabled WRITE setAutoPlayEnabled
                   NOTIFY autoPlayChanged)
    Q_PROPERTY(int autoPlayInterval READ autoPlayInterval WRITE
                   setAutoPlayInterval NOTIFY autoPlayIntervalChanged)
    Q_PROPERTY(
        bool infinite READ isInfinite WRITE setInfinite NOTIFY infiniteChanged)
    Q_PROPERTY(bool enableTouch READ isTouchEnabled WRITE setTouchEnabled NOTIFY
                   touchEnabledChanged)
    Q_PROPERTY(qreal transitionProgress READ transitionProgress WRITE
                   setTransitionProgress NOTIFY transitionProgressChanged)

public:
    explicit FluentCarousel(QWidget* parent = nullptr);
    explicit FluentCarousel(const FluentCarouselConfig& config,
                            QWidget* parent = nullptr);
    ~FluentCarousel() override;

    // Configuration
    const FluentCarouselConfig& config() const noexcept { return m_config; }
    void setConfig(const FluentCarouselConfig& config);

    // Item management
    int itemCount() const noexcept { return static_cast<int>(m_items.size()); }
    void addItem(QWidget* widget);
    void addItem(const FluentCarouselItemData& itemData);
    void insertItem(int index, QWidget* widget);
    void insertItem(int index, const FluentCarouselItemData& itemData);
    void removeItem(int index);
    void removeItem(QWidget* widget);
    void removeItemAt(int index) { removeItem(index); }  // Alias for tests
    void clearItems();
    void clear() { clearItems(); }  // Alias for tests

    QWidget* itemAt(int index) const;
    FluentCarouselItemData itemData(int index) const;
    void setItemData(int index, const FluentCarouselItemData& data);

    // Navigation
    int currentIndex() const noexcept { return m_currentIndex; }
    void setCurrentIndex(int index, bool animated = true);

    bool canGoToPrevious() const;
    bool canGoToNext() const;

    // Transition properties
    FluentCarouselTransition transition() const noexcept {
        return m_config.transition;
    }
    void setTransition(FluentCarouselTransition transition);

    FluentCarouselNavigationStyle navigationStyle() const noexcept {
        return m_config.navigationStyle;
    }
    void setNavigationStyle(FluentCarouselNavigationStyle style);

    FluentCarouselOrientation orientation() const noexcept {
        return m_config.orientation;
    }
    void setOrientation(FluentCarouselOrientation orientation);

    // Auto-play
    bool isAutoPlayEnabled() const noexcept {
        return m_config.autoPlay != FluentCarouselAutoPlay::None;
    }
    void setAutoPlayEnabled(bool enabled);

    int autoPlayInterval() const noexcept {
        return static_cast<int>(m_config.autoPlayInterval.count());
    }
    void setAutoPlayInterval(int milliseconds);

    bool isAutoPlayActive() const noexcept {
        return m_autoPlayTimer && m_autoPlayTimer->isActive();
    }

    // Loop behavior
    bool isInfinite() const noexcept { return m_config.infinite; }
    void setInfinite(bool infinite);

    // Touch/gesture support
    bool isTouchEnabled() const noexcept { return m_config.enableTouch; }
    void setTouchEnabled(bool enabled);

    // Configuration methods for test compatibility
    void setConfiguration(const FluentCarouselConfig& config) {
        setConfig(config);
    }
    int transitionDuration() const {
        return static_cast<int>(m_config.transitionDuration.count());
    }
    void setTransitionDuration(int duration);
    bool wrapAround() const { return m_config.infinite; }
    void setWrapAround(bool wrap);
    bool showNavigationButtons() const { return m_config.showNavigation; }
    void setShowNavigationButtons(bool show);

    // Animation state
    qreal transitionProgress() const noexcept { return m_transitionProgress; }
    void setTransitionProgress(qreal progress);
    bool isTransitioning() const noexcept { return m_transitioning; }

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    // Navigation slots
    void goToPrevious(bool animated = true);
    void goToNext(bool animated = true);
    void goToFirst(bool animated = true);
    void goToLast(bool animated = true);

    // Convenience navigation methods (aliases)
    void next(bool animated = true) { goToNext(animated); }
    void previous(bool animated = true) { goToPrevious(animated); }
    void goToIndex(int index, bool animated = true) {
        setCurrentIndex(index, animated);
    }

    // Auto-play control
    void startAutoPlay();
    void stopAutoPlay();
    void pauseAutoPlay();
    void resumeAutoPlay();

    // Animation control
    void stopTransition();

signals:
    // Navigation signals
    void currentIndexChanged(int index);
    void itemCountChanged(int count);
    void transitionStarted(int fromIndex, int toIndex);
    void transitionFinished(int index);
    void transitionProgressChanged(qreal progress);

    // Item management signals (for test compatibility)
    void itemChanged(QWidget* current, QWidget* previous);
    void itemAdded(QWidget* item, int index);
    void itemRemoved(QWidget* item, int index);

    // Configuration signals
    void transitionChanged(FluentCarouselTransition transition);
    void navigationStyleChanged(FluentCarouselNavigationStyle style);
    void orientationChanged(FluentCarouselOrientation orientation);
    void infiniteChanged(bool infinite);
    void touchEnabledChanged(bool enabled);

    // Auto-play signals
    void autoPlayChanged(bool enabled);
    void autoPlayIntervalChanged(int interval);
    void autoPlayStarted();
    void autoPlayStopped();
    void autoPlayPaused();
    void autoPlayResumed();
    void autoPlayProgressChanged(qreal progress);

    // Interaction signals
    void itemClicked(int index);
    void itemDoubleClicked(int index);
    void itemHovered(int index);
    void swipeDetected(Qt::GestureType gesture, qreal velocity);

    // Additional signals for test compatibility
    void configurationChanged(const FluentCarouselConfig& config);
    void accessibilityUpdateRequested(const QString& announcement);

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void changeEvent(QEvent* event) override;

    // Touch and gesture events
    bool event(QEvent* event) override;
    bool gestureEvent(QGestureEvent* event);
    void touchEvent(QTouchEvent* event);

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

    // Accessibility helpers
    void updateAccessibilityInfo();
    void announceCurrentItem();

private slots:
    void onAutoPlayTimer();
    void onTransitionAnimationValueChanged(const QVariant& value);
    void onTransitionAnimationFinished();
    void onItemClicked();
    void onNavigationClicked(int direction);
    void onIndicatorClicked(int index);
    void onThemeChanged();

private:
    // Initialization
    void initializeComponent();
    void initializeLayout();
    void initializeAnimations();
    void initializeGestures();
    void initializeAccessibility();

    // Layout management
    void updateLayout();
    void updateItemPositions();
    void updateNavigationVisibility();
    void updateIndicatorVisibility();

    // Animation methods
    void startTransition(int fromIndex, int toIndex, bool animated);
    void createTransitionAnimation(int fromIndex, int toIndex);
    void updateTransitionProgress(qreal progress);

    // Gesture handling
    void handlePanGesture(QPanGesture* gesture);
    void handleSwipeGesture(QSwipeGesture* gesture);
    void processTouchPoints(const QList<QTouchEvent::TouchPoint>& touchPoints);

    // Auto-play management
    void updateAutoPlayTimer();
    void resetAutoPlayTimer();

    // Utility methods
    int normalizeIndex(int index) const;
    bool isValidIndex(int index) const;
    QRect getItemRect(int index) const;
    QPoint getItemPosition(int index) const;

private:
    // Configuration
    FluentCarouselConfig m_config;

    // Items and layout
    std::vector<FluentCarouselItemData> m_items;
    QStackedWidget* m_stackedWidget{nullptr};
    QWidget* m_contentWidget{nullptr};
    QHBoxLayout* m_mainLayout{nullptr};

    // Navigation components
    FluentCarouselNavigation* m_navigation{nullptr};
    FluentCarouselIndicator* m_indicator{nullptr};

    // State
    int m_currentIndex{0};
    bool m_transitioning{false};
    qreal m_transitionProgress{0.0};

    // Auto-play
    std::unique_ptr<QTimer> m_autoPlayTimer;
    bool m_autoPlayPaused{false};

    // Animations
    std::unique_ptr<QPropertyAnimation> m_transitionAnimation;
    std::unique_ptr<QSequentialAnimationGroup> m_transitionGroup;

    // Touch/gesture state
    bool m_touchActive{false};
    QPointF m_touchStartPos;
    QPointF m_touchCurrentPos;
    qreal m_touchVelocity{0.0};
    std::chrono::steady_clock::time_point m_touchStartTime;

    // Performance optimization
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
    bool m_layoutUpdateScheduled{false};

    // Repaint coalescing during transitions
    std::unique_ptr<QTimer> m_repaintCoalesceTimer;
    bool m_pendingRepaint{false};
    int m_repaintIntervalMs{16};  // ~60Hz

    // Helpers
    void scheduleCoalescedUpdate();
    void updateMotionPreference();
};

}  // namespace FluentQt::Components
