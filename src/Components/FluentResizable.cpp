// src/Components/FluentResizable.cpp
#include "FluentQt/Components/FluentResizable.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTouchEvent>
#include <QVBoxLayout>
#include <QtMath>

namespace FluentQt::Components {

// Helper class for individual resize handles
class FluentResizeHandleWidget : public QWidget {
    Q_OBJECT

public:
    explicit FluentResizeHandleWidget(FluentResizeHandle handleType,
                                      QWidget* parent = nullptr)
        : QWidget(parent), m_handleType(handleType) {
        setFixedSize(10, 10);  // Slightly larger for better touch targets
        setAttribute(Qt::WA_Hover, true);
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);  // Enable keyboard focus

        // Initialize animations for smooth transitions using FluentUI
        // specifications
        m_hoverAnimation =
            std::make_unique<QPropertyAnimation>(this, "opacity");
        m_hoverAnimation->setDuration(
            150);  // FluentUI secondary motion duration
        m_hoverAnimation->setEasingCurve(
            QEasingCurve::OutCubic);  // FluentUI standard easing

        m_pressAnimation =
            std::make_unique<QPropertyAnimation>(this, "geometry");
        m_pressAnimation->setDuration(100);  // FluentUI utility motion duration
        m_pressAnimation->setEasingCurve(
            QEasingCurve::OutCubic);  // FluentUI standard easing
    }

    FluentResizeHandle handleType() const { return m_handleType; }

signals:
    void hoverChanged(bool hovered);
    void pressedChanged(bool pressed);
    void focusChanged(bool focused);

protected:
    void enterEvent(QEnterEvent* event) override {
        Q_UNUSED(event)
        if (!m_hovered) {
            m_hovered = true;
            startHoverAnimation(true);
            emit hoverChanged(true);
            update();
        }
    }

    void leaveEvent(QEvent* event) override {
        Q_UNUSED(event)
        if (m_hovered) {
            m_hovered = false;
            startHoverAnimation(false);
            emit hoverChanged(false);
            update();
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            m_pressed = true;
            startPressAnimation(true);
            emit pressedChanged(true);
            update();
        }
        QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton && m_pressed) {
            m_pressed = false;
            startPressAnimation(false);
            emit pressedChanged(false);
            update();
        }
        QWidget::mouseReleaseEvent(event);
    }

    void focusInEvent(QFocusEvent* event) override {
        m_focused = true;
        emit focusChanged(true);
        update();
        QWidget::focusInEvent(event);
    }

    void focusOutEvent(QFocusEvent* event) override {
        m_focused = false;
        emit focusChanged(false);
        update();
        QWidget::focusOutEvent(event);
    }

    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect rect = this->rect();

        // Use FluentUI design for handles - rounded rectangles instead of
        // circles
        const int cornerRadius = 2;  // Small corner radius for handles
        QPainterPath path;
        path.addRoundedRect(rect, cornerRadius, cornerRadius);

        // Get proper FluentUI colors based on state
        const auto& theme = FluentQt::Styling::FluentTheme::instance();
        QColor handleColor;
        QColor borderColor;

        if (m_pressed) {
            handleColor = theme.color("controlFillColorTertiary");
            borderColor = theme.color("controlStrokeColorDefault");
        } else if (m_focused) {
            handleColor = theme.color("controlFillColorSecondary");
            borderColor = theme.color("accentFillColorDefault");
        } else if (m_hovered) {
            handleColor = theme.color("controlFillColorSecondary");
            borderColor = theme.color("controlStrokeColorSecondary");
        } else {
            handleColor = theme.color("controlFillColorDefault");
            borderColor = theme.color("controlStrokeColorDefault");
        }

        // Apply border for visual definition
        painter.setPen(QPen(borderColor, 1));

        painter.setBrush(handleColor);
        painter.drawPath(path);
    }

private:
    void startHoverAnimation(bool hovering) {
        if (!m_hoverAnimation)
            return;

        m_hoverAnimation->stop();
        if (hovering) {
            m_hoverAnimation->setStartValue(1.0);
            m_hoverAnimation->setEndValue(0.9);  // Slight scale down on hover
        } else {
            m_hoverAnimation->setStartValue(0.9);
            m_hoverAnimation->setEndValue(1.0);
        }
        m_hoverAnimation->start();
    }

    void startPressAnimation(bool pressing) {
        if (!m_pressAnimation)
            return;

        const QRect currentGeometry = geometry();
        m_pressAnimation->stop();

        if (pressing) {
            // Slight shrink on press
            const QRect pressedGeometry =
                currentGeometry.adjusted(1, 1, -1, -1);
            m_pressAnimation->setStartValue(currentGeometry);
            m_pressAnimation->setEndValue(pressedGeometry);
        } else {
            // Return to normal size
            const QRect normalGeometry = currentGeometry.adjusted(-1, -1, 1, 1);
            m_pressAnimation->setStartValue(currentGeometry);
            m_pressAnimation->setEndValue(normalGeometry);
        }
        m_pressAnimation->start();
    }

    FluentResizeHandle m_handleType;
    bool m_hovered{false};
    bool m_pressed{false};
    bool m_focused{false};

    // Animation support
    std::unique_ptr<QPropertyAnimation> m_hoverAnimation;
    std::unique_ptr<QPropertyAnimation> m_pressAnimation;
};

