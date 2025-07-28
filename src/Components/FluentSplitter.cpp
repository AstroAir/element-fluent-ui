// src/Components/FluentSplitter.cpp
#include "FluentQt/Components/FluentSplitter.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QVBoxLayout>

namespace FluentQt::Components {

FluentSplitter::FluentSplitter(QWidget* parent)
    : Core::FluentComponent(parent)
    , m_resizeMode(FluentSplitterResizeMode::Interactive)
    , m_animatedResize(true)
    , m_resizeAnimation(new QPropertyAnimation(this))
    , m_responsiveTimer(new QTimer(this))
{
    setupSplitter();
    setupLayout();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, [this]() { update(); });
}

FluentSplitter::FluentSplitter(Qt::Orientation orientation, QWidget* parent)
    : Core::FluentComponent(parent)
    , m_resizeMode(FluentSplitterResizeMode::Interactive)
    , m_animatedResize(true)
    , m_resizeAnimation(new QPropertyAnimation(this))
    , m_responsiveTimer(new QTimer(this))
{
    setupSplitter();
    m_splitter->setOrientation(orientation);
    setupLayout();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, [this]() { update(); });
}

void FluentSplitter::setupSplitter() {
    // Create the internal QSplitter
    m_splitter = new QSplitter(this);

    // Set up layout to contain the splitter
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_splitter);
    setLayout(layout);
}

void FluentSplitter::setupLayout() {
    // Setup resize timer for smooth animations
    m_responsiveTimer->setSingleShot(true);
    m_responsiveTimer->setInterval(100);
    connect(m_responsiveTimer, &QTimer::timeout, this, &FluentSplitter::checkResponsiveLayout);

    // Setup resize animation
    m_resizeAnimation->setDuration(250);
    m_resizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_resizeAnimation, &QPropertyAnimation::finished, this, &FluentSplitter::onAnimationFinished);
}

void FluentSplitter::setAnimatedResize(bool animated) {
    if (m_animatedResize == animated) return;

    m_animatedResize = animated;
}

// Orientation
Qt::Orientation FluentSplitter::orientation() const {
    return m_splitter ? m_splitter->orientation() : Qt::Horizontal;
}

void FluentSplitter::setOrientation(Qt::Orientation orientation) {
    if (m_splitter) {
        m_splitter->setOrientation(orientation);
        emit orientationChanged(orientation);
    }
}

// Properties that delegate to internal splitter
bool FluentSplitter::opaqueResize() const {
    return m_splitter ? m_splitter->opaqueResize() : true;
}

void FluentSplitter::setOpaqueResize(bool opaque) {
    if (m_splitter) {
        m_splitter->setOpaqueResize(opaque);
    }
}

bool FluentSplitter::collapsible() const {
    return m_splitter ? m_splitter->childrenCollapsible() : true;
}

void FluentSplitter::setCollapsible(bool collapsible) {
    if (m_splitter) {
        m_splitter->setChildrenCollapsible(collapsible);
    }
}

int FluentSplitter::handleWidth() const {
    return m_splitter ? m_splitter->handleWidth() : 8;
}

void FluentSplitter::setHandleWidth(int width) {
    if (m_splitter) {
        m_splitter->setHandleWidth(width);
    }
}

// Widget management
void FluentSplitter::addWidget(QWidget* widget) {
    if (m_splitter) {
        m_splitter->addWidget(widget);
        updateHandles();
    }
}

void FluentSplitter::insertWidget(int index, QWidget* widget) {
    if (m_splitter) {
        m_splitter->insertWidget(index, widget);
        updateHandles();
    }
}

// More widget management methods
void FluentSplitter::removeWidget(QWidget* widget) {
    if (m_splitter) {
        // QSplitter doesn't have removeWidget, we need to use widget->setParent(nullptr)
        widget->setParent(nullptr);
        updateHandles();
    }
}

QWidget* FluentSplitter::widget(int index) const {
    return m_splitter ? m_splitter->widget(index) : nullptr;
}

int FluentSplitter::count() const {
    return m_splitter ? m_splitter->count() : 0;
}

int FluentSplitter::indexOf(QWidget* widget) const {
    return m_splitter ? m_splitter->indexOf(widget) : -1;
}

// Sizes
QList<int> FluentSplitter::sizes() const {
    return m_splitter ? m_splitter->sizes() : QList<int>();
}

void FluentSplitter::setSizes(const QList<int>& sizes) {
    if (m_splitter) {
        m_splitter->setSizes(sizes);
    }
}

void FluentSplitter::setSizes(const QList<int>& sizes, bool animated) {
    if (animated && m_animatedResize) {
        animateToSizes(sizes);
    } else {
        setSizes(sizes);
    }
}

void FluentSplitter::animateToSizes(const QList<int>& sizes) {
    if (!m_animatedResize || sizes.size() != count()) {
        setSizes(sizes);
        return;
    }

    // Create animation for smooth size transition
    QList<int> currentSizes = this->sizes();

    // For now, just set sizes directly
    // TODO: Implement smooth animation between sizes
    setSizes(sizes);
}

