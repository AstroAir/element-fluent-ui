// src/Components/FluentAccordion.cpp
#include "FluentQt/Components/FluentAccordion.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Accessibility/FluentAccessible.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QAccessible>
#include <QtMath>

namespace FluentQt::Components {

FluentAccordion::FluentAccordion(QWidget* parent)
    : Core::FluentComponent(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setObjectName("FluentAccordion");
    
    setupUI();
    setupAnimations();
    updateColors();
    updateFonts();
    
    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentAccordion::onThemeChanged);
}

FluentAccordion::FluentAccordion(const QString& title, QWidget* parent)
    : FluentAccordion(parent)
{
    setTitle(title);
}

FluentAccordion::~FluentAccordion() = default;

void FluentAccordion::setupUI() {
    FLUENT_PROFILE("FluentAccordion::setupUI");
    
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Create header widget
    m_headerWidget = new QWidget(this);
    m_headerWidget->setObjectName("FluentAccordion_Header");
    m_headerWidget->setFixedHeight(48);
    
    // Create header layout
    m_headerLayout = new QHBoxLayout(m_headerWidget);
    m_headerLayout->setContentsMargins(16, 8, 16, 8);
    m_headerLayout->setSpacing(12);
    
    // Create title label
    m_titleLabel = new QLabel(m_headerWidget);
    m_titleLabel->setObjectName("FluentAccordion_Title");
    m_headerLayout->addWidget(m_titleLabel);
    
    // Add stretch to push chevron to the right
    m_headerLayout->addStretch();
    
    m_mainLayout->addWidget(m_headerWidget);
    
    // Create content container
    m_contentContainer = new QWidget(this);
    m_contentContainer->setObjectName("FluentAccordion_ContentContainer");
    m_contentContainer->setFixedHeight(0);
    
    // Create content layout
    m_contentLayout = new QVBoxLayout(m_contentContainer);
    m_contentLayout->setContentsMargins(16, 8, 16, 16);
    m_contentLayout->setSpacing(8);
    
    m_mainLayout->addWidget(m_contentContainer);
    
    // Set minimum height
    setMinimumHeight(48);
}

void FluentAccordion::setupAnimations() {
    FLUENT_PROFILE("FluentAccordion::setupAnimations");
    
    // Create opacity effect for content
    m_contentOpacityEffect = new QGraphicsOpacityEffect(this);
    m_contentOpacityEffect->setOpacity(0.0);
    m_contentContainer->setGraphicsEffect(m_contentOpacityEffect);
    
    // Create animation group
    m_animationGroup = std::make_unique<QParallelAnimationGroup>(this);
    
    // Create height animation
    m_heightAnimation = std::make_unique<QPropertyAnimation>(this, "contentHeight");
    m_heightAnimation->setDuration(250);
    m_heightAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Create opacity animation
    m_opacityAnimation = std::make_unique<QPropertyAnimation>(this, "contentOpacity");
    m_opacityAnimation->setDuration(200);
    m_opacityAnimation->setEasingCurve(QEasingCurve::OutQuad);
    
    m_animationGroup->addAnimation(m_heightAnimation.get());
    m_animationGroup->addAnimation(m_opacityAnimation.get());
    
    connect(m_animationGroup.get(), &QParallelAnimationGroup::finished,
            this, &FluentAccordion::onAnimationFinished);
}

QString FluentAccordion::title() const {
    return m_title;
}

void FluentAccordion::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        m_titleLabel->setText(title);
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit titleChanged(title);
    }
}

QString FluentAccordion::description() const {
    return m_description;
}

void FluentAccordion::setDescription(const QString& description) {
    if (m_description != description) {
        m_description = description;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit descriptionChanged(description);
    }
}

QIcon FluentAccordion::icon() const {
    return m_icon;
}

void FluentAccordion::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit iconChanged(icon);
    }
}

FluentAccordionState FluentAccordion::accordionState() const {
    return m_accordionState;
}

void FluentAccordion::setAccordionState(FluentAccordionState state) {
    if (m_accordionState != state) {
        m_accordionState = state;
        
        if (state == FluentAccordionState::Expanded) {
            startExpandAnimation();
        } else {
            startCollapseAnimation();
        }
        
        emit accordionStateChanged(state);
    }
}

