// src/Components/FluentCard.cpp
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>
#include <algorithm>

namespace FluentQt::Components {

// FluentCard Implementation
FluentCard::FluentCard(QWidget* parent)
    : Core::FluentComponent(parent),
      m_header(std::make_unique<FluentCardHeader>(this)),
      m_content(std::make_unique<FluentCardContent>(this)),
      m_footer(std::make_unique<FluentCardFooter>(this)) {
    setupLayout();
    setupAnimations();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentCard::onThemeChanged);

    // Connect header signals
    connect(m_header.get(), &FluentCardHeader::clicked, this,
            &FluentCard::headerClicked);

    updateShadowEffect();
    setCornerRadius(getFluentCornerRadius());

    // Apply FluentUI smart defaults
    applyFluentDefaults();

    // Enhanced accessibility setup
    setupAccessibility();

    // Set initial accessibility
    Accessibility::setAccessibleName(this, "Card");
    Accessibility::setAccessibleRole(this, QAccessible::Grouping);
}

FluentCard::FluentCard(const QString& title, QWidget* parent)
    : FluentCard(parent) {
    setTitle(title);
}

FluentCard::FluentCard(const QString& title, const QString& subtitle,
                       QWidget* parent)
    : FluentCard(parent) {
    setTitle(title);
    setSubtitle(subtitle);
}

FluentCard::~FluentCard() {
    // Clean up any running animations
    for (auto* anim : m_currentAnimations) {
        if (anim) {
            anim->stop();
            anim->deleteLater();
        }
    }
    m_currentAnimations.clear();
}

void FluentCard::setTitle(const QString& title) {
    if (m_header->title() != title) {
        m_header->setTitle(title);

        // Update accessibility
        QString accessibleName = title.isEmpty() ? "Card" : title;
        Accessibility::setAccessibleName(this, accessibleName);

        emit titleChanged(title);
    }
}

QString FluentCard::title() const { return m_header->title(); }

void FluentCard::setSubtitle(const QString& subtitle) {
    if (m_header->subtitle() != subtitle) {
        m_header->setSubtitle(subtitle);
        emit subtitleChanged(subtitle);
    }
}

QString FluentCard::subtitle() const { return m_header->subtitle(); }

void FluentCard::setHeaderIcon(const QIcon& icon) {
    if (m_header->icon().cacheKey() != icon.cacheKey()) {
        m_header->setIcon(icon);
        emit headerIconChanged(icon);
    }
}

QIcon FluentCard::headerIcon() const { return m_header->icon(); }

void FluentCard::setHeaderVisible(bool visible) {
    if (m_headerVisible != visible) {
        m_headerVisible = visible;
        updateHeaderVisibility();
    }
}

void FluentCard::setElevation(FluentCardElevation elevation) {
    if (m_elevation != elevation) {
        FluentCardElevation oldElevation = m_elevation;
        m_elevation = elevation;

        if (isAnimated()) {
            animateElevation(oldElevation, elevation);
        } else {
            updateShadowEffect();
        }

        emit elevationChanged(elevation);
    }
}

void FluentCard::setCardStyle(FluentCardStyle style) {
    if (m_cardStyle != style) {
        m_cardStyle = style;
        updateStateStyle();
        emit cardStyleChanged(style);
    }
}

void FluentCard::setShadowOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_shadowOpacity, opacity)) {
        m_shadowOpacity = qBound(0.0, opacity, 1.0);
        updateShadowEffect();
    }
}

void FluentCard::setExpansionProgress(qreal progress) {
    if (!qFuzzyCompare(m_expansionProgress, progress)) {
        m_expansionProgress = qBound(0.0, progress, 1.0);

        // Update height based on expansion progress
        if (m_expandedHeight > 0 && m_collapsedHeight > 0) {
            const int targetHeight =
                m_collapsedHeight +
                static_cast<int>((m_expandedHeight - m_collapsedHeight) *
                                 m_expansionProgress);
            setFixedHeight(targetHeight);
        }

        update();
    }
}

QColor FluentCard::backgroundColor() const {
    return m_backgroundColor.isValid() ? m_backgroundColor
                                       : getBackgroundColor();
}

void FluentCard::setBackgroundColor(const QColor& color) {
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        update();
    }
}

void FluentCard::setSelectable(bool selectable) {
    if (m_selectable != selectable) {
        m_selectable = selectable;

        if (selectable) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
            setSelected(false);
        }

        updateStateStyle();
    }
}

void FluentCard::setSelected(bool selected) {
    if (m_selectable && m_selected != selected) {
        m_selected = selected;
        updateStateStyle();

        // Update accessibility
        QString description = title();
        if (selected) {
            description += " (selected)";
        }
        Accessibility::setAccessibleDescription(this, description);

        emit selectedChanged(selected);
    }
}

