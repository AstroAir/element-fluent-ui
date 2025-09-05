// include/FluentQt/Components/FluentDropdown.h
#pragma once

#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <memory>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

// Fluent UI compliant size variants
enum class FluentDropdownSize {
    Small,   // 24px height - compact scenarios
    Medium,  // 32px height - default
    Large    // 40px height - touch-friendly
};

// Fluent UI visual styles
enum class FluentDropdownStyle {
    Outline,    // Default outlined style
    Filled,     // Filled background style
    Underlined  // Underlined style for forms
};

// Enhanced dropdown item with grouping support
struct FluentDropdownItem {
    QString text;
    QString description;  // Secondary text for rich items
    QString tooltip;      // Tooltip text
    QIcon icon;
    QVariant data;
    bool enabled{true};
    bool separator{false};
    bool isGroupHeader{false};  // For item grouping
    QString groupId;            // Group identifier
    int indentLevel{0};         // For hierarchical items

    FluentDropdownItem() = default;
    FluentDropdownItem(const QString& text, const QVariant& data = QVariant())
        : text(text), data(data) {}
    FluentDropdownItem(const QIcon& icon, const QString& text,
                       const QVariant& data = QVariant())
        : text(text), icon(icon), data(data) {}

    // Factory methods for special item types
    static FluentDropdownItem createSeparator();
    static FluentDropdownItem createGroupHeader(const QString& title);
};