bool FluentAccordion::isCollapsible() const {
    return m_collapsible;
}

void FluentAccordion::setCollapsible(bool collapsible) {
    if (m_collapsible != collapsible) {
        m_collapsible = collapsible;
        emit collapsibleChanged(collapsible);
    }
}

qreal FluentAccordion::contentOpacity() const {
    return m_contentOpacity;
}

void FluentAccordion::setContentOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_contentOpacity, opacity)) {
        m_contentOpacity = qBound(0.0, opacity, 1.0);
        m_contentOpacityEffect->setOpacity(m_contentOpacity);
    }
}

int FluentAccordion::contentHeight() const {
    return m_contentHeight;
}

void FluentAccordion::setContentHeight(int height) {
    if (m_contentHeight != height) {
        m_contentHeight = height;
        m_contentContainer->setFixedHeight(height);
        updateGeometry();
    }
}

void FluentAccordion::setContentWidget(QWidget* widget) {
    if (m_content) {
        m_contentLayout->removeWidget(m_content);
        m_content->setParent(nullptr);
    }
    
    m_content = widget;
    if (widget) {
        m_contentLayout->addWidget(widget);
        m_expandedHeight = calculateContentHeight();
        m_sizeHintValid = false;
        updateGeometry();
    }
}

QWidget* FluentAccordion::contentWidget() const {
    return m_content;
}

QSize FluentAccordion::sizeHint() const {
    FLUENT_PROFILE("FluentAccordion::sizeHint");
    
    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }
    
    int width = 200; // Minimum width
    int height = 48; // Header height
    
    if (m_accordionState == FluentAccordionState::Expanded && m_content) {
        height += m_expandedHeight;
    }
    
    m_cachedSizeHint = QSize(width, height);
    m_sizeHintValid = true;
    
    return m_cachedSizeHint;
}

QSize FluentAccordion::minimumSizeHint() const {
    return QSize(150, 48);
}

void FluentAccordion::toggle() {
    if (m_collapsible) {
        setAccordionState(m_accordionState == FluentAccordionState::Expanded ? 
                         FluentAccordionState::Collapsed : FluentAccordionState::Expanded);
    }
}

void FluentAccordion::expand() {
    setAccordionState(FluentAccordionState::Expanded);
}

void FluentAccordion::collapse() {
    if (m_collapsible) {
        setAccordionState(FluentAccordionState::Collapsed);
    }
}

void FluentAccordion::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentAccordion::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintHeader(painter, headerRect());

    // Paint border
    paintBorder(painter, rect);
}

void FluentAccordion::paintHeader(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Background color based on state
    QColor backgroundColor;
    switch (state()) {
        case Core::FluentState::Normal:
            backgroundColor = palette.neutralLighter;
            break;
        case Core::FluentState::Hovered:
            backgroundColor = palette.neutralLight;
            break;
        case Core::FluentState::Pressed:
            backgroundColor = palette.neutralQuaternaryAlt;
            break;
        case Core::FluentState::Disabled:
            backgroundColor = palette.neutralLighter;
            break;
        default:
            backgroundColor = palette.neutralLighter;
    }

    // Create path with rounded corners
    QPainterPath path;
    const int radius = cornerRadius();
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, backgroundColor);

    // Paint icon if present
    if (!m_icon.isNull()) {
        paintIcon(painter, iconRect());
    }

    // Paint chevron
    paintChevron(painter, chevronRect());
}

void FluentAccordion::paintIcon(QPainter& painter, const QRect& rect) {
    if (m_icon.isNull()) return;

    const QIcon::Mode iconMode = isEnabled() ?
        (state() == Core::FluentState::Pressed ? QIcon::Selected : QIcon::Normal) :
        QIcon::Disabled;

    m_icon.paint(&painter, rect, Qt::AlignCenter, iconMode);
}

