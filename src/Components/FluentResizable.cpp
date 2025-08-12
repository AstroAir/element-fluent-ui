// src/Components/FluentResizable.cpp
#include "FluentQt/Components/FluentResizable.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
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
        setFixedSize(8, 8);
        setAttribute(Qt::WA_Hover, true);
        setMouseTracking(true);
    }

    FluentResizeHandle handleType() const { return m_handleType; }

signals:
    void hoverChanged(bool hovered);

protected:
    void enterEvent(QEnterEvent* event) override {
        Q_UNUSED(event)
        m_hovered = true;
        emit hoverChanged(true);
        update();
    }

    void leaveEvent(QEvent* event) override {
        Q_UNUSED(event)
        m_hovered = false;
        emit hoverChanged(false);
        update();
    }

    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect rect = this->rect();
        const QColor handleColor =
            m_hovered ? QColor(0, 120, 215) : QColor(100, 100, 100);

        painter.setBrush(handleColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect);
    }

private:
    FluentResizeHandle m_handleType;
    bool m_hovered{false};
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
        const QColor previewColor = QColor(0, 120, 215, 80);

        painter.setBrush(previewColor);
        painter.setPen(QPen(QColor(0, 120, 215), 2, Qt::DashLine));
        painter.drawRect(rect.adjusted(1, 1, -1, -1));
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
        connect(m_handles[i].get(), &FluentResizeHandleWidget::hoverChanged,
                this, &FluentResizable::onHandleHoverChanged);
    }

    updateHandlePositions();
    updateHandleVisibility();
}

void FluentResizable::setupAnimations() {
    m_resizeAnimation->setDuration(200);
    m_resizeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_resizeAnimation.get(), &QPropertyAnimation::valueChanged, this,
            &FluentResizable::onResizeAnimationValueChanged);
    connect(m_resizeAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentResizable::onResizeAnimationFinished);
}

void FluentResizable::setupAccessibility() {
    setAccessibleName("Resizable Container");
    setAccessibleDescription(
        "A resizable container that can be adjusted by dragging the resize "
        "handles");

    // Set focus policy to enable keyboard navigation
    setFocusPolicy(Qt::StrongFocus);
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

    const int step = event->modifiers() & Qt::ShiftModifier ? 10 : 1;
    QSize newSize = size();

    switch (event->key()) {
        case Qt::Key_Left:
            newSize.setWidth(
                qMax(m_minimumSize.width(), newSize.width() - step));
            break;
        case Qt::Key_Right:
            newSize.setWidth(
                qMin(m_maximumSize.width(), newSize.width() + step));
            break;
        case Qt::Key_Up:
            newSize.setHeight(
                qMax(m_minimumSize.height(), newSize.height() - step));
            break;
        case Qt::Key_Down:
            newSize.setHeight(
                qMin(m_maximumSize.height(), newSize.height() + step));
            break;
        case Qt::Key_Escape:
            if (m_resizing) {
                cancelResize();
            }
            break;
        default:
            Core::FluentComponent::keyPressEvent(event);
            return;
    }

    if (newSize != size()) {
        resizeToSize(newSize, false);
        event->accept();
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

void FluentResizable::onThemeChanged() { updateStateStyle(); }

// Handle management methods
void FluentResizable::updateHandlePositions() {
    if (!m_showHandles) {
        return;
    }

    const QRect rect = this->rect();
    const int handleSize = m_handleSize;
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
            m_handles[i]->setVisible(visible && handleEnabled);
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

    m_resizeAnimation->setStartValue(geometry());
    m_resizeAnimation->setEndValue(QRect(pos(), targetSize));
    m_resizeAnimation->start();
}

void FluentResizable::updateResizePreview(const QSize& previewSize) {
    if (m_resizePreview) {
        m_resizePreview->showPreview(QRect(pos(), previewSize));
    }
}

// Painting methods
void FluentResizable::paintBackground(QPainter& painter, const QRect& rect) {
    const QColor backgroundColor = getBackgroundColor();
    painter.fillRect(rect, backgroundColor);
}

void FluentResizable::paintBorder(QPainter& painter, const QRect& rect) {
    const QPen borderPen = getBorderPen();
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect.adjusted(0, 0, -1, -1));
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

// Utility methods
void FluentResizable::updateContentGeometry() {
    if (!m_contentWidget) {
        return;
    }

    const QRect rect = this->rect();
    const int margin = m_showHandles ? m_handleSize / 2 : 0;
    const QRect contentRect = rect.adjusted(margin, margin, -margin, -margin);

    m_contentWidget->setGeometry(contentRect);
}

}  // namespace FluentQt::Components

#include "FluentResizable.moc"
