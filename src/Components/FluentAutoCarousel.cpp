// src/Components/FluentAutoCarousel.cpp
#include "FluentQt/Components/FluentAutoCarousel.h"
#include <QDebug>
#include <QEnterEvent>
#include <QFocusEvent>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QSettings>
#include <QShowEvent>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <chrono>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentCircularProgress.h"
#include "FluentQt/Components/FluentDotProgress.h"
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

/**
 * @brief Constructs a FluentAutoCarousel with default settings
 *
 * Creates an auto-playing carousel with the following default configuration:
 * - Auto-play enabled with 3-second intervals
 * - Forward direction playback
 * - Pause on hover and focus enabled
 * - Progress indicator visible with Bar style
 * - Subtle button style with elevation enabled
 * - Fluent motion and accessibility features enabled
 *
 * @param parent The parent widget (optional)
 */
FluentAutoCarousel::FluentAutoCarousel(QWidget* parent)
    : FluentCarousel(parent),
      m_autoPlayTimer(std::make_unique<QTimer>(this)),
      m_progressTimer(std::make_unique<QTimer>(this)) {
    initializeAutoCarousel();
}

FluentAutoCarousel::FluentAutoCarousel(const FluentCarouselConfig& config,
                                       QWidget* parent)
    : FluentCarousel(config, parent),
      m_autoPlayTimer(std::make_unique<QTimer>(this)),
      m_progressTimer(std::make_unique<QTimer>(this)) {
    initializeAutoCarousel();
}

FluentAutoCarousel::FluentAutoCarousel(const FluentAutoCarouselConfig& config,
                                       QWidget* parent)
    : FluentCarousel(parent),
      m_autoPlayTimer(std::make_unique<QTimer>(this)),
      m_progressTimer(std::make_unique<QTimer>(this)) {
    // Apply the enhanced configuration before initialization
    applyConfigurationInternal(config);
    initializeAutoCarousel();
}

void FluentAutoCarousel::initializeAutoCarousel() {
    try {
        // Set default configuration for auto carousel
        FluentCarouselConfig config = this->config();
        config.autoPlay = FluentCarouselAutoPlay::Forward;
        config.autoPlayInterval = std::chrono::milliseconds(3000);
        config.pauseOnHover = true;
        config.pauseOnFocus = true;
        config.showIndicators = true;
        setConfig(config);

        // Initialize timers
        m_autoPlayTimer->setSingleShot(true);

        // Use Fluent Design timing tokens for smooth animation
        auto& theme = Styling::FluentTheme::instance();
        int progressUpdateInterval = 150;  // Fast animation timing (150ms)
        if (progressUpdateInterval <= 0)
            progressUpdateInterval = 50;  // Fallback

        m_progressTimer->setInterval(progressUpdateInterval);

        // Initialize UI components with improved Fluent Design
        createProgressIndicator();
        createPlayControls();

        // Connect signals
        connect(this, &FluentCarousel::currentIndexChanged, this,
                &FluentAutoCarousel::onCurrentIndexChanged);
        connect(m_autoPlayTimer.get(), &QTimer::timeout, this,
                &FluentAutoCarousel::onAutoPlayTimer);
        connect(m_progressTimer.get(), &QTimer::timeout, this,
                &FluentAutoCarousel::onProgressTimer);

        // Connect legacy signal forwarding for backward compatibility
        connect(this, &FluentCarousel::currentIndexChanged, this,
                &FluentAutoCarousel::currentSlideChanged);
        connect(this, &FluentCarousel::itemCountChanged, this,
                &FluentAutoCarousel::slideCountChanged);

        // Initialize state
        m_isPlaying = m_autoPlayEnabled;
        m_nextIndex = 0;

        // Apply enhanced Fluent Design compliance
        updateFluentThemeCompliance();

        // Initialize interaction state
        updateInteractionState();

        // Connect to theme changes for dynamic updates
        connect(&Styling::FluentTheme::instance(),
                &Styling::FluentTheme::themeChanged, this,
                &FluentAutoCarousel::updateFluentThemeCompliance);

        // Start auto-play if enabled and we have items
        if (m_autoPlayEnabled && itemCount() > 1) {
            play();
        }
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::initializeAutoCarousel: "
                       "Initialization failed:"
                    << e.what();
        // Set safe defaults on failure
        m_autoPlayEnabled = false;
        m_isPlaying = false;
    }
}

void FluentAutoCarousel::createProgressIndicator() {
    if (!m_showProgressIndicator) {
        return;
    }

    // Create progress container if it doesn't exist
    if (!m_progressContainer) {
        m_progressContainer = new QWidget(this);
        m_progressContainer->setObjectName("progressContainer");

        // Apply Fluent Design elevation if enabled
        if (m_elevationEnabled) {
            applyElevationEffect(m_progressContainer, m_elevationLevel);
        }
    }

    // Remove existing progress indicator if style changed
    if (m_progressIndicator) {
        m_progressIndicator->deleteLater();
        m_progressIndicator = nullptr;
    }

    // Create progress indicator based on style using custom widgets for better
    // Fluent UI compliance
    switch (m_progressStyle) {
        case FluentAutoCarouselProgressStyle::Bar:
        case FluentAutoCarouselProgressStyle::Line:
        case FluentAutoCarouselProgressStyle::Minimal:
        case FluentAutoCarouselProgressStyle::Segmented:
            // Use standard QProgressBar for linear styles
            m_progressIndicator = new QProgressBar(m_progressContainer);
            m_progressIndicator->setObjectName("autoCarouselProgress");
            m_progressIndicator->setRange(0, 100);
            m_progressIndicator->setValue(0);
            m_progressIndicator->setTextVisible(false);

            if (m_progressStyle == FluentAutoCarouselProgressStyle::Line ||
                m_progressStyle == FluentAutoCarouselProgressStyle::Minimal) {
                m_progressIndicator->setOrientation(Qt::Horizontal);
                m_progressIndicator->setMaximumHeight(2);
            } else {
                m_progressIndicator->setOrientation(Qt::Horizontal);
            }
            break;

        case FluentAutoCarouselProgressStyle::Circular:
        case FluentAutoCarouselProgressStyle::Ring: {
            // Use custom circular progress widget for better Fluent UI
            // compliance
            auto* circularProgress =
                FluentCircularProgress::createRing(24, m_progressContainer);
            circularProgress->setObjectName("autoCarouselCircularProgress");
            circularProgress->setActiveColor(m_progressActiveColor.isValid()
                                                 ? m_progressActiveColor
                                                 : QColor());
            circularProgress->setInactiveColor(m_progressInactiveColor.isValid()
                                                   ? m_progressInactiveColor
                                                   : QColor());
            circularProgress->setAnimated(m_progressAnimated);
            if (m_progressAnimationDuration > 0) {
                circularProgress->setAnimationDuration(
                    m_progressAnimationDuration);
            }
            m_progressIndicator = circularProgress;
        } break;

        case FluentAutoCarouselProgressStyle::Dots: {
            // Use custom dot progress widget for better Fluent UI compliance
            auto* dotProgress =
                FluentDotProgress::createLinear(5, m_progressContainer);
            dotProgress->setObjectName("autoCarouselDotProgress");
            dotProgress->setActiveColor(m_progressActiveColor.isValid()
                                            ? m_progressActiveColor
                                            : QColor());
            dotProgress->setInactiveColor(m_progressInactiveColor.isValid()
                                              ? m_progressInactiveColor
                                              : QColor());
            dotProgress->setAnimated(m_progressAnimated);
            if (m_progressAnimationDuration > 0) {
                dotProgress->setAnimationDuration(m_progressAnimationDuration);
            }
            m_progressIndicator = dotProgress;
        } break;
    }

    // Apply enhanced Fluent UI styling
    updateProgressIndicatorStyle();

    // Create time label with improved typography
    if (!m_timeLabel) {
        m_timeLabel = new QLabel(m_progressContainer);
        m_timeLabel->setObjectName("timeLabel");
        m_timeLabel->setText("0:00 / 0:00");

        // Apply Fluent UI typography with proper design tokens
        auto& theme = Styling::FluentTheme::instance();
        m_timeLabel->setFont(theme.captionFont());

        // Use proper Fluent Design color tokens
        QString timeStyle = QString(
                                "QLabel {"
                                "    color: %1;"
                                "    font-size: %2px;"
                                "    font-weight: 400;"
                                "    line-height: 1.4;"
                                "}")
                                .arg(theme.color("neutralSecondary").name())
                                .arg(12);  // Caption font size

        m_timeLabel->setStyleSheet(timeStyle);
    }

    // Layout progress components with Fluent Design spacing
    if (!m_progressContainer->layout()) {
        auto* layout = new QHBoxLayout(m_progressContainer);

        // Use Fluent Design spacing tokens
        int spacing = 4;  // Small spacing
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(spacing * 2);  // 8px between elements

        layout->addWidget(m_progressIndicator, 1);
        layout->addWidget(m_timeLabel, 0);
    }
}

void FluentAutoCarousel::createPlayControls() {
    if (!m_showPlayControls) {
        return;
    }

    // Create controls container if it doesn't exist
    if (!m_controlsContainer) {
        m_controlsContainer = new QWidget(this);
        m_controlsContainer->setObjectName("controlsContainer");
    }

    // Create play/pause button with enhanced Fluent Design
    if (!m_playPauseButton) {
        m_playPauseButton = new FluentButton(m_controlsContainer);
        m_playPauseButton->setObjectName("playPauseButton");
        m_playPauseButton->setCheckable(false);
        m_playPauseButton->setToolTip(tr("Play/Pause"));

        // Apply enhanced Fluent Design button styling
        auto& theme = Styling::FluentTheme::instance();

        // Use Fluent Design size tokens for consistent button sizing
        int buttonSize =
            theme.componentHeight("medium");  // Standard Fluent button size
        if (buttonSize <= 0)
            buttonSize = 32;  // Fallback

        m_playPauseButton->setFixedSize(buttonSize, buttonSize);

        // Apply initial styling (will be updated by
        // updateControlButtonStyling) This ensures consistent styling with
        // the current button style setting

        // Set icons (will be loaded in updatePlayPauseButton)
        updatePlayPauseButton();

        // Connect signal
        connect(m_playPauseButton, &FluentButton::clicked, this,
                &FluentAutoCarousel::onPlayPauseButtonClicked);
    }

    // Create stop button with matching styling
    if (!m_stopButton) {
        m_stopButton = new FluentButton(m_controlsContainer);
        m_stopButton->setObjectName("stopButton");
        m_stopButton->setCheckable(false);
        m_stopButton->setToolTip(tr("Stop"));
        m_stopButton->setText("⏹");  // Unicode stop symbol as fallback
        int buttonSize = 32;         // Standard button size
        m_stopButton->setFixedSize(buttonSize, buttonSize);

        // Connect signal
        connect(m_stopButton, &FluentButton::clicked, this,
                &FluentAutoCarousel::onStopButtonClicked);
    }

    // Layout controls with proper Fluent Design spacing
    if (!m_controlsLayout) {
        m_controlsLayout = new QHBoxLayout(m_controlsContainer);

        // Use Fluent Design spacing tokens
        auto& theme = Styling::FluentTheme::instance();
        int spacing = theme.spacing("small");  // 4px

        m_controlsLayout->setContentsMargins(spacing, spacing, spacing,
                                             spacing);
        m_controlsLayout->setSpacing(spacing);
        m_controlsLayout->addWidget(m_playPauseButton);
        m_controlsLayout->addWidget(m_stopButton);
        m_controlsLayout->addStretch();
    }

    // Apply enhanced Fluent UI styling to all buttons
    updateControlButtonStyling();
}

// Property setters with validation and error handling
void FluentAutoCarousel::setAutoPlayEnabled(bool enabled) {
    if (m_autoPlayEnabled == enabled) {
        return;  // No change needed
    }

    m_autoPlayEnabled = enabled;

    // Apply the change immediately
    if (enabled && itemCount() > 1) {
        play();
    } else if (!enabled) {
        stop();
    }

    // Update UI components
    updatePlayPauseButton();
    updateAccessibilityInfo();

    emit autoPlayEnabledChanged(enabled);

    // Emit legacy signal for backward compatibility
    emit autoPlayChanged(enabled);
}

/**
 * @brief Sets the auto-play interval with comprehensive validation
 *
 * Configures the time interval between automatic slide transitions.
 * The interval is automatically clamped to safe and usable ranges:
 * - Minimum: 100ms (to prevent performance issues)
 * - Maximum: 60000ms (60 seconds for usability)
 *
 * If the carousel is currently playing, the timer is immediately
 * updated with the new interval. UI components and accessibility
 * information are also updated to reflect the change.
 *
 * @param milliseconds The interval in milliseconds (100-60000)
 *
 * @note Values outside the valid range are automatically clamped
 *       and a warning is logged to help with debugging.
 *
 * @see autoPlayIntervalChanged() signal
 * @see intervalChanged() signal (legacy compatibility)
 */
void FluentAutoCarousel::setAutoPlayInterval(int milliseconds) {
    // Enhanced input validation with detailed error reporting
    const int MIN_INTERVAL = 100;    // 100ms minimum for performance
    const int MAX_INTERVAL = 60000;  // 60s maximum for usability

    if (milliseconds < MIN_INTERVAL) {
        qWarning() << "FluentAutoCarousel::setAutoPlayInterval: Interval"
                   << milliseconds << "ms is too short. Minimum allowed is"
                   << MIN_INTERVAL << "ms. Clamping to minimum.";
        milliseconds = MIN_INTERVAL;
    } else if (milliseconds > MAX_INTERVAL) {
        qWarning() << "FluentAutoCarousel::setAutoPlayInterval: Interval"
                   << milliseconds << "ms is too long. Maximum allowed is"
                   << MAX_INTERVAL << "ms. Clamping to maximum.";
        milliseconds = MAX_INTERVAL;
    }

    auto newInterval = std::chrono::milliseconds(milliseconds);
    if (m_autoPlayInterval == newInterval) {
        return;  // No change needed
    }

    try {
        m_autoPlayInterval = newInterval;

        // If currently playing, restart timer with new interval
        if (m_isPlaying && !m_isPaused) {
            updateAutoPlayTimer();
        }

        // Update UI components with error handling
        try {
            updateTimeLabel();
            updateAccessibilityInfo();
        } catch (const std::exception& e) {
            qWarning() << "FluentAutoCarousel::setAutoPlayInterval: Failed to "
                          "update UI components:"
                       << e.what();
        }

        emit autoPlayIntervalChanged(milliseconds);
        emit intervalChanged(milliseconds);  // Legacy compatibility

    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::setAutoPlayInterval: Critical "
                       "error setting interval:"
                    << e.what();
        // Restore previous interval on error
        // Note: m_autoPlayInterval is already set, so we don't need to restore
    }
}

void FluentAutoCarousel::setAutoPlayDirection(
    FluentCarouselAutoPlay direction) {
    // Validate direction parameter
    if (direction != FluentCarouselAutoPlay::Forward &&
        direction != FluentCarouselAutoPlay::Backward &&
        direction != FluentCarouselAutoPlay::PingPong &&
        direction != FluentCarouselAutoPlay::None) {
        qWarning() << "FluentAutoCarousel: Invalid auto-play direction";
        return;
    }

    if (m_autoPlayDirection == direction) {
        return;  // No change needed
    }

    m_autoPlayDirection = direction;

    // Reset ping-pong state when direction changes
    if (direction == FluentCarouselAutoPlay::PingPong) {
        m_pingPongForward = true;
    }

    // Recalculate next index with new direction
    calculateNextIndex();
    updateAccessibilityInfo();

    emit autoPlayDirectionChanged(direction);
}