void FluentAccordion::paintChevron(QPainter& painter, const QRect& rect) {
    painter.save();

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    QPen pen(palette.neutralSecondary, 2);
    painter.setPen(pen);

    const QPointF center = rect.center();
    const qreal size = 6;

    // Rotate based on state
    painter.translate(center);
    if (m_accordionState == FluentAccordionState::Expanded) {
        painter.rotate(180);
    }

    // Draw chevron down arrow
    QPainterPath chevron;
    chevron.moveTo(-size, -size/2);
    chevron.lineTo(0, size/2);
    chevron.lineTo(size, -size/2);

    painter.drawPath(chevron);

    painter.restore();
}

void FluentAccordion::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QPen borderPen(palette.neutralQuaternary, 1);
    painter.setPen(borderPen);

    const int radius = cornerRadius();
    const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentAccordion::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && headerRect().contains(event->pos())) {
        m_pressed = true;
        setState(Core::FluentState::Pressed);
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentAccordion::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        if (headerRect().contains(event->pos())) {
            toggle();
        }

        setState(rect().contains(event->pos()) ?
                 Core::FluentState::Hovered : Core::FluentState::Normal);
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentAccordion::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        if (!event->isAutoRepeat()) {
            m_pressed = true;
            setState(Core::FluentState::Pressed);
        }
        event->accept();
        return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentAccordion::keyReleaseEvent(QKeyEvent* event) {
    if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) &&
        m_pressed && !event->isAutoRepeat()) {

        m_pressed = false;
        toggle();
        setState(Core::FluentState::Focused);

        event->accept();
        return;
    }

    Core::FluentComponent::keyReleaseEvent(event);
}

void FluentAccordion::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
    m_sizeHintValid = false;
}

void FluentAccordion::updateStateStyle() {
    update();
}

void FluentAccordion::performStateTransition(Core::FluentState from, Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    update();
}

void FluentAccordion::onThemeChanged() {
    updateColors();
    updateFonts();
    update();
}

void FluentAccordion::onAnimationFinished() {
    updateContentVisibility();

    if (m_accordionState == FluentAccordionState::Expanded) {
        emit expanded();
    } else {
        emit collapsed();
    }
}

void FluentAccordion::updateColors() {
    // Colors are handled in paintEvent
}

void FluentAccordion::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();
    m_titleLabel->setFont(theme.bodyFont());
}

void FluentAccordion::startExpandAnimation() {
    if (!isAnimated()) {
        setContentHeight(m_expandedHeight);
        setContentOpacity(1.0);
        updateContentVisibility();
        return;
    }

    m_heightAnimation->setStartValue(0);
    m_heightAnimation->setEndValue(m_expandedHeight);

    m_opacityAnimation->setStartValue(0.0);
    m_opacityAnimation->setEndValue(1.0);

    m_animationGroup->start();
}

void FluentAccordion::startCollapseAnimation() {
    if (!isAnimated()) {
        setContentHeight(0);
        setContentOpacity(0.0);
        updateContentVisibility();
        return;
    }

    m_heightAnimation->setStartValue(m_expandedHeight);
    m_heightAnimation->setEndValue(0);

    m_opacityAnimation->setStartValue(1.0);
    m_opacityAnimation->setEndValue(0.0);

    m_animationGroup->start();
}

void FluentAccordion::updateContentVisibility() {
    if (m_content) {
        m_content->setVisible(m_accordionState == FluentAccordionState::Expanded);
    }
}

QRect FluentAccordion::headerRect() const {
    return QRect(0, 0, width(), 48);
}

QRect FluentAccordion::contentRect() const {
    return QRect(0, 48, width(), height() - 48);
}

QRect FluentAccordion::iconRect() const {
    if (m_icon.isNull()) return QRect();

    const int iconSize = 20;
    const int margin = 16;
    return QRect(margin, (48 - iconSize) / 2, iconSize, iconSize);
}

QRect FluentAccordion::chevronRect() const {
    const int chevronSize = 16;
    const int margin = 16;
    return QRect(width() - margin - chevronSize, (48 - chevronSize) / 2, chevronSize, chevronSize);
}

int FluentAccordion::calculateContentHeight() const {
    if (!m_content) return 0;

    const int margins = m_contentLayout->contentsMargins().top() +
                       m_contentLayout->contentsMargins().bottom();
    return m_content->sizeHint().height() + margins;
}

} // namespace FluentQt::Components
