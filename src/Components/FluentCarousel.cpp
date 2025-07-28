// src/Components/FluentCarousel.cpp
#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QPainter>
#include <QStyleOption>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTouchEvent>
#include <QGestureEvent>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <cmath>

namespace FluentQt::Components {

FluentCarousel::FluentCarousel(QWidget* parent)
    : FluentComponent(parent)
    , m_autoPlayTimer(std::make_unique<QTimer>(this))
    , m_transitionAnimation(std::make_unique<QPropertyAnimation>(this))
    , m_transitionGroup(std::make_unique<QSequentialAnimationGroup>(this))
{
    initializeComponent();
}

FluentCarousel::FluentCarousel(const FluentCarouselConfig& config, QWidget* parent)
    : FluentComponent(parent)
    , m_config(config)
    , m_autoPlayTimer(std::make_unique<QTimer>(this))
    , m_transitionAnimation(std::make_unique<QPropertyAnimation>(this))
    , m_transitionGroup(std::make_unique<QSequentialAnimationGroup>(this))
{
    initializeComponent();
}

FluentCarousel::~FluentCarousel() = default;

void FluentCarousel::initializeComponent() {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    setAttribute(Qt::WA_Hover, true);
    
    initializeLayout();
    initializeAnimations();
    initializeGestures();
    initializeAccessibility();
    
    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentCarousel::onThemeChanged);
    
    // Setup auto-play timer
    m_autoPlayTimer->setSingleShot(false);
    connect(m_autoPlayTimer.get(), &QTimer::timeout,
            this, &FluentCarousel::onAutoPlayTimer);
    
    updateAutoPlayTimer();
}

void FluentCarousel::initializeLayout() {
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(m_config.contentMargins);
    m_mainLayout->setSpacing(0);
    
    // Create content widget
    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("FluentCarousel_Content");
    
    // Create stacked widget for items
    m_stackedWidget = new QStackedWidget(m_contentWidget);
    m_stackedWidget->setObjectName("FluentCarousel_Stack");
    
    // Layout content widget
    auto* contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->addWidget(m_stackedWidget);
    
    m_mainLayout->addWidget(m_contentWidget);
    
    updateLayout();
}

void FluentCarousel::initializeAnimations() {
    // Setup transition animation
    m_transitionAnimation->setTargetObject(this);
    m_transitionAnimation->setPropertyName("transitionProgress");
    m_transitionAnimation->setDuration(static_cast<int>(m_config.transitionDuration.count()));
    m_transitionAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    connect(m_transitionAnimation.get(), &QPropertyAnimation::valueChanged,
            this, &FluentCarousel::onTransitionAnimationValueChanged);
    connect(m_transitionAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentCarousel::onTransitionAnimationFinished);
    
    // Setup transition group for complex animations
    m_transitionGroup->addAnimation(m_transitionAnimation.get());
}

void FluentCarousel::initializeGestures() {
    if (m_config.enableTouch) {
        grabGesture(Qt::PanGesture);
        grabGesture(Qt::SwipeGesture);
    }
}

void FluentCarousel::initializeAccessibility() {
    if (m_config.enableAccessibility) {
        setAccessibleName(m_config.ariaLabel);
        setAccessibleDescription("Carousel with " + QString::number(itemCount()) + " items");
        updateAccessibilityInfo();
    }
}

void FluentCarousel::setConfig(const FluentCarouselConfig& config) {
    if (m_config.transition != config.transition ||
        m_config.navigationStyle != config.navigationStyle ||
        m_config.orientation != config.orientation) {
        
        m_config = config;
        
        // Update animations
        m_transitionAnimation->setDuration(static_cast<int>(m_config.transitionDuration.count()));
        
        // Update layout
        updateLayout();
        updateNavigationVisibility();
        updateIndicatorVisibility();
        
        // Update auto-play
        updateAutoPlayTimer();
        
        // Update gestures
        if (m_config.enableTouch) {
            grabGesture(Qt::PanGesture);
            grabGesture(Qt::SwipeGesture);
        } else {
            ungrabGesture(Qt::PanGesture);
            ungrabGesture(Qt::SwipeGesture);
        }
        
        // Emit configuration change signals
        emit transitionChanged(m_config.transition);
        emit navigationStyleChanged(m_config.navigationStyle);
        emit orientationChanged(m_config.orientation);
        emit infiniteChanged(m_config.infinite);
        emit touchEnabledChanged(m_config.enableTouch);
        
        update();
    }
}

