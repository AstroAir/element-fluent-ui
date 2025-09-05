// src/Components/FluentPopover.cpp
#include "FluentQt/Components/FluentPopover.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QDesktopWidget>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QHideEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QShowEvent>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentPopover::FluentPopover(QWidget* parent) : Core::FluentComponent(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    setObjectName("FluentPopover");

    setupUI();
    setupAnimations();
    updateColors();
    updateFonts();
    updateAccessibility();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentPopover::onThemeChanged);
}

FluentPopover::FluentPopover(const QString& content, QWidget* parent)
    : FluentPopover(parent) {
    setContent(content);
}

FluentPopover::FluentPopover(const QString& title, const QString& content,
                             QWidget* parent)
    : FluentPopover(parent) {
    setTitle(title);
    setContent(content);
}

FluentPopover::~FluentPopover() { removeTargetEventFilter(); }

void FluentPopover::setupUI() {
    FLUENT_PROFILE("FluentPopover::setupUI");

    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(12, 8, 12, 8);
    m_mainLayout->setSpacing(8);

    // Create header layout
    m_headerLayout = new QHBoxLayout();
    m_headerLayout->setContentsMargins(0, 0, 0, 0);
    m_headerLayout->setSpacing(8);

    // Create icon label
    m_iconLabel = new QLabel(this);
    m_iconLabel->setObjectName("FluentPopover_Icon");
    m_iconLabel->setFixedSize(16, 16);
    m_iconLabel->hide();  // Initially hidden
    m_headerLayout->addWidget(m_iconLabel);

    // Create title label
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("FluentPopover_Title");
    m_titleLabel->setWordWrap(true);
    m_titleLabel->hide();  // Initially hidden
    m_headerLayout->addWidget(m_titleLabel, 1);

    // Create close button
    m_closeButton = new QPushButton("×", this);
    m_closeButton->setObjectName("FluentPopover_CloseButton");
    m_closeButton->setFixedSize(20, 20);
    m_closeButton->setFlat(true);
    m_closeButton->hide();  // Initially hidden
    m_headerLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_headerLayout);

    // Create content label
    m_contentLabel = new QLabel(this);
    m_contentLabel->setObjectName("FluentPopover_Content");
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_mainLayout->addWidget(m_contentLabel);

    // Connect signals
    connect(m_closeButton, &QPushButton::clicked, this,
            &FluentPopover::onCloseButtonClicked);

    // Set size constraints
    setMaximumWidth(m_maxWidth);
    setMinimumWidth(m_minWidth);
    setMaximumHeight(m_maxHeight);

    // Initially hidden
    QWidget::hide();
}

void FluentPopover::setupAnimations() {
    FLUENT_PROFILE("FluentPopover::setupAnimations");

    // Create opacity effect
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(0.0);
    setGraphicsEffect(m_opacityEffect);

    // Create shadow effect with enhanced elevation
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    updateShadowEffect();

    // Create individual animations
    m_showAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect, "opacity");
    m_showAnimation->setDuration(250);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);

    m_hideAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect, "opacity");
    m_hideAnimation->setDuration(200);
    m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);

    // Setup scale animation
    setupScaleAnimation();

    // Create animation groups
    m_showAnimationGroup = std::make_unique<QParallelAnimationGroup>(this);
    m_hideAnimationGroup = std::make_unique<QParallelAnimationGroup>(this);

    updateAnimationProperties();

    // Create auto-hide timer
    m_autoHideTimer = new QTimer(this);
    m_autoHideTimer->setSingleShot(true);

    // Connect animations
    connect(m_showAnimationGroup.get(), &QParallelAnimationGroup::finished,
            this, &FluentPopover::onShowAnimationFinished);
    connect(m_hideAnimationGroup.get(), &QParallelAnimationGroup::finished,
            this, &FluentPopover::onHideAnimationFinished);
    connect(m_autoHideTimer, &QTimer::timeout, this,
            &FluentPopover::onAutoHideTimer);
}

QString FluentPopover::title() const { return m_title; }

void FluentPopover::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        m_titleLabel->setText(title);
        m_titleLabel->setVisible(!title.isEmpty());
        m_sizeHintValid = false;
        updateGeometry();
        emit titleChanged(title);
    }
}