void FluentCard::setExpandable(bool expandable) {
    if (m_expandable != expandable) {
        m_expandable = expandable;

        if (!expandable) {
            setExpanded(true);
        }

        updateStateStyle();
    }
}

void FluentCard::setExpanded(bool expanded) {
    if (m_expandable && m_expanded != expanded) {
        if (isAnimated()) {
            if (expanded) {
                expandWithAnimation();
            } else {
                collapseWithAnimation();
            }
        } else {
            m_expanded = expanded;
            updateExpansionState();
            emit expandedChanged(expanded);
        }
    }
}

void FluentCard::setFooterVisible(bool visible) {
    if (m_footerVisible != visible) {
        m_footerVisible = visible;
        updateFooterVisibility();
    }
}

void FluentCard::setContentWidget(QWidget* widget) {
    m_content->setContentWidget(widget);
    updateGeometry();
}

QWidget* FluentCard::contentWidget() const {
    return m_content->contentWidget();
}

void FluentCard::addHeaderAction(QAction* action) {
    m_header->addAction(action);
}

void FluentCard::removeHeaderAction(QAction* action) {
    m_header->removeAction(action);
}

void FluentCard::clearHeaderActions() { m_header->clearActions(); }

void FluentCard::addFooterWidget(QWidget* widget) {
    m_footer->addWidget(widget);
    if (!m_footerVisible) {
        setFooterVisible(true);
    }
}

void FluentCard::removeFooterWidget(QWidget* widget) {
    m_footer->removeWidget(widget);
}

void FluentCard::clearFooterWidgets() { m_footer->clearWidgets(); }

QLayout* FluentCard::contentLayout() const {
    // Create content widget and layout on-demand if needed
    if (!m_content->widget()) {
        auto* contentWidget = new QWidget();
        auto* layout = new QVBoxLayout(contentWidget);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(8);

        // Cast away const to modify the content widget
        const_cast<FluentCardContent*>(m_content.get())
            ->setContentWidget(contentWidget);
    }

    return m_content->widget() ? m_content->widget()->layout() : nullptr;
}

QLayout* FluentCard::headerLayout() const { return m_header->layout(); }

QLayout* FluentCard::footerLayout() const { return m_footer->layout(); }

QSize FluentCard::sizeHint() const {
    FLUENT_PROFILE("FluentCard::sizeHint");

    int width = 300;  // Default card width
    int height = 0;

    // Header height
    if (m_headerVisible) {
        height += m_header->sizeHint().height();
    }

    // Content height
    if (m_expanded) {
        height += m_content->sizeHint().height();
    } else {
        height += m_collapsedHeight;
    }

    // Footer height
    if (m_footerVisible) {
        height += m_footer->sizeHint().height();
    }

    // Add margins
    const int margin = 16;
    height += margin;

    return QSize(width, height);
}

QSize FluentCard::minimumSizeHint() const { return QSize(200, 100); }

void FluentCard::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentCard::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    if (m_cardStyle == FluentCardStyle::Outlined || m_selected) {
        paintBorder(painter, rect);
    }

    // Paint selection highlight
    if (m_selected) {
        paintSelection(painter, rect);
    }
}

void FluentCard::paintBackground(QPainter& painter, const QRect& rect) {
    const QColor backgroundColor = getBackgroundColor();

    if (backgroundColor.alpha() == 0) {
        return;
    }

    // Use FluentUI-compliant corner radius
    const int fluentRadius = getFluentCornerRadius();
    QPainterPath path;
    path.addRoundedRect(rect, fluentRadius, fluentRadius);

    // Add subtle gradient for depth
    if (m_cardStyle == FluentCardStyle::Elevated) {
        QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
        gradient.setColorAt(0.0, backgroundColor.lighter(102));
        gradient.setColorAt(1.0, backgroundColor.darker(102));
        painter.fillPath(path, gradient);
    } else {
        painter.fillPath(path, backgroundColor);
    }
}

void FluentCard::paintBorder(QPainter& painter, const QRect& rect) {
    painter.save();

    const QPen borderPen = getBorderPen();
    painter.setPen(borderPen);

    const qreal adjustment = borderPen.widthF() / 2.0;
    const QRectF borderRect =
        rect.adjusted(adjustment, adjustment, -adjustment, -adjustment);

    // Use FluentUI-compliant corner radius
    const int fluentRadius = getFluentCornerRadius();
    painter.drawRoundedRect(borderRect, fluentRadius, fluentRadius);

    painter.restore();
}

