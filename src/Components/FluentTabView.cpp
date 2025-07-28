// src/Components/FluentTabView.cpp
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QScrollArea>
#include <QScroller>

namespace FluentQt::Components {

// FluentTabButton Implementation
FluentTabButton::FluentTabButton(const FluentTabData& data, QWidget* parent)
    : FluentButton(parent), m_tabData(data) {
    
    setText(data.text);
    setIcon(data.icon);
    setToolTip(data.tooltip);
    
    setButtonStyle(FluentButtonStyle::Subtle);
    setCheckable(true);
    
    setupContextMenu();
    updateButtonAppearance();
}

void FluentTabButton::setTabData(const FluentTabData& data) {
    m_tabData = data;
    setText(data.text);
    setIcon(data.icon);
    setToolTip(data.tooltip);
    updateButtonAppearance();
}

void FluentTabButton::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        setChecked(selected);
        updateButtonAppearance();
    }
}

void FluentTabButton::setPinned(bool pinned) {
    if (m_tabData.pinned != pinned) {
        m_tabData.pinned = pinned;
        updateButtonAppearance();
        emit pinRequested(pinned);
    }
}

void FluentTabButton::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    
    QRect tabRect = rect();
    
    // Paint tab background
    QColor backgroundColor;
    if (m_selected) {
        backgroundColor = palette.accent;
    } else if (state() == Core::FluentState::Hovered) {
        backgroundColor = palette.neutralLight;
    } else {
        backgroundColor = Qt::transparent;
    }
    
    if (backgroundColor != Qt::transparent) {
        QPainterPath path;
        path.addRoundedRect(tabRect, cornerRadius(), cornerRadius());
        painter.fillPath(path, backgroundColor);
    }
    
    // Paint tab content
    QRect contentRect = tabRect.adjusted(12, 6, -12, -6);
    
    // Icon
    int x = contentRect.x();
    if (!icon().isNull()) {
        QSize iconSize(16, 16);
        QRect iconRect(x, contentRect.center().y() - iconSize.height() / 2, 
                      iconSize.width(), iconSize.height());
        icon().paint(&painter, iconRect);
        x += iconSize.width() + 6;
    }
    
    // Text
    if (!text().isEmpty()) {
        painter.setPen(m_selected ? palette.neutralLightest : palette.neutralPrimary);
        painter.setFont(font());
        
        QRect textRect(x, contentRect.y(), 
                      contentRect.right() - x - (m_tabData.closeable ? 20 : 0), 
                      contentRect.height());
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
    }
    
    // Close button
    if (m_tabData.closeable && (m_selected || state() == Core::FluentState::Hovered)) {
        QRect closeRect(tabRect.right() - 20, tabRect.center().y() - 8, 16, 16);
        
        painter.setPen(QPen(palette.neutralSecondary, 1.5));
        painter.drawLine(closeRect.topLeft() + QPoint(4, 4), closeRect.bottomRight() - QPoint(4, 4));
        painter.drawLine(closeRect.topRight() + QPoint(-4, 4), closeRect.bottomLeft() - QPoint(-4, 4));
    }
    
    // Pin indicator
    if (m_tabData.pinned) {
        QRect pinRect(tabRect.right() - 8, tabRect.top() + 4, 4, 4);
        painter.fillRect(pinRect, palette.accent);
    }
}

void FluentTabButton::contextMenuEvent(QContextMenuEvent* event) {
    if (m_contextMenu) {
        emit contextMenuRequested(event->pos());
        m_contextMenu->popup(event->globalPos());
    }
    event->accept();
}

void FluentTabButton::updateButtonAppearance() {
    setButtonStyle(m_selected ? FluentButtonStyle::Primary : FluentButtonStyle::Subtle);
    update();
}

void FluentTabButton::setupContextMenu() {
    m_contextMenu = new QMenu(this);
    
    auto* closeAction = m_contextMenu->addAction("Close Tab");
    connect(closeAction, &QAction::triggered, this, &FluentTabButton::closeRequested);
    
    auto* pinAction = m_contextMenu->addAction(m_tabData.pinned ? "Unpin Tab" : "Pin Tab");
    connect(pinAction, &QAction::triggered, [this]() {
        setPinned(!m_tabData.pinned);
    });
    
    m_contextMenu->addSeparator();
    m_contextMenu->addAction("Close Other Tabs");
    m_contextMenu->addAction("Close Tabs to the Right");
    m_contextMenu->addAction("Close Tabs to the Left");
}

// FluentTabView Implementation
FluentTabView::FluentTabView(QWidget* parent) : Core::FluentComponent(parent) {
    setupTabBar();
    setupContentArea();
    
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentTabView::updateTabLayout);
}

