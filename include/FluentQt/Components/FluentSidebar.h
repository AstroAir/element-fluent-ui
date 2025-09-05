#pragma once

#include <QColor>
#include <QEasingCurve>
#include <QEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QPaintEvent>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSequentialAnimationGroup>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVariantMap>
#include <QWidget>
#include <functional>

namespace FluentQt::Components {

/**
 * @brief Sidebar display modes following Fluent UI patterns
 */
enum class FluentSidebarMode {
    Expanded,  // Full sidebar with text and icons (320px standard)
    Compact,   // Icons only, no text (48px standard)
    Overlay,   // Modal overlay for mobile/small screens
    Hidden,    // Completely hidden
    Auto       // Automatically choose based on screen size
};

/**
 * @brief Badge types for sidebar items
 */
enum class FluentSidebarBadgeType {
    None,     // No badge
    Dot,      // Simple dot indicator
    Count,    // Numeric count
    Alert,    // Alert/warning indicator
    Success,  // Success indicator
    Custom    // Custom badge content
};

/**
 * @brief Item state for proper Fluent UI styling
 */
enum class FluentSidebarItemState {
    Normal,    // Default state
    Hover,     // Mouse hover
    Pressed,   // Mouse pressed
    Selected,  // Currently selected
    Disabled,  // Disabled state
    Loading,   // Loading state
    Error      // Error state
};

/**
 * @brief Enhanced sidebar item configuration with full Fluent UI support
 */
struct FluentSidebarItem {
    // Core properties
    QString id;                       // Unique identifier
    QString text;                     // Primary display text
    QString description;              // Secondary text (subtitle)
    QIcon icon;                       // Item icon
    QString tooltip;                  // Tooltip text
    bool enabled = true;              // Whether item is enabled
    bool separator = false;           // Whether this is a separator
    QWidget* customWidget = nullptr;  // Custom widget for item
    std::function<void()> onClicked;  // Click callback
    QVariantMap data;                 // Custom data

    // Hierarchy support
    QString parentId;         // Parent item ID for nested items
    QStringList childIds;     // Child item IDs
    bool expandable = false;  // Can be expanded/collapsed
    bool expanded = false;    // Current expansion state
    int indentLevel = 0;      // Indentation level

    // Badge and notification support
    FluentSidebarBadgeType badgeType = FluentSidebarBadgeType::None;
    int badgeCount = 0;  // Numeric badge count
    QString badgeText;   // Custom badge text
    QColor badgeColor;   // Custom badge color

    // Keyboard and interaction
    QKeySequence keyboardShortcut;  // Keyboard shortcut
    bool showShortcut = false;      // Show shortcut in UI

    // Visual customization
    QColor customTextColor;        // Custom text color
    QColor customBackgroundColor;  // Custom background color
    QIcon customStateIcon;         // Custom state icon (loading, error, etc.)

    // Accessibility (enhanced)
    QString accessibleName;         // Accessible name
    QString accessibleDescription;  // Accessible description
    QString ariaRole = "menuitem";  // ARIA role (menuitem, button, etc.)
    QString ariaLabel;              // ARIA label
    bool ariaExpanded = false;      // ARIA expanded state
    int ariaLevel = 1;              // ARIA level for hierarchy

    // State management
    FluentSidebarItemState state = FluentSidebarItemState::Normal;
    bool focusable = true;   // Can receive focus
    bool selectable = true;  // Can be selected

    // Default constructor
    FluentSidebarItem() = default;

    // Convenience constructors
    FluentSidebarItem(const QString& id, const QString& text,
                      const QIcon& icon = QIcon())
        : id(id), text(text), icon(icon) {}
};

/**
 * @brief Enhanced sidebar component with comprehensive navigation support
 */
class FluentSidebar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(
        FluentSidebarMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(int expandedWidth READ expandedWidth WRITE setExpandedWidth)
    Q_PROPERTY(int compactWidth READ compactWidth WRITE setCompactWidth)
    Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible)
    Q_PROPERTY(bool autoHide READ autoHide WRITE setAutoHide)

public:
    explicit FluentSidebar(QWidget* parent = nullptr);
    ~FluentSidebar() override;

    // Mode management
    FluentSidebarMode mode() const { return m_mode; }
    void setMode(FluentSidebarMode mode);

    // Size configuration
    int expandedWidth() const { return m_expandedWidth; }
    void setExpandedWidth(int width);

    int compactWidth() const { return m_compactWidth; }
    void setCompactWidth(int width);

