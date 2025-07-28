// include/FluentQt/Components/FluentAutoCarousel.h
#pragma once

#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentButton.h"
#include <QTimer>
#include <QProgressBar>
#include <QLabel>
#include <chrono>

namespace FluentQt::Components {

/**
 * @brief Auto-playing carousel component with configurable timing
 * 
 * FluentAutoCarousel provides an enhanced carousel with automatic playback:
 * - Configurable auto-play timing and direction
 * - Play/pause controls with visual feedback
 * - Progress indicator showing time until next transition
 * - Pause on hover and focus functionality
 * - Smooth transitions with customizable easing
 * - Auto-play state persistence
 */
class FluentAutoCarousel : public FluentCarousel {
    Q_OBJECT
    Q_PROPERTY(bool autoPlayEnabled READ isAutoPlayEnabled WRITE setAutoPlayEnabled NOTIFY autoPlayEnabledChanged)
    Q_PROPERTY(int autoPlayInterval READ autoPlayInterval WRITE setAutoPlayInterval NOTIFY autoPlayIntervalChanged)
    Q_PROPERTY(FluentCarouselAutoPlay autoPlayDirection READ autoPlayDirection WRITE setAutoPlayDirection NOTIFY autoPlayDirectionChanged)
    Q_PROPERTY(bool pauseOnHover READ pauseOnHover WRITE setPauseOnHover NOTIFY pauseOnHoverChanged)
    Q_PROPERTY(bool pauseOnFocus READ pauseOnFocus WRITE setPauseOnFocus NOTIFY pauseOnFocusChanged)
    Q_PROPERTY(bool showPlayControls READ showPlayControls WRITE setShowPlayControls NOTIFY playControlsVisibilityChanged)
    Q_PROPERTY(bool showProgressIndicator READ showProgressIndicator WRITE setShowProgressIndicator NOTIFY progressIndicatorVisibilityChanged)
    Q_PROPERTY(qreal playbackProgress READ playbackProgress NOTIFY playbackProgressChanged)

public:
    explicit FluentAutoCarousel(QWidget* parent = nullptr);
    explicit FluentAutoCarousel(const FluentCarouselConfig& config, QWidget* parent = nullptr);
    ~FluentAutoCarousel() override = default;

    // Auto-play configuration
    bool isAutoPlayEnabled() const noexcept { return m_autoPlayEnabled; }
    void setAutoPlayEnabled(bool enabled);

    int autoPlayInterval() const noexcept { return static_cast<int>(m_autoPlayInterval.count()); }
    void setAutoPlayInterval(int milliseconds);

    FluentCarouselAutoPlay autoPlayDirection() const noexcept { return m_autoPlayDirection; }
    void setAutoPlayDirection(FluentCarouselAutoPlay direction);

    // Pause behavior
    bool pauseOnHover() const noexcept { return m_pauseOnHover; }
    void setPauseOnHover(bool pause);

    bool pauseOnFocus() const noexcept { return m_pauseOnFocus; }
    void setPauseOnFocus(bool pause);

    // UI controls
    bool showPlayControls() const noexcept { return m_showPlayControls; }
    void setShowPlayControls(bool show);

    bool showProgressIndicator() const noexcept { return m_showProgressIndicator; }
    void setShowProgressIndicator(bool show);

    // State information
    bool isPlaying() const noexcept { return m_isPlaying; }
    bool isPaused() const noexcept { return m_isPaused; }
    qreal playbackProgress() const noexcept { return m_playbackProgress; }

    // Control access for customization
    FluentButton* playPauseButton() const noexcept { return m_playPauseButton; }
    FluentButton* stopButton() const noexcept { return m_stopButton; }
    QProgressBar* progressIndicator() const noexcept { return m_progressIndicator; }

    // Convenience factory methods
    static FluentAutoCarousel* createWithInterval(int milliseconds, QWidget* parent = nullptr);
    static FluentAutoCarousel* createPingPong(int milliseconds, QWidget* parent = nullptr);
    static FluentAutoCarousel* createWithProgress(int milliseconds, QWidget* parent = nullptr);

public slots:
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    void restart();
    void resetProgress();

signals:
    void autoPlayEnabledChanged(bool enabled);
    void autoPlayIntervalChanged(int interval);
    void autoPlayDirectionChanged(FluentCarouselAutoPlay direction);
    void pauseOnHoverChanged(bool pause);
    void pauseOnFocusChanged(bool pause);
    void playControlsVisibilityChanged(bool visible);
    void progressIndicatorVisibilityChanged(bool visible);
    void playbackProgressChanged(qreal progress);
    void playbackStarted();
    void playbackPaused();
    void playbackStopped();
    void playbackRestarted();
    void intervalCompleted();

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
    void updatePlayPauseButton();
    void updateProgressIndicator();
    void updateAutoPlayTimer();
    void handlePauseOnHover();
    void handlePauseOnFocus();
    void calculateNextIndex();
    void updateAccessibilityInfo();
    void saveAutoPlayState();
    void restoreAutoPlayState();

private:
    // Auto-play configuration
    bool m_autoPlayEnabled{true};
    std::chrono::milliseconds m_autoPlayInterval{3000};
    FluentCarouselAutoPlay m_autoPlayDirection{FluentCarouselAutoPlay::Forward};
    
    // Pause behavior
    bool m_pauseOnHover{true};
    bool m_pauseOnFocus{true};
    
    // UI configuration
    bool m_showPlayControls{true};
    bool m_showProgressIndicator{true};
    
    // State
    bool m_isPlaying{false};
    bool m_isPaused{false};
    bool m_pausedByHover{false};
    bool m_pausedByFocus{false};
    qreal m_playbackProgress{0.0};
    int m_nextIndex{0};
    bool m_pingPongForward{true};
    
    // Timers
    std::unique_ptr<QTimer> m_autoPlayTimer;
    std::unique_ptr<QTimer> m_progressTimer;
    std::chrono::steady_clock::time_point m_intervalStartTime;
    
    // UI controls
    FluentButton* m_playPauseButton{nullptr};
    FluentButton* m_stopButton{nullptr};
    QProgressBar* m_progressIndicator{nullptr};
    QWidget* m_controlsContainer{nullptr};
    QHBoxLayout* m_controlsLayout{nullptr};
    
    // Icons
    QIcon m_playIcon;
    QIcon m_pauseIcon;
    QIcon m_stopIcon;
};

} // namespace FluentQt::Components
