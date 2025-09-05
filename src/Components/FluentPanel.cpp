// src/Components/FluentPanel.cpp
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QFontMetrics>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QtMath>

namespace FluentQt::Components {

FluentPanel::FluentPanel(QWidget* parent)
    : FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setObjectName("FluentPanel");
    setupUI();
    setupAnimations();
    updateColors();
    updatePanelStyle();
    updateAccessibility();
    updateResponsiveState();
    setupKeyboardShortcuts();
}

FluentPanel::FluentPanel(FluentPanelType type, QWidget* parent)
    : FluentPanel(parent) {
    setPanelType(type);
}

FluentPanel::FluentPanel(const QString& title, QWidget* parent)
    : FluentPanel(parent) {
    setTitle(title);
}

FluentPanel::~FluentPanel() = default;

void FluentPanel::setupUI() {
    FLUENT_PROFILE("FluentPanel::setupUI");

    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Content widget
    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("FluentPanelContent");

    // Initially add content widget (title will be added when set)
    m_mainLayout->addWidget(m_contentWidget);

    // Connect theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentPanel::updateColors);
}

void FluentPanel::setupAnimations() {
    FLUENT_PROFILE("FluentPanel::setupAnimations");

    // Collapse animation
    m_collapseAnimation = new QPropertyAnimation(this, "maximumHeight", this);
    m_collapseAnimation->setDuration(ANIMATION_DURATION);
    m_collapseAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_collapseAnimation, &QPropertyAnimation::finished, this,
            &FluentPanel::onCollapseAnimationFinished);

    // Fade animation for content
    m_fadeAnimation =
        new QPropertyAnimation(m_contentWidget, "windowOpacity", this);
    m_fadeAnimation->setDuration(ANIMATION_DURATION / 2);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Shadow effect
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(0);
    m_shadowEffect->setOffset(0, 0);
    m_shadowEffect->setColor(QColor(0, 0, 0, 0));
    setGraphicsEffect(m_shadowEffect);
}

FluentPanelType FluentPanel::panelType() const { return m_panelType; }

void FluentPanel::setPanelType(FluentPanelType type) {
    if (m_panelType != type) {
        m_panelType = type;
        updatePanelStyle();
        updateShadowEffect();
        update();
        emit panelTypeChanged(type);
    }
}

FluentPanelElevation FluentPanel::elevation() const { return m_elevation; }

void FluentPanel::setElevation(FluentPanelElevation elevation) {
    if (m_elevation != elevation) {
        m_elevation = elevation;
        updateShadowEffect();
        update();
        emit elevationChanged(elevation);
    }
}

FluentPanelCornerRadius FluentPanel::cornerRadius() const {
    return m_cornerRadius;
}

void FluentPanel::setCornerRadius(FluentPanelCornerRadius radius) {
    if (m_cornerRadius != radius) {
        m_cornerRadius = radius;
        update();
        emit cornerRadiusChanged(radius);
    }
}

QColor FluentPanel::backgroundColor() const { return m_backgroundColor; }

void FluentPanel::setBackgroundColor(const QColor& color) {
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        update();
        emit backgroundColorChanged(color);
    }
}

QColor FluentPanel::borderColor() const { return m_borderColor; }

void FluentPanel::setBorderColor(const QColor& color) {
    if (m_borderColor != color) {
        m_borderColor = color;
        update();
        emit borderColorChanged(color);
    }
}

int FluentPanel::borderWidth() const { return m_borderWidth; }

void FluentPanel::setBorderWidth(int width) {
    const int clampedWidth = qMax(0, width);
    if (m_borderWidth != clampedWidth) {
        m_borderWidth = clampedWidth;
        update();
        emit borderWidthChanged(clampedWidth);
    }
}

QMargins FluentPanel::padding() const { return m_padding; }

void FluentPanel::setPadding(const QMargins& padding) {
    if (m_padding != padding) {
        m_padding = padding;

        // Update content widget margins
        if (m_contentWidget && m_contentWidget->layout()) {
            m_contentWidget->layout()->setContentsMargins(padding);
        }

        update();
        emit paddingChanged(padding);
    }
}

void FluentPanel::setPadding(int padding) {
    setPadding(QMargins(padding, padding, padding, padding));
}

void FluentPanel::setPadding(int left, int top, int right, int bottom) {
    setPadding(QMargins(left, top, right, bottom));
}

bool FluentPanel::isAnimated() const { return m_animated; }

void FluentPanel::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

// Accessibility methods implementation
QString FluentPanel::accessibleName() const {
    return m_accessibleName.isEmpty() ? m_title : m_accessibleName;
}

