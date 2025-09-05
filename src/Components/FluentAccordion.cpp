// src/Components/FluentAccordion.cpp
#include "FluentQt/Components/FluentAccordion.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QtMath>

namespace FluentQt::Components {

FluentAccordion::FluentAccordion(QWidget* parent)
    : Core::FluentComponent(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
    setObjectName("FluentAccordion");

    setupUI();
    setupAnimations();
    updateColors();
    updateFonts();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentAccordion::onThemeChanged);
}

FluentAccordion::FluentAccordion(const QString& title, QWidget* parent)
    : FluentAccordion(parent) {
    setTitle(title);
}

FluentAccordion::~FluentAccordion() = default;

void FluentAccordion::setupUI() {
    FLUENT_PROFILE("FluentAccordion::setupUI");

    const auto& theme = Styling::FluentTheme::instance();

    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create header widget with Fluent Design height
    m_headerWidget = new QWidget(this);
    m_headerWidget->setObjectName("FluentAccordion_Header");
    m_headerWidget->setFixedHeight(
        theme.spacing("component.accordion.header.height"));

    // Create header layout with Fluent Design spacing
    m_headerLayout = new QHBoxLayout(m_headerWidget);
    const int horizontalPadding =
        theme.spacing("component.accordion.padding.horizontal");
    const int verticalPadding =
        theme.spacing("component.accordion.padding.vertical");
    const int itemSpacing = theme.spacing("component.accordion.item.spacing");

    m_headerLayout->setContentsMargins(horizontalPadding, verticalPadding,
                                       horizontalPadding, verticalPadding);
    m_headerLayout->setSpacing(itemSpacing);

    // Create title and description container
    auto* textContainer = new QWidget(m_headerWidget);
    auto* textLayout = new QVBoxLayout(textContainer);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(theme.spacing("component.accordion.text.spacing"));

    // Create title label with semantic typography
    m_titleLabel = new QLabel(textContainer);
    m_titleLabel->setObjectName("FluentAccordion_Title");
    m_titleLabel->setFont(theme.subtitleFont());
    textLayout->addWidget(m_titleLabel);

    // Create description label with semantic typography
    m_descriptionLabel = new QLabel(textContainer);
    m_descriptionLabel->setObjectName("FluentAccordion_Description");
    m_descriptionLabel->setFont(theme.captionFont());
    m_descriptionLabel->setVisible(false);  // Initially hidden
    textLayout->addWidget(m_descriptionLabel);

    m_headerLayout->addWidget(textContainer);

    // Add stretch to push chevron to the right
    m_headerLayout->addStretch();

    m_mainLayout->addWidget(m_headerWidget);

    // Create content container with Fluent Design styling
    m_contentContainer = new QWidget(this);
    m_contentContainer->setObjectName("FluentAccordion_ContentContainer");
    m_contentContainer->setFixedHeight(0);

    // Create content layout with Fluent Design spacing
    m_contentLayout = new QVBoxLayout(m_contentContainer);
    const int contentPadding =
        theme.spacing("component.accordion.content.padding");
    const int contentSpacing =
        theme.spacing("component.accordion.content.spacing");

    m_contentLayout->setContentsMargins(contentPadding, contentSpacing,
                                        contentPadding, contentPadding);
    m_contentLayout->setSpacing(contentSpacing);

    m_mainLayout->addWidget(m_contentContainer);

    // Set minimum height to header height
    const int minHeight = theme.spacing("component.accordion.header.height");
    setMinimumHeight(minHeight);

    // Apply initial Fluent styling
    updateFluentStyling();
    applyElevationEffect();
    updateAccessibilityAttributes();
}