void FluentSplitter::collapseWidget(int index) {
    if (index < 0 || index >= count()) return;
    
    QList<int> sizes = this->sizes();
    if (index < sizes.size()) {
        sizes[index] = 0;
        animateToSizes(sizes);
    }
}

void FluentSplitter::expandWidget(int index) {
    if (index < 0 || index >= count()) return;

    QList<int> sizes = this->sizes();
    if (index < sizes.size() && sizes[index] == 0) {
        // Calculate reasonable default size
        int size = (orientation() == Qt::Horizontal ? width() : height()) / count();
        sizes[index] = size;
        animateToSizes(sizes);
    }
}

void FluentSplitter::toggleWidget(int index) {
    if (isWidgetCollapsed(index)) {
        expandWidget(index);
    } else {
        collapseWidget(index);
    }
}

bool FluentSplitter::isWidgetCollapsed(int index) const {
    if (index < 0 || index >= count()) return false;
    
    QList<int> sizes = this->sizes();
    return index < sizes.size() && sizes[index] == 0;
}

FluentSplitterHandle* FluentSplitter::createHandle() {
    return new FluentSplitterHandle(orientation(), m_splitter);
}

void FluentSplitter::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);

    if (m_responsiveTimer) {
        m_responsiveTimer->start();
    }
}

// Implement required methods
void FluentSplitter::updateHandles() {
    // Update all handles with current styling
    // This is called when widgets are added/removed
    // For now, just ensure the splitter is properly updated
    if (m_splitter) {
        m_splitter->update();
    }
}

// Slot implementations
void FluentSplitter::onHandleMoved(int delta) {
    Q_UNUSED(delta)
    // Handle moved - could implement custom behavior here
}

void FluentSplitter::onHandlePressed() {
    // Handle pressed - could implement custom behavior here
}

void FluentSplitter::onHandleReleased() {
    // Handle released - could implement custom behavior here
}

void FluentSplitter::onHandleDoubleClicked() {
    // Handle double clicked - could implement custom behavior here
}

void FluentSplitter::onCollapseRequested() {
    // Collapse requested - could implement custom behavior here
}

void FluentSplitter::onAnimationFinished() {
    // Animation finished - could implement custom behavior here
}

void FluentSplitter::checkResponsiveLayout() {
    // Check if responsive layout needs to be updated
    // For now, just a placeholder
}

// Additional required methods
FluentSplitterHandle* FluentSplitter::handleAt(int index) const {
    // For now, return nullptr - would need to track custom handles
    Q_UNUSED(index)
    return nullptr;
}

int FluentSplitter::handleIndex(FluentSplitterHandle* handle) const {
    // For now, return -1 - would need to track custom handles
    Q_UNUSED(handle)
    return -1;
}

QList<int> FluentSplitter::calculateSizes(const QList<double>& proportions) const {
    if (!m_splitter || proportions.isEmpty()) return QList<int>();

    int totalSize = (orientation() == Qt::Horizontal) ? width() : height();
    QList<int> sizes;

    for (double proportion : proportions) {
        sizes.append(static_cast<int>(totalSize * proportion));
    }

    return sizes;
}

QList<double> FluentSplitter::calculateProportions(const QList<int>& sizes) const {
    if (sizes.isEmpty()) return QList<double>();

    int totalSize = 0;
    for (int size : sizes) {
        totalSize += size;
    }

    if (totalSize == 0) return QList<double>();

    QList<double> proportions;
    for (int size : sizes) {
        proportions.append(static_cast<double>(size) / totalSize);
    }

    return proportions;
}

// FluentSplitterHandle implementation
FluentSplitterHandle::FluentSplitterHandle(Qt::Orientation orientation, QSplitter* parent)
    : QSplitterHandle(orientation, parent)
    , m_orientation(orientation)
    , m_hovered(false)
    , m_pressed(false)
{
    setAttribute(Qt::WA_Hover, true);
}

void FluentSplitterHandle::enterEvent(QEnterEvent* event) {
    QSplitterHandle::enterEvent(event);
    m_hovered = true;
    update();
}

void FluentSplitterHandle::leaveEvent(QEvent* event) {
    QSplitterHandle::leaveEvent(event);
    m_hovered = false;
    update();
}

void FluentSplitterHandle::mousePressEvent(QMouseEvent* event) {
    QSplitterHandle::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        m_pressPos = event->pos();
        update();
        emit handlePressed();
    }
}

void FluentSplitterHandle::mouseReleaseEvent(QMouseEvent* event) {
    QSplitterHandle::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        m_pressed = false;
        update();
        emit handleReleased();
    }
}

void FluentSplitterHandle::mouseMoveEvent(QMouseEvent* event) {
    QSplitterHandle::mouseMoveEvent(event);
    if (m_pressed) {
        QPoint delta = event->pos() - m_pressPos;
        int deltaValue = (orientation() == Qt::Horizontal) ? delta.x() : delta.y();
        emit handleMoved(deltaValue);
    }
}

