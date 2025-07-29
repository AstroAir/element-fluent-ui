// src/Components/FluentNavigationView.cpp
#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHash>
#include <QTimer>
#include <QSplitter>
#include <memory>
#include <QToolButton>
#include <QLabel>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QApplication>
#include <QScreen>

namespace FluentQt::Components {

class FluentNavButton : public QToolButton {
    Q_OBJECT

public:
    explicit FluentNavButton(const FluentNavigationItem& item, QWidget* parent = nullptr)
        : QToolButton(parent), m_item(item), m_selected(false) {
        
        setText(item.text);
        setIcon(item.icon);
        setToolTip(item.text);
        setCheckable(true);
        setAutoRaise(true);
        
        setFixedHeight(40);
        setIconSize(QSize(16, 16));
        
        // Connect theme changes
        connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
                this, &FluentNavButton::updateStyle);
        
        updateStyle();
    }

    void setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            setChecked(selected);
            updateStyle();
        }
    }

    bool isSelected() const { return m_selected; }
    const FluentNavigationItem& item() const { return m_item; }

signals:
    void itemClicked(const QString& tag);

protected:
    void paintEvent(QPaintEvent* event) override {
        FLUENT_PROFILE("FluentNavButton::paintEvent");
        
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        const QRect rect = this->rect();
        const auto& theme = Styling::FluentTheme::instance();
        const auto& palette = theme.currentPalette();
        
        // Paint background
        if (m_selected) {
            painter.fillRect(rect, palette.accent.lighter(180));
            
            // Selection indicator
            QRect indicator(0, 0, 3, rect.height());
            painter.fillRect(indicator, palette.accent);
        } else if (underMouse()) {
            painter.fillRect(rect, palette.neutralLight);
        }
        
        // Paint icon and text
        const int iconSize = 16;
        const int margin = 16;
        const int spacing = 12;
        
        QRect iconRect(margin, (rect.height() - iconSize) / 2, iconSize, iconSize);
        QRect textRect(margin + iconSize + spacing, 0, 
                      rect.width() - margin - iconSize - spacing - margin, rect.height());
        
        // Draw icon
        if (!icon().isNull()) {
            QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
            QIcon::State state = m_selected ? QIcon::On : QIcon::Off;
            icon().paint(&painter, iconRect, Qt::AlignCenter, mode, state);
        }
        
        // Draw text
        painter.setPen(m_selected ? palette.accent : palette.neutralPrimary);
        painter.setFont(font());
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
    }

    void mousePressEvent(QMouseEvent* event) override {
        QToolButton::mousePressEvent(event);
        emit itemClicked(m_item.tag);
    }

    void enterEvent(QEnterEvent* event) override {
        QToolButton::enterEvent(event);
        if (!m_selected) {
            update();
        }
    }

    void leaveEvent(QEvent* event) override {
        QToolButton::leaveEvent(event);
        if (!m_selected) {
            update();
        }
    }

private slots:
    void updateStyle() {
        const auto& theme = Styling::FluentTheme::instance();
        setFont(theme.bodyFont());
        update();
    }

private:
    FluentNavigationItem m_item;
    bool m_selected;
};

FluentNavigationView::FluentNavigationView(QWidget* parent)
    : Core::FluentComponent(parent)
    , m_splitter(new QSplitter(Qt::Horizontal, this))
    , m_paneWidget(new QWidget())
    , m_contentWidget(new QWidget())
    , m_contentStack(new QStackedWidget())
    , m_paneLayout(new QVBoxLayout(m_paneWidget))
    , m_hamburgerButton(new QToolButton())
{
    setupLayout();
    setupPaneContent();
    
    // Initialize responsive behavior
    connect(this, &FluentNavigationView::resizeEvent, 
            this, &FluentNavigationView::updateDisplayModeFromWidth);
            
    // Connect theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, [this]() { update(); });
}

void FluentNavigationView::setupLayout() {
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Setup splitter
    m_splitter->addWidget(m_paneWidget);
    m_splitter->addWidget(m_contentWidget);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    
    // Content widget setup
    auto* contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->addWidget(m_contentStack);
    
    mainLayout->addWidget(m_splitter);
    
    // Initial pane width
    updatePaneWidth();
}