void FluentCarousel::addItem(QWidget* widget) {
    if (!widget) return;
    
    FluentCarouselItemData itemData(widget);
    addItem(itemData);
}

void FluentCarousel::addItem(const FluentCarouselItemData& itemData) {
    if (!itemData.widget) return;
    
    m_items.push_back(itemData);
    m_stackedWidget->addWidget(itemData.widget);
    
    // If this is the first item, make it current
    if (m_items.size() == 1) {
        m_currentIndex = 0;
        m_stackedWidget->setCurrentIndex(0);
    }
    
    updateNavigationVisibility();
    updateIndicatorVisibility();
    updateAccessibilityInfo();
    
    emit itemCountChanged(static_cast<int>(m_items.size()));
    
    m_sizeHintValid = false;
    updateGeometry();
}

void FluentCarousel::insertItem(int index, QWidget* widget) {
    if (!widget) return;
    
    FluentCarouselItemData itemData(widget);
    insertItem(index, itemData);
}

void FluentCarousel::insertItem(int index, const FluentCarouselItemData& itemData) {
    if (!itemData.widget || index < 0 || index > static_cast<int>(m_items.size())) {
        return;
    }
    
    m_items.insert(m_items.begin() + index, itemData);
    m_stackedWidget->insertWidget(index, itemData.widget);
    
    // Adjust current index if necessary
    if (index <= m_currentIndex) {
        m_currentIndex++;
    }
    
    // If this is the first item, make it current
    if (m_items.size() == 1) {
        m_currentIndex = 0;
        m_stackedWidget->setCurrentIndex(0);
    }
    
    updateNavigationVisibility();
    updateIndicatorVisibility();
    updateAccessibilityInfo();
    
    emit itemCountChanged(static_cast<int>(m_items.size()));
    
    m_sizeHintValid = false;
    updateGeometry();
}

void FluentCarousel::removeItem(int index) {
    if (!isValidIndex(index)) return;
    
    QWidget* widget = m_items[index].widget;
    m_items.erase(m_items.begin() + index);
    m_stackedWidget->removeWidget(widget);
    
    // Adjust current index
    if (index < m_currentIndex) {
        m_currentIndex--;
    } else if (index == m_currentIndex && m_currentIndex >= static_cast<int>(m_items.size())) {
        m_currentIndex = std::max(0, static_cast<int>(m_items.size()) - 1);
    }
    
    // Update current widget
    if (!m_items.empty()) {
        m_stackedWidget->setCurrentIndex(m_currentIndex);
    }
    
    updateNavigationVisibility();
    updateIndicatorVisibility();
    updateAccessibilityInfo();
    
    emit itemCountChanged(static_cast<int>(m_items.size()));
    
    m_sizeHintValid = false;
    updateGeometry();
}

void FluentCarousel::removeItem(QWidget* widget) {
    if (!widget) return;
    
    auto it = std::find_if(m_items.begin(), m_items.end(),
                          [widget](const FluentCarouselItemData& item) {
                              return item.widget == widget;
                          });
    
    if (it != m_items.end()) {
        int index = static_cast<int>(std::distance(m_items.begin(), it));
        removeItem(index);
    }
}

void FluentCarousel::clearItems() {
    m_items.clear();
    
    // Remove all widgets from stacked widget
    while (m_stackedWidget->count() > 0) {
        QWidget* widget = m_stackedWidget->widget(0);
        m_stackedWidget->removeWidget(widget);
    }
    
    m_currentIndex = 0;
    
    updateNavigationVisibility();
    updateIndicatorVisibility();
    updateAccessibilityInfo();
    
    emit itemCountChanged(0);
    
    m_sizeHintValid = false;
    updateGeometry();
}

QWidget* FluentCarousel::itemAt(int index) const {
    if (!isValidIndex(index)) return nullptr;
    return m_items[index].widget;
}

FluentCarouselItemData FluentCarousel::itemData(int index) const {
    if (!isValidIndex(index)) return FluentCarouselItemData();
    return m_items[index];
}