// Helper class for resize preview
class FluentResizePreview : public QWidget {
public:
    explicit FluentResizePreview(QWidget* parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setVisible(false);
    }

    void showPreview(const QRect& rect) {
        setGeometry(rect);
        setVisible(true);
        update();
    }

    void hidePreview() { setVisible(false); }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect rect = this->rect();

        // Use FluentUI design for preview - rounded corners and proper colors
        const auto& theme = FluentQt::Styling::FluentTheme::instance();
        const QColor accentColor = theme.color("accentFillColorDefault");
        const QColor previewColor =
            QColor(accentColor.red(), accentColor.green(), accentColor.blue(),
                   60);  // Semi-transparent

        const int cornerRadius = 4;  // Medium corner radius for preview
        QPainterPath path;
        path.addRoundedRect(rect.adjusted(1, 1, -1, -1), cornerRadius,
                            cornerRadius);

        painter.setBrush(previewColor);
        painter.setPen(QPen(accentColor, 2, Qt::SolidLine));
        painter.drawPath(path);
    }
};

FluentResizable::FluentResizable(QWidget* parent)
    : Core::FluentComponent(parent),
      m_resizeAnimation(std::make_unique<QPropertyAnimation>(this, "geometry")),
      m_resizePreview(std::make_unique<FluentResizePreview>(this)) {
    setupLayout();
    setupHandles();
    setupAnimations();
    setupAccessibility();

    // Connect to theme changes
    connect(&FluentQt::Styling::FluentTheme::instance(),
            &FluentQt::Styling::FluentTheme::themeChanged, this,
            &FluentResizable::onThemeChanged);

    setMinimumSize(m_minimumSize);
    setMaximumSize(m_maximumSize);

    // Initialize visual enhancements
    updateShadowEffect();
}

FluentResizable::FluentResizable(QWidget* contentWidget, QWidget* parent)
    : Core::FluentComponent(parent),
      m_resizeAnimation(std::make_unique<QPropertyAnimation>(this, "geometry")),
      m_resizePreview(std::make_unique<FluentResizePreview>(this)) {
    setupLayout();
    setupHandles();
    setupAnimations();
    setupAccessibility();

    // Connect to theme changes
    connect(&FluentQt::Styling::FluentTheme::instance(),
            &FluentQt::Styling::FluentTheme::themeChanged, this,
            &FluentResizable::onThemeChanged);

    setMinimumSize(m_minimumSize);
    setMaximumSize(m_maximumSize);

    // Initialize visual enhancements
    updateShadowEffect();

    setContentWidget(contentWidget);
}

FluentResizable::~FluentResizable() = default;

void FluentResizable::setContentWidget(QWidget* widget) {
    if (m_contentWidget == widget) {
        return;
    }

    if (m_contentWidget) {
        m_contentWidget->setParent(nullptr);
    }

    m_contentWidget = widget;
    if (m_contentWidget) {
        m_contentWidget->setParent(this);
        updateContentGeometry();
    }

    m_sizeHintValid = false;
    updateGeometry();
}

void FluentResizable::setResizeDirection(FluentResizeDirection direction) {
    if (m_resizeDirection != direction) {
        m_resizeDirection = direction;
        updateHandleVisibility();
        emit resizeDirectionChanged(direction);
    }
}

void FluentResizable::setEnabledHandles(FluentResizeHandles handles) {
    if (m_enabledHandles != handles) {
        m_enabledHandles = handles;
        updateHandleVisibility();
        emit enabledHandlesChanged(handles);
    }
}

void FluentResizable::setResizeMode(FluentResizeMode mode) {
    if (m_resizeMode != mode) {
        m_resizeMode = mode;
        emit resizeModeChanged(mode);
    }
}

void FluentResizable::setHandleSize(int size) {
    if (m_handleSize != size && size > 0) {
        m_handleSize = size;
        updateHandlePositions();
        emit handleSizeChanged(size);
    }
}

void FluentResizable::setShowHandles(bool show) {
    if (m_showHandles != show) {
        m_showHandles = show;
        updateHandleVisibility();
        emit showHandlesChanged(show);
    }
}

void FluentResizable::setMinimumSize(const QSize& size) {
    if (m_minimumSize != size) {
        m_minimumSize = size;
        QWidget::setMinimumSize(size);
        emit minimumSizeChanged(size);
    }
}

void FluentResizable::setMaximumSize(const QSize& size) {
    if (m_maximumSize != size) {
        m_maximumSize = size;
        QWidget::setMaximumSize(size);
        emit maximumSizeChanged(size);
    }
}

void FluentResizable::setSmoothResize(bool smooth) { m_smoothResize = smooth; }

void FluentResizable::setTouchEnabled(bool enabled) {
    m_touchEnabled = enabled;
    setAttribute(Qt::WA_AcceptTouchEvents, enabled);
}

