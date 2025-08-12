// src/Components/FluentSelectItem.cpp
#include "FluentQt/Components/FluentSelectItem.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QPainter>
#include <QStyleOptionViewItem>

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

// FluentSelectItem implementation
FluentSelectItem::FluentSelectItem() = default;

FluentSelectItem::FluentSelectItem(const QString& text) : m_data(text) {}

FluentSelectItem::FluentSelectItem(const QString& text, const QVariant& data)
    : m_data(text, data) {}

FluentSelectItem::FluentSelectItem(const QIcon& icon, const QString& text)
    : m_data(icon, text) {}

FluentSelectItem::FluentSelectItem(const QIcon& icon, const QString& text,
                                   const QVariant& data)
    : m_data(icon, text, data) {}

FluentSelectItem::FluentSelectItem(const FluentSelectItemData& data)
    : m_data(data) {}

FluentSelectItem::FluentSelectItem(const FluentSelectItem& other)
    : m_data(other.m_data) {}

FluentSelectItem& FluentSelectItem::operator=(const FluentSelectItem& other) {
    if (this != &other) {
        m_data = other.m_data;
    }
    return *this;
}

bool FluentSelectItem::matches(const QString& searchText,
                               Qt::MatchFlags flags) const {
    if (searchText.isEmpty()) {
        return true;
    }

    const QString text = m_data.text;
    const QString description = m_data.description;

    if (flags & Qt::MatchExactly) {
        return text == searchText || description == searchText;
    } else if (flags & Qt::MatchStartsWith) {
        return text.startsWith(searchText, Qt::CaseInsensitive) ||
               description.startsWith(searchText, Qt::CaseInsensitive);
    } else if (flags & Qt::MatchContains) {
        return text.contains(searchText, Qt::CaseInsensitive) ||
               description.contains(searchText, Qt::CaseInsensitive);
    }

    return false;
}

QString FluentSelectItem::displayText() const {
    if (!m_data.text.isEmpty()) {
        return m_data.text;
    } else if (!m_data.description.isEmpty()) {
        return m_data.description;
    }
    return QString();
}

// Static factory methods
FluentSelectItem FluentSelectItem::createSeparator() {
    FluentSelectItemData data;
    data.type = FluentSelectItemType::Separator;
    data.separator = true;
    data.enabled = false;
    return FluentSelectItem(data);
}

FluentSelectItem FluentSelectItem::createGroup(const QString& title) {
    FluentSelectItemData data;
    data.text = title;
    data.type = FluentSelectItemType::Group;
    data.group = true;
    data.enabled = false;
    return FluentSelectItem(data);
}

FluentSelectItem FluentSelectItem::createCheckableItem(const QString& text,
                                                       bool checked) {
    FluentSelectItemData data;
    data.text = text;
    data.checkable = true;
    data.checked = checked;
    return FluentSelectItem(data);
}

FluentSelectItem FluentSelectItem::createIconItem(const QIcon& icon,
                                                  const QString& text,
                                                  const QVariant& data) {
    return FluentSelectItem(icon, text, data);
}

FluentSelectItem FluentSelectItem::createDisabledItem(const QString& text,
                                                      const QVariant& data) {
    FluentSelectItemData itemData;
    itemData.text = text;
    itemData.data = data;
    itemData.enabled = false;
    return FluentSelectItem(itemData);
}

bool FluentSelectItem::operator==(const FluentSelectItem& other) const {
    return m_data.text == other.m_data.text &&
           m_data.data == other.m_data.data && m_data.type == other.m_data.type;
}

// FluentSelectModel implementation
FluentSelectModel::FluentSelectModel(QObject* parent)
    : QStandardItemModel(parent) {}

FluentSelectModel::~FluentSelectModel() = default;

void FluentSelectModel::addItem(const FluentSelectItem& item) {
    insertItem(m_items.size(), item);
}