QString FluentPopover::content() const { return m_content; }

void FluentPopover::setContent(const QString& content) {
    if (m_content != content) {
        m_content = content;
        m_contentLabel->setText(content);
        m_sizeHintValid = false;
        updateGeometry();
        emit contentChanged(content);
    }
}

QIcon FluentPopover::icon() const { return m_icon; }

void FluentPopover::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;

        if (!icon.isNull()) {
            m_iconLabel->setPixmap(icon.pixmap(16, 16));
            m_iconLabel->show();
        } else {
            m_iconLabel->hide();
        }

        m_sizeHintValid = false;
        updateGeometry();
        emit iconChanged(icon);
    }
}

FluentPopoverPlacement FluentPopover::placement() const { return m_placement; }

void FluentPopover::setPlacement(FluentPopoverPlacement placement) {
    if (m_placement != placement) {
        m_placement = placement;
        if (m_isVisible) {
            updatePosition();
        }
        emit placementChanged(placement);
    }
}

FluentPopoverTrigger FluentPopover::trigger() const { return m_trigger; }

void FluentPopover::setTrigger(FluentPopoverTrigger trigger) {
    if (m_trigger != trigger) {
        m_trigger = trigger;

        // Update event filter based on trigger
        if (m_target) {
            removeTargetEventFilter();
            installTargetEventFilter();
        }

        emit triggerChanged(trigger);
    }
}

bool FluentPopover::showArrow() const { return m_showArrow; }

void FluentPopover::setShowArrow(bool show) {
    if (m_showArrow != show) {
        m_showArrow = show;
        update();
        emit showArrowChanged(show);
    }
}

bool FluentPopover::showCloseButton() const { return m_showCloseButton; }

void FluentPopover::setShowCloseButton(bool show) {
    if (m_showCloseButton != show) {
        m_showCloseButton = show;
        m_closeButton->setVisible(show);
        m_sizeHintValid = false;
        updateGeometry();
        emit showCloseButtonChanged(show);
    }
}

int FluentPopover::autoHideDelay() const { return m_autoHideDelay; }

void FluentPopover::setAutoHideDelay(int delay) {
    if (m_autoHideDelay != delay) {
        m_autoHideDelay = delay;
        emit autoHideDelayChanged(delay);
    }
}

int FluentPopover::maxWidth() const { return m_maxWidth; }

void FluentPopover::setMaxWidth(int width) {
    if (m_maxWidth != width && width > 0) {
        m_maxWidth = width;
        setMaximumWidth(width);
        m_sizeHintValid = false;
        updateGeometry();
        emit maxWidthChanged(width);
    }
}

int FluentPopover::elevation() const { return m_elevation; }

void FluentPopover::setElevation(int elevation) {
    if (m_elevation != elevation && elevation >= 0 && elevation <= 4) {
        m_elevation = elevation;
        updateColors();
        update();
        emit elevationChanged(elevation);
    }
}

bool FluentPopover::enableBackdrop() const { return m_enableBackdrop; }

void FluentPopover::setEnableBackdrop(bool enable) {
    if (m_enableBackdrop != enable) {
        m_enableBackdrop = enable;
        update();
        emit enableBackdropChanged(enable);
    }
}

bool FluentPopover::enableScaleAnimation() const {
    return m_enableScaleAnimation;
}

void FluentPopover::setEnableScaleAnimation(bool enable) {
    if (m_enableScaleAnimation != enable) {
        m_enableScaleAnimation = enable;
        updateAnimationProperties();
        emit enableScaleAnimationChanged(enable);
    }
}

int FluentPopover::borderWidth() const { return m_borderWidth; }

void FluentPopover::setBorderWidth(int width) {
    if (m_borderWidth != width && width >= 0) {
        m_borderWidth = width;
        update();
        emit borderWidthChanged(width);
    }
}

QColor FluentPopover::customBackgroundColor() const {
    return m_customBackgroundColor;
}

void FluentPopover::setCustomBackgroundColor(const QColor& color) {
    if (m_customBackgroundColor != color) {
        m_customBackgroundColor = color;
        update();
        emit customBackgroundColorChanged(color);
    }
}