void FluentAccordion::setupAnimations() {
    FLUENT_PROFILE("FluentAccordion::setupAnimations");

    // Create opacity effect for content
    m_contentOpacityEffect = new QGraphicsOpacityEffect(this);
    m_contentOpacityEffect->setOpacity(0.0);
    m_contentContainer->setGraphicsEffect(m_contentOpacityEffect);

    // Create animation group
    m_animationGroup = std::make_unique<QParallelAnimationGroup>(this);

    // Create height animation with Fluent Design specifications
    m_heightAnimation =
        std::make_unique<QPropertyAnimation>(this, "contentHeight");
    m_heightAnimation->setDuration(
        350);  // Fluent Design standard expansion duration

    // Use Fluent Design expansion curve (0.25, 0.46, 0.45, 0.94)
    QEasingCurve expansionCurve(QEasingCurve::BezierSpline);
    expansionCurve.addCubicBezierSegment(
        QPointF(0.25, 0.46), QPointF(0.45, 0.94), QPointF(1.0, 1.0));
    m_heightAnimation->setEasingCurve(expansionCurve);

    // Create opacity animation with staggered timing for better visual
    // hierarchy
    m_opacityAnimation =
        std::make_unique<QPropertyAnimation>(this, "contentOpacity");
    m_opacityAnimation->setDuration(250);  // Slightly shorter for fade effect

    // Use Fluent Design fade curve
    QEasingCurve fadeCurve(QEasingCurve::BezierSpline);
    fadeCurve.addCubicBezierSegment(QPointF(0.33, 0.0), QPointF(0.67, 1.0),
                                    QPointF(1.0, 1.0));
    m_opacityAnimation->setEasingCurve(fadeCurve);

    m_animationGroup->addAnimation(m_heightAnimation.get());
    m_animationGroup->addAnimation(m_opacityAnimation.get());

    connect(m_animationGroup.get(), &QParallelAnimationGroup::finished, this,
            &FluentAccordion::onAnimationFinished);
}

QString FluentAccordion::title() const { return m_title; }

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

QString FluentAccordion::description() const { return m_description; }

void FluentAccordion::setDescription(const QString& description) {
    if (m_description != description) {
        m_description = description;
        if (m_descriptionLabel) {
            m_descriptionLabel->setText(description);
            m_descriptionLabel->setVisible(m_showDescription &&
                                           !description.isEmpty());
        }
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit descriptionChanged(description);
    }
}

QIcon FluentAccordion::icon() const { return m_icon; }

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

bool FluentAccordion::isCollapsible() const { return m_collapsible; }

void FluentAccordion::setCollapsible(bool collapsible) {
    if (m_collapsible != collapsible) {
        m_collapsible = collapsible;
        emit collapsibleChanged(collapsible);
    }
}

int FluentAccordion::elevation() const { return m_elevation; }

void FluentAccordion::setElevation(int elevation) {
    if (m_elevation != elevation) {
        m_elevation = elevation;
        applyElevationEffect();
        update();
        emit elevationChanged(elevation);
    }
}

bool FluentAccordion::showDescription() const { return m_showDescription; }

void FluentAccordion::setShowDescription(bool show) {
    if (m_showDescription != show) {
        m_showDescription = show;
        if (m_descriptionLabel) {
            m_descriptionLabel->setVisible(show && !m_description.isEmpty());
        }
        m_sizeHintValid = false;
        updateGeometry();
        emit showDescriptionChanged(show);
    }
}

bool FluentAccordion::showIcon() const { return m_showIcon; }

void FluentAccordion::setShowIcon(bool show) {
    if (m_showIcon != show) {
        m_showIcon = show;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit showIconChanged(show);
    }
}

QString FluentAccordion::ariaLabel() const {
    return m_ariaLabel.isEmpty() ? m_title : m_ariaLabel;
}

void FluentAccordion::setAriaLabel(const QString& label) {
    if (m_ariaLabel != label) {
        m_ariaLabel = label;
        updateAccessibilityAttributes();
        emit ariaLabelChanged(label);
    }
}

bool FluentAccordion::reducedMotion() const { return m_reducedMotion; }

void FluentAccordion::setReducedMotion(bool reduced) {
    if (m_reducedMotion != reduced) {
        m_reducedMotion = reduced;
        emit reducedMotionChanged(reduced);
    }
}

