// include/FluentQt/Components/FluentBreadcrumb.h
#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVariant>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentBreadcrumbStyle {
    Default,   // Standard breadcrumb with separators
    Compact,   // Compact style with minimal spacing
    Pills,     // Pill-shaped items
    Underline  // Underlined active item
};

enum class FluentBreadcrumbSeparator {
    Chevron,    // > chevron (default)
    Slash,      // / forward slash
    Backslash,  // \ backslash
    Pipe,       // | vertical pipe
    Arrow,      // → arrow
    Dot,        // • bullet
    Custom      // Custom separator text
};

struct FluentBreadcrumbItem {
    QString text;
    QString tooltip;
    QVariant data;        // Custom data associated with item
    QIcon icon;           // Optional icon
    bool enabled{true};   // Whether item is clickable
    bool current{false};  // Whether this is the current item

    FluentBreadcrumbItem() = default;
    FluentBreadcrumbItem(const QString& t) : text(t) {}
    FluentBreadcrumbItem(const QString& t, const QVariant& d)
        : text(t), data(d) {}
    FluentBreadcrumbItem(const QIcon& i, const QString& t) : text(t), icon(i) {}
    FluentBreadcrumbItem(const QIcon& i, const QString& t, const QVariant& d)
        : text(t), data(d), icon(i) {}
};

class FluentBreadcrumb : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentBreadcrumbStyle style READ style WRITE setStyle NOTIFY
                   styleChanged)
    Q_PROPERTY(FluentBreadcrumbSeparator separator READ separator WRITE
                   setSeparator NOTIFY separatorChanged)
    Q_PROPERTY(QString customSeparator READ customSeparator WRITE
                   setCustomSeparator NOTIFY customSeparatorChanged)
    Q_PROPERTY(
        int maxItems READ maxItems WRITE setMaxItems NOTIFY maxItemsChanged)
    Q_PROPERTY(bool showIcons READ showIcons WRITE setShowIcons NOTIFY
                   showIconsChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY
                   currentIndexChanged)

public:
    explicit FluentBreadcrumb(QWidget* parent = nullptr);
    explicit FluentBreadcrumb(const QStringList& items,
                              QWidget* parent = nullptr);
    ~FluentBreadcrumb() override;

    // Style and appearance
    FluentBreadcrumbStyle style() const;
    void setStyle(FluentBreadcrumbStyle style);

    FluentBreadcrumbSeparator separator() const;
    void setSeparator(FluentBreadcrumbSeparator separator);

    QString customSeparator() const;
    void setCustomSeparator(const QString& separator);

    int maxItems() const;
    void setMaxItems(int maxItems);

    bool showIcons() const;
    void setShowIcons(bool show);

    bool isAnimated() const;
    void setAnimated(bool animated);

    // Current item
    int currentIndex() const;
    void setCurrentIndex(int index);

    FluentBreadcrumbItem currentItem() const;
    void setCurrentItem(const FluentBreadcrumbItem& item);

    // Item management
    void addItem(const QString& text);
    void addItem(const QString& text, const QVariant& data);
    void addItem(const QIcon& icon, const QString& text);
    void addItem(const QIcon& icon, const QString& text, const QVariant& data);
    void addItem(const FluentBreadcrumbItem& item);

    void insertItem(int index, const QString& text);
    void insertItem(int index, const QString& text, const QVariant& data);
    void insertItem(int index, const QIcon& icon, const QString& text);
    void insertItem(int index, const QIcon& icon, const QString& text,
                    const QVariant& data);
    void insertItem(int index, const FluentBreadcrumbItem& item);

    void removeItem(int index);
    void removeItem(const QString& text);
    void clear();

    // Item access
    FluentBreadcrumbItem itemAt(int index) const;
    void setItemAt(int index, const FluentBreadcrumbItem& item);

    int itemCount() const;
    QList<FluentBreadcrumbItem> items() const;
    void setItems(const QList<FluentBreadcrumbItem>& items);
    void setItems(const QStringList& items);

    // Item properties
    void setItemEnabled(int index, bool enabled);
    bool isItemEnabled(int index) const;

    void setItemText(int index, const QString& text);
    QString itemText(int index) const;

    void setItemIcon(int index, const QIcon& icon);
    QIcon itemIcon(int index) const;

    void setItemData(int index, const QVariant& data);
    QVariant itemData(int index) const;

    void setItemTooltip(int index, const QString& tooltip);
    QString itemTooltip(int index) const;

    // Navigation
    int findItem(const QString& text) const;
    int findItem(const QVariant& data) const;

    // Path operations
    void setPath(const QString& path, const QString& separator = "/");
    QString path(const QString& separator = "/") const;

    void navigateToPath(const QString& path, const QString& separator = "/");
    void navigateToIndex(int index);
    void navigateToItem(const FluentBreadcrumbItem& item);

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void refresh();

signals:
    void styleChanged(FluentBreadcrumbStyle style);
    void separatorChanged(FluentBreadcrumbSeparator separator);
    void customSeparatorChanged(const QString& separator);
    void maxItemsChanged(int maxItems);
    void showIconsChanged(bool show);
    void animatedChanged(bool animated);
    void currentIndexChanged(int index);
    void itemClicked(int index);
    void itemClicked(const FluentBreadcrumbItem& item);
    void pathChanged(const QString& path);
    void navigationRequested(int index);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onItemClicked();
    void updateColors();
    void onAnimationFinished();

private:
    void setupUI();
    void setupAnimations();
    void rebuildUI();
    void updateItemStyles();
    void updateSeparators();
    void updateOverflow();

    void createItemWidget(int index);
    void removeItemWidget(int index);
    void updateItemWidget(int index);

    QString getSeparatorText() const;
    QWidget* createSeparatorWidget() const;

    void animateItemChange(int index);
    void animatePathChange();

    void applyItemStyle(QPushButton* button, int index, bool isCurrent) const;
    void applySeparatorStyle(QLabel* separator) const;

    QList<int> getVisibleItemIndices() const;
    void createOverflowButton();
    void updateOverflowButton();

private:
    // Configuration
    FluentBreadcrumbStyle m_style{FluentBreadcrumbStyle::Default};
    FluentBreadcrumbSeparator m_separator{FluentBreadcrumbSeparator::Chevron};
    QString m_customSeparator{"/"};
    int m_maxItems{-1};  // -1 for unlimited
    bool m_showIcons{true};
    bool m_animated{true};
    int m_currentIndex{-1};

    // Items
    QList<FluentBreadcrumbItem> m_items;

    // UI components
    QHBoxLayout* m_layout{nullptr};
    QList<QPushButton*> m_itemButtons;
    QList<QLabel*> m_separatorLabels;
    QPushButton* m_overflowButton{nullptr};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_fadeAnimation{nullptr};
    QPropertyAnimation* m_slideAnimation{nullptr};

    // State
    bool m_rebuildScheduled{false};
    QSize m_lastSize;
};

}  // namespace FluentQt::Components