QColor FluentPopover::customBorderColor() const { return m_customBorderColor; }

void FluentPopover::setCustomBorderColor(const QColor& color) {
    if (m_customBorderColor != color) {
        m_customBorderColor = color;
        update();
        emit customBorderColorChanged(color);
    }
}

QString FluentPopover::accessibleName() const { return m_accessibleName; }

void FluentPopover::setAccessibleName(const QString& name) {
    if (m_accessibleName != name) {
        m_accessibleName = name;
        updateAccessibility();
        emit accessibleNameChanged(name);
    }
}

QString FluentPopover::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentPopover::setAccessibleDescription(const QString& description) {
    if (m_accessibleDescription != description) {
        m_accessibleDescription = description;
        updateAccessibility();
        emit accessibleDescriptionChanged(description);
    }
}

QString FluentPopover::ariaLabel() const { return m_ariaLabel; }

void FluentPopover::setAriaLabel(const QString& label) {
    if (m_ariaLabel != label) {
        m_ariaLabel = label;
        updateAccessibility();
        emit ariaLabelChanged(label);
    }
}

QString FluentPopover::ariaDescribedBy() const { return m_ariaDescribedBy; }

void FluentPopover::setAriaDescribedBy(const QString& describedBy) {
    if (m_ariaDescribedBy != describedBy) {
        m_ariaDescribedBy = describedBy;
        updateAccessibility();
        emit ariaDescribedByChanged(describedBy);
    }
}

int FluentPopover::minWidth() const { return m_minWidth; }

void FluentPopover::setMinWidth(int width) {
    if (m_minWidth != width && width > 0) {
        m_minWidth = width;
        setMinimumWidth(width);
        m_sizeHintValid = false;
        updateGeometry();
        emit minWidthChanged(width);
    }
}

int FluentPopover::maxHeight() const { return m_maxHeight; }

void FluentPopover::setMaxHeight(int height) {
    if (m_maxHeight != height && height > 0) {
        m_maxHeight = height;
        setMaximumHeight(height);
        m_sizeHintValid = false;
        updateGeometry();
        emit maxHeightChanged(height);
    }
}

bool FluentPopover::adaptiveWidth() const { return m_adaptiveWidth; }

void FluentPopover::setAdaptiveWidth(bool adaptive) {
    if (m_adaptiveWidth != adaptive) {
        m_adaptiveWidth = adaptive;
        if (adaptive) {
            calculateResponsiveSize();
        }
        emit adaptiveWidthChanged(adaptive);
    }
}

bool FluentPopover::adaptiveHeight() const { return m_adaptiveHeight; }

void FluentPopover::setAdaptiveHeight(bool adaptive) {
    if (m_adaptiveHeight != adaptive) {
        m_adaptiveHeight = adaptive;
        if (adaptive) {
            calculateResponsiveSize();
        }
        emit adaptiveHeightChanged(adaptive);
    }
}

void FluentPopover::setContentWidget(QWidget* widget) {
    if (m_contentWidget) {
        m_mainLayout->removeWidget(m_contentWidget);
        m_contentWidget->setParent(nullptr);
    }

    m_contentWidget = widget;
    if (widget) {
        m_mainLayout->addWidget(widget);
        m_contentLabel->hide();
        m_sizeHintValid = false;
        updateGeometry();
    } else {
        m_contentLabel->show();
    }
}

QWidget* FluentPopover::contentWidget() const { return m_contentWidget; }

void FluentPopover::setTarget(QWidget* target) {
    if (m_target != target) {
        removeTargetEventFilter();
        m_target = target;
        installTargetEventFilter();
    }
}

QWidget* FluentPopover::target() const { return m_target; }

