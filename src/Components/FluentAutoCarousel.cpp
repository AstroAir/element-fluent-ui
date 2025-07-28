// src/Components/FluentAutoCarousel.cpp
#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QPropertyAnimation>
#include <QTimer>
#include <QDebug>

namespace FluentQt::Components {

FluentAutoCarousel::FluentAutoCarousel(QWidget* parent)
    : FluentCarousel(parent)
    , m_progressTimer(std::make_unique<QTimer>(this))
    , m_progressAnimation(std::make_unique<QPropertyAnimation>(this))
{
    initializeAutoCarousel();
}

FluentAutoCarousel::FluentAutoCarousel(const FluentCarouselConfig& config, QWidget* parent)
    : FluentCarousel(config, parent)
    , m_progressTimer(std::make_unique<QTimer>(this))
    , m_progressAnimation(std::make_unique<QPropertyAnimation>(this))
{
    initializeAutoCarousel();
}

void FluentAutoCarousel::initializeAutoCarousel() {
    // Set default configuration for auto carousel
    FluentCarouselConfig config = this->config();
    config.autoPlay = FluentCarouselAutoPlay::Forward;
    config.autoPlayInterval = std::chrono::milliseconds(3000);
    config.pauseOnHover = true;
    config.pauseOnFocus = true;
    config.showIndicators = true;
    setConfig(config);
    
    createProgressIndicator();
    createPlayPauseControls();
    setupProgressAnimation();
    
    // Connect signals
    connect(this, &FluentCarousel::currentIndexChanged,
            this, &FluentAutoCarousel::onCurrentIndexChanged);
    connect(this, &FluentCarousel::autoPlayStarted,
            this, &FluentAutoCarousel::onAutoPlayStarted);
    connect(this, &FluentCarousel::autoPlayStopped,
            this, &FluentAutoCarousel::onAutoPlayStopped);
    connect(this, &FluentCarousel::autoPlayPaused,
            this, &FluentAutoCarousel::onAutoPlayPaused);
    connect(this, &FluentCarousel::autoPlayResumed,
            this, &FluentAutoCarousel::onAutoPlayResumed);
    
    updateProgressIndicator();
    updatePlayPauseButton();
}

void FluentAutoCarousel::createProgressIndicator() {
    // Create progress container
    m_progressContainer = new QWidget(this);
    m_progressContainer->setObjectName("FluentAutoCarousel_Progress");
    m_progressContainer->setFixedHeight(40);
    
    // Create progress layout
    auto* progressLayout = new QHBoxLayout(m_progressContainer);
    progressLayout->setContentsMargins(8, 4, 8, 4);
    progressLayout->setSpacing(8);
    
    // Create progress bar
    m_progressBar = new QProgressBar(m_progressContainer);
    m_progressBar->setObjectName("FluentAutoCarousel_ProgressBar");
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setAccessibleName("Auto-play progress");
    
    // Create time label
    m_timeLabel = new QLabel(m_progressContainer);
    m_timeLabel->setObjectName("FluentAutoCarousel_TimeLabel");
    m_timeLabel->setText("0:00 / 0:00");
    m_timeLabel->setMinimumWidth(80);
    m_timeLabel->setAccessibleName("Time remaining");
    
    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_timeLabel);
    
    // Add to main layout
    if (auto* mainLayout = qobject_cast<QVBoxLayout*>(layout())) {
        mainLayout->addWidget(m_progressContainer);
    }
}

void FluentAutoCarousel::createPlayPauseControls() {
    // Create controls container
    m_controlsContainer = new QWidget(this);
    m_controlsContainer->setObjectName("FluentAutoCarousel_Controls");
    m_controlsContainer->setFixedHeight(50);
    
    // Create controls layout
    auto* controlsLayout = new QHBoxLayout(m_controlsContainer);
    controlsLayout->setContentsMargins(8, 8, 8, 8);
    controlsLayout->setSpacing(8);
    
    // Create play/pause button
    m_playPauseButton = new FluentButton(m_controlsContainer);
    m_playPauseButton->setObjectName("FluentAutoCarousel_PlayPauseButton");
    m_playPauseButton->setButtonStyle(FluentButtonStyle::Standard);
    m_playPauseButton->setIcon(QIcon(":/icons/pause"));
    m_playPauseButton->setText("Pause");
    m_playPauseButton->setAccessibleName("Play/Pause auto-play");
    
    // Create previous button
    m_previousButton = new FluentButton(m_controlsContainer);
    m_previousButton->setObjectName("FluentAutoCarousel_PreviousButton");
    m_previousButton->setButtonStyle(FluentButtonStyle::Standard);
    m_previousButton->setIcon(QIcon(":/icons/skip-previous"));
    m_previousButton->setText("Previous");
    m_previousButton->setAccessibleName("Previous item");
    
    // Create next button
    m_nextButton = new FluentButton(m_controlsContainer);
    m_nextButton->setObjectName("FluentAutoCarousel_NextButton");
    m_nextButton->setButtonStyle(FluentButtonStyle::Standard);
    m_nextButton->setIcon(QIcon(":/icons/skip-next"));
    m_nextButton->setText("Next");
    m_nextButton->setAccessibleName("Next item");
    
    controlsLayout->addWidget(m_previousButton);
    controlsLayout->addWidget(m_playPauseButton);
    controlsLayout->addWidget(m_nextButton);
    controlsLayout->addStretch();
    
    // Connect button signals
    connect(m_playPauseButton, &FluentButton::clicked,
            this, &FluentAutoCarousel::onPlayPauseButtonClicked);
    connect(m_previousButton, &FluentButton::clicked,
            this, &FluentAutoCarousel::onPreviousButtonClicked);
    connect(m_nextButton, &FluentButton::clicked,
            this, &FluentAutoCarousel::onNextButtonClicked);
    
    // Add to main layout
    if (auto* mainLayout = qobject_cast<QVBoxLayout*>(layout())) {
        mainLayout->addWidget(m_controlsContainer);
    }
}