void FluentCard::paintSelection(QPainter& painter, const QRect& rect) {
    if (!m_selected) {
        return;
    }

    painter.save();

    const QColor selectionColor = getSelectionColor();
    QPen selectionPen(selectionColor, 2);
    painter.setPen(selectionPen);

    const QRectF selectionRect = rect.adjusted(1, 1, -1, -1);
    // Use FluentUI-compliant corner radius
    const int fluentRadius = getFluentCornerRadius();
    painter.drawRoundedRect(selectionRect, fluentRadius, fluentRadius);

    painter.restore();
}

void FluentCard::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        m_pressPosition = event->position().toPoint();

        if (m_selectable) {
            setState(Core::FluentState::Pressed);
        }
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentCard::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        const bool wasInside = rect().contains(event->position().toPoint());

        if (wasInside) {
            if (m_selectable) {
                setSelected(!m_selected);
            }

            emit cardClicked();
        }

        setState(wasInside && m_selectable ? Core::FluentState::Hovered
                                           : Core::FluentState::Normal);
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentCard::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit cardDoubleClicked();

        if (m_expandable) {
            toggleExpanded();
        }
    }

    Core::FluentComponent::mouseDoubleClickEvent(event);
}

void FluentCard::updateStateStyle() {
    // TODO: Fix backgroundColor property animation
    // Temporarily disabled to prevent segfault
    update();
}

void FluentCard::enterEvent(QEnterEvent* event) {
    if (isEnabled() && m_selectable) {
        setState(Core::FluentState::Hovered);
    }
    update();
    Core::FluentComponent::enterEvent(event);
}

void FluentCard::leaveEvent(QEvent* event) {
    if (isEnabled()) {
        setState(hasFocus() ? Core::FluentState::Focused
                            : Core::FluentState::Normal);
    }
    update();
    Core::FluentComponent::leaveEvent(event);
}

void FluentCard::resizeEvent(QResizeEvent* event) {
    updateShadowEffect();
    Core::FluentComponent::resizeEvent(event);
}

void FluentCard::changeEvent(QEvent* event) {
    if (event->type() == QEvent::EnabledChange ||
        event->type() == QEvent::FontChange ||
        event->type() == QEvent::StyleChange) {
        updateStateStyle();
        updateShadowEffect();
        update();
    }
    Core::FluentComponent::changeEvent(event);
}

void FluentCard::performStateTransition(Core::FluentState from,
                                        Core::FluentState to) {
    if (isAnimated()) {
        // Enhanced FluentUI-compliant state transitions
        if (to == Core::FluentState::Hovered && m_selectable &&
            from != Core::FluentState::Pressed) {
            // Subtle elevation increase on hover for interactive cards
            FluentCardElevation hoverElevation = m_elevation;
            if (m_elevation == FluentCardElevation::Flat) {
                hoverElevation = FluentCardElevation::Low;
            } else if (m_elevation == FluentCardElevation::Low) {
                hoverElevation = FluentCardElevation::Medium;
            } else if (m_elevation == FluentCardElevation::Medium) {
                hoverElevation = FluentCardElevation::High;
            }
            animateElevation(m_elevation, hoverElevation);
        } else if (to == Core::FluentState::Normal &&
                   (from == Core::FluentState::Hovered ||
                    from == Core::FluentState::Focused)) {
            // Return to original elevation when leaving hover/focus
            animateElevation(FluentCardElevation::Medium, m_elevation);
        } else if (to == Core::FluentState::Pressed && m_selectable) {
            // Slight elevation decrease on press for tactile feedback
            FluentCardElevation pressElevation = FluentCardElevation::Flat;
            if (m_elevation > FluentCardElevation::Low) {
                pressElevation = static_cast<FluentCardElevation>(
                    static_cast<int>(m_elevation) / 2);
            }
            animateElevation(m_elevation, pressElevation);
        }
        updateStateStyle();
    } else {
        update();
    }
}

void FluentCard::setupLayout() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Add components
    m_mainLayout->addWidget(m_header.get());
    m_mainLayout->addWidget(m_content.get(), 1);
    m_mainLayout->addWidget(m_footer.get());

    updateHeaderVisibility();
    updateFooterVisibility();
}