void FluentPanel::setAccessibleName(const QString& name) {
    if (m_accessibleName != name) {
        m_accessibleName = name;
        updateAccessibility();
        emit accessibleNameChanged(name);
    }
}

QString FluentPanel::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentPanel::setAccessibleDescription(const QString& description) {
    if (m_accessibleDescription != description) {
        m_accessibleDescription = description;
        updateAccessibility();
        emit accessibleDescriptionChanged(description);
    }
}

QString FluentPanel::accessibleRole() const { return m_accessibleRole; }

void FluentPanel::setAccessibleRole(const QString& role) {
    if (m_accessibleRole != role) {
        m_accessibleRole = role;
        updateAccessibility();
        emit accessibleRoleChanged(role);
    }
}

bool FluentPanel::isKeyboardNavigationEnabled() const {
    return m_keyboardNavigationEnabled;
}

void FluentPanel::setKeyboardNavigationEnabled(bool enabled) {
    if (m_keyboardNavigationEnabled != enabled) {
        m_keyboardNavigationEnabled = enabled;
        setFocusPolicy(enabled ? Qt::StrongFocus : Qt::NoFocus);
        emit keyboardNavigationEnabledChanged(enabled);
    }
}

bool FluentPanel::hasScreenReaderSupport() const {
    return m_screenReaderSupport;
}

void FluentPanel::setScreenReaderSupport(bool enabled) {
    if (m_screenReaderSupport != enabled) {
        m_screenReaderSupport = enabled;
        updateAccessibility();
        emit screenReaderSupportChanged(enabled);
    }
}

int FluentPanel::tabOrder() const { return m_tabOrder; }

void FluentPanel::setTabOrder(int order) {
    if (m_tabOrder != order) {
        m_tabOrder = order;
        // Note: Tab order is managed by the parent widget
        emit tabOrderChanged(order);
    }
}

// Keyboard navigation methods
void FluentPanel::setFocusToContent() {
    if (m_contentWidget && m_keyboardNavigationEnabled) {
        m_contentWidget->setFocus();
    }
}

void FluentPanel::setFocusToTitle() {
    if (m_titleLabel && m_keyboardNavigationEnabled) {
        m_titleLabel->setFocus();
    }
}

bool FluentPanel::handleKeyboardNavigation(QKeyEvent* event) {
    if (!m_keyboardNavigationEnabled || !event) {
        return false;
    }

    switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (m_collapsible) {
                toggleCollapsed();
                return true;
            }
            break;
        case Qt::Key_Tab:
            if (m_cycleFocusEnabled) {
                cycleFocus(!(event->modifiers() & Qt::ShiftModifier));
                return true;
            }
            break;
        case Qt::Key_Escape:
            if (hasFocus()) {
                clearFocus();
                return true;
            }
            break;
        default:
            // Check for custom keyboard shortcuts
            QString keyString =
                QKeySequence(event->key() | event->modifiers()).toString();
            if (handleKeyboardShortcut(keyString)) {
                return true;
            }
            break;
    }

    return false;
}

QString FluentPanel::title() const { return m_title; }

void FluentPanel::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        updateTitleVisibility();
        update();
        emit titleChanged(title);
    }
}

bool FluentPanel::isCollapsible() const { return m_collapsible; }

void FluentPanel::setCollapsible(bool collapsible) {
    if (m_collapsible != collapsible) {
        m_collapsible = collapsible;
        updateTitleVisibility();
        emit collapsibleChanged(collapsible);
    }
}

bool FluentPanel::isCollapsed() const { return m_collapsed; }

void FluentPanel::setCollapsed(bool collapsed) {
    if (m_collapsed != collapsed) {
        m_collapsed = collapsed;

        if (m_animated) {
            if (collapsed) {
                startCollapseAnimation();
            } else {
                startExpandAnimation();
            }
        } else {
            m_contentWidget->setVisible(!collapsed);
        }

        emit collapsedChanged(collapsed);
    }
}

void FluentPanel::setLayout(QLayout* layout) {
    if (m_contentWidget) {
        m_contentWidget->setLayout(layout);
        if (layout) {
            layout->setContentsMargins(m_padding);
        }
    }
}

QLayout* FluentPanel::layout() const {
    return m_contentWidget ? m_contentWidget->layout() : nullptr;
}

QVBoxLayout* FluentPanel::createVerticalLayout() {
    auto* layout = new QVBoxLayout();
    setLayout(layout);
    return layout;
}

QHBoxLayout* FluentPanel::createHorizontalLayout() {
    auto* layout = new QHBoxLayout();
    setLayout(layout);
    return layout;
}

QGridLayout* FluentPanel::createGridLayout() {
    auto* layout = new QGridLayout();
    setLayout(layout);
    return layout;
}