void FluentNavigationView::setupPaneContent() {
    m_paneLayout->setContentsMargins(0, 0, 0, 0);
    m_paneLayout->setSpacing(0);
    
    // Hamburger button
    m_hamburgerButton->setIcon(QIcon(":/icons/hamburger.png"));
    m_hamburgerButton->setFixedHeight(48);
    m_hamburgerButton->setIconSize(QSize(16, 16));
    m_hamburgerButton->setToolTip("Toggle navigation pane");
    
    connect(m_hamburgerButton, &QToolButton::clicked,
            this, &FluentNavigationView::onPaneToggleRequested);
    
    m_paneLayout->addWidget(m_hamburgerButton);
    
    // Header content placeholder
    if (m_headerContent) {
        m_paneLayout->addWidget(m_headerContent);
    }
    
    // Navigation items will be added here
    m_paneLayout->addStretch();
    
    // Footer content placeholder
    if (m_footerContent) {
        m_paneLayout->addWidget(m_footerContent);
    }
    
    // Style the pane
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    
    QString paneStyle = QString(
        "QWidget { "
        "  background-color: %1; "
        "  border-right: 1px solid %2; "
        "}"
    ).arg(palette.neutralLightest.name())
     .arg(palette.neutralQuaternaryAlt.name());
    
    m_paneWidget->setStyleSheet(paneStyle);
}

void FluentNavigationView::addNavigationItem(const FluentNavigationItem& item) {
    m_navigationItems.push_back(item);
    
    // Create button for the item
    auto* button = new FluentNavButton(item, m_paneWidget);
    connect(button, &FluentNavButton::itemClicked,
            this, &FluentNavigationView::onItemClicked);
    
    // Insert before stretch
    int insertIndex = m_paneLayout->count() - 1;
    if (m_footerContent) {
        insertIndex -= 1; // Account for footer
    }
    
    m_paneLayout->insertWidget(insertIndex, button);
    m_itemButtons[item.tag] = button;
    
    // Add content to stack if provided
    if (item.content) {
        m_contentStack->addWidget(item.content);
        m_contentPages[item.tag] = item.content;
    }
    
    updateItemVisibility();
}

void FluentNavigationView::setSelectedItemTag(const QString& tag) {
    if (m_selectedItemTag == tag) {
        return;
    }
    
    // Deselect previous item
    if (!m_selectedItemTag.isEmpty() && m_itemButtons.contains(m_selectedItemTag)) {
        static_cast<FluentNavButton*>(m_itemButtons[m_selectedItemTag])->setSelected(false);
    }
    
    m_selectedItemTag = tag;
    
    // Select new item
    if (m_itemButtons.contains(tag)) {
        static_cast<FluentNavButton*>(m_itemButtons[tag])->setSelected(true);
        
        // Switch content
        if (m_contentPages.contains(tag)) {
            m_contentStack->setCurrentWidget(m_contentPages[tag]);
        }
        
        emit selectedItemChanged(tag);
    }
}

void FluentNavigationView::setDisplayMode(FluentNavigationDisplayMode mode) {
    if (m_displayMode == mode) {
        return;
    }
    
    m_displayMode = mode;
    updatePaneWidth();
    updateItemVisibility();
    emit displayModeChanged(mode);
}

void FluentNavigationView::setIsPaneOpen(bool open) {
    if (m_isPaneOpen == open) {
        return;
    }
    
    m_isPaneOpen = open;
    animatePaneTransition();
    emit paneOpenChanged(open);
}

void FluentNavigationView::updatePaneWidth() {
    int targetWidth;
    
    switch (m_displayMode) {
        case FluentNavigationDisplayMode::Minimal:
            targetWidth = 0;
            break;
        case FluentNavigationDisplayMode::Compact:
            targetWidth = m_compactPaneWidth;
            break;
        case FluentNavigationDisplayMode::Expanded:
            targetWidth = m_isPaneOpen ? m_expandedPaneWidth : m_compactPaneWidth;
            break;
        case FluentNavigationDisplayMode::Auto:
            // Will be handled by responsive logic
            return;
    }
    
    m_splitter->setSizes({targetWidth, width() - targetWidth});
}

void FluentNavigationView::updateItemVisibility() {
    bool showText = (m_displayMode == FluentNavigationDisplayMode::Expanded && m_isPaneOpen) ||
                    m_displayMode == FluentNavigationDisplayMode::Auto;
    
    for (auto it = m_itemButtons.begin(); it != m_itemButtons.end(); ++it) {
        auto* navButton = static_cast<FluentNavButton*>(it.value());
        navButton->setToolButtonStyle(showText ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly);
    }
}