void FluentCard::setupAnimations() {
    // Expansion animation with FluentUI motion specifications
    m_expansionAnimation =
        std::make_unique<QPropertyAnimation>(this, "expansionProgress");
    m_expansionAnimation->setDuration(
        350);  // FluentUI standard expansion duration

    // Use FluentUI card expansion curve (0.25, 0.46, 0.45, 0.94)
    QEasingCurve expansionCurve(QEasingCurve::BezierSpline);
    expansionCurve.addCubicBezierSegment(
        QPointF(0.25, 0.46), QPointF(0.45, 0.94), QPointF(1.0, 1.0));
    m_expansionAnimation->setEasingCurve(expansionCurve);

    connect(m_expansionAnimation.get(), &QPropertyAnimation::valueChanged, this,
            &FluentCard::onExpansionAnimationValueChanged);
    connect(m_expansionAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentCard::onExpansionAnimationFinished);

    // Elevation animation with FluentUI decelerate curve
    m_elevationAnimation =
        std::make_unique<QPropertyAnimation>(this, "shadowOpacity");
    m_elevationAnimation->setDuration(150);  // FluentUI hover response duration

    // Use FluentUI decelerate curve (0.1, 0.9, 0.2, 1.0)
    QEasingCurve elevationCurve(QEasingCurve::BezierSpline);
    elevationCurve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0),
                                         QPointF(1.0, 1.0));
    m_elevationAnimation->setEasingCurve(elevationCurve);

    connect(m_elevationAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentCard::onElevationAnimationFinished);
}

void FluentCard::updateShadowEffect() {
    if (m_elevation == FluentCardElevation::Flat) {
        setGraphicsEffect(nullptr);
        m_shadowEffect.reset();
        return;
    }

    if (!m_shadowEffect) {
        m_shadowEffect = std::make_unique<QGraphicsDropShadowEffect>(this);
        if (!m_shadowEffect) {
            qWarning() << "Failed to create shadow effect";
            return;
        }
        setGraphicsEffect(m_shadowEffect.get());
    }

    // Additional null check for safety
    if (!m_shadowEffect) {
        qWarning() << "Shadow effect is null, cannot update";
        return;
    }

    // Use FluentUI-compliant shadow specifications
    const auto& theme = Styling::FluentTheme::instance();
    bool isDarkMode = (theme.mode() == Styling::FluentThemeMode::Dark);

    int blurRadius = 0;
    int offsetY = 0;
    int alpha = 0;

    // FluentUI elevation specifications
    switch (m_elevation) {
        case FluentCardElevation::Flat:
            // Already handled above
            break;
        case FluentCardElevation::Low:
            blurRadius = 4;  // shadow.2 equivalent
            offsetY = 1;
            alpha = isDarkMode ? 40 : 28;  // Higher contrast in dark mode
            break;
        case FluentCardElevation::Medium:
            blurRadius = 8;  // shadow.4 equivalent
            offsetY = 2;
            alpha = isDarkMode ? 50 : 35;
            break;
        case FluentCardElevation::High:
            blurRadius = 16;  // shadow.8 equivalent
            offsetY = 4;
            alpha = isDarkMode ? 60 : 42;
            break;
        case FluentCardElevation::VeryHigh:
            blurRadius = 28;  // shadow.16 equivalent
            offsetY = 8;
            alpha = isDarkMode ? 70 : 50;
            break;
    }

    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(0, offsetY);

    // Apply theme-aware shadow color with opacity
    QColor shadowColor = QColor(0, 0, 0, alpha);
    shadowColor.setAlphaF(shadowColor.alphaF() * m_shadowOpacity);
    m_shadowEffect->setColor(shadowColor);
}

void FluentCard::animateElevation(FluentCardElevation from,
                                  FluentCardElevation to) {
    if (!m_elevationAnimation) {
        return;
    }

    m_elevationAnimation->stop();

    // Calculate shadow opacity based on elevation levels
    const qreal fromOpacity = static_cast<qreal>(from) /
                              static_cast<qreal>(FluentCardElevation::VeryHigh);
    const qreal toOpacity = static_cast<qreal>(to) /
                            static_cast<qreal>(FluentCardElevation::VeryHigh);

    m_elevationAnimation->setStartValue(fromOpacity);
    m_elevationAnimation->setEndValue(toOpacity);

    // Use FluentUI hover response timing - faster for better responsiveness
    m_elevationAnimation->setDuration(150);  // FluentUI hover response duration

    // Use FluentUI decelerate curve for elevation changes (0.1, 0.9, 0.2, 1.0)
    QEasingCurve elevationCurve(QEasingCurve::BezierSpline);
    elevationCurve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0),
                                         QPointF(1.0, 1.0));
    m_elevationAnimation->setEasingCurve(elevationCurve);

    m_elevationAnimation->start();
}

void FluentCard::updateHeaderVisibility() {
    m_header->setVisible(m_headerVisible);
    m_mainLayout->setContentsMargins(0, m_headerVisible ? 0 : 12, 0,
                                     m_footerVisible ? 0 : 12);
}

void FluentCard::updateFooterVisibility() {
    m_footer->setVisible(m_footerVisible);
}

void FluentCard::updateExpansionState() {
    if (m_expanded) {
        m_content->show();
        setFixedHeight(QWIDGETSIZE_MAX);
    } else {
        m_content->hide();
        setFixedHeight(getHeaderHeight() +
                       (m_footerVisible ? getFooterHeight() : 0) + 24);
    }

    updateGeometry();
}