QStackedLayout* FluentPanel::createStackedLayout() {
    auto* layout = new QStackedLayout();
    setLayout(layout);
    return layout;
}

QWidget* FluentPanel::contentWidget() const { return m_contentWidget; }

void FluentPanel::setContentWidget(QWidget* widget) {
    if (widget && widget != m_contentWidget) {
        // Remove old content widget
        if (m_contentWidget) {
            m_mainLayout->removeWidget(m_contentWidget);
            m_contentWidget->deleteLater();
        }

        // Set new content widget
        m_contentWidget = widget;
        m_contentWidget->setParent(this);

        // Add to layout (after title if present)
        if (m_titleLabel) {
            m_mainLayout->addWidget(m_contentWidget);
        } else {
            m_mainLayout->insertWidget(0, m_contentWidget);
        }
    }
}

QSize FluentPanel::sizeHint() const {
    QSize hint = FluentComponent::sizeHint();

    // Add title height if present
    if (!m_title.isEmpty()) {
        hint.setHeight(hint.height() + getTitleHeight());
    }

    // Add padding
    hint += QSize(m_padding.left() + m_padding.right(),
                  m_padding.top() + m_padding.bottom());

    return hint;
}

QSize FluentPanel::minimumSizeHint() const {
    QSize hint = FluentComponent::minimumSizeHint();

    // Add title height if present
    if (!m_title.isEmpty()) {
        hint.setHeight(hint.height() + getTitleHeight());
    }

    // Add padding
    hint += QSize(m_padding.left() + m_padding.right(),
                  m_padding.top() + m_padding.bottom());

    return hint;
}

void FluentPanel::expand() { setCollapsed(false); }

void FluentPanel::collapse() { setCollapsed(true); }

void FluentPanel::toggleCollapsed() { setCollapsed(!m_collapsed); }

void FluentPanel::animateIn() {
    if (m_animator) {
        m_animator->fadeIn(this);
    }
}

void FluentPanel::animateOut() {
    if (m_animator) {
        m_animator->fadeOut(this);
    }
}

void FluentPanel::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawPanel(painter);
}

void FluentPanel::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);

    // Store expanded height for collapse animation
    if (!m_collapsed) {
        m_expandedHeight = height();
    }

    // Update responsive layout
    if (m_responsiveEnabled) {
        updateResponsiveLayout();
    }
}

void FluentPanel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;

        // Trigger press effects
        if (m_pressAnimationsEnabled) {
            triggerPressEffect();
        }

        // Check if click is on title area and panel is collapsible
        if (m_collapsible && !m_title.isEmpty()) {
            const QRect titleRect = getTitleRect();
            if (titleRect.contains(event->pos())) {
                // Will handle in mouseReleaseEvent
                return;
            }
        }

        update();
    }

    FluentComponent::mousePressEvent(event);
}

void FluentPanel::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;

        // Check if click is on title area and panel is collapsible
        if (m_collapsible && !m_title.isEmpty()) {
            const QRect titleRect = getTitleRect();
            if (titleRect.contains(event->pos())) {
                toggleCollapsed();
                return;
            }
        }

        emit clicked();
        update();
    }

    FluentComponent::mouseReleaseEvent(event);
}

void FluentPanel::enterEvent(QEnterEvent* event) {
    m_isHovered = true;

    // Trigger hover effects
    if (m_hoverEffectsEnabled) {
        triggerHoverEffect();
    }

    update();
    FluentComponent::enterEvent(event);
}

void FluentPanel::leaveEvent(QEvent* event) {
    m_isHovered = false;
    m_isPressed = false;
    update();
    FluentComponent::leaveEvent(event);
}

void FluentPanel::changeEvent(QEvent* event) {
    FluentComponent::changeEvent(event);

    if (event->type() == QEvent::FontChange) {
        updateTitleVisibility();
    }
}

// Override focus events to add focus effects
void FluentPanel::focusInEvent(QFocusEvent* event) {
    FluentComponent::focusInEvent(event);

    if (m_focusIndicatorVisible) {
        triggerFocusEffect();
    }

    emit focusChanged(true);
}

void FluentPanel::focusOutEvent(QFocusEvent* event) {
    FluentComponent::focusOutEvent(event);

    update();  // Remove focus indicator
    emit focusChanged(false);
}

// Override key press event for keyboard navigation
void FluentPanel::keyPressEvent(QKeyEvent* event) {
    if (handleKeyboardNavigation(event)) {
        event->accept();
        return;
    }

    FluentComponent::keyPressEvent(event);
}

