// include/FluentQt/Components/FluentAutoCarousel.h
#pragma once

#include <QColor>
#include <QLabel>
#include <QPair>
#include <QProgressBar>
#include <QSettings>
#include <QTimer>
#include <chrono>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCarousel.h"

namespace FluentQt::Components {

/**
 * @brief Progress indicator styles for auto carousel following Fluent Design
 */
enum class FluentAutoCarouselProgressStyle {
    Bar,       // Traditional progress bar with Fluent styling
    Circular,  // Circular progress indicator (Fluent ring style)
    Dots,      // Dot-based progress with Fluent animations
    Line,      // Thin line progress with Fluent colors
    Ring,      // Ring-style progress (primary Fluent Design)
    Minimal,   // Minimal line at bottom
    Segmented  // Segmented progress for better accessibility
};

/**
 * @brief Play state for auto carousel (for backward compatibility)
 */
enum class FluentCarouselPlayState {
    Stopped = 0,  // Auto-play is stopped
    Playing = 1,  // Auto-play is active
    Paused = 2    // Auto-play is paused
};

/**
 * @brief Semantic color schemes for enhanced Fluent Design compliance
 */
enum class FluentSemanticColorScheme {
    Auto,         // Automatically follow system theme
    Light,        // Force light theme colors
    Dark,         // Force dark theme colors
    HighContrast  // High contrast mode for accessibility
};

/**
 * @brief Configuration structure for FluentAutoCarousel
 */
struct FluentAutoCarouselConfig {
    // Auto-play settings
    bool autoPlayEnabled{true};
    std::chrono::milliseconds autoPlayInterval{3000};
    FluentCarouselAutoPlay autoPlayDirection{FluentCarouselAutoPlay::Forward};

    // Pause behavior
    bool pauseOnHover{true};
    bool pauseOnFocus{true};

    // UI configuration
    bool showPlayControls{true};
    bool showProgressIndicator{true};
    FluentAutoCarouselProgressStyle progressStyle{
        FluentAutoCarouselProgressStyle::Bar};

    // Enhanced Fluent UI properties
    FluentButtonStyle controlButtonStyle{FluentButtonStyle::Subtle};
    bool elevationEnabled{true};
    int elevationLevel{2};
    int cornerRadius{0};  // 0 = use theme default
    bool fluentMotionEnabled{true};
    bool respectSystemMotionSettings{true};
    FluentSemanticColorScheme semanticColorScheme{
        FluentSemanticColorScheme::Auto};

    // Accessibility
    bool accessibilityAnnouncementsEnabled{true};
    int accessibilityAnnouncementDelay{500};

    // Colors (optional overrides)
    QColor progressActiveColor;
    QColor progressInactiveColor;