void FluentCard::toggleExpanded() { setExpanded(!m_expanded); }

void FluentCard::expandWithAnimation() {
    if (m_expanded)
        return;

    m_expandedHeight = sizeHint().height();
    m_collapsedHeight = height();

    m_expansionAnimation->stop();
    m_expansionAnimation->setStartValue(0.0);
    m_expansionAnimation->setEndValue(1.0);
    m_expansionAnimation->start();

    m_expanded = true;
}

void FluentCard::collapseWithAnimation() {
    if (!m_expanded)
        return;

    m_expandedHeight = height();
    m_collapsedHeight =
        getHeaderHeight() + (m_footerVisible ? getFooterHeight() : 0) + 24;

    m_expansionAnimation->stop();
    m_expansionAnimation->setStartValue(1.0);
    m_expansionAnimation->setEndValue(0.0);
    m_expansionAnimation->start();

    m_expanded = false;
}

void FluentCard::animateIn() {
    hide();

    // Use Fluent Design entrance animation timing and curves
    Animation::FluentAnimationConfig slideConfig;
    slideConfig.duration = 250ms;  // Fluent standard duration
    slideConfig.easing =
        Animation::FluentEasing::FluentStandard;  // Fluent standard curve
    auto slideAnimation =
        Animation::FluentAnimator::slideIn(this, QPoint(0, 20), slideConfig);

    Animation::FluentAnimationConfig fadeConfig;
    fadeConfig.duration =
        200ms;  // Slightly faster fade for better perceived performance
    fadeConfig.easing =
        Animation::FluentEasing::FluentDecelerate;  // Fluent decelerate curve
    auto fadeAnimation = Animation::FluentAnimator::fadeIn(this, fadeConfig);

    show();

    // Store animations to prevent destruction and start them
    if (slideAnimation) {
        auto* anim = slideAnimation.release();
        m_currentAnimations.append(anim);

        // Clean up when finished
        connect(anim, &QPropertyAnimation::finished, this, [this, anim]() {
            m_currentAnimations.removeOne(anim);
            anim->deleteLater();
        });

        anim->start();
    }
    if (fadeAnimation) {
        auto* anim = fadeAnimation.release();
        m_currentAnimations.append(anim);

        // Clean up when finished
        connect(anim, &QPropertyAnimation::finished, this, [this, anim]() {
            m_currentAnimations.removeOne(anim);
            anim->deleteLater();
        });

        anim->start();
    }
}

void FluentCard::animateOut() {
    Animation::FluentAnimationConfig slideConfig;
    slideConfig.duration = 300ms;
    auto slideAnimation =
        Animation::FluentAnimator::slideIn(this, QPoint(0, -20), slideConfig);

    Animation::FluentAnimationConfig fadeConfig;
    fadeConfig.duration = 200ms;
    auto fadeAnimation = Animation::FluentAnimator::fadeOut(this, fadeConfig);

    // Store animations to prevent destruction and start them
    if (slideAnimation) {
        auto* anim = slideAnimation.release();
        m_currentAnimations.append(anim);

        // Clean up when finished
        connect(anim, &QPropertyAnimation::finished, this, [this, anim]() {
            m_currentAnimations.removeOne(anim);
            anim->deleteLater();
        });

        anim->start();
    }
    if (fadeAnimation) {
        auto* anim = fadeAnimation.release();
        m_currentAnimations.append(anim);

        // Clean up when finished and hide widget
        connect(anim, &QPropertyAnimation::finished, this, [this, anim]() {
            m_currentAnimations.removeOne(anim);
            anim->deleteLater();
            this->hide();
        });

        anim->start();
    }
}

// Color methods
QColor FluentCard::getBackgroundColor() const {
    // Use FluentUI design tokens for consistent theming
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    switch (m_cardStyle) {
        case FluentCardStyle::Filled:
            switch (state()) {
                case Core::FluentState::Normal:
                    return tokenUtils.getSemanticColor("surface.secondary");
                case Core::FluentState::Hovered:
                    return tokenUtils.getSemanticColor("surface.tertiary");
                case Core::FluentState::Pressed:
                    return tokenUtils.getNeutralColor(8);
                default:
                    return tokenUtils.getSemanticColor("surface.secondary");
            }

        case FluentCardStyle::Outlined:
            return Qt::transparent;

        case FluentCardStyle::Subtle:
            switch (state()) {
                case Core::FluentState::Normal:
                    return tokenUtils.getNeutralColor(2);
                case Core::FluentState::Hovered:
                    return m_selectable ? tokenUtils.getNeutralColor(4)
                                        : tokenUtils.getNeutralColor(2);
                case Core::FluentState::Pressed:
                    return tokenUtils.getNeutralColor(6);
                default:
                    return tokenUtils.getNeutralColor(2);
            }

        default:  // Default and Elevated - use card design tokens
            switch (state()) {
                case Core::FluentState::Normal:
                    return tokenUtils.getSemanticColor("surface.primary");
                case Core::FluentState::Hovered:
                    return m_selectable
                               ? tokenUtils.getNeutralColor(1)
                               : tokenUtils.getSemanticColor("surface.primary");
                case Core::FluentState::Pressed:
                    return tokenUtils.getNeutralColor(3);
                default:
                    return tokenUtils.getSemanticColor("surface.primary");
            }
    }
}