void FluentPanel::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    // Set default colors based on panel type if not explicitly set
    if (!m_backgroundColor.isValid()) {
        switch (m_panelType) {
            case FluentPanelType::Card:
                m_backgroundColor =
                    theme.color("cardBackgroundFillColorDefault");
                break;
            case FluentPanelType::Surface:
                m_backgroundColor = theme.color("layerFillColorDefault");
                break;
            case FluentPanelType::Acrylic:
                m_backgroundColor =
                    theme.color("acrylicBackgroundFillColorDefault");
                break;
            case FluentPanelType::Mica:
                m_backgroundColor = theme.color("micaBackgroundFillColorBase");
                break;
            case FluentPanelType::Sidebar:
                m_backgroundColor = theme.color("layerFillColorAlt");
                break;
            case FluentPanelType::Content:
                m_backgroundColor = theme.color("backgroundPrimary");
                break;
            case FluentPanelType::Dialog:
                m_backgroundColor = theme.color("solidBackgroundFillColorBase");
                break;
            case FluentPanelType::Flyout:
                m_backgroundColor = theme.color("flyoutBackgroundFillColor");
                break;
        }
    }

    if (!m_borderColor.isValid()) {
        m_borderColor = theme.color("controlStrokeDefault");
    }

    update();
}

void FluentPanel::onCollapseAnimationFinished() {
    if (m_collapsed) {
        m_contentWidget->setVisible(false);
    }
}

void FluentPanel::onExpandAnimationFinished() {
    // Animation complete
}

void FluentPanel::updateShadowEffect() {
    if (!m_shadowEffect)
        return;

    const auto& theme = Styling::FluentTheme::instance();

    // Configure shadow based on elevation and panel type
    int blurRadius = 0;
    int offsetY = 0;
    int alpha = 0;

    if (m_panelType == FluentPanelType::Card ||
        m_panelType == FluentPanelType::Dialog ||
        m_panelType == FluentPanelType::Flyout) {
        switch (m_elevation) {
            case FluentPanelElevation::None:
                blurRadius = 0;
                offsetY = 0;
                alpha = 0;
                break;
            case FluentPanelElevation::Low:
                blurRadius = 4;
                offsetY = 1;
                alpha = 20;
                break;
            case FluentPanelElevation::Medium:
                blurRadius = 8;
                offsetY = 2;
                alpha = 30;
                break;
            case FluentPanelElevation::High:
                blurRadius = 16;
                offsetY = 4;
                alpha = 40;
                break;
            case FluentPanelElevation::VeryHigh:
                blurRadius = 24;
                offsetY = 8;
                alpha = 50;
                break;
        }
    }

    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(0, offsetY);
    m_shadowEffect->setColor(QColor(0, 0, 0, alpha));
}

void FluentPanel::updatePanelStyle() {
    // Update default elevation based on panel type
    switch (m_panelType) {
        case FluentPanelType::Card:
            if (m_elevation == FluentPanelElevation::Medium) {
                setElevation(FluentPanelElevation::Low);
            }
            break;
        case FluentPanelType::Surface:
            setElevation(FluentPanelElevation::None);
            break;
        case FluentPanelType::Dialog:
            if (m_elevation == FluentPanelElevation::Medium) {
                setElevation(FluentPanelElevation::High);
            }
            break;
        case FluentPanelType::Flyout:
            if (m_elevation == FluentPanelElevation::Medium) {
                setElevation(FluentPanelElevation::Medium);
            }
            break;
        default:
            break;
    }

    updateColors();
}

void FluentPanel::updateTitleVisibility() {
    const bool shouldShowTitle = !m_title.isEmpty();

    if (shouldShowTitle && !m_titleLabel) {
        // Create title label
        m_titleLabel = new QLabel(m_title, this);
        m_titleLabel->setObjectName("FluentPanelTitle");

        const auto& theme = Styling::FluentTheme::instance();
        m_titleLabel->setFont(theme.subtitleFont());
        m_titleLabel->setStyleSheet(
            QString("color: %1; padding: 8px 16px;")
                .arg(theme.color("textPrimary").name()));

        // Create title layout with collapse button if collapsible
        auto* titleWidget = new QWidget(this);
        auto* titleLayout = new QHBoxLayout(titleWidget);
        titleLayout->setContentsMargins(0, 0, 0, 0);
        titleLayout->addWidget(m_titleLabel);

        if (m_collapsible) {
            m_collapseButton = new QPushButton(this);
            m_collapseButton->setObjectName("FluentPanelCollapseButton");
            m_collapseButton->setFixedSize(COLLAPSE_BUTTON_SIZE,
                                           COLLAPSE_BUTTON_SIZE);
            m_collapseButton->setText(m_collapsed ? "▶" : "▼");

            connect(m_collapseButton, &QPushButton::clicked, this,
                    &FluentPanel::toggleCollapsed);
            connect(this, &FluentPanel::collapsedChanged,
                    [this](bool collapsed) {
                        if (m_collapseButton) {
                            m_collapseButton->setText(collapsed ? "▶" : "▼");
                        }
                    });

            titleLayout->addWidget(m_collapseButton);
        }

        titleLayout->addStretch();

        // Insert title at the beginning
        m_mainLayout->insertWidget(0, titleWidget);

    } else if (!shouldShowTitle && m_titleLabel) {
        // Remove title
        m_mainLayout->removeWidget(m_titleLabel->parentWidget());
        m_titleLabel->parentWidget()->deleteLater();
        m_titleLabel = nullptr;
        m_collapseButton = nullptr;
    } else if (shouldShowTitle && m_titleLabel) {
        // Update existing title
        m_titleLabel->setText(m_title);
    }
}