void FluentTabView::setTabPosition(FluentTabPosition position) {
    if (m_tabPosition != position) {
        m_tabPosition = position;
        
        // Recreate layout with new orientation
        delete m_mainLayout;
        setupTabBar();
        setupContentArea();
        
        emit tabPositionChanged(position);
    }
}

void FluentTabView::setTabStyle(FluentTabStyle style) {
    if (m_tabStyle != style) {
        m_tabStyle = style;
        updateTabLayout();
        emit tabStyleChanged(style);
    }
}

void FluentTabView::setCurrentIndex(int index) {
    if (index >= 0 && index < static_cast<int>(m_tabData.size()) && index != m_currentIndex) {
        int oldIndex = m_currentIndex;
        m_currentIndex = index;
        
        // Update tab buttons
        for (int i = 0; i < static_cast<int>(m_tabButtons.size()); ++i) {
            m_tabButtons[i]->setSelected(i == index);
        }
        
        // Switch content
        if (m_tabData[index].content) {
            m_contentStack->setCurrentWidget(m_tabData[index].content);
        }
        
        // Animate transition
        if (oldIndex >= 0 && isAnimated()) {
            animateTabTransition(oldIndex, index);
        }
        
        emit currentChanged(index);
    }
}

QWidget* FluentTabView::currentWidget() const {
    if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_tabData.size())) {
        return m_tabData[m_currentIndex].content;
    }
    return nullptr;
}

void FluentTabView::setCurrentWidget(QWidget* widget) {
    for (int i = 0; i < static_cast<int>(m_tabData.size()); ++i) {
        if (m_tabData[i].content == widget) {
            setCurrentIndex(i);
            break;
        }
    }
}

int FluentTabView::addTab(const QString& text, QWidget* widget) {
    FluentTabData data;
    data.text = text;
    data.content = widget;
    return addTab(data);
}

int FluentTabView::addTab(const QIcon& icon, const QString& text, QWidget* widget) {
    FluentTabData data;
    data.icon = icon;
    data.text = text;
    data.content = widget;
    return addTab(data);
}

int FluentTabView::addTab(const FluentTabData& data) {
    return insertTab(static_cast<int>(m_tabData.size()), data);
}

int FluentTabView::insertTab(int index, const FluentTabData& data) {
    if (index < 0 || index > static_cast<int>(m_tabData.size())) {
        index = static_cast<int>(m_tabData.size());
    }
    
    // Insert data
    m_tabData.insert(m_tabData.begin() + index, data);
    
    // Create tab button
    auto* tabButton = createTabButton(data);
    m_tabButtons.insert(m_tabButtons.begin() + index, tabButton);
    m_tabBarLayout->insertWidget(index, tabButton);
    
    // Add content to stack
    if (data.content) {
        m_contentStack->addWidget(data.content);
    }
    
    // Update current index
    if (m_currentIndex < 0) {
        setCurrentIndex(0);
    } else if (index <= m_currentIndex) {
        m_currentIndex++;
    }
    
    updateTabPositions();
    return index;
}

void FluentTabView::removeTab(int index) {
    if (index < 0 || index >= static_cast<int>(m_tabData.size())) {
        return;
    }
    
    // Remove tab button
    auto* button = m_tabButtons[index];
    m_tabBarLayout->removeWidget(button);
    m_tabButtons.erase(m_tabButtons.begin() + index);
    button->deleteLater();
    
    // Remove content
    if (m_tabData[index].content) {
        m_contentStack->removeWidget(m_tabData[index].content);
    }
    
    // Remove data
    m_tabData.erase(m_tabData.begin() + index);
    
    // Update current index
    if (index == m_currentIndex) {
        if (!m_tabData.empty()) {
            int newIndex = qMin(index, static_cast<int>(m_tabData.size()) - 1);
            setCurrentIndex(newIndex);
        } else {
            m_currentIndex = -1;
        }
    } else if (index < m_currentIndex) {
        m_currentIndex--;
    }
    
    updateTabPositions();
}

int FluentTabView::count() const {
    return static_cast<int>(m_tabData.size());
}

QString FluentTabView::tabText(int index) const {
    if (index >= 0 && index < static_cast<int>(m_tabData.size())) {
        return m_tabData[index].text;
    }
    return QString();
}

void FluentTabView::setTabText(int index, const QString& text) {
    if (index >= 0 && index < static_cast<int>(m_tabData.size())) {
        m_tabData[index].text = text;
        m_tabButtons[index]->setText(text);
    }
}

QVariantMap FluentTabView::saveSession() const {
    QVariantMap session;
    session["currentIndex"] = m_currentIndex;
    session["tabPosition"] = static_cast<int>(m_tabPosition);
    session["tabStyle"] = static_cast<int>(m_tabStyle);
    
    QVariantList tabs;
    for (const auto& data : m_tabData) {
        QVariantMap tabMap;
        tabMap["text"] = data.text;
        tabMap["tooltip"] = data.tooltip;
        tabMap["closeable"] = data.closeable;
        tabMap["pinned"] = data.pinned;
        tabMap["userData"] = data.userData;
        tabs.append(tabMap);
    }
    session["tabs"] = tabs;
    
    return session;
}