void FluentCarousel::setItemData(int index, const FluentCarouselItemData& data) {
    if (!isValidIndex(index) || !data.widget) return;
    
    m_items[index] = data;
    
    // Replace widget in stacked widget if different
    if (m_stackedWidget->widget(index) != data.widget) {
        QWidget* oldWidget = m_stackedWidget->widget(index);
        m_stackedWidget->removeWidget(oldWidget);
        m_stackedWidget->insertWidget(index, data.widget);
        
        if (index == m_currentIndex) {
            m_stackedWidget->setCurrentIndex(index);
        }
    }
    
    updateAccessibilityInfo();
    
    m_sizeHintValid = false;
    updateGeometry();
}

void FluentCarousel::setCurrentIndex(int index, bool animated) {
    if (!isValidIndex(index) || index == m_currentIndex) return;
    
    int oldIndex = m_currentIndex;
    
    if (animated && !m_transitioning) {
        startTransition(oldIndex, index, true);
    } else {
        m_currentIndex = index;
        m_stackedWidget->setCurrentIndex(index);
        emit currentIndexChanged(index);
        
        updateAccessibilityInfo();
        if (m_config.announceChanges) {
            announceCurrentItem();
        }
    }
    
    resetAutoPlayTimer();
}

bool FluentCarousel::canGoToPrevious() const {
    return m_config.infinite || m_currentIndex > 0;
}

bool FluentCarousel::canGoToNext() const {
    return m_config.infinite || m_currentIndex < static_cast<int>(m_items.size()) - 1;
}

void FluentCarousel::setTransition(FluentCarouselTransition transition) {
    if (m_config.transition != transition) {
        m_config.transition = transition;
        emit transitionChanged(transition);
        update();
    }
}

void FluentCarousel::setNavigationStyle(FluentCarouselNavigationStyle style) {
    if (m_config.navigationStyle != style) {
        m_config.navigationStyle = style;
        updateNavigationVisibility();
        emit navigationStyleChanged(style);
    }
}

void FluentCarousel::setOrientation(FluentCarouselOrientation orientation) {
    if (m_config.orientation != orientation) {
        m_config.orientation = orientation;
        updateLayout();
        emit orientationChanged(orientation);
    }
}

void FluentCarousel::setAutoPlayEnabled(bool enabled) {
    FluentCarouselAutoPlay newAutoPlay = enabled ? FluentCarouselAutoPlay::Forward : FluentCarouselAutoPlay::None;
    
    if (m_config.autoPlay != newAutoPlay) {
        m_config.autoPlay = newAutoPlay;
        updateAutoPlayTimer();
        emit autoPlayChanged(enabled);
    }
}

void FluentCarousel::setAutoPlayInterval(int milliseconds) {
    auto newInterval = std::chrono::milliseconds(milliseconds);
    
    if (m_config.autoPlayInterval != newInterval) {
        m_config.autoPlayInterval = newInterval;
        updateAutoPlayTimer();
        emit autoPlayIntervalChanged(milliseconds);
    }
}

void FluentCarousel::setInfinite(bool infinite) {
    if (m_config.infinite != infinite) {
        m_config.infinite = infinite;
        updateNavigationVisibility();
        emit infiniteChanged(infinite);
    }
}

void FluentCarousel::setTouchEnabled(bool enabled) {
    if (m_config.enableTouch != enabled) {
        m_config.enableTouch = enabled;
        
        if (enabled) {
            grabGesture(Qt::PanGesture);
            grabGesture(Qt::SwipeGesture);
        } else {
            ungrabGesture(Qt::PanGesture);
            ungrabGesture(Qt::SwipeGesture);
        }
        
        emit touchEnabledChanged(enabled);
    }
}

QSize FluentCarousel::sizeHint() const {
    if (!m_sizeHintValid) {
        QSize hint(400, 300); // Default size
        
        if (!m_items.empty() && m_stackedWidget) {
            hint = m_stackedWidget->sizeHint();
        }
        
        // Add margins
        hint += QSize(m_config.contentMargins.left() + m_config.contentMargins.right(),
                     m_config.contentMargins.top() + m_config.contentMargins.bottom());
        
        m_cachedSizeHint = hint;
        m_sizeHintValid = true;
    }
    
    return m_cachedSizeHint;
}

QSize FluentCarousel::minimumSizeHint() const {
    QSize hint(200, 150); // Minimum size
    
    if (!m_items.empty() && m_stackedWidget) {
        hint = m_stackedWidget->minimumSizeHint();
    }
    
    // Add margins
    hint += QSize(m_config.contentMargins.left() + m_config.contentMargins.right(),
                 m_config.contentMargins.top() + m_config.contentMargins.bottom());
    
    return hint;
}