void FluentPanel::drawPanel(QPainter& painter) {
    const QRect rect = this->rect();

    // Draw background
    drawBackground(painter, rect);

    // Draw border
    if (m_borderWidth > 0) {
        drawBorder(painter, rect);
    }

    // Draw focus indicator
    if (hasFocus() && m_focusIndicatorVisible) {
        drawFocusIndicator(painter, rect);
    }
}

void FluentPanel::drawBackground(QPainter& painter, const QRect& rect) {
    const int radius = getCornerRadiusPixels();
    const QColor bgColor = getBackgroundColor();

    if (radius > 0) {
        QPainterPath path;
        path.addRoundedRect(rect, radius, radius);
        painter.fillPath(path, bgColor);
    } else {
        painter.fillRect(rect, bgColor);
    }
}

void FluentPanel::drawBorder(QPainter& painter, const QRect& rect) {
    const int radius = getCornerRadiusPixels();
    const QColor borderColor = getBorderColor();

    painter.setPen(QPen(borderColor, m_borderWidth));

    if (radius > 0) {
        QPainterPath path;
        const QRectF borderRect =
            rect.adjusted(m_borderWidth / 2.0, m_borderWidth / 2.0,
                          -m_borderWidth / 2.0, -m_borderWidth / 2.0);
        path.addRoundedRect(borderRect, radius, radius);
        painter.drawPath(path);
    } else {
        const QRect borderRect =
            rect.adjusted(m_borderWidth / 2, m_borderWidth / 2,
                          -m_borderWidth / 2, -m_borderWidth / 2);
        painter.drawRect(borderRect);
    }
}

QRect FluentPanel::getTitleRect() const {
    if (m_title.isEmpty()) {
        return QRect();
    }
    return QRect(0, 0, width(), getTitleHeight());
}

QRect FluentPanel::getContentRect() const {
    const int titleHeight = m_title.isEmpty() ? 0 : getTitleHeight();
    return QRect(0, titleHeight, width(), height() - titleHeight);
}

int FluentPanel::getTitleHeight() const { return TITLE_HEIGHT; }

int FluentPanel::getCornerRadiusPixels() const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_cornerRadius) {
        case FluentPanelCornerRadius::None:
            return 0;
        case FluentPanelCornerRadius::Small:
            return theme.borderRadius("small");
        case FluentPanelCornerRadius::Medium:
            return theme.borderRadius("medium");
        case FluentPanelCornerRadius::Large:
            return theme.borderRadius("large");
        case FluentPanelCornerRadius::ExtraLarge:
            return theme.borderRadius("extraLarge");
    }
    return theme.borderRadius("medium");
}

QColor FluentPanel::getBackgroundColor() const {
    if (m_backgroundColor.isValid()) {
        return m_backgroundColor;
    }
    return Styling::FluentTheme::instance().color(
        "cardBackgroundFillColorDefault");
}

QColor FluentPanel::getBorderColor() const {
    if (m_borderColor.isValid()) {
        return m_borderColor;
    }
    return Styling::FluentTheme::instance().color("controlStrokeDefault");
}

void FluentPanel::startCollapseAnimation() {
    if (!m_animated) {
        m_contentWidget->setVisible(false);
        return;
    }

    m_expandedHeight = height();
    m_collapsedHeight = getTitleHeight();

    m_collapseAnimation->setStartValue(m_expandedHeight);
    m_collapseAnimation->setEndValue(m_collapsedHeight);
    m_collapseAnimation->start();
}