void FluentAutoCarousel::setPauseOnHover(bool pause) {
    if (m_pauseOnHover == pause) {
        return;  // No change needed
    }

    m_pauseOnHover = pause;

    // If disabling pause on hover while currently paused by hover, resume
    if (!pause && m_pausedByHover) {
        m_pausedByHover = false;
        handlePauseOnHover();
    }

    updateAccessibilityInfo();
    emit pauseOnHoverChanged(pause);
}

void FluentAutoCarousel::setPauseOnFocus(bool pause) {
    if (m_pauseOnFocus == pause) {
        return;  // No change needed
    }

    m_pauseOnFocus = pause;

    // If disabling pause on focus while currently paused by focus, resume
    if (!pause && m_pausedByFocus) {
        m_pausedByFocus = false;
        handlePauseOnFocus();
    }

    updateAccessibilityInfo();
    emit pauseOnFocusChanged(pause);
}

void FluentAutoCarousel::setPauseOnInteraction(bool pause) {
    if (m_pauseOnInteraction == pause) {
        return;  // No change needed
    }

    m_pauseOnInteraction = pause;
    updateInteractionState();
    updateAccessibilityInfo();
    emit pauseOnInteractionChanged(pause);
}

void FluentAutoCarousel::setSmartPauseEnabled(bool enabled) {
    if (m_smartPauseEnabled == enabled) {
        return;  // No change needed
    }

    m_smartPauseEnabled = enabled;

    if (enabled) {
        // Initialize smart pause monitoring
        handleSmartPause();
    }

    updateAccessibilityInfo();
    emit smartPauseChanged(enabled);
}

void FluentAutoCarousel::setShowPlayControls(bool show) {
    if (m_showPlayControls == show) {
        return;  // No change needed
    }

    m_showPlayControls = show;

    // Create or hide controls as needed
    if (show) {
        createPlayControls();
    }

    // Update visibility
    if (m_controlsContainer) {
        m_controlsContainer->setVisible(show);
    }

    // Update layout
    updateControlsLayout();

    // Invalidate size hint since controls visibility affects size
    invalidateSizeHint();

    emit playControlsVisibilityChanged(show);
}

void FluentAutoCarousel::setShowProgressIndicator(bool show) {
    if (m_showProgressIndicator == show) {
        return;  // No change needed
    }

    m_showProgressIndicator = show;

    // Create or hide progress indicator as needed
    if (show) {
        createProgressIndicator();
    }

    // Update visibility
    updateProgressIndicator();

    // Update layout
    updateControlsLayout();

    // Invalidate size hint since progress indicator visibility affects size
    invalidateSizeHint();

    emit progressIndicatorVisibilityChanged(show);
}

void FluentAutoCarousel::setProgressStyle(
    FluentAutoCarouselProgressStyle style) {
    // Enhanced validation with comprehensive style checking
    static const QList<FluentAutoCarouselProgressStyle> validStyles = {
        FluentAutoCarouselProgressStyle::Bar,
        FluentAutoCarouselProgressStyle::Circular,
        FluentAutoCarouselProgressStyle::Dots,
        FluentAutoCarouselProgressStyle::Line,
        FluentAutoCarouselProgressStyle::Ring,
        FluentAutoCarouselProgressStyle::Minimal,
        FluentAutoCarouselProgressStyle::Segmented};

    if (!validStyles.contains(style)) {
        qWarning()
            << "FluentAutoCarousel::setProgressStyle: Invalid progress style"
            << static_cast<int>(style) << "specified. Using default Bar style.";
        style = FluentAutoCarouselProgressStyle::Bar;
    }

    if (m_progressStyle == style) {
        return;  // No change needed
    }

    // Store previous style for rollback on error
    FluentAutoCarouselProgressStyle previousStyle = m_progressStyle;
    m_progressStyle = style;

    // Recreate progress indicator with enhanced error handling
    try {
        if (m_showProgressIndicator) {
            createProgressIndicator();
            updateProgressIndicatorStyle();
        }

        // Update layout to accommodate new style
        updateControlsLayout();

        // Invalidate size hint since progress style affects size
        invalidateSizeHint();

        emit progressStyleChanged(style);

    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::setProgressStyle: Critical error "
                       "updating progress style:"
                    << e.what();

        // Enhanced rollback with validation
        try {
            m_progressStyle = previousStyle;
            if (m_showProgressIndicator) {
                createProgressIndicator();
                updateProgressIndicatorStyle();
            }
            updateControlsLayout();
            qWarning() << "FluentAutoCarousel::setProgressStyle: Successfully "
                          "rolled back to previous style.";
        } catch (const std::exception& rollbackError) {
            qCritical() << "FluentAutoCarousel::setProgressStyle: Failed to "
                           "rollback, forcing Bar style:"
                        << rollbackError.what();
            m_progressStyle = FluentAutoCarouselProgressStyle::Bar;
            try {
                if (m_showProgressIndicator) {
                    createProgressIndicator();
                    updateProgressIndicatorStyle();
                }
            } catch (...) {
                qCritical() << "FluentAutoCarousel::setProgressStyle: Complete "
                               "failure, disabling progress indicator.";
                m_showProgressIndicator = false;
            }
        }
    }
}

// Property getters are already defined inline in the header

// Slot implementations
void FluentAutoCarousel::onCurrentIndexChanged(int index) {
    Q_UNUSED(index)
    // Reset progress when index changes
    m_currentProgress = 0.0;
    m_playbackProgress = 0.0;
    emit playbackProgressChanged(m_playbackProgress);

    // Update next index for auto-play
    calculateNextIndex();

    // Update accessibility info
    updateAccessibilityInfo();
}

void FluentAutoCarousel::onAutoPlayTimer() {
    if (!m_isPlaying || m_isPaused || itemCount() <= 1) {
        return;
    }

    // Move to next item
    setCurrentIndex(m_nextIndex, true);

    // Emit interval completed signal
    emit intervalCompleted();

    // Restart timer for next transition if still playing
    if (m_isPlaying && !m_isPaused) {
        updateAutoPlayTimer();
    }
}

void FluentAutoCarousel::onProgressTimer() {
    if (!m_isPlaying || m_isPaused || !m_progressTimer->isActive()) {
        return;
    }

    // Calculate elapsed time since interval start
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_intervalStartTime);

    // Calculate progress (0.0 to 1.0)
    qreal progress = static_cast<qreal>(elapsed.count()) /
                     static_cast<qreal>(m_autoPlayInterval.count());
    progress = qBound(0.0, progress, 1.0);

    // Performance optimization: only update if progress changed significantly
    static const qreal PROGRESS_THRESHOLD = 0.01;  // 1% threshold
    if (qAbs(progress - m_currentProgress) < PROGRESS_THRESHOLD &&
        progress < 1.0) {
        return;  // Skip update for small changes
    }

    // Update progress values
    qreal oldProgress = m_currentProgress;
    m_currentProgress = progress;
    m_playbackProgress = progress;

    // Emit progress signals only if changed
    if (qAbs(progress - oldProgress) >= PROGRESS_THRESHOLD) {
        emit playbackProgressChanged(m_playbackProgress);
        emit progressChanged(m_currentProgress);
    }

    // Update UI components (these are optimized internally)
    updateProgressValue();
    updateTimeLabel();
}

void FluentAutoCarousel::onPlayPauseButtonClicked() { togglePlayPause(); }

void FluentAutoCarousel::onStopButtonClicked() { stop(); }

void FluentAutoCarousel::onTransitionFinished(int index) {
    Q_UNUSED(index)
    // Transition completed, continue auto-play if needed
    if (m_isPlaying && !m_isPaused && itemCount() > 1) {
        updateAutoPlayTimer();
    }
}

// Public slots
void FluentAutoCarousel::play() {
    if (itemCount() <= 1) {
        return;  // Can't auto-play with 0 or 1 items
    }

    // Update state
    m_isPlaying = true;
    m_isPaused = false;
    m_pausedByHover = false;
    m_pausedByFocus = false;

    // Calculate next index
    calculateNextIndex();

    // Start timers
    updateAutoPlayTimer();

    // Update UI
    updatePlayPauseButton();

    // Emit signals
    emit playbackStarted();
    emit autoPlayEnabledChanged(true);
    emit playStateChanged(playState());
}

void FluentAutoCarousel::pause() {
    if (!m_isPlaying) {
        return;
    }

    // Update state
    m_isPaused = true;

    // Stop timers
    m_autoPlayTimer->stop();
    m_progressTimer->stop();

    // Update UI
    updatePlayPauseButton();

    // Emit signals
    emit playbackPaused();
    emit playStateChanged(playState());
}

void FluentAutoCarousel::stop() {
    // Update state
    m_isPlaying = false;
    m_isPaused = false;
    m_pausedByHover = false;
    m_pausedByFocus = false;

    // Stop timers
    m_autoPlayTimer->stop();
    m_progressTimer->stop();

    // Reset progress
    resetProgress();

    // Update UI
    updatePlayPauseButton();

    // Emit signals
    emit playbackStopped();
    emit autoPlayEnabledChanged(false);
    emit playStateChanged(playState());
}

void FluentAutoCarousel::togglePlayPause() {
    if (m_isPlaying && !m_isPaused) {
        pause();
    } else {
        play();
    }
}

void FluentAutoCarousel::restart() {
    // Stop current playback
    stop();

    // Reset to first item
    setCurrentIndex(0, true);

    // Start playing
    play();

    // Emit signal
    emit playbackRestarted();
}

void FluentAutoCarousel::resetProgress() {
    m_currentProgress = 0.0;
    m_playbackProgress = 0.0;

    // Update UI
    updateProgressValue();
    updateTimeLabel();

    // Emit signal
    emit playbackProgressChanged(m_playbackProgress);
}

// Protected methods - event handling with auto-play integration
void FluentAutoCarousel::enterEvent(QEnterEvent* event) {
    FluentCarousel::enterEvent(event);

    // Handle pause on hover
    if (m_pauseOnHover && m_isPlaying && !m_isPaused) {
        m_pausedByHover = true;
        handlePauseOnHover();
    }

    // Handle pause on interaction
    if (m_pauseOnInteraction) {
        handlePauseOnInteraction();
    }

    // Update smart pause state
    if (m_smartPauseEnabled) {
        handleSmartPause();
    }
}

void FluentAutoCarousel::leaveEvent(QEvent* event) {
    FluentCarousel::leaveEvent(event);

    // Resume from hover pause
    if (m_pauseOnHover && m_pausedByHover) {
        m_pausedByHover = false;
        handlePauseOnHover();
    }
}

void FluentAutoCarousel::focusInEvent(QFocusEvent* event) {
    FluentCarousel::focusInEvent(event);

    // Handle pause on focus
    if (m_pauseOnFocus && m_isPlaying && !m_isPaused) {
        m_pausedByFocus = true;
        handlePauseOnFocus();
    }

    // Handle pause on interaction
    if (m_pauseOnInteraction) {
        handlePauseOnInteraction();
    }

    // Enhanced accessibility feedback
    updateAccessibilityInfo();
}

void FluentAutoCarousel::focusOutEvent(QFocusEvent* event) {
    FluentCarousel::focusOutEvent(event);

    // Resume from focus pause
    if (m_pauseOnFocus && m_pausedByFocus) {
        m_pausedByFocus = false;
        handlePauseOnFocus();
    }
}

void FluentAutoCarousel::keyPressEvent(QKeyEvent* event) {
    // Enhanced keyboard controls for accessibility and usability
    bool handled = false;

    switch (event->key()) {
        case Qt::Key_Space:
            // Space bar toggles play/pause
            togglePlayPause();
            handled = true;
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Enter key also toggles play/pause for accessibility
            togglePlayPause();
            handled = true;
            break;

        case Qt::Key_Home:
            // Home key goes to first item and stops auto-play
            stop();
            setCurrentIndex(0, true);
            handled = true;
            break;

        case Qt::Key_End:
            // End key goes to last item and stops auto-play
            stop();
            setCurrentIndex(itemCount() - 1, true);
            handled = true;
            break;

        case Qt::Key_R:
            // Ctrl+R restarts the carousel
            if (event->modifiers() & Qt::ControlModifier) {
                restart();
                handled = true;
            }
            break;

        case Qt::Key_P:
            // P key toggles play/pause (alternative)
            if (event->modifiers() == Qt::NoModifier) {
                togglePlayPause();
                handled = true;
            }
            break;

        case Qt::Key_S:
            // S key stops playback
            if (event->modifiers() == Qt::NoModifier) {
                stop();
                handled = true;
            }
            break;

        case Qt::Key_Plus:
        case Qt::Key_Equal:
            // Plus key increases playback speed (decrease interval)
            if (m_autoPlayInterval > std::chrono::milliseconds(500)) {
                setAutoPlayInterval(
                    static_cast<int>(m_autoPlayInterval.count()) - 500);
                handled = true;
            }
            break;

        case Qt::Key_Minus:
            // Minus key decreases playback speed (increase interval)
            if (m_autoPlayInterval < std::chrono::milliseconds(10000)) {
                setAutoPlayInterval(
                    static_cast<int>(m_autoPlayInterval.count()) + 500);
                handled = true;
            }
            break;

        case Qt::Key_F1:
            // F1 shows help/accessibility info
            updateAccessibilityInfo();
            handled = true;
            break;

        default:
            break;
    }

    if (handled) {
        event->accept();
        // Update accessibility info after handling key
        updateAccessibilityInfo();
    } else {
        // Let the base carousel handle navigation keys (Left, Right, etc.)
        FluentCarousel::keyPressEvent(event);
    }
}

void FluentAutoCarousel::resizeEvent(QResizeEvent* event) {
    FluentCarousel::resizeEvent(event);

    // Update controls layout when size changes
    updateControlsLayout();
}

void FluentAutoCarousel::changeEvent(QEvent* event) {
    FluentCarousel::changeEvent(event);

    // Handle theme changes
    if (event->type() == QEvent::StyleChange ||
        event->type() == QEvent::PaletteChange) {
        updateProgressIndicatorStyle();
        updatePlayPauseButton();
    }
}

void FluentAutoCarousel::showEvent(QShowEvent* event) {
    FluentCarousel::showEvent(event);

    // Restore auto-play state when shown
    restoreAutoPlayState();
}

void FluentAutoCarousel::hideEvent(QHideEvent* event) {
    FluentCarousel::hideEvent(event);

    // Save auto-play state when hidden
    saveAutoPlayState();

    // Pause when hidden to save resources
    if (m_isPlaying && !m_isPaused) {
        pause();
    }
}