void FluentTabView::restoreSession(const QVariantMap& session) {
    // Clear existing tabs
    while (count() > 0) {
        removeTab(0);
    }
    
    // Restore properties
    setTabPosition(static_cast<FluentTabPosition>(session.value("tabPosition", 0).toInt()));
    setTabStyle(static_cast<FluentTabStyle>(session.value("tabStyle", 0).toInt()));
    
    // Restore tabs
    QVariantList tabs = session.value("tabs").toList();
    for (const auto& tabVariant : tabs) {
        QVariantMap tabMap = tabVariant.toMap();
        
        FluentTabData data;
        data.text = tabMap.value("text").toString();
        data.tooltip = tabMap.value("tooltip").toString();
        data.closeable = tabMap.value("closeable", true).toBool();
        data.pinned = tabMap.value("pinned", false).toBool();
        data.userData = tabMap.value("userData").toMap();
        
        addTab(data);
    }
    
    // Restore current index
    int currentIndex = session.value("currentIndex", 0).toInt();
    if (currentIndex >= 0 && currentIndex < count()) {
        setCurrentIndex(currentIndex);
    }
}

void FluentTabView::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    
    // Paint tab bar background
    QRect tabBarRect = m_tabBar->geometry();
    painter.fillRect(tabBarRect, palette.neutralLighter);
    
    // Paint tab bar border
    QPen borderPen(palette.neutralQuaternary, 1);
    painter.setPen(borderPen);
    
    switch (m_tabPosition) {
        case FluentTabPosition::Top:
            painter.drawLine(tabBarRect.bottomLeft(), tabBarRect.bottomRight());
            break;
        case FluentTabPosition::Bottom:
            painter.drawLine(tabBarRect.topLeft(), tabBarRect.topRight());
            break;
        case FluentTabPosition::Left:
            painter.drawLine(tabBarRect.topRight(), tabBarRect.bottomRight());
            break;
        case FluentTabPosition::Right:
            painter.drawLine(tabBarRect.topLeft(), tabBarRect.bottomLeft());
            break;
    }
    
    Core::FluentComponent::paintEvent(event);
}

void FluentTabView::onTabButtonClicked() {
    auto* button = qobject_cast<FluentTabButton*>(sender());
    if (!button) return;
    
    for (int i = 0; i < static_cast<int>(m_tabButtons.size()); ++i) {
        if (m_tabButtons[i] == button) {
            setCurrentIndex(i);
            emit tabBarClicked(i);
            break;
        }
    }
}

void FluentTabView::onTabCloseRequested() {
    auto* button = qobject_cast<FluentTabButton*>(sender());
    if (!button) return;
    
    for (int i = 0; i < static_cast<int>(m_tabButtons.size()); ++i) {
        if (m_tabButtons[i] == button) {
            emit tabCloseRequested(i);
            break;
        }
    }
}

void FluentTabView::onAddButtonClicked() {
    emit addTabRequested();
}

void FluentTabView::setupTabBar() {
    if (m_tabPosition == FluentTabPosition::Top || m_tabPosition == FluentTabPosition::Bottom) {
        m_mainLayout = new QVBoxLayout(this);
    } else {
        m_mainLayout = new QHBoxLayout(this);
    }
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Tab bar container
    m_tabBar = new QWidget(this);
    m_tabBarLayout = new QHBoxLayout(m_tabBar);
    m_tabBarLayout->setContentsMargins(8, 4, 8, 4);
    m_tabBarLayout->setSpacing(2);
    
    if (m_scrollable) {
        m_tabScrollArea = new QScrollArea(this);
        m_tabScrollArea->setWidget(m_tabBar);
        m_tabScrollArea->setWidgetResizable(true);
        m_tabScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_tabScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_tabScrollArea->setFrameShape(QFrame::NoFrame);
    }
    
    // Add button
    if (m_showAddButton) {
        m_addButton = new FluentButton("+", this);
        m_addButton->setButtonStyle(FluentButtonStyle::Subtle);
        m_addButton->setFixedSize(32, 32);
        connect(m_addButton, &FluentButton::clicked, this, &FluentTabView::onAddButtonClicked);
        m_tabBarLayout->addWidget(m_addButton);
    }
    
    m_tabBarLayout->addStretch();
}