// Navigation slots
void FluentCarousel::goToPrevious(bool animated) {
    if (!canGoToPrevious()) return;

    int newIndex = m_currentIndex - 1;
    if (newIndex < 0 && m_config.infinite) {
        newIndex = static_cast<int>(m_items.size()) - 1;
    }

    if (newIndex >= 0) {
        setCurrentIndex(newIndex, animated);
    }
}

void FluentCarousel::goToNext(bool animated) {
    if (!canGoToNext()) return;

    int newIndex = m_currentIndex + 1;
    if (newIndex >= static_cast<int>(m_items.size()) && m_config.infinite) {
        newIndex = 0;
    }

    if (newIndex < static_cast<int>(m_items.size())) {
        setCurrentIndex(newIndex, animated);
    }
}

void FluentCarousel::goToFirst(bool animated) {
    setCurrentIndex(0, animated);
}

void FluentCarousel::goToLast(bool animated) {
    if (!m_items.empty()) {
        setCurrentIndex(static_cast<int>(m_items.size()) - 1, animated);
    }
}

// Auto-play control
void FluentCarousel::startAutoPlay() {
    if (m_config.autoPlay != FluentCarouselAutoPlay::None && !m_items.empty()) {
        m_autoPlayTimer->start(static_cast<int>(m_config.autoPlayInterval.count()));
        emit autoPlayStarted();
    }
}

void FluentCarousel::stopAutoPlay() {
    if (m_autoPlayTimer->isActive()) {
        m_autoPlayTimer->stop();
        emit autoPlayStopped();
    }
}

void FluentCarousel::pauseAutoPlay() {
    if (m_autoPlayTimer->isActive() && !m_autoPlayPaused) {
        m_autoPlayTimer->stop();
        m_autoPlayPaused = true;
        emit autoPlayPaused();
    }
}

void FluentCarousel::resumeAutoPlay() {
    if (m_autoPlayPaused) {
        m_autoPlayPaused = false;
        startAutoPlay();
        emit autoPlayResumed();
    }
}

void FluentCarousel::stopTransition() {
    if (m_transitioning) {
        m_transitionAnimation->stop();
        m_transitionGroup->stop();
        m_transitioning = false;
        m_transitionProgress = 0.0;
        emit transitionProgressChanged(0.0);
    }
}

// Event handling
void FluentCarousel::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Paint background
    auto& theme = Styling::FluentTheme::instance();
    painter.fillRect(rect(), theme.color("surface"));

    // Paint border if needed
    if (state() == Core::FluentState::Focused) {
        QPen focusPen(theme.color("accent"), 2);
        painter.setPen(focusPen);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), cornerRadius(), cornerRadius());
    }
}

void FluentCarousel::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);
    updateLayout();
    updateItemPositions();
    m_sizeHintValid = false;
}

void FluentCarousel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_touchStartPos = event->position();
        m_touchCurrentPos = event->position();
        m_touchActive = true;
        m_touchStartTime = std::chrono::steady_clock::now();

        // Pause auto-play on interaction
        if (m_autoPlayTimer->isActive()) {
            pauseAutoPlay();
        }
    }

    FluentComponent::mousePressEvent(event);
}

void FluentCarousel::mouseMoveEvent(QMouseEvent* event) {
    if (m_touchActive) {
        m_touchCurrentPos = event->position();

        // Calculate drag distance
        QPointF delta = m_touchCurrentPos - m_touchStartPos;
        qreal distance = (m_config.orientation == FluentCarouselOrientation::Horizontal)
                        ? delta.x() : delta.y();

        // Update visual feedback for drag
        if (std::abs(distance) > 10) {
            update();
        }
    }

    FluentComponent::mouseMoveEvent(event);
}