void FluentPanel::startExpandAnimation() {
    if (!m_animated) {
        m_contentWidget->setVisible(true);
        return;
    }

    m_contentWidget->setVisible(true);

    if (m_expandedHeight == 0) {
        m_expandedHeight = sizeHint().height();
    }

    m_collapseAnimation->setStartValue(height());
    m_collapseAnimation->setEndValue(m_expandedHeight);
    m_collapseAnimation->start();
}

// Responsive design methods implementation
bool FluentPanel::isResponsiveEnabled() const { return m_responsiveEnabled; }

void FluentPanel::setResponsiveEnabled(bool enabled) {
    if (m_responsiveEnabled != enabled) {
        m_responsiveEnabled = enabled;
        updateResponsiveState();
        emit responsiveEnabledChanged(enabled);
    }
}

QSize FluentPanel::minimumContentSize() const { return m_minimumContentSize; }

void FluentPanel::setMinimumContentSize(const QSize& size) {
    if (m_minimumContentSize != size) {
        m_minimumContentSize = size;
        if (m_contentWidget) {
            m_contentWidget->setMinimumSize(size);
        }
        emit minimumContentSizeChanged(size);
    }
}

QSize FluentPanel::maximumContentSize() const { return m_maximumContentSize; }

void FluentPanel::setMaximumContentSize(const QSize& size) {
    if (m_maximumContentSize != size) {
        m_maximumContentSize = size;
        if (m_contentWidget) {
            m_contentWidget->setMaximumSize(size);
        }
        emit maximumContentSizeChanged(size);
    }
}

bool FluentPanel::hasAdaptiveLayout() const { return m_adaptiveLayout; }

void FluentPanel::setAdaptiveLayout(bool adaptive) {
    if (m_adaptiveLayout != adaptive) {
        m_adaptiveLayout = adaptive;
        updateResponsiveState();
        emit adaptiveLayoutChanged(adaptive);
    }
}

bool FluentPanel::isTouchOptimized() const { return m_touchOptimized; }

void FluentPanel::setTouchOptimized(bool optimized) {
    if (m_touchOptimized != optimized) {
        m_touchOptimized = optimized;
        updateResponsiveState();
        emit touchOptimizedChanged(optimized);
    }
}

int FluentPanel::breakpointWidth() const { return m_breakpointWidth; }

void FluentPanel::setBreakpointWidth(int width) {
    if (m_breakpointWidth != width) {
        m_breakpointWidth = width;
        updateResponsiveState();
        emit breakpointWidthChanged(width);
    }
}

void FluentPanel::updateResponsiveLayout() {
    if (!m_responsiveEnabled) {
        return;
    }

    bool wasCompact = m_compactMode;
    m_compactMode = isInCompactBreakpoint();

    if (wasCompact != m_compactMode) {
        emit compactModeChanged(m_compactMode);

        // Adjust padding for compact mode
        if (m_compactMode) {
            setPadding(8, 8, 8, 8);
        } else {
            setPadding(16, 16, 16, 16);
        }

        // Update title height for touch optimization
        if (m_touchOptimized && m_compactMode) {
            // Increase touch targets in compact mode
            if (m_collapseButton) {
                m_collapseButton->setFixedSize(32, 32);
            }
        }
    }
}

bool FluentPanel::isCompactMode() const { return m_compactMode; }

void FluentPanel::setCompactMode(bool compact) {
    if (m_compactMode != compact) {
        m_compactMode = compact;
        updateResponsiveLayout();
        emit compactModeChanged(compact);
    }
}

// Enhanced functionality helper methods
void FluentPanel::updateAccessibility() {
    if (!m_screenReaderSupport) {
        return;
    }

    // Set accessible properties
    setAccessibleName(accessibleName());
    setAccessibleDescription(m_accessibleDescription);

    // Set accessible role using property
    if (m_accessibleRole == "panel") {
        setProperty("accessibleRole", QAccessible::Pane);
    } else if (m_accessibleRole == "region") {
        setProperty("accessibleRole", QAccessible::Section);
    } else if (m_accessibleRole == "group") {
        setProperty("accessibleRole", QAccessible::Grouping);
    }

    // Update collapsible state for screen readers
    if (m_collapsible) {
        setProperty("aria-expanded", !m_collapsed);
    }
}

void FluentPanel::updateResponsiveState() {
    if (m_responsiveEnabled) {
        updateResponsiveLayout();
    }
}

void FluentPanel::updateVisualEffects() {
    // This will be called when visual state changes
    update();
}

void FluentPanel::updateKeyboardShortcuts() {
    // Clear existing shortcuts
    m_keyboardShortcuts.clear();

    if (m_keyboardShortcutsEnabled) {
        // Register default shortcuts
        if (!m_expandShortcut.isEmpty()) {
            m_keyboardShortcuts[m_expandShortcut] = "expand";
        }
        if (!m_collapseShortcut.isEmpty()) {
            m_keyboardShortcuts[m_collapseShortcut] = "collapse";
        }
    }
}