void FluentTabView::setupContentArea() {
    m_contentStack = new QStackedWidget(this);
    
    // Add widgets to layout based on tab position
    if (m_tabPosition == FluentTabPosition::Top) {
        m_mainLayout->addWidget(m_scrollable ? m_tabScrollArea : m_tabBar);
        m_mainLayout->addWidget(m_contentStack);
    } else if (m_tabPosition == FluentTabPosition::Bottom) {
        m_mainLayout->addWidget(m_contentStack);
        m_mainLayout->addWidget(m_scrollable ? m_tabScrollArea : m_tabBar);
    } else if (m_tabPosition == FluentTabPosition::Left) {
        m_mainLayout->addWidget(m_scrollable ? m_tabScrollArea : m_tabBar);
        m_mainLayout->addWidget(m_contentStack);
    } else { // Right
        m_mainLayout->addWidget(m_contentStack);
        m_mainLayout->addWidget(m_scrollable ? m_tabScrollArea : m_tabBar);
    }
}

FluentTabButton* FluentTabView::createTabButton(const FluentTabData& data) {
    auto* button = new FluentTabButton(data, this);
    
    connect(button, &FluentTabButton::clicked, this, &FluentTabView::onTabButtonClicked);
    connect(button, &FluentTabButton::closeRequested, this, &FluentTabView::onTabCloseRequested);
    connect(button, &FluentTabButton::pinRequested, this, &FluentTabView::onTabPinRequested);
    
    return button;
}

void FluentTabView::updateTabLayout() {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    
    // Update tab bar styling
    QString tabBarStyle = QString(R"(
        QWidget {
            background-color: %1;
            border: none;
        }
    )").arg(palette.neutralLighter.name());
    
    m_tabBar->setStyleSheet(tabBarStyle);
    
    // Update scroll area styling if used
    if (m_tabScrollArea) {
        QString scrollStyle = QString(R"(
            QScrollArea {
                background-color: %1;
                border: none;
            }
            QScrollBar:horizontal {
                height: 8px;
                background-color: %2;
                border-radius: 4px;
            }
            QScrollBar::handle:horizontal {
                background-color: %3;
                border-radius: 4px;
                min-width: 20px;
            }
        )").arg(
            palette.neutralLighter.name(),
            palette.neutralQuaternary.name(),
            palette.neutralSecondary.name()
        );
        
        m_tabScrollArea->setStyleSheet(scrollStyle);
    }
    
    update();
}

void FluentTabView::updateTabPositions() {
    // This would handle tab reordering animations and positioning
    // Implementation depends on specific drag-and-drop requirements
}

void FluentTabView::animateTabTransition(int from, int to) {
    if (!isAnimated()) return;
    
    // Create smooth transition animation between tabs
    m_transitionAnimation = std::make_unique<QPropertyAnimation>(m_contentStack, "currentIndex");
    m_transitionAnimation->setDuration(200);
    m_transitionAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_transitionAnimation->setStartValue(from);
    m_transitionAnimation->setEndValue(to);
    m_transitionAnimation->start();
}

// Missing methods
void FluentTabView::onTabPinRequested(bool pinned) {
    auto* button = qobject_cast<FluentTabButton*>(sender());
    if (!button) return;

    for (int i = 0; i < static_cast<int>(m_tabButtons.size()); ++i) {
        if (m_tabButtons[i] == button) {
            m_tabData[i].pinned = pinned;
            // Note: No signal emission here as this is a slot, not a signal
            break;
        }
    }
}

void FluentTabView::setScrollable(bool scrollable) {
    if (m_scrollable != scrollable) {
        m_scrollable = scrollable;
        // Recreate tab bar with/without scroll area
        delete m_mainLayout;
        setupTabBar();
        setupContentArea();
    }
}

void FluentTabView::setTabsMovable(bool movable) {
    m_tabsMovable = movable;
    // Enable/disable drag and drop for tab buttons
    for (auto* button : m_tabButtons) {
        button->setProperty("movable", movable);
    }
}

void FluentTabView::setTabsClosable(bool closable) {
    m_tabsClosable = closable;
    // Update all tab data and buttons
    for (size_t i = 0; i < m_tabData.size(); ++i) {
        m_tabData[i].closeable = closable;
        m_tabButtons[i]->setTabData(m_tabData[i]);
    }
}

void FluentTabView::setCloseMode(FluentTabCloseMode mode) {
    m_closeMode = mode;
    // Update close button behavior based on mode
}

void FluentTabView::setShowAddButton(bool show) {
    if (m_showAddButton != show) {
        m_showAddButton = show;
        if (m_addButton) {
            m_addButton->setVisible(show);
        }
    }
}

void FluentTabView::wheelEvent(QWheelEvent* event) {
    if (m_scrollable && m_tabScrollArea) {
        // Forward wheel events to scroll area
        QApplication::sendEvent(m_tabScrollArea, event);
    } else {
        Core::FluentComponent::wheelEvent(event);
    }
}

void FluentTabView::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);
    updateTabLayout();
}

} // namespace FluentQt::Components