void FluentSelectModel::insertItem(int index, const FluentSelectItem& item) {
    if (index < 0 || index > m_items.size()) {
        return;
    }

    m_items.insert(index, item);

    auto* standardItem = new QStandardItem();
    setupItem(standardItem, item);

    QStandardItemModel::insertRow(index, standardItem);

    emit itemAdded(index);
}

void FluentSelectModel::removeItem(int index) {
    if (index < 0 || index >= m_items.size()) {
        return;
    }

    m_items.removeAt(index);
    QStandardItemModel::removeRow(index);

    emit itemRemoved(index);
}

void FluentSelectModel::clear() {
    m_items.clear();
    m_groups.clear();
    m_selectedIndexes.clear();
    QStandardItemModel::clear();
}

FluentSelectItem FluentSelectModel::itemAt(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items.at(index);
    }
    return FluentSelectItem();
}

void FluentSelectModel::setItemAt(int index, const FluentSelectItem& item) {
    if (index < 0 || index >= m_items.size()) {
        return;
    }

    m_items[index] = item;

    auto* standardItem = QStandardItemModel::item(index);
    if (standardItem) {
        setupItem(standardItem, item);
    }

    emit itemChanged(index);
}

int FluentSelectModel::itemCount() const { return m_items.size(); }

bool FluentSelectModel::isEmpty() const { return m_items.isEmpty(); }

QList<int> FluentSelectModel::findItems(const QString& text,
                                        Qt::MatchFlags flags) const {
    QList<int> found;

    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].matches(text, flags)) {
            found.append(i);
        }
    }

    return found;
}

QList<int> FluentSelectModel::findItemsByData(const QVariant& data,
                                              Qt::MatchFlags flags) const {
    QList<int> found;

    for (int i = 0; i < m_items.size(); ++i) {
        const QVariant itemData = m_items[i].data();

        if (flags & Qt::MatchExactly) {
            if (itemData == data) {
                found.append(i);
            }
        } else if (flags & Qt::MatchContains) {
            if (itemData.toString().contains(data.toString(),
                                             Qt::CaseInsensitive)) {
                found.append(i);
            }
        }
    }

    return found;
}

void FluentSelectModel::addGroup(const QString& title) {
    const FluentSelectItem groupItem = FluentSelectItem::createGroup(title);
    addItem(groupItem);

    if (!m_groups.contains(title)) {
        m_groups[title] = QList<int>();
    }
}

void FluentSelectModel::addItemToGroup(const QString& groupTitle,
                                       const FluentSelectItem& item) {
    addItem(item);

    if (m_groups.contains(groupTitle)) {
        m_groups[groupTitle].append(m_items.size() - 1);
    }
}

QStringList FluentSelectModel::groups() const { return m_groups.keys(); }

QList<FluentSelectItem> FluentSelectModel::itemsInGroup(
    const QString& groupTitle) const {
    QList<FluentSelectItem> items;

    if (m_groups.contains(groupTitle)) {
        const QList<int>& indexes = m_groups[groupTitle];
        for (int index : indexes) {
            if (index >= 0 && index < m_items.size()) {
                items.append(m_items[index]);
            }
        }
    }

    return items;
}

QList<int> FluentSelectModel::selectedIndexes() const {
    return m_selectedIndexes;
}

void FluentSelectModel::setSelectedIndexes(const QList<int>& indexes) {
    m_selectedIndexes = indexes;
    emit selectionChanged();
}

void FluentSelectModel::clearSelection() {
    m_selectedIndexes.clear();
    emit selectionChanged();
}

QVariant FluentSelectModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QStandardItemModel::data(index, role);
    }

    const FluentSelectItem& item = m_items[index.row()];

    switch (role) {
        case Qt::DisplayRole:
            return item.text();
        case Qt::DecorationRole:
            return item.icon();
        case Qt::ToolTipRole:
            return item.tooltip();
        case Qt::UserRole:
            return item.data();
        case ItemDataRole:
            return QVariant::fromValue(item.itemData());
        case ItemTypeRole:
            return static_cast<int>(item.type());
        case ItemStateRole:
            return static_cast<int>(item.state());
        case DescriptionRole:
            return item.description();
        case TooltipRole:
            return item.tooltip();
        case CheckableRole:
            return item.isCheckable();
        case IndentLevelRole:
            return item.indentLevel();
        case StatusIconRole:
            return item.statusIcon();
        case CustomPropertiesRole:
            return QVariant::fromValue(item.properties());
        default:
            return QStandardItemModel::data(index, role);
    }
}