bool FluentPanel::isInCompactBreakpoint() const {
    return width() < m_breakpointWidth;
}

void FluentPanel::setupKeyboardShortcuts() { updateKeyboardShortcuts(); }

void FluentPanel::handleAccessibilityEvent(QAccessibleEvent* event) {
    if (!m_screenReaderSupport || !event) {
        return;
    }

    // Handle accessibility events as needed
    // This is a placeholder for future accessibility enhancements
}

void FluentPanel::drawFocusIndicator(QPainter& painter, const QRect& rect) {
    if (!m_focusIndicatorVisible) {
        return;
    }

    const QColor focusColor = focusIndicatorColor();
    const int width = m_focusIndicatorWidth;
    const int radius = getCornerRadiusPixels();

    painter.save();
    painter.setPen(QPen(focusColor, width));
    painter.setBrush(Qt::NoBrush);

    // Draw focus indicator as an outline
    if (radius > 0) {
        QPainterPath path;
        const QRectF focusRect =
            rect.adjusted(width / 2.0, width / 2.0, -width / 2.0, -width / 2.0);
        path.addRoundedRect(focusRect, radius, radius);
        painter.drawPath(path);
    } else {
        const QRect focusRect =
            rect.adjusted(width / 2, width / 2, -width / 2, -width / 2);
        painter.drawRect(focusRect);
    }

    painter.restore();
}

void FluentPanel::applyHoverEffect() {
    if (!m_hoverEffectsEnabled) {
        return;
    }

    // Apply hover opacity effect
    setWindowOpacity(m_hoverOpacity);
}

void FluentPanel::applyPressEffect() {
    if (!m_pressAnimationsEnabled) {
        return;
    }

    // Apply press scale effect (simplified implementation)
    // In a real implementation, this would use QGraphicsEffect or similar
    update();
}

void FluentPanel::resetToNormalState() {
    setWindowOpacity(1.0);
    update();
}

// Enhanced keyboard navigation methods
bool FluentPanel::areKeyboardShortcutsEnabled() const {
    return m_keyboardShortcutsEnabled;
}

void FluentPanel::setKeyboardShortcutsEnabled(bool enabled) {
    if (m_keyboardShortcutsEnabled != enabled) {
        m_keyboardShortcutsEnabled = enabled;
        updateKeyboardShortcuts();
        emit keyboardShortcutsEnabledChanged(enabled);
    }
}

QString FluentPanel::expandShortcut() const { return m_expandShortcut; }

void FluentPanel::setExpandShortcut(const QString& shortcut) {
    if (m_expandShortcut != shortcut) {
        m_expandShortcut = shortcut;
        updateKeyboardShortcuts();
        emit expandShortcutChanged(shortcut);
    }
}

QString FluentPanel::collapseShortcut() const { return m_collapseShortcut; }

void FluentPanel::setCollapseShortcut(const QString& shortcut) {
    if (m_collapseShortcut != shortcut) {
        m_collapseShortcut = shortcut;
        updateKeyboardShortcuts();
        emit collapseShortcutChanged(shortcut);
    }
}

bool FluentPanel::isCycleFocusEnabled() const { return m_cycleFocusEnabled; }

void FluentPanel::setCycleFocusEnabled(bool enabled) {
    if (m_cycleFocusEnabled != enabled) {
        m_cycleFocusEnabled = enabled;
        emit cycleFocusEnabledChanged(enabled);
    }
}

void FluentPanel::registerKeyboardShortcut(const QString& key,
                                           const QString& action) {
    m_keyboardShortcuts[key] = action;
}

void FluentPanel::unregisterKeyboardShortcut(const QString& key) {
    m_keyboardShortcuts.remove(key);
}

bool FluentPanel::handleKeyboardShortcut(const QString& key) {
    if (!m_keyboardShortcutsEnabled || !m_keyboardShortcuts.contains(key)) {
        return false;
    }

    const QString action = m_keyboardShortcuts[key];

    if (action == "expand" && m_collapsible && m_collapsed) {
        expand();
        emit keyboardShortcutTriggered(action);
        return true;
    } else if (action == "collapse" && m_collapsible && !m_collapsed) {
        collapse();
        emit keyboardShortcutTriggered(action);
        return true;
    }

    return false;
}

void FluentPanel::cycleFocus(bool forward) {
    if (!m_cycleFocusEnabled) {
        return;
    }

    QWidget* nextWidget = getNextFocusableWidget(forward);
    if (nextWidget) {
        nextWidget->setFocus();
    }
}

