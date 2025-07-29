// src/Components/FluentSheet.cpp
#include "FluentQt/Components/FluentSheet.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QAccessible>
#include <QDebug>
#include <QScreen>
#include <QWindow>

namespace FluentQt::Components {

// Overlay widget for modal behavior
class FluentSheetOverlay : public QWidget {
    Q_OBJECT

public:
    explicit FluentSheetOverlay(QWidget* parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setAttribute(Qt::WA_NoSystemBackground, false);
        setAutoFillBackground(true);
        setVisible(false);
    }

    void setOverlayOpacity(qreal opacity) {
        m_opacity = qBound(0.0, opacity, 1.0);
        update();
    }

    qreal overlayOpacity() const { return m_opacity; }

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        
        QPainter painter(this);
        const QColor overlayColor = QColor(0, 0, 0, qRound(m_opacity * 255));
        painter.fillRect(rect(), overlayColor);
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
        QWidget::mousePressEvent(event);
    }

private:
    qreal m_opacity{0.5};
};

// Content container for the sheet
class FluentSheetContent : public QWidget {
    Q_OBJECT

public:
    explicit FluentSheetContent(QWidget* parent = nullptr) : QWidget(parent) {
        setupLayout();
    }

    void setTitle(const QString& title) {
        if (m_titleLabel) {
            m_titleLabel->setText(title);
            m_titleLabel->setVisible(!title.isEmpty());
        }
    }

    QString title() const {
        return m_titleLabel ? m_titleLabel->text() : QString();
    }

    void setSubtitle(const QString& subtitle) {
        if (m_subtitleLabel) {
            m_subtitleLabel->setText(subtitle);
            m_subtitleLabel->setVisible(!subtitle.isEmpty());
        }
    }

    QString subtitle() const {
        return m_subtitleLabel ? m_subtitleLabel->text() : QString();
    }

    void setContentWidget(QWidget* widget) {
        if (m_contentWidget == widget) {
            return;
        }

        if (m_contentWidget) {
            m_contentLayout->removeWidget(m_contentWidget);
            m_contentWidget->setParent(nullptr);
        }

        m_contentWidget = widget;
        if (m_contentWidget) {
            m_contentLayout->addWidget(m_contentWidget);
        }
    }

