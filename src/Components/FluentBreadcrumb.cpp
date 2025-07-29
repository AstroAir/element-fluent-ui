// src/Components/FluentBreadcrumb.cpp
#include "FluentQt/Components/FluentBreadcrumb.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QMenu>
#include <QToolTip>
#include <QtMath>

namespace FluentQt::Components {

FluentBreadcrumb::FluentBreadcrumb(QWidget* parent)
    : FluentComponent(parent)
    , m_animator(std::make_unique<Animation::FluentAnimator>(this))
{
    setObjectName("FluentBreadcrumb");
    setupUI();
    setupAnimations();
    updateColors();
}

FluentBreadcrumb::FluentBreadcrumb(const QStringList& items, QWidget* parent)
    : FluentBreadcrumb(parent)
{
    setItems(items);
}

FluentBreadcrumb::~FluentBreadcrumb() = default;

void FluentBreadcrumb::setupUI() {
    FLUENT_PROFILE("FluentBreadcrumb::setupUI");
    
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(4);
    
    // Connect theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentBreadcrumb::updateColors);
}

void FluentBreadcrumb::setupAnimations() {
    FLUENT_PROFILE("FluentBreadcrumb::setupAnimations");
    
    // Fade animation for item changes
    m_fadeAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_fadeAnimation->setDuration(200);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_fadeAnimation, &QPropertyAnimation::finished,
            this, &FluentBreadcrumb::onAnimationFinished);
    
    // Slide animation for path changes
    m_slideAnimation = new QPropertyAnimation(this, "geometry", this);
    m_slideAnimation->setDuration(300);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

FluentBreadcrumbStyle FluentBreadcrumb::style() const {
    return m_style;
}

void FluentBreadcrumb::setStyle(FluentBreadcrumbStyle style) {
    if (m_style != style) {
        m_style = style;
        updateItemStyles();
        emit styleChanged(style);
    }
}

FluentBreadcrumbSeparator FluentBreadcrumb::separator() const {
    return m_separator;
}

void FluentBreadcrumb::setSeparator(FluentBreadcrumbSeparator separator) {
    if (m_separator != separator) {
        m_separator = separator;
        updateSeparators();
        emit separatorChanged(separator);
    }
}

QString FluentBreadcrumb::customSeparator() const {
    return m_customSeparator;
}

void FluentBreadcrumb::setCustomSeparator(const QString& separator) {
    if (m_customSeparator != separator) {
        m_customSeparator = separator;
        if (m_separator == FluentBreadcrumbSeparator::Custom) {
            updateSeparators();
        }
        emit customSeparatorChanged(separator);
    }
}

int FluentBreadcrumb::maxItems() const {
    return m_maxItems;
}

void FluentBreadcrumb::setMaxItems(int maxItems) {
    if (m_maxItems != maxItems) {
        m_maxItems = maxItems;
        updateOverflow();
        emit maxItemsChanged(maxItems);
    }
}

bool FluentBreadcrumb::showIcons() const {
    return m_showIcons;
}

void FluentBreadcrumb::setShowIcons(bool show) {
    if (m_showIcons != show) {
        m_showIcons = show;
        updateItemStyles();
        emit showIconsChanged(show);
    }
}

bool FluentBreadcrumb::isAnimated() const {
    return m_animated;
}

void FluentBreadcrumb::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

int FluentBreadcrumb::currentIndex() const {
    return m_currentIndex;
}

void FluentBreadcrumb::setCurrentIndex(int index) {
    if (index >= -1 && index < m_items.size() && m_currentIndex != index) {
        // Update previous current item
        if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
            m_items[m_currentIndex].current = false;
        }
        
        m_currentIndex = index;
        
        // Update new current item
        if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
            m_items[m_currentIndex].current = true;
        }
        
        updateItemStyles();
        
        if (m_animated) {
            animateItemChange(index);
        }
        
        emit currentIndexChanged(index);
    }
}