void FluentAutoCarousel::setupProgressAnimation() {
    // Setup progress timer
    m_progressTimer->setSingleShot(false);
    m_progressTimer->setInterval(100); // Update every 100ms
    connect(m_progressTimer.get(), &QTimer::timeout,
            this, &FluentAutoCarousel::updateProgressValue);
    
    // Setup progress animation
    m_progressAnimation->setTargetObject(m_progressBar);
    m_progressAnimation->setPropertyName("value");
    m_progressAnimation->setDuration(100);
    m_progressAnimation->setEasingCurve(QEasingCurve::Linear);
}

void FluentAutoCarousel::setShowProgressIndicator(bool show) {
    if (m_showProgressIndicator != show) {
        m_showProgressIndicator = show;
        
        if (m_progressContainer) {
            m_progressContainer->setVisible(show);
        }
        
        emit progressIndicatorVisibilityChanged(show);
    }
}

void FluentAutoCarousel::setShowPlayPauseControls(bool show) {
    if (m_showPlayPauseControls != show) {
        m_showPlayPauseControls = show;
        
        if (m_controlsContainer) {
            m_controlsContainer->setVisible(show);
        }
        
        emit playPauseControlsVisibilityChanged(show);
    }
}

void FluentAutoCarousel::setProgressIndicatorStyle(FluentAutoCarouselProgressStyle style) {
    if (m_progressStyle != style) {
        m_progressStyle = style;
        updateProgressIndicatorStyle();
        emit progressIndicatorStyleChanged(style);
    }
}

void FluentAutoCarousel::setAutoPlayDirection(FluentCarouselAutoPlay direction) {
    FluentCarouselConfig config = this->config();
    if (config.autoPlay != direction) {
        config.autoPlay = direction;
        setConfig(config);
        emit autoPlayDirectionChanged(direction);
    }
}

void FluentAutoCarousel::setAutoPlaySpeed(int milliseconds) {
    FluentCarouselConfig config = this->config();
    auto newInterval = std::chrono::milliseconds(milliseconds);
    
    if (config.autoPlayInterval != newInterval) {
        config.autoPlayInterval = newInterval;
        setConfig(config);
        
        // Reset progress if auto-play is active
        if (isAutoPlayActive()) {
            resetProgress();
        }
        
        emit autoPlaySpeedChanged(milliseconds);
    }
}

bool FluentAutoCarousel::isAutoPlayActive() const {
    // Check if the base carousel's auto-play timer is active
    return config().autoPlay != FluentCarouselAutoPlay::None;
}

bool FluentAutoCarousel::isAutoPlayPaused() const {
    // This would need to be exposed from the base class
    return false; // Placeholder
}

qreal FluentAutoCarousel::currentProgress() const {
    return m_currentProgress;
}

int FluentAutoCarousel::remainingTime() const {
    if (!isAutoPlayActive()) return 0;
    
    int totalTime = static_cast<int>(config().autoPlayInterval.count());
    return totalTime - static_cast<int>(m_currentProgress * totalTime / 100.0);
}

// Factory methods
FluentAutoCarousel* FluentAutoCarousel::createWithProgress(QWidget* parent) {
    auto* carousel = new FluentAutoCarousel(parent);
    carousel->setShowProgressIndicator(true);
    carousel->setShowPlayPauseControls(true);
    return carousel;
}

FluentAutoCarousel* FluentAutoCarousel::createMinimal(QWidget* parent) {
    auto* carousel = new FluentAutoCarousel(parent);
    carousel->setShowProgressIndicator(false);
    carousel->setShowPlayPauseControls(false);
    return carousel;
}

FluentAutoCarousel* FluentAutoCarousel::createWithCustomSpeed(int milliseconds, QWidget* parent) {
    FluentCarouselConfig config;
    config.autoPlay = FluentCarouselAutoPlay::Forward;
    config.autoPlayInterval = std::chrono::milliseconds(milliseconds);
    
    auto* carousel = new FluentAutoCarousel(config, parent);
    return carousel;
}