void FluentNavigationView::animatePaneTransition() {
    if (!m_paneAnimation) {
        m_paneAnimation = new QPropertyAnimation(this, "paneWidth");
        m_paneAnimation->setDuration(400); // Increased for smoother animation
        m_paneAnimation->setEasingCurve(QEasingCurve::OutBack); // Spring-like easing
    }

    int currentWidth = m_splitter->sizes().first();
    int targetWidth = m_isPaneOpen ? m_expandedPaneWidth : m_compactPaneWidth;

    m_paneAnimation->setStartValue(currentWidth);
    m_paneAnimation->setEndValue(targetWidth);

    connect(m_paneAnimation, &QPropertyAnimation::valueChanged,
            this, [this](const QVariant& value) {
                int width = value.toInt();
                m_splitter->setSizes({width, this->width() - width});
            });

    connect(m_paneAnimation, &QPropertyAnimation::finished,
            this, [this]() {
                updateItemVisibility();
                animateNavigationItems(m_isPaneOpen);
                m_paneAnimation->disconnect();
            });

    // Start with staggered item animations if expanding
    if (m_isPaneOpen) {
        animateNavigationItems(true);
    }

    m_paneAnimation->start();
}

void FluentNavigationView::animateNavigationItems(bool expanding) {
    const int staggerDelay = 50; // 50ms between each item
    int delay = 0;

    for (auto it = m_itemButtons.begin(); it != m_itemButtons.end(); ++it) {
        auto* navButton = it.value();

        QTimer::singleShot(delay, [this, navButton, expanding]() {
            animateNavigationItem(navButton, expanding);
        });

        delay += staggerDelay;
    }
}