FluentBreadcrumbItem FluentBreadcrumb::currentItem() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
        return m_items[m_currentIndex];
    }
    return FluentBreadcrumbItem();
}

void FluentBreadcrumb::setCurrentItem(const FluentBreadcrumbItem& item) {
    const int index = findItem(item.text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

void FluentBreadcrumb::addItem(const QString& text) {
    addItem(FluentBreadcrumbItem(text));
}

void FluentBreadcrumb::addItem(const QString& text, const QVariant& data) {
    addItem(FluentBreadcrumbItem(text, data));
}

void FluentBreadcrumb::addItem(const QIcon& icon, const QString& text) {
    addItem(FluentBreadcrumbItem(icon, text));
}

void FluentBreadcrumb::addItem(const QIcon& icon, const QString& text, const QVariant& data) {
    addItem(FluentBreadcrumbItem(icon, text, data));
}

void FluentBreadcrumb::addItem(const FluentBreadcrumbItem& item) {
    insertItem(m_items.size(), item);
}

void FluentBreadcrumb::insertItem(int index, const QString& text) {
    insertItem(index, FluentBreadcrumbItem(text));
}

void FluentBreadcrumb::insertItem(int index, const QString& text, const QVariant& data) {
    insertItem(index, FluentBreadcrumbItem(text, data));
}

void FluentBreadcrumb::insertItem(int index, const QIcon& icon, const QString& text) {
    insertItem(index, FluentBreadcrumbItem(icon, text));
}

void FluentBreadcrumb::insertItem(int index, const QIcon& icon, const QString& text, const QVariant& data) {
    insertItem(index, FluentBreadcrumbItem(icon, text, data));
}

void FluentBreadcrumb::insertItem(int index, const FluentBreadcrumbItem& item) {
    const int clampedIndex = qBound(0, index, m_items.size());
    m_items.insert(clampedIndex, item);
    
    // Adjust current index if necessary
    if (m_currentIndex >= clampedIndex) {
        m_currentIndex++;
    }
    
    rebuildUI();
}

void FluentBreadcrumb::removeItem(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
        
        // Adjust current index
        if (m_currentIndex == index) {
            m_currentIndex = -1;
        } else if (m_currentIndex > index) {
            m_currentIndex--;
        }
        
        rebuildUI();
    }
}

void FluentBreadcrumb::removeItem(const QString& text) {
    const int index = findItem(text);
    if (index >= 0) {
        removeItem(index);
    }
}

void FluentBreadcrumb::clear() {
    m_items.clear();
    m_currentIndex = -1;
    rebuildUI();
}

FluentBreadcrumbItem FluentBreadcrumb::itemAt(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items[index];
    }
    return FluentBreadcrumbItem();
}

void FluentBreadcrumb::setItemAt(int index, const FluentBreadcrumbItem& item) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index] = item;
        updateItemWidget(index);
    }
}

int FluentBreadcrumb::itemCount() const {
    return m_items.size();
}

QList<FluentBreadcrumbItem> FluentBreadcrumb::items() const {
    return m_items;
}

void FluentBreadcrumb::setItems(const QList<FluentBreadcrumbItem>& items) {
    m_items = items;
    m_currentIndex = -1;
    rebuildUI();
}

void FluentBreadcrumb::setItems(const QStringList& items) {
    m_items.clear();
    for (const QString& text : items) {
        m_items.append(FluentBreadcrumbItem(text));
    }
    m_currentIndex = -1;
    rebuildUI();
}

void FluentBreadcrumb::setItemEnabled(int index, bool enabled) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index].enabled = enabled;
        updateItemWidget(index);
    }
}

bool FluentBreadcrumb::isItemEnabled(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items[index].enabled;
    }
    return false;
}

void FluentBreadcrumb::setItemText(int index, const QString& text) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index].text = text;
        updateItemWidget(index);
    }
}

QString FluentBreadcrumb::itemText(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items[index].text;
    }
    return QString();
}