void FluentSplitterHandle::mouseDoubleClickEvent(QMouseEvent* event) {
    QSplitterHandle::mouseDoubleClickEvent(event);
    if (event->button() == Qt::LeftButton) {
        emit handleDoubleClicked();
        if (m_collapsible) {
            emit collapseRequested();
        }
    }
}

void FluentSplitterHandle::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    
    QColor handleColor = palette.neutralQuaternaryAlt;
    if (m_pressed) {
        handleColor = palette.accent;
    } else if (m_hovered) {
        handleColor = palette.accent.lighter(150);
    }
    
    // Draw handle
    QRect handleRect = rect();
    if (orientation() == Qt::Horizontal) {
        // Vertical handle for horizontal splitter
        int centerX = handleRect.center().x();
        handleRect = QRect(centerX - 1, handleRect.top() + 4, 2, handleRect.height() - 8);
    } else {
        // Horizontal handle for vertical splitter
        int centerY = handleRect.center().y();
        handleRect = QRect(handleRect.left() + 4, centerY - 1, handleRect.width() - 8, 2);
    }
    
    painter.fillRect(handleRect, handleColor);
    
    // Draw grip dots for visual feedback
    if (m_hovered || m_pressed) {
        painter.setBrush(handleColor.darker(120));
        painter.setPen(Qt::NoPen);
        
        QPoint center = rect().center();
        int dotSize = 2;
        int spacing = 6;
        
        if (orientation() == Qt::Horizontal) {
            // Vertical dots
            for (int i = -1; i <= 1; ++i) {
                QPoint dotPos(center.x(), center.y() + i * spacing);
                painter.drawEllipse(dotPos, dotSize, dotSize);
            }
        } else {
            // Horizontal dots
            for (int i = -1; i <= 1; ++i) {
                QPoint dotPos(center.x() + i * spacing, center.y());
                painter.drawEllipse(dotPos, dotSize, dotSize);
            }
        }
    }
}

QSize FluentSplitterHandle::sizeHint() const {
    QSize size = QSplitterHandle::sizeHint();
    if (orientation() == Qt::Horizontal) {
        size.setWidth(m_handleWidth);
    } else {
        size.setHeight(m_handleWidth);
    }
    return size;
}

void FluentSplitterHandle::setOrientation(Qt::Orientation orientation) {
    if (m_orientation != orientation) {
        m_orientation = orientation;
        update();
    }
}

void FluentSplitterHandle::setCollapsible(bool collapsible) {
    if (m_collapsible != collapsible) {
        m_collapsible = collapsible;
        update();
    }
}

void FluentSplitterHandle::setHandleWidth(int width) {
    if (m_handleWidth != width) {
        m_handleWidth = width;
        updateGeometry();
        update();
    }
}

// Additional FluentSplitter methods
void FluentSplitter::distributeEvenly() {
    distributeEvenly(true);
}

void FluentSplitter::distributeEvenly(bool animated) {
    if (!m_splitter) return;

    QList<int> sizes;
    int totalSize = (orientation() == Qt::Horizontal) ? width() : height();
    int widgetCount = m_splitter->count();

    if (widgetCount == 0) return;

    int sizePerWidget = totalSize / widgetCount;
    for (int i = 0; i < widgetCount; ++i) {
        sizes.append(sizePerWidget);
    }

    if (animated && m_animatedResize) {
        // TODO: Implement animated resize
        m_splitter->setSizes(sizes);
    } else {
        m_splitter->setSizes(sizes);
    }
}

void FluentSplitter::resetToDefaults() {
    if (!m_splitter) return;

    // Reset to default proportions
    distributeEvenly(false);

    // Reset other properties
    m_resizeMode = FluentSplitterResizeMode::Interactive;
    m_animatedResize = false;
}

void FluentSplitter::refresh() {
    if (!m_splitter) return;

    updateHandles();
    update();
}

void FluentSplitter::setResizeMode(FluentSplitterResizeMode mode) {
    if (m_resizeMode != mode) {
        m_resizeMode = mode;
        // Apply resize mode logic here if needed
        update();
    }
}

// Event handlers
void FluentSplitter::childEvent(QChildEvent* event) {
    Core::FluentComponent::childEvent(event);
    if (m_splitter && event->type() == QEvent::ChildAdded) {
        updateHandles();
    }
}

void FluentSplitter::mousePressEvent(QMouseEvent* event) {
    Core::FluentComponent::mousePressEvent(event);
    // Delegate to internal splitter if needed
}

void FluentSplitter::mouseReleaseEvent(QMouseEvent* event) {
    Core::FluentComponent::mouseReleaseEvent(event);
    // Delegate to internal splitter if needed
}

void FluentSplitter::mouseMoveEvent(QMouseEvent* event) {
    Core::FluentComponent::mouseMoveEvent(event);
    // Delegate to internal splitter if needed
}

void FluentSplitter::paintEvent(QPaintEvent* event) {
    Core::FluentComponent::paintEvent(event);
    // Additional custom painting if needed
}

} // namespace FluentQt::Components