    // Behavior configuration
    bool isCollapsible() const { return m_collapsible; }
    void setCollapsible(bool collapsible);

    bool autoHide() const { return m_autoHide; }
    void setAutoHide(bool autoHide);

    // Item management
    void addItem(const FluentSidebarItem& item);
    void insertItem(int index, const FluentSidebarItem& item);
    void removeItem(const QString& id);
    void removeItem(int index);
    void clearItems();

    FluentSidebarItem* findItem(const QString& id);
    int itemCount() const;

    // Enhanced item management
    void updateItem(const QString& id, const FluentSidebarItem& item);
    void setItemEnabled(const QString& id, bool enabled);
    void setItemVisible(const QString& id, bool visible);
    void setItemText(const QString& id, const QString& text);
    void setItemDescription(const QString& id, const QString& description);
    void setItemIcon(const QString& id, const QIcon& icon);
    void setItemTooltip(const QString& id, const QString& tooltip);

    // Badge and notification management
    void setItemBadge(const QString& id, FluentSidebarBadgeType type,
                      int count = 0);
    void setItemBadgeText(const QString& id, const QString& text);
    void setItemBadgeColor(const QString& id, const QColor& color);
    void clearItemBadge(const QString& id);

    // Hierarchy management
    void addItemGroup(const QString& groupId, const QString& title);
    void setItemParent(const QString& itemId, const QString& parentId);
    void expandItem(const QString& id, bool expanded = true);
    void collapseItem(const QString& id);
    bool isItemExpanded(const QString& id) const;

    // Keyboard shortcuts
    void setItemKeyboardShortcut(const QString& id,
                                 const QKeySequence& shortcut);
    void clearItemKeyboardShortcut(const QString& id);

    // Selection
    QString selectedItemId() const { return m_selectedItemId; }
    void setSelectedItem(const QString& id);
    void clearSelection();

    // Multi-selection support
    QStringList selectedItemIds() const { return m_selectedItemIds; }
    void setMultiSelectionEnabled(bool enabled);
    bool isMultiSelectionEnabled() const { return m_multiSelectionEnabled; }
    void addToSelection(const QString& id);
    void removeFromSelection(const QString& id);
    void clearAllSelections();

    // Header and footer
    void setHeader(QWidget* header);
    void setFooter(QWidget* footer);
    QWidget* header() const { return m_header; }
    QWidget* footer() const { return m_footer; }

    // Responsive behavior
    void setResponsiveBreakpoints(int compactBreakpoint, int hiddenBreakpoint);
    void enableResponsiveBehavior(bool enable);

    // Animation configuration
    void setAnimationDuration(int duration);
    void setAnimationEasing(QEasingCurve::Type easing);
    void setAnimationsEnabled(bool enabled);
    bool areAnimationsEnabled() const { return m_animationsEnabled; }

    // Design token integration
    void setUseFluentDesignTokens(bool use);
    bool usesFluentDesignTokens() const { return m_useFluentDesignTokens; }
    void refreshDesignTokens();

    // Accessibility (enhanced)
    void setAccessibleName(const QString& name);
    void setAccessibleDescription(const QString& description);
    void setAccessibleRole(const QString& role);
    void announceToScreenReader(const QString& message);
    void setFocusIndicatorVisible(bool visible);
    bool isFocusIndicatorVisible() const { return m_focusIndicatorVisible; }

    // Keyboard navigation
    void setKeyboardNavigationEnabled(bool enabled);
    bool isKeyboardNavigationEnabled() const {
        return m_keyboardNavigationEnabled;
    }
    void setTypeaheadSearchEnabled(bool enabled);
    bool isTypeaheadSearchEnabled() const { return m_typeaheadSearchEnabled; }

    // Focus management
    void setFocusOnFirstItem();
    void setFocusOnLastItem();
    void setFocusOnItem(const QString& id);
    QString focusedItemId() const { return m_focusedItemId; }

public slots:
    void expand();
    void collapse();
    void toggle();
    void showOverlay();
    void hideOverlay();

signals:
    void modeChanged(FluentSidebarMode mode);
    void itemClicked(const QString& id);
    void itemSelected(const QString& id);
    void itemDeselected(const QString& id);
    void selectionChanged(const QStringList& selectedIds);
    void itemExpanded(const QString& id);
    void itemCollapsed(const QString& id);
    void itemBadgeChanged(const QString& id, FluentSidebarBadgeType type,
                          int count);
    void itemStateChanged(const QString& id, FluentSidebarItemState state);
    void itemFocused(const QString& id);
    void itemHovered(const QString& id);
    void expanded();
    void collapsed();
    void overlayShown();
    void overlayHidden();
    void keyboardNavigationUsed();
    void accessibilityAnnouncement(const QString& message);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void onItemClicked();
    void onAnimationFinished();
    void onResponsiveTimer();
    void updateResponsiveMode();

private:
    void setupUI();
    void setupAnimations();
    void setupAccessibility();
    void updateLayout();
    void updateItemVisibility();
    void updateItemStyles();
    void animateToMode(FluentSidebarMode newMode);
    void createItemWidget(const FluentSidebarItem& item, int index);
    void removeItemWidget(int index);
    void updateItemWidget(int index);