    QWidget* contentWidget() const {
        return m_contentWidget;
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect rect = this->rect();
        const auto& theme = FluentQt::Styling::FluentTheme::instance();
        const QColor backgroundColor = theme.color("layerFillColorDefault");
        const QColor borderColor = theme.color("controlStrokeColorDefault");

        // Paint background
        painter.fillRect(rect, backgroundColor);

        // Paint border
        painter.setPen(QPen(borderColor, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rect.adjusted(0, 0, -1, -1));
    }

private:
    void setupLayout() {
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(16, 16, 16, 16);
        m_mainLayout->setSpacing(8);

        // Header section
        m_headerLayout = new QVBoxLayout();
        m_headerLayout->setSpacing(4);

        m_titleLabel = new QLabel();
        m_titleLabel->setObjectName("FluentSheetTitle");
        m_titleLabel->setVisible(false);
        m_headerLayout->addWidget(m_titleLabel);

        m_subtitleLabel = new QLabel();
        m_subtitleLabel->setObjectName("FluentSheetSubtitle");
        m_subtitleLabel->setVisible(false);
        m_headerLayout->addWidget(m_subtitleLabel);

        m_mainLayout->addLayout(m_headerLayout);

        // Content section
        m_contentLayout = new QVBoxLayout();
        m_mainLayout->addLayout(m_contentLayout);
    }

private:
    QVBoxLayout* m_mainLayout{nullptr};
    QVBoxLayout* m_headerLayout{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};
    QLabel* m_titleLabel{nullptr};
    QLabel* m_subtitleLabel{nullptr};
    QWidget* m_contentWidget{nullptr};
};

FluentSheet::FluentSheet(QWidget* parent)
    : Core::FluentComponent(parent)
    , m_overlay(std::make_unique<FluentSheetOverlay>(this))
    , m_content(std::make_unique<FluentSheetContent>(this))
    , m_openAnimation(std::make_unique<QPropertyAnimation>(this, "geometry"))
    , m_closeAnimation(std::make_unique<QPropertyAnimation>(this, "geometry"))
    , m_overlayAnimation(std::make_unique<QPropertyAnimation>(m_overlay.get(), "overlayOpacity"))
    , m_animationGroup(std::make_unique<QParallelAnimationGroup>(this))
{
    setupSheet();
    setupOverlay();
    setupContent();
    setupAnimations();
    setupAccessibility();

    // Connect to theme changes
    connect(&FluentQt::Styling::FluentTheme::instance(), &FluentQt::Styling::FluentTheme::themeChanged,
            this, &FluentSheet::onThemeChanged);
}

FluentSheet::FluentSheet(FluentSheetDirection direction, QWidget* parent)
    : FluentSheet(parent)
{
    setDirection(direction);
}

FluentSheet::FluentSheet(FluentSheetDirection direction, FluentSheetSize size, QWidget* parent)
    : FluentSheet(direction, parent)
{
    setSheetSize(size);
}

FluentSheet::~FluentSheet() = default;

void FluentSheet::setDirection(FluentSheetDirection direction) {
    if (m_direction != direction) {
        m_direction = direction;
        updateGeometry();
        emit directionChanged(direction);
    }
}

void FluentSheet::setSheetSize(FluentSheetSize size) {
    if (m_sheetSize != size) {
        m_sheetSize = size;
        updateGeometry();
        emit sheetSizeChanged(size);
    }
}

void FluentSheet::setCustomSize(const QSize& size) {
    if (m_customSize != size) {
        m_customSize = size;
        if (m_sheetSize == FluentSheetSize::Custom) {
            updateGeometry();
        }
    }
}

void FluentSheet::setBehavior(FluentSheetBehavior behavior) {
    if (m_behavior != behavior) {
        m_behavior = behavior;
        updateOverlayGeometry();
        emit behaviorChanged(behavior);
    }
}

void FluentSheet::setOverlayVisible(bool visible) {
    if (m_overlayVisible != visible) {
        m_overlayVisible = visible;
        if (m_isOpen) {
            m_overlay->setVisible(visible);
        }
    }
}

void FluentSheet::setOverlayOpacity(qreal opacity) {
    if (m_overlayOpacity != opacity) {
        m_overlayOpacity = qBound(0.0, opacity, 1.0);
        m_overlay->setOverlayOpacity(m_overlayOpacity);
    }
}

void FluentSheet::setAnimationDuration(int duration) {
    if (m_animationDuration != duration && duration >= 0) {
        m_animationDuration = duration;
        m_openAnimation->setDuration(duration);
        m_closeAnimation->setDuration(duration);
        m_overlayAnimation->setDuration(duration);
    }
}

void FluentSheet::setEasingCurve(QEasingCurve::Type curve) {
    if (m_easingCurve != curve) {
        m_easingCurve = curve;
        m_openAnimation->setEasingCurve(curve);
        m_closeAnimation->setEasingCurve(curve);
        m_overlayAnimation->setEasingCurve(curve);
    }
}

void FluentSheet::setCloseOnOverlayClick(bool close) {
    m_closeOnOverlayClick = close;
}

void FluentSheet::setCloseOnEscape(bool close) {
    m_closeOnEscape = close;
}

void FluentSheet::setDragToClose(bool drag) {
    m_dragToClose = drag;
}

void FluentSheet::setAutoResize(bool resize) {
    if (m_autoResize != resize) {
        m_autoResize = resize;
        if (resize) {
            updateGeometry();
        }
    }
}

void FluentSheet::setContentWidget(QWidget* widget) {
    m_content->setContentWidget(widget);
    m_sizeHintValid = false;
    if (m_autoResize) {
        updateGeometry();
    }
}

QWidget* FluentSheet::contentWidget() const {
    return m_content->contentWidget();
}

void FluentSheet::setTitle(const QString& title) {
    m_content->setTitle(title);
}

QString FluentSheet::title() const {
    return m_content->title();
}

void FluentSheet::setSubtitle(const QString& subtitle) {
    m_content->setSubtitle(subtitle);
}

QString FluentSheet::subtitle() const {
    return m_content->subtitle();
}

bool FluentSheet::isAnimating() const {
    return m_animationGroup->state() == QAbstractAnimation::Running;
}

QSize FluentSheet::sizeHint() const {
    if (!m_sizeHintValid) {
        m_cachedSizeHint = calculateSheetSize();
        m_sizeHintValid = true;
    }
    return m_cachedSizeHint;
}

QSize FluentSheet::minimumSizeHint() const {
    return QSize(200, 150);
}

// Static factory methods
FluentSheet* FluentSheet::createBottomSheet(QWidget* parent) {
    return new FluentSheet(FluentSheetDirection::Bottom, parent);
}

FluentSheet* FluentSheet::createTopSheet(QWidget* parent) {
    return new FluentSheet(FluentSheetDirection::Top, parent);
}

FluentSheet* FluentSheet::createLeftSheet(QWidget* parent) {
    return new FluentSheet(FluentSheetDirection::Left, parent);
}

FluentSheet* FluentSheet::createRightSheet(QWidget* parent) {
    return new FluentSheet(FluentSheetDirection::Right, parent);
}

// Public slots
void FluentSheet::open() {
    if (m_isOpen || isAnimating()) {
        return;
    }

    emit aboutToOpen();

    m_isOpen = true;
    setVisible(true);

    if (m_behavior == FluentSheetBehavior::Modal && m_overlayVisible) {
        showOverlay();
    }

    captureFocus();
    startOpenAnimation();
}

void FluentSheet::close() {
    if (!m_isOpen || isAnimating()) {
        return;
    }

    emit aboutToClose();

    m_isOpen = false;
    restoreFocus();
    startCloseAnimation();
}

void FluentSheet::toggle() {
    if (m_isOpen) {
        close();
    } else {
        open();
    }
}

void FluentSheet::openWithAnimation() {
    open();
}

void FluentSheet::closeWithAnimation() {
    close();
}

// Event handling
void FluentSheet::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);

    // Paint shadow
    paintShadow(painter, rect);
}