qreal FluentAccordion::contentOpacity() const { return m_contentOpacity; }

void FluentAccordion::setContentOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_contentOpacity, opacity)) {
        m_contentOpacity = qBound(0.0, opacity, 1.0);
        m_contentOpacityEffect->setOpacity(m_contentOpacity);
    }
}

int FluentAccordion::contentHeight() const { return m_contentHeight; }

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

QWidget* FluentAccordion::contentWidget() const { return m_content; }

QSize FluentAccordion::sizeHint() const {
    FLUENT_PROFILE("FluentAccordion::sizeHint");

    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }

    int width = 200;  // Minimum width
    int height = 48;  // Header height

    if (m_accordionState == FluentAccordionState::Expanded && m_content) {
        height += m_expandedHeight;
    }

    m_cachedSizeHint = QSize(width, height);
    m_sizeHintValid = true;

    return m_cachedSizeHint;
}

QSize FluentAccordion::minimumSizeHint() const { return QSize(150, 48); }

void FluentAccordion::toggle() {
    if (m_collapsible) {
        setAccordionState(m_accordionState == FluentAccordionState::Expanded
                              ? FluentAccordionState::Collapsed
                              : FluentAccordionState::Expanded);
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

    // Use Fluent Design semantic colors based on state
    QColor backgroundColor;
    switch (state()) {
        case Core::FluentState::Normal:
            backgroundColor = theme.color("surface.primary");
            break;
        case Core::FluentState::Hovered:
            backgroundColor = theme.color("surface.primary.hover");
            break;
        case Core::FluentState::Pressed:
            backgroundColor = theme.color("surface.primary.pressed");
            break;
        case Core::FluentState::Focused:
            backgroundColor = theme.color("surface.primary.focused");
            break;
        case Core::FluentState::Disabled:
            backgroundColor = theme.color("surface.primary.disabled");
            break;
        default:
            backgroundColor = theme.color("surface.primary");
    }

    // Create path with Fluent Design corner radius
    QPainterPath path;
    const int radius = cornerRadius();
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, backgroundColor);

    // Paint elevation shadow if enabled
    if (m_elevation > 0) {
        paintElevationShadow(painter, rect);
    }

    // Paint focus indicator if focused
    if (state() == Core::FluentState::Focused) {
        paintFocusIndicator(painter, rect);
    }

    // Paint icon if present and enabled
    if (!m_icon.isNull() && m_showIcon) {
        paintIcon(painter, iconRect());
    }

    // Paint chevron
    paintChevron(painter, chevronRect());
}