QColor FluentCard::getBorderColor() const {
    // Use FluentUI design tokens for consistent border colors
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    if (m_selected) {
        return tokenUtils.getSemanticColor("border.focus");
    }

    switch (state()) {
        case Core::FluentState::Hovered:
            return tokenUtils.getSemanticColor("border.primary");
        case Core::FluentState::Focused:
            return tokenUtils.getSemanticColor("border.focus");
        default:
            return tokenUtils.getSemanticColor("border.secondary");
    }
}

QColor FluentCard::getSelectionColor() const {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    return tokenUtils.getSemanticColor("text.accent");
}

QPen FluentCard::getBorderPen() const {
    return QPen(getBorderColor(), getBorderWidth());
}

int FluentCard::getBorderWidth() const { return m_selected ? 2 : 1; }

int FluentCard::getFluentCornerRadius() const {
    return getFluentCornerRadius(size());
}

int FluentCard::getFluentCornerRadius(const QSize& size) const {
    const auto& theme = Styling::FluentTheme::instance();

    // Use FluentUI corner radius specifications based on component size
    const int minDimension = qMin(size.width(), size.height());

    if (minDimension < 32) {
        // Small components use 2px radius
        return theme.borderRadius("small");
    } else if (minDimension >= 200) {
        // Large components use 8px radius
        return theme.borderRadius("large");
    } else {
        // Standard components use 4px radius (medium)
        return theme.borderRadius("medium");
    }
}

int FluentCard::getHeaderHeight() const {
    return m_headerVisible ? m_header->sizeHint().height() : 0;
}

int FluentCard::getFooterHeight() const {
    return m_footerVisible ? m_footer->sizeHint().height() : 0;
}

// Animation slots
void FluentCard::onExpansionAnimationValueChanged(const QVariant& value) {
    m_expansionProgress = value.toReal();

    const int targetHeight =
        m_collapsedHeight +
        static_cast<int>((m_expandedHeight - m_collapsedHeight) *
                         m_expansionProgress);

    setFixedHeight(targetHeight);
}

void FluentCard::onExpansionAnimationFinished() {
    setFixedHeight(QWIDGETSIZE_MAX);
    updateExpansionState();
    emit expandedChanged(m_expanded);
}

void FluentCard::onElevationAnimationFinished() { updateShadowEffect(); }

void FluentCard::onThemeChanged() {
    updateStateStyle();
    updateShadowEffect();

    // Trigger smooth color transitions when theme changes
    if (isAnimated()) {
        // Create a smooth transition animation for background color changes
        auto* colorTransition = new QPropertyAnimation(this, "windowOpacity");
        colorTransition->setDuration(200);
        colorTransition->setEasingCurve(QEasingCurve::OutCubic);
        colorTransition->setStartValue(0.95);
        colorTransition->setEndValue(1.0);

        m_currentAnimations.append(colorTransition);

        connect(colorTransition, &QPropertyAnimation::finished,
                [this, colorTransition]() {
                    m_currentAnimations.removeOne(colorTransition);
                    colorTransition->deleteLater();
                });

        colorTransition->start();
    }

    // Force repaint to apply new colors immediately
    update();
}

// Micro-interaction methods
void FluentCard::addHoverEffect() {
    if (!m_selectable)
        return;

    // Micro-interactions are handled through the existing state transition
    // system The enhanced performStateTransition method already provides
    // FluentUI-compliant hover effects with proper elevation changes and
    // animations
}

void FluentCard::addPressEffect() {
    if (!m_selectable)
        return;

    // Press effects are handled through the existing state transition system
    // The enhanced performStateTransition method provides proper press feedback
    // with elevation changes that follow FluentUI interaction patterns
}

void FluentCard::addFocusEffect() {
    // Focus effects are handled through the existing state transition system
    // The enhanced performStateTransition method provides proper focus feedback
    // with appropriate visual indicators following FluentUI guidelines
}

// Convenience methods for enhanced developer experience
void FluentCard::setCardContent(QWidget* content) {
    if (content) {
        setContentWidget(content);
    }
}