QSize FluentPopover::sizeHint() const {
    FLUENT_PROFILE("FluentPopover::sizeHint");

    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }

    // Calculate content size
    QSize contentSize;

    if (m_contentWidget) {
        contentSize = m_contentWidget->sizeHint();
    } else {
        const QFontMetrics fm(m_contentLabel->font());
        const int textWidth =
            std::min(m_maxWidth - 24,
                     fm.horizontalAdvance(m_content));  // 24 for margins
        contentSize =
            fm.boundingRect(0, 0, textWidth, 0, Qt::TextWordWrap, m_content)
                .size();
    }

    // Add header size if needed
    if (!m_title.isEmpty() || !m_icon.isNull() || m_showCloseButton) {
        contentSize.setHeight(contentSize.height() + 24);  // Header height
    }

    // Add margins and arrow space
    contentSize += QSize(24, 16);  // Margins
    if (m_showArrow) {
        contentSize += QSize(0, 8);  // Arrow space
    }

    // Apply responsive constraints
    if (m_adaptiveWidth || m_adaptiveHeight) {
        const QSize adaptiveSize = getAdaptiveSize();
        if (m_adaptiveWidth) {
            contentSize.setWidth(adaptiveSize.width());
        }
        if (m_adaptiveHeight) {
            contentSize.setHeight(adaptiveSize.height());
        }
    }

    // Apply size constraints
    contentSize.setWidth(qBound(m_minWidth, contentSize.width(), m_maxWidth));
    contentSize.setHeight(qBound(50, contentSize.height(), m_maxHeight));

    m_cachedSizeHint = contentSize;
    m_sizeHintValid = true;

    return contentSize;
}

QSize FluentPopover::minimumSizeHint() const { return QSize(m_minWidth, 50); }

void FluentPopover::show() {
    if (!m_isVisible) {
        m_isVisible = true;
        emit aboutToShow();

        // Calculate responsive size if needed
        calculateResponsiveSize();

        // Update position and accessibility
        updatePosition();
        updateAccessibility();

        QWidget::show();
        startShowAnimation();

        // Start auto-hide timer if configured
        if (m_autoHideDelay > 0) {
            m_autoHideTimer->start(m_autoHideDelay);
        }

        emit visibilityChanged(true);
        emit positionChanged(pos());
    }
}

void FluentPopover::hide() {
    if (m_isVisible) {
        m_isVisible = false;
        emit aboutToHide();

        m_autoHideTimer->stop();
        startHideAnimation();

        emit visibilityChanged(false);
    }
}

void FluentPopover::toggle() {
    if (m_isVisible) {
        hide();
    } else {
        show();
    }
}

void FluentPopover::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentPopover::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QRect rect = this->rect();

    // Paint elevation shadow (behind the popover)
    if (m_elevation > 0) {
        paintElevationShadow(painter, rect);
    }

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);

    // Paint arrow
    if (m_showArrow) {
        paintArrow(painter);
    }
}

void FluentPopover::paintBackground(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Create path with rounded corners
    QPainterPath path;
    const int radius = cornerRadius();
    path.addRoundedRect(rect, radius, radius);

    // Use custom background color if set, otherwise use theme color
    QColor backgroundColor = m_customBackgroundColor.isValid()
                                 ? m_customBackgroundColor
                                 : palette.neutralLighter;

    // Add subtle gradient for depth
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0.0, backgroundColor.lighter(105));
    gradient.setColorAt(1.0, backgroundColor.darker(102));

    // Fill background with gradient
    painter.fillPath(path, QBrush(gradient));

    // Add backdrop effect if enabled
    if (m_enableBackdrop) {
        paintBackdrop(painter);
    }
}

void FluentPopover::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    // Use custom border color if set, otherwise use theme color
    QColor borderColor = m_customBorderColor.isValid()
                             ? m_customBorderColor
                             : palette.neutralQuaternary;

    QPen borderPen(borderColor, m_borderWidth);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    const int radius = cornerRadius();
    const qreal adjustment = m_borderWidth / 2.0;
    const QRectF borderRect =
        QRectF(rect).adjusted(adjustment, adjustment, -adjustment, -adjustment);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentPopover::paintBackdrop(QPainter& painter) {
    painter.save();

    // Create a subtle backdrop blur effect
    QColor backdropColor = QColor(255, 255, 255, 20);
    if (Styling::FluentTheme::instance().mode() ==
        Styling::FluentThemeMode::Dark) {
        backdropColor = QColor(0, 0, 0, 30);
    }

    painter.fillRect(rect(), backdropColor);

    painter.restore();
}