    // Animation settings
    bool progressAnimated{true};
    int progressAnimationDuration{0};  // 0 = use Fluent tokens
};

/**
 * @brief Auto-playing carousel component with configurable timing and Fluent UI
 * design
 *
 * FluentAutoCarousel extends FluentCarousel with comprehensive auto-play
 * functionality following Microsoft Fluent Design principles. It provides:
 *
 * **Core Features:**
 * - Configurable auto-play timing (100ms - 60s intervals)
 * - Multiple playback directions (forward, backward, ping-pong)
 * - Play/pause/stop controls with visual feedback
 * - Progress indicator with multiple styles (bar, circular, dots, line)
 * - Pause on hover and focus functionality
 * - Smooth transitions with Fluent Design animations
 * - Auto-play state persistence across show/hide events
 *
 * **Enhanced Fluent UI Integration:**
 * - Semantic color schemes (Auto, Light, Dark, High Contrast)
 * - Fluent Design typography and spacing tokens
 * - Enhanced hover, focus, and pressed state animations with cubic-bezier
 * easing
 * - Configurable elevation effects with shadow levels (1-5)
 * - Responsive design with touch support and proper sizing
 * - Motion preferences respect for accessibility
 * - Performance-optimized styling with intelligent caching
 *
 * **Enhanced Accessibility:**
 * - Comprehensive keyboard navigation (Space, Escape, F1, Arrow keys)
 * - Enhanced screen reader support with detailed ARIA attributes
 * - High contrast mode compatibility with semantic colors
 * - Configurable announcements for state changes with timing control
 * - Advanced focus management and visual indicators
 * - Progress indicators with timing information for screen readers
 * - Keyboard shortcuts with tooltips and ARIA descriptions
 *
 * **Usage Example:**
 * @code
 * // Create auto carousel with 3-second intervals
 * auto* carousel = new FluentAutoCarousel(this);
 * carousel->setAutoPlayInterval(3000);
 * carousel->setPauseOnHover(true);
 * carousel->setShowProgressIndicator(true);
 *
 * // Add content
 * for (const QString& imagePath : imagePaths) {
 *     auto* label = new QLabel();
 *     label->setPixmap(QPixmap(imagePath));
 *     carousel->addItem(label);
 * }
 *
 * // Start auto-play
 * carousel->play();
 * @endcode
 *
 * @see FluentCarousel for base carousel functionality
 * @see FluentCarouselConfig for configuration options
 */
class FluentAutoCarousel : public FluentCarousel {
    Q_OBJECT
    Q_PROPERTY(bool autoPlayEnabled READ isAutoPlayEnabled WRITE
                   setAutoPlayEnabled NOTIFY autoPlayEnabledChanged)
    Q_PROPERTY(int autoPlayInterval READ autoPlayInterval WRITE
                   setAutoPlayInterval NOTIFY autoPlayIntervalChanged)
    Q_PROPERTY(FluentCarouselAutoPlay autoPlayDirection READ autoPlayDirection
                   WRITE setAutoPlayDirection NOTIFY autoPlayDirectionChanged)
    Q_PROPERTY(bool pauseOnHover READ pauseOnHover WRITE setPauseOnHover NOTIFY
                   pauseOnHoverChanged)
    Q_PROPERTY(bool pauseOnFocus READ pauseOnFocus WRITE setPauseOnFocus NOTIFY
                   pauseOnFocusChanged)
    Q_PROPERTY(bool showPlayControls READ showPlayControls WRITE
                   setShowPlayControls NOTIFY playControlsVisibilityChanged)
    Q_PROPERTY(
        bool showProgressIndicator READ showProgressIndicator WRITE
            setShowProgressIndicator NOTIFY progressIndicatorVisibilityChanged)
    Q_PROPERTY(FluentAutoCarouselProgressStyle progressStyle READ progressStyle
                   WRITE setProgressStyle NOTIFY progressStyleChanged)
    Q_PROPERTY(qreal playbackProgress READ playbackProgress NOTIFY
                   playbackProgressChanged)

public:
    /**
     * @brief Constructs a FluentAutoCarousel with default configuration
     * @param parent Parent widget (optional)
     */
    explicit FluentAutoCarousel(QWidget* parent = nullptr);

    /**
     * @brief Constructs a FluentAutoCarousel with custom configuration
     * @param config Carousel configuration settings
     * @param parent Parent widget (optional)
     */
    explicit FluentAutoCarousel(const FluentCarouselConfig& config,
                                QWidget* parent = nullptr);