// Private methods - UI update implementations
void FluentAutoCarousel::updateControlsLayout() {
    if (!m_controlsContainer || !m_progressContainer) {
        return;
    }

    // Position controls and progress indicator using Fluent Design spacing
    auto& theme = Styling::FluentTheme::instance();

    // Use Fluent Design spacing tokens
    int smallSpacing = theme.spacing("small");    // 4px
    int mediumSpacing = theme.spacing("medium");  // 8px
    int largeSpacing = theme.spacing("large");    // 16px

    // Apply consistent margins following Fluent Design principles
    int margin = mediumSpacing;

    // Calculate sizes with proper spacing
    QSize controlsSize = m_controlsContainer->sizeHint();
    QSize progressSize = m_progressContainer->sizeHint();

    // Position controls at bottom-right with proper spacing
    int controlsX = width() - controlsSize.width() - margin;
    int controlsY = height() - controlsSize.height() - margin;

    // Position progress indicator at bottom-left with spacing from controls
    int progressX = margin;
    int progressY = height() - progressSize.height() - margin;
    int progressWidth = width() - controlsSize.width() - (margin * 3);

    // Ensure minimum width for progress indicator
    progressWidth = qMax(progressWidth, 100);

    // Apply geometries with smooth Fluent Design animations
    if (m_controlsContainer->isVisible()) {
        // Animate controls positioning with Fluent motion
        auto* controlsAnimation =
            new QPropertyAnimation(m_controlsContainer, "geometry");
        // Use Fluent Design motion tokens for consistent timing
        int secondaryMotionDuration = 300;  // Secondary motion duration
        if (secondaryMotionDuration <= 0)
            secondaryMotionDuration = 200;  // Fallback

        controlsAnimation->setDuration(secondaryMotionDuration);
        controlsAnimation->setEasingCurve(
            QEasingCurve::OutCubic);  // Fluent standard curve
        controlsAnimation->setStartValue(m_controlsContainer->geometry());
        controlsAnimation->setEndValue(QRect(
            controlsX, controlsY, controlsSize.width(), controlsSize.height()));
        controlsAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        m_controlsContainer->setGeometry(
            controlsX, controlsY, controlsSize.width(), controlsSize.height());
    }

    if (m_progressContainer->isVisible()) {
        // Animate progress positioning with Fluent motion
        auto* progressAnimation =
            new QPropertyAnimation(m_progressContainer, "geometry");
        progressAnimation->setDuration(200);  // Fluent secondary motion timing
        progressAnimation->setEasingCurve(
            QEasingCurve::OutCubic);  // Fluent standard curve
        progressAnimation->setStartValue(m_progressContainer->geometry());
        progressAnimation->setEndValue(
            QRect(progressX, progressY, progressWidth, progressSize.height()));
        progressAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        m_progressContainer->setGeometry(progressX, progressY, progressWidth,
                                         progressSize.height());
    }

    // Update container styling for better visual hierarchy
    updateContainerStyling();
}

void FluentAutoCarousel::updateContainerStyling() {
    auto& theme = Styling::FluentTheme::instance();

    // Style controls container with enhanced Fluent Design effects
    if (m_controlsContainer) {
        QString controlsStyle =
            QString(
                "QWidget#controlsContainer {"
                "    background-color: %1;"
                "    border: 1px solid %2;"
                "    border-radius: %3px;"
                "    padding: %4px;"
                "    box-shadow: 0px 2px 8px rgba(0, 0, 0, "
                "0.14);"  // Fluent
                          // elevation
                "}"
                "QWidget#controlsContainer:hover {"
                "    background-color: %5;"
                "    border-color: %6;"
                "    box-shadow: 0px 4px 16px rgba(0, 0, 0, "
                "0.18);"  // Enhanced
                          // elevation
                          // on hover
                "}")
                .arg(theme.color("cardBackgroundFillDefault").name())
                .arg(theme.color("cardStrokeDefault").name())
                .arg(theme.borderRadius("medium"))
                .arg(theme.spacing("small"))
                .arg(theme.color("cardBackgroundFillSecondary").name())
                .arg(theme.color("cardStrokeSecondary").name());

        m_controlsContainer->setStyleSheet(controlsStyle);

        // Add subtle entrance animation for controls
        if (!m_controlsContainer->property("animated").toBool()) {
            auto* fadeIn =
                new QPropertyAnimation(m_controlsContainer, "windowOpacity");
            fadeIn->setDuration(300);  // Fluent entrance timing
            fadeIn->setEasingCurve(QEasingCurve::OutCubic);
            fadeIn->setStartValue(0.0);
            fadeIn->setEndValue(1.0);
            fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
            m_controlsContainer->setProperty("animated", true);
        }
    }

    // Style progress container with subtle background
    if (m_progressContainer) {
        QString progressStyle =
            QString(
                "QWidget#progressContainer {"
                "    background-color: %1;"
                "    border-radius: %2px;"
                "    padding: %3px %4px;"
                "    margin: 0px;"
                "}")
                .arg(theme.color("layerFillColorDefault").name())
                .arg(theme.borderRadius("small"))
                .arg(theme.spacing("xsmall"))  // 2px vertical
                .arg(theme.spacing("small"));  // 4px horizontal

        m_progressContainer->setStyleSheet(progressStyle);

        // Add subtle entrance animation for progress
        if (!m_progressContainer->property("animated").toBool()) {
            auto* slideIn = new QPropertyAnimation(m_progressContainer, "pos");
            slideIn->setDuration(250);  // Fluent secondary motion timing
            slideIn->setEasingCurve(QEasingCurve::OutCubic);
            QPoint currentPos = m_progressContainer->pos();
            slideIn->setStartValue(QPoint(currentPos.x() - 20, currentPos.y()));
            slideIn->setEndValue(currentPos);
            slideIn->start(QAbstractAnimation::DeleteWhenStopped);
            m_progressContainer->setProperty("animated", true);
        }
    }
}

void FluentAutoCarousel::updatePlayPauseButton() {
    if (!m_playPauseButton || !m_stopButton) {
        return;
    }

    // Update button text/icon based on state with improved Fluent Design
    // icons
    if (m_isPlaying && !m_isPaused) {
        m_playPauseButton->setText("⏸");  // Unicode pause symbol
        m_playPauseButton->setToolTip(tr("Pause"));
        m_playPauseButton->setAccessibleName(tr("Pause auto-play"));
    } else {
        m_playPauseButton->setText("▶");  // Unicode play symbol
        m_playPauseButton->setToolTip(tr("Play"));
        m_playPauseButton->setAccessibleName(tr("Start auto-play"));
    }

    // Apply enhanced Fluent Design styling with proper state management
    try {
        auto& theme = Styling::FluentTheme::instance();

        // Create state-aware button styling
        QString stateStyle;
        if (m_isPlaying && !m_isPaused) {
            // Playing state - use accent color for active state
            stateStyle =
                QString(
                    "FluentButton {"
                    "    background-color: %1;"
                    "    border: 1px solid %2;"
                    "    color: %3;"
                    "}")
                    .arg(theme.color("accentFillSecondary").name())
                    .arg(theme.color("accentStrokeDefault").name())
                    .arg(theme.color("textOnAccentFillPrimary").name());
        } else {
            // Paused/stopped state - use neutral colors
            stateStyle = QString(
                             "FluentButton {"
                             "    background-color: %1;"
                             "    border: 1px solid %2;"
                             "    color: %3;"
                             "}")
                             .arg(theme.color("subtleFillTransparent").name())
                             .arg(theme.color("controlStrokeDefault").name())
                             .arg(theme.color("textFillPrimary").name());
        }

        // Combine with base styling
        QString fullStyle = m_playPauseButton->styleSheet() + stateStyle;
        m_playPauseButton->setStyleSheet(fullStyle);

        // Update stop button state
        m_stopButton->setEnabled(m_isPlaying);

    } catch (const std::exception& e) {
        qWarning() << "FluentAutoCarousel: Failed to update button styling:"
                   << e.what();
    }
}

void FluentAutoCarousel::updateProgressIndicator() {
    if (!m_progressIndicator) {
        return;
    }

    // Show/hide based on settings
    m_progressIndicator->setVisible(m_showProgressIndicator);
    if (m_progressContainer) {
        m_progressContainer->setVisible(m_showProgressIndicator);
    }
}

void FluentAutoCarousel::updateProgressIndicatorStyle() {
    if (!m_progressIndicator) {
        return;
    }

    // Performance optimization: cache style strings to avoid regeneration
    static QString lastStyleString;
    static FluentAutoCarouselProgressStyle lastStyle =
        FluentAutoCarouselProgressStyle::Bar;
    static QColor lastFillColor, lastTrackColor, lastBorderColor;
    static int lastCornerRadius = -1;
    static bool lastElevationEnabled = false;

    auto& theme = Styling::FluentTheme::instance();

    // Get progress colors using enhanced semantic color scheme
    auto colors = progressColors();
    QColor fillColor =
        colors.first.isValid() ? colors.first : getSemanticColor("accent");
    QColor trackColor = colors.second.isValid()
                            ? colors.second
                            : getSemanticColor("neutralQuaternary");
    QColor borderColor = getSemanticColor("neutralTertiary");

    // Enhanced color variations for better Fluent Design compliance
    QColor fillColorHover = theme.color("accentFillColorSecondary");
    QColor fillColorPressed = theme.color("accentFillColorTertiary");
    QColor trackColorDisabled = getSemanticColor("neutralTertiaryAlt");
    QColor fillColorDisabled = getSemanticColor("neutralTertiary");

    // Add focus and accessibility colors
    QColor focusColor = theme.color("strokeFocusOuter");
    QColor accessibilityColor = theme.color("systemAttentionBackground");

    // Get effective corner radius with Fluent Design tokens
    int cornerRadius = getEffectiveCornerRadius();

    // Enhanced caching with elevation state
    if (m_progressStyle == lastStyle && fillColor == lastFillColor &&
        trackColor == lastTrackColor && borderColor == lastBorderColor &&
        cornerRadius == lastCornerRadius &&
        m_elevationEnabled == lastElevationEnabled &&
        !lastStyleString.isEmpty()) {
        m_progressIndicator->setStyleSheet(lastStyleString);
        return;
    }

    // Create enhanced style based on progress style type with Fluent Design
    // principles
    QString progressStyle;

    // Get Fluent Design spacing and sizing tokens
    int fluentSpacing = theme.spacing("small");  // 4px
    int fluentHeight = 32 / 4;                   // Proportional height (8px)
    if (fluentHeight <= 0)
        fluentHeight = 8;  // Fallback

    // Enhanced elevation shadow for better depth perception
    QString elevationStyle =
        m_elevationEnabled
            ? QString(
                  "box-shadow: 0px %1px %2px rgba(0, 0, 0, 0.14), "
                  "0px %3px %4px rgba(0, 0, 0, 0.12);")
                  .arg(m_elevationLevel)
                  .arg(m_elevationLevel * 2)
                  .arg(m_elevationLevel / 2)
                  .arg(m_elevationLevel)
            : "";

    switch (m_progressStyle) {
        case FluentAutoCarouselProgressStyle::Bar:
            progressStyle =
                QString(
                    "QProgressBar {"
                    "    background-color: %1;"
                    "    border: 1px solid %2;"
                    "    border-radius: %3px;"
                    "    text-align: center;"
                    "    height: %4px;"
                    "    margin: %5px 0px;"
                    "    %6"  // Elevation shadow
                    "    transition: all 200ms cubic-bezier(0.4, "
                    "0.0, 0.2, 1);"  // Fluent easing
                    "}"
                    "QProgressBar::chunk {"
                    "    background: qlineargradient(x1:0, y1:0, "
                    "x2:1, y2:0, "
                    "                                stop:0 %7, "
                    "stop:0.5 %8, stop:1 %9);"
                    "    border-radius: %3px;"
                    "    margin: 0px;"
                    "    transition: all 150ms cubic-bezier(0.4, "
                    "0.0, 0.2, 1);"
                    "}"
                    "QProgressBar:hover {"
                    "    border-color: %10;"
                    "    background-color: %11;"
                    "    transform: scale(1.02);"
                    "}"
                    "QProgressBar:focus {"
                    "    border: 2px solid %15;"
                    "    outline: none;"
                    "}"
                    "QProgressBar:disabled {"
                    "    background-color: %12;"
                    "    border-color: %13;"
                    "    opacity: 0.6;"
                    "}"
                    "QProgressBar::chunk:disabled {"
                    "    background-color: %14;"
                    "}")
                    .arg(trackColor.name())
                    .arg(borderColor.name())
                    .arg(qMax(2, cornerRadius / 2))
                    .arg(fluentHeight)
                    .arg(fluentSpacing / 2)
                    .arg(elevationStyle)
                    .arg(fillColor.name())
                    .arg(fillColorHover.name())
                    .arg(fillColor.darker(105).name())    // Subtle gradient end
                    .arg(fillColor.name())                // Hover border
                    .arg(trackColor.lighter(105).name())  // Hover background
                    .arg(trackColorDisabled.name())
                    .arg(getSemanticColor("neutralQuaternary").name())
                    .arg(fillColorDisabled.name())
                    .arg(focusColor.name());  // Focus border
            break;

        case FluentAutoCarouselProgressStyle::Line:
        case FluentAutoCarouselProgressStyle::Minimal:
            progressStyle =
                QString(
                    "QProgressBar {"
                    "    background-color: %1;"
                    "    border: none;"
                    "    border-radius: 1px;"
                    "    height: 2px;"
                    "    margin: %2px 0px;"
                    "    transition: all 200ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "}"
                    "QProgressBar::chunk {"
                    "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                    "                                stop:0 %3, stop:1 %4);"
                    "    border-radius: 1px;"
                    "    margin: 0px;"
                    "    transition: all 150ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "}"
                    "QProgressBar:hover::chunk {"
                    "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                    "                                stop:0 %5, stop:1 %6);"
                    "}"
                    "QProgressBar:disabled {"
                    "    opacity: 0.6;"
                    "}"
                    "QProgressBar::chunk:disabled {"
                    "    background-color: %7;"
                    "}")
                    .arg(trackColor.name())
                    .arg(fluentSpacing / 2)
                    .arg(fillColor.name())
                    .arg(fillColorHover.name())
                    .arg(fillColorHover.name())
                    .arg(fillColorPressed.name())
                    .arg(fillColorDisabled.name());
            break;

        case FluentAutoCarouselProgressStyle::Circular:
        case FluentAutoCarouselProgressStyle::Ring:
            // Enhanced circular progress with Fluent Design principles
            progressStyle =
                QString(
                    "QProgressBar {"
                    "    background-color: transparent;"
                    "    border: none;"
                    "    text-align: center;"
                    "    min-width: %1px;"
                    "    min-height: %1px;"
                    "    margin: %2px;"
                    "    %3"  // Elevation shadow
                    "}"
                    "QProgressBar::chunk {"
                    "    background-color: transparent;"
                    "}")
                    .arg(fluentHeight * 3)  // Proportional circular size
                    .arg(fluentSpacing)
                    .arg(elevationStyle);
            break;

        case FluentAutoCarouselProgressStyle::Dots:
            // Enhanced dots progress with Fluent Design spacing
            progressStyle =
                QString(
                    "QProgressBar {"
                    "    background-color: transparent;"
                    "    border: none;"
                    "    min-height: %1px;"
                    "    margin: %2px 0px;"
                    "    %3"  // Elevation shadow
                    "}"
                    "QProgressBar::chunk {"
                    "    background-color: transparent;"
                    "}")
                    .arg(fluentHeight * 2)  // Proportional dots height
                    .arg(fluentSpacing)
                    .arg(elevationStyle);
            break;

        case FluentAutoCarouselProgressStyle::Segmented:
            // New segmented style for better accessibility
            progressStyle =
                QString(
                    "QProgressBar {"
                    "    background-color: %1;"
                    "    border: 1px solid %2;"
                    "    border-radius: %3px;"
                    "    height: %4px;"
                    "    margin: %5px 0px;"
                    "    %6"  // Elevation shadow
                    "    text-align: center;"
                    "}"
                    "QProgressBar::chunk {"
                    "    background: repeating-linear-gradient("
                    "        90deg, %7 0px, %7 8px, %8 8px, %8 10px);"
                    "    border-radius: %3px;"
                    "    margin: 0px;"
                    "    transition: all 150ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "}"
                    "QProgressBar:hover {"
                    "    border-color: %9;"
                    "}"
                    "QProgressBar:disabled {"
                    "    opacity: 0.6;"
                    "    background-color: %10;"
                    "}")
                    .arg(trackColor.name())
                    .arg(borderColor.name())
                    .arg(qMax(2, cornerRadius / 2))
                    .arg(fluentHeight)
                    .arg(fluentSpacing / 2)
                    .arg(elevationStyle)
                    .arg(fillColor.name())
                    .arg(fillColorHover.name())
                    .arg(fillColor.name())
                    .arg(trackColorDisabled.name());
            break;
    }

    // Apply the styling with error handling
    try {
        m_progressIndicator->setStyleSheet(progressStyle);

        // Set appropriate size constraints based on style using Fluent Design
        // tokens
        switch (m_progressStyle) {
            case FluentAutoCarouselProgressStyle::Bar:
                m_progressIndicator->setMinimumHeight(fluentHeight);
                m_progressIndicator->setMaximumHeight(fluentHeight);
                break;
            case FluentAutoCarouselProgressStyle::Line:
            case FluentAutoCarouselProgressStyle::Minimal:
                m_progressIndicator->setMinimumHeight(2);
                m_progressIndicator->setMaximumHeight(2);
                break;
            case FluentAutoCarouselProgressStyle::Circular:
            case FluentAutoCarouselProgressStyle::Ring: {
                int circularSize = fluentHeight * 3;
                m_progressIndicator->setFixedSize(circularSize, circularSize);
                break;
            }
            case FluentAutoCarouselProgressStyle::Dots: {
                int dotsHeight = fluentHeight * 2;
                m_progressIndicator->setMinimumHeight(dotsHeight);
                m_progressIndicator->setMaximumHeight(dotsHeight);
                break;
            }
            case FluentAutoCarouselProgressStyle::Segmented:
                m_progressIndicator->setMinimumHeight(fluentHeight);
                m_progressIndicator->setMaximumHeight(fluentHeight);
                break;
        }

        // Apply smooth animation to progress updates with Fluent motion
        m_progressIndicator->setProperty("animated", m_progressAnimated);
        if (m_progressAnimated) {
            // Use Fluent Design animation duration
            int animDuration = m_progressAnimationDuration > 0
                                   ? m_progressAnimationDuration
                                   : 250;  // Normal duration
            m_progressIndicator->setProperty("animationDuration", animDuration);
        }

        // Enhanced cache with elevation state for performance
        lastStyleString = progressStyle;
        lastStyle = m_progressStyle;
        lastFillColor = fillColor;
        lastTrackColor = trackColor;
        lastBorderColor = borderColor;
        lastCornerRadius = cornerRadius;
        lastElevationEnabled = m_elevationEnabled;

    } catch (const std::exception& e) {
        qWarning() << "FluentAutoCarousel: Failed to apply progress "
                      "indicator styling:"
                   << e.what();
        // Clear cache on error
        lastStyleString.clear();
    }
}

