// include/FluentQt/Components/FluentSelectItem.h
#pragma once

#include <QAbstractItemModel>
#include <QColor>
#include <QFont>
#include <QHash>
#include <QIcon>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QPainter>
#include <QRect>
#include <QSize>
#include <QStandardItemModel>
#include <QString>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QVariant>
#include <memory>

namespace FluentQt::Components {

enum class FluentSelectItemType {
    Item,       // Regular selectable item
    Separator,  // Visual separator
    Group,      // Group header
    Custom      // Custom item type
};

enum class FluentSelectItemState {
    Normal,
    Disabled,
    Selected,
    Highlighted,
    Checked
};

struct FluentSelectItemData {
    QString text;
    QString description;
    QString tooltip;
    QIcon icon;
    QVariant data;
    QVariant userData;

    FluentSelectItemType type{FluentSelectItemType::Item};
    FluentSelectItemState state{FluentSelectItemState::Normal};

    bool enabled{true};
    bool visible{true};
    bool checkable{false};
    bool checked{false};
    bool separator{false};
    bool group{false};

    // Visual properties
    QFont font;
    QColor textColor;
    QColor backgroundColor;
    QIcon statusIcon;
    int indentLevel{0};

    // Custom properties
    QHash<QString, QVariant> properties;

    FluentSelectItemData() = default;
    FluentSelectItemData(const QString& itemText) : text(itemText) {}
    FluentSelectItemData(const QString& itemText, const QVariant& itemData)
        : text(itemText), data(itemData) {}
    FluentSelectItemData(const QIcon& itemIcon, const QString& itemText)
        : text(itemText), icon(itemIcon) {}
    FluentSelectItemData(const QIcon& itemIcon, const QString& itemText,
                         const QVariant& itemData)
        : text(itemText), icon(itemIcon), data(itemData) {}
};

class FluentSelectItem {
public:
    FluentSelectItem();
    explicit FluentSelectItem(const QString& text);
    FluentSelectItem(const QString& text, const QVariant& data);
    FluentSelectItem(const QIcon& icon, const QString& text);
    FluentSelectItem(const QIcon& icon, const QString& text,
                     const QVariant& data);
    FluentSelectItem(const FluentSelectItemData& data);
    FluentSelectItem(const FluentSelectItem& other);
    FluentSelectItem& operator=(const FluentSelectItem& other);
    ~FluentSelectItem() = default;

    // Content properties
    QString text() const { return m_data.text; }
    void setText(const QString& text) { m_data.text = text; }

    QString description() const { return m_data.description; }
    void setDescription(const QString& description) {
        m_data.description = description;
    }

    QString tooltip() const { return m_data.tooltip; }
    void setTooltip(const QString& tooltip) { m_data.tooltip = tooltip; }

    QIcon icon() const { return m_data.icon; }
    void setIcon(const QIcon& icon) { m_data.icon = icon; }

    QVariant data() const { return m_data.data; }
    void setData(const QVariant& data) { m_data.data = data; }

    QVariant userData() const { return m_data.userData; }
    void setUserData(const QVariant& data) { m_data.userData = data; }

    // Type and state
    FluentSelectItemType type() const { return m_data.type; }
    void setType(FluentSelectItemType type) { m_data.type = type; }

    FluentSelectItemState state() const { return m_data.state; }
    void setState(FluentSelectItemState state) { m_data.state = state; }

    // Behavior properties
    bool isEnabled() const { return m_data.enabled; }
    void setEnabled(bool enabled) { m_data.enabled = enabled; }

    bool isVisible() const { return m_data.visible; }
    void setVisible(bool visible) { m_data.visible = visible; }

    bool isCheckable() const { return m_data.checkable; }
    void setCheckable(bool checkable) { m_data.checkable = checkable; }

    bool isChecked() const { return m_data.checked; }
    void setChecked(bool checked) { m_data.checked = checked; }

    bool isSeparator() const {
        return m_data.separator ||
               m_data.type == FluentSelectItemType::Separator;
    }
    void setSeparator(bool separator) {
        m_data.separator = separator;
        if (separator)
            m_data.type = FluentSelectItemType::Separator;
    }

    bool isGroup() const {
        return m_data.group || m_data.type == FluentSelectItemType::Group;
    }
    void setGroup(bool group) {
        m_data.group = group;
        if (group)
            m_data.type = FluentSelectItemType::Group;
    }

    // Visual properties
    QFont font() const { return m_data.font; }
    void setFont(const QFont& font) { m_data.font = font; }

    QColor textColor() const { return m_data.textColor; }
    void setTextColor(const QColor& color) { m_data.textColor = color; }

    QColor backgroundColor() const { return m_data.backgroundColor; }
    void setBackgroundColor(const QColor& color) {
        m_data.backgroundColor = color;
    }

    QIcon statusIcon() const { return m_data.statusIcon; }
    void setStatusIcon(const QIcon& icon) { m_data.statusIcon = icon; }

    int indentLevel() const { return m_data.indentLevel; }
    void setIndentLevel(int level) { m_data.indentLevel = qMax(0, level); }

    // Custom properties
    QVariant property(const QString& name) const {
        return m_data.properties.value(name);
    }
    void setProperty(const QString& name, const QVariant& value) {
        m_data.properties[name] = value;
    }
    bool hasProperty(const QString& name) const {
        return m_data.properties.contains(name);
    }
    void removeProperty(const QString& name) { m_data.properties.remove(name); }
    QHash<QString, QVariant> properties() const { return m_data.properties; }

    // Data access
    FluentSelectItemData itemData() const { return m_data; }
    void setItemData(const FluentSelectItemData& data) { m_data = data; }