void FluentBreadcrumb::setItemIcon(int index, const QIcon& icon) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index].icon = icon;
        updateItemWidget(index);
    }
}

QIcon FluentBreadcrumb::itemIcon(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items[index].icon;
    }
    return QIcon();
}

void FluentBreadcrumb::setItemData(int index, const QVariant& data) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index].data = data;
    }
}

QVariant FluentBreadcrumb::itemData(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items[index].data;
    }
    return QVariant();
}

void FluentBreadcrumb::setItemTooltip(int index, const QString& tooltip) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index].tooltip = tooltip;
        updateItemWidget(index);
    }
}

QString FluentBreadcrumb::itemTooltip(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items[index].tooltip;
    }
    return QString();
}

int FluentBreadcrumb::findItem(const QString& text) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].text == text) {
            return i;
        }
    }
    return -1;
}

int FluentBreadcrumb::findItem(const QVariant& data) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].data == data) {
            return i;
        }
    }
    return -1;
}

void FluentBreadcrumb::setPath(const QString& path, const QString& separator) {
    const QStringList parts = path.split(separator, Qt::SkipEmptyParts);
    setItems(parts);
}

QString FluentBreadcrumb::path(const QString& separator) const {
    QStringList parts;
    for (const auto& item : m_items) {
        parts.append(item.text);
    }
    return parts.join(separator);
}

void FluentBreadcrumb::navigateToPath(const QString& path, const QString& separator) {
    setPath(path, separator);
    if (!m_items.isEmpty()) {
        setCurrentIndex(m_items.size() - 1);
    }
    emit pathChanged(path);
}

void FluentBreadcrumb::navigateToIndex(int index) {
    if (index >= 0 && index < m_items.size()) {
        setCurrentIndex(index);
        emit navigationRequested(index);
    }
}

void FluentBreadcrumb::navigateToItem(const FluentBreadcrumbItem& item) {
    const int index = findItem(item.text);
    if (index >= 0) {
        navigateToIndex(index);
    }
}

QSize FluentBreadcrumb::sizeHint() const {
    if (m_items.isEmpty()) {
        return QSize(100, 32);
    }

    const QFontMetrics fm(font());
    int totalWidth = 0;
    int maxHeight = fm.height() + 8; // 8px padding

    for (int i = 0; i < m_items.size(); ++i) {
        const QString& text = m_items[i].text;
        totalWidth += fm.horizontalAdvance(text) + 16; // 16px padding

        if (m_showIcons && !m_items[i].icon.isNull()) {
            totalWidth += 20; // Icon width + spacing
        }

        // Add separator width (except for last item)
        if (i < m_items.size() - 1) {
            totalWidth += fm.horizontalAdvance(getSeparatorText()) + 8;
        }
    }

    return QSize(totalWidth, maxHeight);
}

QSize FluentBreadcrumb::minimumSizeHint() const {
    const QFontMetrics fm(font());
    return QSize(50, fm.height() + 8);
}

void FluentBreadcrumb::refresh() {
    rebuildUI();
}

void FluentBreadcrumb::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);

    if (event->size() != m_lastSize) {
        m_lastSize = event->size();
        updateOverflow();
    }
}

void FluentBreadcrumb::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    // Custom painting is handled by child widgets
}

void FluentBreadcrumb::onItemClicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    const int index = m_itemButtons.indexOf(button);
    if (index >= 0 && index < m_items.size()) {
        setCurrentIndex(index);
        emit itemClicked(index);
        emit itemClicked(m_items[index]);
        emit navigationRequested(index);
    }
}

void FluentBreadcrumb::updateColors() {
    updateItemStyles();
    updateSeparators();
}

void FluentBreadcrumb::onAnimationFinished() {
    // Animation complete
}

