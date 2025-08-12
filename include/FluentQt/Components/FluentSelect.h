// include/FluentQt/Components/FluentSelect.h
#pragma once

#include <QAbstractItemModel>
#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QModelIndex>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QSequentialAnimationGroup>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>
#include <memory>
#include "FluentQt/Components/FluentSelectItem.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

// Forward declarations
class FluentSelectDropdown;
class FluentSelectModel;

enum class FluentSelectMode {
    Single,       // Single selection
    Multiple,     // Multiple selection with checkboxes
    MultipleList  // Multiple selection with list display
};

enum class FluentSelectDropDirection {
    Auto,  // Automatically determine direction
    Down,  // Always drop down
    Up     // Always drop up
};

enum class FluentSelectSearchMode {
    None,        // No search functionality
    StartsWith,  // Search items that start with typed text
    Contains,    // Search items that contain typed text
    Custom       // Custom search implementation
};

enum class FluentSelectSize { Small, Medium, Large };

struct FluentSelectConfig {
    FluentSelectMode mode{FluentSelectMode::Single};
    FluentSelectDropDirection dropDirection{FluentSelectDropDirection::Auto};
    FluentSelectSearchMode searchMode{FluentSelectSearchMode::Contains};
    FluentSelectSize size{FluentSelectSize::Medium};

    bool editable{false};
    bool clearable{true};
    bool searchable{true};
    bool groupable{true};
    bool sortable{false};
    bool checkable{false};  // Show checkboxes for items

    int maxVisibleItems{10};
    int maxDropdownHeight{300};
    int minDropdownWidth{0};  // 0 = same as select width
    int maxDropdownWidth{0};  // 0 = no limit

    QString placeholderText;
    QString emptyText{"No items available"};
    QString searchPlaceholder{"Search..."};
    QString noResultsText{"No results found"};

    bool animated{true};
    int animationDuration{200};
    QEasingCurve easingCurve{QEasingCurve::OutCubic};

    // Custom styling
    QColor customBackgroundColor;
    QColor customTextColor;
    QColor customBorderColor;
    QIcon customDropdownIcon;

    bool autoCalculateColors{true};
};