void FluentResizable::setSnapToGrid(bool snap) { m_snapToGrid = snap; }

void FluentResizable::setGridSize(int size) {
    if (size > 0) {
        m_gridSize = size;
    }
}

void FluentResizable::setAspectRatio(qreal ratio) {
    m_aspectRatio = qMax(0.0, ratio);
}

void FluentResizable::setMaintainAspectRatio(bool maintain) {
    m_maintainAspectRatio = maintain;
}

void FluentResizable::setEnableShadows(bool enable) {
    if (m_enableShadows != enable) {
        m_enableShadows = enable;
        updateShadowEffect();
        emit enableShadowsChanged(enable);
    }
}

void FluentResizable::setElevationLevel(const QString& level) {
    if (m_elevationLevel != level) {
        m_elevationLevel = level;
        updateShadowEffect();
        emit elevationLevelChanged(level);
    }
}

void FluentResizable::setEnableHandleAnimations(bool enable) {
    if (m_enableHandleAnimations != enable) {
        m_enableHandleAnimations = enable;
        emit enableHandleAnimationsChanged(enable);
    }
}

QSize FluentResizable::sizeHint() const {
    if (!m_sizeHintValid) {
        if (m_contentWidget) {
            m_cachedSizeHint = m_contentWidget->sizeHint() +
                               QSize(m_handleSize * 2, m_handleSize * 2);
        } else {
            m_cachedSizeHint = QSize(200, 150);
        }
        m_sizeHintValid = true;
    }
    return m_cachedSizeHint;
}

QSize FluentResizable::minimumSizeHint() const { return m_minimumSize; }

void FluentResizable::resizeToSize(const QSize& size, bool animated) {
    const QSize constrainedSize = constrainSize(size);
    if (animated && m_smoothResize) {
        animateToSize(constrainedSize);
    } else {
        resize(constrainedSize);
    }
}

void FluentResizable::resizeToWidth(int width, bool animated) {
    resizeToSize(QSize(width, height()), animated);
}

void FluentResizable::resizeToHeight(int height, bool animated) {
    resizeToSize(QSize(width(), height), animated);
}

void FluentResizable::resetToMinimumSize() {
    resizeToSize(m_minimumSize, true);
}

void FluentResizable::resetToMaximumSize() {
    resizeToSize(m_maximumSize, true);
}

void FluentResizable::resetToSizeHint() { resizeToSize(sizeHint(), true); }

void FluentResizable::toggleHandleVisibility() {
    setShowHandles(!m_showHandles);
}

void FluentResizable::setupLayout() {
    // No specific layout needed as we manually position content and handles
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
}

void FluentResizable::setupHandles() {
    // Create handle widgets for each resize direction
    const std::array<FluentResizeHandle, 8> handleTypes = {
        FluentResizeHandle::Top,    FluentResizeHandle::TopRight,
        FluentResizeHandle::Right,  FluentResizeHandle::BottomRight,
        FluentResizeHandle::Bottom, FluentResizeHandle::BottomLeft,
        FluentResizeHandle::Left,   FluentResizeHandle::TopLeft};

    for (size_t i = 0; i < m_handles.size(); ++i) {
        m_handles[i] =
            std::make_unique<FluentResizeHandleWidget>(handleTypes[i], this);

        // Connect to all handle state signals for enhanced interaction
        connect(m_handles[i].get(), &FluentResizeHandleWidget::hoverChanged,
                this, &FluentResizable::onHandleHoverChanged);
        connect(m_handles[i].get(), &FluentResizeHandleWidget::pressedChanged,
                this, &FluentResizable::onHandlePressedChanged);
        connect(m_handles[i].get(), &FluentResizeHandleWidget::focusChanged,
                this, &FluentResizable::onHandleFocusChanged);
    }

    updateHandlePositions();
    updateHandleVisibility();
}

void FluentResizable::setupAnimations() {
    // Use FluentUI standard animation timing
    m_resizeAnimation->setDuration(250);  // FluentUI primary motion duration
    m_resizeAnimation->setEasingCurve(
        QEasingCurve::OutCubic);  // FluentUI standard easing

    connect(m_resizeAnimation.get(), &QPropertyAnimation::valueChanged, this,
            &FluentResizable::onResizeAnimationValueChanged);
    connect(m_resizeAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentResizable::onResizeAnimationFinished);

    // Initialize handle entrance/exit animations
    setupHandleAnimations();
}

void FluentResizable::setupHandleAnimations() {
    // Handle animations are now managed within each FluentResizeHandleWidget
    // This method can be used for coordinated handle animations in the future

    // Set up entrance/exit animations for handle visibility
    // These will be triggered when handles are shown/hidden
}

void FluentResizable::setupAccessibility() {
    setAccessibleName("Resizable Container");
    setAccessibleDescription(
        "A resizable container that can be adjusted by dragging the resize "
        "handles or using keyboard shortcuts. Use arrow keys to resize, "
        "Shift+arrow keys for larger steps, Escape to cancel resize.");

    // Set focus policy to enable keyboard navigation
    setFocusPolicy(Qt::StrongFocus);

    // Set ARIA attributes for better screen reader support
    setProperty("role", "widget");
    setProperty("aria-label", "Resizable container");
    setProperty("aria-describedby", "Use drag handles or arrow keys to resize");

    // Enable high contrast mode detection
    // Note: QApplication doesn't have paletteChanged signal, we'll use theme
    // changes instead

    // Check for reduced motion preferences
    updateMotionPreferences();
}