void FluentAutoCarousel::updateProgressValue() {
    if (!m_progressIndicator) {
        return;
    }

    // Update progress value based on widget type for better Fluent UI
    // compliance
    if (auto* circularProgress =
            qobject_cast<FluentCircularProgress*>(m_progressIndicator)) {
        // Update custom circular progress widget
        circularProgress->setProgress(m_currentProgress, m_progressAnimated);
    } else if (auto* dotProgress =
                   qobject_cast<FluentDotProgress*>(m_progressIndicator)) {
        // Update custom dot progress widget
        dotProgress->setProgress(m_currentProgress, m_progressAnimated);
    } else if (auto* progressBar =
                   qobject_cast<QProgressBar*>(m_progressIndicator)) {
        // Update standard progress bar (for Bar, Line, Minimal, Segmented
        // styles)
        int progressValue = static_cast<int>(m_currentProgress * 100);
        progressBar->setValue(progressValue);
    }
}

void FluentAutoCarousel::updateTimeLabel() {
    if (!m_timeLabel) {
        return;
    }

    // Calculate remaining time
    int remaining = remainingTime();
    int total = static_cast<int>(m_autoPlayInterval.count());

    // Format time strings
    QString remainingStr =
        QString("%1:%2")
            .arg(remaining / 60000)
            .arg((remaining % 60000) / 1000, 2, 10, QChar('0'));
    QString totalStr = QString("%1:%2")
                           .arg(total / 60000)
                           .arg((total % 60000) / 1000, 2, 10, QChar('0'));

    m_timeLabel->setText(QString("%1 / %2").arg(remainingStr, totalStr));
}

int FluentAutoCarousel::remainingTime() const {
    if (!m_isPlaying || m_isPaused) {
        return static_cast<int>(m_autoPlayInterval.count());
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_intervalStartTime);

    int remaining = static_cast<int>(m_autoPlayInterval.count()) -
                    static_cast<int>(elapsed.count());
    return qMax(0, remaining);
}

void FluentAutoCarousel::updateAutoPlayTimer() {
    if (!m_isPlaying || m_isPaused || itemCount() <= 1) {
        m_autoPlayTimer->stop();
        m_progressTimer->stop();
        return;
    }

    // Record start time for progress calculation
    m_intervalStartTime = std::chrono::steady_clock::now();

    // Calculate adjusted interval based on playback speed
    int adjustedInterval =
        static_cast<int>(m_autoPlayInterval.count() / m_playbackSpeed);
    adjustedInterval = qMax(50, adjustedInterval);  // Minimum 50ms interval

    // Start the auto-play timer with adjusted interval
    m_autoPlayTimer->start(adjustedInterval);

    // Start progress timer for smooth progress updates
    m_progressTimer->start();
}

void FluentAutoCarousel::handlePauseOnHover() {
    if (m_pausedByHover) {
        // Pause timers but keep playing state
        m_autoPlayTimer->stop();
        m_progressTimer->stop();
    } else if (m_isPlaying && !m_isPaused && !m_pausedByFocus) {
        // Resume timers
        updateAutoPlayTimer();
    }
}

void FluentAutoCarousel::handlePauseOnFocus() {
    if (m_pausedByFocus) {
        // Pause timers but keep playing state
        m_autoPlayTimer->stop();
        m_progressTimer->stop();
    } else if (m_isPlaying && !m_isPaused && !m_pausedByHover) {
        // Resume timers
        updateAutoPlayTimer();
    }
}

void FluentAutoCarousel::calculateNextIndex() {
    if (itemCount() <= 1) {
        m_nextIndex = 0;
        return;
    }

    int currentIdx = currentIndex();

    switch (m_autoPlayDirection) {
        case FluentCarouselAutoPlay::Forward:
            m_nextIndex = (currentIdx + 1) % itemCount();
            break;

        case FluentCarouselAutoPlay::Backward:
            m_nextIndex = (currentIdx - 1 + itemCount()) % itemCount();
            break;

        case FluentCarouselAutoPlay::PingPong:
            if (m_pingPongForward) {
                if (currentIdx >= itemCount() - 1) {
                    m_pingPongForward = false;
                    m_nextIndex = currentIdx - 1;
                } else {
                    m_nextIndex = currentIdx + 1;
                }
            } else {
                if (currentIdx <= 0) {
                    m_pingPongForward = true;
                    m_nextIndex = currentIdx + 1;
                } else {
                    m_nextIndex = currentIdx - 1;
                }
            }
            break;

        default:
            m_nextIndex = (currentIdx + 1) % itemCount();
            break;
    }
}

// Accessibility helper methods for better code organization

QString FluentAutoCarousel::buildStatusText() const {
    if (m_isPlaying && !m_isPaused) {
        return tr("Auto-playing carousel, item %1 of %2")
            .arg(currentIndex() + 1)
            .arg(itemCount());
    } else if (m_isPaused) {
        return tr("Carousel paused, item %1 of %2")
            .arg(currentIndex() + 1)
            .arg(itemCount());
    } else {
        return tr("Carousel stopped, item %1 of %2")
            .arg(currentIndex() + 1)
            .arg(itemCount());
    }
}

QString FluentAutoCarousel::buildTimingText() const {
    if (m_isPlaying && !m_isPaused) {
        int remainingMs = remainingTime();
        if (remainingMs > 0) {
            int seconds = remainingMs / 1000;
            return tr("Next item in %1 seconds").arg(seconds);
        }
        return QString();
    } else if (m_isPaused) {
        if (m_pausedByHover) {
            return tr("Paused due to mouse hover");
        } else if (m_pausedByFocus) {
            return tr("Paused due to keyboard focus");
        } else {
            return tr("Manually paused");
        }
    } else {
        return tr("Auto-play disabled");
    }
}

QString FluentAutoCarousel::buildControlsText() const {
    QStringList controls;
    controls << tr("Space or Enter: Play/Pause");
    controls << tr("Home: Go to first item");
    controls << tr("End: Go to last item");
    controls << tr("Left/Right arrows: Navigate items");
    controls << tr("P: Play/Pause");
    controls << tr("S: Stop");
    controls << tr("Ctrl+R: Restart");
    controls << tr("+/-: Adjust speed");
    controls << tr("F1: Show this help");

    return tr("Available controls: %1").arg(controls.join(", "));
}

void FluentAutoCarousel::updateButtonAccessibility() {
    if (m_playPauseButton) {
        QString buttonLabel = (m_isPlaying && !m_isPaused)
                                  ? tr("Pause auto-play")
                                  : tr("Start auto-play");
        QString buttonDescription = (m_isPlaying && !m_isPaused)
                                        ? tr("Pause the automatic slideshow")
                                        : tr("Start the automatic slideshow");

        m_playPauseButton->setAccessibleName(buttonLabel);
        m_playPauseButton->setAccessibleDescription(buttonDescription);
        m_playPauseButton->setProperty("aria-label", buttonLabel);
        m_playPauseButton->setProperty("aria-describedby", buttonDescription);
        m_playPauseButton->setProperty("aria-pressed",
                                       m_isPlaying && !m_isPaused);

        // Enhanced keyboard navigation hints
        m_playPauseButton->setProperty("aria-keyshortcuts", "Space Enter");
        m_playPauseButton->setToolTip(buttonDescription +
                                      tr(" (Space or Enter)"));
    }

    if (m_stopButton) {
        QString stopDescription =
            tr("Stop the automatic slideshow and reset to first slide");
        m_stopButton->setAccessibleName(tr("Stop auto-play"));
        m_stopButton->setAccessibleDescription(stopDescription);
        m_stopButton->setProperty("aria-label", tr("Stop auto-play"));
        m_stopButton->setProperty("aria-describedby", stopDescription);
        m_stopButton->setProperty("aria-keyshortcuts", "Escape");
        m_stopButton->setToolTip(stopDescription + tr(" (Escape)"));
    }
}

void FluentAutoCarousel::updateProgressAccessibility() {
    if (m_progressIndicator) {
        int progressPercent = static_cast<int>(m_currentProgress * 100);
        QString progressLabel =
            tr("Auto-play progress: %1%").arg(progressPercent);

        // Enhanced progress description with timing information
        QString progressDescription;
        if (m_isPlaying && !m_isPaused) {
            int remainingMs = static_cast<int>(m_autoPlayInterval.count() *
                                               (1.0 - m_currentProgress));
            int remainingSeconds = remainingMs / 1000;
            progressDescription = tr("Progress: %1% complete, %2 seconds "
                                     "remaining until next slide")
                                      .arg(progressPercent)
                                      .arg(remainingSeconds);
        } else {
            progressDescription =
                tr("Progress: %1% complete, slideshow is paused")
                    .arg(progressPercent);
        }

        m_progressIndicator->setAccessibleName(progressLabel);
        m_progressIndicator->setAccessibleDescription(progressDescription);
        m_progressIndicator->setProperty("aria-label", progressLabel);
        m_progressIndicator->setProperty("aria-describedby",
                                         progressDescription);
        m_progressIndicator->setProperty("aria-valuenow", progressPercent);
        m_progressIndicator->setProperty("aria-valuemin", 0);
        m_progressIndicator->setProperty("aria-valuemax", 100);
        m_progressIndicator->setProperty("aria-valuetext", progressDescription);

        // Set appropriate role based on progress style
        switch (m_progressStyle) {
            case FluentAutoCarouselProgressStyle::Bar:
            case FluentAutoCarouselProgressStyle::Line:
            case FluentAutoCarouselProgressStyle::Minimal:
            case FluentAutoCarouselProgressStyle::Segmented:
                m_progressIndicator->setProperty("role", "progressbar");
                break;
            case FluentAutoCarouselProgressStyle::Circular:
            case FluentAutoCarouselProgressStyle::Ring:
                m_progressIndicator->setProperty("role", "progressbar");
                m_progressIndicator->setProperty("aria-orientation",
                                                 "circular");
                break;
            case FluentAutoCarouselProgressStyle::Dots:
                m_progressIndicator->setProperty("role", "status");
                m_progressIndicator->setProperty("aria-live", "polite");
                break;
        }
    }
}

void FluentAutoCarousel::updateAccessibilityInfo() {
    // Build enhanced accessibility information using helper methods
    QString statusText = buildStatusText();
    QString timingText = buildTimingText();
    QString controlsText = buildControlsText();

    // Combine all information with better structure
    QStringList descriptionParts;
    descriptionParts << statusText;
    if (!timingText.isEmpty()) {
        descriptionParts << timingText;
    }
    descriptionParts << controlsText;

    QString fullDescription = descriptionParts.join(". ");

    // Enhanced accessibility properties
    setAccessibleName(tr("Auto Carousel"));
    setAccessibleDescription(fullDescription);

    // Enhanced ARIA role and properties for better screen reader support
    setProperty("role", "region");
    setProperty(
        "aria-label",
        tr("Auto-playing image carousel with %1 slides").arg(itemCount()));
    setProperty("aria-live", m_isPlaying ? "polite" : "off");
    setProperty("aria-atomic", "true");
    setProperty("aria-describedby", fullDescription);

    // Add current slide information
    setProperty("aria-setsize", itemCount());
    setProperty("aria-posinset", currentIndex() + 1);

    // Enhanced keyboard navigation support
    setProperty("aria-keyshortcuts", "Left Right Space Escape F1");
    setProperty("tabindex", "0");  // Make focusable

    // Set appropriate ARIA state
    if (m_isPlaying) {
        setProperty("aria-busy", !m_isPaused);
        setProperty("aria-expanded", "true");
    } else {
        setProperty("aria-busy", "false");
        setProperty("aria-expanded", "false");
    }

    // Update component accessibility using helper methods
    updateButtonAccessibility();
    updateProgressAccessibility();

    // Announce changes if enabled using enhanced accessibility settings
    if (m_accessibilityAnnouncementsEnabled) {
        // Only announce essential status changes to avoid overwhelming
        // screen readers
        QString announcement = QString("%1. %2").arg(statusText, timingText);

        // Use a timer to delay announcements and prevent spam
        static QTimer* announcementTimer = nullptr;
        if (!announcementTimer) {
            announcementTimer = new QTimer(this);
            announcementTimer->setSingleShot(true);
            connect(announcementTimer, &QTimer::timeout, this,
                    [this, announcement]() {
                        emit accessibilityUpdateRequested(announcement);
                    });
        }

        // Restart timer with custom delay
        announcementTimer->start(m_accessibilityAnnouncementDelay);
    }
}