void FluentPopover::paintElevationShadow(QPainter& painter, const QRect& rect) {
    if (m_elevation <= 0)
        return;

    painter.save();

    const int radius = cornerRadius();
    const int shadowOffset = 2 + (m_elevation * 2);
    const int shadowBlur = 8 + (m_elevation * 4);
    const int shadowAlpha = 40 + (m_elevation * 10);

    // Create shadow path
    QPainterPath shadowPath;
    QRect shadowRect = rect.adjusted(0, shadowOffset, 0, shadowOffset);
    shadowPath.addRoundedRect(shadowRect, radius, radius);

    // Create shadow gradient
    QRadialGradient shadowGradient(shadowRect.center(), shadowBlur);
    shadowGradient.setColorAt(0.0, QColor(0, 0, 0, qMin(shadowAlpha, 80)));
    shadowGradient.setColorAt(1.0, QColor(0, 0, 0, 0));

    painter.fillPath(shadowPath, QBrush(shadowGradient));

    painter.restore();
}

void FluentPopover::paintArrow(QPainter& painter) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    const int arrowSize = 8;
    QPainterPath arrowPath;

    // Calculate arrow position and direction based on placement
    switch (m_actualPlacement) {
        case FluentPopoverPlacement::Top:
        case FluentPopoverPlacement::TopStart:
        case FluentPopoverPlacement::TopEnd:
            // Arrow pointing down
            arrowPath.moveTo(m_arrowPosition.x() - arrowSize, height());
            arrowPath.lineTo(m_arrowPosition.x(), height() + arrowSize);
            arrowPath.lineTo(m_arrowPosition.x() + arrowSize, height());
            break;

        case FluentPopoverPlacement::Bottom:
        case FluentPopoverPlacement::BottomStart:
        case FluentPopoverPlacement::BottomEnd:
            // Arrow pointing up
            arrowPath.moveTo(m_arrowPosition.x() - arrowSize, 0);
            arrowPath.lineTo(m_arrowPosition.x(), -arrowSize);
            arrowPath.lineTo(m_arrowPosition.x() + arrowSize, 0);
            break;

        case FluentPopoverPlacement::Left:
        case FluentPopoverPlacement::LeftStart:
        case FluentPopoverPlacement::LeftEnd:
            // Arrow pointing right
            arrowPath.moveTo(width(), m_arrowPosition.y() - arrowSize);
            arrowPath.lineTo(width() + arrowSize, m_arrowPosition.y());
            arrowPath.lineTo(width(), m_arrowPosition.y() + arrowSize);
            break;

        case FluentPopoverPlacement::Right:
        case FluentPopoverPlacement::RightStart:
        case FluentPopoverPlacement::RightEnd:
            // Arrow pointing left
            arrowPath.moveTo(0, m_arrowPosition.y() - arrowSize);
            arrowPath.lineTo(-arrowSize, m_arrowPosition.y());
            arrowPath.lineTo(0, m_arrowPosition.y() + arrowSize);
            break;

        default:
            break;
    }

    // Fill arrow
    painter.fillPath(arrowPath, palette.neutralLighter);

    // Draw arrow border
    painter.setPen(QPen(palette.neutralQuaternary, 1));
    painter.drawPath(arrowPath);

    painter.restore();
}

void FluentPopover::mousePressEvent(QMouseEvent* event) {
    // Don't hide on click inside popover
    event->accept();
}

void FluentPopover::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            hide();
            event->accept();
            return;

        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            // Enhanced tab navigation within popover
            if (m_contentWidget) {
                // Let the content widget handle tab navigation
                m_contentWidget->setFocus();
            } else if (m_showCloseButton && m_closeButton->isVisible()) {
                m_closeButton->setFocus();
            }
            event->accept();
            return;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            // Activate focused element or close if no specific action
            if (m_showCloseButton && m_closeButton->hasFocus()) {
                hide();
                event->accept();
                return;
            }
            break;

        default:
            break;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentPopover::focusOutEvent(QFocusEvent* event) {
    // Hide popover when losing focus (unless it's due to clicking inside)
    if (event->reason() != Qt::PopupFocusReason) {
        hide();
    }

    Core::FluentComponent::focusOutEvent(event);
}