class FluentSelect : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentSelectMode selectMode READ selectMode WRITE setSelectMode
                   NOTIFY modeChanged)
    Q_PROPERTY(FluentSelectSearchMode searchMode READ searchMode WRITE
                   setSearchMode NOTIFY searchModeChanged)
    Q_PROPERTY(FluentSelectSize selectSize READ selectSize WRITE setSelectSize
                   NOTIFY sizeChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE
                   setPlaceholderText NOTIFY placeholderChanged)
    Q_PROPERTY(QString currentText READ currentText NOTIFY currentTextChanged)
    Q_PROPERTY(QVariant currentData READ currentData NOTIFY currentDataChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY
                   currentIndexChanged)
    Q_PROPERTY(
        bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(bool clearable READ isClearable WRITE setClearable NOTIFY
                   clearableChanged)
    Q_PROPERTY(bool searchable READ isSearchable WRITE setSearchable NOTIFY
                   searchableChanged)
    Q_PROPERTY(bool dropdownVisible READ isDropdownVisible WRITE
                   setDropdownVisible NOTIFY dropdownVisibilityChanged)
    Q_PROPERTY(int maxVisibleItems READ maxVisibleItems WRITE setMaxVisibleItems
                   NOTIFY maxVisibleItemsChanged)

public:
    explicit FluentSelect(QWidget* parent = nullptr);
    explicit FluentSelect(FluentSelectMode mode, QWidget* parent = nullptr);
    explicit FluentSelect(const FluentSelectConfig& config,
                          QWidget* parent = nullptr);
    ~FluentSelect() override;

    // Configuration properties
    FluentSelectMode selectMode() const { return m_config.mode; }
    void setSelectMode(FluentSelectMode mode);

    FluentSelectSearchMode searchMode() const { return m_config.searchMode; }
    void setSearchMode(FluentSelectSearchMode mode);

    FluentSelectSize selectSize() const { return m_config.size; }
    void setSelectSize(FluentSelectSize size);

    // Text properties
    QString placeholderText() const { return m_config.placeholderText; }
    void setPlaceholderText(const QString& text);

    QString currentText() const;
    QVariant currentData() const;
    QVariant currentData(int role) const;

    // Selection properties
    int currentIndex() const;
    void setCurrentIndex(int index);

    QModelIndex currentModelIndex() const;
    void setCurrentModelIndex(const QModelIndex& index);

    // Multiple selection support
    QList<int> selectedIndexes() const;
    void setSelectedIndexes(const QList<int>& indexes);

    QModelIndexList selectedModelIndexes() const;
    void setSelectedModelIndexes(const QModelIndexList& indexes);

    QStringList selectedTexts() const;
    QVariantList selectedData() const;

    // Behavior properties
    bool isEditable() const { return m_config.editable; }
    void setEditable(bool editable);

    bool isClearable() const { return m_config.clearable; }
    void setClearable(bool clearable);

    bool isSearchable() const { return m_config.searchable; }
    void setSearchable(bool searchable);

    bool isDropdownVisible() const { return m_dropdownVisible; }
    void setDropdownVisible(bool visible);

    int maxVisibleItems() const { return m_config.maxVisibleItems; }
    void setMaxVisibleItems(int max);

    // Configuration management
    FluentSelectConfig configuration() const { return m_config; }
    void setConfiguration(const FluentSelectConfig& config);

    // Model management
    QAbstractItemModel* model() const;
    void setModel(QAbstractItemModel* model);

    FluentSelectModel* selectModel() const { return m_selectModel; }

    // Item management (convenience methods)
    void addItem(const QString& text, const QVariant& data = QVariant());
    void addItem(const QIcon& icon, const QString& text,
                 const QVariant& data = QVariant());
    void addItem(const FluentSelectItem& item);
    void addItems(const QStringList& texts);
    void addSeparator();
    void addGroup(const QString& title);

    void insertItem(int index, const QString& text,
                    const QVariant& data = QVariant());
    void insertItem(int index, const QIcon& icon, const QString& text,
                    const QVariant& data = QVariant());
    void insertItem(int index, const FluentSelectItem& item);

    void removeItem(int index);
    void clear();

    int count() const;
    bool isEmpty() const;

    FluentSelectItem itemAt(int index) const;
    void setItemAt(int index, const FluentSelectItem& item);

    int findText(const QString& text,
                 Qt::MatchFlags flags = Qt::MatchExactly) const;
    int findData(const QVariant& data, int role = Qt::UserRole,
                 Qt::MatchFlags flags = Qt::MatchExactly) const;

    // Search functionality
    void setSearchFilter(const QString& filter);
    QString searchFilter() const;
    void clearSearchFilter();

    // Custom search implementation
    void setCustomSearchFunction(
        std::function<bool(const FluentSelectItem&, const QString&)>
            searchFunc);

    // Validation
    bool isValid() const;
    QString validationError() const;
    void setValidator(std::function<bool(const QVariant&)> validator);
    void setValidationErrorMessage(const QString& message);

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Static factory methods
    static FluentSelect* createSingleSelect(QWidget* parent = nullptr);
    static FluentSelect* createMultiSelect(QWidget* parent = nullptr);
    static FluentSelect* createSearchableSelect(QWidget* parent = nullptr);
    static FluentSelect* createEditableSelect(QWidget* parent = nullptr);

public slots:
    void showDropdown();
    void hideDropdown();
    void toggleDropdown();
    void clearSelection();
    void selectAll();        // For multiple selection mode
    void invertSelection();  // For multiple selection mode
    void refresh();

signals:
    void modeChanged(FluentSelectMode mode);
    void searchModeChanged(FluentSelectSearchMode mode);
    void sizeChanged(FluentSelectSize size);
    void placeholderChanged(const QString& text);
    void currentTextChanged(const QString& text);
    void currentDataChanged(const QVariant& data);
    void currentIndexChanged(int index);
    void editableChanged(bool editable);
    void clearableChanged(bool clearable);
    void searchableChanged(bool searchable);
    void dropdownVisibilityChanged(bool visible);
    void maxVisibleItemsChanged(int max);

    void activated(int index);
    void activated(const QString& text);
    void highlighted(int index);
    void highlighted(const QString& text);
    void currentIndexChanged(int index, int previousIndex);
    void selectionChanged();
    void textChanged(const QString& text);
    void textEdited(const QString& text);
    void searchFilterChanged(const QString& filter);
    void dropdownAboutToShow();
    void dropdownShown();
    void dropdownAboutToHide();
    void dropdownHidden();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

    virtual void updateStateStyle() override;
    virtual void performStateTransition(Core::FluentState from,
                                        Core::FluentState to) override;

private slots:
    void onDropdownItemActivated(const QModelIndex& index);
    void onDropdownItemClicked(const QModelIndex& index);
    void onDropdownSelectionChanged();
    void onSearchTextChanged(const QString& text);
    void onSearchTextEdited(const QString& text);
    void onClearButtonClicked();
    void onDropdownButtonClicked();
    void onModelDataChanged(const QModelIndex& topLeft,
                            const QModelIndex& bottomRight);
    void onModelRowsInserted(const QModelIndex& parent, int first, int last);
    void onModelRowsRemoved(const QModelIndex& parent, int first, int last);
    void onModelReset();
    void onDropdownAnimationFinished();
    void onThemeChanged();

private:
    void setupUI();
    void setupDropdown();
    void setupAnimations();
    void setupModel();
    void setupConnections();

    void updateLayout();
    void updateColors();
    void updateFonts();
    void updateDropdownGeometry();
    void updateDropdownPosition();
    void updateDisplayText();
    void updatePlaceholder();
    void updateAccessibility();

    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintDropdownButton(QPainter& painter, const QRect& rect);
    void paintClearButton(QPainter& painter, const QRect& rect);
    void paintFocusRing(QPainter& painter, const QRect& rect);

    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getBorderColor() const;
    QColor getPlaceholderColor() const;
    QFont getFont() const;

    QRect getContentRect() const;
    QRect getTextRect() const;
    QRect getDropdownButtonRect() const;
    QRect getClearButtonRect() const;
    QRect getDropdownRect() const;

    void showDropdownAnimated();
    void hideDropdownAnimated();
    void updateDropdownVisibility();

    bool eventFilter(QObject* object, QEvent* event) override;
    void handleKeyNavigation(QKeyEvent* event);
    void handleTextInput(QKeyEvent* event);

    QString formatDisplayText() const;
    QString formatMultipleSelectionText() const;

    void applySearchFilter();
    void resetSearchFilter();

    int getItemHeight() const;
    int getDropdownHeight() const;
    QSize getDropdownSize() const;
    QSize calculateSizeHintInternal() const;

private:
    FluentSelectConfig m_config;

    // UI Components
    QHBoxLayout* m_mainLayout{nullptr};
    QLabel* m_displayLabel{nullptr};
    QLineEdit* m_searchEdit{nullptr};
    QPushButton* m_dropdownButton{nullptr};
    QPushButton* m_clearButton{nullptr};

    // Dropdown
    FluentSelectDropdown* m_dropdown{nullptr};
    QListView* m_listView{nullptr};
    QScrollArea* m_scrollArea{nullptr};

    // Models
    FluentSelectModel* m_selectModel{nullptr};
    QSortFilterProxyModel* m_proxyModel{nullptr};
    QItemSelectionModel* m_selectionModel{nullptr};

    // Animation
    std::unique_ptr<QPropertyAnimation> m_dropdownAnimation;
    std::unique_ptr<QSequentialAnimationGroup> m_showAnimation;
    std::unique_ptr<QSequentialAnimationGroup> m_hideAnimation;

    // State
    bool m_dropdownVisible{false};
    bool m_pressed{false};
    bool m_searchActive{false};
    QString m_searchText;
    QString m_lastValidText;

    // Custom functions
    std::function<bool(const FluentSelectItem&, const QString&)>
        m_customSearchFunction;
    std::function<bool(const QVariant&)> m_validator;
    QString m_validationErrorMessage;

    // Cached values
    QSize m_cachedSizeHint;
    bool m_sizeHintValid{false};
    QRect m_lastRect;
    QString m_cachedDisplayText;

    // Colors (cached from theme)
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_borderColor;
    QColor m_placeholderColor;
    QColor m_focusColor;
};

}  // namespace FluentQt::Components