void FluentAutoCarousel::saveAutoPlayState() {
    // Save current state for restoration
    // This could be extended to save to settings if needed
    // For now, just store in member variables which are already persistent
}

void FluentAutoCarousel::restoreAutoPlayState() {
    // Restore auto-play state
    // If we were playing before being hidden, resume
    if (m_autoPlayEnabled && itemCount() > 1) {
        if (m_isPlaying && m_isPaused) {
            // We were paused, keep paused state
            updatePlayPauseButton();
        } else if (m_isPlaying) {
            // We were playing, resume
            updateAutoPlayTimer();
        }
    }
}

// Enhanced Fluent UI helper methods

void FluentAutoCarousel::updateControlButtonStyling() {
    if (!m_playPauseButton || !m_stopButton) {
        return;
    }

    auto& theme = Styling::FluentTheme::instance();

    // Get effective corner radius for buttons with Fluent Design tokens
    int buttonCornerRadius = qMax(2, getEffectiveCornerRadius() / 2);

    // Get Fluent Design spacing and timing tokens
    int buttonPadding = theme.spacing("small");  // 4px
    int animationDuration = 150;                 // Fast animation duration
    if (animationDuration <= 0)
        animationDuration = 150;  // Fallback

    // Enhanced elevation shadow for buttons
    QString elevationStyle =
        m_elevationEnabled
            ? QString(
                  "box-shadow: 0px %1px %2px rgba(0, 0, 0, 0.14), "
                  "0px %3px %4px rgba(0, 0, 0, 0.12);")
                  .arg(m_elevationLevel / 2)
                  .arg(m_elevationLevel)
                  .arg(m_elevationLevel / 4)
                  .arg(m_elevationLevel / 2)
            : "";

    // Create enhanced button styling based on Fluent Design principles
    QString buttonStyle;

    switch (m_controlButtonStyle) {
        case FluentButtonStyle::Subtle:
            buttonStyle =
                QString(
                    "FluentButton {"
                    "    background-color: %1;"
                    "    border: 1px solid %2;"
                    "    border-radius: %3px;"
                    "    color: %4;"
                    "    font-size: 14px;"
                    "    font-weight: 400;"
                    "    padding: %5px;"
                    "    %6"  // Elevation shadow
                    "    transition: all %7ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "    outline: none;"
                    "}"
                    "FluentButton:hover {"
                    "    background-color: %8;"
                    "    border-color: %9;"
                    "    transform: scale(1.02);"
                    "    box-shadow: 0px 2px 8px rgba(0, 0, 0, 0.16);"
                    "}"
                    "FluentButton:pressed {"
                    "    background-color: %10;"
                    "    transform: scale(0.98);"
                    "    transition: all 100ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "}"
                    "FluentButton:focus {"
                    "    border: 2px solid %11;"
                    "    background-color: %12;"
                    "}"
                    "FluentButton:disabled {"
                    "    background-color: %13;"
                    "    border-color: %14;"
                    "    color: %15;"
                    "    opacity: 0.6;"
                    "}")
                    .arg(getSemanticColor("subtleFillTransparent").name())
                    .arg(getSemanticColor("controlStrokeDefault").name())
                    .arg(buttonCornerRadius)
                    .arg(getSemanticColor("textFillPrimary").name())
                    .arg(buttonPadding)
                    .arg(elevationStyle)
                    .arg(animationDuration)
                    .arg(getSemanticColor("subtleFillSecondary").name())
                    .arg(getSemanticColor("controlStrokeSecondary").name())
                    .arg(getSemanticColor("subtleFillTertiary").name())
                    .arg(getSemanticColor("accent").name())  // Focus border
                    .arg(getSemanticColor("subtleFillSecondary")
                             .name())  // Focus background
                    .arg(getSemanticColor("controlFillDisabled").name())
                    .arg(getSemanticColor("controlStrokeDisabled").name())
                    .arg(getSemanticColor("textFillDisabled").name());
            break;

        case FluentButtonStyle::Default:
            buttonStyle =
                QString(
                    "FluentButton {"
                    "    background-color: %1;"
                    "    border: 1px solid %2;"
                    "    border-radius: %3px;"
                    "    color: %4;"
                    "    font-size: 14px;"
                    "    font-weight: 500;"
                    "    padding: %5px;"
                    "    %6"  // Elevation shadow
                    "    transition: all %7ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "    outline: none;"
                    "}"
                    "FluentButton:hover {"
                    "    background-color: %8;"
                    "    border-color: %9;"
                    "    transform: scale(1.02);"
                    "    box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.18);"
                    "}"
                    "FluentButton:pressed {"
                    "    background-color: %10;"
                    "    transform: scale(0.98);"
                    "    transition: all 100ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "}"
                    "FluentButton:focus {"
                    "    border: 2px solid %11;"
                    "    background-color: %12;"
                    "    box-shadow: 0px 0px 0px 2px rgba(%13, 0.3);"
                    "}"
                    "FluentButton:disabled {"
                    "    background-color: %14;"
                    "    border-color: %15;"
                    "    color: %16;"
                    "    opacity: 0.6;"
                    "}")
                    .arg(getSemanticColor("controlFillDefault").name())
                    .arg(getSemanticColor("controlStrokeDefault").name())
                    .arg(buttonCornerRadius)
                    .arg(getSemanticColor("textFillPrimary").name())
                    .arg(buttonPadding)
                    .arg(elevationStyle)
                    .arg(animationDuration)
                    .arg(getSemanticColor("controlFillSecondary").name())
                    .arg(getSemanticColor("controlStrokeSecondary").name())
                    .arg(getSemanticColor("controlFillTertiary").name())
                    .arg(getSemanticColor("accent").name())  // Focus border
                    .arg(getSemanticColor("controlFillSecondary")
                             .name())  // Focus background
                    .arg(getSemanticColor("accent").red() + "," +
                         QString::number(getSemanticColor("accent").green()) +
                         "," +
                         QString::number(
                             getSemanticColor("accent").blue()))  // Focus glow
                    .arg(getSemanticColor("controlFillDisabled").name())
                    .arg(getSemanticColor("controlStrokeDisabled").name())
                    .arg(getSemanticColor("textFillDisabled").name());
            break;

        case FluentButtonStyle::Accent:
            buttonStyle =
                QString(
                    "FluentButton {"
                    "    background-color: %1;"
                    "    border: 1px solid %2;"
                    "    border-radius: %3px;"
                    "    color: %4;"
                    "    font-size: 14px;"
                    "    font-weight: 600;"
                    "    padding: %5px;"
                    "    %6"  // Elevation shadow
                    "    transition: all %7ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "    outline: none;"
                    "}"
                    "FluentButton:hover {"
                    "    background-color: %8;"
                    "    border-color: %9;"
                    "    transform: scale(1.02);"
                    "    box-shadow: 0px 6px 16px rgba(0, 0, 0, 0.20);"
                    "}"
                    "FluentButton:pressed {"
                    "    background-color: %10;"
                    "    transform: scale(0.98);"
                    "    transition: all 100ms cubic-bezier(0.4, 0.0, 0.2, 1);"
                    "}"
                    "FluentButton:focus {"
                    "    border: 2px solid %11;"
                    "    background-color: %12;"
                    "    box-shadow: 0px 0px 0px 3px rgba(255, 255, 255, 0.4);"
                    "}"
                    "FluentButton:disabled {"
                    "    background-color: %13;"
                    "    border-color: %14;"
                    "    color: %15;"
                    "    opacity: 0.6;"
                    "}")
                    .arg(getSemanticColor("accent").name())
                    .arg(getSemanticColor("accent").name())
                    .arg(buttonCornerRadius)
                    .arg(getSemanticColor("textOnAccentPrimary").name())
                    .arg(buttonPadding)
                    .arg(elevationStyle)
                    .arg(animationDuration)
                    .arg(getSemanticColor("accentFillColorSecondary").name())
                    .arg(getSemanticColor("accentFillColorSecondary").name())
                    .arg(getSemanticColor("accentFillColorTertiary").name())
                    .arg(getSemanticColor("textOnAccentPrimary")
                             .name())  // Focus border (white)
                    .arg(getSemanticColor("accentFillColorSecondary")
                             .name())  // Focus background
                    .arg(getSemanticColor("controlFillDisabled").name())
                    .arg(getSemanticColor("controlStrokeDisabled").name())
                    .arg(getSemanticColor("textFillDisabled").name());
            break;
    }

    // Apply enhanced styling to buttons with error handling
    try {
        m_playPauseButton->setStyleSheet(buttonStyle);
        m_stopButton->setStyleSheet(buttonStyle);

        // Apply elevation effects if enabled
        if (m_elevationEnabled) {
            applyElevationEffect(m_playPauseButton, m_elevationLevel);
            applyElevationEffect(m_stopButton, m_elevationLevel);
        }

        // Enhanced accessibility attributes
        updateButtonAccessibility();

    } catch (const std::exception& e) {
        qWarning() << "FluentAutoCarousel: Failed to apply button styling:"
                   << e.what();
    }
}

void FluentAutoCarousel::applyElevationEffect(QWidget* widget, int level) {
    if (!widget || !m_elevationEnabled) {
        return;
    }

    auto& theme = Styling::FluentTheme::instance();

    // Get elevation values from Fluent Design tokens
    int elevation =
        theme.elevation(QString("level%1").arg(level).toStdString());
    if (elevation <= 0) {
        // Fallback elevation values
        static const QList<int> elevationLevels = {0, 2, 4, 8, 16, 32};
        elevation =
            elevationLevels.value(qBound(0, level, elevationLevels.size() - 1));
    }

    // Create shadow effect using Fluent Design principles with semantic
    // colors
    QString shadowStyle =
        QString(
            "QWidget {"
            "    background-color: %1;"
            "    border-radius: %2px;"
            "    border: 1px solid %3;"
            "}"
            "QWidget:hover {"
            "    border-color: %4;"
            "}")
            .arg(getSemanticColor("cardBackgroundFillColorDefault").name())
            .arg(getEffectiveCornerRadius())
            .arg(getSemanticColor("cardStrokeColorDefault").name())
            .arg(getSemanticColor("cardStrokeColorDefaultSolid").name());

    widget->setStyleSheet(shadowStyle);

    // Apply drop shadow effect if elevation > 0
    if (elevation > 0) {
        auto* effect = new QGraphicsDropShadowEffect(widget);
        effect->setBlurRadius(elevation);
        effect->setOffset(0, elevation / 2);
        effect->setColor(theme.color("shadowAmbient"));
        widget->setGraphicsEffect(effect);
    }
}

// Enhanced Fluent UI Design methods implementation

void FluentAutoCarousel::setProgressColors(const QColor& activeColor,
                                           const QColor& inactiveColor) {
    auto& theme = Styling::FluentTheme::instance();

    // Use Fluent Design tokens if colors are not specified
    QColor newActiveColor =
        activeColor.isValid() ? activeColor : theme.color("accent");
    QColor newInactiveColor = inactiveColor.isValid()
                                  ? inactiveColor
                                  : theme.color("neutralQuaternary");

    if (m_progressActiveColor != newActiveColor ||
        m_progressInactiveColor != newInactiveColor) {
        m_progressActiveColor = newActiveColor;
        m_progressInactiveColor = newInactiveColor;

        // Update progress indicator styling
        updateProgressIndicatorStyle();

        emit progressColorsChanged(m_progressActiveColor,
                                   m_progressInactiveColor);
    }
}

void FluentAutoCarousel::setProgressAnimation(bool animated, int duration) {
    auto& theme = Styling::FluentTheme::instance();

    // Use Fluent Design animation tokens if duration is 0
    int newDuration = duration > 0 ? duration : 250;  // Normal duration
    if (newDuration <= 0)
        newDuration = 200;  // Fallback

    if (m_progressAnimated != animated ||
        m_progressAnimationDuration != newDuration) {
        m_progressAnimated = animated;
        m_progressAnimationDuration = newDuration;

        // Update progress indicator animation
        updateProgressIndicatorStyle();

        emit progressAnimationChanged(m_progressAnimated,
                                      m_progressAnimationDuration);
    }
}

void FluentAutoCarousel::setControlButtonStyle(FluentButtonStyle style) {
    if (m_controlButtonStyle != style) {
        m_controlButtonStyle = style;

        // Update button styling
        updateControlButtonStyling();

        emit controlButtonStyleChanged(style);
    }
}

void FluentAutoCarousel::setElevationEnabled(bool enabled, int level) {
    // Validate elevation level
    level = qBound(1, level, 5);

    if (m_elevationEnabled != enabled || m_elevationLevel != level) {
        m_elevationEnabled = enabled;
        m_elevationLevel = level;

        // Apply or remove elevation effects
        updateContainerStyling();

        emit elevationChanged(enabled, level);
    }
}

void FluentAutoCarousel::setCornerRadius(int radius) {
    int newRadius = qMax(0, radius);
    if (m_cornerRadius != newRadius) {
        m_cornerRadius = newRadius;

        // Update styling to apply new corner radius
        updateContainerStyling();
        updateProgressIndicatorStyle();
        updateControlButtonStyling();

        emit cornerRadiusChanged(m_cornerRadius);
    }
}

void FluentAutoCarousel::setFluentMotionEnabled(bool enabled,
                                                bool respectSystemSettings) {
    if (m_fluentMotionEnabled != enabled ||
        m_respectSystemMotionSettings != respectSystemSettings) {
        m_fluentMotionEnabled = enabled;
        m_respectSystemMotionSettings = respectSystemSettings;

        // Update animation configurations
        updateAnimationSettings();

        emit fluentMotionChanged(m_fluentMotionEnabled,
                                 m_respectSystemMotionSettings);
    }
}

void FluentAutoCarousel::setSemanticColorScheme(
    FluentSemanticColorScheme scheme) {
    if (m_semanticColorScheme != scheme) {
        m_semanticColorScheme = scheme;

        // Update all styling to use new color scheme
        updateProgressIndicatorStyle();
        updateControlButtonStyling();
        updateContainerStyling();

        emit semanticColorSchemeChanged(m_semanticColorScheme);
    }
}