void FluentAccordion::paintIcon(QPainter& painter, const QRect& rect) {
    if (m_icon.isNull())
        return;

    const QIcon::Mode iconMode =
        isEnabled() ? (state() == Core::FluentState::Pressed ? QIcon::Selected
                                                             : QIcon::Normal)
                    : QIcon::Disabled;

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
    chevron.moveTo(-size, -size / 2);
    chevron.lineTo(0, size / 2);
    chevron.lineTo(size, -size / 2);

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
    if (event->button() == Qt::LeftButton &&
        headerRect().contains(event->pos())) {
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

        setState(rect().contains(event->pos()) ? Core::FluentState::Hovered
                                               : Core::FluentState::Normal);
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

void FluentAccordion::updateStateStyle() { update(); }

void FluentAccordion::performStateTransition(Core::FluentState from,
                                             Core::FluentState to) {
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
    if (!isAnimated() || shouldUseReducedMotion()) {
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
    if (!isAnimated() || shouldUseReducedMotion()) {
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
        m_content->setVisible(m_accordionState ==
                              FluentAccordionState::Expanded);
    }
}

QRect FluentAccordion::headerRect() const { return QRect(0, 0, width(), 48); }

QRect FluentAccordion::contentRect() const {
    return QRect(0, 48, width(), height() - 48);
}

QRect FluentAccordion::iconRect() const {
    if (m_icon.isNull())
        return QRect();

    const int iconSize = 20;
    const int margin = 16;
    return QRect(margin, (48 - iconSize) / 2, iconSize, iconSize);
}

QRect FluentAccordion::chevronRect() const {
    const int chevronSize = 16;
    const int margin = 16;
    return QRect(width() - margin - chevronSize, (48 - chevronSize) / 2,
                 chevronSize, chevronSize);
}

int FluentAccordion::calculateContentHeight() const {
    if (!m_content)
        return 0;

    const int margins = m_contentLayout->contentsMargins().top() +
                        m_contentLayout->contentsMargins().bottom();
    return m_content->sizeHint().height() + margins;
}

QRect FluentAccordion::titleRect() const {
    if (!m_titleLabel)
        return QRect();
    return m_titleLabel->geometry();
}

QRect FluentAccordion::descriptionRect() const {
    if (!m_descriptionLabel)
        return QRect();
    return m_descriptionLabel->geometry();
}

void FluentAccordion::updateFluentStyling() {
    const auto& theme = Styling::FluentTheme::instance();

    // Update typography
    if (m_titleLabel) {
        m_titleLabel->setFont(theme.subtitleFont());
    }
    if (m_descriptionLabel) {
        m_descriptionLabel->setFont(theme.captionFont());
    }

    // Update colors based on current theme
    updateColors();
}

void FluentAccordion::applyElevationEffect() {
    // Apply shadow effect based on elevation level
    if (m_elevation <= 0) {
        setGraphicsEffect(nullptr);
        return;
    }

    auto* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(m_elevation * 2);
    shadowEffect->setOffset(0, m_elevation);
    shadowEffect->setColor(
        QColor(0, 0, 0, 32));  // Fluent Design shadow opacity
    setGraphicsEffect(shadowEffect);
}

void FluentAccordion::updateAccessibilityAttributes() {
    // Set ARIA role and properties
    setAccessibleName(ariaLabel());
    setAccessibleDescription(m_description);

    // Set expanded state for screen readers
    const bool expanded = (m_accordionState == FluentAccordionState::Expanded);
    setProperty("aria-expanded", expanded);
    setProperty("aria-controls", m_contentContainer->objectName());

    // Set role as button since it's clickable
    setProperty("role", "button");
}

bool FluentAccordion::shouldUseReducedMotion() const {
    // Check system preference or manual override
    return m_reducedMotion ||
           QApplication::testAttribute(Qt::AA_DisableWindowContextHelpButton);
}

void FluentAccordion::paintElevationShadow(QPainter& painter,
                                           const QRect& rect) {
    if (m_elevation <= 0)
        return;

    painter.save();

    const auto& theme = Styling::FluentTheme::instance();
    const int shadowBlur = m_elevation * 2;
    const int shadowOffset = m_elevation;

    // Create shadow color with appropriate opacity
    QColor shadowColor = theme.color("shadow.primary");
    shadowColor.setAlpha(32 +
                         (m_elevation * 8));  // Increase opacity with elevation

    // Draw shadow using gradient
    QRadialGradient gradient(rect.center(), rect.width() / 2);
    gradient.setColorAt(0, shadowColor);
    gradient.setColorAt(1, Qt::transparent);

    QPainterPath shadowPath;
    shadowPath.addRoundedRect(rect.adjusted(0, shadowOffset, 0, shadowOffset),
                              cornerRadius(), cornerRadius());

    painter.fillPath(shadowPath, QBrush(gradient));
    painter.restore();
}

void FluentAccordion::paintFocusIndicator(QPainter& painter,
                                          const QRect& rect) {
    painter.save();

    const auto& theme = Styling::FluentTheme::instance();
    QColor focusColor = theme.color("accent.primary");

    QPen focusPen(focusColor, 2);
    focusPen.setStyle(Qt::SolidLine);
    painter.setPen(focusPen);

    // Draw focus ring with slight inset
    const QRect focusRect = rect.adjusted(1, 1, -1, -1);
    painter.drawRoundedRect(focusRect, cornerRadius(), cornerRadius());

    painter.restore();
}

}  // namespace FluentQt::Components