QWidget* FluentPanel::getNextFocusableWidget(bool forward) const {
    // Simple implementation - can be enhanced
    QList<QWidget*> focusableWidgets;

    if (m_titleLabel && m_titleLabel->focusPolicy() != Qt::NoFocus) {
        focusableWidgets.append(m_titleLabel);
    }

    if (m_collapseButton && m_collapseButton->focusPolicy() != Qt::NoFocus) {
        focusableWidgets.append(m_collapseButton);
    }

    if (m_contentWidget && m_contentWidget->focusPolicy() != Qt::NoFocus) {
        focusableWidgets.append(m_contentWidget);
    }

    if (focusableWidgets.isEmpty()) {
        return nullptr;
    }

    QWidget* currentFocus = QApplication::focusWidget();
    int currentIndex = focusableWidgets.indexOf(currentFocus);

    if (currentIndex == -1) {
        return focusableWidgets.first();
    }

    if (forward) {
        return focusableWidgets[(currentIndex + 1) % focusableWidgets.size()];
    } else {
        return focusableWidgets[(currentIndex - 1 + focusableWidgets.size()) %
                                focusableWidgets.size()];
    }
}

// Enhanced visual effects implementation
bool FluentPanel::isFocusIndicatorVisible() const {
    return m_focusIndicatorVisible;
}

void FluentPanel::setFocusIndicatorVisible(bool visible) {
    if (m_focusIndicatorVisible != visible) {
        m_focusIndicatorVisible = visible;
        update();
        emit focusIndicatorVisibleChanged(visible);
    }
}

QColor FluentPanel::focusIndicatorColor() const {
    if (m_focusIndicatorColor.isValid()) {
        return m_focusIndicatorColor;
    }
    return Styling::FluentTheme::instance().color("accentDefault");
}

void FluentPanel::setFocusIndicatorColor(const QColor& color) {
    if (m_focusIndicatorColor != color) {
        m_focusIndicatorColor = color;
        update();
        emit focusIndicatorColorChanged(color);
    }
}

int FluentPanel::focusIndicatorWidth() const { return m_focusIndicatorWidth; }

void FluentPanel::setFocusIndicatorWidth(int width) {
    if (m_focusIndicatorWidth != width) {
        m_focusIndicatorWidth = width;
        update();
        emit focusIndicatorWidthChanged(width);
    }
}

bool FluentPanel::areHoverEffectsEnabled() const {
    return m_hoverEffectsEnabled;
}

void FluentPanel::setHoverEffectsEnabled(bool enabled) {
    if (m_hoverEffectsEnabled != enabled) {
        m_hoverEffectsEnabled = enabled;
        emit hoverEffectsEnabledChanged(enabled);
    }
}

bool FluentPanel::arePressAnimationsEnabled() const {
    return m_pressAnimationsEnabled;
}

void FluentPanel::setPressAnimationsEnabled(bool enabled) {
    if (m_pressAnimationsEnabled != enabled) {
        m_pressAnimationsEnabled = enabled;
        emit pressAnimationsEnabledChanged(enabled);
    }
}

qreal FluentPanel::hoverOpacity() const { return m_hoverOpacity; }

void FluentPanel::setHoverOpacity(qreal opacity) {
    if (m_hoverOpacity != opacity) {
        m_hoverOpacity = qBound(0.0, opacity, 1.0);
        emit hoverOpacityChanged(m_hoverOpacity);
    }
}

qreal FluentPanel::pressedScale() const { return m_pressedScale; }

void FluentPanel::setPressedScale(qreal scale) {
    if (m_pressedScale != scale) {
        m_pressedScale = qBound(0.1, scale, 1.0);
        emit pressedScaleChanged(m_pressedScale);
    }
}

void FluentPanel::triggerHoverEffect() {
    if (!m_hoverEffectsEnabled || !m_animator) {
        return;
    }

    auto hoverAnimation = m_animator->hoverEffect(this);
    if (hoverAnimation) {
        hoverAnimation->start();
    }
}

void FluentPanel::triggerPressEffect() {
    if (!m_pressAnimationsEnabled || !m_animator) {
        return;
    }

    auto pressAnimation = m_animator->pressEffect(this);
    if (pressAnimation) {
        pressAnimation->start();
    }
}

void FluentPanel::triggerFocusEffect() {
    if (!m_focusIndicatorVisible) {
        return;
    }

    update();  // Redraw to show focus indicator
}

void FluentPanel::resetVisualEffects() {
    // Reset any ongoing visual effects
    setWindowOpacity(1.0);

    update();
}

}  // namespace FluentQt::Components