void FluentCard::setCardHeader(const QString& title, const QString& subtitle) {
    if (!title.isEmpty()) {
        setTitle(title);
        if (!subtitle.isEmpty()) {
            setSubtitle(subtitle);
        }
        setHeaderVisible(true);
    }
}

void FluentCard::setCardFooter(QWidget* footer) {
    if (footer) {
        addFooterWidget(footer);
    }
}

void FluentCard::setCardIcon(const QIcon& icon) {
    if (!icon.isNull()) {
        setHeaderIcon(icon);
    }
}

void FluentCard::setCardActions(const QList<QAction*>& actions) {
    if (!actions.isEmpty()) {
        // Add actions to header for better FluentUI compliance
        for (auto* action : actions) {
            if (action) {
                addHeaderAction(action);
            }
        }
    }
}

// Smart defaults based on FluentUI guidelines
void FluentCard::applyFluentDefaults() {
    // Apply FluentUI-compliant defaults
    setElevation(FluentCardElevation::Low);
    setCardStyle(FluentCardStyle::Default);
    setSelectable(true);
    setAnimated(true);

    // Set appropriate corner radius based on current size
    setCornerRadius(getFluentCornerRadius());

    // Apply default spacing and padding using FluentUI specifications
    int defaultPadding = 16;  // FluentUI standard card padding
    setContentsMargins(defaultPadding, defaultPadding, defaultPadding,
                       defaultPadding);
}

void FluentCard::setFluentSize(FluentCardSize size) {
    QSize targetSize;

    switch (size) {
        case FluentCardSize::Small:
            targetSize = QSize(180, 120);
            break;
        case FluentCardSize::Medium:
            targetSize = QSize(300, 200);
            break;
        case FluentCardSize::Large:
            targetSize = QSize(500, 300);
            break;
        case FluentCardSize::ExtraLarge:
            targetSize = QSize(700, 400);
            break;
    }

    setMinimumSize(targetSize);
    setCornerRadius(getFluentCornerRadius(targetSize));
}

// Enhanced state management
bool FluentCard::isInteractive() const { return m_selectable && isEnabled(); }

void FluentCard::setInteractive(bool interactive) {
    setSelectable(interactive);
    setEnabled(interactive);
}

bool FluentCard::hasContent() const {
    return contentWidget() != nullptr || isHeaderVisible() ||
           !title().isEmpty() || !subtitle().isEmpty();
}

// Enhanced accessibility methods
void FluentCard::setupAccessibility() {
    // Set appropriate accessible name and role
    if (m_selectable) {
        setAccessibleName(tr("Card"));
        setAccessibleDescription(
            tr("Interactive card, press Enter or Space to activate"));
    } else {
        setAccessibleName(tr("Information Card"));
        setAccessibleDescription(tr("Information container"));
    }

    // Enable keyboard focus for interactive cards
    if (m_selectable) {
        setFocusPolicy(Qt::StrongFocus);
        setTabOrder(this, nullptr);
    }

    // Set up accessible description
    updateAccessibilityInfo();

    // Enable accessible navigation
    setAttribute(Qt::WA_AcceptTouchEvents, true);
}

void FluentCard::updateAccessibilityInfo() {
    QString description;

    // Build accessible description from card content
    if (!title().isEmpty()) {
        description += title();
    }

    if (!subtitle().isEmpty()) {
        if (!description.isEmpty())
            description += ", ";
        description += subtitle();
    }

    // Add elevation information for screen readers
    switch (m_elevation) {
        case FluentCardElevation::Flat:
            description += tr(", flat card");
            break;
        case FluentCardElevation::Low:
            description += tr(", low elevation card");
            break;
        case FluentCardElevation::Medium:
            description += tr(", medium elevation card");
            break;
        case FluentCardElevation::High:
            description += tr(", high elevation card");
            break;
        case FluentCardElevation::VeryHigh:
            description += tr(", very high elevation card");
            break;
    }

    // Add interaction information
    if (m_selectable) {
        description += tr(", clickable");
        if (m_selected) {
            description += tr(", selected");
        }
    }

    setAccessibleDescription(description);

    // Update state information
    if (m_selectable) {
        setProperty("accessibleState", m_selected ? "selected" : "unselected");
    }
}

// FluentCardHeader Implementation
FluentCardHeader::FluentCardHeader(QWidget* parent) : QWidget(parent) {
    setupLayout();
    setFixedHeight(48);
}