bool FluentSelectModel::setData(const QModelIndex& index, const QVariant& value,
                                int role) {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QStandardItemModel::setData(index, value, role);
    }

    FluentSelectItem& item = m_items[index.row()];

    switch (role) {
        case Qt::DisplayRole:
            item.setText(value.toString());
            break;
        case Qt::DecorationRole:
            item.setIcon(value.value<QIcon>());
            break;
        case Qt::ToolTipRole:
            item.setTooltip(value.toString());
            break;
        case Qt::UserRole:
            item.setData(value);
            break;
        case DescriptionRole:
            item.setDescription(value.toString());
            break;
        case CheckableRole:
            item.setCheckable(value.toBool());
            break;
        case IndentLevelRole:
            item.setIndentLevel(value.toInt());
            break;
        case StatusIconRole:
            item.setStatusIcon(value.value<QIcon>());
            break;
        default:
            return QStandardItemModel::setData(index, value, role);
    }

    // Update the standard item
    auto* standardItem = QStandardItemModel::item(index.row());
    if (standardItem) {
        setupItem(standardItem, item);
    }

    emit dataChanged(index, index, {role});
    emit itemChanged(index.row());

    return true;
}

Qt::ItemFlags FluentSelectModel::flags(const QModelIndex& index) const {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QStandardItemModel::flags(index);
    }

    const FluentSelectItem& item = m_items[index.row()];

    Qt::ItemFlags flags = Qt::ItemIsSelectable;

    if (item.isEnabled() && item.isSelectable()) {
        flags |= Qt::ItemIsEnabled;
    }

    if (item.isCheckable()) {
        flags |= Qt::ItemIsUserCheckable;
    }

    return flags;
}

void FluentSelectModel::setupItem(QStandardItem* standardItem,
                                  const FluentSelectItem& selectItem) {
    if (!standardItem)
        return;

    standardItem->setText(selectItem.text());
    standardItem->setIcon(selectItem.icon());
    standardItem->setToolTip(selectItem.tooltip());
    standardItem->setData(selectItem.data(), Qt::UserRole);
    standardItem->setEnabled(selectItem.isEnabled());
    standardItem->setCheckable(selectItem.isCheckable());

    if (selectItem.isCheckable()) {
        standardItem->setCheckState(selectItem.isChecked() ? Qt::Checked
                                                           : Qt::Unchecked);
    }
}

FluentSelectItem FluentSelectModel::getSelectItem(
    const QStandardItem* standardItem) const {
    if (!standardItem)
        return FluentSelectItem();

    FluentSelectItemData data;
    data.text = standardItem->text();
    data.icon = standardItem->icon();
    data.tooltip = standardItem->toolTip();
    data.data = standardItem->data(Qt::UserRole);
    data.enabled = standardItem->isEnabled();
    data.checkable = standardItem->isCheckable();
    data.checked = standardItem->checkState() == Qt::Checked;

    return FluentSelectItem(data);
}

void FluentSelectModel::updateItemFromStandard(
    FluentSelectItem& selectItem, const QStandardItem* standardItem) const {
    if (!standardItem)
        return;

    selectItem.setText(standardItem->text());
    selectItem.setIcon(standardItem->icon());
    selectItem.setTooltip(standardItem->toolTip());
    selectItem.setData(standardItem->data(Qt::UserRole));
    selectItem.setEnabled(standardItem->isEnabled());
    selectItem.setCheckable(standardItem->isCheckable());
    selectItem.setChecked(standardItem->checkState() == Qt::Checked);
}

// FluentSelectItemDelegate implementation
FluentSelectItemDelegate::FluentSelectItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