    /**
     * @brief Constructs a FluentAutoCarousel with enhanced configuration
     * @param config Enhanced auto carousel configuration settings
     * @param parent Parent widget (optional)
     */
    explicit FluentAutoCarousel(const FluentAutoCarouselConfig& config,
                                QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~FluentAutoCarousel() override = default;

    // Auto-play configuration

    /**
     * @brief Gets whether auto-play is enabled
     * @return true if auto-play is enabled, false otherwise
     */
    bool isAutoPlayEnabled() const noexcept { return m_autoPlayEnabled; }

    /**
     * @brief Sets whether auto-play is enabled
     * @param enabled true to enable auto-play, false to disable
     * @note Enabling auto-play will start playback if items are available
     */
    void setAutoPlayEnabled(bool enabled);

    /**
     * @brief Gets the auto-play interval in milliseconds
     * @return Auto-play interval (100-60000ms)
     */
    int autoPlayInterval() const noexcept {
        return static_cast<int>(m_autoPlayInterval.count());
    }

    /**
     * @brief Sets the auto-play interval
     * @param milliseconds Interval between transitions (100-60000ms)
     * @note Values outside the valid range will be clamped
     */
    void setAutoPlayInterval(int milliseconds);

    /**
     * @brief Gets the auto-play direction
     * @return Current auto-play direction
     */
    FluentCarouselAutoPlay autoPlayDirection() const noexcept {
        return m_autoPlayDirection;
    }

    /**
     * @brief Sets the auto-play direction
     * @param direction Direction for auto-play (Forward, Backward, PingPong,
     * None)
     */
    void setAutoPlayDirection(FluentCarouselAutoPlay direction);

    // Pause behavior

    /**
     * @brief Gets whether auto-play pauses on mouse hover
     * @return true if pause on hover is enabled
     */
    bool pauseOnHover() const noexcept { return m_pauseOnHover; }

    /**
     * @brief Sets whether auto-play pauses on mouse hover
     * @param pause true to pause on hover, false otherwise
     */
    void setPauseOnHover(bool pause);

    /**
     * @brief Gets whether auto-play pauses on keyboard focus
     * @return true if pause on focus is enabled
     */
    bool pauseOnFocus() const noexcept { return m_pauseOnFocus; }

    /**
     * @brief Sets whether auto-play pauses on keyboard focus
     * @param pause true to pause on focus, false otherwise
     */
    void setPauseOnFocus(bool pause);

    /**
     * @brief Gets whether auto-play pauses on user interaction
     * @return true if pause on interaction is enabled
     */
    bool pauseOnInteraction() const noexcept { return m_pauseOnInteraction; }

    /**
     * @brief Sets pause behavior on user interaction with enhanced UX
     * @param pause True to pause auto-play during user interactions
     * @note Includes touch, mouse, and keyboard interactions for better UX
     */
    void setPauseOnInteraction(bool pause);

    /**
     * @brief Gets whether smart pause is enabled
     * @return true if smart pause is enabled
     */
    bool smartPauseEnabled() const noexcept { return m_smartPauseEnabled; }

    /**
     * @brief Configures smart pause behavior based on content visibility
     * @param enabled True to automatically pause when content is not visible
     * @note Uses intersection observer pattern for optimal performance
     */
    void setSmartPauseEnabled(bool enabled);

    // UI controls

    /**
     * @brief Gets whether play controls are visible
     * @return true if play controls are shown
     */
    bool showPlayControls() const noexcept { return m_showPlayControls; }

    /**
     * @brief Sets whether play controls are visible
     * @param show true to show play controls, false to hide
     */
    void setShowPlayControls(bool show);

    /**
     * @brief Gets whether progress indicator is visible
     * @return true if progress indicator is shown
     */
    bool showProgressIndicator() const noexcept {
        return m_showProgressIndicator;
    }

    /**
     * @brief Sets whether progress indicator is visible
     * @param show true to show progress indicator, false to hide
     */
    void setShowProgressIndicator(bool show);

    /**
     * @brief Gets the progress indicator style
     * @return Current progress indicator style
     */
    FluentAutoCarouselProgressStyle progressStyle() const noexcept {
        return m_progressStyle;
    }

    /**
     * @brief Sets the progress indicator style
     * @param style Progress indicator style (Bar, Circular, Dots, Line, Ring,
     * Minimal)
     */
    void setProgressStyle(FluentAutoCarouselProgressStyle style);

    // Enhanced Fluent UI styling methods

    /**
     * @brief Sets custom progress indicator colors using Fluent Design tokens
     * @param activeColor Color for active progress (defaults to accent color)
     * @param inactiveColor Color for inactive progress (defaults to neutral)
     */
    void setProgressColors(const QColor& activeColor = QColor(),
                           const QColor& inactiveColor = QColor());

    /**
     * @brief Sets the progress indicator animation style
     * @param animated Whether progress changes should be animated
     * @param duration Animation duration in milliseconds (uses Fluent tokens if
     * 0)
     */
    void setProgressAnimation(bool animated, int duration = 0);

    /**
     * @brief Sets the control button style using Fluent Design principles
     * @param style Button style (Subtle, Standard, Accent)
     */
    void setControlButtonStyle(FluentButtonStyle style);

    /**
     * @brief Enables or disables Fluent Design elevation effects
     * @param enabled Whether to show elevation shadows
     * @param level Elevation level (1-5, uses Fluent elevation tokens)
     */
    void setElevationEnabled(bool enabled, int level = 2);

    /**
     * @brief Sets the corner radius using Fluent Design tokens
     * @param radius Corner radius in pixels (0 = use theme default)
     */
    void setCornerRadius(int radius = 0);

    /**
     * @brief Enables or disables Fluent Design motion effects
     * @param enabled Whether to use enhanced motion animations
     * @param respectSystemSettings Whether to respect system motion preferences
     */
    void setFluentMotionEnabled(bool enabled,
                                bool respectSystemSettings = true);

    /**
     * @brief Sets the semantic color scheme for the component
     * @param scheme Color scheme (Auto, Light, Dark, HighContrast)
     */
    void setSemanticColorScheme(FluentSemanticColorScheme scheme);

    /**
     * @brief Configures accessibility enhancements
     * @param enableAnnouncements Whether to announce state changes
     * @param customAnnouncementDelay Delay between announcements in ms
     */
    void setAccessibilityEnhancements(bool enableAnnouncements,
                                      int customAnnouncementDelay = 500);

    /**
     * @brief Applies a complete configuration to the carousel
     * @param config Configuration object with all settings
     */
    void applyConfiguration(const FluentAutoCarouselConfig& config);

    /**
     * @brief Gets the current complete configuration
     * @return Current configuration object
     */
    FluentAutoCarouselConfig currentConfiguration() const;

    /**
     * @brief Saves current configuration to QSettings
     * @param settings QSettings object to save to
     * @param group Settings group name (optional)
     */
    void saveConfiguration(QSettings& settings,
                           const QString& group = QString()) const;

    /**
     * @brief Loads configuration from QSettings
     * @param settings QSettings object to load from
     * @param group Settings group name (optional)
     */
    void loadConfiguration(const QSettings& settings,
                           const QString& group = QString());

    // State information

    /**
     * @brief Gets whether auto-play is currently active
     * @return true if playing, false if stopped
     */
    bool isPlaying() const noexcept { return m_isPlaying; }

    /**
     * @brief Gets whether auto-play is currently paused
     * @return true if paused, false otherwise
     */
    bool isPaused() const noexcept { return m_isPaused; }

    /**
     * @brief Gets the current playback progress
     * @return Progress value between 0.0 and 1.0
     */
    qreal playbackProgress() const noexcept { return m_playbackProgress; }

    // Control access for customization

    /**
     * @brief Gets the play/pause button for customization
     * @return Pointer to the play/pause button, or nullptr if not created
     */
    FluentButton* playPauseButton() const noexcept { return m_playPauseButton; }

    /**
     * @brief Gets the stop button for customization
     * @return Pointer to the stop button, or nullptr if not created
     */
    FluentButton* stopButton() const noexcept { return m_stopButton; }

    /**
     * @brief Gets the progress indicator for customization
     * @return Pointer to the progress indicator, or nullptr if not created
     */
    QProgressBar* progressIndicator() const noexcept {
        return m_progressIndicator;
    }

    // Enhanced control and state methods

    /**
     * @brief Gets the current progress colors
     * @return Pair of active and inactive colors
     */
    QPair<QColor, QColor> progressColors() const noexcept;

    /**
     * @brief Gets whether progress animation is enabled
     * @return true if progress changes are animated
     */
    bool isProgressAnimated() const noexcept { return m_progressAnimated; }

    /**
     * @brief Gets the progress animation duration
     * @return Animation duration in milliseconds (0 = uses Fluent tokens)
     */
    int progressAnimationDuration() const noexcept {
        return m_progressAnimationDuration;
    }

    /**
     * @brief Gets the current control button style
     * @return Current button style
     */
    FluentButtonStyle controlButtonStyle() const noexcept {
        return m_controlButtonStyle;
    }

    /**
     * @brief Gets whether elevation effects are enabled
     * @return true if elevation is enabled
     */
    bool isElevationEnabled() const noexcept { return m_elevationEnabled; }

    /**
     * @brief Gets the current elevation level
     * @return Elevation level (1-5)
     */
    int elevationLevel() const noexcept { return m_elevationLevel; }

    /**
     * @brief Gets the current corner radius
     * @return Corner radius in pixels (0 = using theme default)
     */
    int cornerRadius() const noexcept { return m_cornerRadius; }

    /**
     * @brief Gets whether Fluent motion effects are enabled
     * @return true if enhanced motion animations are enabled
     */
    bool isFluentMotionEnabled() const noexcept {
        return m_fluentMotionEnabled;
    }

    /**
     * @brief Gets whether system motion settings are respected
     * @return true if system motion preferences are respected
     */
    bool respectsSystemMotionSettings() const noexcept {
        return m_respectSystemMotionSettings;
    }

    /**
     * @brief Gets the current semantic color scheme
     * @return Current color scheme setting
     */
    FluentSemanticColorScheme semanticColorScheme() const noexcept {
        return m_semanticColorScheme;
    }

    /**
     * @brief Gets whether accessibility announcements are enabled
     * @return true if state change announcements are enabled
     */
    bool accessibilityAnnouncementsEnabled() const noexcept {
        return m_accessibilityAnnouncementsEnabled;
    }

    /**
     * @brief Gets the custom announcement delay
     * @return Delay between announcements in milliseconds
     */
    int accessibilityAnnouncementDelay() const noexcept {
        return m_accessibilityAnnouncementDelay;
    }

    /**
     * @brief Invalidates cached size hint for performance optimization
     */
    void invalidateSizeHint() const noexcept { m_sizeHintValid = false; }

    // Overridden size methods for enhanced layout calculation
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Convenience factory methods

    /**
     * @brief Creates an auto carousel with specified interval
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance
     */
    static FluentAutoCarousel* createWithInterval(int milliseconds,
                                                  QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel with ping-pong playback
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance
     */
    static FluentAutoCarousel* createPingPong(int milliseconds,
                                              QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel with progress indicator
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance
     */
    static FluentAutoCarousel* createWithProgress(int milliseconds,
                                                  QWidget* parent = nullptr);

    /**
     * @brief Creates a test-friendly auto carousel without UI initialization
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance for testing
     * @note This constructor skips UI component creation for testing purposes
     */
    static FluentAutoCarousel* createForTesting(QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel with circular progress indicator
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance with circular progress
     */
    static FluentAutoCarousel* createWithCircularProgress(
        int milliseconds, QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel with dot-based progress indicator
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance with dot progress
     */
    static FluentAutoCarousel* createWithDotProgress(int milliseconds,
                                                     QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel with minimal line progress indicator
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance with minimal progress
     */
    static FluentAutoCarousel* createWithMinimalProgress(
        int milliseconds, QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel optimized for accessibility
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance with enhanced accessibility
     */
    static FluentAutoCarousel* createAccessible(int milliseconds,
                                                QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel with custom styling options
     * @param milliseconds Auto-play interval in milliseconds
     * @param progressStyle Progress indicator style
     * @param buttonStyle Control button style
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance with custom styling
     */
    static FluentAutoCarousel* createCustomStyled(
        int milliseconds, FluentAutoCarouselProgressStyle progressStyle,
        FluentButtonStyle buttonStyle, QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel optimized for touch devices
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance optimized for touch
     */
    static FluentAutoCarousel* createTouchOptimized(int milliseconds,
                                                    QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel with high contrast accessibility
     * @param milliseconds Auto-play interval in milliseconds
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance with high contrast styling
     */
    static FluentAutoCarousel* createHighContrast(int milliseconds,
                                                  QWidget* parent = nullptr);

    /**
     * @brief Creates an auto carousel from a configuration object
     * @param config Complete configuration for the carousel
     * @param parent Parent widget (optional)
     * @return New FluentAutoCarousel instance with specified configuration
     */
    static FluentAutoCarousel* createFromConfig(
        const FluentAutoCarouselConfig& config, QWidget* parent = nullptr);

    // Backward compatibility methods for existing tests and code

    /**
     * @brief Legacy method: Gets current slide index
     * @deprecated Use currentIndex() instead
     * @return Current slide index
     */
    int currentSlide() const noexcept { return currentIndex(); }

    /**
     * @brief Legacy method: Gets slide count
     * @deprecated Use itemCount() instead
     * @return Number of slides
     */
    int slideCount() const noexcept { return itemCount(); }

    /**
     * @brief Legacy method: Adds a slide
     * @deprecated Use addItem() instead
     * @param widget Widget to add as slide
     */
    void addSlide(QWidget* widget) { addItem(widget); }

    /**
     * @brief Legacy method: Removes a slide
     * @deprecated Use removeItem() instead
     * @param index Index of slide to remove
     */
    void removeSlide(int index) { removeItem(index); }

    /**
     * @brief Legacy method: Goes to specific slide
     * @deprecated Use setCurrentIndex() instead
     * @param index Target slide index
     */
    void goToSlide(int index) { setCurrentIndex(index); }

    /**
     * @brief Legacy method: Goes to next slide
     * @deprecated Use goToNext() instead
     */
    void next() { goToNext(); }

    /**
     * @brief Legacy method: Goes to previous slide
     * @deprecated Use goToPrevious() instead
     */
    void previous() { goToPrevious(); }

    /**
     * @brief Legacy method: Gets auto-play state
     * @deprecated Use isAutoPlayEnabled() instead
     * @return true if auto-play is enabled
     */
    bool isAutoPlay() const noexcept { return isAutoPlayEnabled(); }

    /**
     * @brief Legacy method: Sets auto-play state
     * @deprecated Use setAutoPlayEnabled() instead
     * @param enabled true to enable auto-play
     */
    void setAutoPlay(bool enabled) { setAutoPlayEnabled(enabled); }

    /**
     * @brief Legacy method: Gets auto-play interval
     * @deprecated Use autoPlayInterval() instead
     * @return Auto-play interval in milliseconds
     */
    int interval() const noexcept { return autoPlayInterval(); }

    /**
     * @brief Legacy method: Sets auto-play interval
     * @deprecated Use setAutoPlayInterval() instead
     * @param milliseconds Auto-play interval
     */
    void setInterval(int milliseconds) { setAutoPlayInterval(milliseconds); }

    /**
     * @brief Legacy method: Gets looping state
     * @deprecated Use the base carousel's infinite property instead
     * @return true if looping is enabled
     */
    bool isLooping() const noexcept;

    /**
     * @brief Legacy method: Sets looping state
     * @deprecated Use the base carousel's infinite property instead
     * @param enabled true to enable looping
     */
    void setLooping(bool enabled);

    /**
     * @brief Legacy method: Gets play state
     * @deprecated Use isPlaying() and isPaused() instead
     * @return Current play state
     */
    FluentCarouselPlayState playState() const noexcept;

public slots:
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    void restart();
    void resetProgress();

    // Enhanced control methods
    void playWithDelay(int delayMs);
    void pauseTemporarily(int durationMs);
    void skipToNext();
    void skipToPrevious();
    void setPlaybackSpeed(qreal multiplier);
    void fadeIn(int durationMs = 300);
    void fadeOut(int durationMs = 300);

signals:
    void autoPlayEnabledChanged(bool enabled);
    void autoPlayIntervalChanged(int interval);
    void autoPlayDirectionChanged(FluentCarouselAutoPlay direction);
    void pauseOnHoverChanged(bool pause);
    void pauseOnFocusChanged(bool pause);
    void pauseOnInteractionChanged(bool pause);
    void smartPauseChanged(bool enabled);
    void playControlsVisibilityChanged(bool visible);
    void progressIndicatorVisibilityChanged(bool visible);
    void progressStyleChanged(FluentAutoCarouselProgressStyle style);
    void playbackProgressChanged(qreal progress);
    void playbackStarted();
    void playbackPaused();
    void playbackStopped();
    void playbackRestarted();
    void intervalCompleted();
    void progressChanged(qreal progress);

    // Enhanced Fluent UI signals
    void progressColorsChanged(const QColor& activeColor,
                               const QColor& inactiveColor);
    void progressAnimationChanged(bool animated, int duration);
    void controlButtonStyleChanged(FluentButtonStyle style);
    void elevationChanged(bool enabled, int level);
    void accessibilityUpdateRequested(const QString& announcement);
    void cornerRadiusChanged(int radius);
    void fluentMotionChanged(bool enabled, bool respectSystemSettings);
    void semanticColorSchemeChanged(FluentSemanticColorScheme scheme);
    void accessibilityEnhancementsChanged(bool enabled, int delay);

    // Enhanced control signals
    void playbackSpeedChanged(qreal multiplier);
    void skipRequested(int direction);  // -1 for previous, 1 for next
    void fadeStarted(bool fadeIn, int duration);
    void fadeCompleted(bool fadeIn);
    void temporaryPauseStarted(int duration);
    void temporaryPauseEnded();

    // Legacy signals for backward compatibility
    void autoPlayChanged(bool enabled);
    void intervalChanged(int interval);
    void currentSlideChanged(int index);
    void slideCountChanged(int count);
    void playStateChanged(FluentCarouselPlayState state);
    void loopingChanged(bool enabled);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private slots:
    void onAutoPlayTimer();
    void onProgressTimer();
    void onPlayPauseButtonClicked();
    void onStopButtonClicked();
    void onCurrentIndexChanged(int index);
    void onTransitionFinished(int index);

private:
    void initializeAutoCarousel();
    void createPlayControls();
    void createProgressIndicator();
    void updateControlsLayout();
    void updateContainerStyling();
    void updatePlayPauseButton();
    void updateProgressIndicator();
    void updateProgressIndicatorStyle();
    void updateProgressValue();
    void updateTimeLabel();
    int remainingTime() const;
    void updateAutoPlayTimer();
    void handlePauseOnHover();
    void handlePauseOnFocus();
    void calculateNextIndex();
    void updateAccessibilityInfo();
    void saveAutoPlayState();
    void restoreAutoPlayState();

    // Enhanced Fluent UI helper methods
    void updateControlButtonStyling();
    void applyElevationEffect(QWidget* widget, int level);
    void updateAnimationSettings();
    void updateProgressIndicatorAnimations(bool useReducedMotion);
    void updateControlButtonAnimations(bool useReducedMotion);
    QColor getSemanticColor(const QString& colorRole) const;
    int getEffectiveCornerRadius() const;
    void applyConfigurationInternal(const FluentAutoCarouselConfig& config);

    // Enhanced interaction handling
    void handlePauseOnInteraction();
    void handleSmartPause();
    void updateInteractionState();

    // Enhanced Fluent Design compliance methods
    void applyFluentTypography();
    void applyFluentSpacing();
    void applyFluentMotion();
    void applyFluentColors();
    void updateFluentThemeCompliance();

    // Performance optimization methods
    void optimizeUpdates();
    void deferredUpdate();

    // Accessibility helper methods for better code organization
    QString buildStatusText() const;
    QString buildTimingText() const;
    QString buildControlsText() const;
    void updateButtonAccessibility();
    void updateProgressAccessibility();

private:
    // Auto-play configuration
    bool m_autoPlayEnabled{true};
    std::chrono::milliseconds m_autoPlayInterval{3000};
    FluentCarouselAutoPlay m_autoPlayDirection{FluentCarouselAutoPlay::Forward};

    // Pause behavior
    bool m_pauseOnHover{true};
    bool m_pauseOnFocus{true};
    bool m_pauseOnInteraction{false};
    bool m_smartPauseEnabled{false};

    // UI configuration
    bool m_showPlayControls{true};
    bool m_showProgressIndicator{true};
    FluentAutoCarouselProgressStyle m_progressStyle{
        FluentAutoCarouselProgressStyle::Bar};

    // State
    bool m_isPlaying{false};
    bool m_isPaused{false};
    bool m_pausedByHover{false};
    bool m_pausedByFocus{false};
    qreal m_playbackProgress{0.0};
    qreal m_currentProgress{0.0};
    int m_nextIndex{0};
    bool m_pingPongForward{true};

    // Timers
    std::unique_ptr<QTimer> m_autoPlayTimer;
    std::unique_ptr<QTimer> m_progressTimer;
    std::chrono::steady_clock::time_point m_intervalStartTime;
    std::chrono::steady_clock::time_point m_progressStartTime;

    // UI controls
    FluentButton* m_playPauseButton{nullptr};
    FluentButton* m_stopButton{nullptr};
    QProgressBar* m_progressIndicator{nullptr};
    QProgressBar* m_progressBar{nullptr};
    QLabel* m_timeLabel{nullptr};
    QWidget* m_controlsContainer{nullptr};
    QWidget* m_progressContainer{nullptr};
    QHBoxLayout* m_controlsLayout{nullptr};

    // Icons
    QIcon m_playIcon;
    QIcon m_pauseIcon;
    QIcon m_stopIcon;

    // Enhanced Fluent UI properties
    QColor m_progressActiveColor;
    QColor m_progressInactiveColor;
    bool m_progressAnimated{true};
    int m_progressAnimationDuration{0};  // 0 = use Fluent tokens
    FluentButtonStyle m_controlButtonStyle{FluentButtonStyle::Subtle};
    bool m_elevationEnabled{true};
    int m_elevationLevel{2};
    int m_cornerRadius{0};  // 0 = use theme default
    bool m_fluentMotionEnabled{true};
    bool m_respectSystemMotionSettings{true};
    FluentSemanticColorScheme m_semanticColorScheme{
        FluentSemanticColorScheme::Auto};
    bool m_accessibilityAnnouncementsEnabled{true};
    int m_accessibilityAnnouncementDelay{500};

    // Performance optimization
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};

    // Enhanced control properties
    qreal m_playbackSpeed{1.0};
    std::unique_ptr<QTimer> m_delayTimer;
    std::unique_ptr<QTimer> m_temporaryPauseTimer;
    std::unique_ptr<QPropertyAnimation> m_fadeAnimation;
    bool m_isFading{false};
    bool m_fadeDirection{true};  // true for fade in, false for fade out
};

}  // namespace FluentQt::Components