void FluentBreadcrumb::rebuildUI() {
    if (m_rebuildScheduled) return;
    m_rebuildScheduled = true;

    // Clear existing widgets
    while (QLayoutItem* item = m_layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    m_itemButtons.clear();
    m_separatorLabels.clear();

    // Create new widgets
    const QList<int> visibleIndices = getVisibleItemIndices();

    for (int i = 0; i < visibleIndices.size(); ++i) {
        const int index = visibleIndices[i];

        // Create item button
        createItemWidget(index);

        // Create separator (except for last item)
        if (i < visibleIndices.size() - 1) {
            QWidget* separator = createSeparatorWidget();
            m_layout->addWidget(separator);
        }
    }

    // Create overflow button if needed
    if (m_maxItems > 0 && m_items.size() > m_maxItems) {
        createOverflowButton();
    }

    m_layout->addStretch();
    updateItemStyles();
    m_rebuildScheduled = false;
}

void FluentBreadcrumb::updateItemStyles() {
    const auto& theme = Styling::FluentTheme::instance();

    for (int i = 0; i < m_itemButtons.size(); ++i) {
        QPushButton* button = m_itemButtons[i];
        const bool isCurrent = (i == m_currentIndex);
        applyItemStyle(button, i, isCurrent);
    }

    for (QLabel* separator : m_separatorLabels) {
        applySeparatorStyle(separator);
    }
}

void FluentBreadcrumb::updateSeparators() {
    const QString separatorText = getSeparatorText();
    for (QLabel* separator : m_separatorLabels) {
        separator->setText(separatorText);
        applySeparatorStyle(separator);
    }
}

void FluentBreadcrumb::updateOverflow() {
    if (m_maxItems > 0 && m_items.size() > m_maxItems) {
        if (!m_overflowButton) {
            createOverflowButton();
        }
        updateOverflowButton();
    } else if (m_overflowButton) {
        m_layout->removeWidget(m_overflowButton);
        m_overflowButton->deleteLater();
        m_overflowButton = nullptr;
    }

    rebuildUI();
}

void FluentBreadcrumb::createItemWidget(int index) {
    if (index < 0 || index >= m_items.size()) return;

    const FluentBreadcrumbItem& item = m_items[index];
    QPushButton* button = new QPushButton(this);
    button->setText(item.text);
    button->setEnabled(item.enabled);
    button->setToolTip(item.tooltip);

    if (m_showIcons && !item.icon.isNull()) {
        button->setIcon(item.icon);
    }

    connect(button, &QPushButton::clicked, this, &FluentBreadcrumb::onItemClicked);

    m_itemButtons.append(button);
    m_layout->addWidget(button);
}

void FluentBreadcrumb::removeItemWidget(int index) {
    if (index >= 0 && index < m_itemButtons.size()) {
        QPushButton* button = m_itemButtons.takeAt(index);
        m_layout->removeWidget(button);
        button->deleteLater();
    }
}

void FluentBreadcrumb::updateItemWidget(int index) {
    if (index >= 0 && index < m_itemButtons.size() && index < m_items.size()) {
        QPushButton* button = m_itemButtons[index];
        const FluentBreadcrumbItem& item = m_items[index];

        button->setText(item.text);
        button->setEnabled(item.enabled);
        button->setToolTip(item.tooltip);

        if (m_showIcons && !item.icon.isNull()) {
            button->setIcon(item.icon);
        } else {
            button->setIcon(QIcon());
        }

        const bool isCurrent = (index == m_currentIndex);
        applyItemStyle(button, index, isCurrent);
    }
}

QString FluentBreadcrumb::getSeparatorText() const {
    switch (m_separator) {
        case FluentBreadcrumbSeparator::Chevron: return ">";
        case FluentBreadcrumbSeparator::Slash: return "/";
        case FluentBreadcrumbSeparator::Backslash: return "\\";
        case FluentBreadcrumbSeparator::Pipe: return "|";
        case FluentBreadcrumbSeparator::Arrow: return "→";
        case FluentBreadcrumbSeparator::Dot: return "•";
        case FluentBreadcrumbSeparator::Custom: return m_customSeparator;
    }
    return ">";
}

QWidget* FluentBreadcrumb::createSeparatorWidget() const {
    QLabel* separator = new QLabel(getSeparatorText(), const_cast<FluentBreadcrumb*>(this));
    separator->setAlignment(Qt::AlignCenter);
    applySeparatorStyle(separator);
    const_cast<FluentBreadcrumb*>(this)->m_separatorLabels.append(separator);
    return separator;
}

void FluentBreadcrumb::animateItemChange(int index) {
    if (!m_animated || !m_fadeAnimation) return;

    m_fadeAnimation->setStartValue(1.0);
    m_fadeAnimation->setEndValue(0.8);
    m_fadeAnimation->start();
}

void FluentBreadcrumb::animatePathChange() {
    if (!m_animated || !m_slideAnimation) return;

    const QRect currentGeometry = geometry();
    m_slideAnimation->setStartValue(currentGeometry);
    m_slideAnimation->setEndValue(currentGeometry.adjusted(0, -5, 0, 5));
    m_slideAnimation->start();
}

void FluentBreadcrumb::applyItemStyle(QPushButton* button, int index, bool isCurrent) const {
    if (!button) return;

    const auto& theme = Styling::FluentTheme::instance();
    QString styleSheet;

    switch (m_style) {
        case FluentBreadcrumbStyle::Default:
            if (isCurrent) {
                styleSheet = QString(
                    "QPushButton { "
                    "color: %1; "
                    "background: transparent; "
                    "border: none; "
                    "padding: 4px 8px; "
                    "font-weight: bold; "
                    "} "
                    "QPushButton:hover { "
                    "background: %2; "
                    "}"
                ).arg(theme.color("textPrimary").name())
                 .arg(theme.color("subtleFillColorSecondary").name());
            } else {
                styleSheet = QString(
                    "QPushButton { "
                    "color: %1; "
                    "background: transparent; "
                    "border: none; "
                    "padding: 4px 8px; "
                    "} "
                    "QPushButton:hover { "
                    "background: %2; "
                    "color: %3; "
                    "}"
                ).arg(theme.color("textSecondary").name())
                 .arg(theme.color("subtleFillColorSecondary").name())
                 .arg(theme.color("textPrimary").name());
            }
            break;

        case FluentBreadcrumbStyle::Compact:
            styleSheet = QString(
                "QPushButton { "
                "color: %1; "
                "background: transparent; "
                "border: none; "
                "padding: 2px 4px; "
                "font-size: 12px; "
                "%2"
                "} "
                "QPushButton:hover { "
                "background: %3; "
                "}"
            ).arg(isCurrent ? theme.color("textPrimary").name() : theme.color("textSecondary").name())
             .arg(isCurrent ? "font-weight: bold;" : "")
             .arg(theme.color("subtleFillColorSecondary").name());
            break;

        case FluentBreadcrumbStyle::Pills:
            if (isCurrent) {
                styleSheet = QString(
                    "QPushButton { "
                    "color: %1; "
                    "background: %2; "
                    "border: 1px solid %3; "
                    "border-radius: 12px; "
                    "padding: 4px 12px; "
                    "font-weight: bold; "
                    "} "
                    "QPushButton:hover { "
                    "background: %4; "
                    "}"
                ).arg(theme.color("textOnAccentPrimary").name())
                 .arg(theme.color("accent").name())
                 .arg(theme.color("accent").name())
                 .arg(theme.color("accentFillColorSecondary").name());
            } else {
                styleSheet = QString(
                    "QPushButton { "
                    "color: %1; "
                    "background: %2; "
                    "border: 1px solid %3; "
                    "border-radius: 12px; "
                    "padding: 4px 12px; "
                    "} "
                    "QPushButton:hover { "
                    "background: %4; "
                    "}"
                ).arg(theme.color("textPrimary").name())
                 .arg(theme.color("cardBackgroundFillColorDefault").name())
                 .arg(theme.color("controlStrokeDefault").name())
                 .arg(theme.color("subtleFillColorSecondary").name());
            }
            break;

        case FluentBreadcrumbStyle::Underline:
            if (isCurrent) {
                styleSheet = QString(
                    "QPushButton { "
                    "color: %1; "
                    "background: transparent; "
                    "border: none; "
                    "border-bottom: 2px solid %2; "
                    "padding: 4px 8px; "
                    "font-weight: bold; "
                    "} "
                    "QPushButton:hover { "
                    "background: %3; "
                    "}"
                ).arg(theme.color("textPrimary").name())
                 .arg(theme.color("accent").name())
                 .arg(theme.color("subtleFillColorSecondary").name());
            } else {
                styleSheet = QString(
                    "QPushButton { "
                    "color: %1; "
                    "background: transparent; "
                    "border: none; "
                    "padding: 4px 8px; "
                    "} "
                    "QPushButton:hover { "
                    "background: %2; "
                    "border-bottom: 2px solid %3; "
                    "}"
                ).arg(theme.color("textSecondary").name())
                 .arg(theme.color("subtleFillColorSecondary").name())
                 .arg(theme.color("controlStrokeDefault").name());
            }
            break;
    }

    button->setStyleSheet(styleSheet);
    Q_UNUSED(index)
}

void FluentBreadcrumb::applySeparatorStyle(QLabel* separator) const {
    if (!separator) return;

    const auto& theme = Styling::FluentTheme::instance();
    const QString styleSheet = QString(
        "QLabel { "
        "color: %1; "
        "background: transparent; "
        "padding: 0px 4px; "
        "}"
    ).arg(theme.color("textTertiary").name());

    separator->setStyleSheet(styleSheet);
}

QList<int> FluentBreadcrumb::getVisibleItemIndices() const {
    QList<int> indices;

    if (m_maxItems <= 0 || m_items.size() <= m_maxItems) {
        // Show all items
        for (int i = 0; i < m_items.size(); ++i) {
            indices.append(i);
        }
    } else {
        // Show first item, ellipsis, and last few items
        indices.append(0);

        if (m_items.size() > m_maxItems) {
            // Add last (maxItems - 1) items
            const int startIndex = m_items.size() - (m_maxItems - 1);
            for (int i = startIndex; i < m_items.size(); ++i) {
                indices.append(i);
            }
        }
    }

    return indices;
}

void FluentBreadcrumb::createOverflowButton() {
    if (m_overflowButton) return;

    m_overflowButton = new QPushButton("...", this);
    m_overflowButton->setToolTip(tr("Show hidden items"));

    connect(m_overflowButton, &QPushButton::clicked, [this]() {
        QMenu menu(this);

        // Add hidden items to menu
        const QList<int> visibleIndices = getVisibleItemIndices();
        for (int i = 0; i < m_items.size(); ++i) {
            if (!visibleIndices.contains(i)) {
                const FluentBreadcrumbItem& item = m_items[i];
                QAction* action = menu.addAction(item.icon, item.text);
                action->setData(i);
                connect(action, &QAction::triggered, [this, i]() {
                    navigateToIndex(i);
                });
            }
        }

        menu.exec(m_overflowButton->mapToGlobal(QPoint(0, m_overflowButton->height())));
    });

    // Insert overflow button after first item
    m_layout->insertWidget(1, m_overflowButton);
    updateOverflowButton();
}

void FluentBreadcrumb::updateOverflowButton() {
    if (!m_overflowButton) return;

    const auto& theme = Styling::FluentTheme::instance();
    const QString styleSheet = QString(
        "QPushButton { "
        "color: %1; "
        "background: transparent; "
        "border: none; "
        "padding: 4px 8px; "
        "} "
        "QPushButton:hover { "
        "background: %2; "
        "}"
    ).arg(theme.color("textSecondary").name())
     .arg(theme.color("subtleFillColorSecondary").name());

    m_overflowButton->setStyleSheet(styleSheet);
}

} // namespace FluentQt::Components