void FluentPopover::showEvent(QShowEvent* event) {
    Core::FluentComponent::showEvent(event);
    setFocus();
}

void FluentPopover::hideEvent(QHideEvent* event) {
    Core::FluentComponent::hideEvent(event);
    m_isVisible = false;
}

bool FluentPopover::eventFilter(QObject* object, QEvent* event) {
    if (object == m_target && m_target) {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
                if (m_trigger == FluentPopoverTrigger::Click) {
                    toggle();
                    return true;
                }
                break;

            case QEvent::Enter:
                if (m_trigger == FluentPopoverTrigger::Hover) {
                    show();
                }
                break;

            case QEvent::Leave:
                if (m_trigger == FluentPopoverTrigger::Hover) {
                    hide();
                }
                break;

            case QEvent::FocusIn:
                if (m_trigger == FluentPopoverTrigger::Focus) {
                    show();
                }
                break;

            case QEvent::FocusOut:
                if (m_trigger == FluentPopoverTrigger::Focus) {
                    hide();
                }
                break;

            default:
                break;
        }
    }

    return Core::FluentComponent::eventFilter(object, event);
}

void FluentPopover::updateStateStyle() { update(); }

void FluentPopover::performStateTransition(Core::FluentState from,
                                           Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    update();
}

void FluentPopover::onCloseButtonClicked() { hide(); }

void FluentPopover::onAutoHideTimer() { hide(); }

void FluentPopover::onThemeChanged() {
    updateColors();
    updateFonts();
    update();
}

void FluentPopover::onShowAnimationFinished() { emit shown(); }

void FluentPopover::onHideAnimationFinished() {
    QWidget::hide();
    emit hidden();
}

void FluentPopover::updateColors() {
    // Update shadow effect based on elevation
    updateShadowEffect();

    // Update widget colors based on theme
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Set widget background color for proper rendering
    QPalette widgetPalette = this->palette();
    widgetPalette.setColor(QPalette::Window, palette.neutralLighter);
    widgetPalette.setColor(QPalette::WindowText, palette.neutralPrimary);
    setPalette(widgetPalette);

    // Update child widget colors
    if (m_titleLabel) {
        QPalette titlePalette = m_titleLabel->palette();
        titlePalette.setColor(QPalette::WindowText, palette.neutralPrimary);
        m_titleLabel->setPalette(titlePalette);
    }

    if (m_contentLabel) {
        QPalette contentPalette = m_contentLabel->palette();
        contentPalette.setColor(QPalette::WindowText, palette.neutralSecondary);
        m_contentLabel->setPalette(contentPalette);
    }

    // Colors are also handled in paintEvent for custom drawing
    update();
}

void FluentPopover::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();
    m_titleLabel->setFont(theme.subtitleFont());
    m_contentLabel->setFont(theme.bodyFont());
}

void FluentPopover::startShowAnimation() {
    if (!isAnimated()) {
        m_opacityEffect->setOpacity(1.0);
        emit shown();
        return;
    }

    // Setup scale animation if enabled
    if (m_enableScaleAnimation && m_scaleAnimation) {
        const QRect currentGeometry = geometry();
        const QRect targetGeometry = currentGeometry;
        const QRect startGeometry = QRect(
            currentGeometry.center() - QPoint(currentGeometry.width() / 4,
                                              currentGeometry.height() / 4),
            currentGeometry.size() / 2);

        m_scaleAnimation->setStartValue(startGeometry);
        m_scaleAnimation->setEndValue(targetGeometry);
    }

    m_showAnimationGroup->start();
}

void FluentPopover::startHideAnimation() {
    if (!isAnimated()) {
        m_opacityEffect->setOpacity(0.0);
        QWidget::hide();
        emit hidden();
        return;
    }

    m_hideAnimationGroup->start();
}

void FluentPopover::setupScaleAnimation() {
    m_scaleAnimation = std::make_unique<QPropertyAnimation>(this, "geometry");
    m_scaleAnimation->setDuration(250);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutBack);
}