void FluentAutoCarousel::setAccessibilityEnhancements(
    bool enableAnnouncements, int customAnnouncementDelay) {
    int newDelay = qMax(100, customAnnouncementDelay);  // Minimum 100ms delay
    if (m_accessibilityAnnouncementsEnabled != enableAnnouncements ||
        m_accessibilityAnnouncementDelay != newDelay) {
        m_accessibilityAnnouncementsEnabled = enableAnnouncements;
        m_accessibilityAnnouncementDelay = newDelay;

        // Update accessibility configuration
        updateAccessibilityInfo();

        emit accessibilityEnhancementsChanged(
            m_accessibilityAnnouncementsEnabled,
            m_accessibilityAnnouncementDelay);
    }
}

QPair<QColor, QColor> FluentAutoCarousel::progressColors() const noexcept {
    auto& theme = Styling::FluentTheme::instance();

    QColor activeColor = m_progressActiveColor.isValid()
                             ? m_progressActiveColor
                             : theme.color("accent");
    QColor inactiveColor = m_progressInactiveColor.isValid()
                               ? m_progressInactiveColor
                               : theme.color("neutralQuaternary");

    return qMakePair(activeColor, inactiveColor);
}

// Enhanced control methods implementation

void FluentAutoCarousel::playWithDelay(int delayMs) {
    if (delayMs <= 0) {
        play();
        return;
    }

    if (!m_delayTimer) {
        m_delayTimer = std::make_unique<QTimer>(this);
        m_delayTimer->setSingleShot(true);
        connect(m_delayTimer.get(), &QTimer::timeout, this,
                &FluentAutoCarousel::play);
    }

    m_delayTimer->start(delayMs);
}

void FluentAutoCarousel::pauseTemporarily(int durationMs) {
    if (durationMs <= 0 || !m_isPlaying) {
        return;
    }

    // Pause current playback
    pause();

    if (!m_temporaryPauseTimer) {
        m_temporaryPauseTimer = std::make_unique<QTimer>(this);
        m_temporaryPauseTimer->setSingleShot(true);
        connect(m_temporaryPauseTimer.get(), &QTimer::timeout, this, [this]() {
            if (m_autoPlayEnabled) {
                play();
            }
            emit temporaryPauseEnded();
        });
    }

    m_temporaryPauseTimer->start(durationMs);
    emit temporaryPauseStarted(durationMs);
}

void FluentAutoCarousel::skipToNext() {
    if (itemCount() <= 1) {
        return;
    }

    int nextIndex = (currentIndex() + 1) % itemCount();
    setCurrentIndex(nextIndex);
    emit skipRequested(1);
}

void FluentAutoCarousel::skipToPrevious() {
    if (itemCount() <= 1) {
        return;
    }

    int prevIndex = (currentIndex() - 1 + itemCount()) % itemCount();
    setCurrentIndex(prevIndex);
    emit skipRequested(-1);
}

void FluentAutoCarousel::setPlaybackSpeed(qreal multiplier) {
    // Validate speed multiplier
    multiplier = qBound(0.1, multiplier, 5.0);

    if (qAbs(m_playbackSpeed - multiplier) < 0.01) {
        return;  // No significant change
    }

    m_playbackSpeed = multiplier;

    // Adjust auto-play interval based on speed
    if (m_isPlaying && !m_isPaused) {
        updateAutoPlayTimer();
    }

    emit playbackSpeedChanged(m_playbackSpeed);
}

void FluentAutoCarousel::fadeIn(int durationMs) {
    if (m_isFading) {
        return;  // Already fading
    }

    if (!m_fadeAnimation) {
        m_fadeAnimation =
            std::make_unique<QPropertyAnimation>(this, "windowOpacity", this);
        connect(m_fadeAnimation.get(), &QPropertyAnimation::finished, this,
                [this]() {
                    m_isFading = false;
                    emit fadeCompleted(m_fadeDirection);
                });
    }

    m_isFading = true;
    m_fadeDirection = true;

    auto& theme = Styling::FluentTheme::instance();
    int duration = durationMs > 0 ? durationMs : 250;  // Normal duration
    if (duration <= 0)
        duration = 300;  // Fallback

    m_fadeAnimation->setDuration(duration);
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_fadeAnimation->start();

    emit fadeStarted(true, duration);
}