void FluentSheet::resizeEvent(QResizeEvent* event) {
    updateContentGeometry();
    updateOverlayGeometry();
    m_sizeHintValid = false;
    Core::FluentComponent::resizeEvent(event);
}

void FluentSheet::showEvent(QShowEvent* event) {
    updateParentGeometry();
    updateGeometry();
    Core::FluentComponent::showEvent(event);
}

void FluentSheet::hideEvent(QHideEvent* event) {
    if (m_overlay) {
        m_overlay->setVisible(false);
    }
    Core::FluentComponent::hideEvent(event);
}

void FluentSheet::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_dragToClose) {
        startDrag(event->pos());
    }
    Core::FluentComponent::mousePressEvent(event);
}

void FluentSheet::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging) {
        updateDrag(event->pos());
    }
    Core::FluentComponent::mouseMoveEvent(event);
}

void FluentSheet::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isDragging) {
        finishDrag();
    }
    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentSheet::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape && m_closeOnEscape) {
        close();
        event->accept();
        return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentSheet::changeEvent(QEvent* event) {
    if (event->type() == QEvent::ParentChange) {
        updateParentGeometry();
    }
    Core::FluentComponent::changeEvent(event);
}

void FluentSheet::focusInEvent(QFocusEvent* event) {
    updateFocusChain();
    Core::FluentComponent::focusInEvent(event);
}

void FluentSheet::focusOutEvent(QFocusEvent* event) {
    // Handle focus loss if needed
    Core::FluentComponent::focusOutEvent(event);
}

void FluentSheet::updateStateStyle() {
    update();
}

void FluentSheet::performStateTransition(Core::FluentState from, Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    updateStateStyle();
}

// Slot implementations
void FluentSheet::onOpenAnimationValueChanged(const QVariant& value) {
    Q_UNUSED(value)
    // Animation is handled by Qt's property system
}

void FluentSheet::onOpenAnimationFinished() {
    emit opened();
}

void FluentSheet::onCloseAnimationValueChanged(const QVariant& value) {
    Q_UNUSED(value)
    // Animation is handled by Qt's property system
}

void FluentSheet::onCloseAnimationFinished() {
    setVisible(false);
    hideOverlay();
    emit closed();
}

void FluentSheet::onOverlayClicked() {
    if (m_closeOnOverlayClick) {
        close();
    }
    emit overlayClicked();
}

void FluentSheet::onThemeChanged() {
    updateStateStyle();
}

// Setup methods
void FluentSheet::setupSheet() {
    setWindowFlags(Qt::Widget);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setVisible(false);

    // Initially position the sheet in closed state
    setGeometry(calculateClosedGeometry());
}

void FluentSheet::setupOverlay() {
    m_overlay->setParent(findTopLevelParent());
    m_overlay->setOverlayOpacity(m_overlayOpacity);

    connect(m_overlay.get(), &FluentSheetOverlay::clicked,
            this, &FluentSheet::onOverlayClicked);
}

void FluentSheet::setupContent() {
    m_content->setParent(this);
    updateContentGeometry();
}

void FluentSheet::setupAnimations() {
    // Setup open animation
    m_openAnimation->setDuration(m_animationDuration);
    m_openAnimation->setEasingCurve(m_easingCurve);
    connect(m_openAnimation.get(), &QPropertyAnimation::valueChanged,
            this, &FluentSheet::onOpenAnimationValueChanged);
    connect(m_openAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentSheet::onOpenAnimationFinished);

    // Setup close animation
    m_closeAnimation->setDuration(m_animationDuration);
    m_closeAnimation->setEasingCurve(m_easingCurve);
    connect(m_closeAnimation.get(), &QPropertyAnimation::valueChanged,
            this, &FluentSheet::onCloseAnimationValueChanged);
    connect(m_closeAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentSheet::onCloseAnimationFinished);

    // Setup overlay animation
    m_overlayAnimation->setDuration(m_animationDuration);
    m_overlayAnimation->setEasingCurve(m_easingCurve);

    // Setup animation group
    m_animationGroup->addAnimation(m_openAnimation.get());
    m_animationGroup->addAnimation(m_overlayAnimation.get());
}

void FluentSheet::setupAccessibility() {
    setAccessibleName("Sheet");
    setAccessibleDescription("A sliding panel that can contain content");
    setFocusPolicy(Qt::StrongFocus);
}

// Animation methods
void FluentSheet::startOpenAnimation() {
    stopAllAnimations();

    const QRect startGeometry = calculateClosedGeometry();
    const QRect endGeometry = calculateOpenGeometry();

    setGeometry(startGeometry);

    m_openAnimation->setStartValue(startGeometry);
    m_openAnimation->setEndValue(endGeometry);

    if (m_overlayVisible && m_behavior == FluentSheetBehavior::Modal) {
        m_overlayAnimation->setStartValue(0.0);
        m_overlayAnimation->setEndValue(m_overlayOpacity);
        m_animationGroup->start();
    } else {
        m_openAnimation->start();
    }
}

void FluentSheet::startCloseAnimation() {
    stopAllAnimations();

    const QRect startGeometry = geometry();
    const QRect endGeometry = calculateClosedGeometry();

    m_closeAnimation->setStartValue(startGeometry);
    m_closeAnimation->setEndValue(endGeometry);

    if (m_overlayVisible && m_behavior == FluentSheetBehavior::Modal) {
        m_overlayAnimation->setStartValue(m_overlayOpacity);
        m_overlayAnimation->setEndValue(0.0);

        // Create a new animation group for closing
        auto closeGroup = new QParallelAnimationGroup(this);
        closeGroup->addAnimation(m_closeAnimation.get());
        closeGroup->addAnimation(m_overlayAnimation.get());
        closeGroup->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        m_closeAnimation->start();
    }
}

void FluentSheet::stopAllAnimations() {
    m_animationGroup->stop();
    m_openAnimation->stop();
    m_closeAnimation->stop();
    m_overlayAnimation->stop();
}

QRect FluentSheet::calculateOpenGeometry() const {
    const QWidget* parentWidget = findTopLevelParent();
    if (!parentWidget) {
        return QRect();
    }

    const QRect parentRect = parentWidget->geometry();
    const QSize sheetSize = calculateSheetSize();

    QRect openRect;

    switch (m_direction) {
    case FluentSheetDirection::Top:
        openRect = QRect(parentRect.x(), parentRect.y(), parentRect.width(), sheetSize.height());
        break;
    case FluentSheetDirection::Bottom:
        openRect = QRect(parentRect.x(), parentRect.bottom() - sheetSize.height(),
                        parentRect.width(), sheetSize.height());
        break;
    case FluentSheetDirection::Left:
        openRect = QRect(parentRect.x(), parentRect.y(), sheetSize.width(), parentRect.height());
        break;
    case FluentSheetDirection::Right:
        openRect = QRect(parentRect.right() - sheetSize.width(), parentRect.y(),
                        sheetSize.width(), parentRect.height());
        break;
    }

    return openRect;
}

QRect FluentSheet::calculateClosedGeometry() const {
    const QWidget* parentWidget = findTopLevelParent();
    if (!parentWidget) {
        return QRect();
    }

    const QRect parentRect = parentWidget->geometry();
    const QSize sheetSize = calculateSheetSize();

    QRect closedRect;

    switch (m_direction) {
    case FluentSheetDirection::Top:
        closedRect = QRect(parentRect.x(), parentRect.y() - sheetSize.height(),
                          parentRect.width(), sheetSize.height());
        break;
    case FluentSheetDirection::Bottom:
        closedRect = QRect(parentRect.x(), parentRect.bottom(),
                          parentRect.width(), sheetSize.height());
        break;
    case FluentSheetDirection::Left:
        closedRect = QRect(parentRect.x() - sheetSize.width(), parentRect.y(),
                          sheetSize.width(), parentRect.height());
        break;
    case FluentSheetDirection::Right:
        closedRect = QRect(parentRect.right(), parentRect.y(),
                          sheetSize.width(), parentRect.height());
        break;
    }

    return closedRect;
}

QSize FluentSheet::calculateSheetSize() const {
    const QWidget* parentWidget = findTopLevelParent();
    if (!parentWidget) {
        return m_customSize;
    }

    const QSize parentSize = parentWidget->size();

    switch (m_sheetSize) {
    case FluentSheetSize::Small:
        if (isVerticalDirection()) {
            return QSize(parentSize.width(), parentSize.height() / 4);
        } else {
            return QSize(parentSize.width() / 4, parentSize.height());
        }
    case FluentSheetSize::Medium:
        if (isVerticalDirection()) {
            return QSize(parentSize.width(), parentSize.height() / 2);
        } else {
            return QSize(parentSize.width() / 2, parentSize.height());
        }
    case FluentSheetSize::Large:
        if (isVerticalDirection()) {
            return QSize(parentSize.width(), parentSize.height() * 3 / 4);
        } else {
            return QSize(parentSize.width() * 3 / 4, parentSize.height());
        }
    case FluentSheetSize::Full:
        return parentSize;
    case FluentSheetSize::Custom:
        return m_customSize;
    }

    return m_customSize;
}

// Utility methods
void FluentSheet::showOverlay() {
    if (m_overlay && m_overlayVisible) {
        updateOverlayGeometry();
        m_overlay->setVisible(true);
        m_overlay->raise();
    }
}

void FluentSheet::hideOverlay() {
    if (m_overlay) {
        m_overlay->setVisible(false);
    }
}

void FluentSheet::updateOverlayGeometry() {
    if (!m_overlay) {
        return;
    }

    const QWidget* parentWidget = findTopLevelParent();
    if (parentWidget) {
        m_overlay->setGeometry(parentWidget->geometry());
    }
}

void FluentSheet::updateContentGeometry() {
    if (m_content) {
        m_content->setGeometry(rect());
    }
}

void FluentSheet::updateGeometry() {
    if (m_isOpen) {
        setGeometry(calculateOpenGeometry());
    } else {
        setGeometry(calculateClosedGeometry());
    }
    updateContentGeometry();
}

void FluentSheet::updateParentGeometry() {
    updateOverlayGeometry();
    updateGeometry();
}

QWidget* FluentSheet::findTopLevelParent() const {
    QWidget* topLevel = parentWidget();
    while (topLevel && topLevel->parentWidget()) {
        topLevel = topLevel->parentWidget();
    }
    return topLevel ? topLevel : const_cast<FluentSheet*>(this);
}

bool FluentSheet::isVerticalDirection() const {
    return m_direction == FluentSheetDirection::Top || m_direction == FluentSheetDirection::Bottom;
}

bool FluentSheet::isHorizontalDirection() const {
    return m_direction == FluentSheetDirection::Left || m_direction == FluentSheetDirection::Right;
}

// Focus management
void FluentSheet::captureFocus() {
    m_previousFocusWidget = QApplication::focusWidget();
    setFocus();
    updateFocusChain();
}

void FluentSheet::restoreFocus() {
    if (m_previousFocusWidget) {
        m_previousFocusWidget->setFocus();
        m_previousFocusWidget = nullptr;
    }
}

void FluentSheet::updateFocusChain() {
    m_focusChain.clear();

    // Build focus chain for all focusable widgets in the sheet
    const QList<QWidget*> widgets = findChildren<QWidget*>();
    for (QWidget* widget : widgets) {
        if (widget->focusPolicy() != Qt::NoFocus && widget->isVisible() && widget->isEnabled()) {
            m_focusChain.append(widget);
        }
    }
}

// Drag handling
void FluentSheet::startDrag(const QPoint& startPos) {
    m_isDragging = true;
    m_dragStartPos = startPos;
    m_dragCurrentPos = startPos;
    emit dragStarted();
}

void FluentSheet::updateDrag(const QPoint& currentPos) {
    if (!m_isDragging) {
        return;
    }

    m_dragCurrentPos = currentPos;

    // Calculate drag delta and apply constraints
    const QPoint delta = currentPos - m_dragStartPos;
    QRect newGeometry = geometry();

    switch (m_direction) {
    case FluentSheetDirection::Top:
        newGeometry.moveTop(newGeometry.top() + qMin(0, delta.y()));
        break;
    case FluentSheetDirection::Bottom:
        newGeometry.moveTop(newGeometry.top() + qMax(0, delta.y()));
        break;
    case FluentSheetDirection::Left:
        newGeometry.moveLeft(newGeometry.left() + qMin(0, delta.x()));
        break;
    case FluentSheetDirection::Right:
        newGeometry.moveLeft(newGeometry.left() + qMax(0, delta.x()));
        break;
    }

    setGeometry(newGeometry);
}

void FluentSheet::finishDrag() {
    if (!m_isDragging) {
        return;
    }

    const QPoint delta = m_dragCurrentPos - m_dragStartPos;

    if (shouldCloseFromDrag(delta)) {
        close();
    } else {
        // Snap back to open position
        startOpenAnimation();
    }

    m_isDragging = false;
    emit dragFinished();
}

void FluentSheet::cancelDrag() {
    if (m_isDragging) {
        m_isDragging = false;
        startOpenAnimation();
    }
}

bool FluentSheet::shouldCloseFromDrag(const QPoint& delta) const {
    const int threshold = 100; // Minimum drag distance to close

    switch (m_direction) {
    case FluentSheetDirection::Top:
        return delta.y() < -threshold;
    case FluentSheetDirection::Bottom:
        return delta.y() > threshold;
    case FluentSheetDirection::Left:
        return delta.x() < -threshold;
    case FluentSheetDirection::Right:
        return delta.x() > threshold;
    }

    return false;
}

// Painting methods
void FluentSheet::paintBackground(QPainter& painter, const QRect& rect) {
    const QColor backgroundColor = getBackgroundColor();
    painter.fillRect(rect, backgroundColor);
}

void FluentSheet::paintBorder(QPainter& painter, const QRect& rect) {
    const QPen borderPen = getBorderPen();
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect.adjusted(0, 0, -1, -1));
}

void FluentSheet::paintShadow(QPainter& painter, const QRect& rect) {
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // Shadow can be implemented using QGraphicsDropShadowEffect if needed
}

// Style methods
QColor FluentSheet::getBackgroundColor() const {
    const auto& theme = FluentQt::Styling::FluentTheme::instance();
    return theme.color("layerFillColorDefault");
}

QColor FluentSheet::getBorderColor() const {
    const auto& theme = FluentQt::Styling::FluentTheme::instance();
    return theme.color("controlStrokeColorDefault");
}

QColor FluentSheet::getShadowColor() const {
    return QColor(0, 0, 0, 50);
}

QPen FluentSheet::getBorderPen() const {
    const QColor borderColor = getBorderColor();
    return QPen(borderColor, 1);
}

} // namespace FluentQt::Components

#include "FluentSheet.moc"