    // Utility methods
    bool isSelectable() const {
        return m_data.enabled && !isSeparator() && !isGroup();
    }
    bool matches(const QString& searchText,
                 Qt::MatchFlags flags = Qt::MatchContains) const;
    QString displayText() const;

    // Static factory methods
    static FluentSelectItem createSeparator();
    static FluentSelectItem createGroup(const QString& title);
    static FluentSelectItem createCheckableItem(const QString& text,
                                                bool checked = false);
    static FluentSelectItem createIconItem(const QIcon& icon,
                                           const QString& text,
                                           const QVariant& data = QVariant());
    static FluentSelectItem createDisabledItem(
        const QString& text, const QVariant& data = QVariant());

    // Comparison operators
    bool operator==(const FluentSelectItem& other) const;
    bool operator!=(const FluentSelectItem& other) const {
        return !(*this == other);
    }

private:
    FluentSelectItemData m_data;
};

// Model for FluentSelect
class FluentSelectModel : public QStandardItemModel {
    Q_OBJECT

public:
    explicit FluentSelectModel(QObject* parent = nullptr);
    ~FluentSelectModel() override;

    // Item management
    void addItem(const FluentSelectItem& item);
    void insertItem(int index, const FluentSelectItem& item);
    void removeItem(int index);
    void clear();

    FluentSelectItem itemAt(int index) const;
    void setItemAt(int index, const FluentSelectItem& item);

    int itemCount() const;
    bool isEmpty() const;

    // Search and filtering
    QList<int> findItems(const QString& text,
                         Qt::MatchFlags flags = Qt::MatchContains) const;
    QList<int> findItemsByData(const QVariant& data,
                               Qt::MatchFlags flags = Qt::MatchExactly) const;

    // Group management
    void addGroup(const QString& title);
    void addItemToGroup(const QString& groupTitle,
                        const FluentSelectItem& item);
    QStringList groups() const;
    QList<FluentSelectItem> itemsInGroup(const QString& groupTitle) const;

    // Selection support
    QList<int> selectedIndexes() const;
    void setSelectedIndexes(const QList<int>& indexes);
    void clearSelection();

    // Data access
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Custom roles
    enum CustomRoles {
        ItemDataRole = Qt::UserRole + 1,
        ItemTypeRole,
        ItemStateRole,
        DescriptionRole,
        TooltipRole,
        CheckableRole,
        IndentLevelRole,
        StatusIconRole,
        CustomPropertiesRole
    };

signals:
    void itemAdded(int index);
    void itemRemoved(int index);
    void itemChanged(int index);
    void selectionChanged();

private:
    void setupItem(QStandardItem* standardItem,
                   const FluentSelectItem& selectItem);
    FluentSelectItem getSelectItem(const QStandardItem* standardItem) const;
    void updateItemFromStandard(FluentSelectItem& selectItem,
                                const QStandardItem* standardItem) const;

private:
    QList<FluentSelectItem> m_items;
    QHash<QString, QList<int>> m_groups;
    QList<int> m_selectedIndexes;
};

// Custom delegate for rendering FluentSelectItems
class FluentSelectItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit FluentSelectItemDelegate(QObject* parent = nullptr);
    ~FluentSelectItemDelegate() override;

    // Rendering
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

    // Configuration
    void setItemHeight(int height) { m_itemHeight = height; }
    int itemHeight() const { return m_itemHeight; }

    void setIconSize(const QSize& size) { m_iconSize = size; }
    QSize iconSize() const { return m_iconSize; }

    void setIndentSize(int size) { m_indentSize = size; }
    int indentSize() const { return m_indentSize; }

    void setShowCheckboxes(bool show) { m_showCheckboxes = show; }
    bool showCheckboxes() const { return m_showCheckboxes; }

protected:
    void paintItem(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;
    void paintSeparator(QPainter* painter,
                        const QStyleOptionViewItem& option) const;
    void paintGroup(QPainter* painter, const QStyleOptionViewItem& option,
                    const QModelIndex& index) const;

    void paintBackground(QPainter* painter, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const;
    void paintIcon(QPainter* painter, const QRect& rect,
                   const QIcon& icon) const;
    void paintText(QPainter* painter, const QRect& rect, const QString& text,
                   const QStyleOptionViewItem& option) const;
    void paintCheckbox(QPainter* painter, const QRect& rect, bool checked,
                       const QStyleOptionViewItem& option) const;
    void paintStatusIcon(QPainter* painter, const QRect& rect,
                         const QIcon& icon) const;

    QRect getIconRect(const QStyleOptionViewItem& option,
                      const QModelIndex& index) const;
    QRect getTextRect(const QStyleOptionViewItem& option,
                      const QModelIndex& index) const;
    QRect getCheckboxRect(const QStyleOptionViewItem& option) const;
    QRect getStatusIconRect(const QStyleOptionViewItem& option) const;

    QColor getBackgroundColor(const QStyleOptionViewItem& option,
                              const QModelIndex& index) const;
    QColor getTextColor(const QStyleOptionViewItem& option,
                        const QModelIndex& index) const;
    QFont getFont(const QStyleOptionViewItem& option,
                  const QModelIndex& index) const;

private:
    int m_itemHeight{32};
    QSize m_iconSize{16, 16};
    int m_indentSize{16};
    bool m_showCheckboxes{false};
    int m_separatorHeight{1};
    int m_groupHeight{24};
    int m_padding{8};
    int m_spacing{4};
};

}  // namespace FluentQt::Components

Q_DECLARE_METATYPE(FluentQt::Components::FluentSelectItem)
Q_DECLARE_METATYPE(FluentQt::Components::FluentSelectItemData)
Q_DECLARE_METATYPE(FluentQt::Components::FluentSelectItemType)
Q_DECLARE_METATYPE(FluentQt::Components::FluentSelectItemState)