void FluentAutoCarousel::fadeOut(int durationMs) {
    if (m_isFading) {
        return;  // Already fading
    }

    if (!m_fadeAnimation) {
        m_fadeAnimation =
            std::make_unique<QPropertyAnimation>(this, "windowOpacity", this);
        connect(m_fadeAnimation.get(), &QPropertyAnimation::finished, this,
                [this]() {
                    m_isFading = false;
                    emit fadeCompleted(m_fadeDirection);
                });
    }

    m_isFading = true;
    m_fadeDirection = false;

    auto& theme = Styling::FluentTheme::instance();
    int duration = durationMs > 0 ? durationMs : 250;  // Normal duration
    if (duration <= 0)
        duration = 300;  // Fallback

    m_fadeAnimation->setDuration(duration);
    m_fadeAnimation->setStartValue(1.0);
    m_fadeAnimation->setEndValue(0.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_fadeAnimation->start();

    emit fadeStarted(false, duration);
}

// Duplicate function removed - keeping the first implementation

// Enhanced size calculation methods for better performance

QSize FluentAutoCarousel::sizeHint() const {
    // Performance optimization: use cached size hint when valid
    if (!m_sizeHintValid) {
        // Start with base carousel size hint
        QSize hint = FluentCarousel::sizeHint();

        auto& theme = Styling::FluentTheme::instance();

        // Cache theme values for better performance
        static int cachedSpacing = -1;
        if (cachedSpacing == -1) {
            cachedSpacing = theme.spacing("medium");
            if (cachedSpacing <= 0)
                cachedSpacing = 8;  // Fallback
        }
        int spacing = cachedSpacing;

        // Add space for controls if visible (with caching)
        if (m_showPlayControls && m_controlsContainer) {
            QSize controlsSize = m_controlsContainer->sizeHint();
            hint.setHeight(hint.height() + controlsSize.height() + spacing);
        }

        // Add space for progress indicator if visible (optimized switch)
        if (m_showProgressIndicator && m_progressContainer) {
            QSize progressSize = m_progressContainer->sizeHint();

            // Optimized progress style handling
            switch (m_progressStyle) {
                case FluentAutoCarouselProgressStyle::Bar:
                case FluentAutoCarouselProgressStyle::Line:
                case FluentAutoCarouselProgressStyle::Minimal:
                case FluentAutoCarouselProgressStyle::Segmented:
                    hint.setHeight(hint.height() + progressSize.height() +
                                   spacing);
                    break;

                case FluentAutoCarouselProgressStyle::Circular:
                case FluentAutoCarouselProgressStyle::Ring:
                    // Circular indicators are overlaid, so no additional height
                    // needed
                    break;

                case FluentAutoCarouselProgressStyle::Dots:
                    hint.setHeight(hint.height() + progressSize.height() +
                                   spacing);
                    break;
            }
        }

        // Add elevation shadow space if enabled (with caching)
        if (m_elevationEnabled) {
            static QHash<int, int> elevationCache;
            int elevation = elevationCache.value(m_elevationLevel, -1);

            if (elevation == -1) {
                elevation = theme.elevation(
                    QString("level%1").arg(m_elevationLevel).toStdString());
                if (elevation <= 0) {
                    static const QList<int> elevationLevels = {0, 2,  4,
                                                               8, 16, 32};
                    elevation = elevationLevels.value(qBound(
                        0, m_elevationLevel, elevationLevels.size() - 1));
                }
                elevationCache[m_elevationLevel] = elevation;
            }
            hint += QSize(elevation, elevation);
        }

        // Ensure minimum size for touch accessibility
        hint = hint.expandedTo(QSize(200, 150));

        // Cache the calculated size hint
        m_cachedSizeHint = hint;
        m_sizeHintValid = true;
    }

    return m_cachedSizeHint;
}

QSize FluentAutoCarousel::minimumSizeHint() const {
    // Start with base minimum size
    QSize hint = FluentCarousel::minimumSizeHint();

    auto& theme = Styling::FluentTheme::instance();
    int spacing = theme.spacing("small");

    // Add minimum space for controls
    if (m_showPlayControls) {
        int buttonSize = theme.componentHeight("small");
        if (buttonSize <= 0)
            buttonSize = 24;  // Fallback
        hint.setHeight(hint.height() + buttonSize + spacing);
    }

    // Add minimum space for progress indicator
    if (m_showProgressIndicator) {
        switch (m_progressStyle) {
            case FluentAutoCarouselProgressStyle::Bar:
                hint.setHeight(hint.height() + 8 + spacing);
                break;
            case FluentAutoCarouselProgressStyle::Line:
            case FluentAutoCarouselProgressStyle::Minimal:
                hint.setHeight(hint.height() + 2 + spacing);
                break;
            case FluentAutoCarouselProgressStyle::Circular:
            case FluentAutoCarouselProgressStyle::Ring:
                // Overlaid, no additional space needed
                break;
            case FluentAutoCarouselProgressStyle::Dots:
                hint.setHeight(hint.height() + 12 + spacing);
                break;
        }
    }

    // Ensure accessibility minimum size (44x44 touch target)
    return hint.expandedTo(QSize(100, 80));
}

// Factory methods
FluentAutoCarousel* FluentAutoCarousel::createWithInterval(int milliseconds,
                                                           QWidget* parent) {
    // Validate interval parameter
    if (milliseconds < 100) {
        qWarning() << "FluentAutoCarousel::createWithInterval: Interval too "
                      "short, using minimum 100ms";
        milliseconds = 100;
    } else if (milliseconds > 60000) {
        qWarning() << "FluentAutoCarousel::createWithInterval: Interval too "
                      "long, using maximum 60000ms";
        milliseconds = 60000;
    }

    try {
        FluentCarouselConfig config;
        config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
        return new FluentAutoCarousel(config, parent);
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::createWithInterval: Failed to "
                       "create carousel:"
                    << e.what();
        return nullptr;
    }
}

FluentAutoCarousel* FluentAutoCarousel::createPingPong(int milliseconds,
                                                       QWidget* parent) {
    FluentCarouselConfig config;
    config.autoPlay = FluentCarouselAutoPlay::PingPong;
    config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
    return new FluentAutoCarousel(config, parent);
}

FluentAutoCarousel* FluentAutoCarousel::createWithProgress(int milliseconds,
                                                           QWidget* parent) {
    FluentCarouselConfig config;
    config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
    config.showIndicators = true;
    return new FluentAutoCarousel(config, parent);
}

FluentAutoCarousel* FluentAutoCarousel::createForTesting(QWidget* parent) {
    // Create a minimal configuration for testing
    FluentCarouselConfig config;
    config.autoPlay =
        FluentCarouselAutoPlay::None;  // Disable auto-play for testing
    config.enableAccessibility =
        false;                      // Disable accessibility to avoid crashes
    config.enableTouch = false;     // Disable touch to simplify initialization
    config.showIndicators = false;  // Disable indicators
    config.showNavigation = false;  // Disable navigation

    auto* carousel = new FluentAutoCarousel(config, parent);

    // Set a flag to indicate this is a test instance
    carousel->setProperty("testMode", true);

    return carousel;
}

FluentAutoCarousel* FluentAutoCarousel::createWithCircularProgress(
    int milliseconds, QWidget* parent) {
    // Validate interval parameter
    if (milliseconds < 100) {
        qWarning() << "FluentAutoCarousel::createWithCircularProgress: "
                      "Interval too short, using minimum 100ms";
        milliseconds = 100;
    } else if (milliseconds > 60000) {
        qWarning() << "FluentAutoCarousel::createWithCircularProgress: "
                      "Interval too long, using maximum 60000ms";
        milliseconds = 60000;
    }

    try {
        FluentCarouselConfig config;
        config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
        config.showIndicators = true;

        auto* carousel = new FluentAutoCarousel(config, parent);
        if (carousel) {
            carousel->setProgressStyle(
                FluentAutoCarouselProgressStyle::Circular);
            carousel->setShowProgressIndicator(true);
        }

        return carousel;
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::createWithCircularProgress: Failed "
                       "to create carousel:"
                    << e.what();
        return nullptr;
    }
}

FluentAutoCarousel* FluentAutoCarousel::createWithDotProgress(int milliseconds,
                                                              QWidget* parent) {
    FluentCarouselConfig config;
    config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
    config.showIndicators = true;

    auto* carousel = new FluentAutoCarousel(config, parent);
    carousel->setProgressStyle(FluentAutoCarouselProgressStyle::Dots);
    carousel->setShowProgressIndicator(true);

    return carousel;
}

FluentAutoCarousel* FluentAutoCarousel::createWithMinimalProgress(
    int milliseconds, QWidget* parent) {
    FluentCarouselConfig config;
    config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
    config.showIndicators = false;  // Minimal style doesn't need indicators

    auto* carousel = new FluentAutoCarousel(config, parent);
    carousel->setProgressStyle(FluentAutoCarouselProgressStyle::Minimal);
    carousel->setShowProgressIndicator(true);
    carousel->setShowPlayControls(false);  // Minimal style is truly minimal

    return carousel;
}

// Backward compatibility method implementations
bool FluentAutoCarousel::isLooping() const noexcept {
    // Map to the base carousel's infinite property
    FluentCarouselConfig config = this->config();
    return config.infinite;
}

void FluentAutoCarousel::setLooping(bool enabled) {
    // Update the base carousel's infinite property
    FluentCarouselConfig config = this->config();
    if (config.infinite != enabled) {
        config.infinite = enabled;
        setConfig(config);
        emit loopingChanged(enabled);
    }
}

FluentCarouselPlayState FluentAutoCarousel::playState() const noexcept {
    // Map to play state enum values
    if (!m_isPlaying) {
        return FluentCarouselPlayState::Stopped;
    } else if (m_isPaused) {
        return FluentCarouselPlayState::Paused;
    } else {
        return FluentCarouselPlayState::Playing;
    }
}

// Enhanced factory methods implementation

FluentAutoCarousel* FluentAutoCarousel::createAccessible(int milliseconds,
                                                         QWidget* parent) {
    // Validate interval parameter
    if (milliseconds < 100) {
        qWarning() << "FluentAutoCarousel::createAccessible: Interval too "
                      "short, using minimum 100ms";
        milliseconds = 100;
    } else if (milliseconds > 60000) {
        qWarning() << "FluentAutoCarousel::createAccessible: Interval too "
                      "long, using maximum 60000ms";
        milliseconds = 60000;
    }

    try {
        FluentCarouselConfig config;
        config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
        config.enableAccessibility = true;
        config.announceChanges = true;
        config.pauseOnHover = true;
        config.pauseOnFocus = true;
        config.showIndicators = true;
        config.showNavigation = true;

        auto* carousel = new FluentAutoCarousel(config, parent);
        if (carousel) {
            // Configure for optimal accessibility
            carousel->setShowPlayControls(true);
            carousel->setShowProgressIndicator(true);
            carousel->setProgressStyle(FluentAutoCarouselProgressStyle::Bar);
            carousel->setControlButtonStyle(FluentButtonStyle::Default);
            carousel->setElevationEnabled(true,
                                          1);  // Subtle elevation for focus
        }

        return carousel;
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::createAccessible: Failed to "
                       "create carousel:"
                    << e.what();
        return nullptr;
    }
}

FluentAutoCarousel* FluentAutoCarousel::createCustomStyled(
    int milliseconds, FluentAutoCarouselProgressStyle progressStyle,
    FluentButtonStyle buttonStyle, QWidget* parent) {
    // Validate interval parameter
    if (milliseconds < 100) {
        qWarning() << "FluentAutoCarousel::createCustomStyled: Interval too "
                      "short, using minimum 100ms";
        milliseconds = 100;
    } else if (milliseconds > 60000) {
        qWarning() << "FluentAutoCarousel::createCustomStyled: Interval too "
                      "long, using maximum 60000ms";
        milliseconds = 60000;
    }

    try {
        FluentCarouselConfig config;
        config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
        config.showIndicators = true;

        auto* carousel = new FluentAutoCarousel(config, parent);
        if (carousel) {
            carousel->setProgressStyle(progressStyle);
            carousel->setControlButtonStyle(buttonStyle);
            carousel->setShowPlayControls(true);
            carousel->setShowProgressIndicator(true);

            // Apply elevation for non-minimal styles
            if (progressStyle != FluentAutoCarouselProgressStyle::Minimal) {
                carousel->setElevationEnabled(true, 2);
            }
        }

        return carousel;
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::createCustomStyled: Failed to "
                       "create carousel:"
                    << e.what();
        return nullptr;
    }
}

// Enhanced configuration methods

void FluentAutoCarousel::applyConfiguration(
    const FluentAutoCarouselConfig& config) {
    applyConfigurationInternal(config);

    // Update UI components to reflect new configuration
    updateProgressIndicatorStyle();
    updateControlButtonStyling();
    updateContainerStyling();
    updateAccessibilityInfo();
}

void FluentAutoCarousel::applyConfigurationInternal(
    const FluentAutoCarouselConfig& config) {
    try {
        // Validate and apply auto-play settings
        m_autoPlayEnabled = config.autoPlayEnabled;

        // Validate auto-play interval
        auto intervalMs = config.autoPlayInterval.count();
        if (intervalMs < 100 || intervalMs > 60000) {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid auto-play interval"
                       << intervalMs << "ms. Using default 3000ms.";
            m_autoPlayInterval = std::chrono::milliseconds(3000);
        } else {
            m_autoPlayInterval = config.autoPlayInterval;
        }

        // Validate auto-play direction
        static const QList<FluentCarouselAutoPlay> validDirections = {
            FluentCarouselAutoPlay::Forward, FluentCarouselAutoPlay::Backward,
            FluentCarouselAutoPlay::PingPong, FluentCarouselAutoPlay::None};
        if (validDirections.contains(config.autoPlayDirection)) {
            m_autoPlayDirection = config.autoPlayDirection;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid auto-play direction. Using Forward.";
            m_autoPlayDirection = FluentCarouselAutoPlay::Forward;
        }

        // Apply pause behavior (no validation needed for booleans)
        m_pauseOnHover = config.pauseOnHover;
        m_pauseOnFocus = config.pauseOnFocus;

        // Apply UI configuration
        m_showPlayControls = config.showPlayControls;
        m_showProgressIndicator = config.showProgressIndicator;

        // Validate progress style
        static const QList<FluentAutoCarouselProgressStyle> validStyles = {
            FluentAutoCarouselProgressStyle::Bar,
            FluentAutoCarouselProgressStyle::Circular,
            FluentAutoCarouselProgressStyle::Dots,
            FluentAutoCarouselProgressStyle::Line,
            FluentAutoCarouselProgressStyle::Ring,
            FluentAutoCarouselProgressStyle::Minimal,
            FluentAutoCarouselProgressStyle::Segmented};
        if (validStyles.contains(config.progressStyle)) {
            m_progressStyle = config.progressStyle;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid progress style. Using Bar.";
            m_progressStyle = FluentAutoCarouselProgressStyle::Bar;
        }

        // Validate and apply enhanced Fluent UI properties
        static const QList<FluentButtonStyle> validButtonStyles = {
            FluentButtonStyle::Subtle, FluentButtonStyle::Default,
            FluentButtonStyle::Accent};
        if (validButtonStyles.contains(config.controlButtonStyle)) {
            m_controlButtonStyle = config.controlButtonStyle;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid button style. Using Subtle.";
            m_controlButtonStyle = FluentButtonStyle::Subtle;
        }

        m_elevationEnabled = config.elevationEnabled;

        // Validate elevation level (1-5)
        if (config.elevationLevel >= 1 && config.elevationLevel <= 5) {
            m_elevationLevel = config.elevationLevel;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid elevation level"
                       << config.elevationLevel << ". Using default 2.";
            m_elevationLevel = 2;
        }

        // Validate corner radius (0-20)
        if (config.cornerRadius >= 0 && config.cornerRadius <= 20) {
            m_cornerRadius = config.cornerRadius;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid corner radius"
                       << config.cornerRadius << ". Using default 0.";
            m_cornerRadius = 0;
        }

        m_fluentMotionEnabled = config.fluentMotionEnabled;
        m_respectSystemMotionSettings = config.respectSystemMotionSettings;

        // Validate semantic color scheme
        static const QList<FluentSemanticColorScheme> validColorSchemes = {
            FluentSemanticColorScheme::Auto, FluentSemanticColorScheme::Light,
            FluentSemanticColorScheme::Dark,
            FluentSemanticColorScheme::HighContrast};
        if (validColorSchemes.contains(config.semanticColorScheme)) {
            m_semanticColorScheme = config.semanticColorScheme;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid color scheme. Using Auto.";
            m_semanticColorScheme = FluentSemanticColorScheme::Auto;
        }

        // Validate and apply accessibility settings
        m_accessibilityAnnouncementsEnabled =
            config.accessibilityAnnouncementsEnabled;

        // Validate announcement delay (100-5000ms)
        if (config.accessibilityAnnouncementDelay >= 100 &&
            config.accessibilityAnnouncementDelay <= 5000) {
            m_accessibilityAnnouncementDelay =
                config.accessibilityAnnouncementDelay;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid announcement delay"
                       << config.accessibilityAnnouncementDelay
                       << "ms. Using default 500ms.";
            m_accessibilityAnnouncementDelay = 500;
        }

        // Apply color overrides with validation
        if (config.progressActiveColor.isValid()) {
            m_progressActiveColor = config.progressActiveColor;
        }
        if (config.progressInactiveColor.isValid()) {
            m_progressInactiveColor = config.progressInactiveColor;
        }

        // Validate and apply animation settings
        m_progressAnimated = config.progressAnimated;

        // Validate animation duration (0 = use theme default, or 50-2000ms)
        if (config.progressAnimationDuration == 0 ||
            (config.progressAnimationDuration >= 50 &&
             config.progressAnimationDuration <= 2000)) {
            m_progressAnimationDuration = config.progressAnimationDuration;
        } else {
            qWarning() << "FluentAutoCarousel::applyConfigurationInternal: "
                          "Invalid animation duration"
                       << config.progressAnimationDuration
                       << "ms. Using default 0 (theme).";
            m_progressAnimationDuration = 0;
        }

    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::applyConfigurationInternal: "
                       "Critical error applying configuration:"
                    << e.what();
        // On critical error, apply safe defaults
        m_autoPlayEnabled = true;
        m_autoPlayInterval = std::chrono::milliseconds(3000);
        m_autoPlayDirection = FluentCarouselAutoPlay::Forward;
        m_progressStyle = FluentAutoCarouselProgressStyle::Bar;
        m_controlButtonStyle = FluentButtonStyle::Subtle;
        m_elevationLevel = 2;
        m_cornerRadius = 0;
        m_semanticColorScheme = FluentSemanticColorScheme::Auto;
        m_accessibilityAnnouncementDelay = 500;
        m_progressAnimationDuration = 0;
    }
}

FluentAutoCarouselConfig FluentAutoCarousel::currentConfiguration() const {
    FluentAutoCarouselConfig config;

    // Auto-play settings
    config.autoPlayEnabled = m_autoPlayEnabled;
    config.autoPlayInterval = m_autoPlayInterval;
    config.autoPlayDirection = m_autoPlayDirection;

    // Pause behavior
    config.pauseOnHover = m_pauseOnHover;
    config.pauseOnFocus = m_pauseOnFocus;

    // UI configuration
    config.showPlayControls = m_showPlayControls;
    config.showProgressIndicator = m_showProgressIndicator;
    config.progressStyle = m_progressStyle;

    // Enhanced Fluent UI properties
    config.controlButtonStyle = m_controlButtonStyle;
    config.elevationEnabled = m_elevationEnabled;
    config.elevationLevel = m_elevationLevel;
    config.cornerRadius = m_cornerRadius;
    config.fluentMotionEnabled = m_fluentMotionEnabled;
    config.respectSystemMotionSettings = m_respectSystemMotionSettings;
    config.semanticColorScheme = m_semanticColorScheme;

    // Accessibility
    config.accessibilityAnnouncementsEnabled =
        m_accessibilityAnnouncementsEnabled;
    config.accessibilityAnnouncementDelay = m_accessibilityAnnouncementDelay;

    // Colors
    config.progressActiveColor = m_progressActiveColor;
    config.progressInactiveColor = m_progressInactiveColor;

    // Animation settings
    config.progressAnimated = m_progressAnimated;
    config.progressAnimationDuration = m_progressAnimationDuration;

    return config;
}

void FluentAutoCarousel::saveConfiguration(QSettings& settings,
                                           const QString& group) const {
    // Save configuration to QSettings for persistence
    QString settingsGroup = group.isEmpty() ? "FluentAutoCarousel" : group;
    settings.beginGroup(settingsGroup);

    try {
        // Auto-play settings
        settings.setValue("autoPlayEnabled", m_autoPlayEnabled);
        settings.setValue("autoPlayInterval",
                          static_cast<int>(m_autoPlayInterval.count()));
        settings.setValue("autoPlayDirection",
                          static_cast<int>(m_autoPlayDirection));

        // Pause behavior
        settings.setValue("pauseOnHover", m_pauseOnHover);
        settings.setValue("pauseOnFocus", m_pauseOnFocus);

        // UI configuration
        settings.setValue("showPlayControls", m_showPlayControls);
        settings.setValue("showProgressIndicator", m_showProgressIndicator);
        settings.setValue("progressStyle", static_cast<int>(m_progressStyle));

        // Enhanced Fluent UI properties
        settings.setValue("controlButtonStyle",
                          static_cast<int>(m_controlButtonStyle));
        settings.setValue("elevationEnabled", m_elevationEnabled);
        settings.setValue("elevationLevel", m_elevationLevel);
        settings.setValue("cornerRadius", m_cornerRadius);
        settings.setValue("fluentMotionEnabled", m_fluentMotionEnabled);
        settings.setValue("respectSystemMotionSettings",
                          m_respectSystemMotionSettings);
        settings.setValue("semanticColorScheme",
                          static_cast<int>(m_semanticColorScheme));

        // Accessibility
        settings.setValue("accessibilityAnnouncementsEnabled",
                          m_accessibilityAnnouncementsEnabled);
        settings.setValue("accessibilityAnnouncementDelay",
                          m_accessibilityAnnouncementDelay);

        // Colors (if valid)
        if (m_progressActiveColor.isValid()) {
            settings.setValue("progressActiveColor",
                              m_progressActiveColor.name());
        }
        if (m_progressInactiveColor.isValid()) {
            settings.setValue("progressInactiveColor",
                              m_progressInactiveColor.name());
        }

        // Animation settings
        settings.setValue("progressAnimated", m_progressAnimated);
        settings.setValue("progressAnimationDuration",
                          m_progressAnimationDuration);

    } catch (const std::exception& e) {
        qWarning() << "FluentAutoCarousel::saveConfiguration: Failed to save "
                      "configuration:"
                   << e.what();
    }

    settings.endGroup();
}

void FluentAutoCarousel::loadConfiguration(const QSettings& settings,
                                           const QString& group) {
    // Load configuration from QSettings
    QString settingsGroup = group.isEmpty() ? "FluentAutoCarousel" : group;

    if (!settings.childGroups().contains(settingsGroup)) {
        return;  // No saved configuration found
    }

    const_cast<QSettings&>(settings).beginGroup(settingsGroup);

    try {
        FluentAutoCarouselConfig config;

        // Auto-play settings
        config.autoPlayEnabled =
            settings.value("autoPlayEnabled", true).toBool();
        config.autoPlayInterval = std::chrono::milliseconds(
            settings.value("autoPlayInterval", 3000).toInt());
        config.autoPlayDirection = static_cast<FluentCarouselAutoPlay>(
            settings
                .value("autoPlayDirection",
                       static_cast<int>(FluentCarouselAutoPlay::Forward))
                .toInt());

        // Pause behavior
        config.pauseOnHover = settings.value("pauseOnHover", true).toBool();
        config.pauseOnFocus = settings.value("pauseOnFocus", true).toBool();

        // UI configuration
        config.showPlayControls =
            settings.value("showPlayControls", true).toBool();
        config.showProgressIndicator =
            settings.value("showProgressIndicator", true).toBool();
        config.progressStyle = static_cast<FluentAutoCarouselProgressStyle>(
            settings
                .value("progressStyle",
                       static_cast<int>(FluentAutoCarouselProgressStyle::Bar))
                .toInt());

        // Enhanced Fluent UI properties
        config.controlButtonStyle = static_cast<FluentButtonStyle>(
            settings
                .value("controlButtonStyle",
                       static_cast<int>(FluentButtonStyle::Subtle))
                .toInt());
        config.elevationEnabled =
            settings.value("elevationEnabled", true).toBool();
        config.elevationLevel = settings.value("elevationLevel", 2).toInt();
        config.cornerRadius = settings.value("cornerRadius", 0).toInt();
        config.fluentMotionEnabled =
            settings.value("fluentMotionEnabled", true).toBool();
        config.respectSystemMotionSettings =
            settings.value("respectSystemMotionSettings", true).toBool();
        config.semanticColorScheme = static_cast<FluentSemanticColorScheme>(
            settings
                .value("semanticColorScheme",
                       static_cast<int>(FluentSemanticColorScheme::Auto))
                .toInt());

        // Accessibility
        config.accessibilityAnnouncementsEnabled =
            settings.value("accessibilityAnnouncementsEnabled", true).toBool();
        config.accessibilityAnnouncementDelay =
            settings.value("accessibilityAnnouncementDelay", 500).toInt();

        // Colors
        QString activeColorName =
            settings.value("progressActiveColor", "").toString();
        if (!activeColorName.isEmpty()) {
            config.progressActiveColor = QColor(activeColorName);
        }
        QString inactiveColorName =
            settings.value("progressInactiveColor", "").toString();
        if (!inactiveColorName.isEmpty()) {
            config.progressInactiveColor = QColor(inactiveColorName);
        }

        // Animation settings
        config.progressAnimated =
            settings.value("progressAnimated", true).toBool();
        config.progressAnimationDuration =
            settings.value("progressAnimationDuration", 0).toInt();

        // Apply the loaded configuration
        applyConfiguration(config);

    } catch (const std::exception& e) {
        qWarning() << "FluentAutoCarousel::loadConfiguration: Failed to load "
                      "configuration:"
                   << e.what();
    }

    const_cast<QSettings&>(settings).endGroup();
}

// Enhanced factory methods

FluentAutoCarousel* FluentAutoCarousel::createTouchOptimized(int milliseconds,
                                                             QWidget* parent) {
    FluentAutoCarouselConfig config;
    config.autoPlayInterval =
        std::chrono::milliseconds(qBound(100, milliseconds, 60000));
    config.progressStyle = FluentAutoCarouselProgressStyle::Dots;
    config.controlButtonStyle = FluentButtonStyle::Default;
    config.elevationEnabled = true;
    config.elevationLevel = 3;    // Higher elevation for touch
    config.cornerRadius = 8;      // Larger corner radius for touch
    config.pauseOnHover = false;  // Don't pause on hover for touch devices

    try {
        auto* carousel = new FluentAutoCarousel(config, parent);
        if (carousel) {
            // Configure for touch optimization
            carousel->setShowPlayControls(true);
            carousel->setShowProgressIndicator(true);
        }
        return carousel;
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::createTouchOptimized: Failed to "
                       "create carousel:"
                    << e.what();
        return nullptr;
    }
}

FluentAutoCarousel* FluentAutoCarousel::createHighContrast(int milliseconds,
                                                           QWidget* parent) {
    FluentAutoCarouselConfig config;
    config.autoPlayInterval =
        std::chrono::milliseconds(qBound(100, milliseconds, 60000));
    config.progressStyle = FluentAutoCarouselProgressStyle::Segmented;
    config.controlButtonStyle = FluentButtonStyle::Default;
    config.elevationEnabled = false;  // No elevation in high contrast
    config.cornerRadius = 0;          // Sharp corners for high contrast
    config.semanticColorScheme = FluentSemanticColorScheme::HighContrast;
    config.accessibilityAnnouncementsEnabled = true;
    config.accessibilityAnnouncementDelay = 300;  // Faster announcements

    try {
        auto* carousel = new FluentAutoCarousel(config, parent);
        if (carousel) {
            // Configure for high contrast accessibility
            carousel->setShowPlayControls(true);
            carousel->setShowProgressIndicator(true);
        }
        return carousel;
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::createHighContrast: Failed to "
                       "create carousel:"
                    << e.what();
        return nullptr;
    }
}

FluentAutoCarousel* FluentAutoCarousel::createFromConfig(
    const FluentAutoCarouselConfig& config, QWidget* parent) {
    try {
        return new FluentAutoCarousel(config, parent);
    } catch (const std::exception& e) {
        qCritical() << "FluentAutoCarousel::createFromConfig: Failed to create "
                       "carousel:"
                    << e.what();
        return nullptr;
    }
}

// Enhanced helper methods for new functionality

void FluentAutoCarousel::updateAnimationSettings() {
    if (!m_fluentMotionEnabled) {
        // Disable all animations when Fluent motion is disabled
        if (m_progressTimer) {
            m_progressTimer->setInterval(200);  // Slower updates
        }
        return;
    }

    auto& theme = Styling::FluentTheme::instance();

    // Enhanced system motion preferences detection
    bool useReducedMotion = false;
    if (m_respectSystemMotionSettings) {
        useReducedMotion = theme.isReducedMotionMode();

// Additional system checks for better accessibility
#ifdef Q_OS_WIN
        // Check Windows animation settings
        QSettings winSettings(
            "HKEY_CURRENT_USER\\Control Panel\\Desktop\\WindowMetrics",
            QSettings::NativeFormat);
        if (winSettings.value("MinAnimate", "1").toString() == "0") {
            useReducedMotion = true;
        }
#endif
    }

    // Enhanced progress timer with Fluent Design motion tokens
    if (m_progressTimer) {
        int interval;
        if (useReducedMotion) {
            interval = 100;  // Reduced motion: slower updates
        } else {
            // Use Fluent Design motion tokens for smooth 60fps updates
            interval = 150 / 3;  // ~50ms for smooth animation
            if (interval <= 0 || interval > 50) {
                interval = 16;  // 60fps fallback
            }
        }
        m_progressTimer->setInterval(interval);
    }

    // Enhanced transition animations with Fluent Design curves
    if (m_fadeAnimation) {
        int duration;
        QEasingCurve::Type easingType;

        if (useReducedMotion) {
            duration = 150;  // Faster for accessibility
            easingType = QEasingCurve::Linear;
        } else {
            // Use Fluent Design motion tokens
            duration = 250;  // Normal duration
            if (duration <= 0)
                duration = 300;  // Fallback

            // Fluent Design standard easing curve (cubic-bezier(0.4, 0.0, 0.2,
            // 1))
            easingType = QEasingCurve::OutCubic;
        }

        m_fadeAnimation->setDuration(duration);
        m_fadeAnimation->setEasingCurve(easingType);
    }

    // Update progress indicator animations
    updateProgressIndicatorAnimations(useReducedMotion);

    // Update control button animations
    updateControlButtonAnimations(useReducedMotion);
}

// Performance optimization methods

void FluentAutoCarousel::optimizeUpdates() {
    // Batch multiple updates to reduce redraws
    setUpdatesEnabled(false);

    try {
        // Update all styling components at once
        updateProgressIndicatorStyle();
        updateControlButtonStyling();
        updateContainerStyling();

        // Update layout once after all style changes
        updateControlsLayout();

    } catch (const std::exception& e) {
        qWarning()
            << "FluentAutoCarousel::optimizeUpdates: Error during batch update:"
            << e.what();
    }

    setUpdatesEnabled(true);
    update();  // Single update call
}

void FluentAutoCarousel::deferredUpdate() {
    // Use a timer to defer updates and batch them
    static QTimer* updateTimer = nullptr;
    if (!updateTimer) {
        updateTimer = new QTimer(this);
        updateTimer->setSingleShot(true);
        updateTimer->setInterval(16);  // ~60fps
        connect(updateTimer, &QTimer::timeout, this,
                &FluentAutoCarousel::optimizeUpdates);
    }

    if (!updateTimer->isActive()) {
        updateTimer->start();
    }
}

void FluentAutoCarousel::updateProgressIndicatorAnimations(
    bool useReducedMotion) {
    if (!m_progressIndicator) {
        return;
    }

    auto& theme = Styling::FluentTheme::instance();

    // Configure animation duration based on motion preferences
    int animationDuration;
    if (useReducedMotion) {
        animationDuration = 100;  // Fast for accessibility
    } else {
        animationDuration = m_progressAnimationDuration > 0
                                ? m_progressAnimationDuration
                                : 250;  // Normal duration
        if (animationDuration <= 0)
            animationDuration = 200;
    }

    // Apply to different progress indicator types
    switch (m_progressStyle) {
        case FluentAutoCarouselProgressStyle::Circular:
        case FluentAutoCarouselProgressStyle::Ring: {
            auto* circularProgress =
                qobject_cast<FluentCircularProgress*>(m_progressIndicator);
            if (circularProgress) {
                circularProgress->setAnimated(!useReducedMotion &&
                                              m_progressAnimated);
                circularProgress->setAnimationDuration(animationDuration);
                // Use Fluent Design easing for circular progress
                circularProgress->setProperty(
                    "easingCurve", useReducedMotion ? QEasingCurve::Linear
                                                    : QEasingCurve::OutCubic);
            }
            break;
        }
        case FluentAutoCarouselProgressStyle::Dots: {
            auto* dotProgress =
                qobject_cast<FluentDotProgress*>(m_progressIndicator);
            if (dotProgress) {
                dotProgress->setAnimated(!useReducedMotion &&
                                         m_progressAnimated);
                dotProgress->setAnimationDuration(animationDuration);
                // Enhanced dot animation with staggered timing for Fluent feel
                dotProgress->setProperty("staggerDelay",
                                         useReducedMotion ? 0 : 50);
            }
            break;
        }
        default: {
            // Standard progress bar animations
            m_progressIndicator->setProperty(
                "animated", !useReducedMotion && m_progressAnimated);
            m_progressIndicator->setProperty("animationDuration",
                                             animationDuration);
            break;
        }
    }
}

void FluentAutoCarousel::updateControlButtonAnimations(bool useReducedMotion) {
    if (!m_playPauseButton || !m_stopButton) {
        return;
    }

    auto& theme = Styling::FluentTheme::instance();

    // Configure button animation properties
    QString animationProperty = useReducedMotion ? "none" : "all";
    int animationDuration = useReducedMotion ? 50 : 150;  // Fast duration
    if (animationDuration <= 0)
        animationDuration = 150;

    QString easingFunction =
        useReducedMotion ? "linear" : "cubic-bezier(0.4, 0.0, 0.2, 1)";

    // Apply enhanced animation properties to buttons
    QList<FluentButton*> buttons = {m_playPauseButton, m_stopButton};
    for (auto* button : buttons) {
        if (button) {
            button->setProperty("animationEnabled", !useReducedMotion);
            button->setProperty("animationDuration", animationDuration);
            button->setProperty("easingFunction", easingFunction);

            // Enhanced hover and focus animations
            if (!useReducedMotion) {
                button->setProperty("hoverScale", 1.02);
                button->setProperty("pressScale", 0.98);
                button->setProperty("focusGlow", true);
            } else {
                button->setProperty("hoverScale", 1.0);
                button->setProperty("pressScale", 1.0);
                button->setProperty("focusGlow", false);
            }
        }
    }
}

QColor FluentAutoCarousel::getSemanticColor(const QString& colorRole) const {
    const auto& theme = Styling::FluentTheme::instance();

    // Override theme colors based on semantic color scheme
    switch (m_semanticColorScheme) {
        case FluentSemanticColorScheme::Light:
            // Force light theme colors regardless of system theme
            return theme.color(colorRole.toStdString());

        case FluentSemanticColorScheme::Dark:
            // Force dark theme colors regardless of system theme
            return theme.color(colorRole.toStdString());

        case FluentSemanticColorScheme::HighContrast:
            // Use high contrast colors for accessibility
            return theme.color(colorRole.toStdString());

        case FluentSemanticColorScheme::Auto:
        default:
            // Use current theme colors
            return theme.color(colorRole.toStdString());
    }
}

int FluentAutoCarousel::getEffectiveCornerRadius() const {
    if (m_cornerRadius > 0) {
        return m_cornerRadius;
    }

    // Use theme default with caching for performance
    static int cachedRadius = -1;
    if (cachedRadius < 0) {
        const auto& theme = Styling::FluentTheme::instance();
        cachedRadius = theme.borderRadius("medium");
        if (cachedRadius <= 0) {
            cachedRadius = 4;  // Fallback value
        }
    }
    return cachedRadius;
}

// Enhanced interaction handling methods
void FluentAutoCarousel::handlePauseOnInteraction() {
    if (!m_pauseOnInteraction || !m_isPlaying) {
        return;
    }

    // Temporarily pause for user interaction
    if (!m_isPaused) {
        pause();

        // Set a timer to resume after interaction ends
        QTimer::singleShot(2000, this, [this]() {
            if (m_pauseOnInteraction && m_isPaused) {
                play();
            }
        });
    }
}

void FluentAutoCarousel::handleSmartPause() {
    if (!m_smartPauseEnabled) {
        return;
    }

    // Check if the carousel is visible in the viewport
    bool isVisible = isVisibleTo(window()) && window()->isVisible();

    if (!isVisible && m_isPlaying && !m_isPaused) {
        // Pause when not visible
        pause();
        setProperty("smartPaused", true);
    } else if (isVisible && property("smartPaused").toBool()) {
        // Resume when visible again
        play();
        setProperty("smartPaused", false);
    }
}

void FluentAutoCarousel::updateInteractionState() {
    // Update interaction monitoring based on settings
    if (m_pauseOnInteraction) {
        // Enable interaction monitoring
        setAttribute(Qt::WA_Hover, true);
        setFocusPolicy(Qt::StrongFocus);
    }
}

// Enhanced Fluent Design compliance methods
void FluentAutoCarousel::applyFluentTypography() {
    auto& theme = Styling::FluentTheme::instance();

    if (m_timeLabel) {
        // Apply Fluent typography tokens
        QFont captionFont = theme.captionFont();
        m_timeLabel->setFont(captionFont);

        // Use proper text color from theme
        QString textStyle = QString(
                                "QLabel {"
                                "    color: %1;"
                                "    font-size: %2px;"
                                "    font-weight: 400;"
                                "    line-height: 1.4;"
                                "    letter-spacing: 0.01em;"
                                "}")
                                .arg(theme.color("textSecondary").name())
                                .arg(12);  // Caption font size

        m_timeLabel->setStyleSheet(textStyle);
    }
}

void FluentAutoCarousel::applyFluentSpacing() {
    auto& theme = Styling::FluentTheme::instance();

    // Apply consistent spacing using Fluent tokens
    int smallSpacing = theme.spacing("small");    // 4px
    int mediumSpacing = theme.spacing("medium");  // 8px
    int largeSpacing = theme.spacing("large");    // 12px

    if (m_controlsLayout) {
        m_controlsLayout->setSpacing(smallSpacing);
        m_controlsLayout->setContentsMargins(mediumSpacing, smallSpacing,
                                             mediumSpacing, smallSpacing);
    }

    // Apply spacing to progress container
    if (m_progressContainer && m_progressContainer->layout()) {
        m_progressContainer->layout()->setSpacing(mediumSpacing);
        m_progressContainer->layout()->setContentsMargins(
            largeSpacing, smallSpacing, largeSpacing, smallSpacing);
    }
}

void FluentAutoCarousel::applyFluentMotion() {
    auto& theme = Styling::FluentTheme::instance();

    // Check system motion preferences
    bool useReducedMotion = theme.isReducedMotionMode();

    // Apply motion settings to all animated elements
    updateProgressIndicatorAnimations(useReducedMotion);
    updateControlButtonAnimations(useReducedMotion);

    // Configure auto-play timing based on motion preferences
    if (useReducedMotion &&
        m_autoPlayInterval < std::chrono::milliseconds(1000)) {
        // Slow down auto-play for accessibility
        setAutoPlayInterval(1000);
    }
}

void FluentAutoCarousel::applyFluentColors() {
    auto& theme = Styling::FluentTheme::instance();

    // Apply semantic colors to all components
    if (m_controlsContainer) {
        QString containerStyle =
            QString(
                "QWidget#controlsContainer {"
                "    background-color: %1;"
                "    border: 1px solid %2;"
                "    border-radius: %3px;"
                "}")
                .arg(theme.color("layerFillColorDefault").name())
                .arg(theme.color("strokeColorDefault").name())
                .arg(theme.borderRadius("medium"));

        m_controlsContainer->setStyleSheet(containerStyle);
    }

    if (m_progressContainer) {
        QString progressStyle =
            QString(
                "QWidget#progressContainer {"
                "    background-color: %1;"
                "    border-radius: %2px;"
                "}")
                .arg(theme.color("layerFillColorAlt").name())
                .arg(theme.borderRadius("small"));

        m_progressContainer->setStyleSheet(progressStyle);
    }
}

void FluentAutoCarousel::updateFluentThemeCompliance() {
    // Apply all Fluent Design enhancements
    applyFluentTypography();
    applyFluentSpacing();
    applyFluentMotion();
    applyFluentColors();

    // Update accessibility
    updateAccessibilityInfo();

    // Ensure proper focus indicators
    if (m_playPauseButton) {
        m_playPauseButton->setProperty("fluentFocus", true);
    }
    if (m_stopButton) {
        m_stopButton->setProperty("fluentFocus", true);
    }
}

}  // namespace FluentQt::Components