class FluentDropdown : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString currentText READ currentText WRITE setCurrentText NOTIFY
                   currentTextChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY
                   currentIndexChanged)
    Q_PROPERTY(QVariant currentData READ currentData NOTIFY currentDataChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE
                   setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(
        bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(bool dropdownVisible READ isDropdownVisible WRITE
                   setDropdownVisible NOTIFY dropdownVisibilityChanged)
    Q_PROPERTY(int maxVisibleItems READ maxVisibleItems WRITE setMaxVisibleItems
                   NOTIFY maxVisibleItemsChanged)

    // Enhanced Fluent UI properties
    Q_PROPERTY(
        FluentDropdownSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(
        FluentDropdownStyle style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(
        bool hasError READ hasError WRITE setHasError NOTIFY errorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage
                   NOTIFY errorMessageChanged)
    Q_PROPERTY(
        bool isLoading READ isLoading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(bool searchEnabled READ isSearchEnabled WRITE setSearchEnabled
                   NOTIFY searchEnabledChanged)
    Q_PROPERTY(QString searchPlaceholder READ searchPlaceholder WRITE
                   setSearchPlaceholder NOTIFY searchPlaceholderChanged)
    Q_PROPERTY(bool groupingEnabled READ isGroupingEnabled WRITE
                   setGroupingEnabled NOTIFY groupingEnabledChanged)

    // Accessibility properties
    Q_PROPERTY(
        QString accessibleName READ accessibleName WRITE setAccessibleName)
    Q_PROPERTY(QString accessibleDescription READ accessibleDescription WRITE
                   setAccessibleDescription)

public:
    explicit FluentDropdown(QWidget* parent = nullptr);
    ~FluentDropdown() override;

    // Content properties
    QString currentText() const;
    void setCurrentText(const QString& text);

    int currentIndex() const;
    void setCurrentIndex(int index);

    QVariant currentData() const;

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    // State properties
    bool isEditable() const;
    void setEditable(bool editable);

    bool isDropdownVisible() const;
    void setDropdownVisible(bool visible);

    int maxVisibleItems() const;
    void setMaxVisibleItems(int count);

    // Enhanced Fluent UI properties
    FluentDropdownSize size() const;
    void setSize(FluentDropdownSize size);

    FluentDropdownStyle style() const;
    void setStyle(FluentDropdownStyle style);

    bool hasError() const;
    void setHasError(bool hasError);

    QString errorMessage() const;
    void setErrorMessage(const QString& message);

    bool isLoading() const;
    void setLoading(bool loading);

    bool isSearchEnabled() const;
    void setSearchEnabled(bool enabled);

    QString searchPlaceholder() const;
    void setSearchPlaceholder(const QString& placeholder);

    bool isGroupingEnabled() const;
    void setGroupingEnabled(bool enabled);

    // Accessibility properties
    QString accessibleName() const;
    void setAccessibleName(const QString& name);

    QString accessibleDescription() const;
    void setAccessibleDescription(const QString& description);

    // Item management
    void addItem(const QString& text, const QVariant& data = QVariant());
    void addItem(const QIcon& icon, const QString& text,
                 const QVariant& data = QVariant());
    void addItem(const FluentDropdownItem& item);
    void addSeparator();

    void insertItem(int index, const QString& text,
                    const QVariant& data = QVariant());
    void insertItem(int index, const QIcon& icon, const QString& text,
                    const QVariant& data = QVariant());
    void insertItem(int index, const FluentDropdownItem& item);
    void insertSeparator(int index);

    void removeItem(int index);
    void clear();

    int count() const;
    FluentDropdownItem itemAt(int index) const;
    void setItemAt(int index, const FluentDropdownItem& item);

    int findText(const QString& text,
                 Qt::MatchFlags flags = Qt::MatchExactly) const;
    int findData(const QVariant& data) const;

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void showDropdown();
    void hideDropdown();
    void toggleDropdown();

signals:
    void currentTextChanged(const QString& text);
    void currentIndexChanged(int index);
    void currentDataChanged(const QVariant& data);
    void placeholderTextChanged(const QString& text);
    void editableChanged(bool editable);
    void dropdownVisibilityChanged(bool visible);
    void maxVisibleItemsChanged(int count);
    void itemSelected(int index);
    void textEdited(const QString& text);

    // Enhanced Fluent UI signals
    void sizeChanged(FluentDropdownSize size);
    void styleChanged(FluentDropdownStyle style);
    void errorChanged(bool hasError);
    void errorMessageChanged(const QString& message);
    void loadingChanged(bool loading);
    void searchEnabledChanged(bool enabled);
    void searchPlaceholderChanged(const QString& placeholder);
    void groupingEnabledChanged(bool enabled);

    // Search and filtering signals
    void searchTextChanged(const QString& text);
    void itemsFiltered(int visibleCount);

    // Accessibility signals
    void accessibilityChanged();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

private slots:
    void onListItemClicked(int index);
    void onTextEdited(const QString& text);
    void onThemeChanged();
    void onDropdownAnimationFinished();

private:
    // Setup methods
    void setupUI();
    void setupDropdown();
    void setupAnimations();
    void updateColors();
    void updateFonts();

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintChevron(QPainter& painter, const QRect& rect);
    void paintFocusRing(QPainter& painter, const QRect& rect);

    // Animation methods
    void startShowDropdownAnimation();
    void startHideDropdownAnimation();

    // Utility methods
    void updateDisplayText();
    void updateDropdownPosition();
    void updateDropdownSize();
    QRect chevronRect() const;
    QRect textRect() const;
    int calculateDropdownHeight() const;

    // Enhanced Fluent UI methods
    void setupAccessibility();
    void updateAccessibility();
    void setupShadowEffect();
    void updateShadowEffect();
    void setupSearchFunctionality();
    void updateComponentSize();
    void updateComponentStyle();
    void updateErrorState();
    void updateLoadingState();
    void filterItems(const QString& searchText);
    void applyFluentDesignTokens();

    // Theme integration
    int getComponentHeight() const;
    QMargins getComponentPadding() const;
    int getBorderRadius() const;
    int getElevation() const;

private:
    // Content
    QList<FluentDropdownItem> m_items;
    int m_currentIndex{-1};
    QString m_placeholderText{"Select item"};

    // Enhanced Fluent UI state
    FluentDropdownSize m_size{FluentDropdownSize::Medium};
    FluentDropdownStyle m_style{FluentDropdownStyle::Outline};
    bool m_hasError{false};
    QString m_errorMessage;
    bool m_isLoading{false};
    bool m_searchEnabled{false};
    QString m_searchPlaceholder{"Search..."};
    bool m_groupingEnabled{false};

    // Accessibility
    QString m_accessibleName;
    QString m_accessibleDescription;

    // Legacy state
    bool m_editable{false};
    bool m_dropdownVisible{false};
    bool m_pressed{false};
    int m_maxVisibleItems{10};

    // UI components
    QHBoxLayout* m_mainLayout{nullptr};
    QLabel* m_displayLabel{nullptr};
    QLineEdit* m_searchEdit{nullptr};  // For search functionality
    QWidget* m_dropdownContainer{nullptr};
    QListWidget* m_listWidget{nullptr};
    QLabel* m_errorLabel{nullptr};    // For error messages
    QLabel* m_loadingLabel{nullptr};  // For loading state

    // Enhanced effects
    std::unique_ptr<QPropertyAnimation> m_dropdownAnimation;
    QGraphicsOpacityEffect* m_dropdownOpacityEffect{nullptr};
    QGraphicsDropShadowEffect* m_shadowEffect{nullptr};  // For elevation

    // Search functionality
    QTimer* m_searchTimer{nullptr};  // For debounced search
    QString m_currentSearchText;

    // Cached values
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

}  // namespace FluentQt::Components