FluentSelectItemDelegate::~FluentSelectItemDelegate() = default;

void FluentSelectItemDelegate::paint(QPainter* painter,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
    if (!painter || !index.isValid()) {
        return;
    }

    const FluentSelectItemType type = static_cast<FluentSelectItemType>(
        index.data(FluentSelectModel::ItemTypeRole).toInt());

    switch (type) {
        case FluentSelectItemType::Separator:
            paintSeparator(painter, option);
            break;
        case FluentSelectItemType::Group:
            paintGroup(painter, option, index);
            break;
        case FluentSelectItemType::Item:
        case FluentSelectItemType::Custom:
        default:
            paintItem(painter, option, index);
            break;
    }
}

QSize FluentSelectItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const {
    if (!index.isValid()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    const FluentSelectItemType type = static_cast<FluentSelectItemType>(
        index.data(FluentSelectModel::ItemTypeRole).toInt());

    switch (type) {
        case FluentSelectItemType::Separator:
            return QSize(option.rect.width(), m_separatorHeight);
        case FluentSelectItemType::Group:
            return QSize(option.rect.width(), m_groupHeight);
        case FluentSelectItemType::Item:
        case FluentSelectItemType::Custom:
        default:
            return QSize(option.rect.width(), m_itemHeight);
    }
}

void FluentSelectItemDelegate::paintItem(QPainter* painter,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const {
    painter->save();

    // Paint background
    paintBackground(painter, option, index);

    // Get rects
    const QRect iconRect = getIconRect(option, index);
    const QRect textRect = getTextRect(option, index);
    const QRect checkboxRect = getCheckboxRect(option);
    const QRect statusIconRect = getStatusIconRect(option);

    // Paint checkbox if needed
    if (m_showCheckboxes &&
        index.data(FluentSelectModel::CheckableRole).toBool()) {
        const bool checked =
            index.data(Qt::CheckStateRole).toInt() == Qt::Checked;
        paintCheckbox(painter, checkboxRect, checked, option);
    }

    // Paint icon
    const QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    if (!icon.isNull()) {
        paintIcon(painter, iconRect, icon);
    }

    // Paint text
    const QString text = index.data(Qt::DisplayRole).toString();
    if (!text.isEmpty()) {
        paintText(painter, textRect, text, option);
    }

    // Paint status icon
    const QIcon statusIcon =
        index.data(FluentSelectModel::StatusIconRole).value<QIcon>();
    if (!statusIcon.isNull()) {
        paintStatusIcon(painter, statusIconRect, statusIcon);
    }

    painter->restore();
}

void FluentSelectItemDelegate::paintSeparator(
    QPainter* painter, const QStyleOptionViewItem& option) const {
    painter->save();

    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter->setPen(QPen(palette.neutralTertiary, 1));

    const int y = option.rect.center().y();
    painter->drawLine(option.rect.left() + m_padding, y,
                      option.rect.right() - m_padding, y);

    painter->restore();
}

void FluentSelectItemDelegate::paintGroup(QPainter* painter,
                                          const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const {
    painter->save();

    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Paint background
    painter->fillRect(option.rect, palette.neutralLighter);

    // Paint text
    const QString text = index.data(Qt::DisplayRole).toString();
    const QFont font = getFont(option, index);

    painter->setFont(font);
    painter->setPen(palette.neutralPrimary);

    const QRect textRect = option.rect.adjusted(m_padding, 0, -m_padding, 0);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

    painter->restore();
}

void FluentSelectItemDelegate::paintBackground(
    QPainter* painter, const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
    const QColor backgroundColor = getBackgroundColor(option, index);

    if (backgroundColor.isValid()) {
        painter->fillRect(option.rect, backgroundColor);
    }
}

void FluentSelectItemDelegate::paintIcon(QPainter* painter, const QRect& rect,
                                         const QIcon& icon) const {
    if (!icon.isNull() && rect.isValid()) {
        icon.paint(painter, rect);
    }
}

void FluentSelectItemDelegate::paintText(
    QPainter* painter, const QRect& rect, const QString& text,
    const QStyleOptionViewItem& option) const {
    if (text.isEmpty() || !rect.isValid()) {
        return;
    }

    const QFont font = getFont(option, QModelIndex());
    const QColor textColor = getTextColor(option, QModelIndex());

    painter->setFont(font);
    painter->setPen(textColor);

    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
}

void FluentSelectItemDelegate::paintCheckbox(
    QPainter* painter, const QRect& rect, bool checked,
    const QStyleOptionViewItem& option) const {
    Q_UNUSED(option)

    if (!rect.isValid()) {
        return;
    }

    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Draw checkbox background
    painter->setBrush(checked ? palette.accent : palette.neutralLightest);
    painter->setPen(QPen(palette.neutralTertiary, 1));
    painter->drawRect(rect);

    // Draw checkmark if checked
    if (checked) {
        painter->setPen(QPen(palette.neutralLightest, 2));
        const int margin = 2;
        const QRect checkRect = rect.adjusted(margin, margin, -margin, -margin);

        // Draw a simple checkmark
        painter->drawLine(checkRect.left(), checkRect.center().y(),
                          checkRect.center().x(), checkRect.bottom());
        painter->drawLine(checkRect.center().x(), checkRect.bottom(),
                          checkRect.right(), checkRect.top());
    }
}

void FluentSelectItemDelegate::paintStatusIcon(QPainter* painter,
                                               const QRect& rect,
                                               const QIcon& icon) const {
    paintIcon(painter, rect, icon);
}

QRect FluentSelectItemDelegate::getIconRect(const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const {
    Q_UNUSED(index)

    const int x = option.rect.left() + m_padding;
    const int y = option.rect.center().y() - m_iconSize.height() / 2;

    return QRect(QPoint(x, y), m_iconSize);
}

QRect FluentSelectItemDelegate::getTextRect(const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const {
    Q_UNUSED(index)

    int left = option.rect.left() + m_padding;

    // Adjust for checkbox
    if (m_showCheckboxes) {
        left += 20 + m_spacing;
    }

    // Adjust for icon
    const QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    if (!icon.isNull()) {
        left += m_iconSize.width() + m_spacing;
    }

    const int right = option.rect.right() - m_padding - m_iconSize.width() -
                      m_spacing;  // Reserve space for status icon

    return QRect(left, option.rect.top(), right - left, option.rect.height());
}

QRect FluentSelectItemDelegate::getCheckboxRect(
    const QStyleOptionViewItem& option) const {
    if (!m_showCheckboxes) {
        return QRect();
    }

    const int size = 16;
    const int x = option.rect.left() + m_padding;
    const int y = option.rect.center().y() - size / 2;

    return QRect(x, y, size, size);
}

QRect FluentSelectItemDelegate::getStatusIconRect(
    const QStyleOptionViewItem& option) const {
    const int x = option.rect.right() - m_padding - m_iconSize.width();
    const int y = option.rect.center().y() - m_iconSize.height() / 2;

    return QRect(QPoint(x, y), m_iconSize);
}

QColor FluentSelectItemDelegate::getBackgroundColor(
    const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(index)

    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (option.state & QStyle::State_Selected) {
        return palette.accent;
    } else if (option.state & QStyle::State_MouseOver) {
        return palette.neutralLighter;
    }

    return QColor();  // Transparent
}

QColor FluentSelectItemDelegate::getTextColor(
    const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(index)

    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (option.state & QStyle::State_Selected) {
        return palette.neutralLightest;
    } else if (!(option.state & QStyle::State_Enabled)) {
        return palette.neutralTertiary;
    }

    return palette.neutralPrimary;
}

QFont FluentSelectItemDelegate::getFont(const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const {
    Q_UNUSED(index)

    const auto& theme = FluentTheme::instance();

    QFont font = option.font;
    if (font.family().isEmpty()) {
        font = theme.bodyFont();
    }

    return font;
}