void FluentPopover::updateAnimationProperties() {
    if (!m_showAnimationGroup || !m_hideAnimationGroup)
        return;

    // Clear existing animations
    m_showAnimationGroup->clear();
    m_hideAnimationGroup->clear();

    // Always add opacity animation
    m_showAnimationGroup->addAnimation(m_showAnimation.get());
    m_hideAnimationGroup->addAnimation(m_hideAnimation.get());

    // Add scale animation if enabled
    if (m_enableScaleAnimation && m_scaleAnimation) {
        m_showAnimationGroup->addAnimation(m_scaleAnimation.get());
        m_hideAnimationGroup->addAnimation(m_scaleAnimation.get());
    }
}

void FluentPopover::updateShadowEffect() {
    if (!m_shadowEffect)
        return;

    // Update shadow based on elevation level
    const int blurRadius = 8 + (m_elevation * 4);
    const int offsetY = 2 + (m_elevation * 2);
    const int alpha = 40 + (m_elevation * 10);

    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(0, offsetY);
    m_shadowEffect->setColor(QColor(0, 0, 0, qMin(alpha, 80)));
}

void FluentPopover::updatePosition() {
    if (!m_target)
        return;

    // Store old position for comparison
    const QPoint oldPosition = pos();

    // Determine optimal placement
    m_actualPlacement = (m_placement == FluentPopoverPlacement::Auto)
                            ? getOptimalPlacement()
                            : m_placement;

    // Calculate position
    const QPoint position = calculatePosition(m_actualPlacement);
    move(position);

    // Update arrow position
    updateArrowPosition();

    // Emit position changed signal if position actually changed
    if (oldPosition != pos()) {
        emit positionChanged(pos());
    }
}

void FluentPopover::updateArrowPosition() {
    if (!m_target || !m_showArrow)
        return;

    const QRect targetRect = m_target->geometry();
    const QPoint targetCenter = m_target->mapToGlobal(targetRect.center());
    const QPoint popoverPos = pos();

    // Calculate arrow position relative to popover
    switch (m_actualPlacement) {
        case FluentPopoverPlacement::Top:
        case FluentPopoverPlacement::Bottom:
            m_arrowPosition.setX(targetCenter.x() - popoverPos.x());
            m_arrowPosition.setX(qBound(16, m_arrowPosition.x(), width() - 16));
            break;

        case FluentPopoverPlacement::Left:
        case FluentPopoverPlacement::Right:
            m_arrowPosition.setY(targetCenter.y() - popoverPos.y());
            m_arrowPosition.setY(
                qBound(16, m_arrowPosition.y(), height() - 16));
            break;

        default:
            break;
    }
}

QPoint FluentPopover::calculatePosition(
    FluentPopoverPlacement placement) const {
    if (!m_target)
        return QPoint();

    const QRect targetRect = m_target->geometry();
    const QPoint targetGlobal = m_target->mapToGlobal(QPoint(0, 0));
    const QSize popoverSize = sizeHint();
    const int arrowOffset = m_showArrow ? 8 : 0;

    QPoint position;

    switch (placement) {
        case FluentPopoverPlacement::Top:
            position =
                QPoint(targetGlobal.x() +
                           (targetRect.width() - popoverSize.width()) / 2,
                       targetGlobal.y() - popoverSize.height() - arrowOffset);
            break;

        case FluentPopoverPlacement::Bottom:
            position =
                QPoint(targetGlobal.x() +
                           (targetRect.width() - popoverSize.width()) / 2,
                       targetGlobal.y() + targetRect.height() + arrowOffset);
            break;

        case FluentPopoverPlacement::Left:
            position =
                QPoint(targetGlobal.x() - popoverSize.width() - arrowOffset,
                       targetGlobal.y() +
                           (targetRect.height() - popoverSize.height()) / 2);
            break;

        case FluentPopoverPlacement::Right:
            position =
                QPoint(targetGlobal.x() + targetRect.width() + arrowOffset,
                       targetGlobal.y() +
                           (targetRect.height() - popoverSize.height()) / 2);
            break;

        default:
            position = targetGlobal;
            break;
    }

    return position;
}

