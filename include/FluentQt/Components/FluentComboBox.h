// include/FluentQt/Components/FluentComboBox.h
#pragma once

#include <QAbstractItemModel>
#include <QIcon>
#include <QLineEdit>
#include <QListView>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentComboBoxStyle {
    Standard,    // Standard dropdown
    Editable,    // Editable with search
    MultiSelect  // Multiple selection support
};

enum class FluentComboBoxDropDirection {
    Auto,  // Automatically determine direction
    Down,  // Always drop down
    Up     // Always drop up
};

struct FluentComboBoxItem {
    QString text;
    QIcon icon;
    QVariant data;
    bool enabled{true};
    bool separator{false};

    FluentComboBoxItem() = default;
    FluentComboBoxItem(const QString& t) : text(t) {}
    FluentComboBoxItem(const QString& t, const QVariant& d)
        : text(t), data(d) {}
    FluentComboBoxItem(const QString& t, const QIcon& i, const QVariant& d)
        : text(t), icon(i), data(d) {}
};

class FluentComboBox : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(
        FluentComboBoxStyle style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE
                   setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(bool searchEnabled READ isSearchEnabled WRITE setSearchEnabled
                   NOTIFY searchEnabledChanged)
    Q_PROPERTY(bool multiSelectEnabled READ isMultiSelectEnabled WRITE
                   setMultiSelectEnabled NOTIFY multiSelectEnabledChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY
                   currentIndexChanged)
    Q_PROPERTY(QString currentText READ currentText WRITE setCurrentText NOTIFY
                   currentTextChanged)
    Q_PROPERTY(QVariant currentData READ currentData NOTIFY currentDataChanged)
    Q_PROPERTY(int maxVisibleItems READ maxVisibleItems WRITE setMaxVisibleItems
                   NOTIFY maxVisibleItemsChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(FluentComboBoxDropDirection dropDirection READ dropDirection
                   WRITE setDropDirection NOTIFY dropDirectionChanged)

public:
    explicit FluentComboBox(QWidget* parent = nullptr);
    explicit FluentComboBox(FluentComboBoxStyle style,
                            QWidget* parent = nullptr);
    ~FluentComboBox() override;

    // Style and behavior
    FluentComboBoxStyle style() const;
    void setStyle(FluentComboBoxStyle style);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    bool isSearchEnabled() const;
    void setSearchEnabled(bool enabled);

    bool isMultiSelectEnabled() const;
    void setMultiSelectEnabled(bool enabled);

    // Current selection
    int currentIndex() const;
    void setCurrentIndex(int index);

    QString currentText() const;
    void setCurrentText(const QString& text);

    QVariant currentData() const;

    QList<int> selectedIndexes() const;
    void setSelectedIndexes(const QList<int>& indexes);

    QStringList selectedTexts() const;
    QList<QVariant> selectedData() const;

    // Items management
    int count() const;
    void addItem(const QString& text);
    void addItem(const QString& text, const QVariant& data);
    void addItem(const QString& text, const QIcon& icon,
                 const QVariant& data = QVariant());
    void addItem(const FluentComboBoxItem& item);
    void addItems(const QStringList& texts);
    void addItems(const QList<FluentComboBoxItem>& items);

    void insertItem(int index, const QString& text);
    void insertItem(int index, const QString& text, const QVariant& data);
    void insertItem(int index, const QString& text, const QIcon& icon,
                    const QVariant& data = QVariant());
    void insertItem(int index, const FluentComboBoxItem& item);

    void removeItem(int index);
    void clear();

    QString itemText(int index) const;
    void setItemText(int index, const QString& text);

    QIcon itemIcon(int index) const;
    void setItemIcon(int index, const QIcon& icon);

    QVariant itemData(int index) const;
    void setItemData(int index, const QVariant& data);

    bool isItemEnabled(int index) const;
    void setItemEnabled(int index, bool enabled);

    void addSeparator();
    void insertSeparator(int index);

    // Model support
    QAbstractItemModel* model() const;
    void setModel(QAbstractItemModel* model);

    // Dropdown configuration
    int maxVisibleItems() const;
    void setMaxVisibleItems(int count);

    FluentComboBoxDropDirection dropDirection() const;
    void setDropDirection(FluentComboBoxDropDirection direction);

    bool isAnimated() const;
    void setAnimated(bool animated);

    // Search functionality
    QString searchText() const;
    void setSearchText(const QString& text);
    void clearSearch();

    bool isCaseSensitiveSearch() const;
    void setCaseSensitiveSearch(bool sensitive);

    // Utility methods
    int findText(const QString& text,
                 Qt::MatchFlags flags = Qt::MatchExactly) const;
    int findData(const QVariant& data) const;

    bool isDropdownVisible() const;
    void showDropdown();
    void hideDropdown();

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void showPopup();
    void hidePopup();

signals:
    void styleChanged(FluentComboBoxStyle style);
    void placeholderTextChanged(const QString& text);
    void searchEnabledChanged(bool enabled);
    void multiSelectEnabledChanged(bool enabled);
    void currentIndexChanged(int index);
    void currentTextChanged(const QString& text);
    void currentDataChanged(const QVariant& data);
    void maxVisibleItemsChanged(int count);
    void animatedChanged(bool animated);
    void dropDirectionChanged(FluentComboBoxDropDirection direction);

    void activated(int index);
    void highlighted(int index);
    void textActivated(const QString& text);
    void textHighlighted(const QString& text);
    void selectionChanged();
    void searchTextChanged(const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onDropdownItemClicked(const QModelIndex& index);
    void onSearchTextChanged(const QString& text);
    void onDropdownAnimationFinished();
    void updateColors();

private:
    void setupUI();
    void setupDropdown();
    void setupAnimations();
    void updateLayout();
    void updateDropdownPosition();
    void updateDropdownSize();

    // Painting methods
    void paintBackground(QPainter* painter);
    void paintBorder(QPainter* painter);
    void paintContent(QPainter* painter);
    void paintDropdownArrow(QPainter* painter);
    void paintFocusIndicator(QPainter* painter);

    // Item management
    void updateModel();
    void updateSelection();
    void updateCurrentItem();
    void filterItems();

    // Dropdown management
    void positionDropdown();
    void animateDropdown(bool show);
    QRect calculateDropdownGeometry() const;
    int calculateDropdownHeight() const;

    // Utility methods
    QString getDisplayText() const;
    QRect getContentRect() const;
    QRect getArrowRect() const;
    bool hitTestArrow(const QPoint& position) const;

private:
    // Configuration
    FluentComboBoxStyle m_style{FluentComboBoxStyle::Standard};
    QString m_placeholderText;
    bool m_searchEnabled{false};
    bool m_multiSelectEnabled{false};
    int m_maxVisibleItems{10};
    bool m_animated{true};
    FluentComboBoxDropDirection m_dropDirection{
        FluentComboBoxDropDirection::Auto};
    bool m_caseSensitiveSearch{false};

    // State
    int m_currentIndex{-1};
    QList<int> m_selectedIndexes;
    bool m_dropdownVisible{false};
    bool m_hovered{false};
    bool m_pressed{false};

    // UI components
    QLineEdit* m_searchEdit{nullptr};
    QWidget* m_dropdown{nullptr};
    QListView* m_listView{nullptr};
    QStandardItemModel* m_model{nullptr};
    QStandardItemModel* m_filteredModel{nullptr};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_dropdownAnimation{nullptr};

    // Search
    QString m_searchText;
    QTimer* m_searchTimer{nullptr};

    // Layout cache
    QRect m_contentRect;
    QRect m_arrowRect;
    bool m_layoutDirty{true};
};

}  // namespace FluentQt::Components