void FluentCardHeader::setupLayout() {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(16, 8, 16, 8);
    m_layout->setSpacing(12);

    // Icon label
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(24, 24);
    m_iconLabel->setScaledContents(true);
    m_iconLabel->hide();

    // Title and subtitle
    auto* textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    textLayout->setContentsMargins(0, 0, 0, 0);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet("font-weight: 600; font-size: 14px;");

    m_subtitleLabel = new QLabel(this);
    m_subtitleLabel->setStyleSheet("color: #666; font-size: 12px;");
    m_subtitleLabel->hide();

    textLayout->addWidget(m_titleLabel);
    textLayout->addWidget(m_subtitleLabel);

    // Actions layout
    m_actionsLayout = new QHBoxLayout();
    m_actionsLayout->setSpacing(4);
    m_actionsLayout->setContentsMargins(0, 0, 0, 0);

    // Add to main layout
    m_layout->addWidget(m_iconLabel);
    m_layout->addLayout(textLayout, 1);
    m_layout->addLayout(m_actionsLayout);
}

void FluentCardHeader::setTitle(const QString& title) {
    m_titleLabel->setText(title);
    m_titleLabel->setVisible(!title.isEmpty());
}

QString FluentCardHeader::title() const { return m_titleLabel->text(); }

void FluentCardHeader::setSubtitle(const QString& subtitle) {
    m_subtitleLabel->setText(subtitle);
    m_subtitleLabel->setVisible(!subtitle.isEmpty());
}

QString FluentCardHeader::subtitle() const { return m_subtitleLabel->text(); }

void FluentCardHeader::setIcon(const QIcon& icon) {
    if (!icon.isNull()) {
        m_iconLabel->setPixmap(icon.pixmap(24, 24));
        m_iconLabel->show();
    } else {
        m_iconLabel->hide();
    }
}

QIcon FluentCardHeader::icon() const {
    const QPixmap pixmap = m_iconLabel->pixmap();
    return pixmap.isNull() ? QIcon() : QIcon(pixmap);
}

void FluentCardHeader::addAction(QAction* action) {
    auto* button = new QToolButton(this);
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setAutoRaise(true);

    m_actionButtons.push_back(button);
    m_actionsLayout->addWidget(button);
}

void FluentCardHeader::removeAction(QAction* action) {
    auto it = std::find_if(m_actionButtons.begin(), m_actionButtons.end(),
                           [action](QToolButton* button) {
                               return button->defaultAction() == action;
                           });

    if (it != m_actionButtons.end()) {
        (*it)->deleteLater();
        m_actionButtons.erase(it);
    }
}

void FluentCardHeader::clearActions() {
    for (auto* button : m_actionButtons) {
        button->deleteLater();
    }
    m_actionButtons.clear();
}

QSize FluentCardHeader::sizeHint() const { return QSize(300, 48); }

void FluentCardHeader::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
    }
    QWidget::mousePressEvent(event);
}

void FluentCardHeader::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;
        if (rect().contains(event->position().toPoint())) {
            emit clicked();
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void FluentCardHeader::paintEvent(QPaintEvent* event) {
    // Optional: Add header-specific styling
    QWidget::paintEvent(event);
}

// FluentCardContent Implementation
FluentCardContent::FluentCardContent(QWidget* parent) : QScrollArea(parent) {
    setFrameShape(QFrame::NoFrame);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Don't create default content widget - will be created on-demand
    m_contentWidget = nullptr;
}

void FluentCardContent::setContentWidget(QWidget* widget) {
    m_contentWidget = widget;
    setWidget(widget);
}

QSize FluentCardContent::sizeHint() const {
    if (m_contentWidget) {
        return m_contentWidget->sizeHint();
    }
    return QSize(300, 200);
}

QSize FluentCardContent::minimumSizeHint() const {
    if (m_contentWidget) {
        return m_contentWidget->minimumSizeHint();
    }
    return QSize(100, 50);
}

void FluentCardContent::resizeEvent(QResizeEvent* event) {
    QScrollArea::resizeEvent(event);

    if (m_contentWidget) {
        m_contentWidget->resize(event->size());
    }
}

// FluentCardFooter Implementation
FluentCardFooter::FluentCardFooter(QWidget* parent) : QWidget(parent) {
    setupLayout();
    setFixedHeight(40);
}

void FluentCardFooter::setupLayout() {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(16, 8, 16, 8);
    m_layout->setSpacing(8);
}

void FluentCardFooter::addWidget(QWidget* widget) {
    m_layout->addWidget(widget);
}

void FluentCardFooter::removeWidget(QWidget* widget) {
    m_layout->removeWidget(widget);
}

void FluentCardFooter::clearWidgets() {
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

QSize FluentCardFooter::sizeHint() const { return QSize(300, 40); }

void FluentCardFooter::paintEvent(QPaintEvent* event) {
    // Optional: Add separator line at top
    QPainter painter(this);
    painter.setPen(QColor(0, 0, 0, 20));
    painter.drawLine(16, 0, width() - 16, 0);

    QWidget::paintEvent(event);
}

}  // namespace FluentQt::Components