FluentPopoverPlacement FluentPopover::getOptimalPlacement() const {
    if (!m_target)
        return FluentPopoverPlacement::Top;

    const QRect targetRect = m_target->geometry();
    const QPoint targetGlobal = m_target->mapToGlobal(QPoint(0, 0));
    const QSize popoverSize = sizeHint();
    const QRect screenRect = QApplication::primaryScreen()->availableGeometry();

    // Check available space in each direction
    const int spaceTop = targetGlobal.y() - screenRect.top();
    const int spaceBottom =
        screenRect.bottom() - (targetGlobal.y() + targetRect.height());
    const int spaceLeft = targetGlobal.x() - screenRect.left();
    const int spaceRight =
        screenRect.right() - (targetGlobal.x() + targetRect.width());

    // Choose placement with most available space
    if (spaceBottom >= popoverSize.height()) {
        return FluentPopoverPlacement::Bottom;
    } else if (spaceTop >= popoverSize.height()) {
        return FluentPopoverPlacement::Top;
    } else if (spaceRight >= popoverSize.width()) {
        return FluentPopoverPlacement::Right;
    } else if (spaceLeft >= popoverSize.width()) {
        return FluentPopoverPlacement::Left;
    } else {
        return FluentPopoverPlacement::Bottom;  // Default fallback
    }
}

void FluentPopover::installTargetEventFilter() {
    if (m_target && m_trigger != FluentPopoverTrigger::Manual) {
        m_target->installEventFilter(this);
    }
}

void FluentPopover::removeTargetEventFilter() {
    if (m_target) {
        m_target->removeEventFilter(this);
    }
}

void FluentPopover::updateAccessibility() {
    // Set accessible name
    if (!m_accessibleName.isEmpty()) {
        setAccessibleName(m_accessibleName);
    } else if (!m_title.isEmpty()) {
        setAccessibleName(m_title);
    } else if (!m_ariaLabel.isEmpty()) {
        setAccessibleName(m_ariaLabel);
    }

    // Set accessible description
    if (!m_accessibleDescription.isEmpty()) {
        setAccessibleDescription(m_accessibleDescription);
    } else if (!m_content.isEmpty()) {
        setAccessibleDescription(m_content);
    }

    // Set ARIA properties as dynamic properties for CSS/styling
    if (!m_ariaLabel.isEmpty()) {
        setProperty("aria-label", m_ariaLabel);
    }
    if (!m_ariaDescribedBy.isEmpty()) {
        setProperty("aria-describedby", m_ariaDescribedBy);
    }

    // Set role
    setProperty("role", "dialog");
    setProperty("aria-modal", "true");
}

void FluentPopover::calculateResponsiveSize() {
    if (!m_adaptiveWidth && !m_adaptiveHeight)
        return;

    const QSize adaptiveSize = getAdaptiveSize();

    if (m_adaptiveWidth) {
        const int newWidth =
            qBound(m_minWidth, adaptiveSize.width(), m_maxWidth);
        if (width() != newWidth) {
            resize(newWidth, height());
            emit sizeChanged(size());
        }
    }

    if (m_adaptiveHeight) {
        const int newHeight = qBound(50, adaptiveSize.height(), m_maxHeight);
        if (height() != newHeight) {
            resize(width(), newHeight);
            emit sizeChanged(size());
        }
    }
}

QSize FluentPopover::getAdaptiveSize() const {
    if (!m_target)
        return sizeHint();

    // Get screen geometry
    const QRect screenGeometry =
        QApplication::primaryScreen()->availableGeometry();
    const QRect targetGeometry = m_target->geometry();
    const QPoint targetGlobal = m_target->mapToGlobal(QPoint(0, 0));

    // Calculate available space
    const int availableWidth =
        screenGeometry.width() - targetGlobal.x() - 40;  // 40px margin
    const int availableHeight = screenGeometry.height() - targetGlobal.y() -
                                targetGeometry.height() - 40;

    // Calculate adaptive size based on content and available space
    QSize contentSize = sizeHint();

    if (m_adaptiveWidth) {
        contentSize.setWidth(qMin(contentSize.width(), availableWidth));
    }

    if (m_adaptiveHeight) {
        contentSize.setHeight(qMin(contentSize.height(), availableHeight));
    }

    return contentSize;
}

}  // namespace FluentQt::Components