void FluentResizable::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);

    // Paint grid lines if enabled
    if (m_snapToGrid && (m_resizing || hasFocus())) {
        paintGridLines(painter);
    }

    // Paint resize preview if in preview mode
    if (m_resizeMode == FluentResizeMode::Preview && m_resizing) {
        paintResizePreview(painter);
    }
}

void FluentResizable::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        const FluentResizeHandle handle = getHandleAt(event->pos());
        if (handle != FluentResizeHandle::None) {
            startResize(handle, event->pos());
            event->accept();
            return;
        }
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentResizable::mouseMoveEvent(QMouseEvent* event) {
    if (m_resizing) {
        updateResize(event->pos());
        event->accept();
        return;
    }

    // Update cursor based on handle under mouse
    const FluentResizeHandle handle = getHandleAt(event->pos());
    setCursor(getCursorForHandle(handle));

    Core::FluentComponent::mouseMoveEvent(event);
}

void FluentResizable::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_resizing) {
        finishResize();
        event->accept();
        return;
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentResizable::enterEvent(QEnterEvent* event) {
    if (m_showHandles) {
        updateHandleVisibility();
    }
    Core::FluentComponent::enterEvent(event);
}

void FluentResizable::leaveEvent(QEvent* event) {
    if (!m_resizing) {
        setCursor(Qt::ArrowCursor);
    }
    Core::FluentComponent::leaveEvent(event);
}

void FluentResizable::resizeEvent(QResizeEvent* event) {
    updateContentGeometry();
    updateHandlePositions();
    m_sizeHintValid = false;
    Core::FluentComponent::resizeEvent(event);
}

void FluentResizable::changeEvent(QEvent* event) {
    if (event->type() == QEvent::EnabledChange) {
        updateHandleVisibility();
    }
    Core::FluentComponent::changeEvent(event);
}

bool FluentResizable::event(QEvent* event) {
    if (m_touchEnabled && event->type() == QEvent::TouchBegin) {
        touchEvent(static_cast<QTouchEvent*>(event));
        return true;
    } else if (m_touchEnabled && (event->type() == QEvent::TouchUpdate ||
                                  event->type() == QEvent::TouchEnd)) {
        touchEvent(static_cast<QTouchEvent*>(event));
        return true;
    }

    return Core::FluentComponent::event(event);
}

void FluentResizable::touchEvent(QTouchEvent* event) {
    const QList<QTouchEvent::TouchPoint> touchPoints = event->points();

    if (touchPoints.isEmpty()) {
        return;
    }

    const QTouchEvent::TouchPoint& touchPoint = touchPoints.first();

    switch (event->type()) {
        case QEvent::TouchBegin:
            if (m_activeTouchId == -1) {
                m_activeTouchId = touchPoint.id();
                m_touchStartPos = touchPoint.position();

                const FluentResizeHandle handle =
                    getHandleAt(touchPoint.position().toPoint());
                if (handle != FluentResizeHandle::None) {
                    startResize(handle, touchPoint.position().toPoint());
                }
            }
            break;

        case QEvent::TouchUpdate:
            if (touchPoint.id() == m_activeTouchId && m_resizing) {
                updateResize(touchPoint.position().toPoint());
            }
            break;

        case QEvent::TouchEnd:
            if (touchPoint.id() == m_activeTouchId) {
                if (m_resizing) {
                    finishResize();
                }
                m_activeTouchId = -1;
            }
            break;

        default:
            break;
    }

    event->accept();
}

void FluentResizable::keyPressEvent(QKeyEvent* event) {
    if (!hasFocus()) {
        Core::FluentComponent::keyPressEvent(event);
        return;
    }

    // Enhanced keyboard navigation with accessibility improvements
    const int step = event->modifiers() & Qt::ShiftModifier ? 10 : 1;
    const int largeStep = event->modifiers() & Qt::ControlModifier ? 50 : step;
    QSize newSize = size();
    bool handled = true;

    switch (event->key()) {
        case Qt::Key_Left:
            newSize.setWidth(
                qMax(m_minimumSize.width(), newSize.width() - largeStep));
            break;
        case Qt::Key_Right:
            newSize.setWidth(
                qMin(m_maximumSize.width(), newSize.width() + largeStep));
            break;
        case Qt::Key_Up:
            newSize.setHeight(
                qMax(m_minimumSize.height(), newSize.height() - largeStep));
            break;
        case Qt::Key_Down:
            newSize.setHeight(
                qMin(m_maximumSize.height(), newSize.height() + largeStep));
            break;
        case Qt::Key_Escape:
            if (m_resizing) {
                cancelResize();
            } else {
                clearFocus();  // Allow focus to move away
            }
            break;
        case Qt::Key_Home:
            // Reset to minimum size
            newSize = m_minimumSize;
            break;
        case Qt::Key_End:
            // Reset to maximum size (or reasonable maximum)
            newSize = QSize(qMin(m_maximumSize.width(), 800),
                            qMin(m_maximumSize.height(), 600));
            break;
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Toggle handle visibility for better accessibility
            toggleHandleVisibility();
            break;
        default:
            handled = false;
            break;
    }

    if (handled) {
        if (newSize != size()) {
            // Announce size change for screen readers
            setAccessibleDescription(QString("Resized to %1 by %2 pixels")
                                         .arg(newSize.width())
                                         .arg(newSize.height()));
            resizeToSize(newSize, m_smoothResize);
        }
        event->accept();
    } else {
        Core::FluentComponent::keyPressEvent(event);
    }
}

void FluentResizable::keyReleaseEvent(QKeyEvent* event) {
    Core::FluentComponent::keyReleaseEvent(event);
}

void FluentResizable::updateStateStyle() {
    // Update visual appearance based on current state
    update();
}

void FluentResizable::performStateTransition(Core::FluentState from,
                                             Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    // Handle state transitions if needed
    updateStateStyle();
}

// Slot implementations
void FluentResizable::onResizeAnimationValueChanged(const QVariant& value) {
    Q_UNUSED(value)
    updateContentGeometry();
    emit resizing(size());
}

void FluentResizable::onResizeAnimationFinished() {
    emit resizeFinished(size());
}

void FluentResizable::onHandleHoverChanged(bool hovered) {
    Q_UNUSED(hovered)
    update();
}

void FluentResizable::onHandlePressedChanged(bool pressed) {
    Q_UNUSED(pressed)
    // Handle press state changes for enhanced visual feedback
    update();
}

void FluentResizable::onHandleFocusChanged(bool focused) {
    Q_UNUSED(focused)
    // Handle focus state changes for accessibility
    update();
}

void FluentResizable::onThemeChanged() {
    updateStateStyle();
    updateShadowEffect();      // Refresh shadow effects when theme changes
    onSystemPaletteChanged();  // Check for high contrast mode changes
}

void FluentResizable::onSystemPaletteChanged() {
    // Check if high contrast mode is enabled and adjust accordingly
    // High contrast mode detection would be implemented here
    // For now, we rely on the theme system to handle this
    update();
}

void FluentResizable::updateMotionPreferences() {
    // Check system preferences for reduced motion
    // This would typically check system accessibility settings
    // Disable animations if reduced motion is preferred
    bool reducedMotion = false;  // Would be set based on system preferences

    if (reducedMotion) {
        m_enableHandleAnimations = false;
        m_smoothResize = false;
        // Disable all animations for accessibility
        if (m_resizeAnimation) {
            m_resizeAnimation->setDuration(0);
        }
    }
}

// Handle management methods
void FluentResizable::updateHandlePositions() {
    if (!m_showHandles) {
        return;
    }

    const QRect rect = this->rect();
    const int handleSize = 10;  // Use fixed size for FluentUI handles
    const int halfHandle = handleSize / 2;

    // Position handles around the perimeter
    if (m_handles[0]) {  // Top
        m_handles[0]->move(rect.center().x() - halfHandle,
                           rect.top() - halfHandle);
    }
    if (m_handles[1]) {  // TopRight
        m_handles[1]->move(rect.right() - halfHandle, rect.top() - halfHandle);
    }
    if (m_handles[2]) {  // Right
        m_handles[2]->move(rect.right() - halfHandle,
                           rect.center().y() - halfHandle);
    }
    if (m_handles[3]) {  // BottomRight
        m_handles[3]->move(rect.right() - halfHandle,
                           rect.bottom() - halfHandle);
    }
    if (m_handles[4]) {  // Bottom
        m_handles[4]->move(rect.center().x() - halfHandle,
                           rect.bottom() - halfHandle);
    }
    if (m_handles[5]) {  // BottomLeft
        m_handles[5]->move(rect.left() - halfHandle,
                           rect.bottom() - halfHandle);
    }
    if (m_handles[6]) {  // Left
        m_handles[6]->move(rect.left() - halfHandle,
                           rect.center().y() - halfHandle);
    }
    if (m_handles[7]) {  // TopLeft
        m_handles[7]->move(rect.left() - halfHandle, rect.top() - halfHandle);
    }
}

void FluentResizable::updateHandleVisibility() {
    const bool visible = m_showHandles && isEnabled();

    for (size_t i = 0; i < m_handles.size(); ++i) {
        if (m_handles[i]) {
            const FluentResizeHandle handleType =
                static_cast<FluentResizeHandle>(1 << i);
            const bool handleEnabled = (m_enabledHandles & handleType) != 0;
            const bool shouldBeVisible = visible && handleEnabled;

            // Add smooth entrance/exit animations for handle visibility
            if (m_enableHandleAnimations &&
                shouldBeVisible != m_handles[i]->isVisible()) {
                animateHandleVisibility(m_handles[i].get(), shouldBeVisible);
            } else {
                m_handles[i]->setVisible(shouldBeVisible);
            }
        }
    }
}

FluentResizeHandle FluentResizable::getHandleAt(const QPoint& pos) const {
    for (size_t i = 0; i < m_handles.size(); ++i) {
        if (m_handles[i] && m_handles[i]->isVisible()) {
            const QRect handleRect =
                getHandleRect(static_cast<FluentResizeHandle>(1 << i));
            if (handleRect.contains(pos)) {
                return static_cast<FluentResizeHandle>(1 << i);
            }
        }
    }
    return FluentResizeHandle::None;
}

QRect FluentResizable::getHandleRect(FluentResizeHandle handle) const {
    const QRect rect = this->rect();
    const int handleSize = m_handleSize;
    const int halfHandle = handleSize / 2;

    switch (handle) {
        case FluentResizeHandle::Top:
            return QRect(rect.center().x() - halfHandle,
                         rect.top() - halfHandle, handleSize, handleSize);
        case FluentResizeHandle::TopRight:
            return QRect(rect.right() - halfHandle, rect.top() - halfHandle,
                         handleSize, handleSize);
        case FluentResizeHandle::Right:
            return QRect(rect.right() - halfHandle,
                         rect.center().y() - halfHandle, handleSize,
                         handleSize);
        case FluentResizeHandle::BottomRight:
            return QRect(rect.right() - halfHandle, rect.bottom() - halfHandle,
                         handleSize, handleSize);
        case FluentResizeHandle::Bottom:
            return QRect(rect.center().x() - halfHandle,
                         rect.bottom() - halfHandle, handleSize, handleSize);
        case FluentResizeHandle::BottomLeft:
            return QRect(rect.left() - halfHandle, rect.bottom() - halfHandle,
                         handleSize, handleSize);
        case FluentResizeHandle::Left:
            return QRect(rect.left() - halfHandle,
                         rect.center().y() - halfHandle, handleSize,
                         handleSize);
        case FluentResizeHandle::TopLeft:
            return QRect(rect.left() - halfHandle, rect.top() - halfHandle,
                         handleSize, handleSize);
        default:
            return QRect();
    }
}

Qt::CursorShape FluentResizable::getCursorForHandle(
    FluentResizeHandle handle) const {
    switch (handle) {
        case FluentResizeHandle::Top:
        case FluentResizeHandle::Bottom:
            return Qt::SizeVerCursor;
        case FluentResizeHandle::Left:
        case FluentResizeHandle::Right:
            return Qt::SizeHorCursor;
        case FluentResizeHandle::TopLeft:
        case FluentResizeHandle::BottomRight:
            return Qt::SizeFDiagCursor;
        case FluentResizeHandle::TopRight:
        case FluentResizeHandle::BottomLeft:
            return Qt::SizeBDiagCursor;
        default:
            return Qt::ArrowCursor;
    }
}

// Resize logic methods
void FluentResizable::startResize(FluentResizeHandle handle,
                                  const QPoint& startPos) {
    m_resizing = true;
    m_activeHandle = handle;
    m_resizeStartPos = startPos;
    m_resizeStartSize = size();
    m_lastMousePos = startPos;

    setCursor(getCursorForHandle(handle));
    emit resizeStarted(m_resizeStartSize);

    if (m_resizeMode == FluentResizeMode::Preview) {
        m_resizePreview->showPreview(geometry());
    }
}

void FluentResizable::updateResize(const QPoint& currentPos) {
    if (!m_resizing) {
        return;
    }

    const QPoint delta = currentPos - m_resizeStartPos;
    const QSize newSize = calculateNewSize(m_activeHandle, delta);
    const QSize constrainedSize = constrainSize(newSize);

    if (m_resizeMode == FluentResizeMode::Immediate) {
        resize(constrainedSize);
        emit resizing(constrainedSize);
    } else if (m_resizeMode == FluentResizeMode::Preview) {
        updateResizePreview(constrainedSize);
    }

    m_lastMousePos = currentPos;
}

void FluentResizable::finishResize() {
    if (!m_resizing) {
        return;
    }

    if (m_resizeMode == FluentResizeMode::Preview) {
        const QPoint delta = m_lastMousePos - m_resizeStartPos;
        const QSize newSize = calculateNewSize(m_activeHandle, delta);
        const QSize constrainedSize = constrainSize(newSize);

        if (m_smoothResize) {
            animateToSize(constrainedSize);
        } else {
            resize(constrainedSize);
        }

        m_resizePreview->hidePreview();
    }

    m_resizing = false;
    m_activeHandle = FluentResizeHandle::None;
    setCursor(Qt::ArrowCursor);

    emit resizeFinished(size());
}

void FluentResizable::cancelResize() {
    if (!m_resizing) {
        return;
    }

    if (m_resizeMode == FluentResizeMode::Preview) {
        m_resizePreview->hidePreview();
    } else {
        // Restore original size
        if (m_smoothResize) {
            animateToSize(m_resizeStartSize);
        } else {
            resize(m_resizeStartSize);
        }
    }

    m_resizing = false;
    m_activeHandle = FluentResizeHandle::None;
    setCursor(Qt::ArrowCursor);

    emit resizeCancelled();
}

QSize FluentResizable::calculateNewSize(FluentResizeHandle handle,
                                        const QPoint& delta) const {
    QSize newSize = m_resizeStartSize;

    // Apply delta based on handle type
    if (handle & FluentResizeHandle::Left) {
        newSize.setWidth(newSize.width() - delta.x());
    } else if (handle & FluentResizeHandle::Right) {
        newSize.setWidth(newSize.width() + delta.x());
    }

    if (handle & FluentResizeHandle::Top) {
        newSize.setHeight(newSize.height() - delta.y());
    } else if (handle & FluentResizeHandle::Bottom) {
        newSize.setHeight(newSize.height() + delta.y());
    }

    // Apply aspect ratio constraint if enabled
    if (m_maintainAspectRatio && m_aspectRatio > 0.0) {
        newSize = maintainAspectRatioSize(newSize, handle);
    }

    // Apply grid snapping if enabled
    if (m_snapToGrid) {
        newSize = snapSizeToGrid(newSize);
    }

    return newSize;
}

QSize FluentResizable::constrainSize(const QSize& size) const {
    QSize constrainedSize = size;

    // Apply minimum size constraints
    constrainedSize = constrainedSize.expandedTo(m_minimumSize);

    // Apply maximum size constraints
    constrainedSize = constrainedSize.boundedTo(m_maximumSize);

    return constrainedSize;
}

QSize FluentResizable::snapSizeToGrid(const QSize& size) const {
    if (m_gridSize <= 1) {
        return size;
    }

    const int snappedWidth =
        qRound(static_cast<qreal>(size.width()) / m_gridSize) * m_gridSize;
    const int snappedHeight =
        qRound(static_cast<qreal>(size.height()) / m_gridSize) * m_gridSize;

    return QSize(snappedWidth, snappedHeight);
}

QSize FluentResizable::maintainAspectRatioSize(
    const QSize& size, FluentResizeHandle handle) const {
    if (m_aspectRatio <= 0.0) {
        return size;
    }

    QSize adjustedSize = size;

    // Determine which dimension to prioritize based on handle
    const bool prioritizeWidth =
        (handle & (FluentResizeHandle::Left | FluentResizeHandle::Right)) != 0;

    if (prioritizeWidth) {
        adjustedSize.setHeight(qRound(adjustedSize.width() / m_aspectRatio));
    } else {
        adjustedSize.setWidth(qRound(adjustedSize.height() * m_aspectRatio));
    }

    return adjustedSize;
}

// Animation methods
void FluentResizable::animateToSize(const QSize& targetSize) {
    if (m_resizeAnimation->state() == QPropertyAnimation::Running) {
        m_resizeAnimation->stop();
    }

    // Use FluentUI primary motion for main content transitions
    m_resizeAnimation->setStartValue(geometry());
    m_resizeAnimation->setEndValue(QRect(pos(), targetSize));

    // Apply FluentUI motion hierarchy - primary motion for main content
    m_resizeAnimation->setDuration(250);  // FluentUI primary motion duration
    m_resizeAnimation->setEasingCurve(
        QEasingCurve::OutCubic);  // FluentUI standard easing

    m_resizeAnimation->start();
}

void FluentResizable::updateResizePreview(const QSize& previewSize) {
    if (m_resizePreview) {
        m_resizePreview->showPreview(QRect(pos(), previewSize));
    }
}

void FluentResizable::animateHandleVisibility(QWidget* handle, bool visible) {
    if (!handle)
        return;

    // Create a fade animation for smooth handle entrance/exit
    auto* fadeAnimation = new QPropertyAnimation(handle, "windowOpacity", this);
    fadeAnimation->setDuration(150);  // FluentUI secondary motion duration
    fadeAnimation->setEasingCurve(
        QEasingCurve::OutCubic);  // FluentUI standard easing

    if (visible) {
        // Fade in animation
        handle->setVisible(true);
        handle->setWindowOpacity(0.0);
        fadeAnimation->setStartValue(0.0);
        fadeAnimation->setEndValue(1.0);
    } else {
        // Fade out animation
        fadeAnimation->setStartValue(1.0);
        fadeAnimation->setEndValue(0.0);

        // Hide the handle when animation finishes
        connect(fadeAnimation, &QPropertyAnimation::finished, [handle]() {
            handle->setVisible(false);
            handle->setWindowOpacity(1.0);  // Reset for next time
        });
    }

    fadeAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}

// Painting methods
void FluentResizable::paintBackground(QPainter& painter, const QRect& rect) {
    const QColor backgroundColor = getBackgroundColor();
    const int cornerRadius = getCornerRadius();

    // Create rounded rectangle path for FluentUI design
    QPainterPath path;
    path.addRoundedRect(rect, cornerRadius, cornerRadius);

    painter.fillPath(path, backgroundColor);
}

void FluentResizable::paintBorder(QPainter& painter, const QRect& rect) {
    const QPen borderPen = getBorderPen();
    const int cornerRadius = getCornerRadius();

    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    // Draw rounded border for FluentUI design
    QPainterPath path;
    path.addRoundedRect(rect.adjusted(0, 0, -1, -1), cornerRadius,
                        cornerRadius);
    painter.drawPath(path);
}

void FluentResizable::paintGridLines(QPainter& painter) {
    if (m_gridSize <= 1) {
        return;
    }

    const QRect rect = this->rect();
    const QColor gridColor = QColor(100, 100, 100, 50);
    painter.setPen(QPen(gridColor, 1, Qt::DotLine));

    // Draw vertical grid lines
    for (int x = m_gridSize; x < rect.width(); x += m_gridSize) {
        painter.drawLine(x, 0, x, rect.height());
    }

    // Draw horizontal grid lines
    for (int y = m_gridSize; y < rect.height(); y += m_gridSize) {
        painter.drawLine(0, y, rect.width(), y);
    }
}

void FluentResizable::paintResizePreview(QPainter& painter) {
    // Preview is handled by the FluentResizePreview widget
    Q_UNUSED(painter)
}

// Style methods
QColor FluentResizable::getBackgroundColor() const {
    const auto& theme = FluentQt::Styling::FluentTheme::instance();
    return theme.color("controlFillColorDefault");
}

QColor FluentResizable::getBorderColor() const {
    const auto& theme = FluentQt::Styling::FluentTheme::instance();

    if (state() == Core::FluentState::Focused) {
        return theme.color("accentFillColorDefault");
    } else if (state() == Core::FluentState::Hovered) {
        return theme.color("controlStrokeColorSecondary");
    }

    return theme.color("controlStrokeColorDefault");
}

QPen FluentResizable::getBorderPen() const {
    const QColor borderColor = getBorderColor();
    return QPen(borderColor, 1);
}

QColor FluentResizable::getHandlePressedColor() const {
    const auto& theme = FluentQt::Styling::FluentTheme::instance();
    return theme.color("controlFillColorTertiary");
}

QColor FluentResizable::getHandleFocusedColor() const {
    const auto& theme = FluentQt::Styling::FluentTheme::instance();
    return theme.color("accentFillColorDefault");
}

QString FluentResizable::getShadowStyle() const {
    const auto& tokenUtils =
        FluentQt::Styling::FluentDesignTokenUtils::instance();
    return tokenUtils.getShadow(m_elevationLevel);
}

int FluentResizable::getCornerRadius() const {
    const auto& tokenUtils =
        FluentQt::Styling::FluentDesignTokenUtils::instance();
    return tokenUtils.getBorderRadius("medium");
}

QGraphicsDropShadowEffect* FluentResizable::createShadowEffect() const {
    if (!m_enableShadows) {
        return nullptr;
    }

    auto* effect = new QGraphicsDropShadowEffect();

    // Use FluentUI shadow specifications
    if (m_elevationLevel == "low") {
        effect->setBlurRadius(4);
        effect->setOffset(0, 1);
        effect->setColor(QColor(0, 0, 0, 28));
    } else if (m_elevationLevel == "medium") {
        effect->setBlurRadius(8);
        effect->setOffset(0, 2);
        effect->setColor(QColor(0, 0, 0, 35));
    } else if (m_elevationLevel == "high") {
        effect->setBlurRadius(16);
        effect->setOffset(0, 4);
        effect->setColor(QColor(0, 0, 0, 42));
    }

    return effect;
}

void FluentResizable::updateShadowEffect() {
    if (m_enableShadows) {
        m_shadowEffect.reset(createShadowEffect());
        setGraphicsEffect(m_shadowEffect.get());
    } else {
        setGraphicsEffect(nullptr);
        m_shadowEffect.reset();
    }
}

// Utility methods
void FluentResizable::updateContentGeometry() {
    if (!m_contentWidget) {
        return;
    }

    const QRect rect = this->rect();
    const int margin =
        m_showHandles ? 5 : 0;  // Use consistent margin for FluentUI
    const QRect contentRect = rect.adjusted(margin, margin, -margin, -margin);

    m_contentWidget->setGeometry(contentRect);

    // Validate geometry constraints for consistency
    validateGeometry();
}

void FluentResizable::validateGeometry() {
    // Ensure the component maintains FluentUI design consistency
    const QSize currentSize = size();

    // Validate minimum size compliance
    if (currentSize.width() < m_minimumSize.width() ||
        currentSize.height() < m_minimumSize.height()) {
        qWarning() << "FluentResizable: Size below minimum constraints";
    }

    // Validate maximum size compliance
    if (currentSize.width() > m_maximumSize.width() ||
        currentSize.height() > m_maximumSize.height()) {
        qWarning() << "FluentResizable: Size exceeds maximum constraints";
    }

    // Validate aspect ratio if maintained
    if (m_maintainAspectRatio && m_aspectRatio > 0.0) {
        const qreal currentRatio =
            static_cast<qreal>(currentSize.width()) / currentSize.height();
        const qreal tolerance = 0.1;  // 10% tolerance
        if (qAbs(currentRatio - m_aspectRatio) > tolerance) {
            qWarning() << "FluentResizable: Aspect ratio deviation detected";
        }
    }
}

}  // namespace FluentQt::Components

#include "FluentResizable.moc"
