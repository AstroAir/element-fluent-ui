// src/Components/FluentCard.cpp
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"
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
    setCornerRadius(static_cast<int>(Core::FluentCornerRadius::Medium));

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

    QPainterPath path;
    path.addRoundedRect(rect, cornerRadius(), cornerRadius());

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

    painter.drawRoundedRect(borderRect, cornerRadius(), cornerRadius());

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
    painter.drawRoundedRect(selectionRect, cornerRadius(), cornerRadius());

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
        // Animate elevation changes based on state
        if (to == Core::FluentState::Hovered &&
            from != Core::FluentState::Pressed) {
            animateElevation(m_elevation, FluentCardElevation::Medium);
        } else if (to == Core::FluentState::Normal) {
            animateElevation(FluentCardElevation::Medium, m_elevation);
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
    // Expansion animation
    m_expansionAnimation =
        std::make_unique<QPropertyAnimation>(this, "expansionProgress");
    m_expansionAnimation->setDuration(300);
    m_expansionAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_expansionAnimation.get(), &QPropertyAnimation::valueChanged, this,
            &FluentCard::onExpansionAnimationValueChanged);
    connect(m_expansionAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentCard::onExpansionAnimationFinished);

    // Elevation animation
    m_elevationAnimation =
        std::make_unique<QPropertyAnimation>(this, "shadowOpacity");
    m_elevationAnimation->setDuration(200);
    m_elevationAnimation->setEasingCurve(QEasingCurve::OutCubic);

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
        m_shadowEffect->setColor(QColor(0, 0, 0, 30));
        setGraphicsEffect(m_shadowEffect.get());
    }

    // Additional null check for safety
    if (!m_shadowEffect) {
        qWarning() << "Shadow effect is null, cannot update";
        return;
    }

    const int elevation = static_cast<int>(m_elevation);
    m_shadowEffect->setBlurRadius(elevation * 2);
    m_shadowEffect->setOffset(0, elevation / 2);

    // Apply opacity with null check
    QColor shadowColor = m_shadowEffect->color();
    shadowColor.setAlphaF(shadowColor.alphaF() * m_shadowOpacity);
    m_shadowEffect->setColor(shadowColor);
}

void FluentCard::animateElevation(FluentCardElevation from,
                                  FluentCardElevation to) {
    Q_UNUSED(from)
    Q_UNUSED(to)

    m_elevationAnimation->stop();
    m_elevationAnimation->setStartValue(m_shadowOpacity);
    m_elevationAnimation->setEndValue(1.0);
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

    Animation::FluentAnimationConfig slideConfig;
    slideConfig.duration = 400ms;
    auto slideAnimation =
        Animation::FluentAnimator::slideIn(this, QPoint(0, 20), slideConfig);

    Animation::FluentAnimationConfig fadeConfig;
    fadeConfig.duration = 300ms;
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
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    switch (m_cardStyle) {
        case FluentCardStyle::Filled:
            switch (state()) {
                case Core::FluentState::Normal:
                    return palette.neutralLighter;
                case Core::FluentState::Hovered:
                    return palette.neutralLight;
                case Core::FluentState::Pressed:
                    return palette.neutralQuaternaryAlt;
                default:
                    return palette.neutralLighter;
            }

        case FluentCardStyle::Outlined:
            return Qt::transparent;

        default:  // Default and Elevated
            switch (state()) {
                case Core::FluentState::Normal:
                    return QColor(255, 255, 255);  // Card background
                case Core::FluentState::Hovered:
                    return m_selectable ? QColor(250, 250, 250)
                                        : QColor(255, 255, 255);
                case Core::FluentState::Pressed:
                    return QColor(245, 245, 245);
                default:
                    return QColor(255, 255, 255);
            }
    }
}

QColor FluentCard::getBorderColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (m_selected) {
        return palette.accent;
    }

    return palette.neutralQuaternaryAlt;
}

QColor FluentCard::getSelectionColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    return theme.currentPalette().accent;
}

QPen FluentCard::getBorderPen() const {
    return QPen(getBorderColor(), getBorderWidth());
}

int FluentCard::getBorderWidth() const { return m_selected ? 2 : 1; }

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