    // Responsive helpers
    void checkResponsiveBreakpoints();
    void handleResponsiveChange();

    // Accessibility helpers
    void updateAccessibilityProperties();
    void announceStateChange(const QString& message);
    void updateItemAccessibility(int index);
    void setupKeyboardNavigation();

    // Design token helpers
    void applyFluentDesignTokens();
    QColor getSemanticColor(const QString& colorName) const;
    QFont getSemanticFont(const QString& fontName) const;
    int getSemanticSpacing(const QString& spacingName) const;

    // Enhanced item management
    void updateItemState(const QString& id, FluentSidebarItemState state);
    void updateItemBadge(const QString& id);
    void updateItemHierarchy();
    void validateItemHierarchy();

    // Focus and selection helpers
    void updateFocusIndicator();
    void handleTypeaheadSearch(const QString& text);
    void clearTypeaheadBuffer();
    int findNextFocusableItem(int currentIndex, int direction) const;
    void updateSelectionState();

    FluentSidebarMode m_mode = FluentSidebarMode::Expanded;
    FluentSidebarMode m_previousMode = FluentSidebarMode::Expanded;

    // Size configuration (using Fluent UI design tokens)
    int m_expandedWidth = 320;  // Updated to Fluent UI standard
    int m_compactWidth = 48;    // Fluent UI compact width
    int m_currentWidth = 320;

    // Behavior flags
    bool m_collapsible = true;
    bool m_autoHide = false;
    bool m_responsiveEnabled = true;
    bool m_animating = false;
    bool m_animationsEnabled = true;
    bool m_useFluentDesignTokens = true;

    // Selection and interaction
    bool m_multiSelectionEnabled = false;
    QStringList m_selectedItemIds;
    QString m_focusedItemId;

    // Accessibility and keyboard navigation
    bool m_focusIndicatorVisible = true;
    bool m_keyboardNavigationEnabled = true;
    bool m_typeaheadSearchEnabled = true;
    QString m_typeaheadBuffer;
    QTimer* m_typeaheadTimer = nullptr;

    // Responsive breakpoints
    int m_compactBreakpoint = 768;
    int m_hiddenBreakpoint = 480;

    // Animation configuration
    int m_animationDuration = 250;
    QEasingCurve::Type m_animationEasing = QEasingCurve::OutCubic;

    // UI components
    QVBoxLayout* m_mainLayout = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_contentWidget = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
    QWidget* m_header = nullptr;
    QWidget* m_footer = nullptr;

    // Items
    QList<FluentSidebarItem> m_items;
    QList<QWidget*> m_itemWidgets;
    QString m_selectedItemId;

    // Animations
    QPropertyAnimation* m_widthAnimation = nullptr;
    QParallelAnimationGroup* m_modeAnimation = nullptr;

    // Responsive behavior
    QTimer* m_responsiveTimer = nullptr;
    QSize m_lastParentSize;

    // Accessibility (enhanced)
    QString m_accessibleName;
    QString m_accessibleDescription;
    QString m_accessibleRole = "navigation";

    // Performance optimization
    mutable QHash<QString, int> m_itemIndexCache;
    bool m_cacheValid = false;

    // Visual state tracking
    QHash<QString, FluentSidebarItemState> m_itemStates;
    QHash<QString, QVariantMap> m_itemBadges;

    // Fluent UI design tokens cache
    mutable QHash<QString, QColor> m_colorTokenCache;
    mutable QHash<QString, QFont> m_fontTokenCache;
    mutable QHash<QString, int> m_spacingTokenCache;
    bool m_tokenCacheValid = false;
};

// Forward declaration for the enhanced item widget
class FluentSidebarItemWidget;

}  // namespace FluentQt::Components

}  // namespace FluentQt::Components