void FluentNavigationView::animateNavigationItem(QWidget* item, bool expanding) {
    if (!item) return;

    // Create opacity animation
    QGraphicsOpacityEffect* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(item->graphicsEffect());

    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(item);
        item->setGraphicsEffect(opacityEffect);
    }

    auto opacityAnim = new QPropertyAnimation(opacityEffect, "opacity", this);
    opacityAnim->setDuration(300);
    opacityAnim->setEasingCurve(QEasingCurve::OutCubic);

    if (expanding) {
        // Fade in with slide
        opacityEffect->setOpacity(0.0);
        opacityAnim->setStartValue(0.0);
        opacityAnim->setEndValue(1.0);

        // Add slide animation using position
        QPoint originalPos = item->pos();
        item->move(originalPos.x() - 20, originalPos.y());

        auto slideAnim = new QPropertyAnimation(item, "pos", this);
        slideAnim->setStartValue(item->pos());
        slideAnim->setEndValue(originalPos);
        slideAnim->setDuration(300);
        slideAnim->setEasingCurve(QEasingCurve::OutCubic);

        auto group = new QParallelAnimationGroup(this);
        group->addAnimation(opacityAnim);
        group->addAnimation(slideAnim);
        group->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        // Fade out
        opacityAnim->setStartValue(1.0);
        opacityAnim->setEndValue(0.3);
        opacityAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void FluentNavigationView::animatePageTransition(QWidget* fromPage, QWidget* toPage) {
    if (!fromPage || !toPage) return;

    // Create slide transition between pages
    QGraphicsOpacityEffect* fromOpacity = new QGraphicsOpacityEffect(fromPage);
    QGraphicsOpacityEffect* toOpacity = new QGraphicsOpacityEffect(toPage);

    fromPage->setGraphicsEffect(fromOpacity);
    toPage->setGraphicsEffect(toOpacity);

    // Setup initial states
    fromOpacity->setOpacity(1.0);
    toOpacity->setOpacity(0.0);
    toPage->show();

    // Create animations
    auto fromFadeAnim = new QPropertyAnimation(fromOpacity, "opacity", this);
    fromFadeAnim->setStartValue(1.0);
    fromFadeAnim->setEndValue(0.0);
    fromFadeAnim->setDuration(300);
    fromFadeAnim->setEasingCurve(QEasingCurve::OutCubic);

    auto toFadeAnim = new QPropertyAnimation(toOpacity, "opacity", this);
    toFadeAnim->setStartValue(0.0);
    toFadeAnim->setEndValue(1.0);
    toFadeAnim->setDuration(300);
    toFadeAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Add slide effect
    QPoint toOriginalPos = toPage->pos();
    toPage->move(toOriginalPos.x() + 50, toOriginalPos.y());

    auto slideAnim = new QPropertyAnimation(toPage, "pos", this);
    slideAnim->setStartValue(toPage->pos());
    slideAnim->setEndValue(toOriginalPos);
    slideAnim->setDuration(300);
    slideAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Create parallel group
    auto group = new QParallelAnimationGroup(this);
    group->addAnimation(fromFadeAnim);
    group->addAnimation(toFadeAnim);
    group->addAnimation(slideAnim);

    // Cleanup when finished
    connect(group, &QParallelAnimationGroup::finished, [fromPage, toPage]() {
        fromPage->setGraphicsEffect(nullptr);
        toPage->setGraphicsEffect(nullptr);
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void FluentNavigationView::animateItemSelection(QWidget* item, bool selected) {
    if (!item) return;

    // Create scale animation for selection feedback
    QPropertyAnimation* scaleAnim = new QPropertyAnimation(item, "geometry", this);

    QRect currentGeometry = item->geometry();
    QRect targetGeometry = currentGeometry;

    if (selected) {
        // Slightly scale up when selected
        targetGeometry.adjust(-2, -1, 2, 1);
    }

    scaleAnim->setStartValue(currentGeometry);
    scaleAnim->setEndValue(targetGeometry);
    scaleAnim->setDuration(150);
    scaleAnim->setEasingCurve(QEasingCurve::OutQuad);

    // Add glow effect for selection
    if (selected) {
        // Add subtle glow animation
        QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect(item);
        glowEffect->setBlurRadius(8);
        glowEffect->setColor(QColor(0, 120, 215, 100));
        glowEffect->setOffset(0, 0);
        item->setGraphicsEffect(glowEffect);

        // Animate glow intensity
        auto glowAnim = new QPropertyAnimation(glowEffect, "color", this);
        glowAnim->setStartValue(QColor(0, 120, 215, 0));
        glowAnim->setEndValue(QColor(0, 120, 215, 100));
        glowAnim->setDuration(200);
        glowAnim->setEasingCurve(QEasingCurve::OutCubic);
        glowAnim->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        // Remove glow effect
        item->setGraphicsEffect(nullptr);
    }

    scaleAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void FluentNavigationView::onItemClicked(const QString& tag) {
    setSelectedItemTag(tag);
    emit itemInvoked(tag);
    
    // Auto-collapse pane on mobile/compact screens
    if (m_displayMode == FluentNavigationDisplayMode::Auto && width() < m_compactModeThreshold) {
        setIsPaneOpen(false);
    }
}

void FluentNavigationView::onPaneToggleRequested() {
    setIsPaneOpen(!m_isPaneOpen);
}

void FluentNavigationView::updateDisplayModeFromWidth() {
    if (m_displayMode != FluentNavigationDisplayMode::Auto) {
        return;
    }
    
    int currentWidth = width();
    
    if (currentWidth < m_compactModeThreshold) {
        // Mobile/narrow: Minimal mode with overlay
        m_splitter->setSizes({0, currentWidth});
        setIsPaneOpen(false);
    } else if (currentWidth < m_expandedModeThreshold) {
        // Tablet: Compact mode
        m_splitter->setSizes({m_compactPaneWidth, currentWidth - m_compactPaneWidth});
        setIsPaneOpen(false);
    } else {
        // Desktop: Expanded mode
        m_splitter->setSizes({m_expandedPaneWidth, currentWidth - m_expandedPaneWidth});
        setIsPaneOpen(true);
    }
    
    updateItemVisibility();
}

void FluentNavigationView::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
    QTimer::singleShot(0, this, &FluentNavigationView::updateDisplayModeFromWidth);
}

void FluentNavigationView::paintEvent(QPaintEvent* event) {
    FLUENT_PROFILE("FluentNavigationView::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Paint background
    painter.fillRect(rect(), palette.neutralLightest);

    Core::FluentComponent::paintEvent(event);
}

// Missing slot implementations
void FluentNavigationView::onBackButtonClicked() {
    emit backRequested();
}

void FluentNavigationView::onNavigationItemClicked(QListWidgetItem* item) {
    if (item) {
        QString tag = item->data(Qt::UserRole).toString();
        setSelectedItemTag(tag);
        emit itemInvoked(tag);
    }
}

void FluentNavigationView::onPaneToggleClicked() {
    togglePane();
}

// Missing public methods
void FluentNavigationView::closePane() {
    setIsPaneOpen(false);
}

void FluentNavigationView::openPane() {
    setIsPaneOpen(true);
}

void FluentNavigationView::togglePane() {
    setIsPaneOpen(!m_isPaneOpen);
}

void FluentNavigationView::setSettingsVisible(bool visible) {
    // Implementation for settings visibility
    Q_UNUSED(visible)
    // This would control visibility of settings item in navigation
}

void FluentNavigationView::setBackButtonVisible(bool visible) {
    // Implementation for back button visibility
    Q_UNUSED(visible)
    // This would control visibility of back button
}

void FluentNavigationView::setHeader(const QString& header) {
    // Implementation for setting header text
    Q_UNUSED(header)
    // This would set header content in the pane
}

void FluentNavigationView::setPaneOpen(bool open) {
    setIsPaneOpen(open);
}

QSize FluentNavigationView::sizeHint() const {
    return QSize(800, 600);
}

QSize FluentNavigationView::minimumSizeHint() const {
    return QSize(320, 240);
}

} // namespace FluentQt::Components

#include "FluentNavigationView.moc"