// Public slots
void FluentAutoCarousel::resetProgress() {
    m_currentProgress = 0.0;
    m_progressStartTime = std::chrono::steady_clock::now();
    
    if (m_progressBar) {
        m_progressBar->setValue(0);
    }
    
    updateTimeLabel();
    emit progressChanged(0.0);
}

void FluentAutoCarousel::updateProgressIndicator() {
    updateProgressValue();
    updateTimeLabel();
    updateProgressIndicatorStyle();
}

// Event handling
void FluentAutoCarousel::enterEvent(QEnterEvent* event) {
    FluentCarousel::enterEvent(event);
    
    if (config().pauseOnHover && isAutoPlayActive()) {
        pauseAutoPlay();
    }
}

void FluentAutoCarousel::leaveEvent(QEvent* event) {
    FluentCarousel::leaveEvent(event);
    
    if (config().pauseOnHover && isAutoPlayPaused()) {
        resumeAutoPlay();
    }
}

void FluentAutoCarousel::focusInEvent(QFocusEvent* event) {
    FluentCarousel::focusInEvent(event);
    
    if (config().pauseOnFocus && isAutoPlayActive()) {
        pauseAutoPlay();
    }
}

void FluentAutoCarousel::focusOutEvent(QFocusEvent* event) {
    FluentCarousel::focusOutEvent(event);
    
    if (config().pauseOnFocus && isAutoPlayPaused()) {
        resumeAutoPlay();
    }
}

// Private slots
void FluentAutoCarousel::onPlayPauseButtonClicked() {
    if (isAutoPlayActive() && !isAutoPlayPaused()) {
        pauseAutoPlay();
    } else {
        resumeAutoPlay();
    }
    
    emit playPauseButtonClicked();
}

void FluentAutoCarousel::onPreviousButtonClicked() {
    goToPrevious();
    resetProgress();
    emit previousButtonClicked();
}

void FluentAutoCarousel::onNextButtonClicked() {
    goToNext();
    resetProgress();
    emit nextButtonClicked();
}

void FluentAutoCarousel::onCurrentIndexChanged(int index) {
    Q_UNUSED(index)
    resetProgress();
}

void FluentAutoCarousel::onAutoPlayStarted() {
    m_progressTimer->start();
    resetProgress();
    updatePlayPauseButton();
}

void FluentAutoCarousel::onAutoPlayStopped() {
    m_progressTimer->stop();
    updatePlayPauseButton();
}

void FluentAutoCarousel::onAutoPlayPaused() {
    m_progressTimer->stop();
    updatePlayPauseButton();
}

void FluentAutoCarousel::onAutoPlayResumed() {
    m_progressTimer->start();
    updatePlayPauseButton();
}

void FluentAutoCarousel::updateProgressValue() {
    if (!isAutoPlayActive()) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_progressStartTime);
    auto total = config().autoPlayInterval;
    
    m_currentProgress = std::min(100.0, (elapsed.count() * 100.0) / total.count());
    
    if (m_progressBar) {
        m_progressBar->setValue(static_cast<int>(m_currentProgress));
    }
    
    updateTimeLabel();
    emit progressChanged(m_currentProgress);
}

void FluentAutoCarousel::updateTimeLabel() {
    if (!m_timeLabel) return;
    
    int remaining = remainingTime();
    int total = static_cast<int>(config().autoPlayInterval.count());
    
    QString timeText = QString("%1:%2 / %3:%4")
                      .arg(remaining / 60000)
                      .arg((remaining % 60000) / 1000, 2, 10, QChar('0'))
                      .arg(total / 60000)
                      .arg((total % 60000) / 1000, 2, 10, QChar('0'));
    
    m_timeLabel->setText(timeText);
}

void FluentAutoCarousel::updatePlayPauseButton() {
    if (!m_playPauseButton) return;
    
    if (isAutoPlayActive() && !isAutoPlayPaused()) {
        m_playPauseButton->setIcon(QIcon(":/icons/pause"));
        m_playPauseButton->setText("Pause");
        m_playPauseButton->setAccessibleDescription("Pause auto-play");
    } else {
        m_playPauseButton->setIcon(QIcon(":/icons/play"));
        m_playPauseButton->setText("Play");
        m_playPauseButton->setAccessibleDescription("Start auto-play");
    }
}

void FluentAutoCarousel::updateProgressIndicatorStyle() {
    if (!m_progressBar) return;
    
    // Apply different styles based on progress style
    switch (m_progressStyle) {
    case FluentAutoCarouselProgressStyle::Bar:
        m_progressBar->setStyleSheet("QProgressBar { border-radius: 4px; }");
        break;
    case FluentAutoCarouselProgressStyle::Circular:
        // Would need custom painting for circular progress
        break;
    case FluentAutoCarouselProgressStyle::Dots:
        // Would need custom widget for dot-based progress
        break;
    case FluentAutoCarouselProgressStyle::Line:
        m_progressBar->setStyleSheet("QProgressBar { border-radius: 1px; height: 2px; }");
        break;
    }
}

} // namespace FluentQt::Components