void FluentCarousel::mouseReleaseEvent(QMouseEvent* event) {
    if (m_touchActive && event->button() == Qt::LeftButton) {
        m_touchActive = false;

        // Calculate swipe
        QPointF delta = m_touchCurrentPos - m_touchStartPos;
        qreal distance = (m_config.orientation == FluentCarouselOrientation::Horizontal)
                        ? delta.x() : delta.y();

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_touchStartTime);
        qreal velocity = std::abs(distance) / duration.count() * 1000.0; // pixels per second

        // Check for swipe gesture
        if (std::abs(distance) > m_config.swipeThreshold * width() &&
            velocity > m_config.velocityThreshold) {

            if (distance > 0) {
                goToPrevious();
            } else {
                goToNext();
            }

            emit swipeDetected(distance > 0 ? Qt::SwipeGesture : Qt::SwipeGesture, velocity);
        }

        // Resume auto-play if it was paused
        if (m_autoPlayPaused) {
            QTimer::singleShot(1000, this, &FluentCarousel::resumeAutoPlay);
        }
    }

    FluentComponent::mouseReleaseEvent(event);
}

void FluentCarousel::keyPressEvent(QKeyEvent* event) {
    if (!m_config.enableKeyboard) {
        FluentComponent::keyPressEvent(event);
        return;
    }

    bool handled = true;

    switch (event->key()) {
    case Qt::Key_Left:
        if (m_config.orientation == FluentCarouselOrientation::Horizontal) {
            goToPrevious();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_Right:
        if (m_config.orientation == FluentCarouselOrientation::Horizontal) {
            goToNext();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_Up:
        if (m_config.orientation == FluentCarouselOrientation::Vertical) {
            goToPrevious();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_Down:
        if (m_config.orientation == FluentCarouselOrientation::Vertical) {
            goToNext();
        } else {
            handled = false;
        }
        break;

    case Qt::Key_Home:
        goToFirst();
        break;

    case Qt::Key_End:
        goToLast();
        break;

    case Qt::Key_Space:
        if (m_autoPlayTimer->isActive()) {
            pauseAutoPlay();
        } else if (m_autoPlayPaused) {
            resumeAutoPlay();
        }
        break;

    default:
        handled = false;
        break;
    }

    if (!handled) {
        FluentComponent::keyPressEvent(event);
    }
}

void FluentCarousel::wheelEvent(QWheelEvent* event) {
    if (!m_config.enableWheel) {
        FluentComponent::wheelEvent(event);
        return;
    }

    QPoint delta = event->angleDelta();
    int steps = 0;

    if (m_config.orientation == FluentCarouselOrientation::Horizontal) {
        steps = delta.x() != 0 ? delta.x() : delta.y();
    } else {
        steps = delta.y();
    }

    if (std::abs(steps) >= 120) { // Standard wheel step
        if (steps > 0) {
            goToPrevious();
        } else {
            goToNext();
        }
    }

    event->accept();
}

void FluentCarousel::focusInEvent(QFocusEvent* event) {
    FluentComponent::focusInEvent(event);

    if (m_config.pauseOnFocus && m_autoPlayTimer->isActive()) {
        pauseAutoPlay();
    }

    updateAccessibilityInfo();
}

void FluentCarousel::focusOutEvent(QFocusEvent* event) {
    FluentComponent::focusOutEvent(event);

    if (m_config.pauseOnFocus && m_autoPlayPaused) {
        QTimer::singleShot(500, this, &FluentCarousel::resumeAutoPlay);
    }
}

void FluentCarousel::enterEvent(QEnterEvent* event) {
    FluentComponent::enterEvent(event);

    if (m_config.pauseOnHover && m_autoPlayTimer->isActive()) {
        pauseAutoPlay();
    }
}

void FluentCarousel::leaveEvent(QEvent* event) {
    FluentComponent::leaveEvent(event);

    if (m_config.pauseOnHover && m_autoPlayPaused) {
        QTimer::singleShot(500, this, &FluentCarousel::resumeAutoPlay);
    }
}

void FluentCarousel::changeEvent(QEvent* event) {
    FluentComponent::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        if (!isEnabled()) {
            stopAutoPlay();
        } else if (m_config.autoPlay != FluentCarouselAutoPlay::None) {
            startAutoPlay();
        }
    }
}

bool FluentCarousel::event(QEvent* event) {
    switch (event->type()) {
    case QEvent::Gesture:
        return gestureEvent(static_cast<QGestureEvent*>(event));
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        touchEvent(static_cast<QTouchEvent*>(event));
        return true;
    default:
        return FluentComponent::event(event);
    }
}

bool FluentCarousel::gestureEvent(QGestureEvent* event) {
    if (!m_config.enableTouch) {
        return false;
    }

    if (QGesture* pan = event->gesture(Qt::PanGesture)) {
        handlePanGesture(static_cast<QPanGesture*>(pan));
    }

    if (QGesture* swipe = event->gesture(Qt::SwipeGesture)) {
        handleSwipeGesture(static_cast<QSwipeGesture*>(swipe));
    }

    return true;
}

void FluentCarousel::touchEvent(QTouchEvent* event) {
    if (!m_config.enableTouch) {
        return;
    }

    const QList<QTouchEvent::TouchPoint>& touchPoints = event->points();
    if (touchPoints.isEmpty()) {
        return;
    }

    processTouchPoints(touchPoints);
}

// Private slot implementations
void FluentCarousel::onAutoPlayTimer() {
    if (m_items.empty()) {
        return;
    }

    switch (m_config.autoPlay) {
    case FluentCarouselAutoPlay::Forward:
        goToNext();
        break;
    case FluentCarouselAutoPlay::Backward:
        goToPrevious();
        break;
    case FluentCarouselAutoPlay::PingPong:
        // Implementation for ping-pong would go here
        goToNext();
        break;
    default:
        break;
    }
}

void FluentCarousel::onTransitionAnimationValueChanged(const QVariant& value) {
    m_transitionProgress = value.toReal();
    emit transitionProgressChanged(m_transitionProgress);
    update();
}

void FluentCarousel::onTransitionAnimationFinished() {
    m_transitioning = false;
    m_transitionProgress = 0.0;

    emit transitionFinished(m_currentIndex);
    emit transitionProgressChanged(0.0);

    updateAccessibilityInfo();
    if (m_config.announceChanges) {
        announceCurrentItem();
    }
}

void FluentCarousel::onItemClicked() {
    // Handle item click - could be implemented by subclasses
    emit itemClicked(m_currentIndex);
}

void FluentCarousel::onNavigationClicked(int direction) {
    if (direction < 0) {
        goToPrevious();
    } else if (direction > 0) {
        goToNext();
    }

    // Navigation button clicked - signal would be defined in subclasses
}

void FluentCarousel::onIndicatorClicked(int index) {
    setCurrentIndex(index);
    // Indicator clicked - signal would be defined in subclasses
}

void FluentCarousel::onThemeChanged() {
    update();
}

// Private helper methods
void FluentCarousel::updateLayout() {
    if (!m_mainLayout) return;

    // Update layout direction based on orientation
    if (m_config.orientation == FluentCarouselOrientation::Vertical) {
        m_mainLayout->setDirection(QBoxLayout::TopToBottom);
    } else {
        m_mainLayout->setDirection(QBoxLayout::LeftToRight);
    }

    // Update margins
    m_mainLayout->setContentsMargins(m_config.contentMargins);

    // Update spacing
    m_mainLayout->setSpacing(m_config.itemSpacing);
}

void FluentCarousel::updateItemPositions() {
    // This method would handle positioning for non-stacked layouts
    // For now, we use QStackedWidget which handles positioning automatically
}

void FluentCarousel::updateNavigationVisibility() {
    // This would be implemented by subclasses that have navigation controls
}

void FluentCarousel::updateIndicatorVisibility() {
    // This would be implemented by subclasses that have indicators
}

void FluentCarousel::startTransition(int fromIndex, int toIndex, bool animated) {
    if (m_transitioning || fromIndex == toIndex) {
        return;
    }

    m_transitioning = animated;

    if (animated) {
        createTransitionAnimation(fromIndex, toIndex);
        emit transitionStarted(fromIndex, toIndex);
        m_transitionAnimation->start();
    } else {
        m_currentIndex = toIndex;
        m_stackedWidget->setCurrentIndex(toIndex);
        emit currentIndexChanged(toIndex);
    }
}

void FluentCarousel::createTransitionAnimation(int fromIndex, int toIndex) {
    Q_UNUSED(fromIndex)

    m_currentIndex = toIndex;
    m_stackedWidget->setCurrentIndex(toIndex);

    // Simple fade transition for now
    m_transitionAnimation->setStartValue(0.0);
    m_transitionAnimation->setEndValue(1.0);
}

void FluentCarousel::updateTransitionProgress(qreal progress) {
    m_transitionProgress = progress;
    emit transitionProgressChanged(progress);
}

void FluentCarousel::handlePanGesture(QPanGesture* gesture) {
    // Basic pan gesture handling
    QPointF delta = gesture->delta();
    qreal distance = (m_config.orientation == FluentCarouselOrientation::Horizontal)
                    ? delta.x() : delta.y();

    if (gesture->state() == Qt::GestureFinished) {
        if (std::abs(distance) > 50) { // Threshold for pan
            if (distance > 0) {
                goToPrevious();
            } else {
                goToNext();
            }
        }
    }
}

void FluentCarousel::handleSwipeGesture(QSwipeGesture* gesture) {
    if (gesture->state() != Qt::GestureFinished) {
        return;
    }

    QSwipeGesture::SwipeDirection direction = gesture->horizontalDirection();
    if (m_config.orientation == FluentCarouselOrientation::Vertical) {
        direction = gesture->verticalDirection();
    }

    switch (direction) {
    case QSwipeGesture::Left:
    case QSwipeGesture::Up:
        goToNext();
        break;
    case QSwipeGesture::Right:
    case QSwipeGesture::Down:
        goToPrevious();
        break;
    default:
        break;
    }

    emit swipeDetected(Qt::SwipeGesture, 0.0);
}

void FluentCarousel::processTouchPoints(const QList<QTouchEvent::TouchPoint>& touchPoints) {
    // Basic touch processing - could be enhanced for multi-touch
    if (touchPoints.size() == 1) {
        const QTouchEvent::TouchPoint& point = touchPoints.first();

        switch (point.state()) {
        case QEventPoint::Pressed:
            m_touchStartPos = point.position();
            m_touchStartTime = std::chrono::steady_clock::now();
            break;
        case QEventPoint::Updated:
            m_touchCurrentPos = point.position();
            break;
        case QEventPoint::Released: {
            QPointF delta = m_touchCurrentPos - m_touchStartPos;
            qreal distance = (m_config.orientation == FluentCarouselOrientation::Horizontal)
                            ? delta.x() : delta.y();

            if (std::abs(distance) > 50) {
                if (distance > 0) {
                    goToPrevious();
                } else {
                    goToNext();
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

void FluentCarousel::updateAutoPlayTimer() {
    if (m_config.autoPlay != FluentCarouselAutoPlay::None && !m_items.empty()) {
        m_autoPlayTimer->setInterval(static_cast<int>(m_config.autoPlayInterval.count()));
        if (isVisible() && isEnabled()) {
            startAutoPlay();
        }
    } else {
        stopAutoPlay();
    }
}

void FluentCarousel::resetAutoPlayTimer() {
    if (m_autoPlayTimer->isActive()) {
        m_autoPlayTimer->stop();
        m_autoPlayTimer->start();
    }
}

int FluentCarousel::normalizeIndex(int index) const {
    if (m_items.empty()) return 0;

    int size = static_cast<int>(m_items.size());
    if (m_config.infinite) {
        return ((index % size) + size) % size;
    } else {
        return std::clamp(index, 0, size - 1);
    }
}

bool FluentCarousel::isValidIndex(int index) const {
    return index >= 0 && index < static_cast<int>(m_items.size());
}

QRect FluentCarousel::getItemRect(int index) const {
    Q_UNUSED(index)
    return m_stackedWidget ? m_stackedWidget->rect() : rect();
}

QPoint FluentCarousel::getItemPosition(int index) const {
    Q_UNUSED(index)
    return m_stackedWidget ? m_stackedWidget->pos() : QPoint(0, 0);
}

void FluentCarousel::updateAccessibilityInfo() {
    if (!m_config.enableAccessibility) return;

    QString description = QString("Carousel item %1 of %2")
                         .arg(m_currentIndex + 1)
                         .arg(m_items.size());

    if (isValidIndex(m_currentIndex) && !m_items[m_currentIndex].title.isEmpty()) {
        description += ": " + m_items[m_currentIndex].title;
    }

    setAccessibleDescription(description);
}

void FluentCarousel::announceCurrentItem() {
    if (!m_config.announceChanges || !isValidIndex(m_currentIndex)) return;

    QString announcement = QString("Item %1 of %2")
                          .arg(m_currentIndex + 1)
                          .arg(m_items.size());

    if (!m_items[m_currentIndex].title.isEmpty()) {
        announcement += ": " + m_items[m_currentIndex].title;
    }

    // This would integrate with screen reader APIs
    // For now, we just update the accessible description
    setAccessibleDescription(announcement);
}

} // namespace FluentQt::Components
